/*
 *  PentaZen Gomoku/Renju Engine
 *                    _______
 *                   / _____/
 *  developed by    / /______  ________
 *                 /____  / / / / __  /
 *                _____/ / /_/ / / / /
 *  2019.04      /______/_____/_/ /_/
 *
 * board.c - board_t data structure and functions
 */

#include "board.h"
#include "table.h"
#include "utility.h"
#include "array.h"
#include "hash.h"

#define D15			15
#define R15			1
#define DL15		14
#define DR15		16
#define D20			20
#define R20			1
#define DL20		19
#define DR20		21

static u16 Step15[4] = { R15, D15, DR15, DL15 };
static u16 Step20[4] = { R20, D20, DR20, DL20 };

static u16 ground2[40] = {
	0,  0,  2,  2,  4,  4,  6,  6,  8,  8,
	10, 10, 12, 12, 14, 14, 16, 16, 18, 18,
	20, 20, 22, 22, 24, 24, 26, 26, 28, 28,
	30, 30, 32, 32, 34, 34, 36, 36, 38, 38
};

// Reset the board.
void board_reset(board_t* bd, const u16 rule, const u16 side, const u16 fcolor)
{
	u16 pos, color;

	bd->rule = rule;
	bd->side = side;
	bd->fcolor = fcolor;
	bd->next = fcolor;
	bd->num = 0;
	bd->zkey = 0;
	bd->timeout = false;

	memset(bd->arr, 0, sizeof(bd->arr));
	pattern_reset(pinc(bd));
	mvlist_reset(mstk(bd));
	mvlist_reset(vlast(bd)[0]);
	mvlist_reset(vlast(bd)[1]);
	hash_reset();

	for (pos = 0; pos < 20 * 20; pos++)
	{
		pattern_reset(&bd->pat[pos]);
		mvlist_reset(&bd->mlist[pos]);
		mvlist_reset(&bd->hlist[pos]);
	}	
	memset(bd->spe, 0, sizeof(bd->spe));

	if (bd->side == 20)
	{
		for (pos = 0; pos < 20 * 20; pos++)
		{
			for (color = 0; color < 2; color++)
			{
				bd->seg[pos][0][color].start = rstart20[pos];
				bd->seg[pos][1][color].start = cstart20[pos];
				bd->seg[pos][2][color].start = mdstart20[pos];
				bd->seg[pos][3][color].start = adstart20[pos];
				bd->seg[pos][0][color].end = rstart20[pos] + R20 * 19;
				bd->seg[pos][1][color].end = cstart20[pos] + D20 * 19;
				bd->seg[pos][2][color].end = mdstart20[pos] + DR20 * (mdlen20[pos] - 1);
				bd->seg[pos][3][color].end = adstart20[pos] + DL20 * (adlen20[pos] - 1);
			}
		}
	}
	else if (bd->side == 15)
	{
		for (pos = 0; pos < 15 * 15; pos++)
		{
			for (color = 0; color < 2; color++)
			{
				bd->seg[pos][0][color].start = rstart15[pos];
				bd->seg[pos][1][color].start = cstart15[pos];
				bd->seg[pos][2][color].start = mdstart15[pos];
				bd->seg[pos][3][color].start = adstart15[pos];
				bd->seg[pos][0][color].end = rstart15[pos] + R15 * 14;
				bd->seg[pos][1][color].end = cstart15[pos] + D15 * 14;
				bd->seg[pos][2][color].end = mdstart15[pos] + DR15 * (mdlen15[pos] - 1);
				bd->seg[pos][3][color].end = adstart15[pos] + DL15 * (adlen15[pos] - 1);
			}
		}
	}
}

