/*
 *  PentaZen Gomoku/Renju Engine
 *                    _______
 *                   / _____/
 *  developed by    / /______  ________
 *                 /____  / / / / __  /
 *                _____/ / /_/ / / / /
 *  2019.04      /______/_____/_/ /_/
 *
 * table.c - table generation and lookup functions
 */

#include "table.h"
#include "utility.h"
#include "pattern.h"
#include "board.h"
#include "array.h"

// neighbor tables
u16 NeiA15[15 * 15];	// # of neighbors 3-disc away in 8 directions
u16 NeiA20[20 * 20];
u16 NeiD15[15 * 15];	// # of neighbors 4-disc away in 8 directions
u16 NeiD20[20 * 20];
u16 Nei15[15 * 15][NEI_SIZE * 8];
u16 Nei20[20 * 20][NEI_SIZE * 8];
u16 Vcx15[15 * 15][VCX_SIZE * 8];	// neighbors <= 4-discs away in 8 directions
u16 Vcx20[20 * 20][VCX_SIZE * 8];

// freestyle pattern tables
static u16 Pat15w[P15][PAT_SIZE + SPE_SIZE];
static u16 Pat14w[P14][PAT_SIZE + SPE_SIZE];
static u16 Pat13w[P13][PAT_SIZE + SPE_SIZE];
static u16 Pat12w[P12][PAT_SIZE + SPE_SIZE];
static u16 Pat11w[P11][PAT_SIZE + SPE_SIZE];
static u16 Pat10w[P10][PAT_SIZE + SPE_SIZE];
static u16 Pat9w[P9][PAT_SIZE + SPE_SIZE];
static u16 Pat8w[P8][PAT_SIZE + SPE_SIZE];
static u16 Pat7w[P7][PAT_SIZE + SPE_SIZE];
static u16 Pat6w[P6][PAT_SIZE + SPE_SIZE];
static u16 Pat5w[P5][PAT_SIZE + SPE_SIZE];

// general pattern tables
static u16 Pat20[P20][PAT_SIZE + SPE_SIZE];
static u16 Pat19[P19][PAT_SIZE + SPE_SIZE];
static u16 Pat18[P18][PAT_SIZE + SPE_SIZE];
static u16 Pat17[P17][PAT_SIZE + SPE_SIZE];
static u16 Pat16[P16][PAT_SIZE + SPE_SIZE];
static u16 Pat15[P15][PAT_SIZE + SPE_SIZE];
static u16 Pat14[P14][PAT_SIZE + SPE_SIZE];
static u16 Pat13[P13][PAT_SIZE + SPE_SIZE];
static u16 Pat12[P12][PAT_SIZE + SPE_SIZE];
static u16 Pat11[P11][PAT_SIZE + SPE_SIZE];
static u16 Pat10[P10][PAT_SIZE + SPE_SIZE];
static u16 Pat9[P9][PAT_SIZE + SPE_SIZE];
static u16 Pat8[P8][PAT_SIZE + SPE_SIZE];
static u16 Pat7[P7][PAT_SIZE + SPE_SIZE];
static u16 Pat6[P6][PAT_SIZE + SPE_SIZE];
static u16 Pat5[P5][PAT_SIZE + SPE_SIZE];

// Array of 2^0-2^19.
static u32 pow2[3][21] = {
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20 },
	{ P0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20 }
};

/*******************************************************************************
                            Neighbor Table Generation                           
*******************************************************************************/
static u16 nei15_helper[23][23];
static u16 nei20_helper[28][28];

void nei15_helper_init()
{
	int r, c;
	for (r = 0; r < 23; r++)
	{
		for (c = 0; c < 23; c++)
		{
			if (r < 4 || r > 18	|| c < 4 || c > 18)
				nei15_helper[r][c] = INVALID;
			else
				nei15_helper[r][c] = (r - 4) * 15 + c - 4;
		}
	}
}

void nei20_helper_init()
{
	int r, c;
	for (r = 0; r < 28; r++)
	{
		for (c = 0; c < 28; c++)
		{
			if (r < 4 || r > 23 || c < 4 || c > 23)
				nei20_helper[r][c] = INVALID;
			else
				nei20_helper[r][c] = (r - 4) * 20 + c - 4;
		}
	}
}

void nei15_init()
{
	int nr, nc, ar, ac;
	int index, i, j;

	for (nr = 0; nr < 15; nr++)
	{
		for (nc = 0; nc < 15; nc++)
		{
			index = 0;
			// 2 discs away
			for (i = 2; i >= -2; i -= 2)
			{
				for (j = 2; j >= -2; j -= 2)
				{
					ar = nr + 4 + i;
					ac = nc + 4 + j;
					if (nei15_helper[ar][ac] != INVALID && (i != 0 || j != 0))
						Nei15[nr * 15 + nc][index++] = nei15_helper[ar][ac];
				}
			}
			// 1 disc away
			for (i = 1; i >= -1; i -= 1)
			{
				for (j = 1; j >= -1; j -= 1)
				{
					ar = nr + 4 + i;
					ac = nc + 4 + j;
					if (nei15_helper[ar][ac] != INVALID && (i != 0 || j != 0))
						Nei15[nr * 15 + nc][index++] = nei15_helper[ar][ac];
				}
			}
			// mark the rest INVALID
			if (index != NEI_SIZE * 8)
				for (i = index; i < NEI_SIZE * 8; i++)
					Nei15[nr * 15 + nc][i] = INVALID;
		}
	}
}

void nei20_init()
{
	int nr, nc, ar, ac;
	int index, i, j;

	for (nr = 0; nr < 20; nr++)
	{
		for (nc = 0; nc < 20; nc++)
		{
			index = 0;
			// 2 discs away
			for (i = 2; i >= -2; i -= 2)
			{
				for (j = 2; j >= -2; j -= 2)
				{
					ar = nr + 4 + i;
					ac = nc + 4 + j;
					if (nei20_helper[ar][ac] != INVALID && (i != 0 || j != 0))
						Nei20[nr * 20 + nc][index++] = nei20_helper[ar][ac];
				}
			}
			// 1 disc away
			for (i = 1; i >= -1; i -= 1)
			{
				for (j = 1; j >= -1; j -= 1)
				{
					ar = nr + 4 + i;
					ac = nc + 4 + j;
					if (nei20_helper[ar][ac] != INVALID && (i != 0 || j != 0))
						Nei20[nr * 20 + nc][index++] = nei20_helper[ar][ac];
				}
			}
			// mark the rest INVALID
			if (index != NEI_SIZE * 8)
				for (i = index; i < NEI_SIZE * 8; i++)
					Nei20[nr * 20 + nc][i] = INVALID;
		}
	}
}

