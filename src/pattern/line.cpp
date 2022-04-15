/*      _____                __    ______
 *     / ___ \              / /   /___  /
 *    / /__/ /___  ____  __/ /_______/ /    ____  ____
 *   / _____/ __ \/ __ \/_   _/ __  / /    / __ \/ __ \
 *  / /    /  ___/ / / / / /_/ /_/ / /____/  ___/ / / /
 * /_/     \____/_/ /_/ /___/\__,_/______/\____/_/ /_/
 *
 * PentaZen pattern generator, by Sun Yuliang.
 */

#include "line.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory.h>

namespace {

// Return the occurrence times of s in str
int num_of(const std::string &str, const std::string &s) {
    std::string::size_type pos = 0;
    int                    cnt = 0;

    while ((pos = str.find(s, pos)) != std::string::npos) {
        ++pos;
        ++cnt;
    }
    return cnt;
}

// Return the number of '1' in str
int pop_cnt(const std::string &str) {
    int ret = 0;

    for (auto &i : str) {
        if (i == '1')
            ++ret;
    }
    return ret;
}

} // namespace

// Set the line with index and length
void Line::set(uint32_t ind, int len) {
    lstr.clear();

    for (auto i = len - 1; i >= 0; --i)
        ind &(1u << i) ? lstr.append("1") : lstr.append("0");
}

// Return the material table index
int Line::index() const {
    int ret = 0, sz = size();

    for (auto i = 0; i != sz; ++i) {
        ret <<= 1;
        ret += lstr[i] - '0';
    }
    return ret + (1 << sz) - 1;
}

// Return index of the leftmost '1'
int Line::start() const {
    int sz = size();

    for (auto i = 0; i != sz; ++i) {
        if (lstr[i] == '1')
            return i;
    }
    return size();
}

// Return index of the rightmost '1'
int Line::end() const {
    int sz = size();

    for (auto i = sz - 1; i >= 0; --i) {
        if (lstr[i] == '1')
            return i;
    }
    return -1;
}

// Return the available pieces for material judgement. This can be too strict.
int Line::piece_avail() const {
    int ret = pop_cnt(lstr);

    for (auto i = C6; i != MATERIAL_NUM; ++i)
        ret -= MatTable[index()][i] * piece_req_of(i);

    return std::max(ret, 0);
}

template <>
bool Line::judge_C6<FREESTYLE>() const {
    return 0;
}

template <>
bool Line::judge_C5<FREESTYLE>() const {
    return num_of(lstr, "11111");
}

template <>
int Line::judge_F4<FREESTYLE>() const {
    return num_of(lstr, "011110");
}

template <>
int Line::judge_F3<FREESTYLE>() const {
    std::string::size_type tmp, pos = 0;
    int                    ret = 0;

    while (static_cast<int>(pos) < size()) {
        tmp = std::min({lstr.find("001110", pos), lstr.find("010110", pos), lstr.find("011010", pos), lstr.find("011100", pos)});

        if (tmp == std::string::npos)
            break;

        pos = tmp + 4;
        ++ret;
    }
    return ret;
}

template <>
bool Line::judge_C6<STANDARD>() const {
    return num_of(lstr, "111111");
}

template <>
bool Line::judge_C5<STANDARD>() const {
    return size() < 6 ? num_of(lstr, "11111") : num_of(lstr.substr(0, 6), "111110") + num_of(lstr, "0111110") + num_of(lstr.substr(size() - 6, 6), "011111");
}

template <>
int Line::judge_F4<STANDARD>() const {
    return size() < 7 ? num_of(lstr, "011110") : num_of(lstr.substr(0, 7), "0111100") + num_of(lstr, "00111100") + num_of(lstr.substr(size() - 7, 7), "0011110");
}

template <>
int Line::judge_F3<STANDARD>() const {
    if (size() <= 5)
        return 0; // this is necessary

    if (size() == 6)
        if (lstr == "001110" || lstr == "010110" || lstr == "011010" || lstr == "011100")
            return 1;

    std::string::size_type tmp, pos = 0;
    int                    ret = 0;

    std::string str = "0" + lstr + "0";

    while (static_cast<int>(pos) < size()) {
        tmp = std::min({str.find("00011100", pos), str.find("00101100", pos), str.find("00110100", pos), str.find("00111000", pos)});

        if (tmp == std::string::npos)
            break;

        pos = tmp + 4;
        ++ret;
    }
    return ret;
}

