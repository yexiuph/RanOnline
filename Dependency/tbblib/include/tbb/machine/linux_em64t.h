/*
    Copyright 2005-2007 Intel Corporation.  All Rights Reserved.

    This file is part of Threading Building Blocks.

    Threading Building Blocks is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
    version 2 as published by the Free Software Foundation.

    Threading Building Blocks is distributed in the hope that it will be
    useful, but WITHOUT ANY WARRANTY; without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Threading Building Blocks; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/

#ifndef __TBB_machine_H
#error Do not include this file directly; include tbb_machine.h instead
#endif

#include <stdint.h>
#include <unistd.h>
#include <sched.h>

#define __TBB_WORDSIZE 8
#define __TBB_BIG_ENDIAN 0

//! Load with acquire semantics, both for hardware and compiler.
template<typename T>
inline T __TBB_load_with_acquire_via_explicit_fence(const volatile T& location) {
    T tmp = location;
    __asm__ __volatile__("": : :"memory");
    return tmp;
}

//! Store with release semantics, both for hardware and compiler.
template<typename T, typename V>
inline void __TBB_store_with_release_via_explicit_fence(volatile T& location, V value) {
    __asm__ __volatile__("": : :"memory");
    location = value;
}

#define __TBB_load_with_acquire __TBB_load_with_acquire_via_explicit_fence
#define __TBB_store_with_release __TBB_store_with_release_via_explicit_fence

#define __MACHINE_DECL_ATOMICS(S,T,X) \
static inline T __TBB_machine_cmpswp##S (volatile void *ptr, T value, T comparand )         \
{                                                                                    \
    T result;                                                                        \
                                                                                     \
    __asm__ __volatile__("lock\ncmpxchg" X " %2,%1"                                       \
                          : "=a"(result), "=m"(*(T *)ptr)                                             \
                          : "q"(value), "0"(comparand)               \
                          : "memory");                                               \
    return result;                                                                   \
}                                                                                    \
                                                                                     \
static inline T __TBB_machine_fetchadd##S(volatile void *ptr, T addend)                     \
{                                                                                    \
    T result;                                                                        \
    __asm__ __volatile__("lock\nxadd" X " %0,%1"                                          \
                          : "=r"(result),"=m"(*(T *)ptr)                                             \
                          : "0"(addend)                              \
                          : "memory");                                               \
    return result;                                                                   \
}                                                                                    \
                                                                                     \
static inline  T __TBB_machine_fetchstore##S(volatile void *ptr, T value)                   \
{                                                                                    \
    T result;                                                                        \
    __asm__ __volatile__("lock\nxchg" X " %0,%1"                                          \
                          : "=r"(result),"=m"(*(T *)ptr)                                             \
                          : "0"(value)                               \
                          : "memory");                                               \
    return result;                                                                   \
}                                                                                    \
                                                                                     
__MACHINE_DECL_ATOMICS(1,int8_t,"")
__MACHINE_DECL_ATOMICS(2,int16_t,"")
__MACHINE_DECL_ATOMICS(4,int32_t,"")
__MACHINE_DECL_ATOMICS(8,int64_t,"q")

static inline int64_t __TBB_machine_lg( uint64_t x ) {
    int64_t j;
    __asm__ ("bsr %1,%0" : "=r"(j) : "r"(x));
    return j;
}

static inline void __TBB_machine_or( volatile void *ptr, uint64_t addend ) {
    __asm__ __volatile__("lock\norq %1,%0" : "=m"(*(uint64_t *)ptr) : "r"(addend) : "memory");
}

static inline void __TBB_machine_pause( int32_t delay ) {
    for (int32_t i = 0; i < delay; i++) {
       __asm__ __volatile__("pause;");
    }
    return;
}   

// Machine specific atomic operations

#define __TBB_CompareAndSwap1(P,V,C) __TBB_machine_cmpswp1(P,V,C)
#define __TBB_CompareAndSwap2(P,V,C) __TBB_machine_cmpswp2(P,V,C)
#define __TBB_CompareAndSwap4(P,V,C) __TBB_machine_cmpswp4(P,V,C)
#define __TBB_CompareAndSwap8(P,V,C) __TBB_machine_cmpswp8(P,V,C)
#define __TBB_CompareAndSwapW(P,V,C) __TBB_machine_cmpswp8(P,V,C)

#define __TBB_FetchAndAdd1(P,V) __TBB_machine_fetchadd1(P,V)
#define __TBB_FetchAndAdd2(P,V) __TBB_machine_fetchadd2(P,V)
#define __TBB_FetchAndAdd4(P,V) __TBB_machine_fetchadd4(P,V)
#define __TBB_FetchAndAdd8(P,V)  __TBB_machine_fetchadd8(P,V)
#define __TBB_FetchAndAddW(P,V)  __TBB_machine_fetchadd8(P,V)

#define __TBB_FetchAndStore1(P,V) __TBB_machine_fetchstore1(P,V)
#define __TBB_FetchAndStore2(P,V) __TBB_machine_fetchstore2(P,V)
#define __TBB_FetchAndStore4(P,V) __TBB_machine_fetchstore4(P,V)
#define __TBB_FetchAndStore8(P,V)  __TBB_machine_fetchstore8(P,V)
#define __TBB_FetchAndStoreW(P,V)  __TBB_machine_fetchstore8(P,V)

#define __TBB_Store8(P,V) (*P = V)
#define __TBB_Load8(P)    (*P)

#define __TBB_AtomicOR(P,V) __TBB_machine_or(P,V)

// Definition of other functions
#define __TBB_Yield()  sched_yield()
#define __TBB_Pause(V) __TBB_machine_pause(V)
#define __TBB_Log2(V)    __TBB_machine_lg(V)

// Special atomic functions
#define __TBB_FetchAndAddWrelease(P,V) __TBB_FetchAndAddW(P,V)
#define __TBB_FetchAndIncrementWacquire(P) __TBB_FetchAndAddW(P,1)
#define __TBB_FetchAndDecrementWrelease(P) __TBB_FetchAndAddW(P,-1)

// Definition of Lock functions
#undef __TBB_TryLockByte
#undef __TBB_LockByte

#define __TBB_cpuid
static inline void __TBB_x86_cpuid( int32_t buffer[4], int32_t mode ) {
    // NOTE: gcc sometimes fails to compile the following asm.  But icc always succeeds.
    __asm__ ("cpuid" : "=a"(buffer[0]),
                       "=b"(buffer[1]),
                       "=c"(buffer[2]),
                       "=d"(buffer[3]) : "0"(mode) : "memory" );
}