void vcx15_init()
{
	int nr, nc, ar, ac;
	int index, i, j;

	memset(NeiA15, 0, sizeof(NeiA15));
	memset(NeiD15, 0, sizeof(NeiD15));

	for (nr = 0; nr < 15; nr++)
	{
		for (nc = 0; nc < 15; nc++)
		{
			index = 0;
			// 1 disc away
			for (i = -1; i <= 1; i += 1)
			{
				for (j = -1; j <= 1; j += 1)
				{
					ar = nr + 4 + i;
					ac = nc + 4 + j;
					if (nei15_helper[ar][ac] != INVALID && (i != 0 || j != 0))
					{
						Vcx15[nr * 15 + nc][index++] = nei15_helper[ar][ac];
						NeiA15[nr * 15 + nc]++;
						NeiD15[nr * 15 + nc]++;
					}
				}
			}
			// 2 discs away
			for (i = -2; i <= 2; i += 2)
			{
				for (j = -2; j <= 2; j += 2)
				{
					ar = nr + 4 + i;
					ac = nc + 4 + j;
					if (nei15_helper[ar][ac] != INVALID && (i != 0 || j != 0))
					{
						Vcx15[nr * 15 + nc][index++] = nei15_helper[ar][ac];
						NeiA15[nr * 15 + nc]++;
						NeiD15[nr * 15 + nc]++;
					}
				}
			}
			// 3 discs away
			for (i = -3; i <= 3; i += 3)
			{
				for (j = -3; j <= 3; j += 3)
				{
					ar = nr + 4 + i;
					ac = nc + 4 + j;
					if (nei15_helper[ar][ac] != INVALID && (i != 0 || j != 0))
					{
						Vcx15[nr * 15 + nc][index++] = nei15_helper[ar][ac];
						NeiA15[nr * 15 + nc]++;
						NeiD15[nr * 15 + nc]++;
					}
				}
			}
			// 4 discs away
			for (i = -4; i <= 4; i += 4)
			{
				for (j = -4; j <= 4; j += 4)
				{
					ar = nr + 4 + i;
					ac = nc + 4 + j;
					if (nei15_helper[ar][ac] != INVALID && (i != 0 || j != 0))
					{
						Vcx15[nr * 15 + nc][index++] = nei15_helper[ar][ac];
						NeiD15[nr * 15 + nc]++;
					}
				}
			}
			// mark the rest INVALID
			if (index != VCX_SIZE * 8)
				for (i = index; i < VCX_SIZE * 8; i++)
					Vcx15[nr * 15 + nc][i] = INVALID;
		}
	}
}

void vcx20_init()
{
	int nr, nc, ar, ac;
	int index, i, j;

	memset(NeiA20, 0, sizeof(NeiA20));
	memset(NeiD20, 0, sizeof(NeiD20));

	for (nr = 0; nr < 20; nr++)
	{
		for (nc = 0; nc < 20; nc++)
		{
			index = 0;
			// 1 disc away
			for (i = -1; i <= 1; i += 1)
			{
				for (j = -1; j <= 1; j += 1)
				{
					ar = nr + 4 + i;
					ac = nc + 4 + j;
					if (nei20_helper[ar][ac] != INVALID && (i != 0 || j != 0))
					{
						Vcx20[nr * 20 + nc][index++] = nei20_helper[ar][ac];
						NeiA20[nr * 20 + nc]++;
						NeiD20[nr * 20 + nc]++;
					}
				}
			}
			// 2 discs away
			for (i = -2; i <= 2; i += 2)
			{
				for (j = -2; j <= 2; j += 2)
				{
					ar = nr + 4 + i;
					ac = nc + 4 + j;
					if (nei20_helper[ar][ac] != INVALID && (i != 0 || j != 0))
					{
						Vcx20[nr * 20 + nc][index++] = nei20_helper[ar][ac];
						NeiA20[nr * 20 + nc]++;
						NeiD20[nr * 20 + nc]++;
					}
				}
			}
			// 3 discs away
			for (i = -3; i <= 3; i += 3)
			{
				for (j = -3; j <= 3; j += 3)
				{
					ar = nr + 4 + i;
					ac = nc + 4 + j;
					if (nei20_helper[ar][ac] != INVALID && (i != 0 || j != 0))
					{
						Vcx20[nr * 20 + nc][index++] = nei20_helper[ar][ac];
						NeiA20[nr * 20 + nc]++;
						NeiD20[nr * 20 + nc]++;
					}
				}
			}
			// 4 discs away
			for (i = -4; i <= 4; i += 4)
			{
				for (j = -4; j <= 4; j += 4)
				{
					ar = nr + 4 + i;
					ac = nc + 4 + j;
					if (nei20_helper[ar][ac] != INVALID && (i != 0 || j != 0))
					{
						Vcx20[nr * 20 + nc][index++] = nei20_helper[ar][ac];
						NeiD20[nr * 20 + nc]++;
					}
				}
			}
			// mark the rest INVALID
			if (index != VCX_SIZE * 8)
				for (i = index; i < VCX_SIZE * 8; i++)
					Vcx20[nr * 20 + nc][i] = INVALID;
		}
	}
}

/*******************************************************************************
                            Pattern Table Generation                            
*******************************************************************************/
// helper structure
typedef struct {
	bool is;
	u16 macro;
	u32 mask;
	u16 index;
} phelp_t;

// helper arrays
static phelp_t lon[P6];
static phelp_t five[P5];
static phelp_t free4[P6];
static phelp_t d4d4b9[P9];
static phelp_t d4d4b8[P8];
static phelp_t d4d4b7[P7];
static phelp_t dead4[P5];
static phelp_t free3a[P7];
static phelp_t free3[P6];
static phelp_t dead3[P5];
static phelp_t free2b[P8];
static phelp_t free2a[P7];
static phelp_t free2[P6];
static phelp_t dead2[P5];
static phelp_t free1c[P9];
static phelp_t free1b[P8];
static phelp_t free1a[P7];
static phelp_t free1[P6];
static phelp_t dead1[P5];
static phelp_t d4b7[P7];
static phelp_t d3b8[P8];
static phelp_t d3b7[P7];
static phelp_t b6[P6];