// Return true if material m increases if adding a piece, i.e. changing '0' to '1',
// at pos. Return false if no increase or pos is not empty, i.e. '0'.
bool Line::judge_promotion(Material m, int pos) const {
    if (!is_empty(pos))
        return false;

    Line tmp(lstr);
    bool ret = false;
    tmp.fill(pos);

    if (tmp.lookup(m) > lookup(m)) {
        ret = true;

        // Check fake promotion. True promotion does not decrease the number of
        // materials of higher or same priority to demote(m).
        for (Material i = C6; !prior_to(demote(m), i); ++i) {
            if (i == m || i == demote(m))
                continue;

            if (tmp.lookup(i) < lookup(i)) {
                ret = false;
                break;
            }
        }
    }

    return ret;
}

// Return the number of material m in the line
template <Rule r>
int Line::judge(Material m) const {
    int ret = 0, pa = piece_avail();

    // Return if there are not enough pieces for material m
    if (pa < piece_req_of(m))
        return ret;

    if (m == C6)
        ret = judge_C6<r>();

    else if (m == C5)
        ret = judge_C5<r>();

    else if (m == F4)
        ret = judge_F4<r>();

    else if (m == F3)
        ret = judge_F3<r>();

    else {
        // Boundary for different material
        int b = m == B4 ? 1 : m == B3 ? 2 :
                          m == F2     ? 2 :
                          m == B2     ? 3 :
                          m == F1     ? 3 :
                                        4;

        // Judge by counting promotion positions
        for (auto i = std::max(start() - b, 0); i <= std::min(end() + b, size() - 1); ++i)
            if (judge_promotion(promote(m), i))
                ++ret;
    }

    // Transform ret to actual material number
    switch (m) {
    case B4:
    case B3:
    case B2:
    case B1:
        ret = ret < 5 - piece_req_of(m) ? 0 : ceil(ret / (5.0 - piece_req_of(m)));
        break;
    case F2:
        ret = ret < 2 ? 0 : ceil(ret / 4.0);
        break;
    case F1:
        ret = ret < 3 ? 0 : ceil(ret / 6.0);
        break;
    default:
        break;
    }

    // Restrict the return value according to avaliable pieces
    return std::min((int)(ceil((double)pa / piece_req_of(m))), ret);
}

namespace {

// Generate part of the table, of the same line length
template <Rule r>
void generate(int len) {
    Line line;

    for (auto m = C6; m != MATERIAL_NUM; ++m)
        for (auto i = 0; i != (1 << len); ++i) {
            line.set(i, len);
            MatTable[line.index()][m] = line.judge<r>(m);
        }
}

} // namespace

// Generate the whole table
void generate(Rule r) {
    memset(MatTable, 0, sizeof(MatTable));

    if (r == FREESTYLE)
        for (auto i = MIN_LINE_LEN; i <= MAX_LINE_LEN; ++i)
            generate<FREESTYLE>(i);

    else if (r == STANDARD)
        for (auto i = MIN_LINE_LEN; i <= MAX_LINE_LEN; ++i)
            generate<STANDARD>(i);
}

// Return merged material info of the line
uint32_t merged_material_info(const Line &line) {
    int ret = 0, cnt = 0, tmp;

    // Skip lower level materials if C6, C5 or F4 exist
    for (auto m = C6; m <= F4; ++m) {
        if (lookup(line, m) > 0) {
            ret |= m << (cnt++ * 4);
            goto skip;
        }
    }

    // Merge the remaining materials
    for (auto m = B4; m <= B1; ++m) {
        tmp = lookup(line, m);
        while (tmp--)
            ret |= m << (cnt++ * 4);
    }

skip:
    // This message should not be printed.
    if (cnt > 8)
        std::cerr << "material capacity is too small\n"
                  << line << std::endl;

    // Fill with material none
    while (cnt < 8)
        ret |= MATERIAL_NONE << (cnt++ * 4);

    return ret;
}

