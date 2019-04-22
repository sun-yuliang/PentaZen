/*
 *  PentaZen Gomoku/Renju Engine
 *                    _______
 *                   / _____/
 *  developed by    / /______  ________
 *                 /____  / / / / __  /
 *                _____/ / /_/ / / / /
 *  2019.04      /______/_____/_/ /_/
 *
 * pair.h - position-key pair array data structure
 */

#ifndef __PAIR_H__
#define __PAIR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "public.h"
#include "mvlist.h"

// pos-key structure
typedef struct {
	u16 pos;
	long key;
} pr_t;

// pair_t structure
typedef struct {
	pr_t arr[20 * 20];
	u16 size;
} pair_t;

void pair_reset(pair_t* pair);

void pair_insert(pair_t* pair, const u16 pos, const long key);

void pair_sort(pair_t* pair);

void pair_to_mvlist(mvlist_t* mv, const pair_t* pair);

#ifdef __cplusplus
}
#endif

#endif