// Update pat(bd), pinc(bd), bd->arr and spe(bd).
static void pattern_update(board_t* bd, const u16 pos, const u16 color)
{
	u16 c = color;
	u16 oc = opp(color);

	u16 r1 = bd->seg[pos][0][c - 1].start;
	u16 r2 = bd->seg[pos][0][c - 1].end;
	u16 r3 = bd->seg[pos][0][oc - 1].start;
	u16 r4 = bd->seg[pos][0][oc - 1].end;

	u16 c1 = bd->seg[pos][1][c - 1].start;
	u16 c2 = bd->seg[pos][1][c - 1].end;
	u16 c3 = bd->seg[pos][1][oc - 1].start;
	u16 c4 = bd->seg[pos][1][oc - 1].end;

	u16 m1 = bd->seg[pos][2][c - 1].start;
	u16 m2 = bd->seg[pos][2][c - 1].end;
	u16 m3 = bd->seg[pos][2][oc - 1].start;
	u16 m4 = bd->seg[pos][2][oc - 1].end;

	u16 a1 = bd->seg[pos][3][c - 1].start;
	u16 a2 = bd->seg[pos][3][c - 1].end;
	u16 a3 = bd->seg[pos][3][oc - 1].start;
	u16 a4 = bd->seg[pos][3][oc - 1].end;

	pattern_reset(pinc(bd));

	if (bd->side == 20)
	{
		line_update(bd, r1, r20[r2] - r20[r1] + 1, R20, c, SUB);
		line_update(bd, c1, c20[c2] - c20[c1] + 1, D20, c, SUB);
		line_update(bd, m1, md20[m2] - md20[m1] + 1, DR20, c, SUB);
		line_update(bd, a1, ad20[a2] - ad20[a1] + 1, DL20, c, SUB);

		line_update(bd, r3, r20[r4] - r20[r3] + 1, R20, oc, SUB);
		line_update(bd, c3, c20[c4] - c20[c3] + 1, D20, oc, SUB);
		line_update(bd, m3, md20[m4] - md20[m3] + 1, DR20, oc, SUB);
		line_update(bd, a3, ad20[a4] - ad20[a3] + 1, DL20, oc, SUB);
	}
	else if (bd->side == 15)
	{
		line_update(bd, r1, r15[r2] - r15[r1] + 1, R15, c, SUB);
		line_update(bd, c1, c15[c2] - c15[c1] + 1, D15, c, SUB);
		line_update(bd, m1, md15[m2] - md15[m1] + 1, DR15, c, SUB);
		line_update(bd, a1, ad15[a2] - ad15[a1] + 1, DL15, c, SUB);

		line_update(bd, r3, r15[r4] - r15[r3] + 1, R15, oc, SUB);
		line_update(bd, c3, c15[c4] - c15[c3] + 1, D15, oc, SUB);
		line_update(bd, m3, md15[m4] - md15[m3] + 1, DR15, oc, SUB);
		line_update(bd, a3, ad15[a4] - ad15[a3] + 1, DL15, oc, SUB);
	}

	bd->arr[pos] = c;

	if (bd->side == 20)
	{
		line_update(bd, r1, r20[r2] - r20[r1] + 1, R20, c, ADD);
		line_update(bd, c1, c20[c2] - c20[c1] + 1, D20, c, ADD);
		line_update(bd, m1, md20[m2] - md20[m1] + 1, DR20, c, ADD);
		line_update(bd, a1, ad20[a2] - ad20[a1] + 1, DL20, c, ADD);

		line_update(bd, r3, r20[pos] - r20[r3], R20, oc, ADD);
		line_update(bd, c3, c20[pos] - c20[c3], D20, oc, ADD);
		line_update(bd, m3, md20[pos] - md20[m3], DR20, oc, ADD);
		line_update(bd, a3, ad20[pos] - ad20[a3], DL20, oc, ADD);

		line_update(bd, pos + R20, r20[r4] - r20[pos], R20, oc, ADD);
		line_update(bd, pos + D20, c20[c4] - c20[pos], D20, oc, ADD);
		line_update(bd, pos + DR20, md20[m4] - md20[pos], DR20, oc, ADD);
		line_update(bd, pos + DL20, ad20[a4] - ad20[pos], DL20, oc, ADD);
	}
	else if (bd->side == 15)
	{
		line_update(bd, r1, r15[r2] - r15[r1] + 1, R15, c, ADD);
		line_update(bd, c1, c15[c2] - c15[c1] + 1, D15, c, ADD);
		line_update(bd, m1, md15[m2] - md15[m1] + 1, DR15, c, ADD);
		line_update(bd, a1, ad15[a2] - ad15[a1] + 1, DL15, c, ADD);

		line_update(bd, r3, r15[pos] - r15[r3], R15, oc, ADD);
		line_update(bd, c3, c15[pos] - c15[c3], D15, oc, ADD);
		line_update(bd, m3, md15[pos] - md15[m3], DR15, oc, ADD);
		line_update(bd, a3, ad15[pos] - ad15[a3], DL15, oc, ADD);

		line_update(bd, pos + R15, r15[r4] - r15[pos], R15, oc, ADD);
		line_update(bd, pos + D15, c15[c4] - c15[pos], D15, oc, ADD);
		line_update(bd, pos + DR15, md15[m4] - md15[pos], DR15, oc, ADD);
		line_update(bd, pos + DL15, ad15[a4] - ad15[pos], DL15, oc, ADD);
	}

	pattern_add(pat(bd), &bd->pat[bd->num - 1], pinc(bd));
}

