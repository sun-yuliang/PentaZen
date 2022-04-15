/*      _____                __    ______
 *     / ___ \              / /   /___  /
 *    / /__/ /___  ____  __/ /_______/ /    ____  ____
 *   / _____/ __ \/ __ \/_   _/ __  / /    / __ \/ __ \
 *  / /    /  ___/ / / / / /_/ /_/ / /____/  ___/ / / /
 * /_/     \____/_/ /_/ /___/\__,_/______/\____/_/ /_/
 *
 * PentaZen, a Gomoku/Renju playing engine developed by Sun Yuliang.
 */

#include "search.h"

#include "movegen.h"
#include "thread.h"
#include "tt.h"

#include <cmath>

namespace {

// Sizes and phases of the skip-blocks, used for distributing search depths across the threads
constexpr int SkipSize[20]  = {1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4};
constexpr int SkipPhase[20] = {0, 1, 0, 1, 2, 3, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 7};

int   FutilityMoveCount[2][DEPTH_NUM];    // [quiet][depth]
Depth Reduction[2][DEPTH_NUM][MOVE_SIZE]; // [pv][depth][moveCnt]

constexpr Score futility_margin(Depth d) {
    return Score(45 * int(d));
}

constexpr bool is_empty(const Pv &pv) {
    return pv[0] == MOVE_NONE;
}

inline void reset_pv(Pv &pv) {
    pv[0] = MOVE_NONE;
}

inline Score score_to_tt(Score sc, Depth ply) {
    return sc > SCORE_WIN_THRESHOLD ? sc + ply : sc < -SCORE_WIN_THRESHOLD ? sc - ply :
                                                                             sc;
}

inline Score score_from_tt(Score sc, Depth ply) {
    return sc > SCORE_WIN_THRESHOLD ? sc - ply : sc < -SCORE_WIN_THRESHOLD ? sc + ply :
                                                                             sc;
}

// Update pv by adding current move only
void update_pv(Pv *pv, Move m) {
    (*pv)[0] = m;
    (*pv)[1] = MOVE_NONE;
}

// Update pv by adding current move and appending child pv
void update_pv(Pv *pv, Move m, const Pv *childPv) {
    auto i   = 0;
    (*pv)[0] = m;

    while ((*childPv)[i++])
        (*pv)[i] = (*childPv)[i - 1];

    (*pv)[i] = MOVE_NONE;
}

// Update pv by adding two moves and appending child pv
void update_pv(Pv *pv, Move m0, Move m1, const Pv *childPv) {
    auto i   = 0;
    (*pv)[0] = m0;
    (*pv)[1] = m1;

    while ((*childPv)[i++])
        (*pv)[i + 1] = (*childPv)[i - 1];

    (*pv)[i + 1] = MOVE_NONE;
}

} // namespace

// Initialize lookup tables. Call at startup.
void search_init() {
    for (auto i = 0; i != DEPTH_NUM; ++i) {
        FutilityMoveCount[0][i] = std::min(int(round(5 * sqrt(i) - 2)), 8);
        FutilityMoveCount[1][i] = std::min(int(round(15 * sqrt(i) - 12)), 32);
    }

    for (auto d = 1; d != DEPTH_NUM; ++d)
        for (auto mc = 1; mc != MOVE_SIZE; ++mc) {
            double r = log(d) * log(mc) / 2.0;

            Reduction[NonPV][d][mc] = Depth(int(round(r)));
            Reduction[PV][d][mc]    = std::max(Reduction[NonPV][d][mc] - 1, DEPTH_ZERO);
        }
}

// Thread::reset_alphabeta() should be called before each alphabeta iteration
void Thread::reset_alphabeta() {
    ply = DEPTH_ZERO;
    reset_pv(rootPv);
    ss[0].pv = &rootPv;
}

// Thread::reset_search() should be called before the whole search
void Thread::reset_search() {
    plyMax  = DEPTH_ZERO;
    itDepth = DEPTH_ITERATIVE_MIN;
    nodeCnt = 0;
    rootBests.clear();
    reset_alphabeta();
}

