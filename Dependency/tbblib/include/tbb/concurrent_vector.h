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

#ifndef __TBB_concurrent_vector_H
#define __TBB_concurrent_vector_H

#include "tbb_stddef.h"
#include <algorithm>
#include <iterator>
#include <memory>
#include <new>
#include "atomic.h"
#include "cache_aligned_allocator.h"
#include "blocked_range.h"

#include "tbb_machine.h"


namespace tbb {

template<typename T, class A = cache_aligned_allocator<T> >
class concurrent_vector;

//! @cond INTERNAL
namespace internal {

    //! Base class of concurrent vector implementation.
    /** @ingroup containers */
    class concurrent_vector_base {
    protected:

        // Basic types declarations
        typedef size_t segment_index_t;
        typedef size_t size_type;

        // Using enumerations due to Mac linking problems of static const variables
        enum {
            // Size constants
            default_initial_segments = 1, // 2 initial items
            //! Number of slots for segment's pointers inside the class
            pointers_per_short_table = 4, // to fit into 8 words of entire structure
            pointers_per_long_table = sizeof(segment_index_t) * 8, // one segment per bit
        };

        // Segment pointer. Can be zero-initialized
        struct segment_t {
            /** Declared volatile because in weak memory model, must have ld.acq/st.rel  */
            void* volatile array;
#if TBB_DO_ASSERT
            ~segment_t() {
                __TBB_ASSERT( !array, "should have been set to NULL by clear" );
            }
#endif /* TBB_DO_ASSERT */
        };
 
        // Data fields

        //! allocator function pointer
        void* (*vector_allocator_ptr)(concurrent_vector_base &, size_t);

        //! count of segments in the first block
        atomic<size_type> my_first_block;

        //! Requested size of vector
        atomic<size_type> my_early_size;

        //! Pointer to the segments table
        atomic<segment_t*> my_segment;

        //! embedded storage of segment pointers
        segment_t my_storage[pointers_per_short_table];

        // Methods

        concurrent_vector_base() {
            my_early_size = 0;
#if !TBB_VECTOR_NO_SOLID
            my_first_block = 0; // here is not default_initial_segments
#else
            my_first_block = default_initial_segments;
#endif
            for( segment_index_t i = 0; i < pointers_per_short_table; i++)
                my_storage[i].array = NULL;
            my_segment = my_storage;
        }
        ~concurrent_vector_base();

#ifdef TBB_VECTOR_SECTIONS
        static const segment_index_t lg_segments_per_section = 5; //can't be bigger then 4.. at least on ia32
        static const segment_index_t segments_per_section = segment_index_t(1)<<lg_segments_per_section;
        static const segment_index_t items_per_section = segment_index_t(1)<<segments_per_section;
#endif//TBB_VECTOR_SECTIONS

        static segment_index_t segment_index_of( size_type index ) {
#ifdef TBB_VECTOR_SECTIONS
            const segment_index_t section = index >> segments_per_section;
            index &= items_per_section-1;
            const segment_index_t segment_base = section * segments_per_section;
            return segment_index_t( __TBB_Log2( index|1 ) + segment_base);
#else//TBB_VECTOR_SECTIONS
            return segment_index_t( __TBB_Log2( index|1 ) );
#endif//TBB_VECTOR_SECTIONS
        }

        static segment_index_t segment_base( segment_index_t k ) {
#ifdef TBB_VECTOR_SECTIONS
            const segment_index_t section = k / segments_per_section;
            k %= segments_per_section;
            const segment_index_t items_base = section << segments_per_section;
            return (segment_index_t(1)<<k & ~segment_index_t(1)) + items_base;
#else//TBB_VECTOR_SECTIONS
            return (segment_index_t(1)<<k & ~segment_index_t(1));
#endif//TBB_VECTOR_SECTIONS
        }

        static inline segment_index_t segment_base_index_of( segment_index_t &index ) {
#ifdef TBB_VECTOR_SECTIONS
            const segment_index_t section = index >> segments_per_section;
            index &= items_per_section-1;
            const segment_index_t segment_base = section * segments_per_section; // believe that compiler can optimize it
            segment_index_t k = segment_index_t( __TBB_Log2( index|1 ) );
            index -= segment_index_t(1) << k & ~segment_index_t(1);
            return k + segment_base;
#else//TBB_VECTOR_SECTIONS
            segment_index_t k = segment_index_of( index );
            index -= segment_base(k);
            return k;
#endif//TBB_VECTOR_SECTIONS
        }

