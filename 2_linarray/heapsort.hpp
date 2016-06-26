#pragma once

namespace {
  typedef size_t size_type;

  template< class RandomIt >
  void sift_down( RandomIt heap_first, RandomIt heap_last, size_type index ) {
    RandomIt root( heap_first + index );
    while ( std::distance(root, heap_last) > 0 ) {
      const size_type left_index = 2 * index + 1;
      const size_type right_index = left_index + 1;
      const RandomIt left_child( heap_first + left_index );
      const RandomIt right_child( heap_first + right_index );

      RandomIt max_child = root;
      if (left_child < heap_last) if (*left_child > *max_child)
        max_child = left_child;
      if (right_child < heap_last) if (*right_child > *max_child)
        max_child = right_child;

      if (max_child == root)
        return;

      std::swap( *root, *max_child );
      root = max_child;
      index = std::distance( heap_first, root );
    }
  }
} //end of namespace

namespace custom {

  template< class RandomIt >
  void heap_sort( RandomIt first, RandomIt last ) {
    // TODO: Exception if first > last ?
    size_type s_sort = std::distance( first, last );

    //building heap
    for( size_type i = s_sort/2; i > 0; --i ) {
      sift_down( first, last, i-1 );
    }

    //sorting
    while ( s_sort > 1 ) {
      std::swap( *first, *--last );
      sift_down( first, last, 0 );
      --s_sort;
    }
  }

} //end of namespace "custom"
