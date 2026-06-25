#pragma once

/*
BrunoNotes C helper lib

References:
- nob.h (https://github.com/tsoding/nob.h)
- odin (https://odin-lang.org)
- gb.h (https://github.com/gingerBill/gb)

#define BN_IMPLEMENTATION
#include "bn.h"

// This library uses a global context called bn_context that must be initialized
bn_initContext((BN_ContextInitParams){});

// the bn_context contains a temp_allocator, that is basically a arena allocator
// that must be freed by the user, at the end of a main loop for example
bn_allocatorFreeAll(&bn_context->temp_allocator);

// (optional) destroy context
bn_destroyContext();

*/

#define BN_ASSERT_ENABLED // comment to disable asserts

#if defined(_WIN32) || defined(_WIN64)
#define BN_PLATFORM_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
#define BN_PLATFORM_OSX
#elif defined(__linux__)
#define BN_PLATFORM_LINUX
#endif

#if defined(__cplusplus)
extern "C" {
#endif

// clang-format off
#ifndef thread_local
    #define thread_local _Thread_local
#endif

#if defined(__GNUC__)
    #ifndef _VA_LIST_DEFINED
        #include <stdio.h>
        typedef __gnuc_va_list va_list;
        #define _VA_LIST_DEFINED
    #endif
#else
    #include <stdarg.h>
#endif

#ifndef va_arg
    #if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
        /* C23 uses a special builtin. */
        #define va_start(...) __builtin_c23_va_start(__VA_ARGS__)
    #else
        /* Versions before C23 do require the second parameter. */
        #define va_start(ap, param) __builtin_va_start(ap, param)
    #endif
    #define va_end(ap) __builtin_va_end(ap)
    #define va_arg(ap, type) __builtin_va_arg(ap, type)
#endif

#if _MSC_VER
    #include <intrin.h>
    #define debugBreak() __debugbreak()
#else
    #define debugBreak() __builtin_trap()
#endif
// clang-format on

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef i8 b8;
typedef i32 b32;
typedef i64 b64;
typedef float f32;
typedef double f64;
typedef size_t usize;
typedef ptrdiff_t isize;

#define bn_vec2Prototype(type)                                                 \
    typedef union U_Vec2##type {                                               \
        type elements[2];                                                      \
        struct {                                                               \
            union {                                                            \
                type x, y;                                                     \
            };                                                                 \
        };                                                                     \
    } Vec2##type;

bn_vec2Prototype(f32);
bn_vec2Prototype(f64);

#define bn_vec3Prototype(type)                                                 \
    typedef union U_Vec3##type {                                               \
        type elements[3];                                                      \
        struct {                                                               \
            union {                                                            \
                type x, y, z;                                                  \
            };                                                                 \
            union {                                                            \
                type r, g, b;                                                  \
            };                                                                 \
        };                                                                     \
    } Vec3##type;

bn_vec3Prototype(f32);
bn_vec3Prototype(f64);

#define bn_vec4Prototype(type)                                                 \
    typedef union U_Vec4##type {                                               \
        type elements[4];                                                      \
        struct {                                                               \
            union {                                                            \
                type x, y, z, w;                                               \
            };                                                                 \
            union {                                                            \
                type r, g, b, a;                                               \
            };                                                                 \
        };                                                                     \
    } Vec4##type;

bn_vec4Prototype(f32);
bn_vec4Prototype(f64);

#define bn_arrayLen(a) (sizeof(a) / sizeof(a[0]))

#define BN_SLICE_MAX_SIZE 256

#define bn_slicePrototype(type)                                                \
    typedef struct {                                                           \
        type items[BN_SLICE_MAX_SIZE];                                         \
        u32 count;                                                             \
    } BN_slice##type

bn_slicePrototype(i8);
bn_slicePrototype(i16);
bn_slicePrototype(i32);
bn_slicePrototype(i64);
bn_slicePrototype(u8);
bn_slicePrototype(u16);
bn_slicePrototype(u32);
bn_slicePrototype(u64);
bn_slicePrototype(b8);
bn_slicePrototype(b32);
bn_slicePrototype(b64);
bn_slicePrototype(f32);
bn_slicePrototype(f64);
bn_slicePrototype(Vec2f32);
bn_slicePrototype(Vec2f64);
bn_slicePrototype(Vec3f32);
bn_slicePrototype(Vec3f64);
bn_slicePrototype(Vec4f32);
bn_slicePrototype(Vec4f64);

