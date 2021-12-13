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

#ifndef __TBB_concurrent_hash_map_H
#define __TBB_concurrent_hash_map_H

#include "tbb_stddef.h"
#include "cache_aligned_allocator.h"
#include "spin_rw_mutex.h"
#include "atomic.h"
#include <iterator>
#include <utility>      // Need std::pair from here

namespace tbb {

//! @cond INTERNAL
namespace internal {
    template<typename Value, typename Iterator>
    class hash_map_range;

    struct hash_map_segment_base {
        //! Mutex that protects this segment
        spin_rw_mutex my_mutex;

        // Number of buckets
        atomic<size_t> my_logical_size;

        // Size of chains
        /** Always zero or a a power of two */
        size_t my_physical_size;

        //! True if my_logical_size>=my_physical_size.
        /** Used to support Intel(R) Thread Checker. */
        bool internal_grow_predicate() const;
    };

    //! Meets requirements of a forward iterator for STL */
    /** Value is either the T or const T type of the container.
        @ingroup containers */ 
    template<typename Container, typename Value>
    class hash_map_iterator {
        typedef typename Container::bucket bucket;
        typedef typename Container::chain chain;

        //! concurrent_hash_map over which we are iterating.
        Container* my_table;

        //! Pointer to bucket that has current item
        bucket* my_bucket;

        //! Index into hash table's array for current item
        size_t my_array_index;  

        //! Index of segment that has array for current item
        size_t my_segment_index;

        template<typename C, typename T, typename U>
        friend bool operator==( const hash_map_iterator<C,T>& i, const hash_map_iterator<C,U>& j );

        template<typename C, typename T, typename U>
        friend bool operator!=( const hash_map_iterator<C,T>& i, const hash_map_iterator<C,U>& j );

        template<typename C, typename T, typename U>
        friend ptrdiff_t operator-( const hash_map_iterator<C,T>& i, const hash_map_iterator<C,U>& j );
    
        template<typename C, typename U>
        friend class internal::hash_map_iterator;

        template<typename V, typename I>
        friend class internal::hash_map_range;

        void advance_to_next_bucket() {
            size_t i = my_array_index+1;
            do {
                while( i<my_table->my_segment[my_segment_index].my_physical_size ) {
                    my_bucket = my_table->my_segment[my_segment_index].my_array[i].bucket_list;
                    if( my_bucket ) goto done;
                    ++i;
                }
                i = 0;
            } while( ++my_segment_index<my_table->n_segment );
        done:
            my_array_index = i;
        }
    public:
        //! Construct undefined iterator
        hash_map_iterator() {}
        hash_map_iterator( const Container& table, size_t segment_index, size_t array_index=0 );
        hash_map_iterator( const hash_map_iterator<Container,const Value>& other ) :
            my_table(other.my_table),
            my_bucket(other.my_bucket),
            my_array_index(other.my_array_index),
            my_segment_index(other.my_segment_index)
        {}
        Value& operator*() const {
            __TBB_ASSERT( my_bucket, "iterator uninitialized or at end of container?" );
            return my_bucket->item;
        }
        Value* operator->() const {return &operator*();}
        hash_map_iterator& operator++();
        
        //! Post increment
        Value* operator++(int) {
            Value* result = &operator*();
            operator++();
            return result;
        }

        // STL support

        typedef ptrdiff_t difference_type;
        typedef Value value_type;
        typedef Value* pointer;
        typedef Value& reference;
        typedef std::forward_iterator_tag iterator_category;
    };

    template<typename Container, typename Value>
    hash_map_iterator<Container,Value>::hash_map_iterator( const Container& table, size_t segment_index, size_t array_index ) : 
        my_table(const_cast<Container*>(&table)),
        my_bucket(NULL),
        my_array_index(array_index),
        my_segment_index(segment_index)
    {
        if( segment_index<my_table->n_segment ) {
            chain* first_chain = my_table->my_segment[segment_index].my_array;
            my_bucket = first_chain ? first_chain[my_array_index].bucket_list : NULL;
            if( !my_bucket ) advance_to_next_bucket();
        }
    }

    template<typename Container, typename Value>
    hash_map_iterator<Container,Value>& hash_map_iterator<Container,Value>::operator++() {
        my_bucket=my_bucket->next;
        if( !my_bucket ) advance_to_next_bucket();
        return *this;
    }

    template<typename Container, typename T, typename U>
    bool operator==( const hash_map_iterator<Container,T>& i, const hash_map_iterator<Container,U>& j ) {
        return i.my_bucket==j.my_bucket;
    }

