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

enum Rule {
    FREESTYLE,
    STANDARD,
    RULE_NUM = 2
};

// Change for different rules and board sizes
constexpr Rule TARGET_RULE    = FREESTYLE;
constexpr int  MIN_LINE_LEN   = 5;
constexpr int  MAX_LINE_LEN   = 15;
constexpr int  MAT_TABLE_SIZE = 1 << (MAX_LINE_LEN + 1);

enum Material {
    C6,
    C5,
    F4,
    B4,
    F3,
    B3,
    F2,
    B2,
    F1,
    B1,
    MATERIAL_NUM  = 10,
    MATERIAL_NONE = 15, // set 0xff
};

constexpr Material operator+(Material m, int i) {
    return Material(static_cast<int>(m) + i);
}

constexpr Material operator-(Material m, int i) {
    return Material(static_cast<int>(m) - i);
}

inline Material &operator++(Material &m) {
    return m = m + 1;
}

// Return the promoted material
// B4 -> C5
// F3 -> F4
// ...
// B1 -> B2
constexpr Material promote(Material m) {
    return m >= B4 ? m - 2 : m;
}

// Return the demoted material
// C5 -> B4
// F4 -> F3
// ...
// B2 -> B1
constexpr Material demote(Material m) {
    return C5 <= m && m <= B2 ? m + 2 : m;
}

// Return #pieces to form the material
// C6 -> 6
// C5 -> 5
// F4 -> 4
// ...
constexpr int piece_req_of(Material m) {
    return m == C6 ? 6 : 5 - m / 2;
}

// Return true if m1 is prior to m2
inline bool prior_to(Material m1, Material m2) {
    return m2 - m1 >= 2 ? true : m2 - m1 <= 0 ? false :
                             m1 & 1           ? false :
                                                true;
}
