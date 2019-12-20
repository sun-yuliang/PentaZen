/*      _____                __    ______
 *     / ___ \              / /   /___  /
 *    / /__/ /___  ____  __/ /_______/ /    ____  ____
 *   / _____/ __ \/ __ \/_   _/ __  / /    / __ \/ __ \
 *  / /    /  ___/ / / / / /_/ /_/ / /____/  ___/ / / /
 * /_/     \____/_/ /_/ /___/\__,_/______/\____/_/ /_/
 *
 * PentaZen, a Gomoku/Renju playing engine developed by Sun.
 *
 * Issue: cannot recognize recursively defined foul and fake F3.
 */

#include "board.h"
#include "distance.h"
#include "pattab.h"
#include "index.h"

#define StepR       1
#define StepD       BD_SIDE
#define StepDR      (BD_SIDE + 1)
#define StepDL      (BD_SIDE - 1)

// Step array.
static Pos Step[4] = { StepR, StepD, StepDR, StepDL };

// Powers of 2.
static u32 Pow2[3][BD_SIDE] = {
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { IIE0, IIE1, IIE2, IIE3, IIE4, IIE5, IIE6, IIE7, IIE8, IIE9, IIE10, IIE11, IIE12, IIE13, IIE14 },
    { IIE0, IIE1, IIE2, IIE3, IIE4, IIE5, IIE6, IIE7, IIE8, IIE9, IIE10, IIE11, IIE12, IIE13, IIE14 }
};

// Step index.
static Pos StepInd[17] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 2 };

// Masks to reset records in one line for one position.
static ADType LineClr[4] = { 0xee, 0xdd, 0xbb, 0x77 };

// Masks to record attacking point and defending point.
static ADType ADMask[4][2] = {
    { 0x01, 0x10 },
    { 0x02, 0x20 },
    { 0x04, 0x40 },
    { 0x08, 0x80 }
};

// Update patterns, score, attacking/defending info.
static void line_update(Board* bd, const Color c, const u8 op, const Pos step, const Pos sta, const u8 len)
{
    if (len < 5)
        return;

    assert(iscolor(c));
    assert(op == ADD || op == SUB);
    assert(step == 1 || step == BD_SIDE - 1 || step == BD_SIDE || step == BD_SIDE + 1);
    assert(ispos(sta));
    assert(len <= BD_SIDE);

    u32 ind = 0;
    Pos pos = sta;
    u8 i, *tab = NULL;

    if (op == ADD)
    {
        for (i = 0; i < len; i++)
        {
            ind |= Pow2[bd->board[pos]][i];
            pos += step;
        }
        if (ind == 0)
            return;
        tab = get_entry[c == bd->fcolor][bd->rule][len](ind);

        for (i = PAT_START; i < tab[1]; i++)
            pat_inc(ppinc(bd), c, tab[i]);

        score(bd)[c - 1] += tab[0];

        for (i = AD_START; i < tab[2]; i += 2)
        {
            if (tab[i + 1] == B4d)
                b4d(bd) = sta + tab[i] * step;
            else
                ad(bd)[c - 1][sta + tab[i] * step] |= ADMask[StepInd[step]][tab[i + 1]];
        }
    }
    else
    {
        for (i = 0; i < len; i++)
        {
            ad(bd)[c - 1][pos] &= LineClr[StepInd[step]];
            ind |= Pow2[bd->board[pos]][i];
            pos += step;
        }
        if (ind == 0)
            return;
        tab = get_entry[c == bd->fcolor][bd->rule][len](ind);

        for (i = PAT_START; i < tab[1]; i++)
            pat_dec(ppinc(bd), c, tab[i]);

        score(bd)[c - 1] -= tab[0];
    }
}

