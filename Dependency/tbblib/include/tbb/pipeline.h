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

#ifndef __TBB_pipeline_H 
#define __TBB_pipeline_H 

#include "task.h"
#include "spin_mutex.h"
#include <cstddef>

namespace tbb {

class pipeline;
class filter;

//! @cond INTERNAL
namespace internal {

typedef unsigned long Token;
class stage_task;
class ordered_buffer;

} // namespace internal
//! @endcond

//! A stage in a pipeline.
/** @ingroup algorithms */
class filter {
private:
    //! Value used to mark "not in pipeline"
    static filter* not_in_pipeline() {return reinterpret_cast<filter*>(internal::intptr(-1));}
protected:
    filter( bool is_serial_ ) : 
        next_filter_in_pipeline(not_in_pipeline()),
        input_buffer(NULL),
        my_is_serial(is_serial_)
    {}
public:
    //! True if filter must receive stream in order.
    bool is_serial() const {return my_is_serial;}

    //! Operate on an item from the input stream, and return item for output stream.
    /** Returns NULL if filter is a sink. */
    virtual void* operator()( void* item ) = 0;

    //! Destroy filter.  
    /** If the filter was added to a pipeline, the pipeline must be destroyed first. */
    virtual ~filter();

private:
    //! Pointer to next filter in the pipeline.
    filter* next_filter_in_pipeline;

    //! Input buffer for filter that requires serial input; NULL otherwise. */
    internal::ordered_buffer* input_buffer;

    friend class internal::stage_task;
    friend class pipeline;

    //! Internal storage for is_serial()
    bool my_is_serial;
};

//! A processing pipeling that applies filters to items.
/** @ingroup algorithms */
class pipeline {
public:
    //! Construct empty pipeline.
    pipeline();

    //! Destroy pipeline.
    virtual ~pipeline();

    //! Add filter to end of pipeline.
    void add_filter( filter& filter );

    //! Run the pipeline to completion.
    void run( size_t max_number_of_live_tokens );

    //! Remove all filters from the pipeline
    void clear();

private:
    friend class internal::stage_task;

    //! Pointer to first filter in the pipeline.
    filter* filter_list;

    //! Pointer to location where address of next filter to be added should be stored.
    filter** filter_end;

    //! task who's reference count is used to determine when all stages are done.
    empty_task* end_counter;

    //! Mutex protecting token_counter and end_of_input.
    spin_mutex input_mutex;

    //! Number of tokens created so far.
    internal::Token token_counter;

    //! False until fetch_input returns NULL.
    bool end_of_input;
    
    //! Attempt to fetch a new input item and put it in the pipeline.
    /** "self" is used only for sake of providing the contextual "this" for task::allocate_child_of. */
    void inject_token( task& self );
};

} // tbb

#endif /* __TBB_pipeline_H */
