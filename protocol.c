/*      _____                __    ______
 *     / ___ \              / /   /___  /
 *    / /__/ /___  ____  __/ /_______/ /    ____  ____
 *   / _____/ __ \/ __ \/_   _/ __  / /    / __ \/ __ \
 *  / /    /  ___/ / / / / /_/ /_/ / /____/  ___/ / / /
 * /_/     \____/_/ /_/ /___/\__,_/______/\____/_/ /_/
 *
 * PentaZen, a Gomoku/Renju playing engine developed by Sun.
 */

#include "protocol.h"
#include "public.h"
#include "distance.h"
#include "pattab.h"
#include "search.h"
#include "display.h"

// Command array.
static char Cmd[CMD_MAX][STR_LEN];

// Powers of 10.
static u32 Pow10[10] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000 };

// Character type.
typedef enum {
    STREND,         // '\0'
    SPACE,          // ' '
    CHARACTER       // others
} CharType;

// Return the character type.
static CharType get_type(const char c)
{
    switch (c)
    {
    case '\0':
        return STREND;
    case ' ':
    case ',':
        return SPACE;
    default:
        return CHARACTER;
    }
}

// Parse command. Return 1 if too many arguments.
static void parse(char* str)
{
    CharType last = SPACE;
    int i, j, cnt = -1;

    // remove line break
    for (i = 0; i < STR_LEN; i++)
    {
        if (str[i] == '\n')
        {
            str[i] = '\0';
            break;
        }
    }

    for (i = 0; i < CMD_MAX; i++)
        Cmd[i][0] = '\0';
    i = 0;
    j = 0;

    // parse the command
    while (1)
    {
        if (last == SPACE)
        {
            last = get_type(str[i]);
            switch (last)
            {
            case STREND:
                return;
            case SPACE:
                break;
            case CHARACTER:
                if (cnt < CMD_MAX - 1)
                {
                    j = 0;
                    Cmd[++cnt][j++] = str[i];
                }
                else
                    return;
                break;
            }
        }

        else if (last == CHARACTER)
        {
            last = get_type(str[i]);
            switch (last)
            {
            case STREND:
                if (0 <= cnt && cnt < CMD_MAX)
                    Cmd[cnt][j++] = '\0';
                else
                    return;
                return;
            case SPACE:
                if (0 <= cnt && cnt < CMD_MAX)
                    Cmd[cnt][j++] = '\0';
                else
                    return;
                break;
            case CHARACTER:
                if (0 <= cnt && cnt < CMD_MAX)
                    Cmd[cnt][j++] = str[i];
                else
                    return;
                break;
            }
        }

        i++;
    }
}

// Return the u32 value of a string.
static u32 str_to_u32(const char* str)
{
    u32 res = 0;
    u8 len, i;

    for (len = 0; len < 10; len++)
        if (str[len] == '\0')
            break;

    assert(len > 0);

    for (i = 0; i < len; i++)
        res += (str[i] - '0') * Pow10[len - 1 - i];

    return res;
}

// Initialize all tables.
static void table_init()
{
    srand((unsigned int)time(NULL));
    randomize();
    generate_pattab();
    generate_dist_nei_table();
}

// Main loop.
void loop(Board* bd)
{
    char rec[STR_LEN], tmp[STR_LEN];
    Color c;
    Pos pos;

    table_init();

    while (1)
    {
        if (fgets(rec, STR_LEN, stdin) != NULL)
        {
            parse(rec);

            if (!strcmp(Cmd[0], "START"))
            {
                board_reset(bd, BLACK, RENJU);
                printf("OK\n");
            }

            else if (!strcmp(Cmd[0], "TURN"))
            {
                pos = str_to_u32(Cmd[1]) + str_to_u32(Cmd[2]) * BD_SIDE;
                do_move(bd, pos);
                pos = get_best(bd);
                do_move(bd, pos);
                printf("%d,%d\n", pos % BD_SIDE, pos / BD_SIDE);
            }

            else if (!strcmp(Cmd[0], "BEGIN"))
            {
                pos = get_best(bd);
                do_move(bd, pos);
                printf("%d,%d\n", pos % BD_SIDE, pos / BD_SIDE);
            }

            else if (!strcmp(Cmd[0], "BOARD"))
            {
                while (1)
                {
                    fgets(tmp, STR_LEN, stdin);
                    parse(tmp);

                    if (!strcmp(Cmd[0], "DONE"))
                        break;
                    else
                    {
                        pos = str_to_u32(Cmd[0]) + str_to_u32(Cmd[1]) * BD_SIDE;
                        c = str_to_u32(Cmd[2]);
                        if (num(bd) == 0)
                        {
                            bd->fcolor = c;
                            bd->next = c;
                        }
                        do_move(bd, pos);
                    }
                }

                pos = get_best(bd);
                do_move(bd, pos);
                printf("%d,%d\n", pos % BD_SIDE, pos / BD_SIDE);
            }

            else if (!strcmp(Cmd[0], "INFO"))
            {
                if (!strcmp(Cmd[1], "timeout_turn"))
                    bd->t_turn = str_to_u32(Cmd[2]);

                else if (!strcmp(Cmd[1], "timeout_match"))
                    bd->t_match = str_to_u32(Cmd[2]);

                else if (!strcmp(Cmd[1], "time_left"))
                    bd->t_left = str_to_u32(Cmd[2]);
            }

            else if (!strcmp(Cmd[0], "END"))
            {
                break;
            }

            else if (!strcmp(Cmd[0], "ABOUT"))
            {
                printf("name=\"PentaZen\", version=\"0.2.1\", author=\"Yuliang Sun\", country=\"China\"");
            }

            else if (!strcmp(Cmd[0], "RESTART"))
            {
                board_reset(bd, bd->fcolor, bd->rule);
                printf("OK\n");
            }

            else if (!strcmp(Cmd[0], "TAKEBACK"))
            {
                undo(bd);
                printf("OK\n");
            }
#if TEST_COMMAND
            else if (!strcmp(Cmd[0], "D"))
            {
                pos = str_to_u32(Cmd[1]) + str_to_u32(Cmd[2]) * BD_SIDE;
                do_move(bd, pos);
                print_board(bd);
            }

            else if (!strcmp(Cmd[0], "U"))
            {
                undo(bd);
                print_board(bd);
            }

            else if (!strcmp(Cmd[0], "P"))
            {
                print_board(bd);
            }
#endif
            fflush(stdout);
        }
    }
}