// Update only patterns and score.
static void line_update_f(Board* bd, const Color c, const u8 op, const Pos step, const Pos sta, const u8 len)
{
    if (len < 5)
        return;

    assert(iscolor(c));
    assert(op == ADD || op == SUB);
    assert(step == 1 || step == BD_SIDE - 1 || step == BD_SIDE || step == BD_SIDE + 1);
    assert(ispos(sta));
    assert(len <= BD_SIDE);

    u32 ind = 0;
    Pos pos = sta;
    u8 i, * tab = NULL;

    for (i = 0; i < len; i++)
    {
        ind |= Pow2[bd->board[pos]][i];
        pos += step;
    }
    if (ind == 0)
        return;
    tab = get_entry[c == bd->fcolor][bd->rule][len](ind);

    if (op == ADD)
    {
        for (i = PAT_START; i < tab[1]; i++)
            pat_inc(ppinc(bd), c, tab[i]);

        score(bd)[c - 1] += tab[0];
    }
    else
    {
        for (i = PAT_START; i < tab[1]; i++)
            pat_dec(ppinc(bd), c, tab[i]);

        score(bd)[c - 1] -= tab[0];
    }
}

// Reset the board.
void board_reset(Board* bd, const Color fc, const Rule ru)
{
    assert(iscolor(fc));
    assert(isrule(ru));

    bd->rule = ru;
    bd->fcolor = fc;
    bd->next = fc;
    bd->ttkey = 0;
    bd->num[0] = 0;
    bd->num[1] = 0;

    pat_reset(ppinc(bd));
    memset(bd->board, 0, sizeof(bd->board));
    memset(bd->mlist, -1, sizeof(bd->mlist));
    memset(bd->ad, 0, sizeof(bd->ad));

    for (u16 i = 0; i < BD_SIZE + 1; i++)
    {
        mvlist_reset(&bd->cand[i]);
        pat_reset(&bd->pat[i]);
        bd->b4d[i] = POS_INVALID;
        bd->sc[i][0] = 0;
        bd->sc[i][1] = 0;
    }

    for (u16 i = 0; i < 2; i++)
    {
        for (u16 j = 0; j < BD_SIZE; j++)
        {
            bd->seg[i][0][j].sta = ro[j];
            bd->seg[i][1][j].sta = fo[j];
            bd->seg[i][2][j].sta = mdo[j];
            bd->seg[i][3][j].sta = ado[j];
            bd->seg[i][0][j].end = ro[j] + StepR * 14;
            bd->seg[i][1][j].end = fo[j] + StepD * 14;
            bd->seg[i][2][j].end = mdo[j] + StepDR * (mdl[j] - 1);
            bd->seg[i][3][j].end = ado[j] + StepDL * (adl[j] - 1);
        }
    }
}