        static size_type segment_size( segment_index_t k ) {
#ifdef TBB_VECTOR_SECTIONS
            k %= segments_per_section; // believe that compiler can optimize it
            if(!k) return segment_index_t(2);
#endif//TBB_VECTOR_SECTIONS
            return segment_index_t(1)<<k; // fake value for k==0
        }

        //! An operation on an n-element array starting at begin.
        typedef void(*internal_array_op1)(void* begin, size_type n );

        //! An operation on n-element destination array and n-element source array.
        typedef void(*internal_array_op2)(void* dst, const void* src, size_type n );

        void internal_reserve( size_type n, size_type element_size, size_type max_size );
        size_type internal_capacity() const;
        void internal_grow_to_at_least( size_type new_size, size_type element_size, internal_array_op2 init, const void *src );
        void internal_grow( size_type start, size_type finish, size_type element_size, internal_array_op2 init, const void *src );
        size_type internal_grow_by( size_type delta, size_type element_size, internal_array_op2 init, const void *src );
        void* internal_push_back( size_type element_size, size_type& index );
        segment_index_t internal_clear( internal_array_op1 destroy );
        segment_t *internal_compact( size_type element_size, void *table, internal_array_op1 destroy, internal_array_op2 copy );
        void internal_copy( const concurrent_vector_base& src, size_type element_size, internal_array_op2 copy );
        void internal_assign( const concurrent_vector_base& src, size_type element_size,
                              internal_array_op1 destroy, internal_array_op2 assign, internal_array_op2 copy );
        void internal_throw_exception(size_type) const;
        void internal_swap(concurrent_vector_base& v);

private:
        //! Private functionality
        class helper;
        friend class helper;
    };

    //TODO: deal with _Range_checked_iterator_tag of MSVC
    //! Meets requirements of a forward iterator for STL and a Value for a blocked_range.*/
    /** Value is either the T or const T type of the container.
        @ingroup containers */
    template<typename Container, typename Value>
    class vector_iterator 
#if defined(_WIN64) && defined(_MSC_VER) 
        // Ensure that Microsoft's internal template function _Val_type works correctly.
        : public std::iterator<std::random_access_iterator_tag,Value>
#endif /* defined(_WIN64) && defined(_MSC_VER) */
    {
        //! concurrent_vector over which we are iterating.
        Container* my_vector;

        //! Index into the vector 
        size_t my_index;

        //! Caches my_vector-&gt;internal_subscript(my_index)
        /** NULL if cached value is not available */
        mutable Value* my_item;

        template<typename C, typename T, typename U>
        friend bool operator==( const vector_iterator<C,T>& i, const vector_iterator<C,U>& j );

        template<typename C, typename T, typename U>
        friend bool operator<( const vector_iterator<C,T>& i, const vector_iterator<C,U>& j );

        template<typename C, typename T, typename U>
        friend ptrdiff_t operator-( const vector_iterator<C,T>& i, const vector_iterator<C,U>& j );
    
        template<typename C, typename U>
        friend class internal::vector_iterator;

#if !defined(_MSC_VER) || defined(__INTEL_COMPILER)
        template<typename T, class A>
        friend class tbb::concurrent_vector;
#else
public: // workaround for MSVC
#endif 

        vector_iterator( const Container& vector, size_t index ) : 
            my_vector(const_cast<Container*>(&vector)), 
            my_index(index), 
            my_item(NULL)
        {}

    public:
        //! Default constructor
        vector_iterator() : my_vector(NULL), my_index(~size_t(0)), my_item(NULL) {}

        vector_iterator( const vector_iterator<Container,typename Container::value_type>& other ) :
            my_vector(other.my_vector),
            my_index(other.my_index),
            my_item(other.my_item)
        {}

        vector_iterator operator+( ptrdiff_t offset ) const {
            return vector_iterator( *my_vector, my_index+offset );
        }
        friend vector_iterator operator+( ptrdiff_t offset, const vector_iterator& v ) {
            return vector_iterator( *v.my_vector, v.my_index+offset );
        }
        vector_iterator operator+=( ptrdiff_t offset ) {
            my_index+=offset;
            my_item = NULL;
            return *this;
        }
        vector_iterator operator-( ptrdiff_t offset ) const {
            return vector_iterator( *my_vector, my_index-offset );
        }
        vector_iterator operator-=( ptrdiff_t offset ) {
            my_index-=offset;
            my_item = NULL;
            return *this;
        }
        Value& operator*() const {
            Value* item = my_item;
            if( !item ) {
                item = my_item = &my_vector->internal_subscript(my_index);
            }
            __TBB_ASSERT( item==&my_vector->internal_subscript(my_index), "corrupt cache" );
            return *item;
        }
        Value& operator[]( ptrdiff_t k ) const {
            return my_vector->internal_subscript(my_index+k);
        }
        Value* operator->() const {return &operator*();}

        //! Pre increment
        vector_iterator& operator++() {
            size_t k = ++my_index;
            if( my_item ) {
                // Following test uses 2's-complement wizardry
                if( (k& k-2)==0 ) {
                    // k is a power of two that is at least k-2
                    my_item= NULL;
                } else {
                    ++my_item;
                }
            }
            return *this;
        }

        //! Pre decrement
        vector_iterator& operator--() {
            __TBB_ASSERT( my_index>0, "operator--() applied to iterator already at beginning of concurrent_vector" ); 
            size_t k = my_index--;
            if( my_item ) {
                // Following test uses 2's-complement wizardry
                if( (k& k-2)==0 ) {
                    // k is a power of two that is at least k-2  
                    my_item= NULL;
                } else {
                    --my_item;
                }
            }
            return *this;
        }

        //! Post increment
        vector_iterator operator++(int) {
            vector_iterator result = *this;
            operator++();
            return result;
        }

        //! Post decrement
        vector_iterator operator--(int) {
            vector_iterator result = *this;
            operator--();
            return result;
        }

        // STL support

        typedef ptrdiff_t difference_type;
        typedef Value value_type;
        typedef Value* pointer;
        typedef Value& reference;
        typedef std::random_access_iterator_tag iterator_category;
    };

