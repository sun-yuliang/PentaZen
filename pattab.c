/*      _____                __    ______
 *     / ___ \              / /   /___  /
 *    / /__/ /___  ____  __/ /_______/ /    ____  ____
 *   / _____/ __ \/ __ \/_   _/ __  / /    / __ \/ __ \
 *  / /    /  ___/ / / / / /_/ /_/ / /____/  ___/ / / /
 * /_/     \____/_/ /_/ /___/\__,_/______/\____/_/ /_/
 *
 * PentaZen, a Gomoku/Renju playing engine developed by Sun.
 */

#include "pattab.h"
#include "pattern.h"

// Pattern weight table.
static Score PatternWeight[9][4] = {
    {   0,   0,   0,   0 },        // C6
    { 255,   0,   0,   0 },        // C5
    { 255,   0,   0,   0 },        // F4
    { 106,   0,   0,   0 },        // B4
    { 100, 108,   0,   0 },        // F3
    {  36,   0,   0,   0 },        // B3
    {  36,  36,  36,   0 },        // F2
    {   7,   0,   0,   0 },        // B2
    {   7,   7,   7,  11 },        // F1
};

// Array exhausting pattern type, index, mask and defending points.
static u16 PatternList[74][8] =
{
    // 0-2
    { C6, 0, 0x3f, 0x3f, X, X, X, X },        // 111111
    { C5, 0, 0x1f, 0x1f, X, X, X, X },        // 11111
    { F4, 0, 0x1e, 0x1e, X, X, X, X },        // 011110
    // 3-5
    { B4, 0, 0x1d7,0x1d7,7, 3, 5, X },        // 111010111
    { B4, 0, 0xdb, 0xdb, 7, 2, 5, X },        // 11011011
    { B4, 0, 0x5d, 0x5d, 7, 1, 5, X },        // 1011101
    // 6-10
    { B4, 0, 0x0f, 0x0f, 6, 4, X, X },        // 01111
    { B4, 0, 0x17, 0x17, 6, 3, X, X },        // 10111
    { B4, 0, 0x1b, 0x1b, 6, 2, X, X },        // 11011
    { B4, 0, 0x1d, 0x1d, 6, 1, X, X },        // 11101
    { B4, 0, 0x1e, 0x1e, 6, 0, X, X },        // 11110
    // 11-15
    { F3, 1, 0x1c, 0x1c, 7, 1, 5, X },        // 0011100
    { F3, 0, 0x0e, 0x0e, 8, 0, 4, 5 },        // 001110
    { F3, 0, 0x16, 0x16, 8, 0, 3, 5 },        // 010110
    { F3, 0, 0x1a, 0x1a, 8, 0, 2, 5 },        // 011010
    { F3, 0, 0x1c, 0x1c, 8, 0, 1, 5 },        // 011100
    // 16-25
    { B3, 0, 0x07, 0x07, X, X, X, X },        // 00111
    { B3, 0, 0x0b, 0x0b, X, X, X, X },        // 01011
    { B3, 0, 0x0d, 0x0d, X, X, X, X },        // 01101
    { B3, 0, 0x0e, 0x0e, X, X, X, X },        // 01110
    { B3, 0, 0x13, 0x13, X, X, X, X },        // 10011
    { B3, 0, 0x15, 0x15, X, X, X, X },        // 10101
    { B3, 0, 0x16, 0x16, X, X, X, X },        // 10110
    { B3, 0, 0x19, 0x19, X, X, X, X },        // 11001
    { B3, 0, 0x1a, 0x1a, X, X, X, X },        // 11010
    { B3, 0, 0x1c, 0x1c, X, X, X, X },        // 11100
    // 26-35
    { F2, 2, 0x18, 0x18, X, X, X, X },        // 00011000
    { F2, 1, 0x0c, 0x0c, X, X, X, X },        // 0001100
    { F2, 1, 0x14, 0x14, X, X, X, X },        // 0010100
    { F2, 1, 0x18, 0x18, X, X, X, X },        // 0011000
    { F2, 0, 0x06, 0x06, X, X, X, X },        // 000110
    { F2, 0, 0x0a, 0x0a, X, X, X, X },        // 001010
    { F2, 0, 0x0c, 0x0c, X, X, X, X },        // 001100
    { F2, 0, 0x12, 0x12, X, X, X, X },        // 010010
    { F2, 0, 0x14, 0x14, X, X, X, X },        // 010100
    { F2, 0, 0x18, 0x18, X, X, X, X },        // 011000
    // 36-45
    { B2, 0, 0x03, 0x03, X, X, X, X },        // 00011
    { B2, 0, 0x05, 0x05, X, X, X, X },        // 00101
    { B2, 0, 0x06, 0x06, X, X, X, X },        // 00110
    { B2, 0, 0x09, 0x09, X, X, X, X },        // 01001
    { B2, 0, 0x0a, 0x0a, X, X, X, X },        // 01010
    { B2, 0, 0x0c, 0x0c, X, X, X, X },        // 01100
    { B2, 0, 0x11, 0x11, X, X, X, X },        // 10001
    { B2, 0, 0x12, 0x12, X, X, X, X },        // 10010
    { B2, 0, 0x14, 0x14, X, X, X, X },        // 10100
    { B2, 0, 0x18, 0x18, X, X, X, X },        // 11000
    // 46-55
    { F1, 3, 0x10, 0x10, X, X, X, X },        // 000010000
    { F1, 2, 0x08, 0x08, X, X, X, X },        // 00001000
    { F1, 2, 0x10, 0x10, X, X, X, X },        // 00010000
    { F1, 1, 0x04, 0x04, X, X, X, X },        // 0000100
    { F1, 1, 0x08, 0x08, X, X, X, X },        // 0001000
    { F1, 1, 0x10, 0x10, X, X, X, X },        // 0010000
    { F1, 0, 0x02, 0x02, X, X, X, X },        // 000010
    { F1, 0, 0x04, 0x04, X, X, X, X },        // 000100
    { F1, 0, 0x08, 0x08, X, X, X, X },        // 001000
    { F1, 0, 0x10, 0x10, X, X, X, X },        // 010000
    // 56-64
    { X,  0, 0xbd, 0xbd, X, X, X, X },        // 10111101
    { B3, 0, 0x3b, 0x3b, X, X, X, X },        // 00111011
    { B3, 0, 0xdc, 0xdc, X, X, X, X },        // 11011100
    { B4, 0, 0x3d, 0x3d, 6, 6, X, X },        // 0111101
    { B4, 0, 0x5e, 0x5e, 6, 0, X, X },        // 1011110
    { X,  0, 0x2f, 0x2f, X, X, X, X },        // 101111
    { X,  0, 0x37, 0x37, X, X, X, X },        // 110111
    { X,  0, 0x3b, 0x3b, X, X, X, X },        // 111011
    { X,  0, 0x3d, 0x3d, X, X, X, X },        // 111101
    // 65-73
    { B3, 0, 0x139,0x38, X, X, X, X },        // 100111001
    { F3, 0, 0x39, 0x38, 8, 2, 6, 7 },        // 00111001
    { F3, 0, 0x9c, 0x1c, 8, 0, 1, 5 },        // 10011100
    { B3, 0, 0x2d, 0x2d, X, X, X, X },        // 0101101
    { B3, 0, 0x35, 0x35, X, X, X, X },        // 0110101
    { B3, 0, 0x39, 0x38, X, X, X, X },        // 0111001
    { B3, 0, 0x4e, 0x0e, X, X, X, X },        // 1001110
    { B3, 0, 0x56, 0x56, X, X, X, X },        // 1010110
    { B3, 0, 0x5a, 0x5a, X, X, X, X },        // 1011010
};