// Update bd->board, ppinc(bd), ppat(bd), ad(bd), b4d(bd), score(bd).
static void lookup_update(Board* bd, const Pos pos)
{
    assert(ispos(pos));

    Color c = opp(bd->next);
    Color oc = bd->next;

    Pos r1 = bd->seg[c - 1][0][pos].sta;
    Pos r2 = bd->seg[c - 1][0][pos].end;
    Pos r3 = bd->seg[oc - 1][0][pos].sta;
    Pos r4 = bd->seg[oc - 1][0][pos].end;

    Pos c1 = bd->seg[c - 1][1][pos].sta;
    Pos c2 = bd->seg[c - 1][1][pos].end;
    Pos c3 = bd->seg[oc - 1][1][pos].sta;
    Pos c4 = bd->seg[oc - 1][1][pos].end;

    Pos m1 = bd->seg[c - 1][2][pos].sta;
    Pos m2 = bd->seg[c - 1][2][pos].end;
    Pos m3 = bd->seg[oc - 1][2][pos].sta;
    Pos m4 = bd->seg[oc - 1][2][pos].end;

    Pos a1 = bd->seg[c - 1][3][pos].sta;
    Pos a2 = bd->seg[c - 1][3][pos].end;
    Pos a3 = bd->seg[oc - 1][3][pos].sta;
    Pos a4 = bd->seg[oc - 1][3][pos].end;
    
    pat_reset(ppinc(bd));
    score(bd)[0] = bd->sc[num(bd) - 1][0];
    score(bd)[1] = bd->sc[num(bd) - 1][1];
    memcpy(ad(bd), bd->ad[num(bd) - 1], sizeof(ad(bd)));
    b4d(bd) = bd->b4d[num(bd) - 1];

    line_update(bd, c, SUB, StepR, r1, r[r2] - r[r1] + 1);
    line_update(bd, c, SUB, StepD, c1, f[c2] - f[c1] + 1);
    line_update(bd, c, SUB, StepDR, m1, md[m2] - md[m1] + 1);
    line_update(bd, c, SUB, StepDL, a1, ad[a2] - ad[a1] + 1);

    line_update(bd, oc, SUB, StepR, r3, r[r4] - r[r3] + 1);
    line_update(bd, oc, SUB, StepD, c3, f[c4] - f[c3] + 1);
    line_update(bd, oc, SUB, StepDR, m3, md[m4] - md[m3] + 1);
    line_update(bd, oc, SUB, StepDL, a3, ad[a4] - ad[a3] + 1);

    bd->board[pos] = c;

    line_update(bd, c, ADD, StepR, r1, r[r2] - r[r1] + 1);
    line_update(bd, c, ADD, StepD, c1, f[c2] - f[c1] + 1);
    line_update(bd, c, ADD, StepDR, m1, md[m2] - md[m1] + 1);
    line_update(bd, c, ADD, StepDL, a1, ad[a2] - ad[a1] + 1);

    line_update(bd, oc, ADD, StepR, r3, r[pos] - r[r3]);
    line_update(bd, oc, ADD, StepD, c3, f[pos] - f[c3]);
    line_update(bd, oc, ADD, StepDR, m3, md[pos] - md[m3]);
    line_update(bd, oc, ADD, StepDL, a3, ad[pos] - ad[a3]);

    line_update(bd, oc, ADD, StepR, pos + StepR, r[r4] - r[pos]);
    line_update(bd, oc, ADD, StepD, pos + StepD, f[c4] - f[pos]);
    line_update(bd, oc, ADD, StepDR, pos + StepDR, md[m4] - md[pos]);
    line_update(bd, oc, ADD, StepDL, pos + StepDL, ad[a4] - ad[pos]);

    pat_add(ppat(bd), &bd->pat[num(bd) - 1], ppinc(bd));
}

