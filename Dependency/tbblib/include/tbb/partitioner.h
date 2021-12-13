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

#ifndef __TBB_partitioner_H
#define __TBB_partitioner_H

#include "task.h"

namespace tbb {

namespace internal {
size_t get_initial_auto_partitioner_divisor();
}

//! A simple partitioner 
/** Divides the range until the range is not divisible. 
    @ingroup algorithms */
class simple_partitioner {
public:
    simple_partitioner() {}
    simple_partitioner(simple_partitioner &partitioner, split) {}

    template <typename Range>
    inline bool should_execute_range(const Range &r, const task &t) {
        return !r.is_divisible();
    }
};

//! An auto partitioner 
/** The range is initial divided into several large chunks.
    Chunks are further subdivided into VICTIM_CHUNKS pieces if they are stolen and divisible.
    @ingroup algorithms */
class auto_partitioner {
    size_t num_chunks;
    static const size_t VICTIM_CHUNKS = 4;

public:
    auto_partitioner() : num_chunks(internal::get_initial_auto_partitioner_divisor())  {
    }

    auto_partitioner(auto_partitioner &partitioner, split) : num_chunks(partitioner.num_chunks/2) {
        partitioner.num_chunks /= 2;
    }

    template <typename Range>
    inline bool should_execute_range(const Range &r, const task &t) {
        if (t.is_stolen_task() && num_chunks < VICTIM_CHUNKS)
            num_chunks = VICTIM_CHUNKS;
        return !r.is_divisible() || num_chunks == 1;
    }
};

}

#endif
