/*      _____                __    ______
 *     / ___ \              / /   /___  /
 *    / /__/ /___  ____  __/ /_______/ /    ____  ____
 *   / _____/ __ \/ __ \/_   _/ __  / /    / __ \/ __ \
 *  / /    /  ___/ / / / / /_/ /_/ / /____/  ___/ / / /
 * /_/     \____/_/ /_/ /___/\__,_/______/\____/_/ /_/
 *
 * PentaZen, a Gomoku/Renju playing engine developed by Sun Yuliang.
 */

#include "board.h"

#include "thread.h"

#include <iomanip>
#include <string>

#ifdef BOARD_SIDE_EQUALS_20
#    include "pattern_20f.h"
#endif

#ifdef BOARD_SIDE_EQUALS_15
#    include "pattern_15f.h"
#endif

#include "pattern_15s.h"

NArray<int, MOVE_CAPACITY, DIRECTION_NUM> Board::indexOfTable;
NArray<int, MOVE_CAPACITY, DIRECTION_NUM> Board::indexOnTable;
NArray<Score, 16384>                      Board::seeTable;

namespace {

NArray<ZobristKey, PIECE_NUM, MOVE_CAPACITY> Zobrists;

constexpr Score ScoreHelper[MATERIAL_NUM] = {
    SCORE_ZERO,
    SCORE_ZERO,
    SCORE_ZERO,
    B4Score,
    F3Score,
    B3Score,
    F2Score,
    B2Score,
    F1Score,
    B1Score,
};

constexpr Score SeeHelper[MATERIAL_NUM] = {
    SCORE_ZERO,
    SCORE_ZERO,
    SCORE_ZERO,
    B4SeeScore,
    F3SeeScore,
    B3SeeScore,
    F2SeeScore,
    B2SeeScore,
    F1SeeScore,
    B1SeeScore,
};

Move start_of(Move m, Direction d) {
    while (is_ok(m - D[d]))
        m -= D[d];
    return m;
}

Move end_of(Move m, Direction d) {
    while (is_ok(m + D[d]))
        m += D[d];
    return m;
}

int mdiag_of(Move m) {
    Move s = start_of(m, D_MDIAG);
    int  r = rank_of(s);
    return r == 0 ? file_of(s) : r + BOARD_SIDE - 1;
}

int adiag_of(Move m) {
    Move s = start_of(m, D_ADIAG);
    int  r = rank_of(s);
    return r == 0 ? file_of(s) : r + BOARD_SIDE - 1;
}

int mdiag_index_on(Move m) {
    int ret = 0;
    while (is_ok(m - D[D_MDIAG])) {
        m -= D[D_MDIAG];
        ++ret;
    }
    return ret;
}

int adiag_index_on(Move m) {
    int ret = 0;
    while (is_ok(m - D[D_ADIAG])) {
        m -= D[D_ADIAG];
        ++ret;
    }
    return ret;
}

int index_of_helper(Move m, Direction d) {
    return d == D_RANK ? rank_of(m) : d == D_FILE ? file_of(m) + BOARD_SIDE :
                                  d == D_MDIAG    ? mdiag_of(m) + BOARD_SIDE * 2 :
                                  d == D_ADIAG    ? adiag_of(m) + BOARD_SIDE * 4 - 1 :
                                                    0;
}

int index_on_helper(Move m, Direction d) {
    return d == D_RANK ? file_of(m) : d == D_FILE ? rank_of(m) :
                                  d == D_MDIAG    ? mdiag_index_on(m) :
                                  d == D_ADIAG    ? adiag_index_on(m) :
                                                    0;
}

} // namespace

// F3 pack update function
template <Rule r>
void F3Pack::update(const Board &bd) {
    int ind1 = 0, ind2 = 0;

    memset(F4a, 0, sizeof(F4a));
    memset(F3d, 0, sizeof(F3d));

    for (auto m = move - D[direction] * 4; m <= move + D[direction] * 4; m += D[direction])
        if (bd.is_empty(m)) {
            if (ind1 < F3Pack::F4a_SIZE && bd.query<US, INC>(piece, m, F4) > 0)
                F4a[ind1++] = m;

            if (ind2 < F3Pack::F3d_SIZE && bd.query<OPP, DEC>(~piece, m, F3) > 0)
                F3d[ind2++] = m;
        }
}