// Return merged position information for pos in line
uint32_t merged_position_info(const Line &line, const int pos) {
    if (line.size() < 5)
        return 0;

    if (!line.is_empty(pos))
        return 0;

    Line     tmp = line;
    Line     left(line.lstr.substr(0, pos));
    Line     right(line.lstr.substr(pos + 1, line.size() - 1 - pos));
    uint32_t ret = 0;

    tmp.fill(pos);

    // Promotion caused by filling my piece
    if (lookup(tmp, C6) > lookup(line, C6))
        ret |= 1u << 0;
    if (lookup(tmp, C5) > lookup(line, C5))
        ret |= 1u << 1;
    if (lookup(tmp, F4) > lookup(line, F4))
        ret |= 1u << 2;
    if (lookup(tmp, B4) > lookup(line, B4))
        ret |= 1u << 3;
    if (lookup(tmp, F3) > lookup(line, F3))
        ret |= 1u << 4;
    if (lookup(tmp, B3) > lookup(line, B3))
        ret |= 1u << 5;
    if (lookup(tmp, F2) > lookup(line, F2))
        ret |= 1u << 6;
    if (lookup(tmp, B2) > lookup(line, B2))
        ret |= 1u << 7;
    if (lookup(tmp, F1) > lookup(line, F1))
        ret |= 1u << 8;
    if (lookup(tmp, B1) > lookup(line, B1))
        ret |= 1u << 9;

    // Demotion caused by filling my piece
    if (lookup(tmp, B4) < lookup(line, B4))
        ret |= 1u << 10;
    if (lookup(tmp, F3) < lookup(line, F3))
        ret |= 1u << 11;
    if (lookup(tmp, B3) < lookup(line, B3))
        ret |= 1u << 12;
    if (lookup(tmp, F2) < lookup(line, F2))
        ret |= 1u << 13;
    if (lookup(tmp, B2) < lookup(line, B2))
        ret |= 1u << 14;
    if (lookup(tmp, F1) < lookup(line, F1))
        ret |= 1u << 15;
    if (lookup(tmp, B1) < lookup(line, B1))
        ret |= 1u << 16;

    // Promotion caused by filling opposite piece
    if (lookup(left, B4) + lookup(right, B4) > lookup(line, B4))
        ret |= 1u << 17;
    if (lookup(left, F3) + lookup(right, F3) > lookup(line, F3))
        ret |= 1u << 18;
    if (lookup(left, B3) + lookup(right, B3) > lookup(line, B3))
        ret |= 1u << 19;
    if (lookup(left, F2) + lookup(right, F2) > lookup(line, F2))
        ret |= 1u << 20;
    if (lookup(left, B2) + lookup(right, B2) > lookup(line, B2))
        ret |= 1u << 21;
    if (lookup(left, F1) + lookup(right, F1) > lookup(line, F1))
        ret |= 1u << 22;
    if (lookup(left, B1) + lookup(right, B1) > lookup(line, B1))
        ret |= 1u << 23;

    // Demotion caused by filling opposite piece
    if (lookup(left, B4) + lookup(right, B4) < lookup(line, B4))
        ret |= 1u << 24;
    if (lookup(left, F3) + lookup(right, F3) < lookup(line, F3))
        ret |= 1u << 25;
    if (lookup(left, B3) + lookup(right, B3) < lookup(line, B3))
        ret |= 1u << 26;
    if (lookup(left, F2) + lookup(right, F2) < lookup(line, F2))
        ret |= 1u << 27;
    if (lookup(left, B2) + lookup(right, B2) < lookup(line, B2))
        ret |= 1u << 28;
    if (lookup(left, F1) + lookup(right, F1) < lookup(line, F1))
        ret |= 1u << 29;
    if (lookup(left, B1) + lookup(right, B1) < lookup(line, B1))
        ret |= 1u << 30;

    // XXX_X X_XXX for VCF
    if (ret & 8 && line.size() >= 7)
        if ((2 <= pos && pos <= line.size() - 4 && line.lstr.substr(pos - 2, 6) == "000110") || (3 <= pos && pos <= line.size() - 3 && line.lstr.substr(pos - 3, 6) == "001010") || (4 <= pos && pos <= line.size() - 2 && line.lstr.substr(pos - 4, 6) == "001100") || (1 <= pos && pos <= line.size() - 5 && line.lstr.substr(pos - 1, 6) == "001100") || (2 <= pos && pos <= line.size() - 4 && line.lstr.substr(pos - 2, 6) == "010100") || (3 <= pos && pos <= line.size() - 3 && line.lstr.substr(pos - 3, 6) == "011000"))
            ret |= 1u << 31;

    return ret;
}
