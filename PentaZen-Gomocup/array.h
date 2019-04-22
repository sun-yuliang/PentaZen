/*
 *  PentaZen Gomoku/Renju Engine
 *                    _______
 *                   / _____/
 *  developed by    / /______  ________
 *                 /____  / / / / __  /
 *                _____/ / /_/ / / / /
 *  2019.04      /______/_____/_/ /_/
 *
 * array.h - arrays of constants
 */

#ifndef __ARRAY_H__
#define __ARRAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "public.h"

extern const u16 r15[15 * 15];
extern const u16 c15[15 * 15];
extern const u16 md15[15 * 15];
extern const u16 ad15[15 * 15];

extern const u16 r20[20 * 20];
extern const u16 c20[20 * 20];
extern const u16 md20[20 * 20];
extern const u16 ad20[20 * 20];

extern const u16 rstart15[15 * 15];
extern const u16 cstart15[15 * 15];
extern const u16 mdstart15[15 * 15];
extern const u16 adstart15[15 * 15];
extern const u16 mdlen15[15 * 15];
extern const u16 adlen15[15 * 15];

extern const u16 rstart20[20 * 20];
extern const u16 cstart20[20 * 20];
extern const u16 mdstart20[20 * 20];
extern const u16 adstart20[20 * 20];
extern const u16 mdlen20[20 * 20];
extern const u16 adlen20[20 * 20];

extern const u64 Zobrist[3][400];

#ifdef __cplusplus
}
#endif

#endif