// F3 pack update function for black in renju rule
template <>
void F3Pack::update<RENJU>(const Board &bd) {
    int ind1 = 0, ind2 = 0;

    memset(F4a, 0, sizeof(F4a));
    memset(F3d, 0, sizeof(F3d));

    for (auto m = move - D[direction] * 4; m <= move + D[direction] * 4; m += D[direction])
        if (bd.is_empty(m)) {
            if (ind1 < F3Pack::F4a_SIZE && bd.query<US, INC>(piece, m, C6) == 0 && bd.query<US, INC>(piece, m, F4) == 1 && bd.query<US, INC>(piece, m, B4) == 0 && bd.query<US, INC>(piece, m, F3) <= 1)
                F4a[ind1++] = m;

            if (ind2 < F3Pack::F3d_SIZE && bd.query<OPP, DEC>(~piece, m, F3) > 0)
                F3d[ind2++] = m;
        }
}

Board::Board() {
    mutex.lock();
    if (tableGenerated == false) {
        table_init();
    }
    mutex.unlock();

    reset();
}

Board::Board(const Board &bd) {
    reset();
    for (auto i = 0; i != bd.pieceCnt; ++i)
        do_move(bd.pieceList[i]);
}

Board &Board::operator=(const Board &bd) {
    reset();
    for (auto i = 0; i != bd.pieceCnt; ++i)
        do_move(bd.pieceList[i]);
    return *this;
}

ZobristKey Board::key_after(Move m) const {
    return key ^ Zobrists[sideToMove][m];
}

template <>
void Board::line_update<INC>(Piece p, Move m, Direction d, const Interval &itv) {
    int iof = index_of(m, d);
    int ion = index_on(m, d);

    // Reset related see array elements if interval length is smaller than 5
    if (itv.length() < 5) {
        for (auto i = itv.begin(); i != itv.end(); ++i)
            see[p][iof][i] = 0;
        return;
    }

    // Get main table entry pointer
    const uint32_t *ptr = (Threads.rule == FREESTYLE || (Threads.rule == RENJU && p == WHITE)) ? Pattern_f[query_vectorBoard(p, iof, itv) + (1 << itv.length()) - 1] : Pattern_s[query_vectorBoard(p, iof, itv) + (1 << itv.length()) - 1];

    F3Pack pack(p, m, d, iof);
    bool   formF3 = false;
    int    ind1 = 0, ind2 = 0;

    // The first element contains merged material info
    uint32_t ele = *ptr++, mat;

    // Update materialInc and score
    while ((mat = (ele & 0xf)) != MATERIAL_NONE) {
        ele >>= 4;
        ++materialInc[p][mat];
        score[pieceCnt][p] += ScoreHelper[mat];

        if (mat == F3)
            formF3 = true;
    }

    // The rest elements contain see info. Copy them to the relevant see array
    // elements. If new F3 is formed and does not exist in F3 stack yet, construct
    // the pack and save it in F3Stack.
    if (formF3 && std::find(F3Stack[pieceCnt].begin(), F3Stack[pieceCnt].end(), pack) == F3Stack[pieceCnt].end()) {
        for (auto i = itv.begin(); i != itv.end(); ++i) {
            // Construct F3 pack
            if ((*ptr & (1u << 2)) && ind1 < F3Pack::F4a_SIZE)
                pack.F4a[ind1++] = m + D[d] * (i - ion);

            if ((*ptr & (1u << 25)) && ind2 < F3Pack::F3d_SIZE)
                pack.F3d[ind2++] = m + D[d] * (i - ion);

            // Save move defending B4
            if (*ptr & (1u << 24))
                B4dStack[pieceCnt] = m + D[d] * (i - ion);

            // Update see array
            see[p][iof][i] = *ptr++;
        }

        // Save F3 pack
        F3Stack[pieceCnt].emplace_back(pack);
    }

    // No F3 forms
    else
        for (auto i = itv.begin(); i != itv.end(); ++i) {
            // Save move defending B4
            if (*ptr & (1u << 24))
                B4dStack[pieceCnt] = m + D[d] * (i - ion);

            // Update see array
            see[p][iof][i] = *ptr++;
        }
}

template <>
void Board::line_update<DEC>(Piece p, Move m, Direction d, const Interval &itv) {
    // Nothing to do if interval length is smaller than 5
    if (itv.length() < 5)
        return;

    const int tmp = query_vectorBoard(p, index_of(m, d), itv);

    // Nothing to do if there is no piece on the interval
    if (tmp == 0)
        return;

    // Get main table entry pointer
    const uint32_t *ptr = (Threads.rule == FREESTYLE || (Threads.rule == RENJU && p == WHITE)) ? Pattern_f[tmp + (1 << itv.length()) - 1] : Pattern_s[tmp + (1 << itv.length()) - 1];

    // The first element contains merged material info
    uint32_t ele = *ptr, mat;

    // Update materialInc and score
    while ((mat = (ele & 0xf)) != MATERIAL_NONE) {
        ele >>= 4;
        --materialInc[p][mat];
        score[pieceCnt][p] -= ScoreHelper[mat];
    }
}

