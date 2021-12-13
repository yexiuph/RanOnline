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

#ifndef __TBB_parallel_for_H
#define __TBB_parallel_for_H

#include "task.h"
#include "partitioner.h"
#include <new>

namespace tbb {

//! @cond INTERNAL
namespace internal {

    //! Task type used in parallel_for
    /** @ingroup algorithms */
    template<typename Range, typename Body, typename Partitioner=simple_partitioner>
    class start_for: public task {
        Range my_range;
        const Body my_body;
        Partitioner my_partitioner;
        /*override*/ task* execute();
    public:
        start_for( const Range& range, const Body& body, const Partitioner& partitioner ) :
            my_range(range),    
            my_body(body),
            my_partitioner(partitioner)
        {
        }
    };

    template<typename Range, typename Body, typename Partitioner>
    task* start_for<Range,Body,Partitioner>::execute()
    {
        if( my_partitioner.should_execute_range(my_range, *this) ) {
            my_body( my_range );
            return NULL;
        } else {
            empty_task& c = *new( allocate_continuation() ) empty_task;
            recycle_as_child_of(c);
            c.set_ref_count(2);
            start_for& b = *new( c.allocate_child() ) start_for(Range(my_range,split()),my_body,Partitioner(my_partitioner,split()));
            c.spawn(b);
            return this;
        }
    } 

} // namespace internal
//! @endcond

//! Parallel iteration over range.
/** The body b must allow:                                      \n
        b(r)                    Apply function to range r.      \n
    r must define:                                              \n
        r.is_divisible()        True if range should be divided \n
        r.empty()               True if range is empty          \n
        R r2(r,split())         Split range into r2 and r.      \n
    @ingroup algorithms */ 
template<typename Range, typename Body>
void parallel_for( const Range& range, const Body& body ) {
    if( !range.empty() ) {
        typedef typename internal::start_for<Range,Body> start_type;
        start_type& a = *new(task::allocate_root()) start_type(range,body,simple_partitioner());
        task::spawn_root_and_wait(a);
    }
}

//! Parallel iteration over range using a partitioner.
/** The body b must allow:                                      \n
        b(r)                    Apply function to range r.      \n
    r must define:                                              \n
        r.is_divisible()        True if range can be divided \n
        r.empty()               True if range is empty          \n
        R r2(r,split())         Split range into r2 and r.      \n
    The partitioner p must define: \n
        p.should_execute_range(r,t)   True if r should be executed to completion without further splits. \n  
        P p2(p,split())               Split the partitioner into p2 and p.      \n
    @ingroup algorithms */
template<typename Range, typename Body, typename Partitioner>
void parallel_for( const Range& range, const Body& body, const Partitioner& partitioner ) {
    if( !range.empty() ) {
        typedef typename internal::start_for<Range,Body,Partitioner> start_type;
        start_type& a = *new(task::allocate_root()) start_type(range,body,partitioner);
        task::spawn_root_and_wait(a);
    }
}


} // namespace tbb

#endif /* __TBB_parallel_for_H */

