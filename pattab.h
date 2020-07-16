/*      _____                __    ______
 *     / ___ \              / /   /___  /
 *    / /__/ /___  ____  __/ /_______/ /    ____  ____
 *   / _____/ __ \/ __ \/_   _/ __  / /    / __ \/ __ \
 *  / /    /  ___/ / / / / /_/ /_/ / /____/  ___/ / / /
 * /_/     \____/_/ /_/ /___/\__,_/______/\____/_/ /_/
 *
 * PentaZen, a Gomoku/Renju playing engine developed by Sun.
 */

#ifndef __PATTAB_H__
#define __PATTAB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "public.h"

#define PAT_SIZE        5
#define AD_SIZE         26
#define PAT_START       3
#define AD_START        (3 + PAT_SIZE)
#define PATTAB_SIZE     (3 + PAT_SIZE + AD_SIZE)

extern u8 Pat5f[IIE5][PATTAB_SIZE];
extern u8 Pat6f[IIE6][PATTAB_SIZE];
extern u8 Pat7f[IIE7][PATTAB_SIZE];
extern u8 Pat8f[IIE8][PATTAB_SIZE];
extern u8 Pat9f[IIE9][PATTAB_SIZE];
extern u8 Pat10f[IIE10][PATTAB_SIZE];
extern u8 Pat11f[IIE11][PATTAB_SIZE];
extern u8 Pat12f[IIE12][PATTAB_SIZE];
extern u8 Pat13f[IIE13][PATTAB_SIZE];
extern u8 Pat14f[IIE14][PATTAB_SIZE];
extern u8 Pat15f[IIE15][PATTAB_SIZE];

extern u8 Pat5[IIE5][PATTAB_SIZE];
extern u8 Pat6[IIE6][PATTAB_SIZE];
extern u8 Pat7[IIE7][PATTAB_SIZE];
extern u8 Pat8[IIE8][PATTAB_SIZE];
extern u8 Pat9[IIE9][PATTAB_SIZE];
extern u8 Pat10[IIE10][PATTAB_SIZE];
extern u8 Pat11[IIE11][PATTAB_SIZE];
extern u8 Pat12[IIE12][PATTAB_SIZE];
extern u8 Pat13[IIE13][PATTAB_SIZE];
extern u8 Pat14[IIE14][PATTAB_SIZE];
extern u8 Pat15[IIE15][PATTAB_SIZE];

extern u8* (*get_entry[2][3][BD_SIZE + 1])(const u32);

void generate_pattab();

void randomize();

#ifdef __cplusplus
}
#endif

#endif
