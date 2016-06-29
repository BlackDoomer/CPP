#pragma once

#include <memory>
#include <iterator>
#include <algorithm>
#include <cmath>

// TODO: Specify noexcept where needed.

#define __LINARRAY_HPP_TYPEDEF_MIXIN(A) \
  private: \
    typedef std::allocator_traits<A>                alloc_traits; \
  public: \
    typedef A                                       allocator_type; \
    typedef typename alloc_traits::value_type       value_type; \
    typedef value_type&                             reference; \
    typedef const value_type&                       const_reference; \
    typedef typename alloc_traits::size_type        size_type; \
    typedef typename alloc_traits::difference_type  difference_type; \
    typedef typename alloc_traits::pointer          pointer; \
    typedef typename alloc_traits::const_pointer    const_pointer; \
    typedef pointer                                 iterator; \
    typedef const_pointer                           const_iterator; \
    typedef std::reverse_iterator<iterator>         reverse_iterator; \
    typedef std::reverse_iterator<const_iterator>   const_reverse_iterator

namespace {
  template< class Allocator >
  class storage_t { __LINARRAY_HPP_TYPEDEF_MIXIN( Allocator );
    private:
      struct { pointer start, end; } data;
      allocator_type& allocator;

    public:
      inline static size_type calc_capacity( size_type count ) {
        return static_cast<size_type>(
            std::pow( 2.0, std::ceil( std::log2( (count>0) ?count :1 ) ) )
        );
      }

      explicit storage_t( size_type count, allocator_type& alloc )
      : allocator(alloc)
      {
        const size_type mem_capacity = calc_capacity( count );
        data.start = alloc_traits::allocate( allocator, mem_capacity );
        data.end = data.start + mem_capacity;
      }

      ~storage_t() {
        alloc_traits::deallocate( allocator, data.start, capacity() );
      }

      inline pointer buffer() { return data.start; }
      inline iterator begin() { return data.start; }
      inline iterator end() { return data.end; }
      inline const size_type capacity() const { return data.end - data.start; }
  };
} //end of namespace

/* ========================================================================== */

template< class T, class Allocator = std::allocator<T> >
class linarray { __LINARRAY_HPP_TYPEDEF_MIXIN( Allocator );
  private:
    typedef storage_t<Allocator> storage;
    allocator_type allocator;
    storage* s_data;
    size_type s_count;

  public:
    explicit linarray( size_type count = 0,
      const allocator_type& alloc = Allocator() )
    : allocator(alloc)
    {
      set_storage( count );
      if (count > 0)
        construct_in_range( s_data->begin(), s_data->end(), T() );
    }

    linarray( size_type count, const_reference value,
      const allocator_type& alloc = Allocator() )
    : allocator(alloc)
    {
      set_storage( count );
      construct_in_range( s_data->begin(), s_data->end(), value );
    }

    //note: next magic fixes conflict with fill constructor
    template< typename InputIt,
      typename std::enable_if<
        !std::is_integral<InputIt>::value
      >::type* = nullptr >
    linarray( InputIt first, InputIt last,
      const allocator_type& alloc = Allocator() )
    : allocator(alloc)
    {
      set_storage_from( first, last );
    }

    linarray( const linarray& other,
      const allocator_type& alloc = Allocator() )
    : allocator(alloc)
    {
      set_storage_from( other.cbegin(), other.cend() );
    }

    linarray( std::initializer_list<value_type> init,
      const allocator_type& alloc = Allocator() )
    : allocator(alloc)
    {
      set_storage_from( init.begin(), init.end() );
    }

    ~linarray() {
      free_storage();
    }

    /* assignment operators */
    linarray& operator= ( const linarray& other ) {
      free_storage();
      set_storage_from( other.cbegin(), other.cend() );
      return (*this);
    }

    linarray& operator= ( std::initializer_list<value_type> init ) {
      free_storage();
      set_storage_from( init.begin(), init.end() );
      return (*this);
    }

    /* iterators */
    inline const_iterator cbegin() const { return s_data->begin(); }
    inline iterator begin() { return const_cast<iterator>( cbegin() ); }

    inline const_iterator cend() const { return cbegin() + s_count; }
    inline iterator end() { return const_cast<iterator>( cend() ); }

    inline const_reverse_iterator
      crbegin() const { return const_reverse_iterator( cend() ); }
    inline reverse_iterator
      rbegin() { return reverse_iterator( end() ); }

    inline const_reverse_iterator
      crend() const { return const_reverse_iterator( cbegin() ); }
    inline reverse_iterator
      rend() { return reverse_iterator( begin() ); }

    /* data access */
    inline const_reference
      operator[] ( size_type pos ) const { return *(begin() + pos); }
    inline reference
      operator[] ( size_type pos ) { return *(begin() + pos); }

    inline const_reference front() const { return *begin(); }
    inline reference front() { return *begin(); }

    inline const_reference back() const { return *(end() - 1); }
    inline reference back() { return *(end() - 1); }

    inline const_pointer data() const { return s_data->buffer(); }
    inline pointer data() { return s_data->buffer(); }

    /* capacity */
    inline bool empty() const { return s_count == 0; }
    inline size_type size() const { return s_count; }
    inline size_type capacity() const { return s_data->capacity(); }

