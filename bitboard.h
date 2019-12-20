/*      _____                __    ______
 *     / ___ \              / /   /___  /
 *    / /__/ /___  ____  __/ /_______/ /    ____  ____
 *   / _____/ __ \/ __ \/_   _/ __  / /    / __ \/ __ \
 *  / /    /  ___/ / / / / /_/ /_/ / /____/  ___/ / / /
 * /_/     \____/_/ /_/ /___/\__,_/______/\____/_/ /_/
 *
 * PentaZen, a Gomoku/Renju playing engine developed by Sun.
 *
 * Bitboard - several u64 variables storing n positions.
 * Reset            O(BB_SIZE)
 * Find             O(1)
 * Insert           O(1)
 * Remove           O(1)
 * Remove all       O(BB_SIZE)
 * Traverse         N/A
 * Memory           O(BB_SIZE)
 */

#ifndef __BITBOARD_H__
#define __BITBOARD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "public.h"

#define BB_SIZE     (BD_SIZE / 64 + 1)

typedef struct {
    u64 bitboard[BB_SIZE];
} BitBoard;

static inline void bb_reset(BitBoard* bb)
{
    memset(bb->bitboard, 0, sizeof(bb->bitboard));
}

static inline bool bb_find(BitBoard* bb, const Pos pos)
{
    return bb->bitboard[pos >> 6] & ((u64)1 << (pos & 0x3f));
}

static inline void bb_insert(BitBoard* bb, const Pos pos)
{
    bb->bitboard[pos >> 6] |= ((u64)1 << (pos & 0x3f));
}

static inline void bb_remove(BitBoard* bb, const Pos pos)
{
    bb->bitboard[pos >> 6] &= ~((u64)1 << (pos & 0x3f));
}

static inline void bb_copy(BitBoard* des, const BitBoard* src)
{
    memcpy(des->bitboard, src->bitboard, sizeof(src->bitboard));
}

#ifdef __cplusplus
}
#endif

#endif
