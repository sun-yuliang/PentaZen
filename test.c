/*      _____                __    ______
 *     / ___ \              / /   /___  /
 *    / /__/ /___  ____  __/ /_______/ /    ____  ____
 *   / _____/ __ \/ __ \/_   _/ __  / /    / __ \/ __ \
 *  / /    /  ___/ / / / / /_/ /_/ / /____/  ___/ / / /
 * /_/     \____/_/ /_/ /___/\__,_/______/\____/_/ /_/
 *
 * PentaZen, a Gomoku/Renju playing engine developed by Sun.
 */

#include "test.h"
#include "public.h"
#include "distance.h"
#include "mvlist.h"
#include "pattab.h"
#include "display.h"

// Output the binary of a u32 number in N bits.
// e.g. If num = 15 and N = 16, print 0000000000001111.
void print_binary(const u32 num, const u8 N)
{
    for (u8 i = 0; i < N; i++)
    {
        if ((num >> (N - 1 - i)) & 0x1)
            putchar('1');
        else
            putchar('0');
    }
}

// Print pattern table.
static void print_pattab()
{
    for (u32 i = 0; i < IIE15; i++)
    {
        print_binary(i, 15);
        printf("\t\t%3d %3d %3d\t", Pat15[i][0], Pat15[i][1], Pat15[i][2]);

        for (u8 j = PAT_START; j < AD_START; j++)
        {
            if (Pat15[i][j] == COL_INVALID)
                printf("__ ");
            else
            {
                switch (Pat15[i][j])
                {
                case C6:
                    printf("C6 ");
                    break;
                case C5:
                    printf("C5 ");
                    break;
                case F4:
                    printf("F4 ");
                    break;
                case B4:
                    printf("B4 ");
                    break;
                case F3:
                    printf("F3 ");
                    break;
                case B3:
                    printf("B3 ");
                    break;
                case F2:
                    printf("F2 ");
                    break;
                default:
                    break;
                }
            }
        }
        putchar('\t');

        for (u8 j = AD_START; j < PATTAB_SIZE; j += 2)
        {
            if (Pat15[i][j] == COL_INVALID)
                printf(" _ ___ ");
            else
            {
                printf("%2d ", Pat15[i][j]);
                switch (Pat15[i][j + 1])
                {
                case B4a:
                    printf("B4a ");
                    break;
                case F3d:
                    printf("F3d ");
                    break;
                case B4d:
                    printf("B4d ");
                    break;
                default:
                    break;
                }
            }
        }
        putchar('\n');
    }
}

// Check if pattab is full.
static void check_pattab()
{
    for (u32 i = 0; i < IIE15; i++)
    {
        if (Pat15f[i][AD_START - 1] != COL_INVALID)
            printf("freestyle pat full %d\n", i);

        if (Pat15[i][AD_START - 1] != COL_INVALID)
            printf("standard/renju pat full %d\n", i);

        if (Pat15f[i][PATTAB_SIZE - 1] != COL_INVALID)
            printf("freestyle ad full %d\n", i);

        if (Pat15[i][PATTAB_SIZE - 1] != COL_INVALID)
            printf("standard/renju ad full %d\n", i);
    }
}

void nei_table_test()
{
    generate_dist_nei_table();
    for (Pos i = 0; i < BD_SIZE; i++)
    {
        printf("%d\t", i);
        for (Pos j = 0; j < NeiInd[i]; j++)
            printf("%3d  ", Nei[i][j]);
        putchar('\n');
    }
}

void mvlist_test()
{
    Mvlist ml, cp;
    mvlist_reset(&ml);
    mvlist_reset(&cp);

    mvlist_insert(&ml, 1, 0);
    mvlist_insert(&ml, 2, 0);
    mvlist_insert(&ml, 2, 0);
    mvlist_insert(&ml, 3, 0);
    mvlist_insert(&ml, 4, 0);
    mvlist_insert(&ml, 4, 0);
    mvlist_insert(&ml, 5, 0);
    print_mvlist(&ml);

    mvlist_remove(&ml, 1);
    mvlist_remove(&ml, 1);
    mvlist_remove(&ml, 2);
    mvlist_remove(&ml, 2);
    print_mvlist(&ml);

    printf("find 1: %d\n", mvlist_find(&ml, 1));
    printf("find 2: %d\n", mvlist_find(&ml, 2));
    printf("find 3: %d\n", mvlist_find(&ml, 3));
    printf("find 4: %d\n", mvlist_find(&ml, 4));
    printf("find 5: %d\n", mvlist_find(&ml, 5));

    mvlist_copy(&cp, &ml);
    print_mvlist(&cp);
}

void pattab_test()
{
    generate_pattab();
    print_pattab();
    check_pattab();
}
