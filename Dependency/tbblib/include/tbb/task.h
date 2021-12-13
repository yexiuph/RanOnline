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

#ifndef __TBB_task_H
#define __TBB_task_H

#include "tbb_stddef.h"

namespace tbb {

class task;
class task_list;

//! @cond INTERNAL
namespace internal {

    class scheduler {
    public:
        //! For internal use only
        virtual void spawn( task& first, task*& next ) = 0;

        //! For internal use only
        virtual void wait_for_all( task& parent, task* child ) = 0;

        //! For internal use only
        virtual void spawn_root_and_wait( task& first, task*& next ) = 0;

        //! Pure virtual destructor;
        //  Have to have it just to shut up overzealous compilation warnings
        virtual ~scheduler() = 0;
    };

    typedef intptr reference_count;

    class allocate_root_proxy {
    public:
        static task& allocate( size_t size );
        static void free( task& );
    };

    class allocate_continuation_proxy {
    public:
        task& allocate( size_t size ) const;
        void free( task& ) const;
    };

    class allocate_child_proxy {
    public:
        task& allocate( size_t size ) const;
        void free( task& ) const;
    };

    class allocate_additional_child_of_proxy {
        task& self;
        task& parent;
    public:
        allocate_additional_child_of_proxy( task& self_, task& parent_ ) : self(self_), parent(parent_) {}
        task& allocate( size_t size ) const;
        void free( task& ) const;
    };

    //! Memory prefix to a task object.
    /** This class is internal to the library.
        Do not reference it directly, except within the library itself.
        @ingroup task_scheduling */
    class task_prefix {
    private:
        friend class tbb::task;
        friend class tbb::task_list;
        friend class internal::scheduler;
        friend class internal::allocate_root_proxy;
        friend class internal::allocate_child_proxy;
        friend class internal::allocate_continuation_proxy;
        friend class internal::allocate_additional_child_of_proxy;


        //! The scheduler that allocated the task, or NULL if task is big.
        /** Small tasks are pooled by the scheduler that allocated the task.
            If a scheduler needs to free a small task allocated by another scheduler,
            it returns the task to that other scheduler.  This policy avoids
            memory space blowup issues for memory allocators that allocate from 
            thread-specific pools. */
        scheduler* origin;

        //! scheduler that owns the task.
        scheduler* owner;

        //! task whose reference count includes me.
        /** In the "blocking style" of programming, this field points to the parent task.
            In the "continuation-passing style" of programming, this field points to the
            continuation of the parent. */
        tbb::task* parent;

        //! Reference count used for synchronization.
        /** In the "continuation-passing style" of programming, this field is
            the difference of the number of allocated children minus the
            number of children that have completed.
            In the "blocking style" of programming, this field is one more than the difference. */
        reference_count ref_count;

        //! Scheduling depth
        int depth;

        //! A task::state_type, stored as a byte for compactness.
        unsigned char state;

        //! Reserved for future use
        unsigned char reserved2;

#if TBB_DO_ASSERT
        //! Used for internal debugging.
        /** Zero if production version of library is linked. */
        unsigned char debug_state;
#else
        //! Reserved for internal use
        unsigned char reserved0;
#endif /* TBB_DO_ASSERT */

        //! Reserved for future use
        unsigned char reserved1;

        //! "next" field for list of task
        /** Assembly coded routine Gettask presumes this field is the last field
            in the prefix. */
        tbb::task* next;

        //! task corresponding to this task_prefix.
        tbb::task& task() {return *reinterpret_cast<tbb::task*>(this+1);}
    };

} // namespace internal
//! @endcond

//! Base class for user-defined tasks.
/** @ingroup task_scheduling */
class task: internal::no_copy {
    //! Set reference count
    void internal_set_ref_count( int count );

protected:
    //! Default constructor.
    task() {}

public:
    //! Destructor.
    virtual ~task() {}

    //! Should be overriden by derived classes.
    virtual task* execute() = 0;

    //! Enumeration of task states that the scheduler considers.
    enum state_type {
        //! task is running, and will be destroyed after method execute() completes.
        executing,
        //! task to be rescheduled.
        reexecute,
        //! task is in ready pool, or is going to be put there, or was just taken off.
        ready,
        //! task object is freshly allocated or recycled.
        allocated,
        //! task object is on free list, or is going to be put there, or was just taken off.
        freed,
        //! task to be recycled as continuation
        recycle
    };

    //------------------------------------------------------------------------
    // Allocating tasks
    //------------------------------------------------------------------------

    //! Returns proxy for overloaded new that allocates a root task.
    static internal::allocate_root_proxy allocate_root() {
        return internal::allocate_root_proxy();
    }

    //! Returns proxy for overloaded new that allocates a continuation task of *this.
    /** The continuation's parent becomes the parent of *this. */
    internal::allocate_continuation_proxy& allocate_continuation() {
        return *reinterpret_cast<internal::allocate_continuation_proxy*>(this);
    }

    //! Returns proxy for overloaded new that allocates a child task of *this.
    internal::allocate_child_proxy& allocate_child() {
        return *reinterpret_cast<internal::allocate_child_proxy*>(this);
    }

