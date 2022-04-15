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

#include "movelist.h"
#include "type.h"

#include <array>
#include <vector>

enum SideType { US,
                OPP };
enum Operation { INC,
                 DEC };

class Board;

// NArray is a generic N-dimensional array. The first template parameter T is
// the base type of the array, the other template parameters (Size and Sizes)
// encode the dimensions of the array.
template <typename T, int Size, int... Sizes>
struct NArray : public std::array<NArray<T, Sizes...>, Size> {
    // Fill the array with v
    void fill(const T &v) {
        T *p = reinterpret_cast<T *>(this);
        std::fill(p, p + sizeof(*this) / sizeof(T), v);
    }
};

template <typename T, int Size>
struct NArray<T, Size> : public std::array<T, Size> {};

// Interval struct stores a begin and an end index with some basic functions,
// working like iterators
struct Interval {
    void init(int bg, int ed) {
        begin_p = bg;
        end_p   = ed;
    }
    int begin() const {
        return begin_p;
    }
    int end() const {
        return end_p;
    }
    int length() const {
        return end_p - begin_p;
    }
    void set_begin(int bg) {
        begin_p = bg;
    }
    void set_end(int ed) {
        end_p = ed;
    }

private:
    int begin_p;
    int end_p;
};

// F3Pack class wraps up the necessary information of one F3
struct F3Pack {
    // More than one F3 in a line is not considered
    static constexpr int F4a_SIZE = 2;
    static constexpr int F3d_SIZE = 3;

    Piece     piece;
    Move      move;
    Direction direction;
    Move      F4a[F4a_SIZE];
    Move      F3d[F3d_SIZE];
    int       vind;
    int       gen;

    F3Pack(Piece p, Move m, Direction d, int ind);
    bool valid() const;
    template <Rule>
    void update(const Board &bd);
};

typedef std::vector<F3Pack> F3Packs;

inline F3Pack::F3Pack(Piece p, Move m, Direction d, int ind) {
    piece     = p;
    move      = m;
    direction = d;
    vind      = ind;
    gen       = 0;
    memset(F4a, 0, sizeof(F4a));
    memset(F3d, 0, sizeof(F3d));
}

inline bool F3Pack::valid() const {
    return F4a[0] == MOVE_NONE || F3d[0] == MOVE_NONE ? false : true;
}

inline bool operator==(const F3Pack &lhs, const F3Pack &rhs) {
    return lhs.piece == rhs.piece && lhs.vind == rhs.vind;
}

// Board class is the most important class, storing all necessary information
// for board operations in searching and game playing
class Board {
    friend std::ostream &operator<<(std::ostream &os, Board &bd);
    friend struct F3Pack;
    friend class MoveGen;

public:
    Board();
    Board(const Board &bd);
    Board &operator=(const Board &bd);

    void reset();

    bool is_empty() const;
    bool is_empty(Move m) const;
    Move last_move(int n) const;
    Move defend_B4() const;
    void switch_side_to_move();

    ZobristKey key_after(Move m) const;

    int query(Piece p, Material m) const;
    int query_inc(Piece p, Material m) const;
    int query_vcf(Piece p, Move m) const;
    template <SideType, Operation>
    int  query(Piece p, Move m, Material mat) const;
    bool is_quiet() const;
    bool is_quiet(Move m) const;

    Score see_of(Move m) const;
    Score evaluate() const;

    void do_move(Move m);
    void undo_move();

    Piece check_wld_already() const;
    Piece check_wld(int &offset) const;
    bool  is_foul(Move m);

public:
    // Data members
    int        pieceCnt;
    Piece      sideToMove;
    Piece      oppoToMove;
    ZobristKey key;

private:
    // Helper tables
    static NArray<int, MOVE_CAPACITY, DIRECTION_NUM> indexOfTable;
    static NArray<int, MOVE_CAPACITY, DIRECTION_NUM> indexOnTable;
    static NArray<Score, 16384>                      seeTable;

    // High level helpers
    int  index_of(Move m, Direction d) const;
    int  index_on(Move m, Direction d) const;
    void update_vectorBoard(Move m);
    void restore_vectorBoard(Move m);
    void update_material_see(Move m);
    void restore_see(Move m);
    void update_interval(Move m);
    void restore_interval(Move m);
    void update_movelist(Move m);

    // Low level helpers
    int  query_vectorBoard(Piece p, int vind, const Interval &itv) const;
    bool query_see(Piece p, int vind, int sind, uint32_t mask) const;
    template <Operation>
    void line_update(Piece p, Move m, Direction d, const Interval &itv);
    void F3Packs_update();
    void table_init();