void Board::F3Packs_update() {
    int16_t F3cnt[PIECE_NUM] = {0, 0};
    auto    it               = F3Stack[pieceCnt].begin();

    while (it != F3Stack[pieceCnt].end()) {
        // Update each F3 pack
        Threads.rule == RENJU && it->piece == BLACK ? it->update<RENJU>(*this) : it->update<FREESTYLE>(*this);

        // Erase if not valid after update
        if (!it->valid())
            it = F3Stack[pieceCnt].erase(it);
        else {
            ++F3cnt[it->piece];
            if (it->gen <= 0)
                ++F3FormedCnt[it->piece]; // Record exact F3 formed number for foul judgement
            it++->gen += 1;
        }
    }

    // Re-update F3 in material
    for (auto p = Piece(0); p != PIECE_NUM; ++p)
        material[pieceCnt][p][F3] = F3cnt[p];

    // Re-update F3 in materialInc
    for (auto p = Piece(0); p != PIECE_NUM; ++p)
        materialInc[p][F3] = material[pieceCnt][p][F3] - material[pieceCnt - 1][p][F3];
}

void Board::update_material_see(Move m) {
    // Reset materialInc. Make material array, score array, F3 stack and B4d stack grow.
    materialInc.fill(0);
    F3FormedCnt.fill(0);
    material[pieceCnt] = material[pieceCnt - 1];
    score[pieceCnt]    = score[pieceCnt - 1];
    F3Stack[pieceCnt]  = F3Stack[pieceCnt - 1];
    B4dStack[pieceCnt] = B4dStack[pieceCnt - 1];

    // Save related see array elements in see stack for restoring
    for (auto p = Piece(0); p != PIECE_NUM; ++p)
        for (auto d = Direction(0); d != DIRECTION_NUM; ++d)
            seeStack[pieceCnt - 1][p][d] = see[p][index_of(m, d)];

    // Substract old info on four directions
    for (auto p = Piece(0); p != PIECE_NUM; ++p)
        for (auto d = Direction(0); d != DIRECTION_NUM; ++d)
            line_update<DEC>(p, m, d, interval[p][index_of(m, d)][index_on(m, d)]);

    // Update vector board
    update_vectorBoard(m);

    int      iof, ion;
    Interval tmpitv;

    // Update line info on four directions
    for (auto d = Direction(0); d != DIRECTION_NUM; ++d) {
        iof = index_of(m, d);
        ion = index_on(m, d);

        line_update<INC>(sideToMove, m, d, interval[sideToMove][iof][ion]);

        tmpitv.init(interval[oppoToMove][iof][ion].begin(), ion);
        line_update<INC>(oppoToMove, m, d, tmpitv);

        tmpitv.init(ion + 1, interval[oppoToMove][iof][ion].end());
        line_update<INC>(oppoToMove, m, d, tmpitv);
    }

    // Add materialInc to material
    for (auto p = Piece(0); p != PIECE_NUM; ++p)
        for (auto i = Material(0); i != MATERIAL_NUM; ++i)
            material[pieceCnt][p][i] += materialInc[p][i];

    // Update F3 packs
    F3Packs_update();
}

// Copy the backup info in seeStack to the related see elements
void Board::restore_see(Move m) {
    for (auto p = Piece(0); p != PIECE_NUM; ++p)
        for (auto d = Direction(0); d != DIRECTION_NUM; ++d)
            see[p][index_of(m, d)] = seeStack[pieceCnt][p][d];
}

void Board::update_interval(Move m) {
    int iof, ion;

    for (auto d = Direction(0); d != DIRECTION_NUM; ++d) {
        iof = index_of(m, d);
        ion = index_on(m, d);

        for (auto i = interval[oppoToMove][iof][ion].begin(); i != ion; ++i)
            interval[oppoToMove][iof][i].set_end(ion);

        for (auto i = ion + 1; i != interval[oppoToMove][iof][ion].end(); ++i)
            interval[oppoToMove][iof][i].set_begin(ion + 1);
    }
}

void Board::restore_interval(Move m) {
    int iof, ion;

    for (auto d = Direction(0); d != DIRECTION_NUM; ++d) {
        iof = index_of(m, d);
        ion = index_on(m, d);

        for (auto i = interval[oppoToMove][iof][ion].begin(); i != interval[oppoToMove][iof][ion].end(); ++i)
            interval[oppoToMove][iof][i] = interval[oppoToMove][iof][ion];
    }
}

