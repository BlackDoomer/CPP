#pragma once

#include <memory>

template<typename T>
class abc_allocator {
  public:
    typedef T                 value_type;
    typedef       value_type* pointer;
    typedef const value_type* const_pointer;
    typedef       value_type& reference;
    typedef const value_type& const_reference;
    typedef std::size_t       size_type;
    typedef std::ptrdiff_t    difference_type;

  public:
    template<typename U>
    struct rebind {
      typedef abc_allocator<U> other;
    };

  public:
    abc_allocator() {}

    template< typename U >
    abc_allocator( const abc_allocator<U>& ) {}

    inline pointer allocate( size_type cnt ) {
      return reinterpret_cast<pointer>( ::operator new( cnt*sizeof(T) ) );
    }
    inline void deallocate( pointer p, size_type ) {
      ::operator delete(p);
    }
};

template< typename T, typename U >
bool operator== ( const abc_allocator<T>&, const abc_allocator<U>& ) {
  return true;
}

template< typename T, typename U >
bool operator!= ( const abc_allocator<T>&, const abc_allocator<U>& ) {
  return false;
}
