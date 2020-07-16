/*      _____                __    ______
 *     / ___ \              / /   /___  /
 *    / /__/ /___  ____  __/ /_______/ /    ____  ____
 *   / _____/ __ \/ __ \/_   _/ __  / /    / __ \/ __ \
 *  / /    /  ___/ / / / / /_/ /_/ / /____/  ___/ / / /
 * /_/     \____/_/ /_/ /___/\__,_/______/\____/_/ /_/
 *
 * PentaZen, a Gomoku/Renju playing engine developed by Sun.
 */

#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#ifdef __cplusplus
extern "C" {
#endif

#define NDEBUG

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Output control.
#define TABLE_GENERATE_INFO         0
#define TEST_COMMAND                0
#define ITERATIVE_INFO              0
#define SEARCH_INFO                 1
#define MESSAGE_STAT                0
#define RANDOM_PCT                  0.0

// Search setting.
#define ITERATIVE_DEEPENING         1
#define SEARCH_DEPTH_MAX            256
#define VICTORY_REPEAT              2
#define FUTILITY_DEPTH_MAX          6

// Pruning switch.
#define EXTENDED_FUTILITY           1
#define BRANCH_NUMBER_LIMIT         1
#define SCORE_DIFFERENCE_CUT        1

// Time management.
#define ITERATIVE_STOP_PCT          0.4
#define TIME_THRE1                  0.6
#define TIME_THRE2                  0.9
#define TIME_DIV1                   14.0
#define TIME_DIV2                   53.0
#define TIME_RSV                    50

// Board display control.
#define DISPLAY_BOARD               1
#define DISPLAY_ATTACK              0
#define DISPLAY_DEFEND              0
#define DISPLAY_PATTERN             1
#define DISPLAY_SCORE               1
#define DISPLAY_COLOR               1
#define DISPLAY_CAND                0
#define DISPLAY_MLIST               0
#define DISPLAY_ZOBRIST             0

// Constants.
#define BD_SIDE                     15
#define BD_SIZE                     (BD_SIDE * BD_SIDE)
#define BD_MID                      112
#define STR_LEN                     128
#define CMD_MAX                     3

#define EMPTY                       0
#define BLACK                       1
#define WHITE                       2
#define DRAW                        3
#define COL_INVALID                 255
#define POS_INVALID                 65535

#define FREESTYLE                   0
#define STANDARD                    1
#define RENJU                       2

#define VAL_V                       32000
#define VAL_TIMEOUT                 32001
#define VAL_INVALID                 -32002
#define VAL_VTHRE                   (VAL_V / 2)
#define VAL_DRAW                    0

#define ADD                         0
#define SUB                         1

// Patterns.
#define PAT_NUM                     7
#define C6                          0
#define C5                          1
#define F4                          2
#define B4                          3
#define F3                          4
#define B3                          5
#define F2                          6
#define B2                          7
#define F1                          8

#define B4a                         0
#define F3d                         1
#define B4d                         2
#define X                           65535

// 2^0 - 2^24.
#define IIE0                        1
#define IIE1                        2
#define IIE2                        4
#define IIE3                        8
#define IIE4                        16
#define IIE5                        32
#define IIE6                        64
#define IIE7                        128
#define IIE8                        256
#define IIE9                        512
#define IIE10                       1024
#define IIE11                       2048
#define IIE12                       4096
#define IIE13                       8192
#define IIE14                       16384
#define IIE15                       32768
#define IIE16                       65536
#define IIE17                       131072
#define IIE18                       262144
#define IIE19                       524288
#define IIE20                       1048576

// Typedefs
typedef uint8_t                     u8;
typedef uint16_t                    u16;
typedef uint32_t                    u32;
typedef uint64_t                    u64;
typedef int8_t                      i8;
typedef int16_t                     i16;
typedef int32_t                     i32;
typedef int64_t                     i64;

typedef u8                          ADType;
typedef u8                          Color;
typedef u8                          Metric;
typedef u8                          PatType;
typedef u16                         Pos;
typedef u8                          Rule;
typedef i16                         Score;
typedef u32                         Time;
typedef u64                         TTKey;

// My max and min macro.
#define MyMax(x, y)                   ((x) > (y) ? (x) : (y))
#define MyMin(x, y)                   ((x) < (y) ? (x) : (y))

// Get current time in ms (vs).
static inline Time get_ms()
{
    return (Time)clock();
}

// Return opponent's color. c should be BLACK(1) or WHITE(2) only.
static inline Color opp(const Color c)
{
    return c ^ 3;
}

// Return true if c is a valid color.
static inline bool iscolor(const Color c)
{
    return c == BLACK || c == WHITE;
}

// Return true if pos is a valid position.
static inline bool ispos(const Pos pos)
{
    return pos < BD_SIZE;
}

// Return true if ru is a rule type.
static inline bool isrule(const Rule ru)
{
    return ru == FREESTYLE || ru == STANDARD || ru == RENJU;
}

#ifdef __cplusplus
}
#endif

#endif
