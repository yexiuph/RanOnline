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
#define __TBB_machine_H

#if _WIN32||_WIN64
// define the parts of stdint.h that are needed 
typedef __int8 int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
typedef __int64 int64_t;
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#else
#include <stdint.h>
#endif

#include <cstdio>
#include <assert.h>
#include "tbb/tbb_stddef.h"

#if _WIN32||_WIN64

#if defined(_M_IX86)
#include "tbb/machine/windows_ia32.h"
#elif defined(_M_AMD64) 
#include "tbb/machine/windows_em64t.h"
#else
#error Unsupported platform
#endif

#elif __linux__

#if __i386__
#include "tbb/machine/linux_ia32.h"
#elif __x86_64__
#include "tbb/machine/linux_em64t.h"
#elif __ia64__
#include "tbb/machine/linux_itanium.h"
#endif

#elif __APPLE__

#if __i386__
#include "tbb/machine/linux_ia32.h"
#elif __x86_64__
#include "tbb/machine/linux_em64t.h"
#elif __POWERPC__
#include "tbb/machine/mac_ppc.h"
#endif

#endif

#if !defined(__TBB_CompareAndSwap4) || !defined(__TBB_CompareAndSwap8) || !defined(__TBB_Yield)
#error Minimal requirements for tbb_machine.h not satisfied 
#endif

#if defined(TBB_DO_ASSERT)
#define WORDSIZE __TBB_WORDSIZE
#endif

#ifndef __TBB_load_with_acquire
    //! This definition works for compilers that insert acquire fences for volatile loads, and load T atomically.
    template<typename T>
    inline T __TBB_load_with_acquire(T const volatile& location) {
        return location;
    }
#endif

#ifndef __TBB_store_with_release
    //! This definition works only for compilers that insert release fences for volatile stores, and store T atomically.
    template<typename T, typename V>
    inline void __TBB_store_with_release(volatile T &location, V const& value) {
        location = value; 
    }
#endif

#ifndef __TBB_Pause
    inline void __TBB_Pause(int32_t) {
        __TBB_Yield();
    }
#endif

namespace tbb {
namespace internal {

//! Class that implements exponential backoff.
/** See implementation of SpinwaitWhileEq for an example. */
class AtomicBackoff {
    //! Time delay, in units of "pause" instructions. 
    /** Should be equal to approximately the number of "pause" instructions
        that take the same time as an context switch. */
    static const int32_t LOOPS_BEFORE_YIELD = 16;
    int32_t count;
public:
    AtomicBackoff() : count(1) {}

