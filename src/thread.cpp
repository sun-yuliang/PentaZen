/*      _____                __    ______
 *     / ___ \              / /   /___  /
 *    / /__/ /___  ____  __/ /_______/ /    ____  ____
 *   / _____/ __ \/ __ \/_   _/ __  / /    / __ \/ __ \
 *  / /    /  ___/ / / / / /_/ /_/ / /____/  ___/ / / /
 * /_/     \____/_/ /_/ /___/\__,_/______/\____/_/ /_/
 *
 * PentaZen, a Gomoku/Renju playing engine developed by Sun Yuliang.
 */

#include "thread.h"

#include "tt.h"

ThreadPool Threads;

// Thread constructor launches the thread and waits until it goes to sleep
// in idle_loop(). Note that 'searching' and 'exit' should be already set.
Thread::Thread(size_t n)
    : idx(n), stdThread(&Thread::idle_loop, this) {
    wait_for_search_finished();
}

// Thread destructor wakes up the thread in idle_loop() and waits
// for its termination. Thread should be already waiting.
Thread::~Thread() {
    assert(!searching);

    exit = true;
    start_searching();
    stdThread.join();
}

// Thread::clear_history() resets pv and histories
void Thread::clear_history() {
    for (auto &i : ss) {
        i.pv         = nullptr;
        i.killers[0] = i.killers[1] = MOVE_NONE;
    }
    counterMoves.fill(MOVE_NONE);
}

// Thread::update_history() updates histories with the move
void Thread::update_history(Move m) {
    assert(is_ok(m));

    ss[ply].update_killers(m);

    if (bd.pieceCnt >= 1)
        counterMoves[bd.last_move(1)] = m;
}

// Thread::start_searching() wakes up the thread that will start the search
void Thread::start_searching() {
    std::lock_guard<std::mutex> lk(mutex);
    searching = true;
    cv.notify_one(); // Wake up the thread in idle_loop()
}

// Thread::wait_for_search_finished() blocks on the condition variable
// until the thread has finished searching.
void Thread::wait_for_search_finished() {
    std::unique_lock<std::mutex> lk(mutex);
    cv.wait(lk, [&] { return !searching; });
}

// Thread::idle_loop() is where the thread is parked, blocked on the
// condition variable, when it has no work to do.
void Thread::idle_loop() {
    // If OS already scheduled us on a different group than 0 then don't overwrite
    // the choice, eventually we are one of many one-threaded processes running on
    // some Windows NUMA hardware, for instance in fishtest. To make it simple,
    // just check if running threads are below a threshold, in this case all this
    // NUMA machinery is not needed.
    if (Threads.threadNum > 8)
        bindThisThread(idx);

    while (true) {
        std::unique_lock<std::mutex> lk(mutex);
        searching = false;
        cv.notify_one(); // Wake up anyone waiting for search finished
        cv.wait(lk, [&] { return searching; });

        if (exit)
            return;

        lk.unlock();

        search();
    }
}

// ThreadPool::set() creates/destroys threads to match the requested number.
// Created and launched threads will immediately go to sleep in idle_loop.
// Upon resizing, threads are recreated to allow for binding if necessary.
void ThreadPool::set(size_t requested) {
    threadNum = requested;

    if (size() > 0) { // destroy any existing thread(s)
        main()->wait_for_search_finished();

        while (size() > 0)
            delete back(), pop_back();
    }

    if (requested > 0) { // create new thread(s)
        push_back(new MainThread(0));

        while (size() < requested)
            push_back(new Thread(size()));
        reset();

        // Sync with main thread
        Thread *mainThread = front();
        for (Thread *th : *this)
            if (th != mainThread)
                th->bd = mainThread->bd;
    }
}

// ThreadPool::reset() resets board and histories for each thread
void ThreadPool::reset() {
    for (Thread *th : *this) {
        th->bd.reset();
        th->clear_history();
    }
}

// ThreadPool::clear_history() clears histories for each thread,
// usually called with TT clear
void ThreadPool::clear_history() {
    for (Thread *th : *this)
        th->clear_history();
}

// ThreadPool::think_and_move() wakes up main thread waiting in idle_loop()
// and returns immediately. Main thread will wake up other threads and start
// the search. After the search finishes, move message will be flushed.
void ThreadPool::think_and_move() {
    // Reset timer as early as possible
    timer.reset();
    update_turn_time();

    // Reset search for each thread as early as possible
    for (Thread *th : *this)
        th->reset_search();

    set_best_thread_with_rem(main(), SCORE_ZERO, DEPTH_ZERO);
    terminate = false;

    main()->start_searching();
}

void ThreadPool::set_rule(Rule r) {
    // TT and histories are invalid after rule changes
    if (rule != r) {
        TT.clear();
        clear_history();
    }

    if (r < 2)
        rule = r;
    else if (r == 2 || r == 4)
        rule = RENJU;
}

bool ThreadPool::is_empty(Move m) const {
    return board()->is_empty(m);
}

bool ThreadPool::is_foul(Move m) const {
    return board()->is_foul(m);
}

void ThreadPool::do_move(Move m) {
    for (Thread *th : *this)
        th->bd.do_move(m);
}

void ThreadPool::undo_move() {
    for (Thread *th : *this)
        th->bd.undo_move();
}

Depth ThreadPool::get_ply_max() {
    Depth dep = DEPTH_ZERO;
    for (Thread *th : *this)
        dep = std::max(dep, th->plyMax);
    return dep;
}

uint64_t ThreadPool::get_node_cnt() {
    uint64_t cnt = 0;
    for (Thread *th : *this)
        cnt += th->nodeCnt;
    return cnt;
}

Score ThreadPool::get_rem_score() {
    std::lock_guard<std::mutex> lk(mutex);
    return rem.score;
}

Depth ThreadPool::get_rem_depth() {
    std::lock_guard<std::mutex> lk(mutex);
    return rem.depth;
}

Thread *ThreadPool::get_best_thread() {
    std::lock_guard<std::mutex> lk(mutex);
    return bestThread;
}

void ThreadPool::set_best_thread_with_rem(Thread *th, Score s, Depth d) {
    std::lock_guard<std::mutex> lk(mutex);
    bestThread = th;
    rem.score  = s;
    rem.depth  = d;
}

void ThreadPool::update_turn_time() {
    // turnTimeMin = std::min(timeoutMatch / 120, timeoutTurn);
    // turnTime    = std::min(timeoutMatch / 120, timeoutTurn);
    // turnTimeMax = std::min(timeoutMatch / 120, timeoutTurn);

    turnTimeMin = std::max(std::min((timeLeft - 10000) / 18, timeoutTurn) - 50, TimePoint(50));
    turnTime    = std::max(std::min((timeLeft - 10000) / 6, timeoutTurn) - 50, TimePoint(50));
    turnTimeMax = std::max(std::min((timeLeft - 10000) / 6, timeoutTurn) - 50, TimePoint(50));
}
