/*      _____                __    ______
 *     / ___ \              / /   /___  /
 *    / /__/ /___  ____  __/ /_______/ /    ____  ____
 *   / _____/ __ \/ __ \/_   _/ __  / /    / __ \/ __ \
 *  / /    /  ___/ / / / / /_/ /_/ / /____/  ___/ / / /
 * /_/     \____/_/ /_/ /___/\__,_/______/\____/_/ /_/
 *
 * PentaZen, a Gomoku/Renju playing engine developed by Sun Yuliang.
 */

#include "movegen.h"

MoveGen::MoveGen(Board *bd, Stage stg, Move ttm, bool rnode, Depth p, Move *karr, Move cm) {
    // Late movelist update
    if (!bd->updatedMoveList[bd->pieceCnt]) {
        bd->update_movelist(bd->last_move(1));
        bd->updatedMoveList[bd->pieceCnt] = true;
    }

    picked   = 0;
    pbd      = bd;
    ttMove   = ttm != MOVE_NONE && pbd->mListStack[pbd->pieceCnt].contains(ttm) ? ttm : MOVE_NONE;
    stage    = stg + int(ttMove == MOVE_NONE);
    rootNode = rnode;
    ply      = p;

    if (karr != nullptr) {
        killers[0] = karr[0];
        killers[1] = karr[1];
    }
    counterMove = cm;
}

// MoveGen::score_of() returns the heuristic score for move picking
Score MoveGen::score_of(Move m) {
    Score score = pbd->see_of(m);
    int   dist[4];

    for (auto i = 0; i != 4; ++i)
        dist[i] = pbd->pieceCnt >= i + 1 ? distance_between(m, pbd->last_move(i + 1)) : 0;

    // Multiply distant weight
    if (dist[0] > 4 && dist[1] > 4 && dist[2] > 4 && dist[3] > 4)
        score *= 0.25;
    else if (dist[0] > 4 && dist[1] > 4)
        score *= 0.5;

    // Add killer move and counter move bonus
    score += (m == killers[0] || m == killers[1] || m == counterMove) ? BONUS_REFUTATION : SCORE_ZERO;

    return score;
}

// MoveGen::Generate<WLD> generates one move to win or lose with its actual score.
// It should only be called when pbd->check_wld(offset) does not return PIECE_NONE
// and offset is greater than 0. This function is not time critical.
template <>
ExtMove MoveGen::generate<WLD>() {
    int offset;

    assert(pbd->check_wld(offset) != PIECE_NONE);
    assert(offset > 0);

    pbd->check_wld(offset);

    // If we have F4 or B4, we form C5 to win
    if (pbd->query(pbd->sideToMove, F4) > 0 || pbd->query(pbd->sideToMove, B4) > 0)
        for (auto &m : pbd->mListStack[pbd->pieceCnt])
            if (pbd->query<US, INC>(pbd->sideToMove, m, C5) > 0) {
                movelist.insert(m, SCORE_WIN - offset);
                return *begin();
            }

    // If opponent has F4, we defend and lose. Cannot call pbd->query in this case
    if (pbd->query(pbd->oppoToMove, F4) > 0)
        for (auto &m : pbd->mListStack[pbd->pieceCnt]) {
            pbd->do_move(m);
            if (pbd->query_inc(pbd->sideToMove, F4) < 0) {
                pbd->undo_move();
                movelist.insert(m, -SCORE_WIN + offset);
                return *begin();
            }
            pbd->undo_move();
        }

    // If opponent has several B4, we defend and lose
    if (pbd->query(pbd->oppoToMove, B4) >= 2) {
        movelist.insert(pbd->defend_B4(), -SCORE_WIN + offset);
        return *begin();
    }

    // If we have F3 and neither has B4, we form F4 to win
    if (pbd->query(pbd->sideToMove, F3) > 0 && pbd->query(pbd->oppoToMove, B4) == 0)
        for (auto &i : pbd->F3Stack[pbd->pieceCnt])
            if (i.piece == pbd->sideToMove) {
                movelist.insert(i.F4a[0], SCORE_WIN - offset);
                return *begin();
            }

    // Should not reach here because the win move or lose move must exist
    assert(false);

    return {MOVE_NONE, SCORE_NONE};
}

// MoveGen::generate<DEFEND_B4> generates one move to defend opponent's B4
template <>
ExtMove MoveGen::generate<DEFEND_B4>() {
    assert(is_ok(pbd->defend_B4()));

    movelist.insert(pbd->defend_B4());

    assert(size() > 0);

    return *begin();
}

// MoveGen::generate<DEFEND_F3> generates moves to defend opponent's F3 or form
// side to move's B4.
template <>
ExtMove MoveGen::generate<DEFEND_F3>() {
    // Generate defending F3 moves from F3 pack. Calling the query function may
    // give wrong result. Add bonus so that these moves can be picked first.
    for (auto &i : pbd->F3Stack[pbd->pieceCnt])
        for (auto &m : i.F3d)
            if (m != MOVE_NONE)
                movelist.insert(m, score_of(m) + BONUS_F3D);

    for (auto &m : pbd->mListStack[pbd->pieceCnt])
        if (pbd->query<US, INC>(pbd->sideToMove, m, B4) > 0)
            movelist.insert(m, score_of(m));

    assert(size() > 0);

    return *begin();
}