// Freetyle rule tables.
u8 Pat5f[IIE5][PATTAB_SIZE];
u8 Pat6f[IIE6][PATTAB_SIZE];
u8 Pat7f[IIE7][PATTAB_SIZE];
u8 Pat8f[IIE8][PATTAB_SIZE];
u8 Pat9f[IIE9][PATTAB_SIZE];
u8 Pat10f[IIE10][PATTAB_SIZE];
u8 Pat11f[IIE11][PATTAB_SIZE];
u8 Pat12f[IIE12][PATTAB_SIZE];
u8 Pat13f[IIE13][PATTAB_SIZE];
u8 Pat14f[IIE14][PATTAB_SIZE];
u8 Pat15f[IIE15][PATTAB_SIZE];

// Standard/Renju rule tables.
u8 Pat5[IIE5][PATTAB_SIZE];
u8 Pat6[IIE6][PATTAB_SIZE];
u8 Pat7[IIE7][PATTAB_SIZE];
u8 Pat8[IIE8][PATTAB_SIZE];
u8 Pat9[IIE9][PATTAB_SIZE];
u8 Pat10[IIE10][PATTAB_SIZE];
u8 Pat11[IIE11][PATTAB_SIZE];
u8 Pat12[IIE12][PATTAB_SIZE];
u8 Pat13[IIE13][PATTAB_SIZE];
u8 Pat14[IIE14][PATTAB_SIZE];
u8 Pat15[IIE15][PATTAB_SIZE];

// get_entry function array.
u8* (*get_entry[2][3][BD_SIZE + 1])(const u32);

inline static u8* get_entry_5f(const u32 ind)
{
    return Pat5f[ind];
}

inline static u8* get_entry_6f(const u32 ind)
{
    return Pat6f[ind];
}

inline static u8* get_entry_7f(const u32 ind)
{
    return Pat7f[ind];
}

inline static u8* get_entry_8f(const u32 ind)
{
    return Pat8f[ind];
}

inline static u8* get_entry_9f(const u32 ind)
{
    return Pat9f[ind];
}

inline static u8* get_entry_10f(const u32 ind)
{
    return Pat10f[ind];
}

inline static u8* get_entry_11f(const u32 ind)
{
    return Pat11f[ind];
}

inline static u8* get_entry_12f(const u32 ind)
{
    return Pat12f[ind];
}

inline static u8* get_entry_13f(const u32 ind)
{
    return Pat13f[ind];
}

inline static u8* get_entry_14f(const u32 ind)
{
    return Pat14f[ind];
}

inline static u8* get_entry_15f(const u32 ind)
{
    return Pat15f[ind];
}

inline static u8* get_entry_5(const u32 ind)
{
    return Pat5[ind];
}

inline static u8* get_entry_6(const u32 ind)
{
    return Pat6[ind];
}

inline static u8* get_entry_7(const u32 ind)
{
    return Pat7[ind];
}

inline static u8* get_entry_8(const u32 ind)
{
    return Pat8[ind];
}

inline static u8* get_entry_9(const u32 ind)
{
    return Pat9[ind];
}

inline static u8* get_entry_10(const u32 ind)
{
    return Pat10[ind];
}

inline static u8* get_entry_11(const u32 ind)
{
    return Pat11[ind];
}

inline static u8* get_entry_12(const u32 ind)
{
    return Pat12[ind];
}

inline static u8* get_entry_13(const u32 ind)
{
    return Pat13[ind];
}

inline static u8* get_entry_14(const u32 ind)
{
    return Pat14[ind];
}

inline static u8* get_entry_15(const u32 ind)
{
    return Pat15[ind];
}

// Helper structure.
typedef struct {
    bool is;
    u8 macro;
    u32 mask;
    u16 index;
} PHelper;