#define bn_sliceAppend(slice, item)                                            \
    do {                                                                       \
        bn_assert((slice).count < BN_SLICE_MAX_SIZE);                         \
        (slice).items[(slice).count] = (item);                                 \
        (slice).count++;                                                       \
    } while (0)

/*
    BN_slicei32 slice = {0};
    bn_sliceAppendArray(slice, (i32[]) {0,1,2,3});
*/

#define bn_sliceAppendArray(slice, item)                                       \
    do {                                                                       \
        (slice).count += bn_arrayLen(item);                                    \
        bn_assert((slice).count <= BN_SLICE_MAX_SIZE);                         \
        for (u32 i = 0; i < (slice).count; i++) {                              \
            (slice).items[i] = (item)[i];                                      \
        }                                                                      \
    } while (0)

typedef struct {
    char* ptr;
    i64 length;
} String;

#define stringLit(s)                                                           \
    (String){(char*)(s), sizeof((s)) - 1} // makes a string from char*
#define stringFmt(s)                                                           \
    (s).length, (s).ptr // to be used with "%.*s" in printf for example

#define Byte 1
#define KiloByte (u64)(1024 * Byte)
#define MegaByte (u64)(1024 * KiloByte)
#define GigaByte (u64)(1024 * MegaByte)
#define TeraByte (u64)(1024 * GigaByte)
#define PetaByte (u64)(1024 * TeraByte)
#define ExaByte (u64)(1024 * PetaByte)

#define Kib(n) ((u64)(n) * KiloByte)
#define Mib(n) ((u64)(n) * MegaByte)
#define Gib(n) ((u64)(n) * GigaByte)

#define bn_min(a, b) (((a) < (b)) ? (a) : (b))
#define bn_max(a, b) (((a) > (b)) ? (a) : (b))
#define bn_abs(x) ((x) < 0 ? -(x) : (x))
#define bn_clamp(x, lower, upper) bn_min(bn_max((x), (lower)), (upper))

#define bn_alignPow2(x, b) (((x) + (b) - 1) & (~((b) - 1)))

typedef enum {
    BN_LogLevel_Debug = 0,
    BN_LogLevel_Info = 1,
    BN_LogLevel_Warning = 2,
    BN_LogLevel_Error = 3,
    BN_LogLevel_Panic = 4,
} BN_LogLevel;

void bn_defaultLogHandler(BN_LogLevel level, const char* fmt, va_list args);

typedef void(BN_LogHandler)(BN_LogLevel level, const char* fmt, va_list args);

void bn_log(BN_LogLevel level, const char* fmt, ...);

#define bn_logInfo(message) bn_log(BN_LogLevel_Info, message)
#define bn_logInfof(...) bn_log(BN_LogLevel_Info, __VA_ARGS__)
#define bn_logWarn(message) bn_log(BN_LogLevel_Warning, message)
#define bn_logWarnf(...) bn_log(BN_LogLevel_Warning, __VA_ARGS__)
#define bn_logError(message) bn_log(BN_LogLevel_Error, message)
#define bn_logErrorf(...) bn_log(BN_LogLevel_Error, __VA_ARGS__)
#define bn_logPanic(message) bn_log(BN_LogLevel_Panic, message)
#define bn_logPanicf(...) bn_log(BN_LogLevel_Panic, __VA_ARGS__)
#define bn_logDebug(message) bn_log(BN_LogLevel_Debug, message)
#define bn_logDebugf(...) bn_log(BN_LogLevel_Debug, __VA_ARGS__)
#define bn_logFatal(message) bn_log(BN_LogLevel_Fatal, message)
#define bn_logFatalf(...) bn_log(BN_LogLevel_Fatal, __VA_ARGS__)