    //! Pause for a while.
    void pause() {
        if( count<=LOOPS_BEFORE_YIELD ) {
            __TBB_Pause(count);
            // Pause twice as long the next time.
            count*=2;
        } else {
            // Pause is so long that we might as well yield CPU to scheduler.
            __TBB_Yield();
        }
    }
    void reset() {
        count = 1;
    }
};

template<size_t S, typename T>
inline intptr_t __TBB_MaskedCompareAndSwap (volatile int32_t *ptr, T value, T comparand ) {
    volatile T *base = (T *)( (uintptr_t)(__TBB_load_with_acquire(ptr)) & ~(uintptr_t)(0x3) );
#if __TBB_BIG_ENDIAN
    uint8_t bitoffset = ( (4-S) - ( (uint8_t *)ptr - (uint8_t *)base) ) * 8;
#else
    uint8_t bitoffset = ( (uint8_t *)ptr - (uint8_t *)base ) * 8;
#endif
    uint32_t mask = ( (1<<(S*8) ) - 1)<<bitoffset;
    uint32_t tmp, result = *(uint32_t *)base;
    if ( (T)( (result & mask) >> bitoffset) == (uint32_t)comparand ) {
      tmp = result;
      uint32_t new_value = ( result & ~mask ) | ( value << bitoffset );
      result = __TBB_CompareAndSwap4( base, new_value, tmp );
      if ( result != tmp && ( (T)( (result & mask) >> bitoffset) == comparand ) ) {
        AtomicBackoff b;
        do {
          b.pause();
          tmp = result;
          uint32_t new_value = ( result & ~mask ) | ( value << bitoffset );
          result = __TBB_CompareAndSwap4( base, new_value, tmp );
        } while ( result != tmp && ( (T)( (result & mask) >> bitoffset) == comparand ) );
      }
    }
    intptr_t to_return;
    __TBB_store_with_release(to_return, (T)( (result & mask) >> bitoffset));
    return to_return;
}

template<size_t S, typename T>
inline T __TBB_CompareAndSwapGeneric (volatile void *ptr, T value, T comparand ) { 
    return __TBB_CompareAndSwapW((T *)ptr,value,comparand);
}

template<>
inline uint8_t __TBB_CompareAndSwapGeneric <1,uint8_t> (volatile void *ptr, uint8_t value, uint8_t comparand ) {
#ifdef __TBB_CompareAndSwap1
    return __TBB_CompareAndSwap1(ptr,value,comparand);
#else
    return __TBB_MaskedCompareAndSwap<1,uint8_t>((volatile int32_t *)ptr,value,comparand);
#endif
}

template<>
inline uint16_t __TBB_CompareAndSwapGeneric <2,uint16_t> (volatile void *ptr, uint16_t value, uint16_t comparand ) {
#ifdef __TBB_CompareAndSwap2
    return __TBB_CompareAndSwap2(ptr,value,comparand);
#else
    return __TBB_MaskedCompareAndSwap<2,uint16_t>((volatile int32_t *)ptr,value,comparand);
#endif
}

template<>
inline uint32_t __TBB_CompareAndSwapGeneric <4,uint32_t> (volatile void *ptr, uint32_t value, uint32_t comparand ) { 
    return __TBB_CompareAndSwap4(ptr,value,comparand);
}

template<>
inline uint64_t __TBB_CompareAndSwapGeneric <8,uint64_t> (volatile void *ptr, uint64_t value, uint64_t comparand ) { 
    return __TBB_CompareAndSwap8(ptr,value,comparand);
}

template<size_t S, typename T>
inline T __TBB_FetchAndAddGeneric (volatile void *ptr, T addend) {
    T result = __TBB_load_with_acquire(*reinterpret_cast<volatile T *>(ptr));
    T tmp = result;
    result = __TBB_CompareAndSwapGeneric<S,T> ( ptr, result+addend, result );
    if (tmp != result) {
        AtomicBackoff b;
        do {
            b.pause();
            tmp = result;
            result = __TBB_CompareAndSwapGeneric<S,T> ( ptr, result+addend, result );
         } while ( tmp != result);
    }
    intptr_t to_return; 
    __TBB_store_with_release(to_return, result);
    return to_return; 
}

template<size_t S, typename T>
inline T __TBB_FetchAndStoreGeneric (volatile void *ptr, T value) {
    T result; 
    T tmp = __TBB_load_with_acquire(*reinterpret_cast<volatile T *>(ptr));
    result = __TBB_CompareAndSwapGeneric<S,T> ( ptr, value, tmp );
    if ( tmp != result ) {
        AtomicBackoff b;
        do {
            b.pause();
            tmp = result;
            result = __TBB_CompareAndSwapGeneric<S,T> ( ptr, value, tmp );
        } while ( tmp != result );
    }
    intptr_t to_return; 
    __TBB_store_with_release(to_return, result);
    return to_return; 
}

}
}

#ifndef __TBB_CompareAndSwap1
#define __TBB_CompareAndSwap1 tbb::internal::__TBB_CompareAndSwapGeneric<1,uint8_t>
#endif

#ifndef __TBB_CompareAndSwap2 
#define __TBB_CompareAndSwap2 tbb::internal::__TBB_CompareAndSwapGeneric<2,uint16_t>
#endif

#ifndef __TBB_CompareAndSwapW
#define __TBB_CompareAndSwapW tbb::internal::__TBB_CompareAndSwapGeneric<sizeof(ptrdiff_t),ptrdiff_t>
#endif

#ifndef __TBB_FetchAndAdd1
#define __TBB_FetchAndAdd1 tbb::internal::__TBB_FetchAndAddGeneric<1,uint8_t>
#endif

#ifndef __TBB_FetchAndAdd2
#define __TBB_FetchAndAdd2 tbb::internal::__TBB_FetchAndAddGeneric<2,uint16_t>
#endif

#ifndef __TBB_FetchAndAdd4
#define __TBB_FetchAndAdd4 tbb::internal::__TBB_FetchAndAddGeneric<4,uint32_t>
#endif

#ifndef __TBB_FetchAndAdd8
#define __TBB_FetchAndAdd8 tbb::internal::__TBB_FetchAndAddGeneric<8,uint64_t>
#endif

#ifndef __TBB_FetchAndAddW
#define __TBB_FetchAndAddW tbb::internal::__TBB_FetchAndAddGeneric<sizeof(ptrdiff_t),ptrdiff_t>
#endif

#ifndef __TBB_FetchAndStore1
#define __TBB_FetchAndStore1 tbb::internal::__TBB_FetchAndStoreGeneric<1,uint8_t>
#endif

#ifndef __TBB_FetchAndStore2
#define __TBB_FetchAndStore2 tbb::internal::__TBB_FetchAndStoreGeneric<2,uint16_t>
#endif