// Pattern information array.
// Include pattern length, index, macro, mask and spetial points in order.
static u16 pattern[73][15] =
{
	// 0-2
	{ 6, 63, LONG,  0x3f, Z, Z, Z, Z, X, Z, Z, X, Z, Z, X },	// 111111
	{ 5, 31, FIVE,  0x1f, Z, Z, Z, Z, X, Z, Z, X, Z, Z, X },	// 11111
	{ 6, 30, FREE4, 0x1e, Z, Z, Z, Z, X, Z, Z, X, Z, Z, X },	// 011110
	// 3-10
	{ 9, 471,DEAD4, 0x1d7,3, 5, Z, Z, X, Z, Z, X, Z, Z, X },	// 111010111
	{ 8, 219,DEAD4, 0xdb, 2, 5, Z, Z, X, Z, Z, X, Z, Z, X },	// 11011011
	{ 7, 93, DEAD4, 0x5d, 1, 5, Z, Z, X, Z, Z, X, Z, Z, X },	// 1011101
	{ 5, 15, DEAD4, 0x0f, 4, Z, Z, Z, X, Z, Z, X, Z, Z, X },	// 01111
	{ 5, 23, DEAD4, 0x17, 3, Z, Z, Z, X, Z, Z, X, Z, Z, X },	// 10111
	{ 5, 27, DEAD4, 0x1b, 2, Z, Z, Z, X, Z, Z, X, Z, Z, X },	// 11011
	{ 5, 29, DEAD4, 0x1d, 1, Z, Z, Z, X, Z, Z, X, Z, Z, X },	// 11101
	{ 5, 30, DEAD4, 0x1e, 0, Z, Z, Z, X, Z, Z, X, Z, Z, X },	// 11110
	// 11-15
	{ 7, 28, FREE3a,0x1c, 1, 5, Z, Z, X, Z, Z, X, Z, Z, X },	// 0011100
	{ 6, 14, FREE3, 0x0e, 0, 4, 5, Z, X, Z, Z, X, Z, Z, X },	// 001110
	{ 6, 22, FREE3, 0x16, 0, 3, 5, Z, X, Z, Z, X, Z, Z, X },	// 010110
	{ 6, 26, FREE3, 0x1a, 0, 2, 5, Z, X, Z, Z, X, Z, Z, X },	// 011010
	{ 6, 28, FREE3, 0x1c, 0, 1, 5, Z, X, Z, Z, X, Z, Z, X },	// 011100
	// 16-25
	{ 5, 7,  DEAD3, 0x07, 3, 4, Z, Z, X, Z, Z, X, Z, Z, X },	// 00111
	{ 5, 11, DEAD3, 0x0b, 2, 4, Z, Z, X, Z, Z, X, Z, Z, X },	// 01011
	{ 5, 13, DEAD3, 0x0d, 1, 4, Z, Z, X, Z, Z, X, Z, Z, X },	// 01101
	{ 5, 14, DEAD3, 0x0e, 0, 4, Z, Z, X, Z, Z, X, Z, Z, X },	// 01110
	{ 5, 19, DEAD3, 0x13, 2, 3, Z, Z, X, Z, Z, X, Z, Z, X },	// 10011
	{ 5, 21, DEAD3, 0x15, 1, 3, Z, Z, X, Z, Z, X, Z, Z, X },	// 10101
	{ 5, 22, DEAD3, 0x16, 0, 3, Z, Z, X, Z, Z, X, Z, Z, X },	// 10110
	{ 5, 25, DEAD3, 0x19, 1, 2, Z, Z, X, Z, Z, X, Z, Z, X },	// 11001
	{ 5, 26, DEAD3, 0x1a, 0, 2, Z, Z, X, Z, Z, X, Z, Z, X },	// 11010
	{ 5, 28, DEAD3, 0x1c, 0, 1, Z, Z, X, Z, Z, X, Z, Z, X },	// 11100
	// 26-35
	{ 8, 24, FREE2a,0x18, 1, 2, 5, 6, X, Z, Z, X, 2, 5, X },	// 00011000
	{ 7, 12, FREE2a,0x0c, 1, 4, 5, Z, X, 4, Z, X, 1, 5, X },	// 0001100
	{ 7, 20, FREE2a,0x14, 1, 3, 5, Z, X, 3, Z, X, 1, 5, X },	// 0010100
	{ 7, 24, FREE2a,0x18, 1, 2, 5, Z, X, 2, Z, X, 1, 5, X },	// 0011000
	{ 6, 6,  FREE2, 0x06, 3, 4, Z, Z, X, 3, 4, X, 0, 5, X },	// 000110
	{ 6, 10, FREE2, 0x0a, 2, 4, Z, Z, X, 2, 4, X, 0, 5, X },	// 001010
	{ 6, 12, FREE2, 0x0c, 1, 4, Z, Z, X, 1, 4, X, 0, 5, X },	// 001100
	{ 6, 18, FREE2, 0x12, 2, 3, Z, Z, X, 2, 3, X, 0, 5, X },	// 010010
	{ 6, 20, FREE2, 0x14, 1, 3, Z, Z, X, 1, 3, X, 0, 5, X },	// 010100
	{ 6, 24, FREE2, 0x18, 1, 2, Z, Z, X, 1, 2, X, 0, 5, X },	// 011000
	// 36-45
	{ 5, 3,  DEAD2, 0x03, 2, 3, 4, Z, X, Z, Z, X, Z, Z, X },	// 00011
	{ 5, 5,  DEAD2, 0x05, 1, 3, 4, Z, X, Z, Z, X, Z, Z, X },	// 00101
	{ 5, 6,  DEAD2, 0x06, 0, 3, 4, Z, X, Z, Z, X, Z, Z, X },	// 00110
	{ 5, 9,  DEAD2, 0x09, 1, 2, 4, Z, X, Z, Z, X, Z, Z, X },	// 01001
	{ 5, 10, DEAD2, 0x0a, 0, 2, 4, Z, X, Z, Z, X, Z, Z, X },	// 01010
	{ 5, 12, DEAD2, 0x0c, 0, 1, 4, Z, X, Z, Z, X, Z, Z, X },	// 01100
	{ 5, 17, DEAD2, 0x11, 1, 2, 3, Z, X, Z, Z, X, Z, Z, X },	// 10001
	{ 5, 18, DEAD2, 0x12, 0, 2, 3, Z, X, Z, Z, X, Z, Z, X },	// 10010
	{ 5, 20, DEAD2, 0x14, 0, 1, 3, Z, X, Z, Z, X, Z, Z, X },	// 10100
	{ 5, 24, DEAD2, 0x18, 0, 1, 2, Z, X, Z, Z, X, Z, Z, X },	// 11000
	// 46-55
	{ 9, 16, FREE1a,0x10, 1, 2, 3, 5, 6, 7, Z, Z, Z, Z, Z },	// 000010000
	{ 8, 8,  FREE1a,0x08, 1, 2, 4, 5, 6, Z, Z, Z, Z, Z, Z },	// 00001000
	{ 8, 16, FREE1a,0x10, 1, 2, 3, 5, 6, Z, Z, Z, Z, Z, Z },	// 00010000
	{ 7, 4,  FREE1a,0x04, 1, 3, 4, 5, Z, Z, Z, Z, Z, Z, Z },	// 0000100
	{ 7, 8,  FREE1a,0x08, 1, 2, 4, 5, Z, Z, Z, Z, Z, Z, Z },	// 0001000
	{ 7, 16, FREE1a,0x10, 1, 2, 3, 5, Z, Z, Z, Z, Z, Z, Z },	// 0010000
	{ 6, 2,  FREE1, 0x02, 2, 3, 4, Z, Z, Z, Z, Z, Z, Z, Z },	// 000010
	{ 6, 4,  FREE1, 0x04, 1, 3, 4, Z, Z, Z, Z, Z, Z, Z, Z },	// 000100
	{ 6, 8,  FREE1, 0x08, 1, 2, 4, Z, Z, Z, Z, Z, Z, Z, Z },	// 001000
	{ 6, 16, FREE1, 0x10, 1, 2, 3, Z, Z, Z, Z, Z, Z, Z, Z },	// 010000
	// 56-60
	{ 5, 1,  DEAD1, 0x01, Z, Z, Z, Z, X, Z, Z, X, Z, Z, X },	// 00001
	{ 5, 2,  DEAD1, 0x02, Z, Z, Z, Z, X, Z, Z, X, Z, Z, X },	// 00010
	{ 5, 4,  DEAD1, 0x04, Z, Z, Z, Z, X, Z, Z, X, Z, Z, X },	// 00100
	{ 5, 8,  DEAD1, 0x08, Z, Z, Z, Z, X, Z, Z, X, Z, Z, X },	// 01000
	{ 5, 16, DEAD1, 0x10, Z, Z, Z, Z, X, Z, Z, X, Z, Z, X },	// 10000	
	// 61-62
	{ 7, 61, DEAD4, 0x3c, 6, Z, Z, Z, X, Z, Z, X, Z, Z, X },	// 0111101
	{ 7, 94, DEAD4, 0x1e, 0, Z, Z, Z, X, Z, Z, X, Z, Z, X },	// 1011110
	// 63-64
	{ 8, 59, DEAD3, 0x38, 6, 7, Z, Z, X, Z, Z, X, Z, Z, X },	// 00111011
	{ 8, 220,DEAD3, 0x1c, 0, 1, Z, Z, X, Z, Z, X, Z, Z, X },	// 11011100
	// 65-68
	{ 7, 45, DEAD3, 0x2c, 4, 6, Z, Z, X, Z, Z, X, Z, Z, X },	// 0101101
	{ 7, 53, DEAD3, 0x34, 3, 6, Z, Z, X, Z, Z, X, Z, Z, X },	// 0110101
	{ 7, 86, DEAD3, 0x16, 0, 3, Z, Z, X, Z, Z, X, Z, Z, X },	// 1010110
	{ 7, 90, DEAD3, 0x1a, 0, 2, Z, Z, X, Z, Z, X, Z, Z, X },	// 1011010
	// 69-72
	{ 6, 47, Z,     0x2f, Z, Z, Z, Z, X, Z, Z, X, Z, Z, X },	// 101111
	{ 6, 55, Z,     0x37, Z, Z, Z, Z, X, Z, Z, X, Z, Z, X },	// 110111
	{ 6, 59, Z,     0x3b, Z, Z, Z, Z, X, Z, Z, X, Z, Z, X },	// 111011
	{ 6, 61, Z,     0x3d, Z, Z, Z, Z, X, Z, Z, X, Z, Z, X },	// 111101
};

