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

#include "misc.h"

#include <algorithm>

#define ENGINE_NAME "PentaZen"
#define ENGINE_VERSION "0.4.17"
#define ENGINE_AUTHOR "Sun Yuliang"

#define BOARD_SIDE_EQUALS_15
// #define BOARD_SIDE_EQUALS_20

// Search and board constants
constexpr bool OUTPUT_MESSAGE = 1;
#ifdef BOARD_SIDE_EQUALS_20
constexpr int BOARD_SIDE = 20;
constexpr int TT_SIZE    = 128;
#endif
#ifdef BOARD_SIDE_EQUALS_15
constexpr int BOARD_SIDE = 15;
constexpr int TT_SIZE    = 256;
#endif
constexpr int BOARD_SIDE_BIT      = 5;
constexpr int BOARD_SIDE_CAPACITY = 1 << BOARD_SIDE_BIT;
constexpr int BOARD_BOUNDARY      = 4;
constexpr int STACK_SIZE          = BOARD_SIDE * BOARD_SIDE + 1;
constexpr int VECTOR_SIZE         = BOARD_SIDE * 6 - 2;

typedef uint64_t ZobristKey;

static_assert(sizeof(int) == sizeof(int32_t), "int should be 32 bits");

enum Rule {
    FREESTYLE = 0,
    STANDARD  = 1,
    RENJU     = 4,
};

enum Move : uint16_t {
    MOVE_NONE     = 0, // must be 0
    MOVE_SIZE     = BOARD_SIDE * BOARD_SIDE,
    MOVE_CAPACITY = BOARD_SIDE_CAPACITY * BOARD_SIDE_CAPACITY,
};

// BLACK refers to the first move side, not necessary to be black.
enum Piece {
    BLACK      = 0,
    WHITE      = 1,
    PIECE_NUM  = 2,
    EMPTY      = 2,
    PIECE_OUT  = 3,
    PIECE_DRAW = 4,
    PIECE_NONE = 5,
};

enum Direction {
    D_RANK,
    D_ADIAG,
    D_FILE,
    D_MDIAG,
    DIRECTION_NUM = 4,
};

enum Material {
    C6,
    C5,
    F4,
    B4,
    F3,
    B3,
    F2,
    B2,
    F1,
    B1,
    MATERIAL_NUM  = 10,
    MATERIAL_NONE = 15,
};

enum Score : int16_t {
    SCORE_ZERO          = 0,
    SCORE_DRAW          = 0,
    SCORE_WIN           = 32000,
    SCORE_INF           = 32001,
    SCORE_NONE          = 32002,
    SCORE_WIN_THRESHOLD = SCORE_WIN - STACK_SIZE,

    B4Score = 364,
    F3Score = 364,
    B3Score = 99,
    F2Score = 99,
    B2Score = 29,
    F1Score = 31,
    B1Score = 8,

    B4SeeScore = 2048,
    F3SeeScore = 2048,
    B3SeeScore = 256,
    F2SeeScore = 256,
    B2SeeScore = 16,
    F1SeeScore = 16,
    B1SeeScore = 1,

    SEE_THRESHOLD    = 12,
    BONUS_F3D        = 1024,
    BONUS_REFUTATION = 1536,
};

enum Bound : uint8_t {
    BOUND_NONE  = 0,
    BOUND_UPPER = 1,
    BOUND_LOWER = 2,
    BOUND_EXACT = BOUND_UPPER | BOUND_LOWER,
};

enum Depth : int8_t {
    DEPTH_ZERO          = 0,
    DEPTH_ITERATIVE_MIN = 1,
    DEPTH_ITERATIVE_MAX = 100,
    DEPTH_MAX           = 120,
    DEPTH_NUM           = DEPTH_MAX + 1,
    DEPTH_NONE          = 127,
};

constexpr int D[4] = {
    1,                         // right
    (1 << BOARD_SIDE_BIT) - 1, // down left
    (1 << BOARD_SIDE_BIT),     // down
    (1 << BOARD_SIDE_BIT) + 1, // down right
};