// Update pat(bd), pinc(bd) and bd->arr.
static void pattern_update_fast(board_t* bd, const u16 pos, const u16 color)
{
	u16 c = color;
	u16 oc = opp(color);

	u16 r1 = bd->seg[pos][0][c - 1].start;
	u16 r2 = bd->seg[pos][0][c - 1].end;
	u16 r3 = bd->seg[pos][0][oc - 1].start;
	u16 r4 = bd->seg[pos][0][oc - 1].end;

	u16 c1 = bd->seg[pos][1][c - 1].start;
	u16 c2 = bd->seg[pos][1][c - 1].end;
	u16 c3 = bd->seg[pos][1][oc - 1].start;
	u16 c4 = bd->seg[pos][1][oc - 1].end;

	u16 m1 = bd->seg[pos][2][c - 1].start;
	u16 m2 = bd->seg[pos][2][c - 1].end;
	u16 m3 = bd->seg[pos][2][oc - 1].start;
	u16 m4 = bd->seg[pos][2][oc - 1].end;

	u16 a1 = bd->seg[pos][3][c - 1].start;
	u16 a2 = bd->seg[pos][3][c - 1].end;
	u16 a3 = bd->seg[pos][3][oc - 1].start;
	u16 a4 = bd->seg[pos][3][oc - 1].end;

	pattern_reset(pinc(bd));

	if (bd->side == 20)
	{
		line_update_fast(bd, r1, r20[r2] - r20[r1] + 1, R20, c, SUB);
		line_update_fast(bd, c1, c20[c2] - c20[c1] + 1, D20, c, SUB);
		line_update_fast(bd, m1, md20[m2] - md20[m1] + 1, DR20, c, SUB);
		line_update_fast(bd, a1, ad20[a2] - ad20[a1] + 1, DL20, c, SUB);

		line_update_fast(bd, r3, r20[r4] - r20[r3] + 1, R20, oc, SUB);
		line_update_fast(bd, c3, c20[c4] - c20[c3] + 1, D20, oc, SUB);
		line_update_fast(bd, m3, md20[m4] - md20[m3] + 1, DR20, oc, SUB);
		line_update_fast(bd, a3, ad20[a4] - ad20[a3] + 1, DL20, oc, SUB);
	}
	else if (bd->side == 15)
	{
		line_update_fast(bd, r1, r15[r2] - r15[r1] + 1, R15, c, SUB);
		line_update_fast(bd, c1, c15[c2] - c15[c1] + 1, D15, c, SUB);
		line_update_fast(bd, m1, md15[m2] - md15[m1] + 1, DR15, c, SUB);
		line_update_fast(bd, a1, ad15[a2] - ad15[a1] + 1, DL15, c, SUB);

		line_update_fast(bd, r3, r15[r4] - r15[r3] + 1, R15, oc, SUB);
		line_update_fast(bd, c3, c15[c4] - c15[c3] + 1, D15, oc, SUB);
		line_update_fast(bd, m3, md15[m4] - md15[m3] + 1, DR15, oc, SUB);
		line_update_fast(bd, a3, ad15[a4] - ad15[a3] + 1, DL15, oc, SUB);
	}

	bd->arr[pos] = c;

	if (bd->side == 20)
	{
		line_update_fast(bd, r1, r20[r2] - r20[r1] + 1, R20, c, ADD);
		line_update_fast(bd, c1, c20[c2] - c20[c1] + 1, D20, c, ADD);
		line_update_fast(bd, m1, md20[m2] - md20[m1] + 1, DR20, c, ADD);
		line_update_fast(bd, a1, ad20[a2] - ad20[a1] + 1, DL20, c, ADD);

		line_update_fast(bd, r3, r20[pos] - r20[r3], R20, oc, ADD);
		line_update_fast(bd, c3, c20[pos] - c20[c3], D20, oc, ADD);
		line_update_fast(bd, m3, md20[pos] - md20[m3], DR20, oc, ADD);
		line_update_fast(bd, a3, ad20[pos] - ad20[a3], DL20, oc, ADD);

		line_update_fast(bd, pos + R20, r20[r4] - r20[pos], R20, oc, ADD);
		line_update_fast(bd, pos + D20, c20[c4] - c20[pos], D20, oc, ADD);
		line_update_fast(bd, pos + DR20, md20[m4] - md20[pos], DR20, oc, ADD);
		line_update_fast(bd, pos + DL20, ad20[a4] - ad20[pos], DL20, oc, ADD);
	}
	else if (bd->side == 15)
	{
		line_update_fast(bd, r1, r15[r2] - r15[r1] + 1, R15, c, ADD);
		line_update_fast(bd, c1, c15[c2] - c15[c1] + 1, D15, c, ADD);
		line_update_fast(bd, m1, md15[m2] - md15[m1] + 1, DR15, c, ADD);
		line_update_fast(bd, a1, ad15[a2] - ad15[a1] + 1, DL15, c, ADD);

		line_update_fast(bd, r3, r15[pos] - r15[r3], R15, oc, ADD);
		line_update_fast(bd, c3, c15[pos] - c15[c3], D15, oc, ADD);
		line_update_fast(bd, m3, md15[pos] - md15[m3], DR15, oc, ADD);
		line_update_fast(bd, a3, ad15[pos] - ad15[a3], DL15, oc, ADD);

		line_update_fast(bd, pos + R15, r15[r4] - r15[pos], R15, oc, ADD);
		line_update_fast(bd, pos + D15, c15[c4] - c15[pos], D15, oc, ADD);
		line_update_fast(bd, pos + DR15, md15[m4] - md15[pos], DR15, oc, ADD);
		line_update_fast(bd, pos + DL15, ad15[a4] - ad15[pos], DL15, oc, ADD);
	}

	pattern_add(pat(bd), &bd->pat[bd->num - 1], pinc(bd));
}

