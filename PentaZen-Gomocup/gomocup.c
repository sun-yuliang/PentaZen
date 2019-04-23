/*
 *  PentaZen Gomoku/Renju Engine
 *                    _______
 *                   / _____/
 *  developed by    / /______  ________
 *                 /____  / / / / __  /
 *                _____/ / /_/ / / / /
 *  2019.04      /______/_____/_/ /_/
 *
 * gomocup.c - gomocup protocol functions
 */

#include "public.h"
#include "gomocup.h"
#include "utility.h"
#include "board.h"
#include "table.h"
#include "hash.h"
#include "search.h"

#if SEARCH_TEST
#include "open.h"
#endif

#define DEAD4_FACTOR	0.95
#define FREEa_FACTOR	1.05
#define FREE3_SCORE		550
#define FREE2_SCORE		200
#define FREE1_SCORE		60
#define DEAD1_SCORE		1

static void table_init()
{
	nei15_helper_init();
	nei20_helper_init();
	nei15_init();
	nei20_init();
	vcx15_init();
	vcx20_init();
	pat_init_freestyle15();
	pat_init(FREESTYLE, 20);
}

static void search_init(search_t* srh)
{
	srh->sc.dead4 = (long)(FREE3_SCORE * DEAD4_FACTOR);
	srh->sc.free3a = (long)(FREE3_SCORE * FREEa_FACTOR);
	srh->sc.free3 = (long)(FREE3_SCORE);
	srh->sc.dead3 = (long)(FREE2_SCORE);
	srh->sc.free2a = (long)(FREE2_SCORE * FREEa_FACTOR);
	srh->sc.free2 = (long)(FREE2_SCORE);
	srh->sc.dead2 = (long)(FREE1_SCORE);
	srh->sc.free1a = (long)(FREE1_SCORE * FREEa_FACTOR);
	srh->sc.free1 = (long)(FREE1_SCORE);
	srh->sc.dead1 = (long)(DEAD1_SCORE);
	srh->me = BLACK;
	srh->hpleaf = 18;
	srh->htleaf = 14;
	srh->mindep = 6;
	srh->maxdep = 20;
	srh->vcfdep = 24;
	srh->vctdep = 18;
}

