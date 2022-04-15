/*      _____                __    ______
 *     / ___ \              / /   /___  /
 *    / /__/ /___  ____  __/ /_______/ /    ____  ____
 *   / _____/ __ \/ __ \/_   _/ __  / /    / __ \/ __ \
 *  / /    /  ___/ / / / / /_/ /_/ / /____/  ___/ / / /
 * /_/     \____/_/ /_/ /___/\__,_/______/\____/_/ /_/
 *
 * PentaZen, a Gomoku/Renju playing engine developed by Sun Yuliang.
 */

#ifdef _WIN32
#    if _WIN32_WINNT < 0x0601
#        undef _WIN32_WINNT
#        define _WIN32_WINNT 0x0601 // Force to include needed API prototypes
#    endif

#    ifndef NOMINMAX
#        define NOMINMAX
#    endif

#    include <windows.h>

// The needed Windows API for processor groups could be missed from old Windows
// versions, so instead of calling them directly (forcing the linker to resolve
// the calls at compile time), try to load them at runtime. To do this we need
// first to define the corresponding function pointers.
extern "C" {
typedef bool (*fun1_t)(LOGICAL_PROCESSOR_RELATIONSHIP,
                       PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX,
                       PDWORD);
typedef bool (*fun2_t)(USHORT, PGROUP_AFFINITY);
typedef bool (*fun3_t)(HANDLE, CONST GROUP_AFFINITY*, PGROUP_AFFINITY);
}
#endif

#include "misc.h"

#include <vector>

// Used to serialize access to std::cout to avoid multiple threads writing at
// the same time.
std::ostream& operator<<(std::ostream& os, SyncCout sc) {
    static std::mutex m;

    if (sc == IO_LOCK)
        m.lock();

    if (sc == IO_UNLOCK)
        m.unlock();

    return os;
}

// prefetch() preloads the given address in L1/L2 cache. This is a non-blocking
// function that doesn't stall the CPU waiting for data to be loaded from memory,
// which can be quite slow.
#ifdef NO_PREFETCH
void prefetch(void*) {
}
#else
void prefetch(void* addr) {
#    if defined(__INTEL_COMPILER)
    // This hack prevents prefetches from being optimized away by
    // Intel compiler. Both MSVC and gcc seem not be affected by this.
    __asm__("");
#    endif
#    if defined(__INTEL_COMPILER) || defined(_MSC_VER)
    _mm_prefetch((char*)addr, _MM_HINT_T0);
#    else
    __builtin_prefetch(addr);
#    endif
}
#endif

// std_aligned_alloc() is our wrapper for systems where the c++17 implementation
// does not guarantee the availability of aligned_alloc(). Memory allocated with
// std_aligned_alloc() must be freed with std_aligned_free().
void* std_aligned_alloc(size_t alignment, size_t size) {
#if defined(POSIXALIGNEDALLOC)
    void* mem;
    return posix_memalign(&mem, alignment, size) ? nullptr : mem;
#elif defined(_WIN32)
    return _mm_malloc(size, alignment);
#else
    return std::aligned_alloc(alignment, size);
#endif
}

void std_aligned_free(void* ptr) {
#if defined(POSIXALIGNEDALLOC)
    free(ptr);
#elif defined(_WIN32)
    _mm_free(ptr);
#else
    free(ptr);
#endif
}

// aligned_large_pages_alloc() will return suitably aligned memory, if possible using large pages.
#if defined(_WIN32)

static void* aligned_large_pages_alloc_win(size_t allocSize) {
    HANDLE hProcessToken{};
    LUID   luid{};
    void*  mem = nullptr;

    const size_t largePageSize = GetLargePageMinimum();
    if (!largePageSize)
        return nullptr;

    // We need SeLockMemoryPrivilege, so try to enable it for the process
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hProcessToken))
        return nullptr;

    if (LookupPrivilegeValue(NULL, SE_LOCK_MEMORY_NAME, &luid)) {
        TOKEN_PRIVILEGES tp{};
        TOKEN_PRIVILEGES prevTp{};
        DWORD            prevTpLen = 0;

        tp.PrivilegeCount           = 1;
        tp.Privileges[0].Luid       = luid;
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

        // Try to enable SeLockMemoryPrivilege. Note that even if AdjustTokenPrivileges() succeeds,
        // we still need to query GetLastError() to ensure that the privileges were actually obtained.
        if (AdjustTokenPrivileges(
                hProcessToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), &prevTp, &prevTpLen) &&
            GetLastError() == ERROR_SUCCESS) {
            // Round up size to full pages and allocate
            allocSize = (allocSize + largePageSize - 1) & ~size_t(largePageSize - 1);
            mem       = VirtualAlloc(
                NULL, allocSize, MEM_RESERVE | MEM_COMMIT | MEM_LARGE_PAGES, PAGE_READWRITE);

            // Privilege no longer needed, restore previous state
            AdjustTokenPrivileges(hProcessToken, FALSE, &prevTp, 0, NULL, NULL);
        }
    }

    CloseHandle(hProcessToken);

    return mem;
}