void Board::update_movelist(Move m) {
    mListStack[pieceCnt] = mListStack[pieceCnt - 1];
    mListStack[pieceCnt].remove(m);

    for (auto &i : N2)
        if (is_empty(m + i))
            mListStack[pieceCnt].insert(m + i);
}

// Update the board after making a move. Update order is critical.
void Board::do_move(Move m) {
    assert(is_ok(m));
    assert(is_empty(m));
    assert(0 <= pieceCnt && pieceCnt < MOVE_SIZE);

    // Late interval update
    if (pieceCnt > 0 && !updatedInterval[pieceCnt]) {
        switch_side_to_move();
        update_interval(last_move(1));
        switch_side_to_move();
        updatedInterval[pieceCnt] = true;
    }

    // Late movelist update
    if (pieceCnt > 0 && !updatedMoveList[pieceCnt]) {
        update_movelist(last_move(1));
        updatedMoveList[pieceCnt] = true;
    }

    // Realtime update
    board[m]              = sideToMove;
    pieceList[pieceCnt++] = m;
    update_material_see(m);
    // update_interval(m);
    // update_movelist(m);
    key ^= Zobrists[sideToMove][m];
    switch_side_to_move();

    // Record later updates
    updatedInterval[pieceCnt] = false;
    updatedMoveList[pieceCnt] = false;
}

// Restore the board after taking a piece. Restore order is critical.
void Board::undo_move() {
    assert(0 < pieceCnt && pieceCnt <= MOVE_SIZE);

    switch_side_to_move();
    Move lastMove   = pieceList[--pieceCnt];
    board[lastMove] = EMPTY;
    restore_see(lastMove);
    restore_vectorBoard(lastMove);

    if (updatedInterval[pieceCnt + 1])
        restore_interval(lastMove);

    materialInc.fill(0);
    F3FormedCnt.fill(0);
    key ^= Zobrists[sideToMove][lastMove];
}

// Return the winning/losing/drawing piece if the game is already over. Return
// PIECE_NONE if the game should continue.
Piece Board::check_wld_already() const {
    if (query(BLACK, C5) > 0)
        return BLACK;

    if (query(WHITE, C5) > 0)
        return WHITE;

    if (pieceCnt >= MOVE_SIZE)
        return PIECE_DRAW;

    if ((Threads.rule == RENJU) && (query(BLACK, C6) > 0 || query_inc(BLACK, F4) + query_inc(BLACK, B4) >= 2 || F3FormedCnt[BLACK] >= 2))
        return WHITE;

    return PIECE_NONE;
}

// Return the winning/losing/drawing piece or PIECE_NONE by quiescence check.
// If the return value is not PIECE_NONE, offset will be updated as the move
// number to game over.
Piece Board::check_wld(int &offset) const {
    Piece p = check_wld_already();

    if (p != PIECE_NONE) {
        offset = 0;
        return p;
    }

    // side to move has F4 or B4
    if (query(sideToMove, F4) > 0 || query(sideToMove, B4) > 0) {
        offset = 1;
        return sideToMove;
    }

    // oppo to move has F4 or several B4
    if (query(oppoToMove, F4) > 0 || query(oppoToMove, B4) >= 2) {
        offset = 2;
        return oppoToMove;
    }

    // side to move has F3 and both do not have B4
    if (query(sideToMove, F3) > 0 && query(oppoToMove, B4) == 0) {
        offset = 3;
        return sideToMove;
    }

    return PIECE_NONE;
}

// Return true if the move is foul
bool Board::is_foul(Move m) {
    bool ret;
    bool needToSwitch = sideToMove != BLACK;

    if (needToSwitch)
        switch_side_to_move();

    do_move(m);
    ret = (Threads.rule == RENJU) && (query(BLACK, C6) > 0 || query_inc(BLACK, F4) + query_inc(BLACK, B4) >= 2 || F3FormedCnt[BLACK] >= 2) ? true : false;
    undo_move();

    if (needToSwitch)
        switch_side_to_move();

    return ret;
}