void gomocup_loop(board_t* bd, search_t* srh)
{
	u32 num;
	u16 pos, color;
	char rec[MAX_STR_LEN], cmd[MAX_STR_LEN], tmp[MAX_STR_LEN];
	char num1[MAX_STR_LEN], num2[MAX_STR_LEN], num3[MAX_STR_LEN];

	while (1)
	{
		if (fgets(rec, MAX_STR_LEN, stdin) != NULL)
		{
			remove_line_break(rec);
			substring(rec, cmd, 0);

			if (!strcmp(cmd, "START"))
			{
				substring(rec, num1, 1);
				num = str_to_int(num1);

				if (num != 15 && num != 20)
					printf("ERROR Board size should be 15 * 15 or 20 * 20\n");
				else
				{
					table_init();
					search_init(srh);
					board_reset(bd, FREESTYLE, num, BLACK);
					printf("OK\n");
				}
			}

			else if (!strcmp(cmd, "TURN"))
			{
				if (bd->num == 0)
					srh->me = opp(bd->fcolor);

				substring(rec, num1, 1);
				substring(rec, num2, 2);
				pos = str_to_int(num2) * bd->side + str_to_int(num1);

				do_move(bd, pos, opp(srh->me));
				pos = get_best(bd, srh);
				do_move(bd, pos, srh->me);
				printf("%d,%d\n", pos % bd->side, pos / bd->side);
			}

			else if (!strcmp(cmd, "BEGIN"))
			{
				srh->me = bd->fcolor;
				pos = get_best(bd, srh);
				do_move(bd, pos, srh->me);
				printf("%d,%d\n", pos % bd->side, pos / bd->side);
			}

			else if (!strcmp(cmd, "BOARD"))
			{
				// place discs
				while (1)
				{
					fgets(tmp, MAX_STR_LEN, stdin);
					remove_line_break(tmp);

					if (!strcmp(tmp, "DONE"))
						break;
					else
					{
						substring(tmp, num1, 0);
						substring(tmp, num2, 1);
						substring(tmp, num3, 2);
						pos = str_to_int(num2) * bd->side + str_to_int(num1);
						color = str_to_int(num3);
						do_move(bd, pos, color);
						if (bd->num == 1)
							bd->fcolor = color;
					}
				}

				// set my color
				if (bd->num % 2 == 0)
					srh->me = bd->fcolor;
				else if (bd->num % 2 == 1)
					srh->me = opp(bd->fcolor);

				// return position
				pos = get_best(bd, srh);
				do_move(bd, pos, srh->me);
				printf("%d,%d\n", pos % bd->side, pos / bd->side);
			}

			else if (!strcmp(cmd, "INFO"))
			{
				substring(rec, tmp, 1);

				if (!strcmp(tmp, "timeout_match"))
				{
					substring(rec, tmp, 2);
					num = str_to_u32(tmp);
					bd->timeout_match = num / 1000.0;
				}
				else if (!strcmp(tmp, "timeout_turn"))
				{
					substring(rec, tmp, 2);
					num = str_to_u32(tmp);
					bd->timeout_turn = num / 1000.0;
				}
				else if (!strcmp(tmp, "time_left"))
				{
					substring(rec, tmp, 2);
					num = str_to_u32(tmp);
					bd->time_left = num / 1000.0;
				}
				else if (!strcmp(tmp, "rule"))
				{
					substring(rec, tmp, 2);
					num = str_to_u32(tmp);

					if (num == RENJU && bd->side != 15)
						printf("ERROR Board size should be 15 * 15 for renju rule\n");
					else if (bd->rule != num)
					{
						bd->rule = num;
						pat_init(num, bd->side);
					}
				}
			}

			else if (!strcmp(cmd, "END"))
			{
				break;
			}

			else if (!strcmp(cmd, "ABOUT"))
			{
				printf("name=\"PentaZen\", version=\"0.1\", author=\"Yuliang Sun\", country=\"China\"");
			}

			else if (!strcmp(cmd, "RESTART"))
			{
				search_init(srh);
				board_reset(bd, bd->rule, bd->side, BLACK);
				printf("OK\n");
			}

			else if (!strcmp(cmd, "TAKEBACK"))
			{
				undo(bd);
				printf("OK\n");
			}

			// commands for debug
			else if (!strcmp(cmd, "B"))
			{
				substring(rec, num1, 1);
				substring(rec, num2, 2);
				pos = str_to_int(num2) * bd->side + str_to_int(num1);
				do_move(bd, pos, BLACK);
				print_board(bd);
			}

			else if (!strcmp(cmd, "W"))
			{
				substring(rec, num1, 1);
				substring(rec, num2, 2);
				pos = str_to_int(num2) * bd->side + str_to_int(num1);
				do_move(bd, pos, WHITE);
				print_board(bd);
			}

			else if (!strcmp(cmd, "U"))
			{
				undo(bd);
				print_board(bd);
			}

			fflush(stdout);
		}
	}
}

#if SEARCH_TEST
void search_test(board_t* bd, search_t* srh)
{
	clock_t start, end;
	double tmp, total = 0, max = 0;
	u16 i, pos, cnt = 0;

	table_init(FREESTYLE);
	search_init(srh);

	for (i = 0; i < OPEN_NUM; i++)
	{
		(*make_opening[i])(bd);
		printf("opening %d\n", i + 1);

		while (1)
		{
			srh->me = WHITE;
			start = clock();
			pos = get_best(bd, srh);
			end = clock();
			tmp = (double)(end - start) / CLOCKS_PER_SEC;
			total = total + tmp;
			if (tmp > max)
				max = tmp;
			do_move(bd, pos, srh->me);
			cnt++;
			if (gameover(bd) != INVALID)
			{
				print_board(bd);
				break;
			}

			srh->me = BLACK;
			start = clock();
			pos = get_best(bd, srh);
			end = clock();
			tmp = (double)(end - start) / CLOCKS_PER_SEC;
			total = total + tmp;
			if (tmp > max)
				max = tmp;
			do_move(bd, pos, srh->me);
			cnt++;
			if (gameover(bd) != INVALID)
			{
				print_board(bd);
				break;
			}
		}
	}

	printf("total ai moves: %d\n", cnt);
	printf("total time cost: %lfs\n", total);
	printf("average time for one move: %lfs\n", total / cnt);
	printf("maximum time for one move: %lfs\n", max);
}
#endif