// Update bd->board, ppinc(bd), ppat(bd), score(bd).
static void lookup_update_f(Board* bd, const Pos pos)
{
    assert(ispos(pos));

    Color c = opp(bd->next);
    Color oc = bd->next;

    Pos r1 = bd->seg[c - 1][0][pos].sta;
    Pos r2 = bd->seg[c - 1][0][pos].end;
    Pos r3 = bd->seg[oc - 1][0][pos].sta;
    Pos r4 = bd->seg[oc - 1][0][pos].end;

    Pos c1 = bd->seg[c - 1][1][pos].sta;
    Pos c2 = bd->seg[c - 1][1][pos].end;
    Pos c3 = bd->seg[oc - 1][1][pos].sta;
    Pos c4 = bd->seg[oc - 1][1][pos].end;

    Pos m1 = bd->seg[c - 1][2][pos].sta;
    Pos m2 = bd->seg[c - 1][2][pos].end;
    Pos m3 = bd->seg[oc - 1][2][pos].sta;
    Pos m4 = bd->seg[oc - 1][2][pos].end;

    Pos a1 = bd->seg[c - 1][3][pos].sta;
    Pos a2 = bd->seg[c - 1][3][pos].end;
    Pos a3 = bd->seg[oc - 1][3][pos].sta;
    Pos a4 = bd->seg[oc - 1][3][pos].end;

    pat_reset(ppinc(bd));
    score(bd)[0] = bd->sc[num(bd) - 1][0];
    score(bd)[1] = bd->sc[num(bd) - 1][1];

    line_update_f(bd, c, SUB, StepR, r1, r[r2] - r[r1] + 1);
    line_update_f(bd, c, SUB, StepD, c1, f[c2] - f[c1] + 1);
    line_update_f(bd, c, SUB, StepDR, m1, md[m2] - md[m1] + 1);
    line_update_f(bd, c, SUB, StepDL, a1, ad[a2] - ad[a1] + 1);

    line_update_f(bd, oc, SUB, StepR, r3, r[r4] - r[r3] + 1);
    line_update_f(bd, oc, SUB, StepD, c3, f[c4] - f[c3] + 1);
    line_update_f(bd, oc, SUB, StepDR, m3, md[m4] - md[m3] + 1);
    line_update_f(bd, oc, SUB, StepDL, a3, ad[a4] - ad[a3] + 1);

    bd->board[pos] = c;

    line_update_f(bd, c, ADD, StepR, r1, r[r2] - r[r1] + 1);
    line_update_f(bd, c, ADD, StepD, c1, f[c2] - f[c1] + 1);
    line_update_f(bd, c, ADD, StepDR, m1, md[m2] - md[m1] + 1);
    line_update_f(bd, c, ADD, StepDL, a1, ad[a2] - ad[a1] + 1);

    line_update_f(bd, oc, ADD, StepR, r3, r[pos] - r[r3]);
    line_update_f(bd, oc, ADD, StepD, c3, f[pos] - f[c3]);
    line_update_f(bd, oc, ADD, StepDR, m3, md[pos] - md[m3]);
    line_update_f(bd, oc, ADD, StepDL, a3, ad[pos] - ad[a3]);

    line_update_f(bd, oc, ADD, StepR, pos + StepR, r[r4] - r[pos]);
    line_update_f(bd, oc, ADD, StepD, pos + StepD, f[c4] - f[pos]);
    line_update_f(bd, oc, ADD, StepDR, pos + StepDR, md[m4] - md[pos]);
    line_update_f(bd, oc, ADD, StepDL, pos + StepDL, ad[a4] - ad[pos]);

    pat_add(ppat(bd), &bd->pat[num(bd) - 1], ppinc(bd));
}

// Update bd->seg.
static void seg_update(Board* bd, const Pos pos)
{
    assert(ispos(pos));

    for (u8 i = 0; i < 4; i++)
    {
        for (Pos j = bd->seg[bd->next - 1][i][pos].sta; j <= pos - Step[i]; j += Step[i])
            bd->seg[bd->next - 1][i][j].end = pos - Step[i];

        for (Pos j = pos + Step[i]; j <= bd->seg[bd->next - 1][i][pos].end; j += Step[i])
            bd->seg[bd->next - 1][i][j].sta = pos + Step[i];
    }
}

// Restore bd->seg.
static void seg_undo(Board* bd, const Pos pos)
{
    assert(ispos(pos));

    for (u16 i = 0; i < 4; i++)
    {
        for (Pos j = bd->seg[opp(bd->next) - 1][i][pos].sta; j <= bd->seg[opp(bd->next) - 1][i][pos].end; j += Step[i])
        {
            bd->seg[opp(bd->next) - 1][i][j].sta = bd->seg[opp(bd->next) - 1][i][pos].sta;
            bd->seg[opp(bd->next) - 1][i][j].end = bd->seg[opp(bd->next) - 1][i][pos].end;
        }
    }
}

// Update pcand(bd).
static void cand_update(Board* bd, const Pos pos)
{
    assert(ispos(pos));

    mvlist_copy(pcand(bd), &bd->cand[num(bd) - 1]);
    mvlist_remove(pcand(bd), pos);

    for (u8 i = 0; i < NeiInd[pos]; i++)
        if (bd->board[Nei[pos][i]] == EMPTY)
            mvlist_insert(pcand(bd), Nei[pos][i], VAL_INVALID);
}