// Update bd->seg.
static void seg_update(board_t* bd, const u16 pos, const u16 color)
{
	u16 i, j, oc = opp(color);

	if (bd->side == 20)
	{
		for (i = 0; i < 4; i++)
		{
			for (j = bd->seg[pos][i][oc - 1].start; j <= pos - Step20[i]; j += Step20[i])
				bd->seg[j][i][oc - 1].end = pos - Step20[i];

			for (j = pos + Step20[i]; j <= bd->seg[pos][i][oc - 1].end; j += Step20[i])
				bd->seg[j][i][oc - 1].start = pos + Step20[i];
		}
	}
	else if (bd->side == 15)
	{
		for (i = 0; i < 4; i++)
		{
			for (j = bd->seg[pos][i][oc - 1].start; j <= pos - Step15[i]; j += Step15[i])
				bd->seg[j][i][oc - 1].end = pos - Step15[i];

			for (j = pos + Step15[i]; j <= bd->seg[pos][i][oc - 1].end; j += Step15[i])
				bd->seg[j][i][oc - 1].start = pos + Step15[i];
		}
	}
}

// Undo bd->seg.
static void seg_undo(board_t* bd, const u16 pos, const u16 color)
{
	u16 i, j, oc = opp(color);

	if (bd->side == 20)
	{
		for (i = 0; i < 4; i++)
		{
			for (j = bd->seg[pos][i][oc - 1].start; j <= bd->seg[pos][i][oc - 1].end; j += Step20[i])
			{
				bd->seg[j][i][oc - 1].start = bd->seg[pos][i][oc - 1].start;
				bd->seg[j][i][oc - 1].end = bd->seg[pos][i][oc - 1].end;
			}
		}
	}
	else if (bd->side == 15)
	{
		for (i = 0; i < 4; i++)
		{
			for (j = bd->seg[pos][i][oc - 1].start; j <= bd->seg[pos][i][oc - 1].end; j += Step15[i])
			{
				bd->seg[j][i][oc - 1].start = bd->seg[pos][i][oc - 1].start;
				bd->seg[j][i][oc - 1].end = bd->seg[pos][i][oc - 1].end;
			}
		}
	}
}

