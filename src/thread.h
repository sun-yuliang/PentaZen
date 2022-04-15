/*      _____                __    ______
 *     / ___ \              / /   /___  /
 *    / /__/ /___  ____  __/ /_______/ /    ____  ____
 *   / _____/ __ \/ __ \/_   _/ __  / /    / __ \/ __ \
 *  / /    /  ___/ / / / / /_/ /_/ / /____/  ___/ / / /
 * /_/     \____/_/ /_/ /___/\__,_/______/\____/_/ /_/
 *
 * PentaZen, a Gomoku/Renju playing engine developed by Sun Yuliang.
 */

#pragma once

#include "board.h"
#include "search.h"

#include <condition_variable>
#include <mutex>
#include <thread>

// Thread class keeps together thread and search related stuff
class Thread {
public:
    explicit Thread(size_t n);
    virtual ~Thread();

    void idle_loop();
    void start_searching();
    void wait_for_search_finished();

    void clear_history();
    void update_history(Move m);
    void reset_alphabeta();
    void reset_search();
    void print_message() const;

    virtual void search();
    template <NodeType NT>
    Score alphabeta(Score alpha, Score beta, Depth depth, bool cautious);
    template <NodeType NT>
    Score vcf(Depth depth, bool rootNode);

    // Single thread level data members
    Board                    bd;
    Depth                    ply, plyMax, itDepth;
    uint64_t                 nodeCnt;
    Pv                       rootPv;
    SearchStack              ss;
    CounterMoveHistory       counterMoves;
    std::vector<RootExtMove> rootBests;

private:
    // Thread related stuff
    std::mutex              mutex;
    std::condition_variable cv;
    size_t                  idx;
    bool                    exit = false, searching = true; // Set before starting std::thread
    std::thread             stdThread;
};

// MainThread is a derived struct specific for main thread
struct MainThread : public Thread {
    using Thread::Thread;
    void search() override;
};

// ThreadPool struct handles all the threads-related stuff like init, starting,
// parking and, most importantly, launching a thread. All the access to threads
// is done through this class.
struct ThreadPool : public std::vector<Thread *> {
    ~ThreadPool() {
        set(0);
    }

    MainThread *main() const {
        return static_cast<MainThread *>(front());
    }
    Board *board() const {
        return &(main()->bd);
    }

    // Methods for protocol level call
    void set(size_t n);
    void reset();
    void clear_history();
    void think_and_move();

    void set_rule(Rule r);
    bool is_empty(Move m) const;
    bool is_foul(Move m) const;
    void do_move(Move m);
    void undo_move();

    Depth    get_ply_max();
    uint64_t get_node_cnt();
    Score    get_rem_score();
    Depth    get_rem_depth();
    Thread * get_best_thread();
    void     set_best_thread_with_rem(Thread *th, Score s, Depth d);

    void update_turn_time();

    // Data members shared between all threads
    Rule   rule       = FREESTYLE;
    bool   yxprotocol = false;
    bool   terminate  = false;
    size_t threadNum  = 1;

    TimePoint timeoutTurn  = 2147483647;
    TimePoint timeoutMatch = 2147483647;
    TimePoint timeLeft     = 2147483647;

    TimePoint turnTime;
    TimePoint turnTimeMax;
    TimePoint turnTimeMin;

    TimeManagement timer;

private:
    std::mutex  mutex;
    RootExtMove rem;
    Thread *    bestThread;
};

extern ThreadPool Threads;