// clang-format off
#if defined(BN_ASSERT_ENABLED)
    #define bn_assert(expr)                                                         \
        do {                                                                       \
            if (expr) {                                                            \
            } else {                                                               \
                bn_logErrorf(                                                       \
                    "Assertion Failure: %s, in file: %s, line: %d", #expr,         \
                    __FILE__, __LINE__                                             \
                );                                                                 \
                debugBreak();                                                      \
            }                                                                      \
        } while (0)

    #define bn_assertMsg(expr, msg)                                                 \
        do {                                                                       \
            if (expr) {                                                            \
            } else {                                                               \
                bn_logErrorf(                                                       \
                    "Assertion Failure: %s, in file: %s, line: %d, msg: %s",       \
                    #expr, __FILE__, __LINE__, msg                                 \
                );                                                                 \
                debugBreak();                                                      \
            }                                                                      \
        } while (0)
#else
    #define bn_assert(expr)
    #define bn_assertMsg(expr, msg)
#endif
// clang-format on

u32 bn_platformGetPageSize(void);
void* bn_platformMemReserve(u64 size);
b32 bn_platformMemCommit(void* ptr, u64 size);
b32 bn_platformMemDecommit(void* ptr, u64 size);
b32 bn_platformMemRelease(void* ptr, u64 size);

#define BN_ARENA_BASE_POS (sizeof(BN_Arena))
#define BN_ARENA_ALIGN (sizeof(void*))

typedef enum {
    BN_ArentaType_Static,
    BN_ArenaType_Growing,
} BN_ArenaType;

typedef struct {
    u64 reserve_size;
    u64 commit_size;
    u64 pos;
    u64 commit_pos;
    BN_ArenaType type;
} BN_Arena;

BN_Arena* bn_initArena(u64 reserve_size, u64 commit_size, BN_ArenaType type);
void bn_destroyArena(BN_Arena* arena);

void* bn_arenaPush(BN_Arena* arena, u64 size, bool non_zero);
void bn_arenaRealloc(BN_Arena* arena, u64 new_pos);
void bn_arenaPop(BN_Arena* arena, u64 size);
void bn_arenaPopTo(BN_Arena* arena, u64 pos);

typedef struct {
    BN_Arena* arena;
    u64 start_pos;
} BN_TempArena;

BN_TempArena bn_beginTempArena(BN_Arena* arena);
void bn_endTempArena(BN_TempArena temp);

void* bn_heapAllocatorPush(u64 size, bool non_zero);
void bn_heapAllocatorFree(void* data, u64 size);

typedef enum {
    BN_AllocatorType_ArenaStatic,
    BN_AllocatorType_ArenaGrowing,
    BN_AllocatorType_HeapAllocator,
    BN_AllocatorType_TempAllocator,
} BN_AllocatorType;

typedef struct {
    u64 reserve_size;
    u64 commit_size;
    BN_AllocatorType type;
} BN_AllocatorInitParams;

typedef struct {
    u64 size;
    b32 non_zero;
} BN_AllocatorParams;

typedef struct {
    BN_AllocatorType type;
    void* data;
    u64 size;
    BN_TempArena _temp;
} BN_Allocator;

BN_Allocator bn_initAllocator(BN_AllocatorInitParams params);
void bn_destroyAllocator(BN_Allocator* alloc);

void* bn_allocatorPush(BN_AllocatorParams params, BN_Allocator* alloc);
void bn_allocatorFree(BN_Allocator* alloc, void* data);
void bn_allocatorFreeAll(BN_Allocator* alloc);

#define bn_allocPushStruct(alloc, T)                                           \
    (T*)bn_allocatorPush((BN_AllocatorParams){sizeof(T), false}, (alloc))
#define bn_allocPushStructNZ(alloc, T)                                         \
    (T*)bn_allocatorPush((BN_AllocatorParams){sizeof(T), true}, (alloc))
#define bnAllocPushArray(alloc, T, len)                                        \
    (T*)bn_allocatorPush(                                                      \
        (BN_AllocatorParams){sizeof(T) * (len), false}, (alloc)                \
    )
#define bnAllocPushArrayNZ(alloc, T, len)                                      \
    (T*)bn_allocatorPush((BN_AllocatorParams){sizeof(T) * (len), true}, (alloc))

#define bn_dArrayPrototype(type)                                               \
    typedef struct {                                                           \
        type* items;                                                           \
        u32 count;                                                             \
        u32 capacity;                                                          \
        BN_Allocator* allocator;                                               \
    } BN_array##type