// Update mlist(bd).
static void mlist_update(board_t* bd, const u16 pos)
{
	int i;

	mvlist_copy(mlist(bd), &bd->mlist[bd->num - 1]);
	mvlist_remove(mlist(bd), pos);

	if (bd->side == 20)
	{
		for (i = 0; i < NEI_SIZE * 8; i++)
		{
			if (Nei20[pos][i] == INVALID)
				break;
			if (bd->arr[Nei20[pos][i]] == EMPTY)
			{
				mvlist_remove(mlist(bd), Nei20[pos][i]);
				mvlist_insert_front(mlist(bd), Nei20[pos][i]);
			}
		}
	}
	else if (bd->side == 15)
	{
		for (i = 0; i < NEI_SIZE * 8; i++)
		{
			if (Nei15[pos][i] == INVALID)
				break;
			if (bd->arr[Nei15[pos][i]] == EMPTY)
			{
				mvlist_remove(mlist(bd), Nei15[pos][i]);
				mvlist_insert_front(mlist(bd), Nei15[pos][i]);
			}
		}
	}
}

// Make a move and update all.
void do_move(board_t* bd, const u16 pos, const u16 color)
{
	bd->num++;
	bd->next = opp(color);
	bd->zkey ^= Zobrist[color][pos];
	mvlist_insert_back(mstk(bd), pos);

	memcpy(spe(bd), bd->spe[bd->num - 1], sizeof(spe(bd)));
	pattern_update(bd, pos, color);
	seg_update(bd, pos, color);
	mlist_update(bd, pos);

	if (pattern_read(pinc(bd), DEAD4, color) > 0 || pattern_read_free3(pinc(bd), color) > 0)
		mvlist_insert_back(vlast(bd)[color - 1], pos);
}

// Make a move fast.
void do_move_fast(board_t* bd, const u16 pos, const u16 color)
{
	bd->num++;
	bd->next = opp(color);
	bd->zkey ^= Zobrist[color][pos];
	mvlist_insert_back(mstk(bd), pos);
	pattern_update_fast(bd, pos, color);
}

// Undo the last move.
void undo(board_t* bd)
{
	u16 pos = mvlist_last(mstk(bd));
	if (bd->num != 0)
	{
		bd->num--;
		bd->arr[pos] = EMPTY;
		bd->next = opp(bd->next);
		bd->zkey ^= Zobrist[bd->next][pos];

		pattern_reset(pinc(bd));
		seg_undo(bd, pos, bd->next);
		mvlist_remove_back(mstk(bd));
		mvlist_remove(vlast(bd)[bd->next - 1], pos);
	}
}

// Undo the last move fast.
void undo_fast(board_t* bd)
{
	u16 pos = mvlist_last(mstk(bd));
	if (bd->num != 0)
	{
		bd->num--;
		bd->arr[pos] = EMPTY;
		bd->next = opp(bd->next);
		bd->zkey ^= Zobrist[bd->next][pos];
		pattern_reset(pinc(bd));
		mvlist_remove_back(mstk(bd));
	}
}

// Strict gameover judgement. Return the win color or DRAW or INVALID.
u16 gameover(const board_t* bd)
{
	if (pattern_read(pat(bd), FIVE, BLACK) > 0)
		return BLACK;

	if (pattern_read(pat(bd), FIVE, WHITE) > 0)
		return WHITE;

	if (bd->num >= bd->side * bd->side)
		return DRAW;

	if (bd->rule == RENJU)
	{
		if (pattern_read(pat(bd), LONG, opp(bd->fcolor)) > 0)
			return opp(bd->fcolor);

		if (pattern_read(pat(bd), LONG, bd->fcolor) > 0)
			return opp(bd->fcolor);

		if (pattern_read(pinc(bd), FREE4, bd->fcolor) + pattern_read(pinc(bd), DEAD4, bd->fcolor) >= 2)
			return opp(bd->fcolor);

		if (pattern_read_free3(pinc(bd), bd->fcolor) >= 2)
			return opp(bd->fcolor);
	}

	return INVALID;
}