void* aligned_large_pages_alloc(size_t allocSize) {
    // Try to allocate large pages
    void* mem = aligned_large_pages_alloc_win(allocSize);

    // Fall back to regular, page aligned, allocation if necessary
    if (!mem)
        mem = VirtualAlloc(NULL, allocSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    return mem;
}

#else

void* aligned_large_pages_alloc(size_t allocSize) {
#    if defined(__linux__)
    constexpr size_t alignment = 2 * 1024 * 1024; // assumed 2MB page size
#    else
    constexpr size_t alignment = 4096; // assumed small page size
#    endif

    // round up to multiples of alignment
    size_t size = ((allocSize + alignment - 1) / alignment) * alignment;
    void*  mem  = std_aligned_alloc(alignment, size);
#    if defined(MADV_HUGEPAGE)
    madvise(mem, size, MADV_HUGEPAGE);
#    endif
    return mem;
}

#endif

// aligned_large_pages_free() will free the previously allocated ttmem
#if defined(_WIN32)

void aligned_large_pages_free(void* mem) {
    if (mem && !VirtualFree(mem, 0, MEM_RELEASE)) {
        DWORD err = GetLastError();
        sync_cout << "MESSAGE Failed to free transposition table. Error code: 0x" << std::hex << err << std::dec << sync_endl;
        exit(EXIT_FAILURE);
    }
}

#else

void aligned_large_pages_free(void* mem) {
    std_aligned_free(mem);
}

#endif

#ifndef _WIN32

void bindThisThread(size_t) {
}

#else

// best_group() retrieves logical processor information using Windows specific
// API and returns the best group id for the thread with index idx. Original
// code from Texel by Peter Österlund.
int best_group(size_t idx) {
    int   threads      = 0;
    int   nodes        = 0;
    int   cores        = 0;
    DWORD returnLength = 0;
    DWORD byteOffset   = 0;

    // Early exit if the needed API is not available at runtime
    HMODULE k32  = GetModuleHandle("Kernel32.dll");
    auto    fun1 = (fun1_t)(void (*)())GetProcAddress(k32, "GetLogicalProcessorInformationEx");
    if (!fun1)
        return -1;

    // First call to get returnLength. We expect it to fail due to null buffer
    if (fun1(RelationAll, nullptr, &returnLength))
        return -1;

    // Once we know returnLength, allocate the buffer
    SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *buffer, *ptr;
    ptr = buffer = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*)malloc(returnLength);

    // Second call, now we expect to succeed
    if (!fun1(RelationAll, buffer, &returnLength)) {
        free(buffer);
        return -1;
    }

    while (byteOffset < returnLength) {
        if (ptr->Relationship == RelationNumaNode)
            nodes++;

        else if (ptr->Relationship == RelationProcessorCore) {
            cores++;
            threads += (ptr->Processor.Flags == LTP_PC_SMT) ? 2 : 1;
        }

        assert(ptr->Size);
        byteOffset += ptr->Size;
        ptr = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*)(((char*)ptr) + ptr->Size);
    }

    free(buffer);

    std::vector<int> groups;

    // Run as many threads as possible on the same node until core limit is
    // reached, then move on filling the next node.
    for (int n = 0; n < nodes; n++)
        for (int i = 0; i < cores / nodes; i++)
            groups.push_back(n);

    // In case a core has more than one logical processor (we assume 2) and we
    // have still threads to allocate, then spread them evenly across available
    // nodes.
    for (int t = 0; t < threads - cores; t++)
        groups.push_back(t % nodes);

    // If we still have more threads than the total number of logical processors
    // then return -1 and let the OS to decide what to do.
    return idx < groups.size() ? groups[idx] : -1;
}

// bindThisThread() set the group affinity of the current thread
void bindThisThread(size_t idx) {
    // Use only local variables to be thread-safe
    int group = best_group(idx);

    if (group == -1)
        return;

    // Early exit if the needed API are not available at runtime
    HMODULE k32  = GetModuleHandle("Kernel32.dll");
    auto    fun2 = (fun2_t)(void (*)())GetProcAddress(k32, "GetNumaNodeProcessorMaskEx");
    auto    fun3 = (fun3_t)(void (*)())GetProcAddress(k32, "SetThreadGroupAffinity");

    if (!fun2 || !fun3)
        return;

    GROUP_AFFINITY affinity;
    if (fun2(group, &affinity))
        fun3(GetCurrentThread(), &affinity, nullptr);
}

#endif