    //! Like allocate_child, except that task's parent becomes "t", not this.
    /** Typically used in conjunction with schedule_to_reexecute to implement while loops.
        Atomically increments the reference count of t.parent() */
    internal::allocate_additional_child_of_proxy allocate_additional_child_of( task& t ) {
        return internal::allocate_additional_child_of_proxy(*this,t);
    }

    //! Destroy a task.
    /** Usually, calling this method is unnecessary, because a task is
        implicitly deleted after its execute() method runs.  However,
        sometimes a task needs to be explicitly deallocated, such as
        when a root task is used as the parent in spawn_and_wait_for_all. */
    void destroy( task& victim );

    //------------------------------------------------------------------------
    // Recycling of tasks
    //------------------------------------------------------------------------

    //! Change this to be a continuation of its former self.
    /** The caller must guarantee that the task's refcount does not become zero until
        after the method execute() returns.  Typically, this is done by having
        method execute() return a pointer to a child of the task.  If the guarantee
        cannot be made, use method recycle_as_safe_continuation instead. 
       
        Because of the hazard, this method may be deprecated in the future. */
    void recycle_as_continuation() {
        __TBB_ASSERT( prefix().state==executing, "execute not running?" );
        prefix().state = allocated;
    }

    //! Recommended to use, safe variant of recycle_as_continuation
    /** For safety, it requires additional increment of ref_count. */
    void recycle_as_safe_continuation() {
        __TBB_ASSERT( prefix().state==executing, "execute not running?" );
        prefix().state = recycle;
    }

    //! Change this to be a child of new_parent.
    void recycle_as_child_of( task& new_parent ) {
        internal::task_prefix& p = prefix();
        __TBB_ASSERT( prefix().state==executing||prefix().state==allocated, "execute not running, or already recycled" );
        __TBB_ASSERT( prefix().ref_count==0, "no child tasks allowed when recycled as a child" );
        __TBB_ASSERT( p.parent==NULL, "parent must be null" );
        __TBB_ASSERT( new_parent.prefix().state<=recycle, "corrupt parent's state" );
        __TBB_ASSERT( new_parent.prefix().state!=freed, "parent already freed" );
        p.state = allocated;
        p.parent = &new_parent;
        p.depth = new_parent.prefix().depth+1;
    }

    //! Schedule this for reexecution after current execute() returns.
    /** Requires that this.execute() be running. */
    void recycle_to_reexecute() {
        __TBB_ASSERT( prefix().state==executing, "execute not running, or already recycled" );
        __TBB_ASSERT( prefix().ref_count==0, "no child tasks allowed when recycled for reexecution" );
        prefix().state = reexecute;
    }

    //! A scheduling depth.
    /** Guaranteed to be a signed integral type. */
    typedef internal::intptr depth_type;

    //! Scheduling depth
    depth_type depth() const {return prefix().depth;}

    //! Set scheduling depth to given value.
    /** The depth must be non-negative */
    void set_depth( depth_type new_depth ) {
        __TBB_ASSERT( state()!=ready, "cannot change depth of ready task" );
        __TBB_ASSERT( new_depth>=0, "depth cannot be negative" );
        __TBB_ASSERT( new_depth==int(new_depth), "integer overflow error");
        prefix().depth = int(new_depth);
    }

    //! Change scheduling depth by given amount.
    /** The resulting depth must be non-negative. */
    void add_to_depth( int delta ) {
        __TBB_ASSERT( state()!=ready, "cannot change depth of ready task" );
        __TBB_ASSERT( prefix().depth>=-delta, "depth cannot be negative" );
        prefix().depth+=delta;
    }

    //------------------------------------------------------------------------
    // Spawning and blocking
    //------------------------------------------------------------------------

    //! Set reference count
    void set_ref_count( int count ) {
#if TBB_DO_ASSERT
        internal_set_ref_count(count);
#else
        prefix().ref_count = count;
#endif /* TBB_DO_ASSERT */
    }

    //! Schedule task for execution when a worker becomes available.
    /** After all children spawned so far finish their method task::execute,
        their parent's method task::execute may start running.  Therefore, it
        is important to ensure that at least one child has not completed until
        the parent is ready to run. */
    void spawn( task& child ) {
        __TBB_ASSERT( is_owned_by_current_thread(), "'this' not owned by current thread" );
        prefix().owner->spawn( child, child.prefix().next );
    }

    //! Spawn multiple tasks and clear list.
    /** All of the tasks must be at the same depth. */
    void spawn( task_list& list );

    //! Similar to spawn followed by wait_for_all, but more efficient.
    void spawn_and_wait_for_all( task& child ) {
        __TBB_ASSERT( is_owned_by_current_thread(), "'this' not owned by current thread" );
        prefix().owner->wait_for_all( *this, &child );
    }

    //! Similar to spawn followed by wait_for_all, but more efficient.
    void spawn_and_wait_for_all( task_list& list );

    //! Spawn task allocated by allocate_root, wait for it to complete, and deallocate it.
    /** The thread that calls spawn_root_and_wait must be the same thread
        that allocated the task. */
    static void spawn_root_and_wait( task& root ) {
        __TBB_ASSERT( root.is_owned_by_current_thread(), "root not owned by current thread" );
        root.prefix().owner->spawn_root_and_wait( root, root.prefix().next );
    }

