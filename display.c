/*      _____                __    ______
 *     / ___ \              / /   /___  /
 *    / /__/ /___  ____  __/ /_______/ /    ____  ____
 *   / _____/ __ \/ __ \/_   _/ __  / /    / __ \/ __ \
 *  / /    /  ___/ / / / / /_/ /_/ / /____/  ___/ / / /
 * /_/     \____/_/ /_/ /___/\__,_/______/\____/_/ /_/
 *
 * PentaZen, a Gomoku/Renju playing engine developed by Sun.
 */

#include "display.h"
#include "search.h"

// Print mvlist.
void print_mvlist(const Mvlist* ml)
{
    printf("%d moves: ", ml->size);

    for (u16 i = 0; i < ml->cut; i++)
        printf("(%d, %d) ", ml->list[i].pos, ml->list[i].val);

    // for (u16 i = 0; i < ml->cut; i++)
    //     printf("%d  ", ml->list[i].pos);

    putchar('\n');
}

// Print pattern.
void print_pattern(const Pattern* pat)
{
    printf("black\t\twhite\n");

    printf("C6: %d\t\t", pat_read(pat, BLACK, C6));
    printf("C6: %d\n", pat_read(pat, WHITE, C6));

    printf("C5: %d\t\t", pat_read(pat, BLACK, C5));
    printf("C5: %d\n", pat_read(pat, WHITE, C5));

    printf("F4: %d\t\t", pat_read(pat, BLACK, F4));
    printf("F4: %d\n", pat_read(pat, WHITE, F4));

    printf("B4: %d\t\t", pat_read(pat, BLACK, B4));
    printf("B4: %d\n", pat_read(pat, WHITE, B4));

    printf("F3: %d\t\t", pat_read(pat, BLACK, F3));
    printf("F3: %d\n", pat_read(pat, WHITE, F3));

    printf("B3: %d\t\t", pat_read(pat, BLACK, B3));
    printf("B3: %d\n", pat_read(pat, WHITE, B3));

    printf("F2: %d\t\t", pat_read(pat, BLACK, F2));
    printf("F2: %d\n", pat_read(pat, WHITE, F2));
}

void print_board(Board* bd)
{
    generate_forced(bd);

#if DISPLAY_BOARD
    Pos pos;

    for (u8 i = 0; i < BD_SIDE + 1; i++)
        printf("====");
    printf("\n");

    for (u8 r = 0; r < BD_SIDE; r++)
    {
        printf("%2d ", r);
        for (u8 f = 0; f < BD_SIDE; f++)
        {
            pos = r * BD_SIDE + f;
            if (f != 0)
                printf("-");
            if (bd->board[pos] == BLACK)
                printf("●");
            else if (bd->board[pos] == WHITE)
                printf("○");
#if DISPLAY_DEFEND
            else if (pos == b4d(bd))
                printf("dd");
#endif
#if DISPLAY_ATTACK
            else if (check(bd, WHITE, B4a, pos))
                printf("aa");
#endif
            else if (r == 0 && f == 0)
                printf("┌-");
            else if (r == 0 && f == BD_SIDE - 1)
                printf("┐");
            else if (r == BD_SIDE - 1 && f == 0)
                printf("└-");
            else if (r == BD_SIDE - 1 && f == BD_SIDE - 1)
                printf("┘");
            else if (r == 0)
                printf("┬-");
            else if (f == 0)
                printf("├-");
            else if (r == BD_SIDE - 1)
                printf("┴-");
            else if (f == BD_SIDE - 1)
                printf("┤");
            else
                printf("┼-");
            if (f != BD_SIDE - 1)
                printf("-");
        }

        if (r != BD_SIDE - 1)
        {
            printf("\n");
            for (u8 i = 0; i < BD_SIDE; i++)
                printf("   │");
            printf("\n");
        }
    }
    printf("\n ");

    for (u8 f = 0; f < BD_SIDE; f++)
        printf("%4d", f);
    printf("\n");

    for (u8 i = 0; i < BD_SIDE + 1; i++)
        printf("====");
    printf("\n");
#endif

#if DISPLAY_ZOBRIST
    printf("%llu\n", bd->ttkey);
#endif

#if DISPLAY_MLIST
    printf("%d moves: ", num(bd));
    for (u16 i = 0; i < num(bd); i++)
        printf("(%d, %d) ", bd->mlist[i] % BD_SIDE, bd->mlist[i] / BD_SIDE);
    putchar('\n');
#endif
    
#if DISPLAY_CAND
    print_mvlist(pcand(bd));
#endif

#if DISPLAY_PATTERN
    print_pattern(ppat(bd));
#endif

#if DISPLAY_SCORE
    u8 offset;
    if (check_wld(bd, &offset) != COL_INVALID)
        printf("wld = %d\n", check_wld(bd, &offset));
    else
        printf("sc: %d\t\tsc: %d\n", score(bd)[0], score(bd)[1]);
#endif

#if DISPLAY_COLOR
    if (bd->fcolor == BLACK)
        printf("fcolor is BLACK. ");
    else if (bd->fcolor == WHITE)
        printf("fcolor is WHITE. ");
    if (bd->next == BLACK)
        printf("next is BLACK.\n");
    else if (bd->next == WHITE)
        printf("next is WHITE.\n");
#endif
}

void print_message(const Board* bd, const u16 dep, const u16 dep_max, const Score ev, const u32 n, const Time tm)
{
    printf("MESSAGE Dep %2d-%2d    ", dep, dep_max);

    if (ev != VAL_INVALID)
    {
        if (ev > VAL_VTHRE)
            printf("Ev +V%d    ", VAL_V - ev - bd->stone_ini);
        else if (ev < -VAL_VTHRE)
            printf("Ev -V%d    ", VAL_V + ev - bd->stone_ini);
        else if (ev >= 0)
            printf("Ev +%d    ", ev);
        else if (ev < 0)
            printf("Ev %d    ", ev);
    }

    if (n > 0)
    {
        if (n >= 1000000000)
            printf("Nd %3dg    ", n / 1000000000);
        else if (n >= 1000000)
            printf("Nd %3dm    ", n / 1000000);
        else if (n >= 1000)
            printf("Nd %3dk    ", n / 1000);
        else
            printf("Nd %3d     ", n);
    }

#if ITERATIVE_DEEPENING
    if (tm > 0)
        printf("Tm %d    Sp %d", tm, (u32)((double)n / (double)tm));
#endif

    putchar('\n');
    fflush(stdout);
}