// Helper arrays.
static PHelper c6_6[IIE6];
static PHelper c5_5[IIE5];
static PHelper x_8[IIE8];
static PHelper b3_8[IIE8];
static PHelper b4_7[IIE7];
static PHelper x_6[IIE6];
static PHelper f4_6[IIE6];
static PHelper b4b4_9[IIE9];
static PHelper b4b4_8[IIE8];
static PHelper b4b4_7[IIE7];
static PHelper b4_5[IIE5];
static PHelper b3_9[IIE9];
static PHelper f3_8[IIE8];
static PHelper b3_7[IIE7];
static PHelper f3_7[IIE7];
static PHelper f3_6[IIE6];
static PHelper b3_5[IIE5];
static PHelper f2_8[IIE8];
static PHelper f2_7[IIE7];
static PHelper f2_6[IIE6];
static PHelper b2_5[IIE5];
static PHelper f1_9[IIE9];
static PHelper f1_8[IIE8];
static PHelper f1_7[IIE7];
static PHelper f1_6[IIE6];

// Initialize helper arrays.
static void phelper_init()
{
    memset(c6_6, 0, sizeof(c6_6));
    memset(c5_5, 0, sizeof(c5_5));
    memset(x_8, 0, sizeof(x_8));
    memset(b3_8, 0, sizeof(b3_8));
    memset(b4_7, 0, sizeof(b4_7));
    memset(x_6, 0, sizeof(x_6));
    memset(f4_6, 0, sizeof(f4_6));
    memset(b4b4_9, 0, sizeof(b4b4_9));
    memset(b4b4_8, 0, sizeof(b4b4_8));
    memset(b4b4_7, 0, sizeof(b4b4_7));
    memset(b4_5, 0, sizeof(b4_5));
    memset(b3_9, 0, sizeof(b3_9));
    memset(f3_8, 0, sizeof(f3_8));
    memset(b3_7, 0, sizeof(b3_7));
    memset(f3_7, 0, sizeof(f3_7));
    memset(f3_6, 0, sizeof(f3_6));
    memset(b3_5, 0, sizeof(b3_5));
    memset(f2_8, 0, sizeof(f2_8));
    memset(f2_7, 0, sizeof(f2_7));
    memset(f2_6, 0, sizeof(f2_6));
    memset(b2_5, 0, sizeof(b2_5));
    memset(f1_9, 0, sizeof(f1_9));
    memset(f1_8, 0, sizeof(f1_8));
    memset(f1_7, 0, sizeof(f1_7));
    memset(f1_6, 0, sizeof(f1_6));

    for (u8 i = 0; i <= 0; i++)
    {
        c6_6[PatternList[i][2]].is = true;
        c6_6[PatternList[i][2]].macro = (u8)PatternList[i][0];
        c6_6[PatternList[i][2]].mask = PatternList[i][3];
        c6_6[PatternList[i][2]].index = i;
    }
    for (u8 i = 1; i <= 1; i++)
    {
        c5_5[PatternList[i][2]].is = true;
        c5_5[PatternList[i][2]].macro = (u8)PatternList[i][0];
        c5_5[PatternList[i][2]].mask = PatternList[i][3];
        c5_5[PatternList[i][2]].index = i;
    }
    for (u8 i = 2; i <= 2; i++)
    {
        f4_6[PatternList[i][2]].is = true;
        f4_6[PatternList[i][2]].macro = (u8)PatternList[i][0];
        f4_6[PatternList[i][2]].mask = PatternList[i][3];
        f4_6[PatternList[i][2]].index = i;
    }
    for (u8 i = 3; i <= 3; i++)
    {
        b4b4_9[PatternList[i][2]].is = true;
        b4b4_9[PatternList[i][2]].macro = (u8)PatternList[i][0];
        b4b4_9[PatternList[i][2]].mask = PatternList[i][3];
        b4b4_9[PatternList[i][2]].index = i;
    }
    for (u8 i = 4; i <= 4; i++)
    {
        b4b4_8[PatternList[i][2]].is = true;
        b4b4_8[PatternList[i][2]].macro = (u8)PatternList[i][0];
        b4b4_8[PatternList[i][2]].mask = PatternList[i][3];
        b4b4_8[PatternList[i][2]].index = i;
    }
    for (u8 i = 5; i <= 5; i++)
    {
        b4b4_7[PatternList[i][2]].is = true;
        b4b4_7[PatternList[i][2]].macro = (u8)PatternList[i][0];
        b4b4_7[PatternList[i][2]].mask = PatternList[i][3];
        b4b4_7[PatternList[i][2]].index = i;
    }
    for (u8 i = 6; i <= 10; i++)
    {
        b4_5[PatternList[i][2]].is = true;
        b4_5[PatternList[i][2]].macro = (u8)PatternList[i][0];
        b4_5[PatternList[i][2]].mask = PatternList[i][3];
        b4_5[PatternList[i][2]].index = i;
    }
    for (u8 i = 11; i <= 11; i++)
    {
        f3_7[PatternList[i][2]].is = true;
        f3_7[PatternList[i][2]].macro = (u8)PatternList[i][0];
        f3_7[PatternList[i][2]].mask = PatternList[i][3];
        f3_7[PatternList[i][2]].index = i;
    }
    for (u8 i = 12; i <= 15; i++)
    {
        f3_6[PatternList[i][2]].is = true;
        f3_6[PatternList[i][2]].macro = (u8)PatternList[i][0];
        f3_6[PatternList[i][2]].mask = PatternList[i][3];
        f3_6[PatternList[i][2]].index = i;
    }
    for (u8 i = 16; i <= 25; i++)
    {
        b3_5[PatternList[i][2]].is = true;
        b3_5[PatternList[i][2]].macro = (u8)PatternList[i][0];
        b3_5[PatternList[i][2]].mask = PatternList[i][3];
        b3_5[PatternList[i][2]].index = i;
    }
    for (u8 i = 26; i <= 26; i++)
    {
        f2_8[PatternList[i][2]].is = true;
        f2_8[PatternList[i][2]].macro = (u8)PatternList[i][0];
        f2_8[PatternList[i][2]].mask = PatternList[i][3];
        f2_8[PatternList[i][2]].index = i;
    }
    for (u8 i = 27; i <= 29; i++)
    {
        f2_7[PatternList[i][2]].is = true;
        f2_7[PatternList[i][2]].macro = (u8)PatternList[i][0];
        f2_7[PatternList[i][2]].mask = PatternList[i][3];
        f2_7[PatternList[i][2]].index = i;
    }
    for (u8 i = 30; i <= 35; i++)
    {
        f2_6[PatternList[i][2]].is = true;
        f2_6[PatternList[i][2]].macro = (u8)PatternList[i][0];
        f2_6[PatternList[i][2]].mask = PatternList[i][3];
        f2_6[PatternList[i][2]].index = i;
    }
    for (u8 i = 36; i <= 45; i++)
    {
        b2_5[PatternList[i][2]].is = true;
        b2_5[PatternList[i][2]].macro = (u8)PatternList[i][0];
        b2_5[PatternList[i][2]].mask = PatternList[i][3];
        b2_5[PatternList[i][2]].index = i;
    }
    for (u8 i = 46; i <= 46; i++)
    {
        f1_9[PatternList[i][2]].is = true;
        f1_9[PatternList[i][2]].macro = (u8)PatternList[i][0];
        f1_9[PatternList[i][2]].mask = PatternList[i][3];
        f1_9[PatternList[i][2]].index = i;
    }
    for (u8 i = 47; i <= 48; i++)
    {
        f1_8[PatternList[i][2]].is = true;
        f1_8[PatternList[i][2]].macro = (u8)PatternList[i][0];
        f1_8[PatternList[i][2]].mask = PatternList[i][3];
        f1_8[PatternList[i][2]].index = i;
    }
    for (u8 i = 49; i <= 51; i++)
    {
        f1_7[PatternList[i][2]].is = true;
        f1_7[PatternList[i][2]].macro = (u8)PatternList[i][0];
        f1_7[PatternList[i][2]].mask = PatternList[i][3];
        f1_7[PatternList[i][2]].index = i;
    }
    for (u8 i = 52; i <= 55; i++)
    {
        f1_6[PatternList[i][2]].is = true;
        f1_6[PatternList[i][2]].macro = (u8)PatternList[i][0];
        f1_6[PatternList[i][2]].mask = PatternList[i][3];
        f1_6[PatternList[i][2]].index = i;
    }
    for (u8 i = 56; i <= 56; i++)
    {
        x_8[PatternList[i][2]].is = true;
        x_8[PatternList[i][2]].macro = (u8)PatternList[i][0];
        x_8[PatternList[i][2]].mask = PatternList[i][3];
        x_8[PatternList[i][2]].index = i;
    }
    for (u8 i = 57; i <= 58; i++)
    {
        b3_8[PatternList[i][2]].is = true;
        b3_8[PatternList[i][2]].macro = (u8)PatternList[i][0];
        b3_8[PatternList[i][2]].mask = PatternList[i][3];
        b3_8[PatternList[i][2]].index = i;
    }
    for (u8 i = 59; i <= 60; i++)
    {
        b4_7[PatternList[i][2]].is = true;
        b4_7[PatternList[i][2]].macro = (u8)PatternList[i][0];
        b4_7[PatternList[i][2]].mask = PatternList[i][3];
        b4_7[PatternList[i][2]].index = i;
    }
    for (u8 i = 61; i <= 64; i++)
    {
        x_6[PatternList[i][2]].is = true;
        x_6[PatternList[i][2]].macro = (u8)PatternList[i][0];
        x_6[PatternList[i][2]].mask = PatternList[i][3];
        x_6[PatternList[i][2]].index = i;
    }
    for (u8 i = 65; i <= 65; i++)
    {
        b3_9[PatternList[i][2]].is = true;
        b3_9[PatternList[i][2]].macro = (u8)PatternList[i][0];
        b3_9[PatternList[i][2]].mask = PatternList[i][3];
        b3_9[PatternList[i][2]].index = i;
    }
    for (u8 i = 66; i <= 67; i++)
    {
        f3_8[PatternList[i][2]].is = true;
        f3_8[PatternList[i][2]].macro = (u8)PatternList[i][0];
        f3_8[PatternList[i][2]].mask = PatternList[i][3];
        f3_8[PatternList[i][2]].index = i;
    }
    for (u8 i = 68; i <= 73; i++)
    {
        b3_7[PatternList[i][2]].is = true;
        b3_7[PatternList[i][2]].macro = (u8)PatternList[i][0];
        b3_7[PatternList[i][2]].mask = PatternList[i][3];
        b3_7[PatternList[i][2]].index = i;
    }
}