// Thread::print_message() outputs iterative deepening info
void Thread::print_message() const {
    if (!OUTPUT_MESSAGE)
        return;

    assert(!rootBests.empty());

    const RootExtMove &rem = rootBests.back();

    assert(is_ok(rem.pv[0]));
    assert(is_ok(rem.score));
    assert(rem.depth > DEPTH_ZERO);

    Depth    plyMax  = Threads.get_ply_max();
    uint64_t nodeCnt = Threads.get_node_cnt() + 1; // add one to ensure positive

    sync_cout << "MESSAGE"
              << " dep " << rem.depth << "-" << std::max(rem.depth, plyMax);

    rem.score > SCORE_WIN_THRESHOLD ? std::cout << " ev "
                                                << "+v" << SCORE_WIN - rem.score :
    rem.score < -SCORE_WIN_THRESHOLD ? std::cout << " ev "
                                                 << "-v" << SCORE_WIN + rem.score :
                                       std::cout << " ev " << rem.score;

    nodeCnt < 10000 ? std::cout << " nd " << nodeCnt : nodeCnt < 10000000   ? std::cout << " nd " << nodeCnt / 1000 << "k" :
                                                   nodeCnt < 10000000000    ? std::cout << " nd " << nodeCnt / 1000000 << "m" :
                                                   nodeCnt < 10000000000000 ? std::cout << " nd " << nodeCnt / 1000000000 << "g" :
                                                                              std::cout << " nd " << nodeCnt / 1000000000000 << "t";

    std::cout << " tm " << Threads.timer.elapsed() + 1;
    std::cout << " sp " << nodeCnt / (Threads.timer.elapsed() + 1); // add one to avoid divided by 0

    if (Threads.yxprotocol && !is_empty(rem.pv)) {
        std::cout << " pv";
        for (auto i = 0; rem.pv[i]; ++i) {
            assert(is_ok(rem.pv[i]));
            std::cout << " " << rem.pv[i];
        }
    }

    std::cout << sync_endl;
}

// MainThread::search() is called by the main thread to search from the root
// position and output the result to GUI
void MainThread::search() {
    assert(bd.check_wld_already() == PIECE_NONE);

    MoveGen     mg(&bd);
    RootExtMove rem;
    ExtMove     em;
    int         offset;
    bool        skipSearch = false;

    // New round of search
    TT.new_search();

    // Check if first move
    if (!skipSearch && bd.is_empty()) {
        rem.score = SCORE_ZERO;
        rem.depth = Depth(1);
        update_pv(&rem.pv, make_move(BOARD_SIDE / 2, BOARD_SIDE / 2));
        skipSearch = true;
    }

    // Check for win/lose/draw
    if (!skipSearch && bd.check_wld(offset) != PIECE_NONE) {
        em        = mg.generate<WLD>();
        rem.score = em.score;
        rem.depth = Depth(offset);
        update_pv(&rem.pv, em.move);
        skipSearch = true;
    }

    // Check for unique move
    if (!skipSearch) {
        em = mg.generate<MAIN>();
        if (mg.size() == 1) {
            rem.score = SCORE_ZERO;
            rem.depth = Depth(1);
            update_pv(&rem.pv, em.move);
            skipSearch = true;
        }
    }

    if (skipSearch) {
        // Check if could skip searching
        rootBests.emplace_back(rem);
        print_message();
    } else {
        // Start non-main threads
        for (Thread *th : Threads)
            if (th != this)
                th->start_searching();

        // Start searching
        Thread::search();
    }

    // Wait until all other threads stop searching
    for (auto &th : Threads)
        if (th != this)
            th->wait_for_search_finished();

    Move bestMove = Threads.get_best_thread()->rootBests.back().pv[0];

    // Update the boards in all threads
    Threads.do_move(bestMove);

    // Output the result
    sync_cout << rank_of(bestMove) << "," << file_of(bestMove) << sync_endl;
}