// Make a move and update all.
void do_move(Board* bd, const Pos pos)
{
    assert(ispos(pos));

    if (bd->board[pos] == EMPTY)
    {
        bd->ttkey ^= Random[bd->next][pos];
        bd->mlist[num(bd)] = pos;
        bd->num[bd->next - 1]++;
        bd->next = opp(bd->next);
        lookup_update(bd, pos);
        seg_update(bd, pos);
        cand_update(bd, pos);
    }
}

// Make a move and update partially.
void do_move_f(Board* bd, const Pos pos)
{
    assert(ispos(pos));

    if (bd->board[pos] == EMPTY)
    {
        bd->ttkey ^= Random[bd->next][pos];
        bd->mlist[num(bd)] = pos;
        bd->num[bd->next - 1]++;
        bd->next = opp(bd->next);
        lookup_update_f(bd, pos);
    }
}

// Undo the last move and restore all.
void undo(Board* bd)
{
    Pos pos;
    if (num(bd) > 0)
    {
        pos = bd->mlist[num(bd) - 1];
        bd->next = opp(bd->next);
        bd->num[bd->next - 1]--;
        bd->board[pos] = EMPTY;
        pat_reset(ppinc(bd));
        seg_undo(bd, pos);
        bd->ttkey ^= Random[bd->next][pos];
    }
}

// Undo the last move and restore partially.
void undo_f(Board* bd)
{
    Pos pos;
    if (num(bd) > 0)
    {
        pos = bd->mlist[num(bd) - 1];
        bd->next = opp(bd->next);
        bd->num[bd->next - 1]--;
        bd->board[pos] = EMPTY;
        pat_reset(ppinc(bd));
        bd->ttkey ^= Random[bd->next][pos];
    }
}

// Strict WLD judgement. Return win/lose color or DRAW or COL_INVALID.
static Color strict_wld(const Board* bd)
{
    if (pat_read(ppat(bd), BLACK, C5) > 0)
        return BLACK;

    if (pat_read(ppat(bd), WHITE, C5) > 0)
        return WHITE;

    if (num(bd) >= BD_SIZE)
        return DRAW;

    if (bd->rule == RENJU)
    {
        if (pat_read(ppat(bd), bd->fcolor, C6) > 0 || pat_read(ppat(bd), opp(bd->fcolor), C6) > 0)
            return opp(bd->fcolor);

        if (pat_read(ppinc(bd), bd->fcolor, F4) + pat_read(ppinc(bd), bd->fcolor, B4) >= 2)
            return opp(bd->fcolor);

        if (pat_read(ppinc(bd), bd->fcolor, F3) >= 2)
            return opp(bd->fcolor);
    }

    return COL_INVALID;
}

// WLD judgement. Return win/lose color or DRAW or COL_INVALID.
// offset is the remaining number of moves to WLD.
Color check_wld(const Board* bd, u8* offset)
{
    // strict wld
    Color wld = strict_wld(bd);
    if (wld != COL_INVALID)
    {
        *offset = 0;
        return wld;
    }

    // bd->next has F4 or B4
    if (pat_read(ppat(bd), bd->next, F4) > 0 || pat_read(ppat(bd), bd->next, B4) > 0)
    {
        *offset = 1;
        return bd->next;
    }

    // opp(bd->next) has F4 or several B4
    if (pat_read(ppat(bd), opp(bd->next), F4) > 0 || pat_read(ppat(bd), opp(bd->next), B4) >= 2)
    {
        *offset = 2;
        return opp(bd->next);
    }

    // bd->next has F3 and both have no B4
    if (pat_read(ppat(bd), bd->next, F3) > 0 && pat_read(ppat(bd), opp(bd->next), B4) == 0)
    {
        *offset = 3;
        return bd->next;
    }

    return COL_INVALID;
}