// MoveGen::generate<ALL> generates all possible moves
template <>
ExtMove MoveGen::generate<DEFAULT>() {
    for (auto &m : pbd->mListStack[pbd->pieceCnt])
        movelist.insert(m, score_of(m));

    assert(size() > 0);

    return *begin();
}

// MoveGen::generate<LARGE> generates all possible moves with a larger neighbor
template <>
ExtMove MoveGen::generate<LARGE>() {
    Move m;

    for (auto i = 0; i != pbd->pieceCnt; ++i) {
        m = pbd->pieceList[i];
        for (auto &i : N3)
            if (pbd->is_empty(m + i) && !movelist.contains(m + i))
                movelist.insert(m + i, score_of(m + i));
    }

    assert(size() > 0);

    return *begin();
}

// MoveGen::generate<MAIN> generates all possible moves using different ways
// automatically, with their heuristic score. Call this function when wld is
// uncertain.
template <>
ExtMove MoveGen::generate<MAIN>() {
    if (pbd->query(pbd->oppoToMove, B4) > 0)
        generate<DEFEND_B4>();

    else if (pbd->query(pbd->oppoToMove, F3) > 0)
        generate<DEFEND_F3>();

    else if (ply < 2 && pbd->pieceCnt < 5)
        generate<LARGE>();

    else
        generate<DEFAULT>();

    assert(size() > 0);

    return *begin();
}

// MoveGen::generate<TT_MOVE> adds one legal tt move to the movelist
template <>
ExtMove MoveGen::generate<TT_MOVE>() {
    movelist.insert(ttMove);

    assert(size() > 0);

    return *begin();
}

// MoveGen::generate<VCF_ROOT> generates moves for VCF search root node
template <>
ExtMove MoveGen::generate<VCF_ROOT>() {
    if (pbd->query(pbd->oppoToMove, B4) > 0) {
        // If opponent has B4, we defend and form F4 or B4
        if (pbd->query<US, INC>(pbd->sideToMove, pbd->defend_B4(), F4) > 0 || pbd->query<US, INC>(pbd->sideToMove, pbd->defend_B4(), B4) > 0)
            movelist.insert(pbd->defend_B4());
    } else {
        // We form B4
        for (auto &m : pbd->mListStack[pbd->pieceCnt])
            if (pbd->query<US, INC>(pbd->sideToMove, m, B4) > 0 && (pbd->query<US, INC>(pbd->sideToMove, m, B4) >= 2 || pbd->query<US, INC>(pbd->sideToMove, m, F3) > 0 || pbd->query<US, INC>(pbd->sideToMove, m, B3) > 0 || pbd->query_vcf(pbd->sideToMove, m) > 0))
                movelist.insert(m, pbd->see_of(m));
    }
    return *begin();
}

// MoveGen::generate<VCF_ROOT> generates moves for VCF search child node
template <>
ExtMove MoveGen::generate<VCF_CHILD>() {
    Move m;

    if (pbd->query(pbd->oppoToMove, B4) > 0) {
        // If opponent has B4, we defend and form F4 or B4
        if (pbd->query<US, INC>(pbd->sideToMove, pbd->defend_B4(), F4) > 0 || pbd->query<US, INC>(pbd->sideToMove, pbd->defend_B4(), B4) > 0)
            movelist.insert(pbd->defend_B4());
    } else {
        // We form B4 in neighborhood
        for (auto &i : N4) {
            m = pbd->last_move(2) + i;

            if (pbd->is_empty(m) && pbd->query<US, INC>(pbd->sideToMove, m, B4) > 0 && (pbd->query<US, INC>(pbd->sideToMove, m, B4) >= 2 || pbd->query<US, INC>(pbd->sideToMove, m, F3) > 0 || pbd->query<US, INC>(pbd->sideToMove, m, B3) > 0 || pbd->query_vcf(pbd->sideToMove, m) > 0))
                movelist.insert(m, pbd->see_of(m));
        }
    }
    return *begin();
}

// MoveGen::next_move() is the most important function of the MoveGen class. It
// returns a new move every time it is called until there are no more moves left,
// picking the move with the highest score from a list of generated moves.
ExtMove MoveGen::next_move() {
    iterator bestIt;
    ExtMove  bestEm;

top:
    switch (stage) {
    case MAIN_TT:
    case VCF_TT:
        generate<TT_MOVE>();
        ++picked;
        ++stage;

        assert(is_ok(begin()->move));
        assert(pbd->mListStack[pbd->pieceCnt].contains(begin()->move));

        return *begin();

    case MAIN_INIT:
        generate<MAIN>();
        ++stage;
        goto top;

    case VCF_INIT:
        rootNode ? generate<VCF_ROOT>() : generate<VCF_CHILD>();
        ++stage;
        goto top;

    case MAIN_PICK:
    case VCF_PICK:
        // No available moves
        if (current() == end()) {
            ++stage;
            goto top;
        }

        // Choose the move with the largest see value
        bestIt = std::max_element(current(), end());
        bestEm = *bestIt;

        assert(current() <= bestIt && bestIt <= end());

        // Swap the picked move ahead
        movelist.swap(current(), bestIt);
        ++picked;

        assert(is_ok(bestEm.move));

        return bestEm;

    case MAIN_END:
    case VCF_END:
        return {MOVE_NONE, SCORE_NONE};
    }

    // Should not reach here
    assert(false);

    return {MOVE_NONE, SCORE_NONE};
}

std::ostream &operator<<(std::ostream &os, const MoveGen &mg) {
    std::cout << mg.movelist;
    return os;
}