// Generate get_entry array.
static void generate_get_entry()
{
    for (u8 i = 0; i < 2; i++)
    {
        for (u8 j = 0; j < 3; j++)
        {
            for (u8 k = 0; k <= BD_SIZE; k++)
            {
                if (j == FREESTYLE || (j == RENJU && !i))
                {
                    switch (k)
                    {
                    case 5:
                        get_entry[i][j][k] = get_entry_5f;
                        break;
                    case 6:
                        get_entry[i][j][k] = get_entry_6f;
                        break;
                    case 7:
                        get_entry[i][j][k] = get_entry_7f;
                        break;
                    case 8:
                        get_entry[i][j][k] = get_entry_8f;
                        break;
                    case 9:
                        get_entry[i][j][k] = get_entry_9f;
                        break;
                    case 10:
                        get_entry[i][j][k] = get_entry_10f;
                        break;
                    case 11:
                        get_entry[i][j][k] = get_entry_11f;
                        break;
                    case 12:
                        get_entry[i][j][k] = get_entry_12f;
                        break;
                    case 13:
                        get_entry[i][j][k] = get_entry_13f;
                        break;
                    case 14:
                        get_entry[i][j][k] = get_entry_14f;
                        break;
                    case 15:
                        get_entry[i][j][k] = get_entry_15f;
                        break;
                    default:
                        get_entry[i][j][k] = NULL;
                        break;
                    }
                }
                else if (j == STANDARD || (j == RENJU && i))
                {
                    switch (k)
                    {
                    case 5:
                        get_entry[i][j][k] = get_entry_5;
                        break;
                    case 6:
                        get_entry[i][j][k] = get_entry_6;
                        break;
                    case 7:
                        get_entry[i][j][k] = get_entry_7;
                        break;
                    case 8:
                        get_entry[i][j][k] = get_entry_8;
                        break;
                    case 9:
                        get_entry[i][j][k] = get_entry_9;
                        break;
                    case 10:
                        get_entry[i][j][k] = get_entry_10;
                        break;
                    case 11:
                        get_entry[i][j][k] = get_entry_11;
                        break;
                    case 12:
                        get_entry[i][j][k] = get_entry_12;
                        break;
                    case 13:
                        get_entry[i][j][k] = get_entry_13;
                        break;
                    case 14:
                        get_entry[i][j][k] = get_entry_14;
                        break;
                    case 15:
                        get_entry[i][j][k] = get_entry_15;
                        break;
                    default:
                        get_entry[i][j][k] = NULL;
                        break;
                    }
                }
                else
                    get_entry[i][j][k] = NULL;
            }
        }
    }
}

