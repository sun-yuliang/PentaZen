/*
 *  PentaZen Gomoku/Renju Engine
 *                    _______
 *                   / _____/
 *  developed by    / /______  ________
 *                 /____  / / / / __  /
 *                _____/ / /_/ / / / /
 *  2019.03      /______/_____/_/ /_/
 *
 * open.h - 26 gomoku openings
 */

#ifndef __OPEN_H__
#define __OPEN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "public.h"
#include "board.h"

#define OPEN_NUM	26

// han xing
static inline void d1(board_t* bd)
{
	board_reset(bd, FREESTYLE, 15, BLACK);
	do_move(bd, 112, BLACK);
	do_move(bd, 97, WHITE);
	do_move(bd, 82, BLACK);
}

// xi yue
static inline void d2(board_t* bd)
{
	board_reset(bd, FREESTYLE, 15, BLACK);
	do_move(bd, 112, BLACK);
	do_move(bd, 97, WHITE);
	do_move(bd, 83, BLACK);
}

// shu xing
static inline void d3(board_t* bd)
{
	board_reset(bd, FREESTYLE, 15, BLACK);
	do_move(bd, 112, BLACK);
	do_move(bd, 97, WHITE);
	do_move(bd, 84, BLACK);
}

// hua yue
static inline void d4(board_t* bd)
{
	board_reset(bd, FREESTYLE, 15, BLACK);
	do_move(bd, 112, BLACK);
	do_move(bd, 97, WHITE);
	do_move(bd, 98, BLACK);
}

// can yue
static inline void d5(board_t* bd)
{
	board_reset(bd, FREESTYLE, 15, BLACK);
	do_move(bd, 112, BLACK);
	do_move(bd, 97, WHITE);
	do_move(bd, 99, BLACK);
}

// yu yue
static inline void d6(board_t* bd)
{
	board_reset(bd, FREESTYLE, 15, BLACK);
	do_move(bd, 112, BLACK);
	do_move(bd, 97, WHITE);
	do_move(bd, 113, BLACK);
}

// jin xing
static inline void d7(board_t* bd)
{
	board_reset(bd, FREESTYLE, 15, BLACK);
	do_move(bd, 112, BLACK);
	do_move(bd, 97, WHITE);
	do_move(bd, 114, BLACK);
}

// song yue
static inline void d8(board_t* bd)
{
	board_reset(bd, FREESTYLE, 15, BLACK);
	do_move(bd, 112, BLACK);
	do_move(bd, 97, WHITE);
	do_move(bd, 127, BLACK);
}

// qiu yue
static inline void d9(board_t* bd)
{
	board_reset(bd, FREESTYLE, 15, BLACK);
	do_move(bd, 112, BLACK);
	do_move(bd, 97, WHITE);
	do_move(bd, 128, BLACK);
}

// xin yue
static inline void d10(board_t* bd)
{
	board_reset(bd, FREESTYLE, 15, BLACK);
	do_move(bd, 112, BLACK);
	do_move(bd, 97, WHITE);
	do_move(bd, 129, BLACK);
}

// rui xing
static inline void d11(board_t* bd)
{
	board_reset(bd, FREESTYLE, 15, BLACK);
	do_move(bd, 112, BLACK);
	do_move(bd, 97, WHITE);
	do_move(bd, 142, BLACK);
}

// shan yue
static inline void d12(board_t* bd)
{
	board_reset(bd, FREESTYLE, 15, BLACK);
	do_move(bd, 112, BLACK);
	do_move(bd, 97, WHITE);
	do_move(bd, 143, BLACK);
}

// you xing
static inline void d13(board_t* bd)
{
	board_reset(bd, FREESTYLE, 15, BLACK);
	do_move(bd, 112, BLACK);
	do_move(bd, 97, WHITE);
	do_move(bd, 144, BLACK);
}

// chang xing
static inline void id1(board_t* bd)
{
	board_reset(bd, FREESTYLE, 15, BLACK);
	do_move(bd, 112, BLACK);
	do_move(bd, 98, WHITE);
	do_move(bd, 84, BLACK);
}

// xia yue
static inline void id2(board_t* bd)
{
	board_reset(bd, FREESTYLE, 15, BLACK);
	do_move(bd, 112, BLACK);
	do_move(bd, 98, WHITE);
	do_move(bd, 99, BLACK);
}

// heng xing
static inline void id3(board_t* bd)
{
	board_reset(bd, FREESTYLE, 15, BLACK);
	do_move(bd, 112, BLACK);
	do_move(bd, 98, WHITE);
	do_move(bd, 114, BLACK);
}

// shui yue
static inline void id4(board_t* bd)
{
	board_reset(bd, FREESTYLE, 15, BLACK);
	do_move(bd, 112, BLACK);
	do_move(bd, 98, WHITE);
	do_move(bd, 129, BLACK);
}

// liu xing
static inline void id5(board_t* bd)
{
	board_reset(bd, FREESTYLE, 15, BLACK);
	do_move(bd, 112, BLACK);
	do_move(bd, 98, WHITE);
	do_move(bd, 144, BLACK);
}

// yun yue
static inline void id6(board_t* bd)
{
	board_reset(bd, FREESTYLE, 15, BLACK);
	do_move(bd, 112, BLACK);
	do_move(bd, 98, WHITE);
	do_move(bd, 113, BLACK);
}

// pu yue
static inline void id7(board_t* bd)
{
	board_reset(bd, FREESTYLE, 15, BLACK);
	do_move(bd, 112, BLACK);
	do_move(bd, 98, WHITE);
	do_move(bd, 128, BLACK);
}

// lan yue
static inline void id8(board_t* bd)
{
	board_reset(bd, FREESTYLE, 15, BLACK);
	do_move(bd, 112, BLACK);
	do_move(bd, 98, WHITE);
	do_move(bd, 143, BLACK);
}

// yin yue
static inline void id9(board_t* bd)
{
	board_reset(bd, FREESTYLE, 15, BLACK);
	do_move(bd, 112, BLACK);
	do_move(bd, 98, WHITE);
	do_move(bd, 127, BLACK);
}

// ming xing
static inline void id10(board_t* bd)
{
	board_reset(bd, FREESTYLE, 15, BLACK);
	do_move(bd, 112, BLACK);
	do_move(bd, 98, WHITE);
	do_move(bd, 142, BLACK);
}

// xie yue
static inline void id11(board_t* bd)
{
	board_reset(bd, FREESTYLE, 15, BLACK);
	do_move(bd, 112, BLACK);
	do_move(bd, 98, WHITE);
	do_move(bd, 126, BLACK);
}

// ming yue
static inline void id12(board_t* bd)
{
	board_reset(bd, FREESTYLE, 15, BLACK);
	do_move(bd, 112, BLACK);
	do_move(bd, 98, WHITE);
	do_move(bd, 141, BLACK);
}

// hui xing
static inline void id13(board_t* bd)
{
	board_reset(bd, FREESTYLE, 15, BLACK);
	do_move(bd, 112, BLACK);
	do_move(bd, 98, WHITE);
	do_move(bd, 140, BLACK);
}

static void(*make_opening[OPEN_NUM])(board_t* bd) = {
	d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11, d12, d13,
	id1, id2, id3, id4, id5, id6, id7, id8, id9, id10, id11, id12, id13,
};

#ifdef __cplusplus
}
#endif

#endif
