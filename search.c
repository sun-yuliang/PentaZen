/*      _____                __    ______
 *     / ___ \              / /   /___  /
 *    / /__/ /___  ____  __/ /_______/ /    ____  ____
 *   / _____/ __ \/ __ \/_   _/ __  / /    / __ \/ __ \
 *  / /    /  ___/ / / / / /_/ /_/ / /____/  ___/ / / /
 * /_/     \____/_/ /_/ /___/\__,_/______/\____/_/ /_/
 *
 * PentaZen, a Gomoku/Renju playing engine developed by Sun.
 */

#include "search.h"
#include "board.h"
#include "display.h"

// set threshold 99.7%
static const u16 BranchNumber[32] = {
     0,  0,  8, 15, 21, 27, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32
};

// set threshold 99.7%
static const Score CutThreshold[32] = {
     0,  0, 25, 38, 42, 47, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
    50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50
};

// set threshold 99.7%
static const Score FutilityMargin[FUTILITY_DEPTH_MAX + 1] = {
    0, 47, 62, 88, 92, 143, 151
};

// Return the evaluation of the current board for bd->next.
Score evaluate(const Board* bd)
{
    Score ev1, ev2, ev3, ev4;

    ev1 = score(bd)[bd->next - 1];
    ev2 = score(bd)[opp(bd->next) - 1];
    ev3 = bd->sc[num(bd) - 1][bd->next - 1];
    ev4 = bd->sc[num(bd) - 1][opp(bd->next) - 1];
    
    return (ev1 - ev2 + ev3 - ev4) / 2;
}

// Alpha-Beta search.
static Score alphabeta(Board* bd, Pos* best, const u16 dep, Score alpha, Score beta)
{
    Pos tmp;
    u16 dep_new = dep - 1;

    // Check timeout.
    if (bd->timeout)
        return bd->next == bd->me ? -VAL_TIMEOUT : VAL_TIMEOUT;
    else
    {
        bd->t_end = get_ms();
        if (bd->t_end - bd->t_begin >= bd->tlimit)
        {
            bd->timeout = true;
            return bd->next == bd->me ? -VAL_TIMEOUT : VAL_TIMEOUT;
        }
    }

    // Update search info.
    bd->node_cnt++;
    if (num(bd) > bd->stone_max)
        bd->stone_max = num(bd);
    
    // Check WLD.
    u8 offset = 0;
    Color wld = check_wld(bd, &offset);

    if (wld == bd->next)
        return VAL_V - num(bd) - offset;
    else if (wld == opp(bd->next))
        return -VAL_V + num(bd) + offset;
    else if (wld == DRAW)
        return VAL_DRAW;
    else if (dep <= 0)
        return evaluate(bd);

    // Victory distance pruning.
    alpha = MyMax(-VAL_V + num(bd), alpha);
    beta = MyMin(VAL_V - num(bd) - 1, beta);
    if (alpha >= beta)
        return alpha;

    // Generate forced moves.
    bool forced = generate_forced(bd);
    if (cand(bd).cut == 1)
    {
        dep_new++;
        goto RECURSE;
    }

    // Static evaluation.
    Score evs = evaluate(bd);

#if EXTENDED_FUTILITY
    // Extended futility pruning.
    if (dep_new + 1 <= FUTILITY_DEPTH_MAX)
    {
        if (evs + FutilityMargin[dep_new + 1] <= alpha)
            return alpha;
        if (evs - FutilityMargin[dep_new + 1] >= beta)
            return beta;
    }
#endif

    if (dep_new <= 0)
        goto RECURSE;

    // Sort moves.
    for (u16 i = 0; i < cand(bd).cut; i++)
    {
        do_move_f(bd, cand(bd).list[i].pos);
        bd->cand[num(bd) - 1].list[i].val = -alphabeta(bd, &tmp, 0, -VAL_V, VAL_V);
        undo_f(bd);
    }
    mvlist_sort(pcand(bd));

#if BRANCH_NUMBER_LIMIT
    // Prune by branch number limitation (may cause inaccurate evaluation).
    if (!forced)
        cand(bd).cut = MyMin(cand(bd).cut, BranchNumber[dep_new + 1]);
#endif

#if SCORE_DIFFERENCE_CUT
    // Prune by score difference (may cause inaccurate evaluation).
    if (!forced)
    {
        for (u16 i = 0; i < cand(bd).cut; i++)
        {
            if (cand(bd).list[0].val - cand(bd).list[i].val > CutThreshold[dep_new + 1])
            {
                cand(bd).cut = i;
                break;
            }
        }
    }
#endif

    // If root node, place the last best move at the front of the move list.
    // Must do this when using iterative deepening.
    if (num(bd) == bd->stone_ini)
    {
        for (u16 i = 0; i < cand(bd).size; i++)
        {
            if (cand(bd).list[i].pos == bd->best)
            {
                mvlist_swap(pcand(bd), 0, i);
                if (i >= cand(bd).cut)
                    mvlist_swap(pcand(bd), cand(bd).cut++, i);
                break;
            }
        }
    }

    // Recurse for each move.
    Score ev;
RECURSE:
    for (u16 i = 0; i < cand(bd).cut; i++)
    {
        if (dep_new == 0)
        {
            do_move_f(bd, cand(bd).list[i].pos);
            ev = -alphabeta(bd, &tmp, dep_new, -beta, -alpha);
            undo_f(bd);
        }
        else
        {
            do_move(bd, cand(bd).list[i].pos);
            ev = -alphabeta(bd, &tmp, dep_new, -beta, -alpha);
            undo(bd);
        }
        
        if (bd->timeout)
            return alpha;

        if (ev >= beta)
            return beta;
        if (ev > alpha)
        {
            *best = cand(bd).list[i].pos;
            alpha = ev;
        }
        if (ev >= VAL_VTHRE)
        {
            *best = cand(bd).list[i].pos;
            return ev;
        }
    }

    return alpha;
}