#ifndef __TBB_FetchAndStore4
#define __TBB_FetchAndStore4 tbb::internal::__TBB_FetchAndStoreGeneric<4,uint32_t>
#endif

#ifndef __TBB_FetchAndStore8
#define __TBB_FetchAndStore8 tbb::internal::__TBB_FetchAndStoreGeneric<8,uint64_t>
#endif

#ifndef __TBB_FetchAndStoreW
#define __TBB_FetchAndStoreW tbb::internal::__TBB_FetchAndStoreGeneric<sizeof(ptrdiff_t),ptrdiff_t>
#endif

#if __TBB_DECL_FENCED_ATOMICS

#ifndef __TBB_CompareAndSwap1__TBB_full_fence
#define __TBB_CompareAndSwap1__TBB_full_fence __TBB_CompareAndSwap1
#endif 
#ifndef __TBB_CompareAndSwap1acquire
#define __TBB_CompareAndSwap1acquire __TBB_CompareAndSwap1__TBB_full_fence
#endif 
#ifndef __TBB_CompareAndSwap1release
#define __TBB_CompareAndSwap1release __TBB_CompareAndSwap1__TBB_full_fence
#endif 

#ifndef __TBB_CompareAndSwap2__TBB_full_fence
#define __TBB_CompareAndSwap2__TBB_full_fence __TBB_CompareAndSwap2
#endif
#ifndef __TBB_CompareAndSwap2acquire
#define __TBB_CompareAndSwap2acquire __TBB_CompareAndSwap2__TBB_full_fence
#endif
#ifndef __TBB_CompareAndSwap2release
#define __TBB_CompareAndSwap2release __TBB_CompareAndSwap2__TBB_full_fence
#endif

#ifndef __TBB_CompareAndSwap4__TBB_full_fence
#define __TBB_CompareAndSwap4__TBB_full_fence __TBB_CompareAndSwap4
#endif 
#ifndef __TBB_CompareAndSwap4acquire
#define __TBB_CompareAndSwap4acquire __TBB_CompareAndSwap4__TBB_full_fence
#endif 
#ifndef __TBB_CompareAndSwap4release
#define __TBB_CompareAndSwap4release __TBB_CompareAndSwap4__TBB_full_fence
#endif 

#ifndef __TBB_CompareAndSwap8__TBB_full_fence
#define __TBB_CompareAndSwap8__TBB_full_fence __TBB_CompareAndSwap8
#endif
#ifndef __TBB_CompareAndSwap8acquire
#define __TBB_CompareAndSwap8acquire __TBB_CompareAndSwap8__TBB_full_fence
#endif
#ifndef __TBB_CompareAndSwap8release
#define __TBB_CompareAndSwap8release __TBB_CompareAndSwap8__TBB_full_fence
#endif

#ifndef __TBB_FetchAndAdd1__TBB_full_fence
#define __TBB_FetchAndAdd1__TBB_full_fence __TBB_FetchAndAdd1
#endif
#ifndef __TBB_FetchAndAdd1acquire
#define __TBB_FetchAndAdd1acquire __TBB_FetchAndAdd1__TBB_full_fence
#endif
#ifndef __TBB_FetchAndAdd1release
#define __TBB_FetchAndAdd1release __TBB_FetchAndAdd1__TBB_full_fence
#endif

#ifndef __TBB_FetchAndAdd2__TBB_full_fence
#define __TBB_FetchAndAdd2__TBB_full_fence __TBB_FetchAndAdd2
#endif
#ifndef __TBB_FetchAndAdd2acquire
#define __TBB_FetchAndAdd2acquire __TBB_FetchAndAdd2__TBB_full_fence
#endif
#ifndef __TBB_FetchAndAdd2release
#define __TBB_FetchAndAdd2release __TBB_FetchAndAdd2__TBB_full_fence
#endif

#ifndef __TBB_FetchAndAdd4__TBB_full_fence
#define __TBB_FetchAndAdd4__TBB_full_fence __TBB_FetchAndAdd4
#endif
#ifndef __TBB_FetchAndAdd4acquire
#define __TBB_FetchAndAdd4acquire __TBB_FetchAndAdd4__TBB_full_fence
#endif
#ifndef __TBB_FetchAndAdd4release
#define __TBB_FetchAndAdd4release __TBB_FetchAndAdd4__TBB_full_fence
#endif

#ifndef __TBB_FetchAndAdd8__TBB_full_fence
#define __TBB_FetchAndAdd8__TBB_full_fence __TBB_FetchAndAdd8
#endif
#ifndef __TBB_FetchAndAdd8acquire
#define __TBB_FetchAndAdd8acquire __TBB_FetchAndAdd8__TBB_full_fence
#endif
#ifndef __TBB_FetchAndAdd8release
#define __TBB_FetchAndAdd8release __TBB_FetchAndAdd8__TBB_full_fence
#endif