constexpr int N2[16] = {
    -D[3] - D[3],
    -D[2] - D[2],
    -D[1] - D[1],
    -D[3],
    -D[2],
    -D[1],
    -D[0] - D[0],
    -D[0],
    D[0],
    D[0] + D[0],
    D[1],
    D[2],
    D[3],
    D[1] + D[1],
    D[2] + D[2],
    D[3] + D[3],
};

constexpr int N3[32] = {
    -D[3] - D[3] - D[3],
    -D[2] - D[2] - D[2],
    -D[1] - D[1] - D[1],
    -D[3] - D[3],
    -D[3] - D[2],
    -D[2] - D[2],
    -D[2] - D[1],
    -D[1] - D[1],
    -D[3] - D[0],
    -D[3],
    -D[2],
    -D[1],
    -D[1] + D[0],
    -D[0] - D[0] - D[0],
    -D[0] - D[0],
    -D[0],
    D[0],
    D[0] + D[0],
    D[0] + D[0] + D[0],
    D[1] - D[0],
    D[1],
    D[2],
    D[3],
    D[3] + D[0],
    D[1] + D[1],
    D[1] + D[2],
    D[2] + D[2],
    D[3] + D[2],
    D[3] + D[3],
    D[1] + D[1] + D[1],
    D[2] + D[2] + D[2],
    D[3] + D[3] + D[3],
};

constexpr int N4[32] = {
    -D[3] - D[3] - D[3] - D[3],
    -D[2] - D[2] - D[2] - D[2],
    -D[1] - D[1] - D[1] - D[1],
    -D[3] - D[3] - D[3],
    -D[2] - D[2] - D[2],
    -D[1] - D[1] - D[1],
    -D[3] - D[3],
    -D[2] - D[2],
    -D[1] - D[1],
    -D[3],
    -D[2],
    -D[1],
    -D[0] - D[0] - D[0] - D[0],
    -D[0] - D[0] - D[0],
    -D[0] - D[0],
    -D[0],
    D[0],
    D[0] + D[0],
    D[0] + D[0] + D[0],
    D[0] + D[0] + D[0] + D[0],
    D[1],
    D[2],
    D[3],
    D[1] + D[1],
    D[2] + D[2],
    D[3] + D[3],
    D[1] + D[1] + D[1],
    D[2] + D[2] + D[2],
    D[3] + D[3] + D[3],
    D[1] + D[1] + D[1] + D[1],
    D[2] + D[2] + D[2] + D[2],
    D[3] + D[3] + D[3] + D[3],
};

#define ENABLE_ADDITION_OPERATORS_ON(T)                                     \
    constexpr T operator+(T d1, T d2) {                                     \
        return static_cast<T>(static_cast<int>(d1) + static_cast<int>(d2)); \
    }                                                                       \
    constexpr T operator-(T d1, T d2) {                                     \
        return static_cast<T>(static_cast<int>(d1) - static_cast<int>(d2)); \
    }                                                                       \
    constexpr T operator+(T d, int i) {                                     \
        return static_cast<T>(static_cast<int>(d) + i);                     \
    }                                                                       \
    constexpr T operator-(T d, int i) {                                     \
        return static_cast<T>(static_cast<int>(d) - i);                     \
    }                                                                       \
    constexpr T operator-(T d) {                                            \
        return static_cast<T>(-static_cast<int>(d));                        \
    }                                                                       \
    inline T &operator+=(T &d1, T d2) {                                     \
        return d1 = d1 + d2;                                                \
    }                                                                       \
    inline T &operator-=(T &d1, T d2) {                                     \
        return d1 = d1 - d2;                                                \
    }                                                                       \
    inline T &operator+=(T &d, int i) {                                     \
        return d = d + i;                                                   \
    }                                                                       \
    inline T &operator-=(T &d, int i) {                                     \
        return d = d - i;                                                   \
    }                                                                       \
    inline T &operator++(T &d) {                                            \
        return d = d + 1;                                                   \
    }                                                                       \
    inline T &operator--(T &d) {                                            \
        return d = d - 1;                                                   \
    }