// Thread::search() is the main iterative deepening loop. It calls alphabeta()
// repeatedly with increasing depth until the allocated thinking time has been
// consumed, the user stops the search, or the maximum search depth is reached.
void Thread::search() {
    RootExtMove rem;
    Score       score;
    double      timeIncrease;
    bool        validResult = true, breakSearch = false, bestMoveChanges = false;

    while (true) {
        // Distribute search depths across the helper threads
        if (this != Threads.main()) {
            int i = (idx - 1) % 20;
            if (((itDepth + SkipPhase[i]) / SkipSize[i]) % 2) {
                ++itDepth;
                continue;
            }
        }

        reset_alphabeta();
        score = alphabeta<PV>(-SCORE_INF, SCORE_INF, itDepth, false);
        rem.set(score, itDepth, rootPv);

        // Have not fully searched any child of the root node. Abort and stop.
        if (Threads.terminate && is_empty(rootPv))
            validResult = false;

        // Stop the iteration if we have exceeded the time limit or have found the
        // win or lose move. In yixin board, stop the iteration after itDepth reaches
        // the remaining move number when the game is a certain win or lose.
        if (Threads.terminate || (abs(rem.score) > SCORE_WIN_THRESHOLD && (!Threads.yxprotocol || itDepth >= int(SCORE_WIN - abs(rem.score)))))
            breakSearch = true;

        // Entries in rootBests should be valid and meaningful
        if (validResult) {
            assert(is_ok(rem.pv[0]));
            assert(is_ok(rem.score));

            // Record if best move changes
            if (rootBests.empty() || rem.pv[0] != rootBests.back().pv[0])
                bestMoveChanges = true;

            // Save the result
            rootBests.emplace_back(rem);

            // Update the best thread
            if (rem.depth > Threads.get_rem_depth()) {
                if (abs(Threads.get_rem_score()) > SCORE_WIN_THRESHOLD) {
                    if (abs(rem.score) > SCORE_WIN_THRESHOLD)
                        Threads.set_best_thread_with_rem(this, rem.score, rem.depth);
                } else {
                    Threads.set_best_thread_with_rem(this, rem.score, rem.depth);
                }
            }

            // Yixin board real time analysis: best move
            if (Threads.yxprotocol && this == Threads.get_best_thread())
                sync_cout << "MESSAGE REALTIME BEST " << rank_of(rem.pv[0]) << "," << file_of(rem.pv[0]) << sync_endl;
        }

        if (!breakSearch && itDepth < DEPTH_ITERATIVE_MAX) {
            // Print every iteration message in yixin board
            if (Threads.yxprotocol && this == Threads.get_best_thread())
                print_message();

            // Adjust turn time
            if (this == Threads.main()) {
                timeIncrease = 1.0;

                // If the best move changes, reset turn time to max
                if (bestMoveChanges) {
                    Threads.turnTime = Threads.turnTimeMax;
                    bestMoveChanges  = false;
                }

                // If the best move remains, reduce turn time gradually
                else if (itDepth >= 7)
                    timeIncrease *= 0.97;

                // Update turn time
                Threads.turnTime = static_cast<TimePoint>(Threads.turnTime * timeIncrease);
                Threads.turnTime = std::clamp(Threads.turnTime, Threads.turnTimeMin, Threads.turnTimeMax);

                // Terminate if we do not have enough time for the next iteration
                if (Threads.timer.elapsed() > Threads.turnTime * 0.7) {
                    Threads.terminate = true;
                    break;
                }
            }

            ++itDepth;
        } else {
            Threads.terminate = true;
            break;
        }
    }

    if (this == Threads.get_best_thread()) {
        assert(rootBests.size() > 0);
        print_message();
    }
}

