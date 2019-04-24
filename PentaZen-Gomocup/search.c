/*
 *  PentaZen Gomoku/Renju Engine
 *                    _______
 *                   / _____/
 *  developed by    / /______  ________
 *                 /____  / / / / __  /
 *                _____/ / /_/ / / / /
 *  2019.04      /______/_____/_/ /_/
 *
 * search.c - search functions
 */

#include "search.h"
#include "public.h"
#include "mvlist.h"
#include "pair.h"
#include "board.h"
#include "table.h"
#include "utility.h"
#include "hash.h"

static clock_t OriginTime, CurrentTime;		// timestamp

// Return the score of the board for color without considering win/lose.
static long evaluate(const board_t* bd, const score_t* sc, const u16 color)
{
	long score = 0;
	u16 oppcolor = opp(color);

	score += sc->dead4 * pattern_read(pat(bd), DEAD4, color);
	score += sc->free3a * pattern_read(pat(bd), FREE3a, color);
	score += sc->free3 * pattern_read(pat(bd), FREE3, color);
	score += sc->dead3 * pattern_read(pat(bd), DEAD3, color);
	score += sc->free2a * pattern_read(pat(bd), FREE2a, color);
	score += sc->free2 * pattern_read(pat(bd), FREE2, color);
	score += sc->dead2 * pattern_read(pat(bd), DEAD2, color);
	score += sc->free1a * pattern_read(pat(bd), FREE1a, color);
	score += sc->free1 * pattern_read(pat(bd), FREE1, color);
	score += sc->dead1 * pattern_read(pat(bd), DEAD1, color);

	score -= sc->dead4 * pattern_read(pat(bd), DEAD4, oppcolor);
	score -= sc->free3a * pattern_read(pat(bd), FREE3a, oppcolor);
	score -= sc->free3 * pattern_read(pat(bd), FREE3, oppcolor);
	score -= sc->dead3 * pattern_read(pat(bd), DEAD3, oppcolor);
	score -= sc->free2a * pattern_read(pat(bd), FREE2a, oppcolor);
	score -= sc->free2 * pattern_read(pat(bd), FREE2, oppcolor);
	score -= sc->dead2 * pattern_read(pat(bd), DEAD2, oppcolor);
	score -= sc->free1a * pattern_read(pat(bd), FREE1a, oppcolor);
	score -= sc->free1 * pattern_read(pat(bd), FREE1, oppcolor);
	score -= sc->dead1 * pattern_read(pat(bd), DEAD1, oppcolor);

	return score;
}

// Return the score of a point without considering win/lose or forced moves.
static long evaluate_point(const board_t* bd, const score_t* sc, const u16 color, const u16 pos)
{
	long score = 0;
	u16 oppcolor = opp(color);

	score += sc->dead4 * count(bd, pos, color, FD4);
	score += sc->free3 * count(bd, pos, color, FF3);
	score += sc->dead3 * (count(bd, pos, color, FD3) + count(bd, pos, oppcolor, FD4));
	score += sc->free2 * (count(bd, pos, color, FF2) + count(bd, pos, color, DF2));
	score += (sc->free2 - sc->dead2) * count(bd, pos, color, DF2P);

	return score;
}

// Generate candidate moves in hlist(bd) by greedy algorithm.
void cand(board_t* bd, const search_t* srh, const u16 color)
{
	pair_t pair;
	u16 pos, i;

	mvlist_reset(hlist(bd));
	pair_reset(&pair);

	if (cand_force(bd, color))
		return;

	pos = mvlist_first(mlist(bd));
	while (pos != END)
	{
		do_move_fast(bd, pos, color);
		pair_insert(&pair, pos, evaluate(bd, &srh->sc, color));
		undo_fast(bd);
		pos = mvlist_next(mlist(bd), pos);
	}

	pair_sort(&pair);
	for (i = 0; i < min_value(srh->htleaf, pair.size); i++)
		mvlist_insert_back(hlist(bd), pair.arr[i].pos);

	// query the hash table for the best move
	pos = hash_query_best(bd->zkey);
	if (pos != INVALID && bd->arr[pos] == EMPTY)
	{
		mvlist_remove(hlist(bd), pos);
		mvlist_insert_front(hlist(bd), pos);
	}
}