    template<typename Container, typename T, typename U>
    bool operator!=( const hash_map_iterator<Container,T>& i, const hash_map_iterator<Container,U>& j ) {
        return i.my_bucket!=j.my_bucket;
    }

    //! Range class used with concurrent_hash_map
    /** @ingroup containers */ 
    template<typename Value, typename Iterator>
    class hash_map_range{
    private:
        Iterator my_begin;
        Iterator my_end;
        mutable Iterator my_midpoint;
        size_t my_grainsize;
        //! Set my_midpoint to point approximately half way between my_begin and my_end.
        void set_midpoint() const;
    public:
        typedef Value value_type;
        typedef Value& reference;
        typedef const Value& const_reference;
        typedef Iterator iterator;
        typedef ptrdiff_t difference_type;

        //! True if range is empty.
        bool empty() const {return my_begin==my_end;}

        //! True if range can be partitioned into two subranges.
        bool is_divisible() const {
            return my_midpoint!=my_end;
        }
        //! Split range.
        hash_map_range( hash_map_range& r, split ) : 
            my_end(r.my_end),
            my_grainsize(r.my_grainsize)
        {
            r.my_end = my_begin = r.my_midpoint;
            set_midpoint();
            r.set_midpoint();
        }
        hash_map_range( const Iterator& begin_, const Iterator& end_, size_t grainsize ) : 
            my_begin(begin_), 
            my_end(end_), 
            my_grainsize(grainsize) 
        {
            set_midpoint();
            __TBB_ASSERT( grainsize>0, "grainsize must be positive" );
        }
        const Iterator& begin() const {return my_begin;}
        const Iterator& end() const {return my_end;}
    };

    template<typename Value, typename Iterator>
    void hash_map_range<Value,Iterator>::set_midpoint() const {
        size_t n = my_end.my_segment_index-my_begin.my_segment_index;
        if( n>1 || (n==1 && my_end.my_array_index>0) ) {
            // Split by groups of segments
            my_midpoint = Iterator(*my_begin.my_table,(my_end.my_segment_index+my_begin.my_segment_index)/2u);
        } else {
            // Split by groups of buckets
            size_t m = my_end.my_array_index-my_begin.my_array_index;
            if( m>my_grainsize ) {
                my_midpoint = Iterator(*my_begin.my_table,my_begin.my_segment_index,m/2u);
            } else {
                my_midpoint = my_end;
            }
        }
        __TBB_ASSERT( my_midpoint.my_segment_index<=my_begin.my_table->n_segment, NULL );
    }  
} // namespace internal
//! @endcond

//! Unorderd map from Key to T.
/** @ingroup containers */
template<typename Key, typename T, typename HashCompare>
class concurrent_hash_map {
public:
    class const_accessor;
    class accessor;

    typedef Key key_type;
    typedef T mapped_type;
    typedef std::pair<const Key,T> value_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

private:
    template<typename Container, typename Value>
    friend class internal::hash_map_iterator;

    template<typename V, typename I>
    friend class internal::hash_map_range;

    typedef spin_rw_mutex bucket_mutex_t;
    typedef spin_rw_mutex chain_mutex_t;
    typedef spin_rw_mutex segment_mutex_t;

    //! Type of a hash code.
    typedef size_t hashcode_t;

    struct bucket;
    friend struct bucket;

    //! Basic unit of storage used in chain.
    struct bucket {
        //! Next bucket in chain
        bucket* next;
        bucket_mutex_t mutex;
        value_type item;
        bucket( const Key& key ) : item(key,T()) {}
        bucket( const Key& key, const T& t ) : item(key,t) {}
    };

    struct chain;
    friend struct chain;

    //! A linked-list of buckets.
    /** Should be zero-initialized before use. */
    struct chain {
        void push_front( bucket& b ) {
            b.next = bucket_list;
            bucket_list = &b;
        }
        chain_mutex_t mutex;
        bucket* bucket_list;
    };

    struct segment;
    friend struct segment;

    //! Segment of the table.
    /** The table is partioned into disjoint segments to reduce conflicts.
        A segment should be zero-intialized before use. */
    struct segment: internal::hash_map_segment_base {
#if TBB_DO_ASSERT
        ~segment() {
            __TBB_ASSERT( !my_array, "should have been cleared earlier" );
        }
#endif /* TBB_DO_ASSERT */

        // Pointer to array of chains
        chain* my_array;

        // Get chain in this segment that corresponds to given hash code.
        chain& get_chain( hashcode_t hashcode, size_t n_segment_bits ) {
            return my_array[(hashcode>>n_segment_bits)&(my_physical_size-1)];
        }
     