// Lookup pattab to get pattern.
static void lookup_pattern(Pattern* pp, const Color c, const Color fc, const Rule ru, const u8 len, const u32 ind)
{
    assert(iscolor(c));
    assert(iscolor(fc));
    assert(isrule(ru));
    assert(5 <= len && len <= BD_SIDE);

    u8* tab = get_entry[c == fc][ru][len](ind);
    for (u8 i = PAT_START; i < tab[1]; i++)
        pat_inc(pp, c, tab[i]);
}

// Generate patterns and defending points in pattab.
// MSB on the left and LSB on the right for index.
static void generate_pattern_defending(u8* tab, const Rule ru, const u8 len, const u32 ind)
{
    Score val = 0;
    u32 mask = 0;
    u16 fiv[16], six[15], sev[14], eig[13], nin[12];
    i8 bd5 = len - 4;
    i8 bd6 = len - 5;
    i8 bd7 = len - 6;
    i8 bd8 = len - 7;
    i8 bd9 = len - 8;
    u8 i, j, ind1 = PAT_START, ind2 = AD_START;
    
    // reset to COL_INVALID
    memset(tab, -1, PATTAB_SIZE);

    // calculate indices
    for (i = 0; i < bd5; i++)
        fiv[i] = (ind >> i) & (u32)0x01f;

    for (i = 0; i < bd6; i++)
        six[i] = (ind >> i) & (u32)0x03f;

    for (i = 0; i < bd7; i++)
        sev[i] = (ind >> i) & (u32)0x07f;

    for (i = 0; i < bd8; i++)
        eig[i] = (ind >> i) & (u32)0x0ff;

    for (i = 0; i < bd9; i++)
        nin[i] = (ind >> i) & (u32)0x1ff;

    // c6_6
    if (ru == STANDARD || ru == RENJU)
    {
        for (i = 0; i < bd6; i++)
        {
            if (c6_6[six[i]].is && !((mask >> i) & PatternList[c6_6[six[i]].index][3]))
            {
                tab[ind1++] = c6_6[six[i]].macro;
                mask |= c6_6[six[i]].mask << i;
            }
        }
    }
    // c5_5
    for (i = 0; i < bd5; i++)
    {
        if (c5_5[fiv[i]].is && !((mask >> i) & PatternList[c5_5[fiv[i]].index][3]))
        {
            tab[ind1++] = c5_5[fiv[i]].macro;
            val += PatternWeight[C5][PatternList[c5_5[fiv[i]].index][1]];
            mask |= c5_5[fiv[i]].mask << i;
            goto GPD_END;
        }
    }
    if (ru == STANDARD || ru == RENJU)
    {
        // x_8
        for (i = 0; i < bd8; i++)
        {
            if (x_8[eig[i]].is && !((mask >> i) & PatternList[x_8[eig[i]].index][3]))
                mask |= x_8[eig[i]].mask << i;
        }
        // b3_8
        for (i = 0; i < bd8; i++)
        {
            if (b3_8[eig[i]].is && !((mask >> i) & PatternList[b3_8[eig[i]].index][3]))
            {
                tab[ind1++] = b3_8[eig[i]].macro;
                val += PatternWeight[B3][PatternList[b3_8[eig[i]].index][1]];
                mask |= b3_8[eig[i]].mask << i;
            }
        }
        // b4_7
        for (i = 0; i < bd7; i++)
        {
            if (b4_7[sev[i]].is && !((mask >> i) & PatternList[b4_7[sev[i]].index][3]))
            {
                for (j = 5; j < PatternList[b4_7[sev[i]].index][4]; j++)
                { 
                    tab[ind2++] = PatternList[b4_7[sev[i]].index][j] + i;
                    tab[ind2++] = B4d;
                }
                tab[ind1++] = b4_7[sev[i]].macro;
                val += PatternWeight[B4][PatternList[b4_7[sev[i]].index][1]];
                mask |= b4_7[sev[i]].mask << i;
            }
        }
        // x_6
        for (i = 0; i < bd6; i++)
        {
            if (x_6[six[i]].is && !((mask >> i) & PatternList[x_6[six[i]].index][3]))
                mask |= x_6[six[i]].mask << i;
        }
    }
    // f4_6
    for (i = 0; i < bd6; i++)
    {
        if (f4_6[six[i]].is && !((mask >> i) & PatternList[f4_6[six[i]].index][3]))
        {
            tab[ind1++] = f4_6[six[i]].macro;
            val += PatternWeight[F4][PatternList[f4_6[six[i]].index][1]];
            mask |= f4_6[six[i]].mask << i;
        }
    }
    // b4b4_9
    for (i = 0; i < bd9; i++)
    {
        if (b4b4_9[nin[i]].is && !((mask >> i) & PatternList[b4b4_9[nin[i]].index][3]))
        {
            for (j = 5; j < PatternList[b4b4_9[nin[i]].index][4]; j++)
            {
                tab[ind2++] = PatternList[b4b4_9[nin[i]].index][j] + i;
                tab[ind2++] = B4d;
            }
            tab[ind1++] = b4b4_9[nin[i]].macro;
            tab[ind1++] = b4b4_9[nin[i]].macro;
            val += PatternWeight[B4][PatternList[b4b4_9[nin[i]].index][1]] * 2;
            mask |= b4b4_9[nin[i]].mask << i;
        }
    }
    // b4b4_8
    for (i = 0; i < bd8; i++)
    {
        if (b4b4_8[eig[i]].is && !((mask >> i) & PatternList[b4b4_8[eig[i]].index][3]))
        {
            for (j = 5; j < PatternList[b4b4_8[eig[i]].index][4]; j++)
            {
                tab[ind2++] = PatternList[b4b4_8[eig[i]].index][j] + i;
                tab[ind2++] = B4d;
            }
            tab[ind1++] = b4b4_8[eig[i]].macro;
            tab[ind1++] = b4b4_8[eig[i]].macro;
            val += PatternWeight[B4][PatternList[b4b4_8[eig[i]].index][1]] * 2;
            mask |= b4b4_8[eig[i]].mask << i;
        }
    }
    // b4b4_7
    for (i = 0; i < bd7; i++)
    {
        if (b4b4_7[sev[i]].is && !((mask >> i) & PatternList[b4b4_7[sev[i]].index][3]))
        {
            for (j = 5; j < PatternList[b4b4_7[sev[i]].index][4]; j++)
            {
                tab[ind2++] = PatternList[b4b4_7[sev[i]].index][j] + i;
                tab[ind2++] = B4d;
            }
            tab[ind1++] = b4b4_7[sev[i]].macro;
            tab[ind1++] = b4b4_7[sev[i]].macro;
            val += PatternWeight[B4][PatternList[b4b4_7[sev[i]].index][1]] * 2;
            mask |= b4b4_7[sev[i]].mask << i;
        }
    }
    // b4_5
    for (i = 0; i < bd5; i++)
    {
        if (b4_5[fiv[i]].is && !((mask >> i) & PatternList[b4_5[fiv[i]].index][3]))
        {
            for (j = 5; j < PatternList[b4_5[fiv[i]].index][4]; j++)
            {
                tab[ind2++] = PatternList[b4_5[fiv[i]].index][j] + i;
                tab[ind2++] = B4d;
            }
            tab[ind1++] = b4_5[fiv[i]].macro;
            val += PatternWeight[B4][PatternList[b4_5[fiv[i]].index][1]];
            mask |= b4_5[fiv[i]].mask << i;
        }
    }
    if (ru == STANDARD || ru == RENJU)
    {
        // b3_9
        for (i = 0; i < bd9; i++)
        {
            if (b3_9[nin[i]].is && !((mask >> i) & PatternList[b3_9[nin[i]].index][3]))
            {
                tab[ind1++] = b3_9[nin[i]].macro;
                val += PatternWeight[B3][PatternList[b3_9[nin[i]].index][1]];
                mask |= b3_9[nin[i]].mask << i;
            }
        }
        // f3_8
        for (i = 0; i < bd8; i++)
        {
            if (f3_8[eig[i]].is && !((mask >> i) & PatternList[f3_8[eig[i]].index][3]))
            {
                for (j = 5; j < PatternList[f3_8[eig[i]].index][4]; j++)
                {
                    tab[ind2++] = PatternList[f3_8[eig[i]].index][j] + i;
                    tab[ind2++] = F3d;
                }
                tab[ind1++] = f3_8[eig[i]].macro;
                val += PatternWeight[F3][PatternList[f3_8[eig[i]].index][1]];
                mask |= f3_8[eig[i]].mask << i;
            }
        }
        // b3_7
        for (i = 0; i < bd7; i++)
        {
            if (b3_7[sev[i]].is && !((mask >> i) & PatternList[b3_7[sev[i]].index][3]))
            {
                tab[ind1++] = b3_7[sev[i]].macro;
                val += PatternWeight[B3][PatternList[b3_7[sev[i]].index][1]];
                mask |= b3_7[sev[i]].mask << i;
            }
        }
    }
    // f3_7
    for (i = 0; i < bd7; i++)
    {
        if (f3_7[sev[i]].is && !((mask >> i) & PatternList[f3_7[sev[i]].index][3]))
        {
            for (j = 5; j < PatternList[f3_7[sev[i]].index][4]; j++)
            {
                tab[ind2++] = PatternList[f3_7[sev[i]].index][j] + i;
                tab[ind2++] = F3d;
            }
            tab[ind1++] = f3_7[sev[i]].macro;
            val += PatternWeight[F3][PatternList[f3_7[sev[i]].index][1]];
            mask |= f3_7[sev[i]].mask << i;
        }
    }
    // f3_6
    for (i = 0; i < bd6; i++)
    {
        if (f3_6[six[i]].is && !((mask >> i) & PatternList[f3_6[six[i]].index][3]))
        {
            for (j = 5; j < PatternList[f3_6[six[i]].index][4]; j++)
            {
                tab[ind2++] = PatternList[f3_6[six[i]].index][j] + i;
                tab[ind2++] = F3d;
            }
            tab[ind1++] = f3_6[six[i]].macro;
            val += PatternWeight[F3][PatternList[f3_6[six[i]].index][1]];
            mask |= f3_6[six[i]].mask << i;
        }
    }
    // b3_5
    for (i = 0; i < bd5; i++)
    {
        if (b3_5[fiv[i]].is && !((mask >> i) & PatternList[b3_5[fiv[i]].index][3]))
        {
            tab[ind1++] = b3_5[fiv[i]].macro;
            val += PatternWeight[B3][PatternList[b3_5[fiv[i]].index][1]];
            mask |= b3_5[fiv[i]].mask << i;
        }
    }
    // f2_8
    for (i = 0; i < bd8; i++)
    {
        if (f2_8[eig[i]].is && !((mask >> i) & PatternList[f2_8[eig[i]].index][3]))
        {
            tab[ind1++] = f2_8[eig[i]].macro;
            val += PatternWeight[F2][PatternList[f2_8[eig[i]].index][1]];
            mask |= f2_8[eig[i]].mask << i;
        }
    }
    // f2_7
    for (i = 0; i < bd7; i++)
    {
        if (f2_7[sev[i]].is && !((mask >> i) & PatternList[f2_7[sev[i]].index][3]))
        {
            tab[ind1++] = f2_7[sev[i]].macro;
            val += PatternWeight[F2][PatternList[f2_7[sev[i]].index][1]];
            mask |= f2_7[sev[i]].mask << i;
        }
    }
    // f2_6
    for (i = 0; i < bd6; i++)
    {
        if (f2_6[six[i]].is && !((mask >> i) & PatternList[f2_6[six[i]].index][3]))
        {
            tab[ind1++] = f2_6[six[i]].macro;
            val += PatternWeight[F2][PatternList[f2_6[six[i]].index][1]];
            mask |= f2_6[six[i]].mask << i;
        }
    }
    // b2_5
    for (i = 0; i < bd5; i++)
    {
        if (b2_5[fiv[i]].is && !((mask >> i) & PatternList[b2_5[fiv[i]].index][3]))
        {
            val += PatternWeight[B2][PatternList[b2_5[fiv[i]].index][1]];
            mask |= b2_5[fiv[i]].mask << i;
        }
    }
    // f1_9
    for (i = 0; i < bd9; i++)
    {
        if (f1_9[nin[i]].is && !((mask >> i) & PatternList[f1_9[nin[i]].index][3]))
        {
            val += PatternWeight[F1][PatternList[f1_9[nin[i]].index][1]];
            mask |= f1_9[nin[i]].mask << i;
        }
    }
    // f1_8
    for (i = 0; i < bd8; i++)
    {
        if (f1_8[eig[i]].is && !((mask >> i) & PatternList[f1_8[eig[i]].index][3]))
        {
            val += PatternWeight[F1][PatternList[f1_8[eig[i]].index][1]];
            mask |= f1_8[eig[i]].mask << i;
        }
    }
    // f1_7
    for (i = 0; i < bd7; i++)
    {
        if (f1_7[sev[i]].is && !((mask >> i) & PatternList[f1_7[sev[i]].index][3]))
        {
            val += PatternWeight[F1][PatternList[f1_7[sev[i]].index][1]];
            mask |= f1_7[sev[i]].mask << i;
        }
    }
    // f1_6
    for (i = 0; i < bd6; i++)
    {
        if (f1_6[six[i]].is && !((mask >> i) & PatternList[f1_6[six[i]].index][3]))
        {
            val += PatternWeight[F1][PatternList[f1_6[six[i]].index][1]];
            mask |= f1_6[six[i]].mask << i;
        }
    }

GPD_END:
    tab[0] = MyMin(val, 255);
    tab[1] = ind1;
    tab[2] = ind2;
}