// Generate candidate moves in hlist(bd) by alpha-beta search.
void cand_root(board_t* bd, const search_t* srh, const u16 dep)
{
	pair_t pair;
	u16 pos, i;

	mvlist_reset(hlist(bd));
	pair_reset(&pair);

	if (cand_force(bd, srh->me))
		return;

	pos = mvlist_first(mlist(bd));
	while (pos != END)
	{
		do_move(bd, pos, srh->me);
		pair_insert(&pair, pos, -alphabeta(bd, srh, dep - 1, opp(srh->me), &i, LOSE - 1, WIN + 1, false, 0));
		undo(bd);
		pos = mvlist_next(mlist(bd), pos);
	}

	pair_sort(&pair);
	for (i = 0; i < min_value(srh->hpleaf, pair.size); i++)
		mvlist_insert_back(hlist(bd), pair.arr[i].pos);
}

// Alpha-Beta search.
long alphabeta(board_t* bd, const search_t* srh, const u16 dep, const u16 next, u16* best,
							long alpha, long beta, const bool isroot, const double tlimit)
{
	long score;
	u16 tmp, offset, pos;
	u16 htype = HASH_ALPHA;
	
	// timeout check
	if (tlimit != 0)
	{
		CurrentTime = clock();
		if (CurrentTime - OriginTime >= tlimit * CLOCKS_PER_SEC)
		{
			bd->timeout = true;
			return sign(LOSE, srh->me, next);
		}
	}

	// query the hash table
	if (!isroot)
		if ((score = hash_query_alphabeta(bd->zkey, bd->num + dep, alpha, beta)) != HASH_SCORE_INV)
			return score;

	// win/lose check and evaluation
	tmp = winlose(bd, next, &offset);
	if (tmp == next)
		return WIN - bd->num - offset;
	else if (tmp == opp(next))
		return LOSE + bd->num + offset;
	else if (tmp == DRAW)
		return 0;
	else if (dep <= 0)
		return evaluate(bd, &srh->sc, next);

	// heuristic generate
	if (dep > 1)
	{
		if (!isroot)
			cand(bd, srh, next);
		pos = mvlist_first(hlist(bd));
	}
	else
		pos = mvlist_first(mlist(bd));

	// recurse for each move
	if (isroot)
		pair_reset(pair(bd));

	while (pos != END)
	{
		if (dep > 1)
			do_move(bd, pos, next);
		else
			do_move_fast(bd, pos, next);

		score = -alphabeta(bd, srh, dep - 1, opp(next), &tmp, -beta, -alpha, false, tlimit);
		
		if (isroot)
			pair_insert(pair(bd), pos, score);
		
		if (dep > 1)
			undo(bd);
		else
			undo_fast(bd);

		if (score >= beta)
		{
			if (!bd->timeout)
				hash_record(bd->zkey, beta, bd->num + dep, HASH_BETA, pos);
			return beta;
		}

		if (score > alpha)
		{
			htype = HASH_EXACT;
			alpha = score;
			*best = pos;
		}

		if (dep > 1)
			pos = mvlist_next(hlist(bd), pos);
		else
			pos = mvlist_next(mlist(bd), pos);
	}
	
	if (!bd->timeout)
		hash_record(bd->zkey, alpha, bd->num + dep, htype, *best);
	return alpha;
}

// Iterative deepening for alpha-beta search.
static long alphabeta_iterate(board_t* bd, const search_t* srh, u16* best, u16* dep, const double tlimit)
{
	long score = 0;
	u16 depth, tmp;

	OriginTime = clock();
	bd->timeout = false;

	for (depth = srh->mindep; depth <= srh->maxdep; depth += 2)
	{
		alphabeta(bd, srh, depth, srh->me, &tmp, LOSE - 1, WIN + 1, true, tlimit);

		if (bd->pair.size == 0)
			break;
		else
		{
			pair_sort(pair(bd));
			*best = bd->pair.arr[0].pos;
			score = bd->pair.arr[0].key;
			pair_to_mvlist(hlist(bd), pair(bd));

			CurrentTime = clock();
			if (CurrentTime - OriginTime > tlimit * CLOCKS_PER_SEC * ITE_STOP_PCT)
				break;
		}
	}

	*dep = min_value(depth, srh->maxdep);
	return score;
}