// Initialize helper arrays.
static void helper_init()
{
	int i;

	memset(lon, 0, sizeof(lon));
	memset(five, 0, sizeof(five));
	memset(free4, 0, sizeof(free4));
	memset(d4d4b9, 0, sizeof(d4d4b9));
	memset(d4d4b8, 0, sizeof(d4d4b8));
	memset(d4d4b7, 0, sizeof(d4d4b7));
	memset(dead4, 0, sizeof(dead4));
	memset(free3a, 0, sizeof(free3a));
	memset(free3, 0, sizeof(free3));
	memset(dead3, 0, sizeof(dead3));
	memset(free2b, 0, sizeof(free2b));
	memset(free2a, 0, sizeof(free2a));
	memset(free2, 0, sizeof(free2));
	memset(dead2, 0, sizeof(dead2));
	memset(free1c, 0, sizeof(free1c));
	memset(free1b, 0, sizeof(free1b));
	memset(free1a, 0, sizeof(free1a));
	memset(free1, 0, sizeof(free1));
	memset(dead1, 0, sizeof(dead1));
	memset(d4b7, 0, sizeof(d4b7));
	memset(d3b8, 0, sizeof(d3b8));
	memset(d3b7, 0, sizeof(d3b7));
	memset(b6, 0, sizeof(b6));

	// long
	for (i = 0; i <= 0; i++)
	{
		lon[pattern[i][1]].is = true;
		lon[pattern[i][1]].macro = pattern[i][2];
		lon[pattern[i][1]].mask = pattern[i][3];
		lon[pattern[i][1]].index = i;
	}
	// five
	for (i = 1; i <= 1; i++)
	{
		five[pattern[i][1]].is = true;
		five[pattern[i][1]].macro = pattern[i][2];
		five[pattern[i][1]].mask = pattern[i][3];
		five[pattern[i][1]].index = i;
	}
	// free4
	for (i = 2; i <= 2; i++)
	{
		free4[pattern[i][1]].is = true;
		free4[pattern[i][1]].macro = pattern[i][2];
		free4[pattern[i][1]].mask = pattern[i][3];
		free4[pattern[i][1]].index = i;
	}
	// d4d4b9
	for (i = 3; i <= 3; i++)
	{
		d4d4b9[pattern[i][1]].is = true;
		d4d4b9[pattern[i][1]].macro = pattern[i][2];
		d4d4b9[pattern[i][1]].mask = pattern[i][3];
		d4d4b9[pattern[i][1]].index = i;
	}
	// d4d4b8
	for (i = 4; i <= 4; i++)
	{
		d4d4b8[pattern[i][1]].is = true;
		d4d4b8[pattern[i][1]].macro = pattern[i][2];
		d4d4b8[pattern[i][1]].mask = pattern[i][3];
		d4d4b8[pattern[i][1]].index = i;
	}
	// d4d4b7
	for (i = 5; i <= 5; i++)
	{
		d4d4b7[pattern[i][1]].is = true;
		d4d4b7[pattern[i][1]].macro = pattern[i][2];
		d4d4b7[pattern[i][1]].mask = pattern[i][3];
		d4d4b7[pattern[i][1]].index = i;
	}
	// dead4
	for (i = 6; i <= 10; i++)
	{
		dead4[pattern[i][1]].is = true;
		dead4[pattern[i][1]].macro = pattern[i][2];
		dead4[pattern[i][1]].mask = pattern[i][3];
		dead4[pattern[i][1]].index = i;
	}
	// free3a
	for (i = 11; i <= 11; i++)
	{
		free3a[pattern[i][1]].is = true;
		free3a[pattern[i][1]].macro = pattern[i][2];
		free3a[pattern[i][1]].mask = pattern[i][3];
		free3a[pattern[i][1]].index = i;
	}
	// free3
	for (i = 12; i <= 15; i++)
	{
		free3[pattern[i][1]].is = true;
		free3[pattern[i][1]].macro = pattern[i][2];
		free3[pattern[i][1]].mask = pattern[i][3];
		free3[pattern[i][1]].index = i;
	}
	// dead3
	for (i = 16; i <= 25; i++)
	{
		dead3[pattern[i][1]].is = true;
		dead3[pattern[i][1]].macro = pattern[i][2];
		dead3[pattern[i][1]].mask = pattern[i][3];
		dead3[pattern[i][1]].index = i;
	}
	// free2b
	for (i = 26; i <= 26; i++)
	{
		free2b[pattern[i][1]].is = true;
		free2b[pattern[i][1]].macro = pattern[i][2];
		free2b[pattern[i][1]].mask = pattern[i][3];
		free2b[pattern[i][1]].index = i;
	}
	// free2a
	for (i = 27; i <= 29; i++)
	{
		free2a[pattern[i][1]].is = true;
		free2a[pattern[i][1]].macro = pattern[i][2];
		free2a[pattern[i][1]].mask = pattern[i][3];
		free2a[pattern[i][1]].index = i;
	}
	// free2
	for (i = 30; i <= 35; i++)
	{
		free2[pattern[i][1]].is = true;
		free2[pattern[i][1]].macro = pattern[i][2];
		free2[pattern[i][1]].mask = pattern[i][3];
		free2[pattern[i][1]].index = i;
	}
	// dead2
	for (i = 36; i <= 45; i++)
	{
		dead2[pattern[i][1]].is = true;
		dead2[pattern[i][1]].macro = pattern[i][2];
		dead2[pattern[i][1]].mask = pattern[i][3];
		dead2[pattern[i][1]].index = i;
	}
	// free1c
	for (i = 46; i <= 46; i++)
	{
		free1c[pattern[i][1]].is = true;
		free1c[pattern[i][1]].macro = pattern[i][2];
		free1c[pattern[i][1]].mask = pattern[i][3];
		free1c[pattern[i][1]].index = i;
	}
	// free1b
	for (i = 47; i <= 48; i++)
	{
		free1b[pattern[i][1]].is = true;
		free1b[pattern[i][1]].macro = pattern[i][2];
		free1b[pattern[i][1]].mask = pattern[i][3];
		free1b[pattern[i][1]].index = i;
	}
	// free1a
	for (i = 49; i <= 51; i++)
	{
		free1a[pattern[i][1]].is = true;
		free1a[pattern[i][1]].macro = pattern[i][2];
		free1a[pattern[i][1]].mask = pattern[i][3];
		free1a[pattern[i][1]].index = i;
	}
	// free1
	for (i = 52; i <= 55; i++)
	{
		free1[pattern[i][1]].is = true;
		free1[pattern[i][1]].macro = pattern[i][2];
		free1[pattern[i][1]].mask = pattern[i][3];
		free1[pattern[i][1]].index = i;
	}
	// dead1
	for (i = 56; i <= 60; i++)
	{
		dead1[pattern[i][1]].is = true;
		dead1[pattern[i][1]].macro = pattern[i][2];
		dead1[pattern[i][1]].mask = pattern[i][3];
		dead1[pattern[i][1]].index = i;
	}
	// d4b7
	for (i = 61; i <= 62; i++)
	{
		d4b7[pattern[i][1]].is = true;
		d4b7[pattern[i][1]].macro = pattern[i][2];
		d4b7[pattern[i][1]].mask = pattern[i][3];
		d4b7[pattern[i][1]].index = i;
	}
	// d3b8
	for (i = 63; i <= 64; i++)
	{
		d3b8[pattern[i][1]].is = true;
		d3b8[pattern[i][1]].macro = pattern[i][2];
		d3b8[pattern[i][1]].mask = pattern[i][3];
		d3b8[pattern[i][1]].index = i;
	}
	// d3b7
	for (i = 65; i <= 68; i++)
	{
		d3b7[pattern[i][1]].is = true;
		d3b7[pattern[i][1]].macro = pattern[i][2];
		d3b7[pattern[i][1]].mask = pattern[i][3];
		d3b7[pattern[i][1]].index = i;
	}
	// b6
	for (i = 69; i <= 72; i++)
	{
		b6[pattern[i][1]].is = true;
		b6[pattern[i][1]].macro = pattern[i][2];
		b6[pattern[i][1]].mask = pattern[i][3];
		b6[pattern[i][1]].index = i;
	}
}