bn_dArrayPrototype(i8);
bn_dArrayPrototype(i16);
bn_dArrayPrototype(i32);
bn_dArrayPrototype(i64);
bn_dArrayPrototype(u8);
bn_dArrayPrototype(u16);
bn_dArrayPrototype(u32);
bn_dArrayPrototype(u64);
bn_dArrayPrototype(b8);
bn_dArrayPrototype(b32);
bn_dArrayPrototype(b64);
bn_dArrayPrototype(f32);
bn_dArrayPrototype(f64);
bn_dArrayPrototype(String);
bn_dArrayPrototype(Vec2f32);
bn_dArrayPrototype(Vec2f64);
bn_dArrayPrototype(Vec3f32);
bn_dArrayPrototype(Vec3f64);
bn_dArrayPrototype(Vec4f32);
bn_dArrayPrototype(Vec4f64);

#ifndef BN_DYNAMIC_ARRAY_INITIAL_CAPACITY
#define BN_DYNAMIC_ARRAY_INITIAL_CAPACITY 256
#endif

#define bn_daInit(alloc)                                                       \
    {.items = NULL, .count = 0, .capacity = 0, .allocator = alloc}

#define bn_daReserve(da, expected_capacity)                                    \
    do {                                                                       \
        if ((expected_capacity) > (da)->capacity) {                            \
            if ((da)->capacity == 0) {                                         \
                (da)->capacity = BN_DYNAMIC_ARRAY_INITIAL_CAPACITY;            \
                (da)->items = bn_allocatorPush(                                \
                    (BN_AllocatorParams){                                      \
                        (da)->capacity * sizeof(*(da)->items), false           \
                    },                                                         \
                    (da)->allocator                                            \
                );                                                             \
            } else {                                                           \
                u32 old_capacity = (da)->capacity;                             \
                while ((expected_capacity) > (da)->capacity) {                 \
                    (da)->capacity *= 2;                                       \
                }                                                              \
                void* new_entries = bn_allocatorPush(                          \
                    (BN_AllocatorParams){                                      \
                        (da)->capacity * sizeof(*(da)->items), false           \
                    },                                                         \
                    (da)->allocator                                            \
                );                                                             \
                memcpy(                                                        \
                    new_entries, (da)->items,                                  \
                    old_capacity * sizeof(*(da)->items)                        \
                );                                                             \
                bn_allocatorFree((da)->allocator, (da)->items);                \
                (da)->items = new_entries;                                     \
            }                                                                  \
            bn_assert((da)->items != NULL);                                    \
        }                                                                      \
    } while (0)

#define bn_daAppend(da, item)                                                  \
    do {                                                                       \
        bn_daReserve((da), (da)->count + 1);                                   \
        (da)->items[(da)->count++] = (item);                                   \
    } while (0)

#define bn_daAppendMany(da, new_items, new_items_count)                        \
    do {                                                                       \
        bn_daReserve((da), (da)->count + (new_items_count));                   \
        memcpy(                                                                \
            (da)->items + (da)->count, (new_items),                            \
            (new_items_count) * sizeof(*(da)->items)                           \
        );                                                                     \
        (da)->count += (new_items_count);                                      \
    } while (0)

#define bn_daFree(da)                                                          \
    do {                                                                       \
        bn_allocatorFree((da)->allocator, (da)->items);                        \
        (da)->count = 0;                                                       \
        (da)->capacity = 0;                                                    \
    } while (0)

#define bn_daResize(da, new_size)                                              \
    do {                                                                       \
        bn_daReserve((da), new_size);                                          \
        (da)->count = (new_size);                                              \
    } while (0)

#ifndef BN_HASH_TABLE_INITIAL_CAPACITY
#define BN_HASH_TABLE_INITIAL_CAPACITY 256
#endif

typedef struct {
    String key;
    void* value;
} BN_HashTableEntry;

typedef struct {
    BN_HashTableEntry* items;
    u32 count;
    u32 capacity;
    BN_Allocator* allocator;
} BN_HashTable;

#define bn_htInit(alloc)                                                       \
    {.items = NULL, .count = 0, .capacity = 0, .allocator = alloc}

void bn_hashTableReserve(BN_HashTable* table, u64 expected_capacity);
void bn_hashTableFree(BN_HashTable* table);
u32 bn_hashTableLinearProbe(BN_HashTable* table, String key);
void* bn_hashTableGet(BN_HashTable* table, String key);
void bn_hashTableAppend(BN_HashTable* table, String key, void* value);