// Generate attacking points in pattab. Call this function after generate_pattern_defending.
// MSB on the left and LSB on the right for index.
static void generate_attacking(u8* tab, const Rule ru, const u8 len, const u32 ind)
{
    Pattern pold, pnew;
    u8 i, j, ind1 = tab[2];

    pat_reset(&pold);
    lookup_pattern(&pold, 1, 1, ru, len, ind);

    if (pat_read(&pold, 1, C5) > 0)
        goto GA_END;

    for (i = 0; i < len; i++)
    {
        if (!((ind >> i) & 0x1))
        {
            pat_reset(&pnew);
            lookup_pattern(&pnew, 1, 1, ru, len, ind | (0x1 << i));
            pat_sub(&pnew, &pnew, &pold);

            if (pat_read(&pnew, 1, B4) > 0 && pat_read(&pnew, 1, C6) == 0 && pat_read(&pnew, 1, F4) == 0 && pat_read(&pnew, 1, F3) >= 0)
            {
                for (j = 0; j < pat_read(&pnew, 1, B4); j++)
                {
                    tab[ind1] = i;
                    tab[ind1 + 1] = B4a;
                    if (ind1 + 2 < PATTAB_SIZE)
                        ind1 += 2;
                    else
                        break;
                }
            }
        }
    }

GA_END:
    tab[2] = ind1;
}