    // Multi-dimensional array members
    NArray<Move, MOVE_SIZE>                                            pieceList;
    NArray<Piece, MOVE_CAPACITY>                                       board;
    NArray<int16_t, PIECE_NUM, MATERIAL_NUM>                           materialInc;
    NArray<int16_t, STACK_SIZE, PIECE_NUM, MATERIAL_NUM>               material;
    NArray<Score, STACK_SIZE, PIECE_NUM>                               score;
    NArray<uint32_t, STACK_SIZE, PIECE_NUM, DIRECTION_NUM, BOARD_SIDE> seeStack;
    NArray<uint32_t, PIECE_NUM, VECTOR_SIZE, BOARD_SIDE>               see;
    NArray<uint32_t, PIECE_NUM, VECTOR_SIZE>                           vectorBoard;
    NArray<Interval, PIECE_NUM, VECTOR_SIZE, BOARD_SIDE>               interval;
    NArray<MoveList<Move>, STACK_SIZE>                                 mListStack;
    NArray<F3Packs, STACK_SIZE>                                        F3Stack;
    NArray<Move, STACK_SIZE>                                           B4dStack;
    NArray<bool, STACK_SIZE>                                           updatedInterval;
    NArray<bool, STACK_SIZE>                                           updatedMoveList;
    NArray<int, PIECE_NUM>                                             F3FormedCnt;

    bool       tableGenerated = false;
    std::mutex mutex;
};

std::ostream &operator<<(std::ostream &os, const Board &bd);

inline bool Board::is_empty() const {
    return pieceCnt == 0;
}

inline bool Board::is_empty(Move m) const {
    return board[m] == EMPTY;
}

inline Move Board::last_move(int n) const {
    assert(pieceCnt >= n);

    return pieceList[pieceCnt - n];
}

inline Move Board::defend_B4() const {
    return B4dStack[pieceCnt];
}

inline void Board::switch_side_to_move() {
    sideToMove = ~sideToMove;
    oppoToMove = ~oppoToMove;
}

inline int Board::query(Piece p, Material m) const {
    return material[pieceCnt][p][m];
}

inline int Board::query_inc(Piece p, Material m) const {
    return materialInc[p][m];
}

inline int Board::query_vcf(Piece p, Move m) const {
    assert(is_ok(m));

    int ret = 0;

    for (auto d = Direction(0); d != DIRECTION_NUM; ++d)
        ret += query_see(p, index_of(m, d), index_on(m, d), 1u << 31);

    return ret;
}

template <>
inline int Board::query<US, INC>(Piece p, Move m, Material mat) const {
    assert(is_ok(m));
    assert(C6 <= mat && mat <= B1);

    int ret = 0;

    for (auto d = Direction(0); d != DIRECTION_NUM; ++d)
        ret += query_see(p, index_of(m, d), index_on(m, d), 1u << mat);

    return ret;
}

template <>
inline int Board::query<OPP, DEC>(Piece p, Move m, Material mat) const {
    assert(is_ok(m));
    assert(B4 <= mat && mat <= B1);

    int ret = 0;

    for (auto d = Direction(0); d != DIRECTION_NUM; ++d)
        ret += query_see(~p, index_of(m, d), index_on(m, d), 1u << (mat + 21));

    return ret;
}

inline bool Board::is_quiet() const {
    return query(BLACK, B4) || query(BLACK, F3) || query(WHITE, B4) || query(WHITE, F3) ? false : true;
}

inline bool Board::is_quiet(Move m) const {
    return query<US, INC>(sideToMove, m, B4) || query<US, INC>(sideToMove, m, F3) ? false : true;
}

// Return see value of the move.
inline Score Board::see_of(Move m) const {
    Score ret = SCORE_ZERO;

    for (auto d = Direction(0); d != DIRECTION_NUM; ++d)
        ret += seeTable[get_bits<uint32_t>(see[sideToMove][index_of(m, d)][index_on(m, d)], 3, 17)] - seeTable[get_bits<uint32_t>(see[oppoToMove][index_of(m, d)][index_on(m, d)], 17, 31)];

    return ret;
}

// Return static evaluation for side to move
inline Score Board::evaluate() const {
    return pieceCnt > 0 ? (score[pieceCnt][sideToMove] - score[pieceCnt][oppoToMove] + score[pieceCnt - 1][sideToMove] - score[pieceCnt - 1][oppoToMove]) / 2 : SCORE_ZERO;
}

inline int Board::index_of(Move m, Direction d) const {
    return indexOfTable[m][d];
}

inline int Board::index_on(Move m, Direction d) const {
    return indexOnTable[m][d];
}

inline void Board::update_vectorBoard(Move m) {
    for (auto d = Direction(0); d != DIRECTION_NUM; ++d)
        set_bit<uint32_t>(vectorBoard[sideToMove][index_of(m, d)], index_on(m, d));
}

inline void Board::restore_vectorBoard(Move m) {
    for (auto d = Direction(0); d != DIRECTION_NUM; ++d)
        reset_bit<uint32_t>(vectorBoard[sideToMove][index_of(m, d)], index_on(m, d));
}

inline int Board::query_vectorBoard(Piece p, int vind, const Interval &itv) const {
    return get_bits<uint32_t>(vectorBoard[p][vind], itv.begin(), itv.end());
}

inline bool Board::query_see(Piece p, int vind, int sind, uint32_t mask) const {
    return see[p][vind][sind] & mask;
}
