/*
 *  PentaZen Gomoku/Renju Engine
 *                    _______
 *                   / _____/
 *  developed by    / /______  ________
 *                 /____  / / / / __  /
 *                _____/ / /_/ / / / /
 *  2019.04      /______/_____/_/ /_/
 *
 * hash.c - hash table functions
 */

#include "public.h" 
#include "hash.h"

// Hash table.
static hash_t HashTable[HASH_SIZE];

// Reset the whole hash table.
void hash_reset()
{
	memset(HashTable, 0, sizeof(HashTable));
}

// Save an entry in the hash table.
// Replace when search depth is greater or equal to the existing one.
void hash_record(const u64 key, const long score, const u16 dep, const u16 type, const u16 best)
{
	hash_t* entry = &HashTable[key & HASH_MASK];

	if (dep >= entry->dep)
	{
		entry->key = key;
		entry->score = score;
		entry->dep = (u8)dep;
		entry->type = (u8)type;
		entry->best = best;
	}
}

// Query the hash table for alphabeta search score.
long hash_query_alphabeta(const u64 key, const u16 dep, const long alpha, const long beta)
{
	hash_t* entry = &HashTable[key & HASH_MASK];

	if (entry->key == key)
	{
		if ((entry->score < LOSE_THRE || entry->score > WIN_THRE) && entry->type == HASH_EXACT)
			return entry->score;

		if (entry->dep >= dep)
		{
			if (entry->type == HASH_EXACT)
				return entry->score;
			else if (entry->type == HASH_ALPHA && entry->score <= alpha)
				return alpha;
			else if (entry->type == HASH_BETA && entry->score >= beta)
				return beta;
		}
	}

	return HASH_SCORE_INV;
}

// Query the hash table for best.
u16 hash_query_best(const u64 key)
{
	hash_t* entry = &HashTable[key & HASH_MASK];

	if (entry->key == key)
	{
		return entry->best;
	}

	return INVALID;
}
