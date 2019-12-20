/*      _____                __    ______
 *     / ___ \              / /   /___  /
 *    / /__/ /___  ____  __/ /_______/ /    ____  ____
 *   / _____/ __ \/ __ \/_   _/ __  / /    / __ \/ __ \
 *  / /    /  ___/ / / / / /_/ /_/ / /____/  ___/ / / /
 * /_/     \____/_/ /_/ /___/\__,_/______/\____/_/ /_/
 *
 * PentaZen, a Gomoku/Renju playing engine developed by Sun.
 */

#ifndef __MVLIST_H__
#define __MVLIST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "public.h"
#include "bitboard.h"

typedef struct {
    Pos pos;
    Score val;
} Move;

typedef struct {
    Move list[BD_SIZE];
    BitBoard bb[BB_SIZE];
    u16 size;
    u16 cut;
} Mvlist;

void mvlist_reset(Mvlist* ml);

bool mvlist_find(Mvlist* ml, const Pos pos);

void mvlist_insert(Mvlist* ml, const Pos pos, const Score val);

void mvlist_remove(Mvlist* ml, const Pos pos);

void mvlist_remove_all(Mvlist* ml);

void mvlist_copy(Mvlist* des, const Mvlist* src);

void mvlist_swap(Mvlist* ml, const u16 ind1, const u16 ind2);

void mvlist_sort(Mvlist* ml);

#ifdef __cplusplus
}
#endif

#endif