    template<typename Container, typename T, typename U>
    bool operator==( const vector_iterator<Container,T>& i, const vector_iterator<Container,U>& j ) {
        return i.my_index==j.my_index;
    }

    template<typename Container, typename T, typename U>
    bool operator!=( const vector_iterator<Container,T>& i, const vector_iterator<Container,U>& j ) {
        return !(i==j);
    }

    template<typename Container, typename T, typename U>
    bool operator<( const vector_iterator<Container,T>& i, const vector_iterator<Container,U>& j ) {
        return i.my_index<j.my_index;
    }

    template<typename Container, typename T, typename U>
    bool operator>( const vector_iterator<Container,T>& i, const vector_iterator<Container,U>& j ) {
        return j<i;
    }

    template<typename Container, typename T, typename U>
    bool operator>=( const vector_iterator<Container,T>& i, const vector_iterator<Container,U>& j ) {
        return !(i<j);
    }

    template<typename Container, typename T, typename U>
    bool operator<=( const vector_iterator<Container,T>& i, const vector_iterator<Container,U>& j ) {
        return !(j<i);
    }

    template<typename Container, typename T, typename U>
    ptrdiff_t operator-( const vector_iterator<Container,T>& i, const vector_iterator<Container,U>& j ) {
        return ptrdiff_t(i.my_index)-ptrdiff_t(j.my_index);
    }

#if defined(_WIN64) && !defined(_CPPLIB_VER)
#pragma message ("Workaround for MS PSDK for Win64: allocator::rebind doesn't work")
#endif

    template<typename T, class A>
    class allocator_base {
    public:
#if defined(_WIN64) && !defined(_CPPLIB_VER)
        typedef A allocator_type;
#else
        typedef typename A::template
            rebind<T>::other allocator_type;
#endif
        allocator_type allocator;