// Return one position to WLD. Return POS_INVALID if doesn't exist.
// Before searching this function must be called to prevent returning invalid move.
Pos generate_wld(Board* bd)
{
    // bd->next draws
    if (num(bd) == BD_SIZE - 1)
        return cand(bd).list[0].pos;

    // bd->next has F4 or B4. bd->next forms C5 wins.
    if (pat_read(ppat(bd), bd->next, F4) > 0 || pat_read(ppat(bd), bd->next, B4) > 0)
    {
        for (u16 i = 0; i < cand(bd).cut; i++)
        {
            do_move(bd, cand(bd).list[i].pos);
            if (pat_read(ppinc(bd), opp(bd->next), C5) > 0)
            {
                undo(bd);
                return cand(bd).list[i].pos;
            }
            undo(bd);
        }
    }

    // opp(bd->next) has F4. bd->next loses.
    if (pat_read(ppat(bd), opp(bd->next), F4) > 0)
    {
        for (u16 i = 0; i < cand(bd).cut; i++)
        {
            do_move(bd, cand(bd).list[i].pos);
            if (pat_read(ppinc(bd), bd->next, F4) < 0)
            {
                undo(bd);
                return cand(bd).list[i].pos;
            }
            undo(bd);
        }
    }

    // opp(bd->next) has several B4. bd->next loses.
    if (pat_read(ppat(bd), opp(bd->next), B4) >= 2)
    {
        return b4d(bd);
    }

    // bd->next has F3 and both have no B4. bd->next forms F4 and wins.
    if (pat_read(ppat(bd), bd->next, F3) > 0 && pat_read(ppat(bd), opp(bd->next), B4) == 0)
    {
        for (u16 i = 0; i < cand(bd).cut; i++)
        {
            do_move(bd, cand(bd).list[i].pos);
            if (pat_read(ppinc(bd), opp(bd->next), F4) > 0)
            {
                undo(bd);
                return cand(bd).list[i].pos;
            }
            undo(bd);
        }
    }

    return COL_INVALID;
}

// Generate forced defending moves. Call this function when WLD is uncertain.
bool generate_forced(Board* bd)
{
    // opp(bd->next) has B4. bd->next defends.
    if (pat_read(ppat(bd), opp(bd->next), B4) > 0)
    {
        cand(bd).cut = 0;
        for (u16 i = 0; i < cand(bd).size; i++)
        {
            if (cand(bd).list[i].pos == b4d(bd))
            {
                mvlist_swap(pcand(bd), cand(bd).cut++, i);
                return true;
            }
        }
    }

    // opp(bd->next) has F3. bd->next forms B4 or defends.
    if (pat_read(ppat(bd), opp(bd->next), F3) > 0)
    {
        cand(bd).cut = 0;
        for (u16 i = 0; i < cand(bd).size; i++)
            if (check(bd, bd->next, B4a, cand(bd).list[i].pos) || check(bd, bd->next, F3d, cand(bd).list[i].pos))
                mvlist_swap(pcand(bd), cand(bd).cut++, i);
        return true;
    }

    return false;
}

// Get next search time limitation.
u32 get_turn_time(Board* bd)
{
    Time t;

    if (bd->t_left > (Time)(bd->t_match * (1.0 - TIME_THRE1)))
    {
        t = (Time)(bd->t_match / TIME_DIV1);
    }
    else if (bd->t_left > (Time)(bd->t_match * (1.0 - TIME_THRE2)))
    {
        t = (Time)(bd->t_match / TIME_DIV2);
    }
    else
    {
        t = (Time)(bd->t_left / ceil(((double)BD_SIZE - bd->num[0] - bd->num[1]) / 2.0 + 10.0));
    }

    return MyMin(t, bd->t_turn) - TIME_RSV;
}