// Remove VCF lose points from hlist(bd). Keep hlist(bd) unchanged if all moves lose.
static void vcf_filter(board_t* bd, search_t* srh, const u16 dep)
{
	mvlist_t mvtmp;
	u16 pos, tmp;

	mvlist_reset(&mvtmp);
	mvlist_copy(&mvtmp, hlist(bd));
	pos = mvlist_first(hlist(bd));

	srh->me = opp(srh->me);
	while (pos != END)
	{
		do_move(bd, pos, opp(srh->me));
		if (vcf(bd, dep, srh->me, srh->me, &tmp, LOSE - 1, WIN + 1, true) > WIN_THRE)
			mvlist_remove(&mvtmp, pos);
		undo(bd);
		pos = mvlist_next(hlist(bd), pos);
	}
	srh->me = opp(srh->me);

	if (mvlist_size(&mvtmp) > 0)
		mvlist_copy(hlist(bd), &mvtmp);
	else
	{
		if (srh->me == BLACK)
			printf("MESSAGE White VCF!\n");
		else if (srh->me == WHITE)
			printf("MESSAGE Black VCF!\n");
	}
}

// Remove VCT lose points from hlist(bd). Keep hlist(bd) unchanged if all moves lose.
static void vct_filter(board_t* bd, search_t* srh, const u16 dep)
{
	mvlist_t mvtmp;
	u16 pos, tmp;

	mvlist_reset(&mvtmp);
	mvlist_copy(&mvtmp, hlist(bd));
	pos = mvlist_first(hlist(bd));

	srh->me = opp(srh->me);
	while (pos != END)
	{
		do_move(bd, pos, opp(srh->me));
		if (vct(bd, dep, srh->me, srh->me, &tmp, LOSE - 1, WIN + 1, true) > WIN_THRE)
			mvlist_remove(&mvtmp, pos);
		undo(bd);
		pos = mvlist_next(hlist(bd), pos);
	}
	srh->me = opp(srh->me);

	if (mvlist_size(&mvtmp) > 0)
		mvlist_copy(hlist(bd), &mvtmp);
	else
	{
		if (srh->me == BLACK)
			printf("MESSAGE White VCT!\n");
		else if (srh->me == WHITE)
			printf("MESSAGE Black VCT!\n");
	}
}

// Return the best position for srh->me.
u16 get_best(board_t* bd, search_t* srh)
{
	double tlimit = 0;
	u16 pos, dep;

	OriginTime = clock();
	mvlist_reset(hlist(bd));

	// the first move
	if (bd->num == 0)
	{
		if (bd->side == 15)
			return 112;
		else if (bd->side == 20)
			return 210;
	}

	// if win/lose is confirmed
	if (cand_winlose(bd, srh->me))
		return mvlist_first(hlist(bd));

	// pre-search to generate hlist(bd)
	cand_root(bd, srh, 2);
	dep = 2;

	// single move
	if (mvlist_size(hlist(bd)) <= 1)
		return mvlist_first(hlist(bd));

#if VCF_SEARCH
	// VCF search
	if (vcf(bd, srh->vcfdep, srh->me, srh->me, &pos, LOSE - 1, WIN + 1, true) > WIN_THRE)
	{
		if (srh->me == BLACK)
			printf("MESSAGE Black VCF!\n");
		else if (srh->me == WHITE)
			printf("MESSAGE White VCF!\n");
		return pos;
	}

	// remove VCF lose points
	//vcf_filter(bd, srh, srh->vcfdep - 2);
#endif

#if VCT_SEARCH
	// VCT search
	if (vct(bd, srh->vctdep, srh->me, srh->me, &pos, LOSE - 1, WIN + 1, true) > WIN_THRE)
	{
		if (srh->me == BLACK)
			printf("MESSAGE Black VCT!\n");
		else if (srh->me == WHITE)
			printf("MESSAGE White VCT!\n");
		return pos;
	}

	// remove VCT lose points
	vct_filter(bd, srh, srh->vctdep - 2);
#endif

#if ITERATIVE
	// iterative deepening for alpha-beta search
	CurrentTime = clock();
	tlimit = get_time(bd) - (double)(CurrentTime - OriginTime) / CLOCKS_PER_SEC - SECOND_RSV;
	if (tlimit > 0)
		alphabeta_iterate(bd, srh, &pos, &dep, tlimit);
	else
		return mvlist_first(hlist(bd));
	printf("MESSAGE Alpha-Beta search %d ply\n", dep);
#else
	alphabeta(bd, srh, 6, srh->me, &pos, LOSE - 1, WIN + 1, true, 0);
#endif

	return pos;
}