// Generate helper tables. Must be called once before reset the board.
void Board::table_init() {
    // Calculate index of and index on table. They must be generated at first
    // or some other tables will not be initialized correctly.
    for (auto m = Move(0); m != MOVE_CAPACITY; ++m)
        for (auto d = Direction(0); d != DIRECTION_NUM; ++d)
            if (is_ok(m)) {
                indexOfTable[m][d] = index_of_helper(m, d);
                indexOnTable[m][d] = index_on_helper(m, d);
            }

    // Calculate see table
    seeTable.fill(SCORE_ZERO);
    for (uint16_t i = 0; i != 16384; ++i) {
        for (auto m = B4; m <= B1; ++m)
            if (i & (1u << (m - 3)))
                seeTable[i] += SeeHelper[m];

        for (auto m = B4; m <= B1; ++m) {
            // Neglect B4 and F3 defending score because we do not use see value to
            // generate or pick these moves
            if (m == B4 || m == F3)
                continue;

            if (i & (1u << (m + 4)))
                seeTable[i] -= SeeHelper[m];
        }
    }

    // Generate Zobrists table
    PRNG rng(1070372);
    for (auto i = 0; i != PIECE_NUM; ++i)
        for (auto j = 0; j != MOVE_CAPACITY; ++j)
            Zobrists[i][j] = rng.rand<uint64_t>();

    tableGenerated = true;
}

// Reset the board to empty status. The reset order is critical.
void Board::reset() {
    // Reset member variables
    pieceCnt   = 0;
    sideToMove = BLACK;
    oppoToMove = ~sideToMove;
    key        = 0;

    // Fill these arrays with zeros
    material.fill(0);
    score.fill(SCORE_ZERO);
    see.fill(0);
    vectorBoard.fill(0);
    updatedInterval.fill(0);
    updatedMoveList.fill(0);

    // Reset the initial move list
    mListStack[pieceCnt].reset();

    // Set board array according to the square type
    for (auto m = Move(0); m != MOVE_CAPACITY; ++m)
        board[m] = is_ok(m) ? EMPTY : PIECE_OUT;

    // Initialize interval array
    for (auto p = Piece(0); p != PIECE_NUM; ++p)
        for (auto m = Move(0); m != MOVE_CAPACITY; ++m)
            if (is_ok(m))
                for (auto d = Direction(0); d != DIRECTION_NUM; ++d) {
                    interval[p][index_of(m, d)][index_on(m, d)].set_begin(0);
                    interval[p][index_of(m, d)][index_on(m, d)].set_end(distance_between(start_of(m, d), end_of(m, d)) + 1);
                }

    // Initialize see array. This array must be initialized because it is non-empty
    // even though there is no piece on the board.
    for (auto p = Piece(0); p != PIECE_NUM; ++p)
        for (auto d = Direction(0); d != DIRECTION_NUM; ++d)
            for (Move m = Move(0); m != MOVE_CAPACITY; ++m)
                if (is_ok(m))
                    line_update<INC>(p, m, ~d, interval[p][index_of(m, ~d)][index_on(m, ~d)]);

    updatedInterval[pieceCnt] = true;
    updatedMoveList[pieceCnt] = true;
}

// Print the board for test and debug.
std::ostream &operator<<(std::ostream &os, Board &bd) {
    // Display Board
    for (auto r = 0; r != BOARD_SIDE; ++r) {
        for (auto f = 0; f != BOARD_SIDE; ++f) {
            Move m = make_move(r, f);

            if (bd.board[m] == BLACK)
                std::cout << "X";
            else if (bd.board[m] == WHITE)
                std::cout << "O";
            // else if (bd.mListStack[bd.pieceCnt].contains(m))
            //     std::cout << "*";
            // else if (bd.query<OPP, DEC>(WHITE, m, F3))
            //     std::cout << "*";
            else if (bd.board[m] == EMPTY)
                std::cout << ".";

            std::cout << " ";
        }
        std::cout << BOARD_SIDE - r << std::endl;
    }

    for (auto i = 0; i != BOARD_SIDE; ++i)
        std::cout << std::left << std::setw(2) << static_cast<char>('A' + i) << std::right;

    // Display piece list
    std::cout << std::endl
              << bd.pieceCnt << (bd.pieceCnt < 2 ? " move " : " moves ");

    for (auto i = 0; i != bd.pieceCnt; ++i)
        std::cout << bd.pieceList[i] << " ";

    // Display side to move
    std::cout << std::endl
              << "side to move " << bd.sideToMove;

    // Display materials
    std::cout << std::endl
              << "      C6 C5 F4 B4 F3 B3 F2 B2 F1 B1";

    for (auto p = Piece(0); p != PIECE_NUM; ++p) {
        std::cout << std::endl
                  << p;

        for (auto i = Material(0); i != MATERIAL_NUM; ++i)
            std::cout << std::setw(3) << bd.query(p, i);
    }

    return os;
}
