/*
 *  PentaZen Gomoku/Renju Engine
 *                    _______
 *                   / _____/
 *  developed by    / /______  ________
 *                 /____  / / / / __  /
 *                _____/ / /_/ / / / /
 *  2019.04      /______/_____/_/ /_/
 *
 * hash.h - hash table functions
 */

#ifndef __HASH_H__
#define __HASH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "public.h"
#include "array.h"

#define HASH_SIZE			8388608
#define HASH_MASK			HASH_SIZE - 1
#define HASH_EMPTY			0
#define HASH_EXACT			1
#define HASH_ALPHA			2
#define HASH_BETA			3
#define HASH_SCORE_INV		-99999999

// hash table entry structure
typedef struct {
	u64 key;		// zobrist key
	long score;		// board score
	u8 dep;			// disc number indicating search depth
	u8 type;		// score type (HASH_EMPTY, HASH_EXACT, HASH_ALPHA, HASH_BETA)
	u16 best;		// next best move position
} hash_t;

void hash_reset();

void hash_record(const u64 key, const long score, const u16 dep, const u16 type, const u16 best);

long hash_query_alphabeta(const u64 key, const u16 dep, const long alpha, const long beta);

u16 hash_query_best(const u64 key);

#ifdef __cplusplus
}
#endif

#endif