#define ENABLE_SCALAR_OPERATORS_ON(T)                   \
    constexpr T operator*(T d, int i) {                 \
        return static_cast<T>(static_cast<int>(d) * i); \
    }                                                   \
    constexpr T operator*(int i, T d) {                 \
        return static_cast<T>(static_cast<int>(d) * i); \
    }                                                   \
    constexpr T operator/(T d, int i) {                 \
        return static_cast<T>(static_cast<int>(d) / i); \
    }                                                   \
    inline T &operator*=(T &d, int i) {                 \
        return d = d * i;                               \
    }                                                   \
    inline T &operator/=(T &d, int i) {                 \
        return d = d / i;                               \
    }

#define ENABLE_FLOAT_OPERATORS_ON(T)                                         \
    constexpr T operator*(T d, double i) {                                   \
        return static_cast<T>(static_cast<int>(static_cast<double>(d) * i)); \
    }                                                                        \
    constexpr T operator/(T d, double i) {                                   \
        return static_cast<T>(static_cast<int>(static_cast<double>(d) / i)); \
    }                                                                        \
    inline T &operator*=(T &d, double i) {                                   \
        return d = d * i;                                                    \
    }                                                                        \
    inline T &operator/=(T &d, double i) {                                   \
        return d = d / i;                                                    \
    }

ENABLE_ADDITION_OPERATORS_ON(Move)
ENABLE_ADDITION_OPERATORS_ON(Piece)
ENABLE_ADDITION_OPERATORS_ON(Material)
ENABLE_ADDITION_OPERATORS_ON(Direction)
ENABLE_ADDITION_OPERATORS_ON(Score)
ENABLE_ADDITION_OPERATORS_ON(Depth)
ENABLE_SCALAR_OPERATORS_ON(Score)
ENABLE_SCALAR_OPERATORS_ON(Depth)
ENABLE_FLOAT_OPERATORS_ON(Score)

inline std::string engine_info() {
    return std::string("MESSAGE ") + ENGINE_NAME + " " + ENGINE_VERSION + " by " + ENGINE_AUTHOR;
}

constexpr int rank_of(Move m) {
    return (m >> BOARD_SIDE_BIT) - BOARD_BOUNDARY;
}

constexpr int file_of(Move m) {
    return (m & ((1 << BOARD_SIDE_BIT) - 1)) - BOARD_BOUNDARY;
}

constexpr Move make_move(int r, int f) {
    return Move(((r + BOARD_BOUNDARY) << BOARD_SIDE_BIT) + f + BOARD_BOUNDARY);
}

inline int distance_between(Move m1, Move m2) {
    return std::max(abs(rank_of(m1) - rank_of(m2)), abs(file_of(m1) - file_of(m2)));
}

constexpr Piece operator~(Piece c) {
    return Piece(c ^ WHITE);
}

constexpr Direction operator~(Direction d) {
    return d == D_RANK ? D_FILE : d == D_FILE ? D_RANK :
                              d == D_MDIAG    ? D_ADIAG :
                              d == D_ADIAG    ? D_MDIAG :
                                                DIRECTION_NUM;
}

constexpr bool is_ok(Move m) {
    return 0 <= rank_of(m) && rank_of(m) < BOARD_SIDE && 0 <= file_of(m) && file_of(m) < BOARD_SIDE;
}

constexpr bool is_ok(Score s) {
    return -SCORE_INF < s && s < SCORE_INF;
}

template <typename T>
inline void set_bit(T &a, int ind) {
    a |= 1u << ind;
}

template <typename T>
inline void reset_bit(T &a, int ind) {
    a &= ~(1u << ind);
}

template <typename T>
inline T get_bits(T a, int begin, int end) {
    return (a >> begin) & ((1u << (end - begin)) - 1);
}

inline std::ostream &operator<<(std::ostream &os, Move m) {
    m != MOVE_NONE ? std::cout << char('a' + file_of(m)) << BOARD_SIDE - rank_of(m) : std::cout << "NONE";
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const Piece p) {
    std::cout << (p == BLACK ? "BLACK" : p == WHITE ? "WHITE" :
                                                      "ERROR_PIECE");
    return os;
}

inline std::ostream &operator<<(std::ostream &os, const Direction d) {
    std::cout << (d == D_RANK ? "RANK" : d == D_FILE ? "FILE" :
                                     d == D_MDIAG    ? "MDIAG" :
                                     d == D_ADIAG    ? "ADIAG" :
                                                       "ERROR_DIRECTION");
    return os;
}
