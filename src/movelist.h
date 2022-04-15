/*      _____                __    ______
 *     / ___ \              / /   /___  /
 *    / /__/ /___  ____  __/ /_______/ /    ____  ____
 *   / _____/ __ \/ __ \/_   _/ __  / /    / __ \/ __ \
 *  / /    /  ___/ / / / / /_/ /_/ / /____/  ___/ / / /
 * /_/     \____/_/ /_/ /___/\__,_/______/\____/_/ /_/
 *
 * PentaZen, a Gomoku/Renju playing engine developed by Sun Yuliang.
 */

#pragma once

#include "type.h"

#include <cstring>
#include <type_traits>

constexpr int BITBOARD_SIZE = MOVE_CAPACITY / 64 + 1;

// ExtMove struct is a move-value pair aggregate class
struct ExtMove {
    Move  move;
    Score score;
};

constexpr inline bool operator<(const ExtMove &lhs, const ExtMove &rhs) {
    return lhs.score < rhs.score;
}

inline std::ostream &operator<<(std::ostream &os, const ExtMove &em) {
    std::cout << "[" << em.move << " " << em.score << "]";
    return os;
}

// Bitboard struct stores moves using several 64-bit unsigned integers. All
// methods are of constant time complexity. Random access is not available.
struct BitBoard {
public:
    BitBoard() {
        reset();
    }
    void reset() {
        memset(bitboard, 0, sizeof(bitboard));
    }
    bool contains(Move m) const {
        return bitboard[m >> 6] & (uint64_t(1) << (m & 63u));
    }
    void insert(Move m) {
        bitboard[m >> 6] |= (uint64_t(1) << (m & 63u));
    }
    void remove(Move m) {
        bitboard[m >> 6] &= ~(uint64_t(1) << (m & 63u));
    }

private:
    uint64_t bitboard[BITBOARD_SIZE];
};

// MoveList class is a templated class storing Moves or ExtMoves.
template <typename T>
class MoveList {
    static_assert(std::is_same<T, Move>::value || std::is_same<T, ExtMove>::value, "T should be Move or ExtMove");

public:
    typedef T *         iterator;
    typedef const T *   const_iterator;
    typedef std::size_t size_type;

    MoveList() {
        reset();
    }
    MoveList(const MoveList<T> &ml);
    MoveList<T> &operator=(const MoveList<T> &ml);

    iterator begin() {
        return movelist;
    }
    iterator end() {
        return offTheEnd;
    }
    const_iterator begin() const {
        return movelist;
    }
    const_iterator end() const {
        return offTheEnd;
    }
    size_type size() const {
        return offTheEnd - movelist;
    }

    void reset();
    bool contains(Move m) const;
    void insert(Move m, Score s = SCORE_NONE);
    void remove(Move m);
    void swap(iterator it1, iterator it2);

private:
    T        movelist[MOVE_SIZE + 1], *offTheEnd;
    BitBoard bitboard;
};

template <typename T>
inline MoveList<T>::MoveList(const MoveList<T> &ml) {
    memcpy(movelist, ml.movelist, sizeof(movelist));
    offTheEnd = movelist + ml.size();
    bitboard  = ml.bitboard;
}

template <typename T>
inline MoveList<T> &MoveList<T>::operator=(const MoveList<T> &ml) {
    memcpy(movelist, ml.movelist, sizeof(movelist));
    offTheEnd = movelist + ml.size();
    bitboard  = ml.bitboard;
    return *this;
}

template <typename T>
inline void MoveList<T>::reset() {
    offTheEnd = movelist;
    bitboard.reset();
}

template <typename T>
inline bool MoveList<T>::contains(Move m) const {
    assert(is_ok(m));

    return bitboard.contains(m);
}

template <>
inline void MoveList<Move>::insert(Move m, Score s) {
    assert(is_ok(m));
    (void)s; // Silence warning

    if (!contains(m)) {
        *offTheEnd++ = m;
        bitboard.insert(m);
    }
}

template <>
inline void MoveList<ExtMove>::insert(Move m, Score s) {
    assert(is_ok(m));

    if (!contains(m)) {
        *offTheEnd++ = {m, s};
        bitboard.insert(m);
    }
}

template <>
inline void MoveList<Move>::remove(Move m) {
    assert(is_ok(m));

    if (contains(m)) {
        Move *pos = std::find_if(begin(), end(), [&](const Move &e) { return e == m; });
        *pos      = *--offTheEnd;
        bitboard.remove(m);
    }
}

template <>
inline void MoveList<ExtMove>::remove(Move m) {
    assert(is_ok(m));

    if (contains(m)) {
        ExtMove *pos = std::find_if(begin(), end(), [&](const ExtMove &e) { return e.move == m; });
        *pos         = *--offTheEnd;
        bitboard.remove(m);
    }
}

template <typename T>
inline void MoveList<T>::swap(iterator it1, iterator it2) {
    assert(begin() <= it1 && it1 < end());
    assert(begin() <= it2 && it2 < end());

    T tmp = *it1;
    *it1  = *it2;
    *it2  = tmp;
}

template <typename T>
inline std::ostream &operator<<(std::ostream &os, const MoveList<T> &ml) {
    for (auto &i : ml)
        std::cout << i << " ";
    return os;
}
