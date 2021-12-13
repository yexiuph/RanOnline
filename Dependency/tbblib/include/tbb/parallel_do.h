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

#ifndef __TBB_parallel_do_H
#define __TBB_parallel_do_H

#include "task.h"
#include <new>

namespace tbb {

template<typename Stream, typename Body> 
void parallel_do( Stream& stream, const Body& body );

//! @cond INTERNAL
namespace internal {
    template<typename Body> class parallel_do_feeder_impl;
    template<typename Body> class do_group_task;
} // namespace internal
//! @endcond

//! Class the user supplied algorithm body uses to add new tasks
/**
    \param Item  Work item type
**/
template<typename Item>
class parallel_do_feeder: internal::no_copy
{
    parallel_do_feeder() {}
    virtual ~parallel_do_feeder () {}
    virtual void internal_add( const Item& item ) = 0;
    template<typename Body>
    friend class internal::parallel_do_feeder_impl;
public:
    //! Add a work item to a running parallel_do.
    void add( const Item& item ) {internal_add(item);}
};


//! @cond INTERNAL
namespace internal {
    //! For internal use only.
    /** Selects one of the two possible forms of function call member operator
        @ingroup algorithms */

    template<class T, bool> struct parallel_do_caller;

    template<class T> struct parallel_do_caller<T, false>
    {
        template<typename A1, typename A2 >
        static void call( const T& obj, A1& arg1, A2& ) {
            obj(arg1);
        }
    };
    template<class T> struct parallel_do_caller<T, true>
    {
        template<typename A1, typename A2 >
        static void call( const T& obj, A1& arg1, A2& arg2 ) {
            obj(arg1, arg2);
        }
    };

    template<class T, typename F1, typename F2>
    struct parallel_do_operator_selector
    {
        template<typename U>
        static  void* discr ( U );
        static  bool  discr ( F2 );

        template<typename A1, typename A2 >
        static void call( const T& obj, A1& arg1, A2& arg2 ) {
            parallel_do_caller<T, sizeof( discr(&T::operator()) ) == sizeof(bool) >::call(obj, arg1, arg2);
        }
    };

    //! For internal use only.
    /** Executes one iteration of a do.
        @ingroup algorithms */
    template<typename Body>
    class do_iteration_task: public task
    {
        typedef typename Body::argument_type    item_type;
        typedef parallel_do_feeder_impl<Body>   feeder_type;
        typedef parallel_do_feeder<item_type>   users_feeder_type;

        item_type my_value;
        parallel_do_feeder_impl<Body>& my_feeder;

        do_iteration_task( const item_type& value, feeder_type& feeder ) : 
            my_value(value), my_feeder(feeder)
        {
        }

        /*override*/ 
        task* execute()
        {
            typedef  void (Body::*pfn_one_arg_t)(item_type) const;
            typedef  void (Body::*pfn_two_args_t)(item_type, users_feeder_type&) const;

            parallel_do_operator_selector<Body, pfn_one_arg_t, pfn_two_args_t>::call(*my_feeder.my_body, my_value, my_feeder);
            return NULL;
        }
        template<typename Body_> friend class parallel_do_feeder_impl;
        template<typename Body_> friend class do_group_task;
    }; // class do_iteration_task

    //! For internal use only.
    /** Implements new task adding procedure.
        @ingroup algorithms **/
    template<class Body>
    class parallel_do_feeder_impl : public parallel_do_feeder<typename Body::argument_type>
    {
        typedef typename Body::argument_type item_type;

        /*override*/ 
        void internal_add( const item_type& item )
        {
            typedef do_iteration_task<Body> iteration_type;

            iteration_type& t = *new (task::self().allocate_additional_child_of(*my_barrier)) iteration_type(item, *this);

            task::self().spawn( t );
        }
    public:
        const Body* my_body;
        empty_task* my_barrier;
    }; // class parallel_do_feeder_impl


    //! For internal use only
    /** Unpacks a block of iterations.
        @ingroup algorithms */
    template<typename Body>
    class do_group_task: public task
    {
        static const size_t max_arg_size = 4;         

        parallel_do_feeder_impl<Body>& my_feeder;
        size_t size;
        typename Body::argument_type my_arg[max_arg_size];

        do_group_task( parallel_do_feeder_impl<Body>& feeder ) 
            : my_feeder(feeder), size(0)
        {
        }

        /*override*/ task* execute()
        {
            typedef do_iteration_task<Body> iteration_type;
            __TBB_ASSERT( size>0, NULL );
            task_list list;
            task* t; 
            size_t k=0; 
            for(;;) {
                t = new( allocate_child() ) iteration_type(my_arg[k], my_feeder);
                if( ++k==size ) break;
                list.push_back(*t);
            }
            set_ref_count(int(k+1));
            spawn(list);
            spawn_and_wait_for_all(*t);
            return NULL;
        }

        template<typename Stream, typename Body_> friend class do_task;
    }; // class do_group_task
    

    //! For internal use only.
    /** Gets block of iterations from a stream and packages them into a do_group_task.
        @ingroup algorithms */
    template<typename Stream, typename Body>
    class do_task: public task
    {
    public:
        do_task( Stream& stream, parallel_do_feeder_impl<Body>& feeder ) : 
            my_stream(stream), 
            my_feeder(feeder)
        {
        }
    
    private:
        Stream& my_stream;
        parallel_do_feeder_impl<Body>& my_feeder;

        /*override*/ task* execute()
        {
            typedef do_group_task<Body> block_type;

            block_type& t = *new( allocate_additional_child_of(*my_feeder.my_barrier) ) block_type(my_feeder);

            size_t k=0; 
            while( my_stream.pop_if_present(t.my_arg[k]) ) {
                if( ++k==block_type::max_arg_size ) {
                    // There might be more iterations.
                    recycle_to_reexecute();
                    break;
                }
            }
            if( k==0 ) {
                destroy(t);
                return NULL;
            } else {
                t.size = k;
                return &t;
            }
        }
    }; // class do_task

} // namespace internal
//! @endcond



//! Parallel iteration over a stream, with optional addition of more work.
/** The Body b has the requirement: \n
        "b(v)"                      \n
        "b.argument_type"           \n
    where v is an argument_type
    @ingroup algorithms */
template<typename Stream, typename Body> 
void parallel_do( Stream& stream, const Body& body )
{
    using namespace internal;

    parallel_do_feeder_impl<Body>  feeder;
    feeder.my_body = &body;
    feeder.my_barrier = new( task::allocate_root() ) empty_task();
    __TBB_ASSERT(feeder.my_barrier, "root task allocation failed");

    do_task<Stream,Body>& t = *new( feeder.my_barrier->allocate_child() ) do_task<Stream,Body>( stream, feeder );

    feeder.my_barrier->set_ref_count(2);
    feeder.my_barrier->spawn_and_wait_for_all(t);

    feeder.my_barrier->destroy(*feeder.my_barrier);
} // parallel_do


} // namespace 

#endif /* __TBB_parallel_do_H */
