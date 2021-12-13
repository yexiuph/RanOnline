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

#ifndef __TBB_parallel_reduce_H
#define __TBB_parallel_reduce_H

#include "task.h"
#include "aligned_space.h"
#include "partitioner.h"
#include <new>

namespace tbb {

//! @cond INTERNAL
namespace internal {

    //! Task type use to combine the partial results of parallel_reduce.
    /** @ingroup algorithms */
    template<typename Body>
    class finish_reduce: public task {
        Body* const my_body;
        Body* right_zombie;
        aligned_space<Body,1> zombie_space;
        finish_reduce( Body* body ) : 
            my_body(body),
            right_zombie(NULL)
        {
        }
        task* execute() {
            if( Body* s = right_zombie ) {
                // Right child was stolen.
                __TBB_ASSERT( my_body!=s, NULL );
                my_body->join( *s );
                s->~Body();
            }
            return NULL;
        }       
        template<typename Range,typename Body_, typename Partitioner>
        friend class start_reduce;
    };

    //! Task type use to split the work of parallel_reduce.
    /** @ingroup algorithms */
    template<typename Range, typename Body, typename Partitioner=simple_partitioner>
    class start_reduce: public task {
        typedef finish_reduce<Body> finish_type;
        Body* my_body;
        Range my_range;
        Partitioner my_partitioner;
        /*override*/ task* execute();
        template<typename Body_>
        friend class finish_reduce;
    public:
        start_reduce( const Range& range, Body* body, const Partitioner &partitioner ) :
            my_body(body),
            my_range(range),
            my_partitioner(partitioner)
        {
        }
    };

    template<typename Range, typename Body, typename Partitioner>
    task* start_reduce<Range,Body,Partitioner>::execute()
    {
        Body* body = my_body;
        if( is_stolen_task() ) {
            finish_reduce<Body>* p = static_cast<finish_type*>(parent() );
            body = new( p->zombie_space.begin() ) Body(*body,split());
            my_body = p->right_zombie = body;
        }
        task* next_task = NULL;
        if ( my_partitioner.should_execute_range(my_range, *this))
            (*my_body)( my_range );
        else {
            finish_reduce<Body>& c = *new( allocate_continuation()) finish_type(body);
            recycle_as_child_of(c);
            c.set_ref_count(2);    
            start_reduce& b = *new( c.allocate_child() ) start_reduce(Range(my_range,split()), body, Partitioner(my_partitioner,split()));
            c.spawn(b);
            next_task = this;
        }
        return next_task;
    } 
} // namespace internal
//! @endcond

//! Parallel iteration with reduction
/** Type Body must have the following signatures: \n
        operator()( const Range& r ); \n
        Body( Body& b, split );  (be aware that b may have concurrent accesses) \n
        void join( Body& ); \n
        ~Body
       @ingroup algorithms
  */
template<typename Range, typename Body>
void parallel_reduce( const Range& range, Body& body ) {
    if( !range.empty() ) {
        typedef typename internal::start_reduce<Range,Body> start_type;
        start_type& a = *new(task::allocate_root()) start_type(range,&body,simple_partitioner());
        task::spawn_root_and_wait( a );
    }
}

//! Parallel iteration with reduction using a partitioner.
/** Type Body must have the following signatures: \n
        operator()( const Range& r ); \n
        Body( Body& b, split );  (be aware that b may have concurrent accesses) \n
        void join( Body& ); \n
        ~Body
    The partitioner p must define: \n
        p.should_execute_range(r,t)   True if r should be executed to completion without further splits. \n
        P p2(p,split())               Split the partitioner into p2 and p.      \n
    @ingroup algorithms
  */
template<typename Range, typename Body, typename Partitioner>
void parallel_reduce( const Range& range, Body& body, const Partitioner &partitioner ) {
    if( !range.empty() ) {
        typedef typename internal::start_reduce<Range,Body,Partitioner> start_type;
        start_type& a = *new(task::allocate_root()) start_type(range,&body,partitioner);
        task::spawn_root_and_wait( a );
    }
}


} // namespace tbb

#endif /* __TBB_parallel_reduce_H */