#define bn_htAppend(ht, key, value)                                            \
    bn_hashTableAppend(ht, stringLit(key), (void*)value)

#define bn_htGet(ht, key) bn_hashTableGet(ht, stringLit(key))

typedef struct {
    BN_Allocator allocator;
    BN_Allocator temp_allocator;
    BN_LogHandler* log_handler;
    BN_LogLevel log_level;
} BN_Context;

typedef struct {
    BN_AllocatorInitParams allocator_params;
} BN_ContextInitParams;

// only used to initialize the context on the heap
static thread_local BN_Allocator _bn_context_allocator = {0};
static thread_local BN_Context* bn_context = {0};

void bn_initContext(BN_ContextInitParams params);
void bn_destroyContext();

// ----------------------------------------------------------------------------
// init BN_IMPLEMENTATION
// ----------------------------------------------------------------------------

#if defined(BN_IMPLEMENTATION)

#include <stdlib.h>
#include <string.h>

#if defined(BN_PLATFORM_WINDOWS)
#include <windows.h>
#else
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#endif

void bn_defaultLogHandler(BN_LogLevel level, const char* fmt, va_list args) {
    if (level < bn_context->log_level) {
        return;
    }

    switch (level) {
    case BN_LogLevel_Debug:
        fprintf(stderr, "\x1b[90m[Debug]\x1b[0m ");
        break;
    case BN_LogLevel_Info:
        fprintf(stderr, "\x1b[0m[Info]\x1b[0m ");
        break;
    case BN_LogLevel_Warning:
        fprintf(stderr, "\x1b[33m[Warn]\x1b[0m ");
        break;
    case BN_LogLevel_Error:
        fprintf(stderr, "\x1b[31m[Error]\x1b[0m ");
        break;
    case BN_LogLevel_Panic:
        fprintf(stderr, "\x1b[35m[Panic]\x1b[0m ");
        break;
    default:
        return;
    }

    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");

    if (level == BN_LogLevel_Panic) {
        exit(1);
    }
}

void bn_log(BN_LogLevel level, const char* fmt, ...) {
    if (bn_context == NULL)
        return;
    if (bn_context->log_handler == NULL)
        return;

    va_list args;
    va_start(args, fmt);

    bn_context->log_handler(level, fmt, args);

    va_end(args);
}

#if defined(BN_PLATFORM_WINDOWS)

u32 bn_platformGetPageSize(void) {
    SYSTEM_INFO sysinfo = {0};
    GetSystemInfo(&sysinfo);

    return sysinfo.dwPageSize;
}

void* bn_platformMemReserve(u64 size) {
    return VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_READWRITE);
}

b32 bn_platformMemCommit(void* ptr, u64 size) {
    void* ret = VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE);
    return ret != NULL;
}

b32 bn_platformMemDecommit(void* ptr, u64 size) {
    return VirtualFree(ptr, size, MEM_DECOMMIT);
}

b32 bn_platformMemRelease(void* ptr, u64 size) {
    return VirtualFree(ptr, size, MEM_RELEASE);
}

#elif defined(BN_PLATFORM_LINUX)

u32 bn_platformGetPageSize(void) {
    return (u32)sysconf(_SC_PAGESIZE);
}

void* bn_platformMemReserve(u64 size) {
    void* out = mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (out == MAP_FAILED) {
        return NULL;
    }
    return out;
}

b32 bn_platformMemCommit(void* ptr, u64 size) {
    i32 ret = mprotect(ptr, size, PROT_READ | PROT_WRITE);
    return ret == 0;
}

b32 bn_platformMemDecommit(void* ptr, u64 size) {
    i32 ret = mprotect(ptr, size, PROT_NONE);
    if (ret != 0)
        return false;
    ret = madvise(ptr, size, MADV_DONTNEED);
    return ret == 0;
}

b32 bn_platformMemRelease(void* ptr, u64 size) {
    i32 ret = munmap(ptr, size);
    return ret == 0;
}

#endif

