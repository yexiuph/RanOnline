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

#ifndef __TBB_tbb_stddef_H
#define __TBB_tbb_stddef_H

// Define groups for Doxygen documentation
/**
 * @defgroup algorithms         Algorithms
 * @defgroup containers         Containers
 * @defgroup memory_allocation  Memory Allocation
 * @defgroup synchronization    Synchronization
 * @defgroup timing             Timing
 * @defgroup task_scheduling    Task Scheduling
 */

// Simple text that is displayed on the main page of Doxygen documentation.
/**
 * \mainpage Main Page
 *
 * Click the tabs above for information about the
 * <a href="./annotated.html">Classes</a> in the library or the
 * <a href="./modules.html">Modules</a> to which they belong.  The
 * <a href="./files.html">Files</a> tab shows which files contain the library
 * components.
 */

// Define preprocessor symbols used to determine architecture
#if _WIN32||_WIN64
#   if defined(_M_AMD64)
#       define __TBB_x86_64 1
#   elif defined(_M_IA64)
#       define __TBB_ipf 1
#   elif defined(_M_IX86)
#       define __TBB_x86_32 1
#   endif
#else /* Assume generic Unix */
#   if !__linux__ && !__APPLE__
#       define __TBB_generic_os 1
#   endif
#   if __x86_64__
#       define __TBB_x86_64 1
#   elif __ia64__
#       define __TBB_ipf 1
#   elif __i386__
#       define __TBB_x86_32 1
#   else
#       define __TBB_generic_arch 1
#   endif
#endif

#include <cstddef>              /* Need size_t and ptrdiff_t from here. */

#if _WIN32||_WIN64
#define __TBB_tbb_windef_H
#include "_tbb_windef.h"
#undef __TBB_tbb_windef_H
#endif /* _WIN32||_WIN64 */

namespace tbb {
    //! Type for an assertion handler
    typedef void(*assertion_handler_type)( const char* filename, int line, const char* expression, const char * comment );
}

#if TBB_DO_ASSERT

//! Assert that x is true.
/** If x is false, print assertion failure message.  
    If the comment argument is not NULL, it is printed as part of the failure message.  
    The comment argument has no other effect. */
#define __TBB_ASSERT(predicate,message) ((predicate)?((void)0):tbb::assertion_failure(__FILE__,__LINE__,#predicate,message))
#define __TBB_ASSERT_EX __TBB_ASSERT

namespace tbb {
    //! Set assertion handler and return previous value of it.
    assertion_handler_type set_assertion_handler( assertion_handler_type new_handler ); 

    //! Process an assertion failure.
    /** Normally called from __TBB_ASSERT macro.
        If assertion handler is null, print message for assertion failure and abort.
        Otherwise call the assertion handler. */
    void assertion_failure( const char* filename, int line, const char* expression, const char* comment );
} // namespace tbb

#else

//! No-op version of __TBB_ASSERT.
#define __TBB_ASSERT(predicate,comment) ((void)0)
//! "Extended" version is useful to suppress warnings if a variable is only used with an assert
#define __TBB_ASSERT_EX(predicate,comment) ((void)(1 && (predicate)))

#endif /* TBB_DO_ASSERT */

//! The namespace tbb contains all components of the library.
namespace tbb {

//! Dummy type that distinguishs splitting constructor from copy constructor.
/**
 * See description of parallel_for and parallel_reduce for example usages.
 * @ingroup algorithms
 */
class split {
};

/**
 * @cond INTERNAL
 * @brief Identifiers declared inside namespace internal should never be used directly by client code.
 */
namespace internal {

//! An unsigned integral type big enough to hold a pointer.
/** There's no guarantee by the C++ standard that a size_t is really big enough,
    but it happens to be for all platforms of interest. */
typedef size_t uintptr;

//! A signed integral type big enough to hold a pointer.
/** There's no guarantee by the C++ standard that a ptrdiff_t is really big enough,
    but it happens to be for all platforms of interest. */
typedef ptrdiff_t intptr;

#if TBB_DO_ASSERT
//! Set p to invalid pointer value.
template<typename T>
inline void poison_pointer( T* volatile & p ) {
    p = reinterpret_cast<T*>(-1);
}
#else
template<typename T>
inline void poison_pointer( T* ) {/*do nothing*/}
#endif /* TBB_DO_ASSERT */

//! Base class for types that should not be copied or assigned.
class no_copy {
    //! Deny copy construction
    no_copy( const no_copy& );

    // Deny assignment
    void operator=( const no_copy& );
public:
    //! Allow default construction
    no_copy() {}
};

} // internal
//! @endcond

} // tbb

#endif /* __TBB_tbb_stddef_H */