// Calculate pattern for a binary line. MSB is on the left and LSB is on the right.
static void binary_line_cnt(u16* tab, const u32 index, const int len, const u16 rule)
{
	u32 mask = 0;
	u16 fiv[16], six[15], sev[14], eig[13], nin[12];
	int bound5 = len - 4;
	int bound6 = len - 5;
	int bound7 = len - 6;
	int bound8 = len - 7;
	int bound9 = len - 8;

	int i, j, ind1 = 0, ind2 = PAT_SIZE;

	// reset tab entry
	for (i = 0; i < PAT_SIZE + SPE_SIZE; i++)
		tab[i] = INVALID;

	// calculate patterns of different length
	for (i = 0; i < bound5; i++)
		fiv[i] = (index >> i) & 0x001f;

	for (i = 0; i < bound6; i++)
		six[i] = (index >> i) & 0x003f;

	for (i = 0; i < bound7; i++)
		sev[i] = (index >> i) & 0x007f;

	for (i = 0; i < bound8; i++)
		eig[i] = (index >> i) & 0x00ff;

	for (i = 0; i < bound9; i++)
		nin[i] = (index >> i) & 0x01ff;

	// long
	if (rule == STANDARD || rule == RENJU)
	{
		for (i = 0; i < bound6; i++)
		{
			if (lon[six[i]].is && !((mask >> i) & 0x3f))
			{
				tab[ind1++] = lon[six[i]].macro;
				mask |= lon[six[i]].mask << i;
			}
		}
	}
	// five
	for (i = 0; i < bound5; i++)
	{
		if (five[fiv[i]].is && !((mask >> i) & 0x1f))
		{
			tab[ind1++] = five[fiv[i]].macro;
			mask |= five[fiv[i]].mask << i;
		}
	}
	if (rule == STANDARD || rule == RENJU)
	{
		// d3b8
		for (i = 0; i < bound8; i++)
		{
			if (d3b8[eig[i]].is && !((mask >> i) & 0xff))
			{
				for (j = 4; j <= 5; j++)
				{
					if (ind2 < PAT_SIZE + SPE_SIZE)
					{
						tab[ind2++] = FD4;
						tab[ind2++] = pattern[d3b8[eig[i]].index][j] + i;
					}
				}
				tab[ind1++] = d3b8[eig[i]].macro;
				mask |= d3b8[eig[i]].mask << i;
			}
		}
		// d4b7
		for (i = 0; i < bound7; i++)
		{
			if (d4b7[sev[i]].is && !((mask >> i) & 0x7f))
			{
				for (j = 4; j <= 4; j++)
				{
					if (ind2 < PAT_SIZE + SPE_SIZE)
					{
						tab[ind2++] = DD4;
						tab[ind2++] = pattern[d4b7[sev[i]].index][j] + i;
					}
				}
				tab[ind1++] = d4b7[sev[i]].macro;
				mask |= d4b7[sev[i]].mask << i;
			}
		}
		// b6
		for (i = 0; i < bound6; i++)
		{
			if (b6[six[i]].is && !((mask >> i) & 0x3f))
				mask |= b6[six[i]].mask << i;
		}
	}
	// free4
	for (i = 0; i < bound6; i++)
	{
		if (free4[six[i]].is && !((mask >> i) & 0x3f))
		{
			tab[ind1++] = free4[six[i]].macro;
			mask |= free4[six[i]].mask << i;
		}
	}
	// d4d4b9
	for (i = 0; i < bound9; i++)
	{
		if (d4d4b9[nin[i]].is && !((mask >> i) & 0x1ff))
		{
			for (j = 4; j <= 5; j++)
			{
				if (ind2 < PAT_SIZE + SPE_SIZE)
				{
					tab[ind2++] = DD4;
					tab[ind2++] = pattern[d4d4b9[nin[i]].index][j] + i;
				}
			}
			tab[ind1++] = d4d4b9[nin[i]].macro;
			tab[ind1++] = d4d4b9[nin[i]].macro;
			mask |= d4d4b9[nin[i]].mask << i;
		}
	}
	// d4d4b8
	for (i = 0; i < bound8; i++)
	{
		if (d4d4b8[eig[i]].is && !((mask >> i) & 0xff))
		{
			for (j = 4; j <= 5; j++)
			{
				if (ind2 < PAT_SIZE + SPE_SIZE)
				{
					tab[ind2++] = DD4;
					tab[ind2++] = pattern[d4d4b8[eig[i]].index][j] + i;
				}
			}
			tab[ind1++] = d4d4b8[eig[i]].macro;
			tab[ind1++] = d4d4b8[eig[i]].macro;
			mask |= d4d4b8[eig[i]].mask << i;
		}
	}
	// d4d4b7
	for (i = 0; i < bound7; i++)
	{
		if (d4d4b7[sev[i]].is && !((mask >> i) & 0x7f))
		{
			for (j = 4; j <= 5; j++)
			{
				if (ind2 < PAT_SIZE + SPE_SIZE)
				{
					tab[ind2++] = DD4;
					tab[ind2++] = pattern[d4d4b7[sev[i]].index][j] + i;
				}
			}
			tab[ind1++] = d4d4b7[sev[i]].macro;
			tab[ind1++] = d4d4b7[sev[i]].macro;
			mask |= d4d4b7[sev[i]].mask << i;
		}
	}
	// dead4
	for (i = 0; i < bound5; i++)
	{
		if (dead4[fiv[i]].is && !((mask >> i) & 0x1f))
		{
			for (j = 4; j <= 4; j++)
			{
				if (ind2 < PAT_SIZE + SPE_SIZE)
				{
					tab[ind2++] = DD4;
					tab[ind2++] = pattern[dead4[fiv[i]].index][j] + i;
				}
			}
			tab[ind1++] = dead4[fiv[i]].macro;
			mask |= dead4[fiv[i]].mask << i;
		}
	}
	// d3b7
	if (rule == STANDARD || rule == RENJU)
	{
		for (i = 0; i < bound7; i++)
		{
			if (d3b7[sev[i]].is && !((mask >> i) & 0x7f))
			{
				for (j = 4; j <= 5; j++)
				{
					if (ind2 < PAT_SIZE + SPE_SIZE)
					{
						tab[ind2++] = FD4;
						tab[ind2++] = pattern[d3b7[sev[i]].index][j] + i;
					}
				}
				tab[ind1++] = d3b7[sev[i]].macro;
				mask |= d3b7[sev[i]].mask << i;
			}
		}
	}
	// free3a
	for (i = 0; i < bound7; i++)
	{
		if (free3a[sev[i]].is && !((mask >> i) & 0x7f))
		{
			for (j = 4; j <= 5; j++)
			{
				if (ind2 < PAT_SIZE + SPE_SIZE)
				{
					tab[ind2++] = DF3;
					tab[ind2++] = pattern[free3a[sev[i]].index][j] + i;
				}
			}
			tab[ind1++] = free3a[sev[i]].macro;
			mask |= free3a[sev[i]].mask << i;
		}
	}
	// free3
	for (i = 0; i < bound6; i++)
	{
		if (free3[six[i]].is && !((mask >> i) & 0x3f))
		{
			for (j = 4; j <= 6; j++)
			{
				if (ind2 < PAT_SIZE + SPE_SIZE)
				{
					tab[ind2++] = DF3;
					tab[ind2++] = pattern[free3[six[i]].index][j] + i;
				}
			}
			tab[ind1++] = free3[six[i]].macro;
			mask |= free3[six[i]].mask << i;
		}
	}
	// dead3
	for (i = 0; i < bound5; i++)
	{
		if (dead3[fiv[i]].is && !((mask >> i) & 0x1f))
		{
			for (j = 4; j <= 5; j++)
			{
				if (ind2 < PAT_SIZE + SPE_SIZE)
				{
					tab[ind2++] = FD4;
					tab[ind2++] = pattern[dead3[fiv[i]].index][j] + i;
				}
			}
			tab[ind1++] = dead3[fiv[i]].macro;
			mask |= dead3[fiv[i]].mask << i;
		}
	}
	// free2b
	for (i = 0; i < bound8; i++)
	{
		if (free2b[eig[i]].is && !((mask >> i) & 0xff))
		{
			for (j = 4; j <= 7; j++)
			{
				if (ind2 < PAT_SIZE + SPE_SIZE)
				{
					tab[ind2++] = FF3;
					tab[ind2++] = pattern[free2b[eig[i]].index][j] + i;
				}
			}
#if SPE_D3F2
			for (j = 12; j <= 13; j++)
			{
				if (ind2 < PAT_SIZE + SPE_SIZE)
				{
					tab[ind2++] = DF2P;
					tab[ind2++] = pattern[free2b[eig[i]].index][j] + i;
				}
			}
#endif
			tab[ind1++] = free2b[eig[i]].macro;
			mask |= free2b[eig[i]].mask << i;
		}
	}
	// free2a
	for (i = 0; i < bound7; i++)
	{
		if (free2a[sev[i]].is && !((mask >> i) & 0x7f))
		{
			for (j = 4; j <= 6; j++)
			{
				if (ind2 < PAT_SIZE + SPE_SIZE)
				{
					tab[ind2++] = FF3;
					tab[ind2++] = pattern[free2a[sev[i]].index][j] + i;
				}
			}
#if SPE_D3F2
			for (j = 9; j <= 9; j++)
			{
				if (ind2 < PAT_SIZE + SPE_SIZE)
				{
					tab[ind2++] = DF2;
					tab[ind2++] = pattern[free2a[sev[i]].index][j] + i;
				}
			}
			for (j = 12; j <= 13; j++)
			{
				if (ind2 < PAT_SIZE + SPE_SIZE)
				{
					tab[ind2++] = DF2P;
					tab[ind2++] = pattern[free2a[sev[i]].index][j] + i;
				}
			}
#endif
			tab[ind1++] = free2a[sev[i]].macro;
			mask |= free2a[sev[i]].mask << i;
		}
	}
	// free2
	for (i = 0; i < bound6; i++)
	{
		if (free2[six[i]].is && !((mask >> i) & 0x3f))
		{
			for (j = 4; j <= 5; j++)
			{
				if (ind2 < PAT_SIZE + SPE_SIZE)
				{
					tab[ind2++] = FF3;
					tab[ind2++] = pattern[free2[six[i]].index][j] + i;
				}
			}
#if SPE_D3F2
			for (j = 9; j <= 10; j++)
			{
				if (ind2 < PAT_SIZE + SPE_SIZE)
				{
					tab[ind2++] = DF2;
					tab[ind2++] = pattern[free2[six[i]].index][j] + i;
				}
			}
			for (j = 12; j <= 13; j++)
			{
				if (ind2 < PAT_SIZE + SPE_SIZE)
				{
					tab[ind2++] = DF2P;
					tab[ind2++] = pattern[free2[six[i]].index][j] + i;
				}
			}
#endif
			tab[ind1++] = free2[six[i]].macro;
			mask |= free2[six[i]].mask << i;
		}
	}
	// dead2
	for (i = 0; i < bound5; i++)
	{
		if (dead2[fiv[i]].is && !((mask >> i) & 0x1f))
		{
#if SPE_D3F2
			for (j = 4; j <= 6; j++)
			{
				if (ind2 < PAT_SIZE + SPE_SIZE)
				{
					tab[ind2++] = FD3;
					tab[ind2++] = pattern[dead2[fiv[i]].index][j] + i;
				}
			}
#endif
			tab[ind1++] = dead2[fiv[i]].macro;
			mask |= dead2[fiv[i]].mask << i;
		}
	}
	// free1c
	for (i = 0; i < bound9; i++)
	{
		if (free1c[nin[i]].is && !((mask >> i) & 0x1ff))
		{
#if SPE_D3F2
			for (j = 4; j <= 9; j++)
			{
				if (ind2 < PAT_SIZE + SPE_SIZE)
				{
					tab[ind2++] = FF2;
					tab[ind2++] = pattern[free1c[nin[i]].index][j] + i;
				}
			}
#endif
			tab[ind1++] = free1c[nin[i]].macro;
			mask |= free1c[nin[i]].mask << i;
		}
	}
	// free1b
	for (i = 0; i < bound8; i++)
	{
		if (free1b[eig[i]].is && !((mask >> i) & 0xff))
		{
#if SPE_D3F2
			for (j = 4; j <= 8; j++)
			{
				if (ind2 < PAT_SIZE + SPE_SIZE)
				{
					tab[ind2++] = FF2;
					tab[ind2++] = pattern[free1b[eig[i]].index][j] + i;
				}
			}
#endif
			tab[ind1++] = free1b[eig[i]].macro;
			mask |= free1b[eig[i]].mask << i;
		}
	}
	// free1a
	for (i = 0; i < bound7; i++)
	{
		if (free1a[sev[i]].is && !((mask >> i) & 0x7f))
		{
#if SPE_D3F2
			for (j = 4; j <= 7; j++)
			{
				if (ind2 < PAT_SIZE + SPE_SIZE)
				{
					tab[ind2++] = FF2;
					tab[ind2++] = pattern[free1a[sev[i]].index][j] + i;
				}
			}
#endif
			tab[ind1++] = free1a[sev[i]].macro;
			mask |= free1a[sev[i]].mask << i;
		}
	}
	// free1
	for (i = 0; i < bound6; i++)
	{
		if (free1[six[i]].is && !((mask >> i) & 0x3f))
		{
#if SPE_D3F2
			for (j = 4; j <= 6; j++)
			{
				if (ind2 < PAT_SIZE + SPE_SIZE)
				{
					tab[ind2++] = FF2;
					tab[ind2++] = pattern[free1[six[i]].index][j] + i;
				}
			}
#endif
			tab[ind1++] = free1[six[i]].macro;
			mask |= free1[six[i]].mask << i;
		}
	}
	// dead1
	for (i = 0; i < bound5; i++)
	{
		if (dead1[fiv[i]].is && !((mask >> i) & 0x1f))
		{
			tab[ind1++] = dead1[fiv[i]].macro;
			mask |= dead1[fiv[i]].mask << i;
		}
	}
}

