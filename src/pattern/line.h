/*      _____                __    ______
 *     / ___ \              / /   /___  /
 *    / /__/ /___  ____  __/ /_______/ /    ____  ____
 *   / _____/ __ \/ __ \/_   _/ __  / /    / __ \/ __ \
 *  / /    /  ___/ / / / / /_/ /_/ / /____/  ___/ / / /
 * /_/     \____/_/ /_/ /___/\__,_/______/\____/_/ /_/
 *
 * PentaZen pattern generator, by Sun Yuliang.
 */

#pragma once

#include "type.h"

#include <string>

extern int MatTable[MAT_TABLE_SIZE][MATERIAL_NUM];

// Line class contains line info and functions to judge materials
class Line {
    friend std::ostream &operator<<(std::ostream &os, const Line &l);

public:
    std::string lstr;

    Line() = default;
    Line(const std::string &str);

    void set(const std::string &str);
    void set(uint32_t ind, int len);

    int  size() const;
    bool is_empty(int pos) const;
    void fill(int pos);
    void take(int pos);
    int  index() const;
    template <Rule r>
    int  judge(Material m) const;
    bool judge_promotion(Material m, int pos) const;

private:
    int start() const;
    int end() const;
    int lookup(Material m) const;
    int piece_avail() const;
    template <Rule r>
    bool judge_C6() const;
    template <Rule r>
    bool judge_C5() const;
    template <Rule r>
    int judge_F4() const;
    template <Rule r>
    int judge_F3() const;
};

std::ostream &operator<<(std::ostream &os, const Line &l);

inline Line::Line(const std::string &str) {
    lstr = str;
}

inline void Line::set(const std::string &str) {
    lstr = str;
}

inline int Line::size() const {
    return lstr.size();
}

inline bool Line::is_empty(int pos) const {
    return lstr[pos] == '0';
}

inline void Line::fill(int pos) {
    lstr[pos] = '1';
}

inline void Line::take(int pos) {
    lstr[pos] = '0';
}

inline int Line::lookup(Material m) const {
    return MatTable[index()][m];
}

inline int lookup(const Line &line, Material m) {
    return MatTable[line.index()][m];
}

inline std::ostream &operator<<(std::ostream &os, const Line &l) {
    os << l.lstr;
    return os;
}

void generate(Rule r);

uint32_t merged_material_info(const Line &line);

uint32_t merged_position_info(const Line &line, const int pos);
