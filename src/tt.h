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

#pragma pack(push, 2)

// TTEntry struct is the 10 bytes transposition table entry, defined as below:
// key        32 bit
// move       16 bit
// score      16 bit
// generation  5 bit
// bound type  2 bit
// pv node     1 bit
// depth       8 bit
struct TTEntry {
    Move move() const {
        return move16;
    }
    Score score() const {
        return score16;
    }
    bool is_pv() const {
        return (bool)(genBound8 & 0x4);
    }
    Bound bound() const {
        return Bound(genBound8 & 0x3);
    }
    Depth depth() const {
        return depth8;
    }
    void save(ZobristKey k, Move m, Score sc, Bound b, bool pv, Depth d);

private:
    friend class TranspositionTable;

    uint32_t key32;
    Move     move16;
    Score    score16;
    Bound    genBound8;
    Depth    depth8;
};

#pragma pack(pop)

static_assert(sizeof(TTEntry) == 10, "TT entry size should be 10 bytes");

// A TranspositionTable is an array of Cluster, of size clusterCount. Each
// cluster consists of ClusterSize number of TTEntry. Each non-empty TTEntry
// contains information on exactly one position. The size of a Cluster should
// divide the size of a cache line for best performance, as the cacheline is
// prefetched when possible.
class TranspositionTable {
    static constexpr int ClusterSize = 3;

    struct Cluster {
        TTEntry entry[ClusterSize];
        char    padding[2]; // Pad to 32 bytes
    };

    static_assert(sizeof(Cluster) == 32, "Unexpected Cluster size");

    // Constants used to refresh the hash table periodically
    static constexpr unsigned GENERATION_BITS  = 3;                                // nb of bits reserved for other things
    static constexpr int      GENERATION_DELTA = (1 << GENERATION_BITS);           // increment for generation field
    static constexpr int      GENERATION_CYCLE = 255 + (1 << GENERATION_BITS);     // cycle length
    static constexpr int      GENERATION_MASK  = (0xFF << GENERATION_BITS) & 0xFF; // mask to pull out generation number

public:
    ~TranspositionTable() {
        aligned_large_pages_free(table);
    }
    void new_search() {
        generation8 += GENERATION_DELTA;
    } // Lower bits are used for other things
    TTEntry* probe(const ZobristKey key, bool& found) const;
    void     resize(size_t mbSize);
    void     clear();

    TTEntry* first_entry(const ZobristKey key) const {
        return &table[mul_hi64(key, clusterCount)].entry[0];
    }

private:
    friend struct TTEntry;

    size_t   clusterCount;
    Cluster* table;
    uint8_t  generation8; // Size must be not bigger than TTEntry::genBound8
};

extern TranspositionTable TT;
