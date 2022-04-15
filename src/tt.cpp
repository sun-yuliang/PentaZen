/*      _____                __    ______
 *     / ___ \              / /   /___  /
 *    / /__/ /___  ____  __/ /_______/ /    ____  ____
 *   / _____/ __ \/ __ \/_   _/ __  / /    / __ \/ __ \
 *  / /    /  ___/ / / / / /_/ /_/ / /____/  ___/ / / /
 * /_/     \____/_/ /_/ /___/\__,_/______/\____/_/ /_/
 *
 * PentaZen, a Gomoku/Renju playing engine developed by Sun Yuliang.
 */

#include "tt.h"

#include "thread.h"

TranspositionTable TT; // Our global transposition table

// TTEntry::save() populates the TTEntry with a new node's data, possibly
// overwriting an old position. Update is not atomic and can be racy.
void TTEntry::save(ZobristKey k, Move m, Score s, Bound b, bool pv, Depth d) {
    // Preserve any existing move for the same position
    if (m || (uint32_t)k != key32)
        move16 = m;

    // Overwrite less valuable entries
    if (b == BOUND_EXACT || (uint32_t)k != key32 || d > depth8 - 4) {
        key32     = (uint32_t)k;
        score16   = s;
        genBound8 = (Bound)(TT.generation8 | uint8_t(pv) << 2 | b);
        depth8    = d;
    }
}

// TranspositionTable::resize() sets the size of the transposition table,
// measured in megabytes. Transposition table consists of a power of 2 number
// of clusters and each cluster consists of ClusterSize number of TTEntry.
void TranspositionTable::resize(size_t mbSize) {
    Threads.main()->wait_for_search_finished();

    aligned_large_pages_free(table);

    clusterCount = mbSize * 1024 * 1024 / sizeof(Cluster);

    table = static_cast<Cluster *>(aligned_large_pages_alloc(clusterCount * sizeof(Cluster)));
    if (!table) {
        sync_cout << "MESSAGE failed to allocate " << mbSize << "mb for transposition table" << sync_endl;
        exit(EXIT_FAILURE);
    }

    clear();
}

// TranspositionTable::clear() initializes the entire transposition table to zero,
// in a multi-threaded way.
void TranspositionTable::clear() {
    std::vector<std::thread> threads;

    for (size_t idx = 0; idx < Threads.threadNum; ++idx) {
        threads.emplace_back([this, idx]() {
            // Thread binding gives faster search on systems with a first-touch policy
            if (Threads.threadNum > 8)
                bindThisThread(idx);

            // Each thread will zero its part of the hash table
            const size_t stride = size_t(clusterCount / Threads.threadNum),
                         start  = size_t(stride * idx),
                         len    = idx != Threads.threadNum - 1 ?
                                      stride :
                                      clusterCount - start;

            std::memset(&table[start], 0, len * sizeof(Cluster));
        });
    }

    for (std::thread &th : threads)
        th.join();
}

// TranspositionTable::probe() looks up the current position in the transposition
// table. It returns true and a pointer to the TTEntry if the position is found.
// Otherwise, it returns false and a pointer to an empty or least valuable TTEntry
// to be replaced later. The replace value of an entry is calculated as its depth
// minus 8 times its relative age. TTEntry t1 is considered more valuable than
// TTEntry t2 if its replace value is greater than that of t2.
TTEntry *TranspositionTable::probe(const ZobristKey key, bool &found) const {
    TTEntry *const tte   = first_entry(key);
    const uint32_t key32 = (uint32_t)key; // Use the low 32 bits as key inside the cluster

    for (int i = 0; i < ClusterSize; ++i)
        if (tte[i].key32 == key32 || !tte[i].depth8) {
            tte[i].genBound8 = Bound(generation8 | (tte[i].genBound8 & (GENERATION_DELTA - 1))); // Refresh

            return found = (bool)tte[i].depth8, &tte[i];
        }

    // Find an entry to be replaced according to the replacement strategy
    TTEntry *replace = tte;
    for (int i = 1; i < ClusterSize; ++i)
        // Due to our packed storage format for generation and its cyclic
        // nature we add GENERATION_CYCLE (256 is the modulus, plus what
        // is needed to keep the unrelated lowest n bits from affecting
        // the result) to calculate the entry age correctly even after
        // generation8 overflows into the next cycle.
        if (replace->depth8 - ((GENERATION_CYCLE + generation8 - replace->genBound8) & GENERATION_MASK) > tte[i].depth8 - ((GENERATION_CYCLE + generation8 - tte[i].genBound8) & GENERATION_MASK))
            replace = &tte[i];

    return found = false, replace;
}