        allocator_base(const allocator_type &a = allocator_type() ) : allocator(a) {}
    };

} // namespace internal
//! @endcond

//! Concurrent vector
/** @ingroup containers */
template<typename T, class A>
class concurrent_vector: protected internal::allocator_base<T, A>,
                         private internal::concurrent_vector_base {
private:
    template<typename I>
    class generic_range_type: public blocked_range<I> {
    public:
        typedef T value_type;
        typedef T& reference;
        typedef const T& const_reference;
        typedef I iterator;
        typedef ptrdiff_t difference_type;
        generic_range_type( I begin_, I end_, size_t grainsize = 1) : blocked_range<I>(begin_,end_,grainsize) {} 
        generic_range_type( generic_range_type& r, split ) : blocked_range<I>(r,split()) {}
    };

    template<typename C, typename U>
    friend class internal::vector_iterator;
public:
    //------------------------------------------------------------------------
    // STL compatible types
    //------------------------------------------------------------------------
    typedef internal::concurrent_vector_base::size_type size_type;
    typedef typename internal::allocator_base<T, A>::allocator_type allocator_type;

    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T *pointer;
    typedef const T *const_pointer;

    typedef internal::vector_iterator<concurrent_vector,T> iterator;
    typedef internal::vector_iterator<concurrent_vector,const T> const_iterator;

#if !defined(_MSC_VER) || _CPPLIB_VER>=300 
    // Assume ISO standard definition of std::reverse_iterator
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
#else
    // Use non-standard std::reverse_iterator
    typedef std::reverse_iterator<iterator,T,T&,T*> reverse_iterator;
    typedef std::reverse_iterator<const_iterator,T,const T&,const T*> const_reverse_iterator;
#endif /* defined(_MSC_VER) && (_MSC_VER<1300) */

    //------------------------------------------------------------------------
    // Parallel algorithm support
    //------------------------------------------------------------------------
    typedef generic_range_type<iterator> range_type;
    typedef generic_range_type<const_iterator> const_range_type;

    //------------------------------------------------------------------------
    // STL compatible constructors & destructors
    //------------------------------------------------------------------------

    //! Construct empty vector.
    concurrent_vector(const allocator_type &a = allocator_type())
        : internal::allocator_base<T, A>(a)
    {
        vector_allocator_ptr = &internal_allocator;
    }

    //! Copying constructor
    concurrent_vector( const concurrent_vector& vector ) {
        vector_allocator_ptr = &internal_allocator;
        internal_copy(vector, sizeof(T), &copy_array);
    }

    //! Copying constructor for vector with different allocator type
    template<class M>
    concurrent_vector( const concurrent_vector<T, M>& vector ) {
        vector_allocator_ptr = &internal_allocator;
        internal_copy(vector.internal_vector_base(), sizeof(T), &copy_array);
    }

    //! Construction with initial size specified by argument n
    explicit concurrent_vector(size_type n)
    {
        vector_allocator_ptr = &internal_allocator;
#ifdef TBB_VECTOR_NO_SOLID
        grow_by( n );
#else
        if ( !n ) return;
        internal_reserve(n, sizeof(T), max_size()); my_early_size = n;
        __TBB_ASSERT( my_first_block == segment_index_of(n)+1, NULL );
        initialize_array(static_cast<T*>(my_segment[0].array), NULL, n);
#endif
    }

    //! Construction with initial size specified by argument n, initialization by copying of t, and given allocator instance
    explicit concurrent_vector(size_type n, const_reference t, const allocator_type& a = allocator_type())
        : internal::allocator_base<T, A>(a)
    {
        vector_allocator_ptr = &internal_allocator;
        internal_assign( n, t );
    }

    //! Construction with copying iteration range and given allocator instance
    template<class I>
    concurrent_vector(I first, I last, const allocator_type &a = allocator_type())
        : internal::allocator_base<T, A>(a)
    {
        vector_allocator_ptr = &internal_allocator;
        internal_assign(first, last);
    }

    //! Assignment
    concurrent_vector& operator=( const concurrent_vector& vector ) {
        if( this != &vector )
            concurrent_vector_base::internal_assign(vector, sizeof(T), &destroy_array, &assign_array, &copy_array);
        return *this;
    }

    //! Assignment for vector with different allocator type
    template<class M>
    concurrent_vector& operator=( const concurrent_vector<T, M>& vector ) {
        if( reinterpret_cast<void*>( this ) != reinterpret_cast<const void*>( &vector ) )
            concurrent_vector_base::internal_assign(vector.internal_vector_base(),
                sizeof(T), &destroy_array, &assign_array, &copy_array);
        return *this;
    }

    //------------------------------------------------------------------------
    // Concurrent operations
    //------------------------------------------------------------------------
    //! Grow by "delta" elements.
    /** Returns old size. */
    size_type grow_by( size_type delta ) {
        return delta ? internal_grow_by( delta, sizeof(T), &initialize_array, NULL ) : my_early_size;
    }

    //! Grow by "delta" elements using copying constuctor.
    /** Returns old size. */
    size_type grow_by( size_type delta, const_reference t ) {
        return delta ? internal_grow_by( delta, sizeof(T), &initialize_array_by, static_cast<const void*>(&t) ) : my_early_size;
    }

    //! Grow array until it has at least n elements.
    void grow_to_at_least( size_type n ) {
        if( my_early_size<n )
            internal_grow_to_at_least( n, sizeof(T), &initialize_array, NULL );
    };

    //! Push item 
    size_type push_back( const_reference item ) {
        size_type k;
        new( internal_push_back(sizeof(T),k) ) T(item);
        return k;
    }

    //! Get reference to element at given index.
    /** This method is thread-safe for concurrent reads, and also while growing the vector,
        as long as the calling thread has checked that index&lt;size(). */
    reference operator[]( size_type index ) {
        return internal_subscript(index);
    }

    //! Get const reference to element at given index.
    const_reference operator[]( size_type index ) const {
        return internal_subscript(index);
    }

    //! Get reference to element at given index.
    reference at( size_type index ) {
        if( index >= size() )
            internal_throw_exception(0);
        return internal_subscript(index);
    }

    //! Get const reference to element at given index.
    const_reference at( size_type index ) const {
        if( index >= size() )
            internal_throw_exception(0);
        return internal_subscript(index);
    }

    range_type range( size_t grainsize = 1) {
        return range_type( begin(), end(), grainsize );
    }

    const_range_type range( size_t grainsize = 1 ) const {
        return const_range_type( begin(), end(), grainsize );
    }
    //------------------------------------------------------------------------
    // Capacity
    //------------------------------------------------------------------------
    //! Return size of vector.
    size_type size() const {return my_early_size;}

    //! Return size of vector.
    bool empty() const {return !my_early_size;}

    //! Maximum size to which array can grow without allocating more memory.
    size_type capacity() const {return internal_capacity();}

    //! Allocate enough space to grow to size n without having to allocate more memory later.
    /** Like most of the methods provided for STL compatibility, this method is *not* thread safe. 
        The capacity afterwards may be bigger than the requested reservation. */
    void reserve( size_type n ) {
        if( n )
            internal_reserve(n, sizeof(T), max_size());
    }

    //! Optimize memory usage and fragmentation. Returns true if optimization occurred.
    void compact();

    //! Upper bound on argument to reserve.
    size_type max_size() const {return (~size_type(0))/sizeof(T);}

    //------------------------------------------------------------------------
    // STL support
    //------------------------------------------------------------------------

    //! start iterator
    iterator begin() {return iterator(*this,0);}
    //! end iterator
    iterator end() {return iterator(*this,size());}
    //! start const iterator
    const_iterator begin() const {return const_iterator(*this,0);}
    //! end const iterator
    const_iterator end() const {return const_iterator(*this,size());}
    //! reverse start iterator
    reverse_iterator rbegin() {return reverse_iterator(end());}
    //! reverse end iterator
    reverse_iterator rend() {return reverse_iterator(begin());}
    //! reverse start const iterator
    const_reverse_iterator rbegin() const {return const_reverse_iterator(end());}
    //! reverse end const iterator
    const_reverse_iterator rend() const {return const_reverse_iterator(begin());}
    //! the first item
    reference front() { return *begin(); }
    //! the first item const
    const_reference front() const { return *begin(); }
    //! the last item
    reference back() { return *--end(); }
    //! the last item const
    const_reference back() const { return *--end(); }
    //! return allocator object
    allocator_type get_allocator() const { return this->allocator; }

    //! assign n items by copying t item
    void assign(size_type n, const_reference t) { clear(); internal_assign( n, t ); }
    //! assign range [first, last)
    template<class I>
    void assign(I first, I last) { clear(); internal_assign( first, last ); }

    //! swap two instances
    void swap(concurrent_vector &vector) {
        if( this != &vector ) {
            concurrent_vector_base::internal_swap(static_cast<concurrent_vector_base&>(vector));
            std::swap(this->allocator, vector.allocator);
        }
    }

    //! Clear container. Not thread safe
    void clear() {
        internal_free_segments( my_segment, internal_clear(&destroy_array), my_first_block );
#if !TBB_VECTOR_NO_SOLID
        my_first_block = 0; // here is not default_initial_segments
#else
        my_first_block = default_initial_segments;
#endif
    }

    //! Clear and destroy vector.
    ~concurrent_vector() {
        clear();
        // base class destructor call should be then
    }

    inline const internal::concurrent_vector_base &internal_vector_base() const { return *this; }
private:
    //! Allocate k items
    static void *internal_allocator(internal::concurrent_vector_base &vb, size_t k) {
        return static_cast<concurrent_vector<T, A>&>(vb).allocator.allocate(k);
    }
    //! Free k segments from table
    void internal_free_segments(segment_t *table, segment_index_t k, segment_index_t first_block);

    //! Get reference to element at given index.
    T& internal_subscript( size_type index ) const;

    //! assign n items by copying t
    void internal_assign(size_type n, const_reference t);

    //! assign by iterators
    template<class I>
    void internal_assign(I first, I last);

    //! Construct n instances of T, starting at "begin".
    static void initialize_array( void* begin, const void*, size_type n );

    //! Construct n instances of T, starting at "begin".
    static void initialize_array_by( void* begin, const void* src, size_type n );

    //! Construct n instances of T, starting at "begin".
    static void copy_array( void* dst, const void* src, size_type n );

    //! Assign n instances of T, starting at "begin".
    static void assign_array( void* dst, const void* src, size_type n );

    //! Destroy n instances of T, starting at "begin".
    static void destroy_array( void* begin, size_type n );
};

template<typename T, class A>
void concurrent_vector<T, A>::compact() {
    segment_index_t old_first_block = my_first_block;
    char table_space[sizeof(segment_t) * pointers_per_long_table];
    segment_t *old_table = internal_compact( sizeof(T), reinterpret_cast<void*>(table_space), &destroy_array, &copy_array );
    if( old_table )
        internal_free_segments( old_table, my_first_block, old_first_block );
}

template<typename T, class A>
void concurrent_vector<T, A>::internal_free_segments(segment_t *table, segment_index_t k, segment_index_t first_block) {
    // Free the arrays
    while( k > first_block ) {
        --k;
        segment_t& s = table[k];
        T* array = reinterpret_cast<T*>(s.array);
        s.array = NULL;
        this->allocator.deallocate( array, segment_size(k) );
    }
    T* array = reinterpret_cast<T*>(table[0].array);
    if( array ) {
        __TBB_ASSERT( first_block > 0, NULL );
        while(k > 0) table[--k].array = 0;
        this->allocator.deallocate( array, segment_size(first_block) );
    }
}

template<typename T, class A>
T& concurrent_vector<T, A>::internal_subscript( size_type index ) const {
    __TBB_ASSERT( index<size(), "index out of bounds" );
    size_type j = index;
    segment_index_t k = segment_base_index_of( j );
    return static_cast<T*>(my_segment[k].array)[j];
}

template<typename T, class A>
void concurrent_vector<T, A>::internal_assign(size_type n, const_reference t)
{
#ifdef TBB_VECTOR_NO_SOLID
    grow_by( n, t );
#else
    if( !n ) return;
    internal_reserve(n, sizeof(T), max_size()); my_early_size = n;
    __TBB_ASSERT( my_first_block == segment_index_of(n)+1, NULL );
    initialize_array_by(static_cast<T*>(my_segment[0].array), static_cast<const void*>(&t), n);
#endif
}

template<typename T, class A> template<class I>
void concurrent_vector<T, A>::internal_assign(I first, I last) {
    size_type n = std::distance(first, last);
    if( !n ) return;
    internal_reserve(n, sizeof(T), max_size()); my_early_size = n;
    __TBB_ASSERT( my_first_block == segment_index_of(n)+1, NULL );
    T* array = static_cast<T*>(my_segment[0].array);
    for( size_type j = 0; j < n; ++j, ++first )
        new( &array[j] ) T( *first );
}

template<typename T, class A>
void concurrent_vector<T, A>::initialize_array( void* begin, const void *, size_type n ) {
    T* array = static_cast<T*>(begin);
    for( size_type j=0; j<n; ++j )
        new( &array[j] ) T();
}

template<typename T, class A>
void concurrent_vector<T, A>::initialize_array_by( void* begin, const void *src, size_type n ) {
    T* array = static_cast<T*>(begin);
    const T &s = *static_cast<const T*>(src);
    for( size_type j=0; j<n; ++j )
        new( &array[j] ) T(s);
}

template<typename T, class A>
void concurrent_vector<T, A>::copy_array( void* dst, const void* src, size_type n ) {
    T* d = static_cast<T*>(dst);
    const T* s = static_cast<const T*>(src);
    for( size_type j=0; j<n; ++j )
        new( &d[j] ) T(s[j]);
}

template<typename T, class A>
void concurrent_vector<T, A>::assign_array( void* dst, const void* src, size_type n ) {
    T* d = static_cast<T*>(dst);
    const T* s = static_cast<const T*>(src);
    for( size_type j=0; j<n; ++j )
        d[j] = s[j];
}

template<typename T, class A>
void concurrent_vector<T, A>::destroy_array( void* begin, size_type n ) {
    T* array = static_cast<T*>(begin);
    for( size_type j=n; j>0; --j )
        array[j-1].~T();
}

// concurrent_vector's template functions
template<typename T, class A1, class A2>
inline bool operator==(const concurrent_vector<T, A1> &a, const concurrent_vector<T, A2> &b) {
    //TODO: deal with _Range_checked_iterator_tag of MSVC.
    // Simply:    return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin());
    if(a.size() != b.size()) return false;
    typename concurrent_vector<T, A1>::const_iterator i(a.begin());
    typename concurrent_vector<T, A2>::const_iterator j(b.begin());
    for(; i != a.end(); ++i, ++j)
        if( !(*i == *j) ) return false;
    return true;
}

template<typename T, class A1, class A2>
inline bool operator!=(const concurrent_vector<T, A1> &a, const concurrent_vector<T, A2> &b)
{    return !(a == b); }

template<typename T, class A1, class A2>
inline bool operator<(const concurrent_vector<T, A1> &a, const concurrent_vector<T, A2> &b)
{    return (std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end())); }