BN_Arena* bn_initArena(u64 reserve_size, u64 commit_size, BN_ArenaType type) {
    bn_assert(reserve_size > 0);
    bn_assert(commit_size > 0);

    u32 page_size = bn_platformGetPageSize();

    reserve_size = bn_alignPow2(reserve_size, page_size);
    commit_size = bn_alignPow2(commit_size, page_size);

    BN_Arena* arena = (BN_Arena*)bn_platformMemReserve(reserve_size);

    if (!bn_platformMemCommit(arena, commit_size)) {
        return NULL;
    }

    arena->reserve_size = reserve_size;
    arena->commit_size = commit_size;
    arena->pos = BN_ARENA_BASE_POS;
    arena->commit_pos = commit_size;
    arena->type = type;

    return arena;
}

void bn_destroyArena(BN_Arena* arena) {
    bool ok = bn_platformMemRelease(arena, arena->reserve_size);
    bn_assert(ok == true);
}

void* bn_arenaPush(BN_Arena* arena, u64 size, bool non_zero) {
    u64 pos_aligned = bn_alignPow2(arena->pos, BN_ARENA_ALIGN);
    u64 new_pos = pos_aligned + size;

    // Grow arena if needed
    if (new_pos > arena->reserve_size && arena->type == BN_ArenaType_Growing) {
        bn_arenaRealloc(arena, new_pos);
    }

    if (new_pos > arena->commit_pos) {
        u64 new_commit_pos = new_pos;
        new_commit_pos += arena->commit_size - 1;
        new_commit_pos -= new_commit_pos % arena->commit_size;
        new_commit_pos = bn_min(new_commit_pos, arena->reserve_size);

        u8* mem = (u8*)arena + arena->commit_pos;
        u64 commit_size = new_commit_pos - arena->commit_pos;

        if (!bn_platformMemCommit(mem, commit_size)) {
            return NULL;
        }

        arena->commit_pos = new_commit_pos;
    }

    arena->pos = new_pos;

    u8* out = (u8*)arena + pos_aligned;

    if (!non_zero) {
        memset(out, 0, size);
    }

    return out;
}

void bn_arenaRealloc(BN_Arena* arena, u64 new_pos) {
    u64 new_reserve_size = arena->reserve_size * 2;
    while (new_reserve_size < new_pos) {
        new_reserve_size *= 2;
    }

    u32 page_size = bn_platformGetPageSize();
    new_reserve_size = bn_alignPow2(new_reserve_size, page_size);

    BN_Arena* new_arena = (BN_Arena*)bn_platformMemReserve(new_reserve_size);
    bn_assert(new_arena != NULL);

    // Copy existing data and commit the current committed size
    memcpy(new_arena, arena, arena->commit_pos);
    if (!bn_platformMemCommit(new_arena, arena->commit_pos)) {
        bn_platformMemRelease(new_arena, new_reserve_size);
        return;
    }

    new_arena->reserve_size = new_reserve_size;
    new_arena->commit_size = arena->commit_size;
    new_arena->pos = arena->pos;
    new_arena->commit_pos = arena->commit_pos;

    // Release old arena and update pointer
    bn_platformMemRelease(arena, arena->reserve_size);

    arena = new_arena;
}

void bn_arenaPop(BN_Arena* arena, u64 size) {
    size = bn_min(size, arena->pos - BN_ARENA_BASE_POS);
    arena->pos -= size;
}

void bn_arenaPopTo(BN_Arena* arena, u64 pos) {
    u64 size = pos < arena->pos ? arena->pos - pos : 0;
    bn_arenaPop(arena, size);
}

BN_TempArena bn_beginTempArena(BN_Arena* arena) {
    return (BN_TempArena){
        .arena = arena,
        .start_pos = arena->pos,
    };
}

void bn_endTempArena(BN_TempArena temp) {
    bn_arenaPopTo(temp.arena, temp.start_pos);
}

