/*
 *  PentaZen Gomoku/Renju Engine
 *                    _______
 *                   / _____/
 *  developed by    / /______  ________
 *                 /____  / / / / __  /
 *                _____/ / /_/ / / / /
 *  2019.04      /______/_____/_/ /_/
 *
 * mvlist.h - mvlist_t data structure
 *
 * Linear List storing positions ranging from 0 to 399.
 * Similar to the doubly-linked list with no repetition.
 * The time complexity of each operation is O(1) if not using memset or memcpy.
 */

#ifndef __MVLIST_H__
#define __MVLIST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "public.h"

#define HEAD	20 * 20
#define END		20 * 20 + 1

// node_t structure
typedef struct {
	bool valid;		// existence of the node
	u16 next;		// next node index
	u16 prev;		// previous node index
} node_t;

// mvlist_t structure
typedef struct {
	node_t arr[20 * 20 + 2];	// node_t array
	u16 size;					// # of nodes
} mvlist_t;

// Reset a mvlist. Use memset.
static void mvlist_reset(mvlist_t* mv);

// Return the size of the mvlist.
static u16 mvlist_size(const mvlist_t* mv);

// Return true if pos exists in the mvlist.
static bool mvlist_find(const mvlist_t* mv, const u16 pos);

// Return the first position.
static u16 mvlist_first(const mvlist_t* mv);

// Return the last position.
static u16 mvlist_last(const mvlist_t* mv);

// Return the next position of pos.
static u16 mvlist_next(const mvlist_t* mv, const u16 pos);

// Return the previous position of pos.
static u16 mvlist_prev(const mvlist_t* mv, const u16 pos);

// Insert pos at the front of the mvlist. Return true if inserted.
static bool mvlist_insert_front(mvlist_t* mv, const u16 pos);

// Insert pos at the back of the mvlist. Return true if inserted.
static bool mvlist_insert_back(mvlist_t* mv, const u16 pos);

// Remove and return the first position. Return INVALID if mvlist is empty.
static u16 mvlist_remove_front(mvlist_t* mv);

// Remove and return the last position. Return INVALID if mvlist is empty.
static u16 mvlist_remove_back(mvlist_t* mv);

// Remove pos from the mvlist. Return false if no such position.
static bool mvlist_remove(mvlist_t* mv, const u16 pos);

// Copy src to des. Use memcpy.
static void mvlist_copy(mvlist_t* des, const mvlist_t* src);

// Implementation
static inline void mvlist_reset(mvlist_t* mv)
{
	memset(mv, 0, sizeof(mvlist_t));
	mv->arr[HEAD].prev = INVALID;
	mv->arr[HEAD].next = END;
	mv->arr[END].prev = HEAD;
	mv->arr[END].next = INVALID;
}

static inline u16 mvlist_size(const mvlist_t* mv)
{
	return mv->size;
}

static inline bool mvlist_find(const mvlist_t* mv, const u16 pos)
{
	return mv->arr[pos].valid;
}

static inline u16 mvlist_first(const mvlist_t* mv)
{
	return mv->arr[HEAD].next;
}

static inline u16 mvlist_last(const mvlist_t* mv)
{
	return mv->arr[END].prev;
}

static inline u16 mvlist_next(const mvlist_t* mv, const u16 pos)
{
	return mv->arr[pos].next;
}

static inline u16 mvlist_prev(const mvlist_t* mv, const u16 pos)
{
	return mv->arr[pos].prev;
}

static inline bool mvlist_insert_front(mvlist_t* mv, const u16 pos)
{
	if (mv->arr[pos].valid)
		return false;

	mv->arr[pos].prev = HEAD;
	mv->arr[pos].next = mv->arr[HEAD].next;
	mv->arr[HEAD].next = pos;
	mv->arr[mv->arr[pos].next].prev = pos;

	mv->arr[pos].valid = true;
	mv->size++;

	return true;
}

static inline bool mvlist_insert_back(mvlist_t* mv, const u16 pos)
{
	if (mv->arr[pos].valid)
		return false;

	mv->arr[pos].prev = mv->arr[END].prev;
	mv->arr[pos].next = END;
	mv->arr[END].prev = pos;
	mv->arr[mv->arr[pos].prev].next = pos;

	mv->arr[pos].valid = true;
	mv->size++;

	return true;
}

static inline bool mvlist_remove(mvlist_t* mv, const u16 pos)
{
	if (!mv->arr[pos].valid)
		return false;

	mv->arr[mv->arr[pos].prev].next = mv->arr[pos].next;
	mv->arr[mv->arr[pos].next].prev = mv->arr[pos].prev;

	mv->arr[pos].valid = false;
	mv->size--;

	return true;
}

static inline u16 mvlist_remove_front(mvlist_t* mv)
{
	u16 pos = mv->arr[HEAD].next;
	if (pos != END)
	{
		mvlist_remove(mv, pos);
		return pos;
	}
	return INVALID;
}

static inline u16 mvlist_remove_back(mvlist_t* mv)
{
	u16 pos = mv->arr[END].prev;
	if (pos != HEAD)
	{
		mvlist_remove(mv, pos);
		return pos;
	}
	return INVALID;
}

static inline void mvlist_copy(mvlist_t* des, const mvlist_t* src)
{
	memcpy(des, src, sizeof(mvlist_t));
}

#ifdef __cplusplus
}
#endif

#endif
