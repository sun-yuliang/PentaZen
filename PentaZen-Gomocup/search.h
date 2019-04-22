/*
 *  PentaZen Gomoku/Renju Engine
 *                    _______
 *                   / _____/
 *  developed by    / /______  ________
 *                 /____  / / / / __  /
 *                _____/ / /_/ / / / /
 *  2019.04      /______/_____/_/ /_/
 *
 * search.h - search functions
 */

#ifndef __SEARCH_H__
#define __SEARCH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "public.h"
#include "board.h"

// score constant structure
typedef struct {
	long dead4;
	long free3a;
	long free3;
	long dead3;
	long free2a;
	long free2;
	long dead2;
	long free1a;
	long free1;
	long dead1;
} score_t;

// search constant structure
typedef struct {
	score_t sc;		// score constants
	u16 me;			// my color
	u16 hpleaf;		// candidate number of cand_root function
	u16 htleaf;		// candidate number of cand function
	u16 mindep;		// iterative deepening minimum depth
	u16 maxdep;		// iterative deepening maximum depth
	u16 vcfdep;		// VCF search depth
	u16 vctdep;		// VCT search depth
} search_t;

void cand(board_t* bd, const search_t* srh, const u16 who);

void cand_root(board_t* bd, const search_t* srh, const u16 dep);

long alphabeta(board_t* bd, const search_t* srh, const u16 dep, const u16 next, u16* best,
							long alpha, long beta, const bool isroot, const double tlimit);

u16 get_best(board_t* bd, search_t* srh);

#ifdef __cplusplus
}
#endif

#endif