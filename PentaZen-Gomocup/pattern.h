/*
 *  PentaZen Gomoku/Renju Engine
 *                    _______
 *                   / _____/
 *  developed by    / /______  ________
 *                 /____  / / / / __  /
 *                _____/ / /_/ / / / /
 *  2019.04      /______/_____/_/ /_/
 *
 * pattern.h - pattern_t data structure
 */

#ifndef __PATTERN_H__
#define __PATTERN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "public.h"

// TYPE macros
#define PAT_NUM		13
#define LONG		0
#define FIVE		1
#define FREE4		2
#define DEAD4		3
#define FREE3a		4
#define FREE3		5
#define DEAD3		6
#define FREE2a		7
#define FREE2		8
#define DEAD2		9
#define FREE1a		10
#define FREE1		11
#define DEAD1		12

// pattern_t structure
typedef	struct {
	char arr[2][PAT_NUM];
} pattern_t;

// Reset a pattern.
static void pattern_reset(pattern_t* pat);

// Copy a pattern.
static void pattern_copy(pattern_t* out, const pattern_t* in);

// Return the value of type in pat.
static int pattern_read(const pattern_t* pat, const u16 type, const u16 color);

// Return the number of (free3 + free3a) in pat.
static int pattern_read_free3(const pattern_t* pat, const u16 color);

// Increase the value of type in pat by 1.
static void pattern_inc(pattern_t* pat, const u16 type, const u16 color);

// Decrease the value of type in pat by 1.
static void pattern_dec(pattern_t* pat, const u16 type, const u16 color);

// Add pattern in1 and pattern in2.
static void pattern_add(pattern_t* out, const pattern_t* in1, const pattern_t* in2);

// Subtract pattern in2 from pattern in1.
static void pattern_sub(pattern_t* out, const pattern_t* in1, const pattern_t* in2);

// Implementation
static inline void pattern_reset(pattern_t* pat)
{
	memset(pat, 0, sizeof(pattern_t));
}

static inline void pattern_copy(pattern_t* out, const pattern_t* in)
{
	memcpy(out, in, sizeof(pattern_t));
}

static inline int pattern_read(const pattern_t* pat, const u16 type, const u16 color)
{
	return pat->arr[color - 1][type];
}

static inline int pattern_read_free3(const pattern_t* pat, const u16 color)
{
	return pat->arr[color - 1][FREE3a] + pat->arr[color - 1][FREE3];
}

static inline void pattern_inc(pattern_t* pat, const u16 type, const u16 color)
{
	pat->arr[color - 1][type]++;
}

static inline void pattern_dec(pattern_t* pat, const u16 type, const u16 color)
{
	pat->arr[color - 1][type]--;
}

static inline void pattern_add(pattern_t* out, const pattern_t* in1, const pattern_t* in2)
{
	int i, j;
	for (i = 0; i < 2; i++)
		for (j = 0; j < PAT_NUM; j++)
			out->arr[i][j] = in1->arr[i][j] + in2->arr[i][j];
}

static inline void pattern_sub(pattern_t* out, const pattern_t* in1, const pattern_t* in2)
{
	int i, j;
	for (i = 0; i < 2; i++)
		for (j = 0; j < PAT_NUM; j++)
			out->arr[i][j] = in1->arr[i][j] - in2->arr[i][j];
}

#ifdef __cplusplus
}
#endif

#endif