// Win/Lose judgement. Return the win color or DRAW or INVALID.
// offset is the remaining # of moves to gameover.
u16 winlose(const board_t* bd, const u16 next, u16* offset)
{
	u16 win = gameover(bd);

	if (win != INVALID)
	{
		*offset = 0;
		return win;
	}
	if (pattern_read(pat(bd), FREE4, next) > 0 || pattern_read(pat(bd), DEAD4, next) > 0)
	{
		*offset = 1;
		return next;
	}
	if (pattern_read(pat(bd), FREE4, opp(next)) > 0 || pattern_read(pat(bd), DEAD4, opp(next)) >= 2)
	{
		*offset = 2;
		return opp(next);
	}
	if (pattern_read_free3(pat(bd), next) > 0 && pattern_read(pat(bd), DEAD4, opp(next)) == 0)
	{
		*offset = 3;
		return next;
	}

	return INVALID;
}

// Add one forced move to win/lose to hlist(bd). Return true if exists.
bool cand_winlose(board_t* bd, const u16 next)
{
	u16 pos;

	// next has free4 or dead4, next choose one position to form five
	if (pattern_read(pat(bd), FREE4, next) > 0 || pattern_read(pat(bd), DEAD4, next) > 0)
	{
		pos = mvlist_first(mlist(bd));
		while (pos != END)
		{
			do_move_fast(bd, pos, next);
			if (pattern_read(pinc(bd), FIVE, next) > 0)
			{
				undo_fast(bd);
				mvlist_insert_back(hlist(bd), pos);
				return true;
			}
			undo_fast(bd);
			pos = mvlist_next(mlist(bd), pos);
		}
	}

	// opp(next) has free4 and next has no four, next choose one position to lose
	if (pattern_read(pat(bd), FREE4, opp(next)) > 0)
	{
		pos = mvlist_first(mlist(bd));
		while (pos != END)
		{
			do_move_fast(bd, pos, next);
			if (pattern_read(pinc(bd), FREE4, opp(next)) < 0)
			{
				undo_fast(bd);
				mvlist_insert_back(hlist(bd), pos);
				return true;
			}
			undo_fast(bd);
			pos = mvlist_next(mlist(bd), pos);
		}
	}

	// next has free3 and both have no four, next choose one position to form free4
	if (pattern_read_free3(pat(bd), next) > 0 && pattern_read(pat(bd), DEAD4, opp(next)) == 0)
	{
		pos = mvlist_first(mlist(bd));
		while (pos != END)
		{
			do_move_fast(bd, pos, next);
			if (bd->rule == RENJU && next == bd->fcolor)
			{
				if (pattern_read(pinc(bd), FREE4, next) == 1 && pattern_read(pinc(bd), DEAD4, next) == 0)
				{
					undo_fast(bd);
					mvlist_insert_back(hlist(bd), pos);
					return true;
				}
			}
			else
			{
				if (pattern_read(pinc(bd), FREE4, next) > 0)
				{
					undo_fast(bd);
					mvlist_insert_back(hlist(bd), pos);
					return true;
				}
			}
			undo_fast(bd);
			pos = mvlist_next(mlist(bd), pos);
		}
	}

	return false;
}

// Add forced moves to hlist(bd). Return true if exists.
bool cand_force(board_t* bd, const u16 next)
{
	u16 pos;

	// opp(next) has dead4 and next has no four, next must defend
	if (pattern_read(pat(bd), DEAD4, opp(next)) > 0)
	{
		pos = mvlist_first(mlist(bd));
		while (pos != END)
		{
			if (check(bd, pos, next, DD4))
			{
				mvlist_insert_back(hlist(bd), pos);
				return true;
			}
			pos = mvlist_next(mlist(bd), pos);
		}
	}

	// opp(next) has free3 and next has no free3 and both have no four, form dead4 or defend
	if (pattern_read_free3(pat(bd), opp(next)) > 0)
	{
		pos = mvlist_first(mlist(bd));
		while (pos != END)
		{
			if (check(bd, pos, next, FD4) || check(bd, pos, next, DF3))
				mvlist_insert_back(hlist(bd), pos);
			pos = mvlist_next(mlist(bd), pos);
		}
		return true;
	}

	return false;
}

