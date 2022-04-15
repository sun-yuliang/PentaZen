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

#include "search.h"

enum GenType {
    WLD,
    DEFEND_B4,
    DEFEND_F3,
    DEFAULT,
    LARGE,
    MAIN,
    TT_MOVE,
    VCF_ROOT,
    VCF_CHILD
};

enum Stage {
    MAIN_TT,
    MAIN_INIT,
    MAIN_PICK,
    MAIN_END,
    VCF_TT,
    VCF_INIT,
    VCF_PICK,
    VCF_END,
};

ENABLE_ADDITION_OPERATORS_ON(Stage)

// MoveGen class is used to generate moves as the requested type and pick one
// move at a time from the current position.
class MoveGen {
    friend std::ostream &operator<<(std::ostream &os, const MoveGen &mg);

public:
    typedef MoveList<ExtMove>::iterator             iterator;
    typedef const MoveList<ExtMove>::const_iterator const_iterator;
    typedef MoveList<ExtMove>::size_type            size_type;

    MoveGen(Board *bd,
            Stage  stg   = MAIN_TT,
            Move   ttm   = MOVE_NONE,
            bool   rnode = false,
            Depth  ply   = DEPTH_NONE,
            Move * karr  = nullptr,
            Move   cm    = MOVE_NONE);

    MoveGen(const MoveGen &) = delete;
    MoveGen &operator=(const MoveGen &) = delete;

    iterator begin() {
        return movelist.begin();
    }
    iterator end() {
        return movelist.end();
    }
    iterator current() {
        return movelist.begin() + picked;
    }
    int size() const {
        return movelist.size();
    }

    template <GenType T>
    ExtMove generate();
    ExtMove next_move();

private:
    MoveList<ExtMove> movelist;
    int               picked;

    Board *pbd;
    Stage  stage;
    Move   ttMove;
    bool   rootNode;
    Depth  ply;
    Move   killers[2];
    Move   counterMove;

    Score score_of(Move m);
};

std::ostream &operator<<(std::ostream &os, const MoveGen &mg);
