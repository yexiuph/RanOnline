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

#ifndef __TBB_task_scheduler_init_H
#define __TBB_task_scheduler_init_H

#include "tbb_stddef.h"

namespace tbb {

//! @cond INTERNAL
namespace internal {
    //! Internal to library. Should not be used by clients.
    /** @ingroup task_scheduling */
    class scheduler;
} // namespace internal
//! @endcond

//! Class representing reference to tbb scheduler.
/** A thread must construct a task_scheduler_init, and keep it alive,
    during the time that it uses the services of class task.
    @ingroup task_scheduling */
class task_scheduler_init: internal::no_copy {
    /** NULL if not currently initialized. */
    internal::scheduler* my_scheduler;
public:
    //! Typedef for number of threads that is automatic.
    static const int automatic = -1;

    //! Argument to initialize() or constructor that causes initialization to be deferred.
    static const int deferred = -2;

    //! Ensure that scheduler exists for this thread
    /** A value of -1 lets tbb decide on the number 
        of threads, which is typically the number of hardware threads. 
        For production code, the default value of -1 should be used, 
        particularly if the client code is mixed with third party clients 
        that might also use tbb.

        The number_of_threads is ignored if any other task_scheduler_inits 
        currently exist.  A thread may construct multiple task_scheduler_inits.  
        Doing so does no harm because the underlying scheduler is reference counted. */
    void initialize( int number_of_threads=automatic );

    //! Inverse of method initialize.
    void terminate();

    //! Shorthand for default constructor followed by call to intialize(number_of_threads).
    task_scheduler_init( int number_of_threads=automatic ) : my_scheduler(NULL)  {
        initialize( number_of_threads );
    }
  
    //! Destroy scheduler for this thread if thread has no other live task_scheduler_inits.
    ~task_scheduler_init() {
        if( my_scheduler ) 
            terminate();
        internal::poison_pointer( my_scheduler );
    }
};

} // namespace tbb

#endif /* __TBB_task_scheduler_init_H */