// Generate freestyle pattern tables.
void pat_init_freestyle15()
{
	u32 i;
	helper_init();

	for (i = 0; i < pow2[1][15]; i++)
	{
		if (i < P15)
			binary_line_cnt(Pat15w[i], i, 15, FREESTYLE);
		if (i < P14)
			binary_line_cnt(Pat14w[i], i, 14, FREESTYLE);
		if (i < P13)
			binary_line_cnt(Pat13w[i], i, 13, FREESTYLE);
		if (i < P12)
			binary_line_cnt(Pat12w[i], i, 12, FREESTYLE);
		if (i < P11)
			binary_line_cnt(Pat11w[i], i, 11, FREESTYLE);
		if (i < P10)
			binary_line_cnt(Pat10w[i], i, 10, FREESTYLE);
		if (i < P9)
			binary_line_cnt(Pat9w[i], i, 9, FREESTYLE);
		if (i < P8)
			binary_line_cnt(Pat8w[i], i, 8, FREESTYLE);
		if (i < P7)
			binary_line_cnt(Pat7w[i], i, 7, FREESTYLE);
		if (i < P6)
			binary_line_cnt(Pat6w[i], i, 6, FREESTYLE);
		if (i < P5)
			binary_line_cnt(Pat5w[i], i, 5, FREESTYLE);
	}
}