    void shrink_to_fit() {
      if ( capacity() < storage::calc_capacity( size() ) ) { return; }
      storage* new_storage = new storage( size(), allocator );
      copy_from_range( cbegin(), cend(), new_storage->begin() );
      free_storage();
      s_data = new_storage;
    }

    /* management */
    void clear() {
      /* this changes capacity, don't use */
      // free_storage();
      // set_storage(0);

      /* right way */
      resize(0);
    }

    void resize( size_type count ) {
      if ( count > size() ) {
        resize( count, T() );
      }
      else if ( count < size() ) {
        const size_type erase_count = size() - count;
        destroy_in_range( cend() - erase_count, cend() );
        s_count -= erase_count;
      }
    }

    void resize( size_type count, const_reference value ) {
      if ( count > size() ) {
        const size_type add_count = count - size();
        const_iterator first_new = insert_empty_space( cend(), add_count );
        construct_in_range( first_new, cend(), value );
      }
      else if ( count < size() ) {
        resize( count );
      }
    }

    iterator insert( size_type count, const_reference value,
      const_iterator pos )
    {
      if ( count == 0 ) { return const_cast<iterator>(pos); }
      iterator new_pos = insert_empty_space( pos, count );
      construct_in_range( new_pos, new_pos + count, value );
      return new_pos;
    }

    template< typename InputIt >
    iterator insert( InputIt first, InputIt last, const_iterator pos ) {
      if ( first == last ) { return const_cast<iterator>(pos); }
      const size_type insert_count = std::distance( first, last );
      iterator new_pos = insert_empty_space( pos, insert_count );
      copy_from_range( first, last, new_pos );
      return new_pos;
    }

    template< typename... Args >
    iterator emplace( const_iterator pos, Args&&... args ) {
      iterator new_pos = insert_empty_space( pos, 1 );
      alloc_traits::construct( allocator,
        std::addressof(*new_pos), std::forward<Args>(args)... );
      return new_pos;
    }

    inline iterator erase( const_iterator pos ) {
      return erase( pos, pos+1 );
    }

    iterator erase( const_iterator first, const_iterator last ) {
      iterator vfirst = const_cast<iterator>(first);
      iterator vlast = const_cast<iterator>(last);
      const size_type erase_count = std::distance( first, last );
      std::move( vlast, end(), vfirst );
      destroy_in_range( cend() - erase_count, cend() );
      s_count -= erase_count;
      return vfirst;
    }

    /* common management */
    inline void push_back( const_reference value ) {
      insert( 1, value, cend() );
    }

    template< typename... Args >
    inline void emplace_back( Args&&... args ) {
      emplace( cend(), std::forward<Args>(args)... );
    }

    inline void pop_back() {
      erase( cend()-1 );
    }

  private:
    /* storage management */
    void set_storage( size_type count ) {
      s_data = new storage( count, allocator );
      s_count = count;
    }

    void free_storage() {
      destroy_in_range( cbegin(), cend() );
      delete s_data;
    }

    template< typename InputIt >
    void set_storage_from( InputIt first, InputIt last ) {
      set_storage( std::distance( first, last ) );
      copy_from_range( first, last, s_data->begin() );
    }

    iterator insert_empty_space( const_iterator pos, size_type count )
    {
      iterator new_pos;

      if ( size() + count <= capacity() ) {
        //const size_type ucopy_count = std::min( count, std::distance(pos, end()) );
        new_pos = const_cast<iterator>(pos);
        iterator ucopy_from = std::max( new_pos, end()-count );
        iterator ucopy_dest = std::max( new_pos+count, end() );
        copy_from_range( ucopy_from, end(), ucopy_dest );
        std::move_backward( new_pos, ucopy_from, ucopy_dest );
        destroy_in_range( pos, pos+count );
      }
      else { //if we have no enough space at the end of the storage
        storage* new_storage = new storage( size() + count, allocator );
        new_pos = copy_from_range( cbegin(), pos, new_storage->begin() );
        copy_from_range( pos, cend(), new_pos + count );
        free_storage();
        s_data = new_storage;
      }

      s_count += count;
      return new_pos;
    }

    /* data management */

    // similar to std::uninitialized_fill(), but uses Allocator
    void construct_in_range(
      const_iterator first, const_iterator last, const_reference value )
    {
      const_iterator current_pos = first;
      try {
        while (current_pos != last) {
          alloc_traits::construct( allocator,
            std::addressof(*current_pos++), value );
        }
      } catch (...) {
        destroy_in_range( first, current_pos );
        throw;
      }
    }

    // similar to std::uninitialized_copy(), but uses Allocator
    template< typename InputIt >
    iterator copy_from_range(
      InputIt first, InputIt last, const_iterator d_first )
    {
      const_iterator current_dest = d_first;
      try {
        while (first != last) {
          alloc_traits::construct( allocator,
            std::addressof(*current_dest++), *first++ );
        }
        return const_cast<iterator>(current_dest);
      } catch (...) {
        destroy_in_range( d_first, current_dest );
        throw;
      }
    }

    void destroy_in_range(
      const_iterator first, const_iterator last )
    {
      while (first != last) {
        alloc_traits::destroy( allocator, first++ );
      }
    }
};

#undef __LINARRAY_HPP_TYPEDEF_MIXIN