// VCF function.
long vcf(board_t* bd, const u16 dep, const u16 attacker, const u16 next, u16* best, long alpha, long beta, const bool isroot)
{
	mvlist_t cand;
	long score;
	u16 tmp, offset, pos, i;
	u16 last = mvlist_last(vlast(bd)[attacker - 1]);

	// win/lose check
	tmp = winlose(bd, next, &offset);
	if (tmp == next)
		return WIN - bd->num - offset;
	else if (tmp == opp(next))
		return LOSE + bd->num + offset;
	else if (tmp == DRAW || dep <= 0)
		return 0;

	mvlist_reset(&cand);

	// opp(next) has dead4, defend
	if (pattern_read(pat(bd), DEAD4, opp(next)) > 0)
	{
		pos = mvlist_first(mlist(bd));
		while (pos != END)
		{
			if (check(bd, pos, next, DD4))
			{
				mvlist_insert_back(&cand, pos);
				break;
			}
			pos = mvlist_next(mlist(bd), pos);
		}
	}

	// opp(next) has no dead4, attacker forms dead4
	else if (next == attacker)
	{
		if (isroot)		// first attack
		{
			pos = mvlist_first(mlist(bd));
			while (pos != END)
			{
				if (check(bd, pos, next, FD4))
					mvlist_insert_back(&cand, pos);
				pos = mvlist_next(mlist(bd), pos);
			}
		}
		else	// continous attack
		{
			if (bd->side == 20)
			{
				for (i = 0; i < NeiD20[last]; i++)
				{
					pos = Vcx20[last][i];
					if (check(bd, pos, next, FD4))
						mvlist_insert_back(&cand, pos);
				}
			}
			else if (bd->side == 15)
			{
				for (i = 0; i < NeiD15[last]; i++)
				{
					pos = Vcx15[last][i];
					if (check(bd, pos, next, FD4))
						mvlist_insert_back(&cand, pos);
				}
			}
		}
	}

	// no valid move, return
	if (mvlist_size(&cand) == 0)
		return 0;

	// recurse for each move
	pos = mvlist_first(&cand);
	while (pos != END)
	{
		if (dep > 1)
			do_move(bd, pos, next);
		else
			do_move_fast(bd, pos, next);

		if (pattern_read(pinc(bd), DEAD4, attacker) > 0)
			last = pos;

		score = -vcf(bd, dep - 1, attacker, opp(next), &tmp, -beta, -alpha, false);

		if (dep > 1)
			undo(bd);
		else
			undo_fast(bd);

		if (score >= beta)
			return beta;

		if (score > alpha)
		{
			alpha = score;
			*best = pos;
		}

		if (next == attacker && alpha > WIN_THRE)	// ignore path length
			break;

		pos = mvlist_next(&cand, pos);
	}

	return alpha;
}

