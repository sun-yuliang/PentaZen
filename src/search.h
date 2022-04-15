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

enum NodeType { NonPV,
                PV };

typedef NArray<Move, STACK_SIZE>    Pv;
typedef NArray<Move, MOVE_CAPACITY> CounterMoveHistory;

// SearchStackElement records information in the search tree
struct SearchStackElement {
    Pv * pv;
    Move killers[2];

    SearchStackElement()
        : pv(nullptr), killers{MOVE_NONE, MOVE_NONE} {
    }

    void update_killers(Move m) {
        if (killers[0] != m) {
            killers[1] = killers[0];
            killers[0] = m;
        }
    }
};

typedef NArray<SearchStackElement, STACK_SIZE> SearchStack;

// RootExtMove struct is a score-depth-pv triple for root nodes.
struct RootExtMove {
    Score score;
    Depth depth;
    Pv    pv;

    RootExtMove()
        : score(SCORE_NONE), depth(DEPTH_ZERO) {
        pv[0] = MOVE_NONE;
    }

    void set(Score s, Depth d, const Pv &p) {
        score = s;
        depth = d;
        pv    = p;
    }
};

void search_init();
