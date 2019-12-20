/*      _____                __    ______
 *     / ___ \              / /   /___  /
 *    / /__/ /___  ____  __/ /_______/ /    ____  ____
 *   / _____/ __ \/ __ \/_   _/ __  / /    / __ \/ __ \
 *  / /    /  ___/ / / / / /_/ /_/ / /____/  ___/ / / /
 * /_/     \____/_/ /_/ /___/\__,_/______/\____/_/ /_/
 *
 * PentaZen, a Gomoku/Renju playing engine developed by Sun.
 */

#ifndef __PATTERN_H__
#define __PATTERN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "public.h"

typedef struct {
    i8 pattern[2][PAT_NUM];
} Pattern;

static inline void pat_reset(Pattern* pat)
{
    memset(pat->pattern, 0, sizeof(pat->pattern));
}

static inline i8 pat_read(const Pattern* pat, const Color c, const PatType t)
{
    assert(iscolor(c));
    assert(t < PAT_NUM);

    return pat->pattern[c - 1][t];
}

static inline void pat_inc(Pattern* pat, const Color c, const PatType t)
{
    assert(iscolor(c));
    assert(t < PAT_NUM);

    pat->pattern[c - 1][t]++;
}

static inline void pat_dec(Pattern* pat, const Color c, const PatType t)
{
    assert(iscolor(c));
    assert(t < PAT_NUM);

    pat->pattern[c - 1][t]--;
}

static inline void pat_add(Pattern* out, const Pattern* in1, const Pattern* in2)
{
    for (u8 i = 0; i < 2; i++)
        for (u8 j = 0; j < PAT_NUM; j++)
            out->pattern[i][j] = in1->pattern[i][j] + in2->pattern[i][j];
}

static inline void pat_sub(Pattern* out, const Pattern* in1, const Pattern* in2)
{
    for (u8 i = 0; i < 2; i++)
        for (u8 j = 0; j < PAT_NUM; j++)
            out->pattern[i][j] = in1->pattern[i][j] - in2->pattern[i][j];
}

static inline void pat_copy(Pattern* des, const Pattern* src)
{
    memcpy(des->pattern, src->pattern, sizeof(src->pattern));
}

#ifdef __cplusplus
}
#endif

#endif
