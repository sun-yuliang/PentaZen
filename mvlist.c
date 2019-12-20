/*      _____                __    ______
 *     / ___ \              / /   /___  /
 *    / /__/ /___  ____  __/ /_______/ /    ____  ____
 *   / _____/ __ \/ __ \/_   _/ __  / /    / __ \/ __ \
 *  / /    /  ___/ / / / / /_/ /_/ / /____/  ___/ / / /
 * /_/     \____/_/ /_/ /___/\__,_/______/\____/_/ /_/
 *
 * PentaZen, a Gomoku/Renju playing engine developed by Sun.
 *
 * Move - position-value pair.
 *
 * Mvlist - Array storing n Moves.
 * Reset            O(BD_SIZE)
 * Find             O(1)
 * Insert           O(1)
 * Remove           O(n)
 * Remove all       O(BB_SIZE)
 * Traversal        O(n)
 * Memory           O(BD_SIZE)
 */

#include "mvlist.h"

static inline bool less(const Score sc1, const Score sc2)
{
    return sc1 < sc2 ? true : false;
}

void mvlist_reset(Mvlist* ml)
{
    memset(ml->list, 0, sizeof(ml->list));
    bb_reset(ml->bb);
    ml->size = 0;
    ml->cut = 0;
}

bool mvlist_find(Mvlist* ml, const Pos pos)
{
    return bb_find(ml->bb, pos);
}

void mvlist_insert(Mvlist* ml, const Pos pos, const Score val)
{
    if (!mvlist_find(ml, pos))
    {
        ml->list[ml->size].pos = pos;
        ml->list[ml->size].val = val;
        bb_insert(ml->bb, pos);
        ml->size++;
        ml->cut++;
    }
}

void mvlist_remove(Mvlist* ml, const Pos pos)
{
    if (mvlist_find(ml, pos))
    {
        for (u16 i = 0; i < ml->size; i++)
        {
            if (ml->list[i].pos == pos)
            {
                ml->list[i].pos = ml->list[ml->size - 1].pos;
                ml->list[i].val = ml->list[ml->size - 1].val;
                ml->size--;
                ml->cut--;
                break;
            }
        }
        bb_remove(ml->bb, pos);
    }
}

void mvlist_remove_all(Mvlist* ml)
{
    bb_reset(ml->bb);
    ml->size = 0;
    ml->cut = 0;
}

void mvlist_copy(Mvlist* des, const Mvlist* src)
{
    memcpy(des->list, src->list, sizeof(src->list));
    bb_copy(des->bb, src->bb);
    des->size = src->size;
    des->cut = src->size;
}

void mvlist_swap(Mvlist* ml, const u16 ind1, const u16 ind2)
{
    Pos ptmp = ml->list[ind1].pos;
    Score vtmp = ml->list[ind1].val;
    ml->list[ind1].pos = ml->list[ind2].pos;
    ml->list[ind1].val = ml->list[ind2].val;
    ml->list[ind2].pos = ptmp;
    ml->list[ind2].val = vtmp;
}

void mvlist_sort(Mvlist* ml)
{
    for (i16 i = 1; i < ml->cut; i++)
    {
        for (i16 j = i - 1; j >= 0; j--)
        {
            if (less(ml->list[j].val, ml->list[j + 1].val))
                mvlist_swap(ml, j, j + 1);
            else
                break;
        }
    }
}
