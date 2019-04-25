/*
 *  PentaZen Gomoku/Renju Engine
 *                    _______
 *                   / _____/
 *  developed by    / /______  ________
 *                 /____  / / / / __  /
 *                _____/ / /_/ / / / /
 *  2019.04      /______/_____/_/ /_/
 *
 * public.h - program macros and typedefs
 */

#ifndef __PUBLIC_H__
#define __PUBLIC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>
#include <time.h>

// time management
#define TIME_DIVIDER1		24.0
#define TIME_DIVIDER2		50.0
#define TIME_DIV_PCT		0.85
#define ITE_STOP_PCT		0.5
#define SECOND_RSV			0.1

// control options
#define SEARCH_TEST			0
#define ITERATIVE			1
#define VCF_SEARCH			1
#define VCT_SEARCH			1
#define SPE_D3F2			0

// display options
#define DISPLAY_BOARD		0
#define DISPLAY_MSTK		0
#define DISPLAY_HLIST		0
#define DISPLAY_PAT			0
#define DISPLAY_PINC		0
#define DISPLAY_MLIST		0
#define DISPLAY_ZOBRIST		0

// basic constants
#define EMPTY				0
#define BLACK				1
#define WHITE				2
#define WIN					10000000
#define LOSE				-10000000
#define WIN_THRE			9999600
#define LOSE_THRE			-9999600
#define DRAW				400
#define INVALID				65535
#define FREESTYLE			0
#define STANDARD			1
#define RENJU				4
#define MAX_STR_LEN			64

// unsigned int types
// u16 is the minimum type for unsigned values in this program
typedef	uint8_t	 u8;
typedef	uint16_t u16;
typedef	uint32_t u32;
typedef	uint64_t u64;

#ifdef __cplusplus
}
#endif

#endif