#ifndef __TBB_FetchAndStore1__TBB_full_fence
#define __TBB_FetchAndStore1__TBB_full_fence __TBB_FetchAndStore1
#endif
#ifndef __TBB_FetchAndStore1acquire
#define __TBB_FetchAndStore1acquire __TBB_FetchAndStore1__TBB_full_fence
#endif
#ifndef __TBB_FetchAndStore1release
#define __TBB_FetchAndStore1release __TBB_FetchAndStore1__TBB_full_fence
#endif

#ifndef __TBB_FetchAndStore2__TBB_full_fence
#define __TBB_FetchAndStore2__TBB_full_fence __TBB_FetchAndStore2
#endif
#ifndef __TBB_FetchAndStore2acquire
#define __TBB_FetchAndStore2acquire __TBB_FetchAndStore2__TBB_full_fence
#endif
#ifndef __TBB_FetchAndStore2release
#define __TBB_FetchAndStore2release __TBB_FetchAndStore2__TBB_full_fence
#endif

#ifndef __TBB_FetchAndStore4__TBB_full_fence
#define __TBB_FetchAndStore4__TBB_full_fence __TBB_FetchAndStore4
#endif
#ifndef __TBB_FetchAndStore4acquire
#define __TBB_FetchAndStore4acquire __TBB_FetchAndStore4__TBB_full_fence
#endif
#ifndef __TBB_FetchAndStore4release
#define __TBB_FetchAndStore4release __TBB_FetchAndStore4__TBB_full_fence
#endif

#ifndef __TBB_FetchAndStore8__TBB_full_fence
#define __TBB_FetchAndStore8__TBB_full_fence __TBB_FetchAndStore8
#endif
#ifndef __TBB_FetchAndStore8acquire
#define __TBB_FetchAndStore8acquire __TBB_FetchAndStore8__TBB_full_fence
#endif
#ifndef __TBB_FetchAndStore8release
#define __TBB_FetchAndStore8release __TBB_FetchAndStore8__TBB_full_fence
#endif

#endif // __TBB_DECL_FENCED_ATOMICS

// Special atomic functions
#ifndef __TBB_FetchAndAddWrelease
#define __TBB_FetchAndAddWrelease __TBB_FetchAndAddW
#endif

#ifndef __TBB_FetchAndIncrementWacquire
#define __TBB_FetchAndIncrementWacquire(P) __TBB_FetchAndAddW(P,1)
#endif

#ifndef __TBB_FetchAndDecrementWrelease
#define __TBB_FetchAndDecrementWrelease(P) __TBB_FetchAndAddW(P,(-1))
#endif

#ifndef __TBB_Store8
inline void __TBB_Store8 (volatile void *ptr, int64_t value) {
    int64_t result = *(int64_t *)ptr;
    int64_t tmp;
    tmp = result; 
    result = __TBB_CompareAndSwap8(ptr,value,result);
    if ( tmp != result ) {
      tbb::internal::AtomicBackoff b;
      do {
        b.pause();
        tmp = result; 
        result = __TBB_CompareAndSwap8(ptr,value,result);
      } while ( tmp != result );
   }
}
#endif

#ifndef __TBB_Load8
inline int64_t __TBB_Load8 (const volatile void *ptr) {
    int64_t result = *(int64_t *)ptr;
    result = __TBB_CompareAndSwap8((volatile void *)ptr,result,result);
    return result;
}
#endif

#ifndef __TBB_Log2
inline intptr_t __TBB_Log2( uintptr_t x ) {
    long result = -1;
    for(; x; x>>=1 ) ++result;
    return result;
}
#endif

#ifndef __TBB_AtomicOR
inline void __TBB_AtomicOR( volatile void *operand, uintptr_t addend ) {
       uintptr_t result, tmp;
       do {
          tmp = *(uintptr_t *)operand;
          result = __TBB_CompareAndSwapW(operand, tmp|addend, tmp);
       } while (result != tmp);
    }
#endif

#ifndef __TBB_TryLockByte
inline bool __TBB_TryLockByte( volatile unsigned char &flag ) {
  volatile void *f = &flag;
  return ( __TBB_CompareAndSwap1(f,1,0) == 0);
}
#endif

#ifndef __TBB_LockByte
inline uintptr_t __TBB_LockByte( volatile unsigned char& flag ) {
    if ( !__TBB_TryLockByte(flag) ) {
        tbb::internal::AtomicBackoff b;
        do {
            b.pause();
        } while ( !__TBB_TryLockByte(flag) );
    }
    return 0;
}
#endif

#endif

