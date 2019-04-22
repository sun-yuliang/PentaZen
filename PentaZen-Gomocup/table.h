/*
 *  PentaZen Gomoku/Renju Engine
 *                    _______
 *                   / _____/
 *  developed by    / /______  ________
 *                 /____  / / / / __  /
 *                _____/ / /_/ / / / /
 *  2019.04      /______/_____/_/ /_/
 *
 * table.h - table generation and lookup functions
 */

#ifndef __TABLE_H__
#define __TABLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "public.h"
#include "board.h"

// table element size
#define NEI_SIZE	2
#define VCX_SIZE	4
#define PAT_SIZE	8
#define SPE_SIZE	26

// table lookup operation
#define ADD			0
#define SUB			1

// special point macro
#define DD4			0
#define DF3			1
#define FD4			2
#define FF3			3
#define DF2			4
#define DF2P		5
#define FD3			6
#define FF2			7

#define BDD4		0
#define BDF3		1
#define BFD4		2
#define BFF3		3
#define BDF2		4
#define BDF2P		5
#define BFD3		6
#define BFF2		7

#define WDD4		8
#define WDF3		9
#define WFD4		10
#define WFF3		11
#define WDF2		12
#define WDF2P		13
#define WFD3		14
#define WFF2		15

#define X			INVALID
#define Z			INVALID

 // Powers of two
#define P0					1
#define P1					2
#define P2					4
#define P3					8
#define P4					16
#define P5					32
#define P6					64
#define P7					128
#define P8					256
#define P9					512
#define P10					1024
#define P11					2048
#define P12					4096
#define P13					8192
#define P14					16384
#define P15					32768
#define P16					65536
#define P17					131072
#define P18					262144
#define P19					524288
#define P20					1048576
#define P21					2097152
#define P22					4194304
#define P23					8388608
#define P24					16777216

// neighbor tables
extern u16 NeiA15[15 * 15];
extern u16 NeiA20[20 * 20];
extern u16 NeiD15[15 * 15];
extern u16 NeiD20[20 * 20];
extern u16 Nei15[15 * 15][NEI_SIZE * 8];
extern u16 Nei20[20 * 20][NEI_SIZE * 8];
extern u16 Vcx15[15 * 15][VCX_SIZE * 8];
extern u16 Vcx20[20 * 20][VCX_SIZE * 8];

// pattern tables
extern u16 Pat20[P20][PAT_SIZE + SPE_SIZE];
extern u16 Pat19[P19][PAT_SIZE + SPE_SIZE];
extern u16 Pat18[P18][PAT_SIZE + SPE_SIZE];
extern u16 Pat17[P17][PAT_SIZE + SPE_SIZE];
extern u16 Pat16[P16][PAT_SIZE + SPE_SIZE];
extern u16 Pat15[P15][PAT_SIZE + SPE_SIZE];
extern u16 Pat14[P14][PAT_SIZE + SPE_SIZE];
extern u16 Pat13[P13][PAT_SIZE + SPE_SIZE];
extern u16 Pat12[P12][PAT_SIZE + SPE_SIZE];
extern u16 Pat11[P11][PAT_SIZE + SPE_SIZE];
extern u16 Pat10[P10][PAT_SIZE + SPE_SIZE];
extern u16 Pat9[P9][PAT_SIZE + SPE_SIZE];
extern u16 Pat8[P8][PAT_SIZE + SPE_SIZE];
extern u16 Pat7[P7][PAT_SIZE + SPE_SIZE];
extern u16 Pat6[P6][PAT_SIZE + SPE_SIZE];
extern u16 Pat5[P5][PAT_SIZE + SPE_SIZE];

void nei15_helper_init();

void nei20_helper_init();

void nei15_init();

void nei20_init();

void vcx15_init();

void vcx20_init();

void pat_init(const u16 rule, const u16 size);

void line_update(board_t* bd, const u16 start, const u16 len, const u16 step, const u16 color, const u16 op);

void line_update_fast(board_t* bd, const u16 start, const u16 len, const u16 step, const u16 color, const u16 op);

#ifdef __cplusplus
}
#endif

#endif
