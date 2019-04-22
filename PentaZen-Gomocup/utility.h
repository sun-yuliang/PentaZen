/*
 *  PentaZen Gomoku/Renju Engine
 *                    _______
 *                   / _____/
 *  developed by    / /______  ________
 *                 /____  / / / / __  /
 *                _____/ / /_/ / / / /
 *  2019.04      /______/_____/_/ /_/
 *
 * utility.h - miscellaneous functions
 */

#ifndef __UTILITY_H__
#define __UTILITY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "mvlist.h"
#include "board.h"

// Return min{a, b}.
static inline int min_value(const u16 a, const u16 b)
{
	return (a < b ? a : b);
}

// Return max{a, b}.
static inline int max_value(const u16 a, const u16 b)
{
	return (a > b ? a : b);
}

// Return min{a, b}.
static inline double min_value_double(const double a, const double b)
{
	return (a < b ? a : b);
}

// Return max{a, b}.
static inline double max_value_double(const double a, const double b)
{
	return (a > b ? a : b);
}

// Return the opposite color.
static inline u16 opp(const u16 color)
{
	return color == BLACK ? WHITE : BLACK;
}

// Change the sign according to node type.
static inline long sign(const long score, const u16 me, const u16 next)
{
	return next == me ? score : -score;
}

void remove_line_break(char* str);

bool substring(const char* in, char* out, const int N);

int str_to_int(const char* str);

u32 str_to_u32(const char* str);

void print_mvlist(const mvlist_t* mv, const u16 side);

void print_board(const board_t* bd);

void print_zobrist();

#ifdef __cplusplus
}
#endif

#endif