    //! Spawn root tasks on list and wait for all of them to finish.
    /** If there are more tasks than worker threads, the tasks are spawned in
        order of front to back. */
    static void spawn_root_and_wait( task_list& root_list );

    //! Wait for reference count to become one, and set reference count to zero.
    /** Works on tasks while waiting. */
    void wait_for_all() {
        __TBB_ASSERT( is_owned_by_current_thread(), "'this' not owned by current thread" );
        prefix().owner->wait_for_all( *this, NULL );
    }

    //! The task() current being run by this thread.
    static task& self();

    //! task on whose behalf this task is working, or NULL if this is a root.
    task* parent() const {return prefix().parent;}

    //! True if task is owned by different thread than thread that owns its parent.
    bool is_stolen_task() const {
        internal::task_prefix& p = prefix();
        internal::task_prefix& q = parent()->prefix();
        return p.owner!=q.owner;
    }


    //------------------------------------------------------------------------
    // Debugging
    //------------------------------------------------------------------------

    //! Current execution state
    state_type state() const {return state_type(prefix().state);}

    //! The internal reference count.
    int ref_count() const {
#if TBB_DO_ASSERT
        internal::reference_count ref_count = prefix().ref_count;
        __TBB_ASSERT( ref_count==int(ref_count), "integer overflow error");
#endif
        return int(prefix().ref_count);
    }

    //! True if this task is owned by the calling thread; false otherwise.
    bool is_owned_by_current_thread() const;

private:
    friend class task_list;
    friend class internal::scheduler;
    friend class internal::allocate_root_proxy;
    friend class internal::allocate_continuation_proxy;
    friend class internal::allocate_child_proxy;
    friend class internal::allocate_additional_child_of_proxy;

    //! Get reference to corresponding task_prefix.
    internal::task_prefix& prefix() const {
        return reinterpret_cast<internal::task_prefix*>(const_cast<task*>(this))[-1];
    }
};

//! task that does nothing.  Useful for synchronization.
/** @ingroup task_scheduling */
class empty_task: public task {
    /*override*/ task* execute() {
        return NULL;
    }
};

//! A list of children.
/** Used for method task::spawn_children
    @ingroup task_scheduling */
class task_list: internal::no_copy {
private:
    task* first;
    task** next_ptr;
    friend class task;
public:
    //! Construct empty list
    task_list() : first(NULL), next_ptr(&first) {}

    //! Destroys the list, but does not destroy the task objects.
    ~task_list() {}

    //! True if list if empty; false otherwise.
    bool empty() const {return !first;}

    //! Push task onto back of list.
    void push_back( task& task ) {
        task.prefix().next = NULL;
        *next_ptr = &task;
        next_ptr = &task.prefix().next;
    }

    //! Pop the front task from the list.
    task& pop_front() {
        __TBB_ASSERT( !empty(), "attempt to pop item from empty task_list" );
        task* result = first;
        first = result->prefix().next;
        if( !first ) next_ptr = &first;
        return *result;
    }
    //! Clear the list
    void clear() {
        first=NULL;
        next_ptr=&first;
    }
};

inline void task::spawn( task_list& list ) {
    __TBB_ASSERT( is_owned_by_current_thread(), "'this' not owned by current thread" );
    if( task* t = list.first ) {
        prefix().owner->spawn( *t, *list.next_ptr );
        list.clear();
    }
}

inline void task::spawn_root_and_wait( task_list& root_list ) {
    if( task* t = root_list.first ) {
        __TBB_ASSERT( t->is_owned_by_current_thread(), "'this' not owned by current thread" );
        t->prefix().owner->spawn_root_and_wait( *t, *root_list.next_ptr );
        root_list.clear();
    }
}


} // namespace tbb

inline void *operator new( size_t bytes, const tbb::internal::allocate_root_proxy& p ) {
    return &p.allocate(bytes);
}

inline void operator delete( void* task, const tbb::internal::allocate_root_proxy& p ) {
    p.free( *static_cast<tbb::task*>(task) );
}

inline void *operator new( size_t bytes, const tbb::internal::allocate_continuation_proxy& p ) {
    return &p.allocate(bytes);
}

inline void operator delete( void* task, const tbb::internal::allocate_continuation_proxy& p ) {
    p.free( *static_cast<tbb::task*>(task) );
}

inline void *operator new( size_t bytes, const tbb::internal::allocate_child_proxy& p ) {
    return &p.allocate(bytes);
}

inline void operator delete( void* task, const tbb::internal::allocate_child_proxy& p ) {
    p.free( *static_cast<tbb::task*>(task) );
}

inline void *operator new( size_t bytes, const tbb::internal::allocate_additional_child_of_proxy& p ) {
    return &p.allocate(bytes);
}

inline void operator delete( void* task, const tbb::internal::allocate_additional_child_of_proxy& p ) {
    p.free( *static_cast<tbb::task*>(task) );
}

#endif /* __TBB_task_H */