// VCT function.
long vct(board_t* bd, const u16 dep, const u16 attacker, const u16 next, u16* best, long alpha, long beta, const bool isroot)
{
	mvlist_t cand;
	long score;
	u16 tmp, offset, pos, i;
	u16 last = mvlist_last(vlast(bd)[attacker - 1]);

	// win/lose check
	tmp = winlose(bd, next, &offset);
	if (tmp == next)
		return WIN - bd->num - offset;
	else if (tmp == opp(next))
		return LOSE + bd->num + offset;
	else if (tmp == DRAW || dep <= 0)
		return 0;

	mvlist_reset(&cand);

	// opp(next) has dead4, defend
	if (pattern_read(pat(bd), DEAD4, opp(next)) > 0)
	{
		pos = mvlist_first(mlist(bd));
		while (pos != END)
		{
			if (check(bd, pos, next, DD4))
			{
				mvlist_insert_back(&cand, pos);
				break;
			}
			pos = mvlist_next(mlist(bd), pos);
		}
	}

	// opp(next) has free3
	else if (pattern_read_free3(pat(bd), opp(next)) > 0)
	{
		// attacker forms dead4 or defends and forms free3
		if (next == attacker && isroot)		// first attack
		{
			pos = mvlist_first(mlist(bd));
			while (pos != END)
			{
				if (check(bd, pos, next, FD4) || (check(bd, pos, next, FF3) && check(bd, pos, next, DF3)))
					mvlist_insert_back(&cand, pos);
				pos = mvlist_next(mlist(bd), pos);
			}
		}
		else if (next == attacker && !isroot)	// continous attack
		{
			if (bd->side == 20)
			{
				for (i = 0; i < NeiA20[last]; i++)
				{
					pos = Vcx20[last][i];
					if (check(bd, pos, next, FD4) || (check(bd, pos, next, FF3) && check(bd, pos, next, DF3)))
						mvlist_insert_back(&cand, pos);
				}
			}
			else if (bd->side == 15)
			{
				for (i = 0; i < NeiA15[last]; i++)
				{
					pos = Vcx15[last][i];
					if (check(bd, pos, next, FD4) || (check(bd, pos, next, FF3) && check(bd, pos, next, DF3)))
						mvlist_insert_back(&cand, pos);
				}
			}
		}
		else	// defend
		{
			// try vcf
			score = vcf(bd, ground2[dep + 1], opp(attacker), opp(attacker), &pos, LOSE - 1, WIN + 1, true);
			if (score > WIN_THRE)
				return sign(-score, attacker, next);

			// form dead4 in last attack position's neighbors
			if (bd->side == 20)
			{
				for (i = 0; i < NeiA20[last]; i++)
				{
					pos = Vcx20[last][i];
					if (check(bd, pos, next, FD4))
						mvlist_insert_back(&cand, pos);
				}
			}
			else if (bd->side == 15)
			{
				for (i = 0; i < NeiA15[last]; i++)
				{
					pos = Vcx15[last][i];
					if (check(bd, pos, next, FD4))
						mvlist_insert_back(&cand, pos);
				}
			}

			// defend free3
			pos = mvlist_first(mlist(bd));
			while (pos != END)
			{
				if (check(bd, pos, next, DF3))
					mvlist_insert_back(&cand, pos);
				pos = mvlist_next(mlist(bd), pos);
			}
		}
	}

	// opp(next) has neither dead4 nor free3, attacker forms dead4 or free3
	else if (next == attacker)
	{
		if (isroot)		// first attack
		{
			pos = mvlist_first(mlist(bd));
			while (pos != END)
			{
				if (check(bd, pos, next, FD4) || check(bd, pos, next, FF3))
					mvlist_insert_back(&cand, pos);
				pos = mvlist_next(mlist(bd), pos);
			}
		}
		else if (!isroot)	// continous attack
		{
			if (bd->side == 20)
			{
				for (i = 0; i < NeiA20[last]; i++)
				{
					pos = Vcx20[last][i];
					if (check(bd, pos, next, FD4) || check(bd, pos, next, FF3))
						mvlist_insert_back(&cand, pos);
				}
			}
			else if (bd->side == 15)
			{
				for (i = 0; i < NeiA15[last]; i++)
				{
					pos = Vcx15[last][i];
					if (check(bd, pos, next, FD4) || check(bd, pos, next, FF3))
						mvlist_insert_back(&cand, pos);
				}
			}
		}
	}

	// no valid move
	if (mvlist_size(&cand) == 0)
		return 0;

	// recurse for each move
	pos = mvlist_first(&cand);
	while (pos != END)
	{
		if (dep > 1)
			do_move(bd, pos, next);
		else
			do_move_fast(bd, pos, next);

		if (pattern_read(pinc(bd), DEAD4, attacker) > 0 || pattern_read_free3(pinc(bd), attacker) > 0)
			last = pos;

		score = -vct(bd, dep - 1, attacker, opp(next), &tmp, -beta, -alpha, false);

		if (dep > 1)
			undo(bd);
		else
			undo_fast(bd);

		if (score >= beta)
			return beta;

		if (score > alpha)
		{
			alpha = score;
			*best = pos;
		}

		if (next == attacker && alpha > WIN_THRE)	// ignore path length
			break;

		pos = mvlist_next(&cand, pos);
	}

	return alpha;
}

// Return the next search time limit.
double get_time(const board_t* bd)
{
	if (bd->time_left > bd->timeout_match * (1 - TIME_DIV_PCT))
		return min_value_double(bd->timeout_match / TIME_DIVIDER1, bd->timeout_turn);
	else
		return min_value_double(bd->time_left / TIME_DIVIDER2, bd->timeout_turn);
}
