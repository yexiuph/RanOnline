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

#ifndef __TBB_queuing_rw_mutex_H
#define __TBB_queuing_rw_mutex_H

#include <cstring>
#include "atomic.h"

namespace tbb {

//! Reader-writer lock with local-only spinning.
/** Adapted from Krieger, Stumm, et al. pseudocode at
    http://www.eecg.toronto.edu/parallel/pubs_abs.html#Krieger_etal_ICPP93
    @ingroup synchronization */
class queuing_rw_mutex {
public:
    //! Construct unacquired mutex.
    queuing_rw_mutex() {
        q_tail = NULL;
    };

    //! Destructor asserts if the mutex is acquired, i.e. q_tail is non-NULL
    ~queuing_rw_mutex() {
#if TBB_DO_ASSERT
        __TBB_ASSERT( !q_tail, "destruction of an acquired mutex");
#endif
    };

    class scoped_lock;
    friend class scoped_lock;

    //! The scoped locking pattern
    /** It helps to avoid the common problem of forgetting to release lock.
        It also nicely provides the "node" for queuing locks. */
    class scoped_lock : private internal::no_copy {
        /** Request type constants for scoped_lock::state */
        enum state_t {
            STATE_NONE = 0,
            STATE_WRITER = 1,
            STATE_READER = 1<<1,
            STATE_READER_UNBLOCKNEXT = 1<<2,
            STATE_COMBINED_WAITINGREADER = STATE_READER | STATE_READER_UNBLOCKNEXT,
            STATE_ACTIVEREADER = 1<<3,
            STATE_COMBINED_READER = STATE_COMBINED_WAITINGREADER | STATE_ACTIVEREADER,
            STATE_UPGRADE_REQUESTED = 1<<4,
            STATE_UPGRADE_WAITING = 1<<5,
            STATE_UPGRADE_LOSER = 1<<6,
            STATE_COMBINED_UPGRADING = STATE_UPGRADE_WAITING | STATE_UPGRADE_LOSER
#if TBB_DO_ASSERT
           ,STATE_INVALID
#endif /* TBB_DO_ASSERT */
        };

        //! Initialize fields
        void initialize() {
            mutex = NULL;
#if TBB_DO_ASSERT
            state = STATE_INVALID;
            internal::poison_pointer(next);
            internal::poison_pointer(prev);
#endif /* TBB_DO_ASSERT */
        }
    public:
        //! Construct lock that has not acquired a mutex.
        /** Equivalent to zero-initialization of *this. */
        scoped_lock() {initialize();}

        //! Acquire lock on given mutex.
        /** Upon entry, *this should not be in the "have acquired a mutex" state. */
        scoped_lock( queuing_rw_mutex& m, bool write=true ) {
            initialize();
            acquire(m,write);
        }

        //! Release lock (if lock is held).
        ~scoped_lock() {
            if( mutex ) release();
        }

        //! Acquire lock on given mutex.
        void acquire( queuing_rw_mutex& m, bool write=true );

        //! Try acquire lock on given mutex.
        bool try_acquire( queuing_rw_mutex& m, bool write=true );

        //! Release lock.
        void release();

        //! Upgrade reader to become a writer.
        /** Returns true if the upgrade happened without re-acquiring the lock and false if opposite */
        bool upgrade_to_writer();

        //! Downgrade writer to become a reader.
        bool downgrade_to_reader();

    private:
        //! The pointer to the current mutex to work
        queuing_rw_mutex* mutex;

        //! The pointer to the previous and next competitors for a mutex
        scoped_lock * prev, * next;

        //! State of the request: reader, writer, active reader, other service states
        atomic<unsigned char> state;

        //! The local spin-wait variable
        /** Corresponds to "spin" in the pseudocode but inverted for the sake of zero-initialization */
        unsigned char going;

        //! A tiny internal lock
        unsigned char internal_lock;

        //! Acquire the internal lock
        void acquire_internal_lock();

        //! Try to acquire the internal lock
        /** Returns true if lock was successfully acquired. */
        bool try_acquire_internal_lock();

        //! Release the internal lock
        void release_internal_lock();

        //! Wait for internal lock to be released
        void wait_for_release_of_internal_lock();

        //! A helper function
        void unblock_or_wait_on_internal_lock( uintptr_t );
    };

private:
    //! The last competitor requesting the lock
    atomic<scoped_lock*> q_tail;

};

} // namespace tbb

#endif /* __TBB_queuing_rw_mutex_H */