        //! Allocate an array with at least new_size chains. 
        /** "new_size" is rounded up to a power of two that occupies at least one cache line.
            Does not deallocate the old array.  Overwrites my_array. */
        void allocate_array( size_t new_size ) {
            size_t n=(internal::NFS_GetLineSize()+sizeof(chain)-1)/sizeof(chain);
            __TBB_ASSERT((n&(n-1))==0, NULL);
            while( n<new_size ) n<<=1;
            chain* array = cache_aligned_allocator<chain>().allocate( n );
            memset( array, 0, n*sizeof(chain) );
            my_array = array;
            my_physical_size = n;
        }
    };

    segment& get_segment( hashcode_t hashcode ) {
        return my_segment[hashcode&n_segment-1];
    }
        
    HashCompare my_hash_compare;

    //! Log2 of n_segment
    /** Placed after my_hash_compare for sake of efficient packing. */
    unsigned char n_segment_bits;

    //! Number of segments 
    size_t n_segment;

    segment* my_segment;

    bucket* search_list( const Key& key, chain& c ) {
        bucket* b = c.bucket_list;
        while( b&& !my_hash_compare.equal(key,b->item.first) )
            b=b->next;
        return b;
    }

    bucket* remove_from_list( const Key& key, chain& c ) {
        bucket** p = &c.bucket_list;
        bucket* b=*p;
        while( b && !my_hash_compare.equal(key,b->item.first ) ) {
            p = &b->next;
            b= *p;
        }
        if( b )
            *p = b->next;
        return b;
    }

    //! Grow segment for which caller has acquired a write lock.
    void grow_segment( segment_mutex_t::scoped_lock& segment_lock, segment& s );

    enum operation {
        op_insert,
        op_find,
        op_remove
    };

    //! Does heavy lifting for "find" and "insert".
    bool lookup( const_accessor* result, const Key& key, bool write, operation op );

    //! Perform initialization on behalf of a constructor
    void initialize() {
        n_segment_bits = 6; 
        n_segment = size_t(1)<<n_segment_bits; 
        my_segment = cache_aligned_allocator<segment>().allocate(n_segment);
        memset( my_segment, 0, sizeof(segment)*n_segment );
     }

    //! Copy "source" to *this, where *this must start out empty.
    void internal_copy( const concurrent_hash_map& source );
public:
    friend class const_accessor;

    //! Combines data access, locking, and garbage collection.
    class const_accessor {
        friend class concurrent_hash_map;
        friend class accessor;
        void operator=( const accessor& ) const; // Deny access
        const_accessor( const accessor& );       // Deny access
    public:
        //! Type of value
        typedef const std::pair<const Key,T> value_type;

        //! True if result is empty.
        bool empty() const {return !my_bucket;}

        //! Set to null
        void release() {
            if( my_bucket ) {
                my_lock.release();
                my_bucket = NULL;
            }   
        }

        //! Return reference to associated value in hash table.
        const value_type& operator*() const {
            __TBB_ASSERT( my_bucket, "attempt to deference empty result" );
            return my_bucket->item;
        }

        //! Return pointer to associated value in hash table.
        const value_type* operator->() const {
            return &operator*();
        }

        //! Create empty result
        const_accessor() : my_bucket(NULL) {}

        //! Destroy result after releasing the underlying reference.
        ~const_accessor() {
            my_bucket = NULL; // my_lock.release() is called in scoped_lock destructor
        }
    private:
        bucket* my_bucket;
        bucket_mutex_t::scoped_lock my_lock;
    };   

    //! Allows write access to elements and combines data access, locking, and garbage collection.
    class accessor: public const_accessor {
    public:
        //! Type of value
        typedef std::pair<const Key,T> value_type;

        //! Return reference to associated value in hash table.
        value_type& operator*() const {
            __TBB_ASSERT( this->my_bucket, "attempt to deference empty result" );
            return this->my_bucket->item;
        }

        //! Return pointer to associated value in hash table.
        value_type* operator->() const {
            return &operator*();
        }       
    };

    //! Construct empty table.
    concurrent_hash_map();

    //! Copy constructor
    concurrent_hash_map( const concurrent_hash_map& table ) {
        initialize();
        internal_copy(table);
    }

    //! Assignment
    concurrent_hash_map& operator=( const concurrent_hash_map& table ) {
        if( this!=&table ) {
            clear();
            internal_copy(table);
        } 
        return *this;
    }

    //! Clear table
    void clear();

