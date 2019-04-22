/*
 *  PentaZen Gomoku/Renju Engine
 *                    _______
 *                   / _____/
 *  developed by    / /______  ________
 *                 /____  / / / / __  /
 *                _____/ / /_/ / / / /
 *  2019.04      /______/_____/_/ /_/
 *
 * board.h - board_t data structure and functions
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "pattern.h"
#include "mvlist.h"
#include "pair.h"

#define pinc(bd)	&bd->pinc
#define mstk(bd)	&bd->mstk
#define vlast(bd)	&bd->vlast
#define pair(bd)	&bd->pair
#define pat(bd)		&bd->pat[bd->num]
#define mlist(bd)	&bd->mlist[bd->num]
#define hlist(bd)	&bd->hlist[bd->num]
#define spe(bd)		bd->spe[bd->num]

// seg_t structure
typedef struct {
	u16 start;
	u16 end;
} seg_t;

// board_t structure
typedef struct
{
	// constants
	u16 rule;						// rule
	u16 side;						// board side length
	u16 fcolor;						// first disc color
	u16 next;						// next move color
	u16 num;						// # of discs
	u64 zkey;						// zobrist key
	bool timeout;					// timeout flag

	// structures
	u16 arr[20 * 20];				// disc array	
	pattern_t pinc;					// increment pattern
	seg_t seg[20 * 20][4][2];		// interval array
	mvlist_t mstk;					// disc move stack
	mvlist_t vlast[2];				// last dead4/free3 point
	pair_t pair;					// position-score pair
	
	// array stacks
	pattern_t pat[20 * 20];			// pattern stack
	mvlist_t mlist[20 * 20];		// mvlist stack
	mvlist_t hlist[20 * 20];		// heuristic mvlist stack
	u64 spe[20 * 20][20 * 20];		// special types stack

	// time information in seconds
	double timeout_match;
	double timeout_turn;
	double time_left;
} board_t;

// check masks
static u64 check_mask[2][8] = {
	{ 0x0000000f00000000, 0x000000f000000000, 0x0000000000000f00, 0x000000000000f000,
	  0x000f000000000000, 0x00f0000000000000, 0x000000000f000000, 0x00000000f0000000 },
	{ 0x000000000000000f, 0x00000000000000f0, 0x00000f0000000000, 0x0000f00000000000,
	  0x00000000000f0000, 0x0000000000f00000, 0x0f00000000000000, 0xf000000000000000 }
};

// count shift number
static u16 count_shift[2][8] = {
	{ 32, 36, 8, 12, 48, 52, 24, 28 },
	{ 0, 4, 40, 44, 16, 20, 56, 60 }
};

// popcnt for 4-bit numbers
static u16 popcnt4[16] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 };

// Check special types in pos.
// e.g. bool flag = check(bd, pos, BLACK, FD4);
static inline bool check(const board_t* bd, const u16 pos, const u16 color, const u16 type)
{
	return spe(bd)[pos] & check_mask[color - 1][type];
}

// Count special types in pos.
// e.g. u16 cnt = count(bd, pos, BLACK, FD4);
static inline u16 count(const board_t* bd, const u16 pos, const u16 color, const u16 type)
{
	return popcnt4[(spe(bd)[pos] & check_mask[color - 1][type]) >> count_shift[color - 1][type]];
}

void board_reset(board_t* bd, const u16 rule, const u16 side, const u16 fcolor);

void do_move(board_t* bd, const u16 pos, const u16 color);

void do_move_fast(board_t* bd, const u16 pos, const u16 color);

void undo(board_t* bd);

void undo_fast(board_t* bd);

u16 gameover(const board_t* bd);

u16 winlose(const board_t* bd, const u16 next, u16* offset);

bool cand_winlose(board_t* bd, const u16 next);

bool cand_force(board_t* bd, const u16 next);

long vcf(board_t* bd, const u16 dep, const u16 attacker, const u16 next, u16* best, long alpha, long beta, const bool isroot);

long vct(board_t* bd, const u16 dep, const u16 attacker, const u16 next, u16* best, long alpha, long beta, const bool isroot);

double get_time(const board_t* bd);

#ifdef __cplusplus
}
#endif

#endif