// Return the symmetry number. If num = 15 and N = 16, return 0xf000.
static u32 symmetry(const u32 num, const u8 N)
{
    u32 res = 0;
    for (u8 i = 0; i < N; i++)
        res |= ((num >> (N - 1 - i)) & 0x1) << i;
    return res;
}

// Merge symmetric entries.
static void merge_table(u8(*tab)[PATTAB_SIZE], const u8 len, const u32 size)
{
    u32 tmp, ind;
    u8 i, j;

    for (ind = 0; ind < size; ind++)
    {
        tmp = symmetry(ind, len);

        if (tab[ind][0] > tab[tmp][0])
        {
            j = AD_START;
            memcpy(tab[tmp], tab[ind], AD_SIZE);
            for (i = tab[ind][2] - 2; i >= AD_START; i -= 2)
            {
                tab[tmp][j++] = len - 1 - tab[ind][i];
                tab[tmp][j++] = tab[ind][i + 1];
            }
        }

        else if (tab[ind][0] < tab[tmp][0])
        {
            j = AD_START;
            memcpy(tab[ind], tab[tmp], AD_SIZE);
            for (i = tab[tmp][2] - 2; i >= AD_START; i -= 2)
            {
                tab[ind][j++] = len - 1 - tab[tmp][i];
                tab[ind][j++] = tab[tmp][i + 1];
            }
        }
    }
}