// Thread::alphabeta() is the search function for both pv and non-pv nodes
template <NodeType NT>
Score Thread::alphabeta(Score alpha, Score beta, Depth depth, bool cautious) {
    // Check stop search
    if (Threads.terminate)
        return ply & 1u ? SCORE_WIN : -SCORE_WIN;

    // Check timeout
    if ((nodeCnt & 511u) == 511u && Threads.timer.elapsed() > Threads.turnTime)
        Threads.terminate = true;

    // Update search stats
    plyMax = std::max(plyMax, ply);
    ++nodeCnt;

    const bool PvNode   = NT == PV;
    const bool rootNode = ply == 0;
    Piece      piece;
    int        offset;

    // Check for win/lose/draw
    if ((piece = bd.check_wld(offset)) != PIECE_NONE)
        return piece == bd.sideToMove ? SCORE_WIN - ply - offset : piece == bd.oppoToMove ? -SCORE_WIN + ply + offset :
                                                               piece == PIECE_DRAW        ? SCORE_DRAW :
                                                                                            SCORE_NONE;

    // Victory distance pruning
    if (!rootNode) {
        alpha = std::max(-SCORE_WIN + ply, alpha);
        beta  = std::min(SCORE_WIN - ply - 1, beta);
        if (alpha >= beta)
            return alpha;
    }

    // Static evaluation
    Score staticScore = bd.evaluate();
    Score score       = staticScore;
    Depth vcfDepth    = Depth(std::min(int(ply) * 2, int(DEPTH_MAX)));

    // Return when depth reaches zero or ply reaches max depth
    if (depth <= DEPTH_ZERO || ply >= DEPTH_MAX) {
        // Try VCF to beat beta
        if (staticScore < beta && bd.query(bd.sideToMove, B3) > 0 && (score = vcf<NT>(vcfDepth, true)) > SCORE_WIN_THRESHOLD)
            return score;

        // Return static evaluation
        return staticScore;
    }

    ExtMove    em;
    Move       bestMove, ttMove;
    Score      bestScore, ttScore;
    Depth      newDepth;
    TTEntry *  tte;
    ZobristKey key;
    Pv         childPv;
    int        moveCnt;
    bool       ttHit, defendB4, quietNode, extend, doFullDepthSearch;

    assert(-SCORE_INF <= alpha && alpha < beta && beta <= SCORE_INF);
    assert(PvNode || alpha == beta - 1);
    assert(DEPTH_ZERO < depth && depth <= DEPTH_MAX);
    assert(DEPTH_ZERO <= ply && ply < DEPTH_MAX);

    // Initialization
    bestMove  = MOVE_NONE;
    ttMove    = MOVE_NONE;
    bestScore = -SCORE_INF;
    ttScore   = SCORE_NONE;
    tte       = nullptr;
    key       = bd.key;
    moveCnt   = 0;
    ttHit     = false;
    defendB4  = bd.query(bd.oppoToMove, B4) > 0;
    quietNode = bd.is_quiet();
    extend    = false;
    reset_pv(childPv);

    ss[ply + 2].killers[0] = MOVE_NONE;
    ss[ply + 2].killers[1] = MOVE_NONE;

    // Transposition table lookup
    tte     = TT.probe(key, ttHit);
    ttMove  = rootNode && !rootBests.empty() ? rootBests.back().pv[0] : ttHit ? tte->move() :
                                                                                MOVE_NONE;
    ttScore = rootNode && !rootBests.empty() ? rootBests.back().score : ttHit ? score_from_tt(tte->score(), ply) :
                                                                                SCORE_NONE;

    // TT cutoff: win move
    if (!PvNode && ttHit && ttScore > SCORE_WIN_THRESHOLD && (tte->bound() & BOUND_LOWER)) {
        // Update history
        if (ttScore >= beta)
            update_history(ttMove);

        return ttScore;
    }

    // TT cutoff: fail high/low move
    if (!PvNode && ttHit && tte->depth() >= depth && ttScore != SCORE_NONE // Possible in case of TT access race
        && (ttScore >= beta ? (tte->bound() & BOUND_LOWER) : (tte->bound() & BOUND_UPPER))) {
        // Update history
        if (ttScore >= beta)
            update_history(ttMove);

        return ttScore;
    }

    // Skip early pruning if opponent has B4
    if (defendB4)
        goto moves_loop;

    // Check if the score in TT is more accurate
    if (ttHit && (tte->bound() & (ttScore > staticScore ? BOUND_LOWER : BOUND_UPPER)))
        staticScore = ttScore;

    // Razoring
    if (!rootNode && depth < 5 && staticScore + futility_margin(depth) <= alpha)
        return alphabeta<NT>(alpha, beta, DEPTH_ZERO, cautious);

    // Extended Futility pruning
    if (!rootNode && depth < 7 && staticScore - futility_margin(depth) >= beta && staticScore < SCORE_WIN_THRESHOLD) // Do not return not verified wins
        return staticScore;

    // Internal iterative deepening
    if (depth >= 7 && ttMove == MOVE_NONE) {
        alphabeta<NT>(alpha, beta, depth / 2, cautious);

        tte     = TT.probe(key, ttHit);
        ttMove  = ttHit ? tte->move() : MOVE_NONE;
        ttScore = ttHit ? score_from_tt(tte->score(), ply) : SCORE_NONE;
    }

moves_loop:
    Move    cm = bd.pieceCnt >= 1 ? counterMoves[bd.last_move(1)] : MOVE_NONE;
    MoveGen mg(&bd, MAIN_TT, ttMove, false, ply, ss[ply].killers, cm);

    // Loop through all moves until no moves remain or a beta cutoff occurs
    while ((em = mg.next_move()).move != MOVE_NONE) {
        assert(is_ok(em.move));

        ++moveCnt;

        // Pruning based on move count
        if (!cautious && ply >= 2) {
            if (moveCnt > FutilityMoveCount[quietNode][depth])
                break;
        } else {
            if (moveCnt > FutilityMoveCount[quietNode][depth] && em.score < SEE_THRESHOLD)
                break;
        }

        // Speculative prefetch as early as possible
        prefetch(TT.first_entry(bd.key_after(em.move)));

        newDepth = depth - 1;

        // Extend if defend B4
        if (defendB4)
            extend = true;

        // Calculate new depth for this move
        newDepth = std::min(newDepth + extend, DEPTH_MAX);

        assert(DEPTH_ZERO <= newDepth && newDepth <= DEPTH_MAX);

        // Make the move
        ss[++ply].pv = &childPv;
        bd.do_move(em.move);

        // LMR Search. Moves will be re-searched at full depth if fail high.
        if (depth >= 3 && moveCnt > 1) {
            Depth r = Reduction[PvNode][depth][moveCnt];

            Depth d = std::clamp(newDepth - r, Depth(1), newDepth);

            score = -alphabeta<NonPV>(-alpha - 1, -alpha, d, cautious);

            doFullDepthSearch = score > alpha && d != newDepth;
        } else
            doFullDepthSearch = !PvNode || moveCnt > 1;

        // Full depth search when LMR is skipped or fails high
        if (doFullDepthSearch)
            score = -alphabeta<NonPV>(-alpha - 1, -alpha, newDepth, cautious);

        // For pv nodes only, do a full pv search on the first move or after a fail
        // high (in the latter case search only if score < beta), otherwise let the
        // parent node fail low with score <= alpha and try another move.
        if (PvNode && (moveCnt == 1 || (score > alpha && (rootNode || score < beta))))
            score = -alphabeta<PV>(-beta, -alpha, newDepth, cautious);

        // Do verification search if we find a win move
        if (PvNode && ply >= 2 && !cautious && score > SCORE_WIN_THRESHOLD) {
            Score s = -alphabeta<PV>(-SCORE_WIN_THRESHOLD, -SCORE_WIN_THRESHOLD + 1, newDepth, true);

            // If fails low, do cautious re-search
            if (s < SCORE_WIN_THRESHOLD)
                score = -alphabeta<PV>(-beta, -alpha, newDepth, true);
        }

        // Un-make the move
        bd.undo_move();
        --ply;

        // If stop search is set, return without updating anything
        if (Threads.terminate)
            return bestScore;

        // Alpha-beta pruning and pv update
        if (score > bestScore) {
            bestScore = score;

            if (score > alpha) {
                bestMove = em.move;

                // Do not update pv when fails high at root node. This may happen
                // when aspiration windows are applied
                if (PvNode && (!rootNode || score < beta)) {
                    assert(ss[ply + 1].pv);
                    update_pv(ss[ply].pv, em.move, ss[ply + 1].pv);
                }

                // Update alpha
                if (PvNode && score < beta)
                    alpha = score;
                else {
                    assert(score >= beta);
                    break; // Fail high
                }
            }
        }
    }

    // Update history
    if (bestMove != MOVE_NONE)
        update_history(bestMove);

    // Save results in TT
    Bound bound = bestScore >= beta ? BOUND_LOWER : PvNode && bestMove != MOVE_NONE ? BOUND_EXACT :
                                                                                      BOUND_UPPER;
    tte->save(key, bestMove, score_to_tt(bestScore, ply), bound, false, depth);

    assert(is_ok(bestScore));

    return bestScore;
}