template<typename T, class A1, class A2>
inline bool operator>(const concurrent_vector<T, A1> &a, const concurrent_vector<T, A2> &b)
{    return b < a; }

template<typename T, class A1, class A2>
inline bool operator<=(const concurrent_vector<T, A1> &a, const concurrent_vector<T, A2> &b)
{    return !(b < a); }

template<typename T, class A1, class A2>
inline bool operator>=(const concurrent_vector<T, A1> &a, const concurrent_vector<T, A2> &b)
{    return !(a < b); }

template<typename T, class A>
inline void swap(concurrent_vector<T, A> &a, concurrent_vector<T, A> &b)
{    a.swap( b ); }

} // namespace tbb

/*******************************************************
Changes in tbb::concurrent_vector since 2.0 version:
* custom allocator support through template argument 
  - defaulting to cache_aligned_allocator for now
* segment sizes start from 2 (then 2, 4, 8...) 
  - faster index calculation 
  - fixes memory blow up for swarm of vector’s instances of small size 
* first segments block solid allocation 
  - first growth call specifies a number of segments to be merged in the first allocation. 
  - introduces additional size_t field in data structure but doesn’t require additional mask on segment pointer
    – potentially faster then “any solid segment” technique. 
  - in conjunction with faster index calculation gives about 5-10% performance grow for test set of standard sequential algorithms. 
* introduced new methods:
  - grow_by(size_type n, const_reference t) growth using copying constructor to init new items. 
  - STL-like constructors.
  - operators ==, < and derivatives
  - at() method
  - get_allocator() method.
  - assign() methods
  - compact() method to defragment first segments increasing items access time. 
  - swap() method
* range() defaults on grainsize = 1 supporting auto grainsize algorithms. 
* clear() behavior changed to freeing segments memory 
********************************************************/
#endif /* __TBB_concurrent_vector_H */
