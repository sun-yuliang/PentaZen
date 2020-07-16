/*      _____                __    ______
 *     / ___ \              / /   /___  /
 *    / /__/ /___  ____  __/ /_______/ /    ____  ____
 *   / _____/ __ \/ __ \/_   _/ __  / /    / __ \/ __ \
 *  / /    /  ___/ / / / / /_/ /_/ / /____/  ___/ / / /
 * /_/     \____/_/ /_/ /___/\__,_/______/\____/_/ /_/
 *
 * PentaZen, a Gomoku/Renju playing engine developed by Sun.
 *
 * attacking and defending array:
 * __ __ __ __   __ __ __ __
 *         F3d           B4a
 * DL DR  D  R   DL DR  D  R
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "public.h"
#include "mvlist.h"
#include "pattern.h"

#define num(bd)         (bd->num[0] + bd->num[1])
#define cand(bd)        bd->cand[num(bd)]
#define ad(bd)          bd->ad[num(bd)]
#define b4d(bd)         bd->b4d[num(bd)]
#define score(bd)       bd->sc[num(bd)]
#define pcand(bd)       &bd->cand[num(bd)]
#define ppat(bd)        &bd->pat[num(bd)]
#define ppinc(bd)       &bd->pinc

typedef struct {
    Pos sta;
    Pos end;
} Segment;

typedef struct
{
    Rule rule;                              // rule
    Color fcolor;                           // first move color
    Color next;                             // next move color
    TTKey ttkey;                            // zobrist key
    u16 num[2];                             // stone number
    Pos board[BD_SIZE];                     // board array
    Pos mlist[BD_SIZE];                     // move array

    Pattern pinc;                           // pattern increment
    Segment seg[2][4][BD_SIZE];             // segment array for each color, direction and position

    Mvlist cand[BD_SIZE + 1];               // candidate movelist stack
    Pattern pat[BD_SIZE + 1];               // pattern stack
    ADType ad[BD_SIZE + 1][2][BD_SIZE];     // attacking and defending info stack
    Pos b4d[BD_SIZE + 1];                   // B4d position stack
    Score sc[BD_SIZE + 1][2];               // score stack

    Color me;                               // color to search for
    u16 search_dep;                         // search depth
    u16 stone_ini;                          // stone number before one search
    u16 stone_max;                          // maximum stone number during one search
    u32 node_cnt;                           // total nodes visited during one search
    Pos best;                               // last best move

    Time t_turn;                            // timeout turn
    Time t_match;                           // timeout match
    Time t_left;                            // time left
    Time t_begin;                           // begin time
    Time t_end;                             // end time
    Time tlimit;                            // time limitation for one search
    bool timeout;                           // timeout flag
} Board;

void board_reset(Board* bd, const Color fc, const Rule ru);

void do_move(Board* bd, const Pos pos);

void do_move_f(Board* bd, const Pos pos);

void undo(Board* bd);

void undo_f(Board* bd);

Color check_wld(const Board* bd, u8* offset);

Pos generate_wld(Board* bd);

bool generate_forced(Board* bd);

u32 get_turn_time(Board* bd);

// Check attacking/defending info.
static inline u8 check(const Board* bd, const Color c, const ADType t, const Pos pos)
{
    assert(iscolor(c));
    assert(t == B4a || t == F3d);
    assert(ispos(pos));

    return t == B4a ? ad(bd)[c - 1][pos] & 0x0f
                    : ad(bd)[opp(c) - 1][pos] & 0xf0;
}

#ifdef __cplusplus
}
#endif

#endif