template <NodeType NT>
Score Thread::vcf(Depth depth, bool rootNode) {
    const bool PvNode = NT == PV;
    Piece      piece;
    int        offset;

    if (!rootNode) {
        // Update search stats
        plyMax = std::max(plyMax, ply);
        ++nodeCnt;

        // Check for win/lose/draw
        if ((piece = bd.check_wld(offset)) != PIECE_NONE)
            return piece == bd.sideToMove ? SCORE_WIN - ply - offset : piece == bd.oppoToMove ? -SCORE_WIN + ply + offset :
                                                                   piece == PIECE_DRAW        ? SCORE_DRAW :
                                                                                                SCORE_NONE;
    }

    // Return when depth reaches zero or ply reaches max depth
    if (depth <= DEPTH_ZERO || ply >= DEPTH_MAX)
        return SCORE_ZERO;

    assert(DEPTH_ZERO < depth && depth <= DEPTH_MAX);
    assert(DEPTH_ZERO <= ply && ply < DEPTH_MAX);

    ExtMove em;
    Move    b4d;
    Score   score, bestScore;
    Pv      childPv;
    int     moveCnt;

    // Initialization
    bestScore = -SCORE_INF;
    moveCnt   = 0;
    reset_pv(childPv);

    MoveGen mg(&bd, VCF_TT, MOVE_NONE, rootNode);

    // Loop through all moves until no moves remain or a win move is found
    while ((em = mg.next_move()).move != MOVE_NONE) {
        ++moveCnt;

        // Move count pruning
        if (!rootNode && moveCnt > 2)
            break;

        // Make the move to form B4
        ply += 2;
        ss[ply].pv = &childPv;
        bd.do_move(em.move);

        // Check sudden win/lose/draw
        if ((piece = bd.check_wld(offset)) != PIECE_NONE) {
            bd.undo_move();
            ply -= 2;

            if (piece == bd.sideToMove) {
                bestScore = SCORE_WIN - ply - offset;

                if (PvNode)
                    update_pv(ss[ply].pv, em.move);
                break;
            } else {
                --moveCnt;
                continue;
            }
        }

        // Make the move to defend B4
        bd.do_move(b4d = bd.defend_B4());

        score = vcf<NT>(depth - 2, false);

        // Un-make two moves
        bd.undo_move();
        bd.undo_move();
        ply -= 2;

        // Break once a win move is found
        if (score > SCORE_WIN_THRESHOLD) {
            bestScore = score;

            if (PvNode) {
                assert(ss[ply + 2].pv);
                update_pv(ss[ply].pv, em.move, b4d, ss[ply + 2].pv);
            }
            break;
        }
    }

    return bestScore;
}