// Iterative deepening.
static Score iterative(Board* bd, Pos* best, u16* dep)
{
    Mvlist ml;
    Pos best_tmp;
    Score ev_tmp, ev = VAL_INVALID;
    Score ev_arr[SEARCH_DEPTH_MAX];
    bool flag;

    bd->best = POS_INVALID;
    mvlist_copy(&ml, pcand(bd));    // save initial mvlist

    for (*dep = 1; *dep <= SEARCH_DEPTH_MAX; *dep += 1)
    {
        best_tmp = POS_INVALID;
        mvlist_copy(pcand(bd), &ml);    // restore mvlist
        ev_tmp = alphabeta(bd, &best_tmp, *dep, -VAL_V, VAL_V);

        if (bd->timeout)
        {
            if (best_tmp != POS_INVALID)
            {
                *best = best_tmp;
                ev = ev_tmp;
            }
            break;
        }
        else
        {
            *best = best_tmp;
            bd->best = best_tmp;
            ev = ev_tmp;
            ev_arr[*dep - 1] = ev;

#if ITERATIVE_INFO
            bd->t_end = get_ms();
            print_message(bd, *dep, bd->stone_max - bd->stone_ini, ev, bd->node_cnt, bd->t_end - bd->t_begin);
#endif

            bd->t_end = get_ms();
            if ((double)(bd->t_end - bd->t_begin) > (double)bd->tlimit* ITERATIVE_STOP_PCT)
                break;

            // if victory move is found
            flag = true;
            if (*dep > VICTORY_REPEAT - 1)
            {    
                for (u16 i = 0; i < VICTORY_REPEAT; i++)
                    if (abs(ev_arr[*dep - 1 - i]) < VAL_VTHRE)
                        flag = false;

                if (flag)
                    break;
            }
        }

    }

    *dep = MyMin(*dep, SEARCH_DEPTH_MAX);
    return ev;
}

Pos get_best(Board* bd)
{
    Pos pos;

    // Initialize time info.
    bd->t_begin = get_ms();
    bd->timeout = false;

    // Initialize search info.
    bd->me = bd->next;
    bd->stone_ini = num(bd);
    bd->stone_max = num(bd);
    bd->node_cnt = 0;
    
    // Check if first move.
    if (num(bd) == 0)
    {
#if SEARCH_INFO
        print_message(bd, 1, 1, VAL_INVALID, 0, 0);
#endif
        return BD_MID;
    }

    // Check immediate WLD.
    pos = generate_wld(bd);
    if (pos != COL_INVALID)
    {
#if SEARCH_INFO
        u8 offset;
        if (check_wld(bd, &offset) == bd->next)
            print_message(bd, 1, 1, VAL_V - num(bd) - offset, 0, 0);
        else
            print_message(bd, 1, 1, -VAL_V + num(bd) + offset, 0, 0);
#endif
        return pos;
    }

    // Check unique forced move.
    generate_forced(bd);
    if (cand(bd).cut == 1)
    {
#if SEARCH_INFO
        print_message(bd, 1, 1, VAL_INVALID, 0, 0);
#endif
        return cand(bd).list[0].pos;
    }

#if ITERATIVE_DEEPENING
    // Iterative alpha-beta search.
    bd->tlimit = get_turn_time(bd);
    Score ev = iterative(bd, &pos, &bd->search_dep);
#else
    bd->search_dep = 6;
    bd->tlimit = 864000000;
    Score ev = alphabeta(bd, &pos, bd->search_dep, -VAL_V, VAL_V);
#endif

#if SEARCH_INFO
    bd->t_end = get_ms();
    print_message(bd, bd->search_dep, bd->stone_max - bd->stone_ini, ev, bd->node_cnt, bd->t_end - bd->t_begin);
#endif
    
#if MESSAGE_STAT
    Pos ptmp;
    Score ev0 = VAL_INVALID, ev1 = VAL_INVALID;

    for (u16 i = 0; i < cand(bd).cut; i++)
    {
        do_move(bd, cand(bd).list[i].pos);
        bd->cand[num(bd) - 1].list[i].val = -alphabeta(bd, &ptmp, 0, -VAL_V, VAL_V);
        undo(bd);

        if (cand(bd).list[i].pos == pos)
            ev1 = cand(bd).list[i].val; // heuristic ev of the true best pos in mvlist
    }
    mvlist_sort(pcand(bd));

    // BranchNumber
    // for (u16 i = 0; i < cand(bd).cut; i++)
    // {
    //     if (cand(bd).list[i].pos == pos)
    //     {
    //         printf("MESSAGE %d\n", i);
    //         break;
    //     }
    // }
    
    // CutThreshold
    // ev0 = cand(bd).list[0].val;  // largest ev in mvlist
    // printf("MESSAGE %d\n", ev0 - ev1);

    // FutilityMargin
    ev0 = evaluate(bd); // static ev
    if (0 <= abs(ev - ev0) && abs(ev - ev0) < VAL_VTHRE)
        printf("MESSAGE %d\n", abs(ev - ev0));
#endif

    assert(ispos(pos));

    if (bd->board[pos] == EMPTY)
        return pos;
    else
    {
        printf("MESSAGE Error\n");
        for (u16 i = 0; i < BD_SIZE; i++)
            if (bd->board[i] == EMPTY)
                return i;
    }

    assert(0);
    return POS_INVALID;
}
