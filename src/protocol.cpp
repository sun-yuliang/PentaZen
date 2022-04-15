/*      _____                __    ______
 *     / ___ \              / /   /___  /
 *    / /__/ /___  ____  __/ /_______/ /    ____  ____
 *   / _____/ __ \/ __ \/_   _/ __  / /    / __ \/ __ \
 *  / /    /  ___/ / / / / /_/ /_/ / /____/  ___/ / / /
 * /_/     \____/_/ /_/ /___/\__,_/______/\____/_/ /_/
 *
 * PentaZen, a Gomoku/Renju playing engine developed by Sun Yuliang.
 */

#include "protocol.h"

#include "thread.h"
#include "tt.h"

#include <sstream>

namespace {

void to_upper(std::string &str) {
    for (auto &c : str)
        if (isalpha(c))
            c = toupper(c);
}

} // namespace

void loop() {
    std::string       cmd, sub_cmd;
    std::stringstream ss;
    Move              move;
    int               num, r, f;
    char              comma;

    // Generate tables first
    search_init();

    // Initialize thread pool with board
    Threads.set(1);

    // Initialize tt with default size
    TT.resize(TT_SIZE);

    // Print engine info
    sync_cout << engine_info() << sync_endl;

    while (true) {
    top:
        std::cin >> cmd;
        to_upper(cmd);

        // Stop all threads when receive a command
        Threads.terminate = true;

        // Wait until all threads have stopped
        for (auto &i : Threads)
            i->wait_for_search_finished();

        if (cmd == "ABOUT")
            sync_cout << "name=\"" << ENGINE_NAME << "\", version=\"" << ENGINE_VERSION << "\", author=\"" << ENGINE_AUTHOR << "\", country=\"China\"" << sync_endl;

        else if (cmd == "BEGIN")
            Threads.think_and_move();

        else if (cmd == "BOARD" || cmd == "YXBOARD") {
            std::cin >> sub_cmd;
            to_upper(sub_cmd);

            while (sub_cmd != "DONE") {
                ss.clear();
                ss << sub_cmd;
                ss >> r >> comma >> f >> comma >> num;
                if (!is_ok(move = make_move(r, f))) {
                    sync_cout << "ERROR invalid move" << sync_endl;
                    goto top;
                }
                Threads.do_move(move);
                std::cin >> sub_cmd;
                to_upper(sub_cmd);
            }
            if (cmd == "BOARD")
                Threads.think_and_move();
        }

        else if (cmd == "END")
            break;

        else if (cmd == "INFO") {
            // Do not check input range. GUI should ensure that.
            std::cin >> sub_cmd;
            to_upper(sub_cmd);

            if (sub_cmd == "HASH_SIZE") {
                size_t kb;
                std::cin >> kb;
                TT.resize(kb / 1024);
            } else if (sub_cmd == "RULE") {
                std::cin >> num;
                if (BOARD_SIDE == 20 && num != FREESTYLE) {
                    sync_cout << "ERROR unsupported rule for this board size" << sync_endl;
                    goto top;
                }
                Threads.set_rule(Rule(num));
            } else if (sub_cmd == "THREAD_NUM") {
                std::cin >> num;
                Threads.set(num);
            } else if (sub_cmd == "TIME_LEFT")
                std::cin >> Threads.timeLeft;

            else if (sub_cmd == "TIMEOUT_MATCH")
                std::cin >> Threads.timeoutMatch;

            else if (sub_cmd == "TIMEOUT_TURN")
                std::cin >> Threads.timeoutTurn;
        }

        else if (cmd == "RECTSTART")
            sync_cout << "ERROR unsupported size" << sync_endl;

        else if (cmd == "RESTART") {
            Threads.reset();
            sync_cout << "OK" << sync_endl;
        }

        else if (cmd == "START") {
            std::cin >> num;
            if (num != BOARD_SIDE) {
                sync_cout << "ERROR unsupported size" << sync_endl;
                goto top;
            }
            Threads.reset();
            sync_cout << "OK" << sync_endl;
        }

        else if (cmd == "TAKEBACK") {
            Threads.undo_move();
            sync_cout << "OK" << sync_endl;
        }

        else if (cmd == "TURN") {
            std::cin >> r >> comma >> f;
            if (!is_ok(move = make_move(r, f))) {
                sync_cout << "ERROR invalid move" << sync_endl;
                goto top;
            }
            Threads.do_move(move);
            Threads.think_and_move();
        }

        else if (cmd == "YXHASHCLEAR") {
            TT.clear();
            Threads.clear_history(); // Clear histories as well
        }

        else if (cmd == "YXSHOWFORBID") {
            if (Threads.rule == RENJU) {
                sync_cout << "FORBID ";
                for (auto m = Move(0); m != MOVE_CAPACITY; ++m)
                    if (Threads.is_empty(m) && Threads.is_foul(m))
                        std::cout << (rank_of(m) < 10 ? "0" : "") << rank_of(m) << (file_of(m) < 10 ? "0" : "") << file_of(m);
                std::cout << "." << sync_endl;
            }
        }

        else if (cmd == "YXSHOWINFO") {
            Threads.yxprotocol = true;
            sync_cout << "MESSAGE INFO MAX_HASH_SIZE 24\n"
                      << "MESSAGE INFO MAX_THREAD_NUM 32" << sync_endl;
        }
#ifndef NDEBUG
        else if (cmd == "D") {
            std::cin >> r >> comma >> f;
            if (!is_ok(move = make_move(r, f))) {
                sync_cout << "ERROR invalid move" << sync_endl;
                goto top;
            }
            Threads.do_move(move);
        }

        else if (cmd == "U")
            Threads.undo_move();

        else if (cmd == "P")
            sync_cout << Threads.main()->bd << sync_endl;
#endif
    }
}