// Generate pattern tables.
void pat_init(const u16 rule, const u16 size)
{
	u32 i;
	helper_init();

	for (i = 0; i < pow2[1][size]; i++)
	{
#if 0
		if (i < P20)
			binary_line_cnt(Pat20[i], i, 20, rule);
		if (i < P19)
			binary_line_cnt(Pat19[i], i, 19, rule);
		if (i < P18)
			binary_line_cnt(Pat18[i], i, 18, rule);
		if (i < P17)
			binary_line_cnt(Pat17[i], i, 17, rule);
		if (i < P16)
			binary_line_cnt(Pat16[i], i, 16, rule);
#endif
		if (i < P15)
			binary_line_cnt(Pat15[i], i, 15, rule);
		if (i < P14)
			binary_line_cnt(Pat14[i], i, 14, rule);
		if (i < P13)
			binary_line_cnt(Pat13[i], i, 13, rule);
		if (i < P12)
			binary_line_cnt(Pat12[i], i, 12, rule);
		if (i < P11)
			binary_line_cnt(Pat11[i], i, 11, rule);
		if (i < P10)
			binary_line_cnt(Pat10[i], i, 10, rule);
		if (i < P9)
			binary_line_cnt(Pat9[i], i, 9, rule);
		if (i < P8)
			binary_line_cnt(Pat8[i], i, 8, rule);
		if (i < P7)
			binary_line_cnt(Pat7[i], i, 7, rule);
		if (i < P6)
			binary_line_cnt(Pat6[i], i, 6, rule);
		if (i < P5)
			binary_line_cnt(Pat5[i], i, 5, rule);
	}
}

/*******************************************************************************
						 Pattern Table Look Up Functions						
*******************************************************************************/
// Step type: R-0, D-1, DR-2, DL-3.
// e.g. step_type[R]
static u32 step_type[24] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 3, 1,
	2, 0, 0, 3, 1, 2, 0, 0
};

// Special point type.
// e.g. spe_type[BLACK][FD4]
static u16 spe_type[2][8] = {
	{ BDD4, BDF3, BFD4, BFF3, BDF2, BDF2P, BFD3, BFF2 },
	{ WDD4, WDF3, WFD4, WFF3, WDF2, WDF2P, WFD3, WFF2 }
};

// Masks to reset special point record of one color in one line for a point.
// e.g. spe(bd)[pos] &= line_clr[BLACK][step_type[step]];
static u64 line_clr[2][4] = {
	{ 0xffffffffeeeeeeee, 0xffffffffdddddddd, 0xffffffffbbbbbbbb, 0xffffffff77777777 },
	{ 0xeeeeeeeeffffffff, 0xddddddddffffffff, 0xbbbbbbbbffffffff, 0x77777777ffffffff }
};

