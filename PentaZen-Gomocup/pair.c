/*
 *  PentaZen Gomoku/Renju Engine
 *                    _______
 *                   / _____/
 *  developed by    / /______  ________
 *                 /____  / / / / __  /
 *                _____/ / /_/ / / / /
 *  2019.04      /______/_____/_/ /_/
 *
 * pair.c - pair_t data structure
 */

#include "public.h" 
#include "pair.h"
#include "mvlist.h"

// return true if a < b
static inline bool less(const pr_t* a, const pr_t* b)
{
	if (a->key < b->key)
		return true;
	else
		return false;
}

// Reset the pair array.
void pair_reset(pair_t* pair)
{
	pair->size = 0;
}

// Insert a pair.
void pair_insert(pair_t* pair, const u16 pos, const long key)
{
	pair->arr[pair->size].pos = pos;
	pair->arr[pair->size].key = key;
	pair->size++;
}

// Sort the first N elements of the pair array in place.
void pair_sort(pair_t* pair)
{
	int i, j;
	pr_t tmp;

	for (i = 1; i < pair->size; i++)
	{
		for (j = i - 1; j >= 0; j--)
		{
			if (less(&pair->arr[j], &pair->arr[j + 1]))
			{
				tmp.pos = pair->arr[j].pos;
				tmp.key = pair->arr[j].key;
				pair->arr[j].pos = pair->arr[j + 1].pos;
				pair->arr[j].key = pair->arr[j + 1].key;
				pair->arr[j + 1].pos = tmp.pos;
				pair->arr[j + 1].key = tmp.key;
			}
			else
				break;
		}
	}
}

// Generate mvlist from the pair array.
void pair_to_mvlist(mvlist_t* mv, const pair_t* pair)
{
	int i;
	mvlist_reset(mv);
	for (i = 0; i < pair->size; i++)
		mvlist_insert_back(mv, pair->arr[i].pos);
}
