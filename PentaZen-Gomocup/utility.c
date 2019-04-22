/*
 *  PentaZen Gomoku/Renju Engine
 *                    _______
 *                   / _____/
 *  developed by    / /______  ________
 *                 /____  / / / / __  /
 *                _____/ / /_/ / / / /
 *  2019.04      /______/_____/_/ /_/
 *
 * utility.c - miscellaneous functions
 */

#include "utility.h"
#include "public.h"
#include "mvlist.h"
#include "board.h"
#include "table.h"

static pow10[8] = {
	1, 10, 100, 1000, 10000, 100000, 1000000, 10000000
};

// Replace '\n' with '\0'.
void remove_line_break(char* str)
{
	int i;
	for (i = 0; i < MAX_STR_LEN; i++)
	{
		if (str[i] == '\n')
		{
			str[i] = '\0';
			break;
		}
	}
}

// Get the Nth sub-string seperated by space or comma. N starts from 0.
bool substring(const char* in, char* out, const int N)
{
	int begin = 0, end = 0, cnt = 0, i;

	while (cnt < N)
	{
		if (begin >= MAX_STR_LEN - 1 || in[begin] == '\0')
		{
			out[0] = '\0';
			return false;
		}
		else if (in[begin] == ' ' || in[begin] == ',')
			cnt++;
		begin++;
	}

	end = begin + 1;
	while (in[end] != ' ' && in[end] != ',' && in[end] != '\0')
		end++;

	cnt = 0;
	for (i = begin; i < end; i++)
		out[cnt++] = in[i];
	out[cnt] = '\0';

	return true;
}

// Return the int value of a string, ranging from 0 to 99.
int str_to_int(const char* str)
{
	int i;
	for (i = 0; i < MAX_STR_LEN; i++)
		if (str[i] == '\0')
			break;
	if (i == 1)
		return str[0] - '0';
	else if (i == 2)
		return 10 * (str[0] - '0') + str[1] - '0';
	else
		return INVALID;
}

// Return the u32 value of a string, ranging from 0 to 99999999.
u32 str_to_u32(const char* str)
{
	int len, i;
	u32 res = 0;
	for (len = 0; len < 8; len++)
		if (str[len] == '\0')
			break;
	for (i = 0; i < len; i++)
		res += (str[i] - '0') * pow10[len - 1 - i];
	return res;
}

// Output mvlist infomation to stdout.
void print_mvlist(const mvlist_t* mv, const u16 side)
{
	u16 pos = mvlist_first(mv);
	while (pos != END)
	{
		printf("(%d, %d) ", pos % side, pos / side);
		pos = mvlist_next(mv, pos);
	}
	putchar('\n');
}

// Output pattern infomation to stdout.
void print_pattern(const pattern_t* pat)
{
	printf("black     \twhite\n");

	printf("long:   %d\t", pattern_read(pat, LONG, BLACK));
	printf("long:   %d\n", pattern_read(pat, LONG, WHITE));

	printf("five:   %d\t", pattern_read(pat, FIVE, BLACK));
	printf("five:   %d\n", pattern_read(pat, FIVE, WHITE));

	printf("free4:  %d\t", pattern_read(pat, FREE4, BLACK));
	printf("free4:  %d\n", pattern_read(pat, FREE4, WHITE));

	printf("dead4:  %d\t", pattern_read(pat, DEAD4, BLACK));
	printf("dead4:  %d\n", pattern_read(pat, DEAD4, WHITE));

	printf("free3a: %d\t", pattern_read(pat, FREE3a, BLACK));
	printf("free3a: %d\n", pattern_read(pat, FREE3a, WHITE));

	printf("free3:  %d\t", pattern_read(pat, FREE3, BLACK));
	printf("free3:  %d\n", pattern_read(pat, FREE3, WHITE));

	printf("dead3:  %d\t", pattern_read(pat, DEAD3, BLACK));
	printf("dead3:  %d\n", pattern_read(pat, DEAD3, WHITE));

	printf("free2a: %d\t", pattern_read(pat, FREE2a, BLACK));
	printf("free2a: %d\n", pattern_read(pat, FREE2a, WHITE));

	printf("free2:  %d\t", pattern_read(pat, FREE2, BLACK));
	printf("free2:  %d\n", pattern_read(pat, FREE2, WHITE));
}

// Output board infomation to stdout.
void print_board(const board_t* bd)
{
#if DISPLAY_BOARD
	int r, c, pos;

	printf("\n ");
	for (c = 0; c < bd->side; c++)
		printf("%4d", c);
	printf("\n\n");

	for (r = 0; r < bd->side; r++)
	{
		printf("%2d", r);
		for (c = 0; c < bd->side; c++)
		{
			pos = r * bd->side + c;

			if (bd->arr[pos] == BLACK)
				printf("  X ");
			else if (bd->arr[pos] == WHITE)
				printf("  O ");
#if DISPLAY_MLIST
			else if (mvlist_find(mlist(bd), pos))
				printf("  ` ");
#endif
#if DISPLAY_HLIST
			else if (mvlist_find(hlist(bd), pos))
				printf("  * ");
#endif
			else
				printf("  . ");
		}
		putchar('\n');
		if (r < bd->side - 1)
			putchar('\n');
	}
	putchar('\n');
#endif

#if DISPLAY_MSTK
	printf("%d moves: ", mvlist_size(mstk(bd)));
	print_mvlist(mstk(bd), bd->side);
#endif

#if DISPLAY_ZOBRIST
	printf("%llu\n", bd->zkey);
#endif

#if DISPLAY_PAT
	print_pattern(pat(bd));
#endif

#if DISPLAY_PINC
	print_pattern(pinc(bd));
#endif
}

// Output a 20 * 20 array of u64 random numbers to stdout.
void print_zobrist()
{
	u64 tmp[400];
	int i, j;
	srand((unsigned int)time(NULL));

	for (i = 0; i < 20; i++)
	{
		for (j = 0; j < 20; j++)
		{
			tmp[i * 20 + j] = rand() ^ ((u64)rand() << 15) ^ ((u64)rand() << 30) ^ ((u64)rand() << 45) ^ ((u64)rand() << 60);
			printf("%llu, ", tmp[i * 20 + j]);
		}
		printf("\n");
	}

	for (i = 0; i < 400; i++)
	{
		for (j = i + 1; j < 400; j++)
		{
			if (tmp[i] == tmp[j])
			{
				printf("Repetition exists\n");
				printf("%d and %d\n", i, j);
				return;
			}
		}
	}

	printf("No repetition\n");
}