    //! Clear table and destroy it.  
    ~concurrent_hash_map();

    //------------------------------------------------------------------------
    // concurrent map operations
    //------------------------------------------------------------------------

    //! Find item and acquire a read lock on the item.
    /** Return true if item is found, false otherwise. */
    bool find( const_accessor& result, const Key& key ) const {
        return const_cast<concurrent_hash_map*>(this)->lookup(&result,key,/*write=*/false,op_find);
    }   

    //! Find item and acquire a write lock on the item.
    /** Return true if item is found, false otherwise. */
    bool find( accessor& result, const Key& key ) {
        return lookup(&result,key,/*write=*/true,op_find);
    }
        
    //! Insert item (if not already present) and acquire a read lock on the item.
    /** Returns true if item is new. */
    bool insert( const_accessor& result, const Key& key ) {
        return lookup(&result,key,/*write=*/false,op_insert);
    }

    //! Insert item (if not already present) and acquire a write lock on the item.
    /** Returns true if item is new. */
    bool insert( accessor& result, const Key& key ) {
        return lookup(&result,key,/*write=*/true,op_insert);
    }

    //! Erase item.
    /** Return true if item was erased. */
    bool erase( const Key& key ) {
        return lookup(NULL,key,/*write=*/true,op_remove);
    }

    //------------------------------------------------------------------------
    // Parallel algorithm support
    //------------------------------------------------------------------------
    typedef internal::hash_map_iterator<concurrent_hash_map,value_type> iterator;
    typedef internal::hash_map_iterator<concurrent_hash_map,const value_type> const_iterator;

    typedef internal::hash_map_range<value_type,iterator> range_type;
    typedef internal::hash_map_range<const value_type,const_iterator> const_range_type;

    range_type range( size_type grainsize ) {
        return range_type( begin(), end(), grainsize );
    }

    const_range_type range( size_type grainsize ) const {
        return const_range_type( begin(), end(), grainsize );
    }

    //------------------------------------------------------------------------
    // STL support
    //------------------------------------------------------------------------
    iterator begin() {return iterator(*this,0);}
    iterator end() {return iterator(*this,n_segment);}
    const_iterator begin() const {return const_iterator(*this,0);}
    const_iterator end() const {return const_iterator(*this,n_segment);}
    
    //! Number of items in table.
    /** Be aware that this method is relatively slow compared to the 
        typical size() method for an STL container. */
    size_type size() const;

    //! True if size()==0.
    bool empty() const;

