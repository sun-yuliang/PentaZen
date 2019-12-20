/*      _____                __    ______
 *     / ___ \              / /   /___  /
 *    / /__/ /___  ____  __/ /_______/ /    ____  ____
 *   / _____/ __ \/ __ \/_   _/ __  / /    / __ \/ __ \
 *  / /    /  ___/ / / / / /_/ /_/ / /____/  ___/ / / /
 * /_/     \____/_/ /_/ /___/\__,_/______/\____/_/ /_/
 *
 * PentaZen, a Gomoku/Renju playing engine developed by Sun.
 */

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "public.h"
#include "mvlist.h"
#include "pattern.h"
#include "board.h"

void print_binary(const u32 num, const u8 N);

void print_mvlist(const Mvlist* ml);

void print_pattern(const Pattern* pat);

void print_board(Board* bd);

void print_message(const Board* bd, const u16 dep, const u16 dep_max, const Score ev, const u32 n, const Time tm);

#ifdef __cplusplus
}
#endif

#endif