// Masks to record one special point record.
// e.g. spe(bd)[pos] |= spe_mask[BFD4][R];
static u64 spe_mask[16][4] = {
	{ 0x0000000000000001, 0x0000000000000002, 0x0000000000000004, 0x0000000000000008 },
	{ 0x0000000000000010, 0x0000000000000020, 0x0000000000000040, 0x0000000000000080 },
	{ 0x0000000000000100, 0x0000000000000200, 0x0000000000000400, 0x0000000000000800 },
	{ 0x0000000000001000, 0x0000000000002000, 0x0000000000004000, 0x0000000000008000 },
	{ 0x0000000000010000, 0x0000000000020000, 0x0000000000040000, 0x0000000000080000 },
	{ 0x0000000000100000, 0x0000000000200000, 0x0000000000400000, 0x0000000000800000 },
	{ 0x0000000001000000, 0x0000000002000000, 0x0000000004000000, 0x0000000008000000 },
	{ 0x0000000010000000, 0x0000000020000000, 0x0000000040000000, 0x0000000080000000 },
	{ 0x0000000100000000, 0x0000000200000000, 0x0000000400000000, 0x0000000800000000 },
	{ 0x0000001000000000, 0x0000002000000000, 0x0000004000000000, 0x0000008000000000 },
	{ 0x0000010000000000, 0x0000020000000000, 0x0000040000000000, 0x0000080000000000 },
	{ 0x0000100000000000, 0x0000200000000000, 0x0000400000000000, 0x0000800000000000 },
	{ 0x0001000000000000, 0x0002000000000000, 0x0004000000000000, 0x0008000000000000 },
	{ 0x0010000000000000, 0x0020000000000000, 0x0040000000000000, 0x0080000000000000 },
	{ 0x0100000000000000, 0x0200000000000000, 0x0400000000000000, 0x0800000000000000 },
	{ 0x1000000000000000, 0x2000000000000000, 0x4000000000000000, 0x8000000000000000 }
};

// Update pattern and special point record of an interval.
void line_update(board_t* bd, const u16 start, const u16 len, const u16 step, const u16 color, const u16 op)
{
	if (len < 5)
		return;
	
	u32 index = 0;
	u16 tmp = start;
	u16 i, *tab;

	if (op == SUB)
	{
		for (i = 0; i < len; i++)
		{
			spe(bd)[tmp] &= line_clr[color - 1][step_type[step]];
			index |= pow2[bd->arr[tmp]][i];
			tmp += step;
		}
	}
	else
	{
		for (i = 0; i < len; i++)
		{
			index |= pow2[bd->arr[tmp]][i];
			tmp += step;
		}
	}

	if (bd->rule == RENJU && color != bd->fcolor)
	{
		switch (len)
		{
		case 5:
			tab = Pat5w[index];
			break;
		case 6:
			tab = Pat6w[index];
			break;
		case 7:
			tab = Pat7w[index];
			break;
		case 8:
			tab = Pat8w[index];
			break;
		case 9:
			tab = Pat9w[index];
			break;
		case 10:
			tab = Pat10w[index];
			break;
		case 11:
			tab = Pat11w[index];
			break;
		case 12:
			tab = Pat12w[index];
			break;
		case 13:
			tab = Pat13w[index];
			break;
		case 14:
			tab = Pat14w[index];
			break;
		case 15:
			tab = Pat15w[index];
			break;
		default:
			return;
		}
	}
	else
	{
		switch (len)
		{
		case 5:
			tab = Pat5[index];
			break;
		case 6:
			tab = Pat6[index];
			break;
		case 7:
			tab = Pat7[index];
			break;
		case 8:
			tab = Pat8[index];
			break;
		case 9:
			tab = Pat9[index];
			break;
		case 10:
			tab = Pat10[index];
			break;
		case 11:
			tab = Pat11[index];
			break;
		case 12:
			tab = Pat12[index];
			break;
		case 13:
			tab = Pat13[index];
			break;
		case 14:
			tab = Pat14[index];
			break;
		case 15:
			tab = Pat15[index];
			break;
		case 16:
			tab = Pat16[index];
			break;
		case 17:
			tab = Pat17[index];
			break;
		case 18:
			tab = Pat18[index];
			break;
		case 19:
			tab = Pat19[index];
			break;
		case 20:
			tab = Pat20[index];
			break;
		default:
			return;
		}
	}
	
	if (op == ADD)
	{
		for (i = 0; i < PAT_SIZE; i++)
		{
			if (tab[i] != INVALID)
				pattern_inc(pinc(bd), tab[i], color);
			else
				break;
		}
		for (i = PAT_SIZE; i < PAT_SIZE + SPE_SIZE; i += 2)
		{
			if (tab[i] != INVALID)
				spe(bd)[start + tab[i + 1] * step] |= spe_mask[spe_type[color - 1][tab[i]]][step_type[step]];
			else
				break;
		}
	}
	else if (op == SUB)
	{
		for (i = 0; i < PAT_SIZE; i++)
		{
			if (tab[i] != INVALID)
				pattern_dec(pinc(bd), tab[i], color);
			else
				break;
		}
	}
}

// Update pattern of an interval.
void line_update_fast(board_t* bd, const u16 start, const u16 len, const u16 step, const u16 color, const u16 op)
{
	if (len < 5)
		return;

	u32 index = 0;
	u16 tmp = start;
	u16 i, *tab;

	for (i = 0; i < len; i++)
	{
		index |= pow2[bd->arr[tmp]][i];
		tmp += step;
	}

	if (bd->rule == RENJU && color != bd->fcolor)
	{
		switch (len)
		{
		case 5:
			tab = Pat5w[index];
			break;
		case 6:
			tab = Pat6w[index];
			break;
		case 7:
			tab = Pat7w[index];
			break;
		case 8:
			tab = Pat8w[index];
			break;
		case 9:
			tab = Pat9w[index];
			break;
		case 10:
			tab = Pat10w[index];
			break;
		case 11:
			tab = Pat11w[index];
			break;
		case 12:
			tab = Pat12w[index];
			break;
		case 13:
			tab = Pat13w[index];
			break;
		case 14:
			tab = Pat14w[index];
			break;
		case 15:
			tab = Pat15w[index];
			break;
		default:
			return;
		}
	}
	else
	{
		switch (len)
		{
		case 5:
			tab = Pat5[index];
			break;
		case 6:
			tab = Pat6[index];
			break;
		case 7:
			tab = Pat7[index];
			break;
		case 8:
			tab = Pat8[index];
			break;
		case 9:
			tab = Pat9[index];
			break;
		case 10:
			tab = Pat10[index];
			break;
		case 11:
			tab = Pat11[index];
			break;
		case 12:
			tab = Pat12[index];
			break;
		case 13:
			tab = Pat13[index];
			break;
		case 14:
			tab = Pat14[index];
			break;
		case 15:
			tab = Pat15[index];
			break;
		case 16:
			tab = Pat16[index];
			break;
		case 17:
			tab = Pat17[index];
			break;
		case 18:
			tab = Pat18[index];
			break;
		case 19:
			tab = Pat19[index];
			break;
		case 20:
			tab = Pat20[index];
			break;
		default:
			return;
		}
	}

	if (op == ADD)
	{
		for (i = 0; i < PAT_SIZE; i++)
		{
			if (tab[i] != INVALID)
				pattern_inc(pinc(bd), tab[i], color);
			else
				break;
		}
	}
	else if (op == SUB)
	{
		for (i = 0; i < PAT_SIZE; i++)
		{
			if (tab[i] != INVALID)
				pattern_dec(pinc(bd), tab[i], color);
			else
				break;
		}
	}
}