    //! Upper bound on size.
    size_type max_size() const {return (~size_type(0))/sizeof(bucket);}
};

template<typename Key, typename T, typename HashCompare>
typename concurrent_hash_map<Key,T,HashCompare>::size_type concurrent_hash_map<Key,T,HashCompare>::size() const {
    size_type result = 0;
    for( size_t k=0; k<n_segment; ++k )
        result += my_segment[k].my_logical_size;
    return result;
}

template<typename Key, typename T, typename HashCompare>
bool concurrent_hash_map<Key,T,HashCompare>::empty() const {
    for( size_t k=0; k<n_segment; ++k )
        if( my_segment[k].my_logical_size )
            return false;
    return true;
}

template<typename Key, typename T, typename HashCompare>
bool concurrent_hash_map<Key,T,HashCompare>::lookup( const_accessor* result, const Key& key, bool write, operation op ) {
    if( result && result->my_bucket )
        result->release();
    hashcode_t h = my_hash_compare.hash( key );
    bucket* b = NULL;
    bucket* b_temp = NULL;
    bool return_value = false;  
    {
        segment& s = get_segment(h);
#if TBB_DO_THREADING_TOOLS||TBB_DO_ASSERT
        bool grow = op==op_insert && s.internal_grow_predicate();
#else
        bool grow = op==op_insert && s.my_logical_size >= s.my_physical_size;
#endif /* TBB_DO_THREADING_TOOLS||TBB_DO_ASSERT */
        segment_mutex_t::scoped_lock segment_lock( s.my_mutex, /*write=*/grow );
        if( grow ) {
            // Load factor is too high  
            grow_segment(segment_lock,s);
        }
        {
            if( !s.my_array ) {
                __TBB_ASSERT( op!=op_insert, NULL ); 
                goto done;
            }
            __TBB_ASSERT( (s.my_physical_size&s.my_physical_size-1)==0, NULL );
            chain& c = s.get_chain(h,n_segment_bits);
            chain_mutex_t::scoped_lock chain_lock( c.mutex, /*write=*/false );
            b = search_list(key,c);
            if( b ) {
                if( op==op_find )
                    return_value = true;
                else if( op==op_remove ) {
                    return_value = true;
                    chain_lock.upgrade_to_writer();
                    bucket_mutex_t::scoped_lock item_lock( b->mutex, true );
                    b_temp = remove_from_list(key,c);
                    --s.my_logical_size;
                    goto delete_temp;
                } 
            } else {
                if( op==op_insert ) {
                    // Search failed
                    b_temp = new bucket(key);

                    if( !chain_lock.upgrade_to_writer() ) {
                        // Rerun search_list, in case another thread inserted the item during the upgrade.
                        b = search_list(key,c);
                    }
                    if( !b ) {
                        return_value = true;
                        b = b_temp;
                        b_temp = NULL;
                        c.push_front( *b );
                        ++s.my_logical_size;
                    }
                }
            }
            if( b ) {
                result->my_lock.acquire(b->mutex,write);
                result->my_bucket = b;
            }
        }
    }
delete_temp:   
    if( b_temp ) {
        {
            // Get write lock after slicing from list, to ensure no one else
            // is currently using *b
            bucket_mutex_t::scoped_lock item_lock( b_temp->mutex, /*write=*/true );
        }
        delete b_temp;
    }
done:
    return return_value;        
}

template<typename Key, typename T, typename HashCompare>
void concurrent_hash_map<Key,T,HashCompare>::clear() {
    for( size_t i=0; i<n_segment; ++i ) {
        segment& s = my_segment[i];
        if( chain* array = s.my_array ) {
            size_t n = s.my_physical_size;
            s.my_array = NULL;
            s.my_physical_size = 0;
            s.my_logical_size = 0;
            for( size_t j=0; j<n; ++j ) {
                while( bucket* b = array[j].bucket_list ) {
                    array[j].bucket_list = b->next;
                    delete b;
                }
            }
            cache_aligned_allocator<chain>().deallocate( array, n );
        }
    }
}

template<typename Key, typename T, typename HashCompare>
void concurrent_hash_map<Key,T,HashCompare>::grow_segment( segment_mutex_t::scoped_lock& segment_lock, segment& s ) {
    // Following is second check in a double-check.
    if( s.my_logical_size >= s.my_physical_size ) {
        chain* old_array = s.my_array;
        size_t old_size = s.my_physical_size;
        s.allocate_array( s.my_logical_size );
        for( size_t k=0; k<old_size; ++k )
            while( bucket* b = old_array[k].bucket_list ) {
                old_array[k].bucket_list = b->next;
                hashcode_t h = my_hash_compare.hash( b->item.first );
                __TBB_ASSERT( &get_segment(h)==&s, "hash function changed?" );
                s.get_chain(h,n_segment_bits).push_front(*b);
            }   
        cache_aligned_allocator<chain>().deallocate( old_array, old_size );
    }
    segment_lock.downgrade_to_reader();
}

template<typename Key, typename T, typename HashCompare>
void concurrent_hash_map<Key,T,HashCompare>::internal_copy( const concurrent_hash_map<Key,T,HashCompare>& source ) {
    __TBB_ASSERT( n_segment==source.n_segment, NULL );
    for( size_t i=0; i<n_segment; ++i ) {
        segment& s = source.my_segment[i];
        __TBB_ASSERT( !my_segment[i].my_array, "caller should have cleared" );
        if( s.my_logical_size ) {
            segment& d = my_segment[i];
            d.my_logical_size = s.my_logical_size;
            d.allocate_array( s.my_logical_size );
            size_t s_size = s.my_physical_size;
            chain* s_array = s.my_array;
            for( size_t k=0; k<s_size; ++k )
                for( bucket* b = s_array[k].bucket_list; b; b=b->next ) {
                    hashcode_t h = my_hash_compare.hash( b->item.first );
                    __TBB_ASSERT( &get_segment(h)==&d, "hash function changed?" );
                    bucket* b_new = new bucket(b->item.first,b->item.second);
                    d.get_chain(h,n_segment_bits).push_front(*b_new);
                }
        }
    }
}

template<typename Key, typename T, typename HashCompare>
concurrent_hash_map<Key,T,HashCompare>::concurrent_hash_map() {
    initialize();
}

template<typename Key, typename T, typename HashCompare>
concurrent_hash_map<Key,T,HashCompare>::~concurrent_hash_map() {
    clear();
    cache_aligned_allocator<segment>().deallocate( my_segment, n_segment );
}

} // namespace tbb

#endif /* __TBB_concurrent_hash_map_H */