BN_Allocator bn_initAllocator(BN_AllocatorInitParams params) {
    BN_Allocator alloc = {0};

    switch (params.type) {
    case BN_AllocatorType_ArenaStatic:
        bn_logDebugf(
            "BN: initializing static arena allocator, reserve size: %d",
            params.reserve_size
        );

        alloc.data = bn_initArena(
            params.reserve_size, params.commit_size, BN_ArentaType_Static
        );
        alloc.type = params.type;
        break;
    case BN_AllocatorType_ArenaGrowing:
        if (params.reserve_size <= 0) {
            params.reserve_size = Mib(32);
        }
        if (params.commit_size <= 0) {
            params.commit_size = Mib(1);
        }

        bn_logDebugf(
            "BN: initializing growing arena allocator, reserve size: %d",
            params.reserve_size
        );

        alloc.data = bn_initArena(
            params.reserve_size, params.commit_size, BN_ArenaType_Growing
        );
        alloc.type = params.type;
        break;
    case BN_AllocatorType_HeapAllocator:
        bn_logDebugf("BN: initializing heap allocator");
        alloc.data = NULL;
        alloc.size = 0;
        alloc.type = params.type;
        break;
    case BN_AllocatorType_TempAllocator:
        bn_logDebugf(
            "BN: initializing temp allocator, reserve size: %d",
            params.reserve_size
        );

        alloc.data = bn_initArena(
            params.reserve_size, params.commit_size, BN_ArenaType_Growing
        );
        alloc.type = params.type;
        alloc._temp = bn_beginTempArena((BN_Arena*)alloc.data);
        break;
    default:
        bn_logPanic("Allocator not implemented");
        break;
    }

    return alloc;
}

void bn_destroyAllocator(BN_Allocator* alloc) {
    switch (alloc->type) {
    case BN_AllocatorType_ArenaStatic:
    case BN_AllocatorType_ArenaGrowing:
        bn_logDebugf("BN: destroying arena allocator");

        bn_destroyArena((BN_Arena*)alloc->data);
        break;
    case BN_AllocatorType_HeapAllocator:
        break;
    case BN_AllocatorType_TempAllocator:
        bn_logDebugf("BN: destroying temp allocator");

        bn_endTempArena(alloc->_temp);
        bn_destroyArena((BN_Arena*)alloc->data);
        break;
    default:
        bn_logPanic("Allocator not implemented");
        break;
    }
}

void* bn_heapAllocatorPush(u64 size, bool non_zero) {
    // void* data = malloc(size);
    u32 page_size = bn_platformGetPageSize();
    size = bn_alignPow2(size, page_size);
    void* data = bn_platformMemReserve(size);

    if (!bn_platformMemCommit(data, size)) {
        return NULL;
    }

    if (!non_zero) {
        memset(data, 0, size);
    }

    return data;
}

void bn_heapAllocatorFree(void* data, u64 size) {
    // free(data);
    // data = NULL;
    bn_platformMemRelease(data, size);
}

void* bn_allocatorPush(BN_AllocatorParams params, BN_Allocator* alloc) {
    switch (alloc->type) {
    case BN_AllocatorType_ArenaStatic:
    case BN_AllocatorType_ArenaGrowing:
        return bn_arenaPush(
            (BN_Arena*)alloc->data, params.size, params.non_zero
        );
    case BN_AllocatorType_HeapAllocator:
        alloc->size = params.size;
        return bn_heapAllocatorPush(params.size, params.non_zero);
    case BN_AllocatorType_TempAllocator:
        return bn_arenaPush(
            (BN_Arena*)alloc->_temp.arena, params.size, params.non_zero
        );
    default:
        bn_logPanic("Allocator not initialized");
        return NULL;
    }
}

void bn_allocatorFree(BN_Allocator* alloc, void* data) {
    switch (alloc->type) {
    case BN_AllocatorType_ArenaStatic:
    case BN_AllocatorType_ArenaGrowing:
    case BN_AllocatorType_TempAllocator:
        break;
    case BN_AllocatorType_HeapAllocator:
        bn_heapAllocatorFree(data, alloc->size);
        break;
    default:
        bn_logPanic("Allocator not initialized");
        break;
    }
}

void bn_allocatorFreeAll(BN_Allocator* alloc) {
    switch (alloc->type) {
    case BN_AllocatorType_ArenaStatic:
    case BN_AllocatorType_ArenaGrowing:
        bn_arenaPopTo((BN_Arena*)alloc->data, BN_ARENA_BASE_POS);
        break;
    case BN_AllocatorType_HeapAllocator:
        break;
    case BN_AllocatorType_TempAllocator:
        bn_endTempArena(alloc->_temp);
        alloc->_temp = bn_beginTempArena((BN_Arena*)alloc->data);
        break;
    default:
        bn_logPanic("Allocator not initialized");
        break;
    }
}

