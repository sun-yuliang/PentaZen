/*
 *  PentaZen Gomoku/Renju Engine
 *                    _______
 *                   / _____/
 *  developed by    / /______  ________
 *                 /____  / / / / __  /
 *                _____/ / /_/ / / / /
 *  2019.04      /______/_____/_/ /_/
 *
 * gomocup.h - gomocup protocol functions
 */

#ifndef __GOMOCUP_H__
#define __GOMOCUP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"
#include "search.h"

void gomocup_loop(board_t* bd, search_t* srh);

#if SEARCH_TEST
void search_test(board_t* bd, search_t* srh);
#endif

#ifdef __cplusplus
}
#endif

#endif