static void generate_pattab1()
{
    for (u32 i = 0; i < IIE15; i++)
    {
        if (i < IIE15)
        {
            generate_pattern_defending(Pat15f[i], FREESTYLE, 15, i);
            generate_pattern_defending(Pat15[i], RENJU, 15, i);
        }
        if (i < IIE14)
        {
            generate_pattern_defending(Pat14f[i], FREESTYLE, 14, i);
            generate_pattern_defending(Pat14[i], RENJU, 14, i);
        }
        if (i < IIE13)
        {
            generate_pattern_defending(Pat13f[i], FREESTYLE, 13, i);
            generate_pattern_defending(Pat13[i], RENJU, 13, i);
        }
        if (i < IIE12)
        {
            generate_pattern_defending(Pat12f[i], FREESTYLE, 12, i);
            generate_pattern_defending(Pat12[i], RENJU, 12, i);
        }
        if (i < IIE11)
        {
            generate_pattern_defending(Pat11f[i], FREESTYLE, 11, i);
            generate_pattern_defending(Pat11[i], RENJU, 11, i);
        }
        if (i < IIE10)
        {
            generate_pattern_defending(Pat10f[i], FREESTYLE, 10, i);
            generate_pattern_defending(Pat10[i], RENJU, 10, i);
        }
        if (i < IIE9)
        {
            generate_pattern_defending(Pat9f[i], FREESTYLE, 9, i);
            generate_pattern_defending(Pat9[i], RENJU, 9, i);
        }
        if (i < IIE8)
        {
            generate_pattern_defending(Pat8f[i], FREESTYLE, 8, i);
            generate_pattern_defending(Pat8[i], RENJU, 8, i);
        }
        if (i < IIE7)
        {
            generate_pattern_defending(Pat7f[i], FREESTYLE, 7, i);
            generate_pattern_defending(Pat7[i], RENJU, 7, i);
        }
        if (i < IIE6)
        {
            generate_pattern_defending(Pat6f[i], FREESTYLE, 6, i);
            generate_pattern_defending(Pat6[i], RENJU, 6, i);
        }
        if (i < IIE5)
        {
            generate_pattern_defending(Pat5f[i], FREESTYLE, 5, i);
            generate_pattern_defending(Pat5[i], RENJU, 5, i);
        }
    }
}

static void generate_pattab2()
{
    for (u32 i = 0; i < IIE15; i++)
    {
        if (i < IIE15)
        {
            generate_attacking(Pat15f[i], FREESTYLE, 15, i);
            generate_attacking(Pat15[i], RENJU, 15, i);
        }
        if (i < IIE14)
        {
            generate_attacking(Pat14f[i], FREESTYLE, 14, i);
            generate_attacking(Pat14[i], RENJU, 14, i);
        }
        if (i < IIE13)
        {
            generate_attacking(Pat13f[i], FREESTYLE, 13, i);
            generate_attacking(Pat13[i], RENJU, 13, i);
        }
        if (i < IIE12)
        {
            generate_attacking(Pat12f[i], FREESTYLE, 12, i);
            generate_attacking(Pat12[i], RENJU, 12, i);
        }
        if (i < IIE11)
        {
            generate_attacking(Pat11f[i], FREESTYLE, 11, i);
            generate_attacking(Pat11[i], RENJU, 11, i);
        }
        if (i < IIE10)
        {
            generate_attacking(Pat10f[i], FREESTYLE, 10, i);
            generate_attacking(Pat10[i], RENJU, 10, i);
        }
        if (i < IIE9)
        {
            generate_attacking(Pat9f[i], FREESTYLE, 9, i);
            generate_attacking(Pat9[i], RENJU, 9, i);
        }
        if (i < IIE8)
        {
            generate_attacking(Pat8f[i], FREESTYLE, 8, i);
            generate_attacking(Pat8[i], RENJU, 8, i);
        }
        if (i < IIE7)
        {
            generate_attacking(Pat7f[i], FREESTYLE, 7, i);
            generate_attacking(Pat7[i], RENJU, 7, i);
        }
        if (i < IIE6)
        {
            generate_attacking(Pat6f[i], FREESTYLE, 6, i);
            generate_attacking(Pat6[i], RENJU, 6, i);
        }
        if (i < IIE5)
        {
            generate_attacking(Pat5f[i], FREESTYLE, 5, i);
            generate_attacking(Pat5[i], RENJU, 5, i);
        }
    }
}

static void merge_table_all()
{
    merge_table(Pat15f, 15, IIE15);
    merge_table(Pat14f, 14, IIE14);
    merge_table(Pat13f, 13, IIE13);
    merge_table(Pat12f, 12, IIE12);
    merge_table(Pat11f, 11, IIE11);
    merge_table(Pat10f, 10, IIE10);
    merge_table(Pat9f, 9, IIE9);
    merge_table(Pat8f, 8, IIE8);
    merge_table(Pat7f, 7, IIE7);
    merge_table(Pat6f, 6, IIE6);
    merge_table(Pat5f, 5, IIE5);

    merge_table(Pat15, 15, IIE15);
    merge_table(Pat14, 14, IIE14);
    merge_table(Pat13, 13, IIE13);
    merge_table(Pat12, 12, IIE12);
    merge_table(Pat11, 11, IIE11);
    merge_table(Pat10, 10, IIE10);
    merge_table(Pat9, 9, IIE9);
    merge_table(Pat8, 8, IIE8);
    merge_table(Pat7, 7, IIE7);
    merge_table(Pat6, 6, IIE6);
    merge_table(Pat5, 5, IIE5);
}

// Generate all pattern tables.
void generate_pattab()
{
#if TABLE_GENERATE_INFO
    Time t1 = get_ms();
#endif

    phelper_init();
    generate_get_entry();
    generate_pattab1();
    merge_table_all();
    generate_pattab2();
    merge_table_all();

#if TABLE_GENERATE_INFO
    Time t2 = get_ms();
    printf("MESSAGE initializing...%dms\n", t2 - t1);
#endif
}

// Randomize pattern weight.
void randomize()
{
    for (u8 i = 0; i < 9; i++)
        for (u8 j = 0; j < 4; j++)
            if (0 < PatternWeight[i][j] && PatternWeight[i][j] < 100)
                PatternWeight[i][j] = (Score)round((double)PatternWeight[i][j] * ((double)rand() / RAND_MAX * RANDOM_PCT * 2.0 - RANDOM_PCT + 1.0));
}