static inline u32 bn_fvn32aHash(u8* data, u32 length) {
    u32 h = 0x811c9dc5;

    for (u32 i = 0; i < length; i++) {
        h = (h ^ (u32)data[i]) * 0x01000193;
    }

    return h;
}

void bn_hashTableReserve(BN_HashTable* table, u64 expected_capacity) {
    if (expected_capacity > table->capacity) {
        if (table->capacity == 0) {
            table->capacity = BN_HASH_TABLE_INITIAL_CAPACITY;
            table->items = (BN_HashTableEntry*)bn_allocatorPush(
                (BN_AllocatorParams){
                    table->capacity * sizeof(*table->items), false
                },
                table->allocator
            );
        } else {
            while (expected_capacity > table->capacity) {
                table->capacity *= 2;
            }
            BN_HashTableEntry* new_entries =
                (BN_HashTableEntry*)bn_allocatorPush(
                    (BN_AllocatorParams){
                        table->capacity * sizeof(*table->items), false
                    },
                    table->allocator
                );

            for (u32 i = 0; i < table->capacity; i++) {
                if (table->items[i].key.ptr != NULL) {
                    new_entries[i] = table->items[i];
                }
            }

            bn_allocatorFree(table->allocator, table->items);
            table->items = new_entries;
        }
        bn_assert(table->items != NULL);
    }
}

void bn_hashTableFree(BN_HashTable* table) {
    bn_allocatorFree(table->allocator, table->items);
    table->count = 0;
    table->capacity = 0;
}

u32 bn_hashTableLinearProbe(BN_HashTable* table, String key) {
    u32 hash = bn_fvn32aHash((u8*)key.ptr, key.length);
    u32 idx = (u32)(hash & (u32)(table->capacity - 1));

    while (table->items[idx].key.ptr != NULL) {
        if (strcmp(key.ptr, table->items[idx].key.ptr) == 0) {
            // Found key
            return idx;
        }
        // Key wasn't in this slot, move to next (linear probing).
        idx++;
        if (idx >= table->capacity) {
            // At end of entries array, wrap around.
            idx = 0;
        }
    }
    return idx;
}

void* bn_hashTableGet(BN_HashTable* table, String key) {
    u32 idx = bn_hashTableLinearProbe(table, key);

    return table->items[idx].value;
}

void bn_hashTableAppend(BN_HashTable* table, String key, void* value) {
    bn_assert(value != NULL);

    bn_hashTableReserve(table, table->count + 1);

    u32 idx = bn_hashTableLinearProbe(table, key);

    if (table->items[idx].key.ptr == NULL) {
        table->items[idx].key = key;
    }
    table->items[idx].value = value;
    table->count++;
}

void bn_initContext(BN_ContextInitParams params) {
    _bn_context_allocator = bn_initAllocator((BN_AllocatorInitParams){
        .reserve_size = Byte * 2,
        .commit_size = Byte * 2,
        .type = BN_AllocatorType_ArenaStatic,
    });

    bn_context = bn_allocPushStruct(&_bn_context_allocator, BN_Context);

#if defined(DEBUG)
    bn_context->log_level = BN_LogLevel_Debug;
#else
    bn_context->log_level = BN_LogLevel_Info;
#endif

    if (bn_context->log_handler == NULL) {
        bn_context->log_handler = &bn_defaultLogHandler;
        bn_logDebugf("BN: default log handler loaded");
    }

    if (
        params.allocator_params.reserve_size == 0
        && params.allocator_params.commit_size == 0
        && params.allocator_params.type == 0 //
    ) {
        params.allocator_params.reserve_size = Mib(4);
        params.allocator_params.commit_size = Kib(2);
        params.allocator_params.type = BN_AllocatorType_ArenaGrowing;
    }

    bn_context->allocator = bn_initAllocator(params.allocator_params);

    bn_context->temp_allocator = bn_initAllocator((BN_AllocatorInitParams){
        .reserve_size = Mib(4),
        .commit_size = Kib(2),
        .type = BN_AllocatorType_TempAllocator,
    });
}

void bn_destroyContext() {
    bn_destroyAllocator(&bn_context->temp_allocator);
    bn_destroyAllocator(&bn_context->allocator);

    bn_logDebugf("BN: destroying context");

    bn_destroyAllocator(&_bn_context_allocator);
}

#endif

#if defined(__cplusplus)
}
#endif
