#include <iostream>
#include <random>
#include <cmath>

#include <measure_exec.hpp>

#include <vector>
#include "linarray.hpp"
#include "abc_allocator.hpp"
#include "heapsort.hpp"

#include "catch/catch_with_main.hpp"

class IntElement {
  private:
    int value;
  public:
    static int RefCount;

    IntElement(const int& val = 0): value(val) { ++IntElement::RefCount; }
    IntElement(const IntElement& val): value(val.get_value()) { ++IntElement::RefCount; }
    ~IntElement() { --IntElement::RefCount; }
    int get_value() const { return value; }

    #define COMPARE_OPERATOR(op) \
      bool operator op ( const IntElement& other ) const \
        { return value op other.get_value(); } \
      bool operator op ( const int& other ) const \
        { return value op other; }
      COMPARE_OPERATOR(==)
      COMPARE_OPERATOR(!=);
      COMPARE_OPERATOR(>=);
      COMPARE_OPERATOR(<=);
      COMPARE_OPERATOR(>);
      COMPARE_OPERATOR(<);
    #undef COMPARE_OPERATOR

    friend std::ostream& operator<< ( std::ostream& os, const IntElement& x );
};

std::ostream& operator<< ( std::ostream& os, const IntElement& x ) {
  os << x.get_value();
  return os;
}

int IntElement::RefCount = 0;

typedef std::vector<int> t_vector;
typedef linarray<IntElement> t_linarray_std;
typedef linarray<IntElement, abc_allocator<IntElement>> t_linarray_abc;

typedef t_vector::size_type size_type;
//typedef t_vector::difference_type difference_type;

typedef shared::measure<> time_measure;

/* ========================================================================== */

template< class T1, class T2 >
static bool IS_EQUAL_CONTAINERS( T1 con1, T2 con2 ) {
  if ( con1.size() != con2.size() ) { return false; }
  for (size_type i = 0; i < con1.size(); ++i) {
    if ( con1[i] != con2[i] ) { return false; }
  }
  return true;
}

template< class T >
static void PRINT_CONTAINER( T con ) {
  std::cout << std::endl;
  for (size_type i = 0; i < con.size(); ++i) {
    std::cout << con[i] << " / ";
  }
  std::cout << std::endl;
}

static size_type EXACT_CAPACITY( size_type count ) {
  return static_cast<size_type>(
    std::pow( 2.0, std::ceil( std::log2( (count>0) ?count :1 ) ) )
  );
}

/* ========================================================================== */
/* ========================================================================== */

#include "unittest_sets.inc"

static const size_type ELEMENTS_COUNT = 116;
static const size_type ELEMENTS_CAPACITY = EXACT_CAPACITY( ELEMENTS_COUNT );
static const int CUSTOM_VALUE = 234;

TEST_CASE( "linarray constructors and destructor", "[life]" ) {
  SECTION( "default constructor" ) {
    t_linarray_std larr_std;
    t_linarray_abc larr_abc;
    REQUIRE( larr_std.size() == 0 );
    REQUIRE( larr_abc.size() == 0 );
  }
  SECTION( "fill constructor, default value" ) {
    t_vector test_vec(ELEMENTS_COUNT);
    t_linarray_std larr_std(ELEMENTS_COUNT);
    t_linarray_abc larr_abc(ELEMENTS_COUNT);
    REQUIRE( IS_EQUAL_CONTAINERS( larr_std, test_vec ) );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_abc, test_vec ) );
  }
  SECTION( "fill constructor, custom value" ) {
    t_vector test_vec(ELEMENTS_COUNT, CUSTOM_VALUE);
    t_linarray_std larr_std(ELEMENTS_COUNT, CUSTOM_VALUE);
    t_linarray_abc larr_abc(ELEMENTS_COUNT, CUSTOM_VALUE);
    REQUIRE( IS_EQUAL_CONTAINERS( larr_std, test_vec ) );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_abc, test_vec ) );
  }
  SECTION( "range constructor" ) {
    t_vector test_vec{ELEMENTS_SET_FORWARD};
    t_linarray_std larr_std( std::begin(test_vec), std::end(test_vec) );
    t_linarray_abc larr_abc( std::begin(test_vec), std::end(test_vec) );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_std, test_vec ) );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_abc, test_vec ) );
  }
  SECTION( "copy constructor" ) {
    t_vector test_vec{ELEMENTS_SET_FORWARD};
    t_linarray_std larr_std1( std::begin(test_vec), std::end(test_vec) );
    t_linarray_std larr_std2( larr_std1 );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_std1, larr_std2 ) );
    t_linarray_abc larr_abc1( std::begin(test_vec), std::end(test_vec) );
    t_linarray_abc larr_abc2( larr_abc1 );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_abc1, larr_abc2 ) );
  }
  SECTION( "list constructor" ) {
    t_vector test_vec{ELEMENTS_SET_FORWARD};
    t_linarray_std larr_std{ELEMENTS_SET_FORWARD};
    t_linarray_abc larr_abc{ELEMENTS_SET_FORWARD};
    REQUIRE( IS_EQUAL_CONTAINERS( larr_std, test_vec ) );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_abc, test_vec ) );
  }
  SECTION( "elements constructors/destructors invocation" ) {
    IntElement::RefCount = 0;
    t_linarray_std* larr_std = new t_linarray_std{ELEMENTS_SET_FORWARD};
    REQUIRE( IntElement::RefCount == ELEMENTS_COUNT );
    t_linarray_abc* larr_abc = new t_linarray_abc{ELEMENTS_SET_FORWARD};
    REQUIRE( IntElement::RefCount == 2 * ELEMENTS_COUNT );
    delete larr_std;
    REQUIRE( IntElement::RefCount == ELEMENTS_COUNT );
    delete larr_abc;
    REQUIRE( IntElement::RefCount == 0 );
  }
}

TEST_CASE( "linarray assignment operators", "[assign]" ) {
  SECTION( "linarray assignment" ) {
    t_linarray_std larr_std1{ELEMENTS_SET_FORWARD};
    t_linarray_std larr_std2 = larr_std1;
    REQUIRE( IS_EQUAL_CONTAINERS( larr_std1, larr_std2 ) );
    t_linarray_abc larr_abc1{ELEMENTS_SET_FORWARD};
    t_linarray_abc larr_abc2 = larr_abc1;
    REQUIRE( IS_EQUAL_CONTAINERS( larr_abc1, larr_abc2 ) );
  }
  SECTION( "initializer list assignment" ) {
    t_vector test_vec;
    test_vec = {ELEMENTS_SET_FORWARD};
    t_linarray_std larr_std;
    larr_std = {ELEMENTS_SET_FORWARD};
    REQUIRE( IS_EQUAL_CONTAINERS( larr_std, test_vec ) );
    t_linarray_abc larr_abc;
    larr_abc = {ELEMENTS_SET_FORWARD};
    REQUIRE( IS_EQUAL_CONTAINERS( larr_abc, test_vec ) );
  }
}

TEST_CASE( "linarray data access", "[data]" ) {
  t_vector vec_fw{ELEMENTS_SET_FORWARD};
  t_vector vec_bk{ELEMENTS_SET_BACKWARD};
  t_linarray_std larr_test{ELEMENTS_SET_FORWARD};
  SECTION( "forward and reverse iterators" ) {
    t_linarray_std larr_fw( larr_test.cbegin(), larr_test.cend() );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_fw, vec_fw ) );
    t_linarray_std larr_bk( larr_test.crbegin(), larr_test.crend() );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_bk, vec_bk ) );
  }
  SECTION( "index access operator, front() and back() access methods" ) {
    t_linarray_std::const_iterator iter_fw = larr_test.cbegin();
    for (size_type i = 0; i < larr_test.size(); ++i) {
      REQUIRE( larr_test[i] == *iter_fw++ );
    }
    t_linarray_std::const_reverse_iterator iter_rv = larr_test.crbegin();
    for (size_type i = larr_test.size(); i > 0; --i) {
      REQUIRE( larr_test[i-1] == *iter_rv++ );
    }
    REQUIRE( larr_test.front() == *--iter_rv );
    REQUIRE( larr_test.back() == *--iter_fw );
  }
}

TEST_CASE( "linarray capacity methods", "[size]" ) {
  SECTION( "size and capacity correctness" ) {
    t_linarray_std larr_std{ELEMENTS_SET_FORWARD};
    t_linarray_abc larr_abc{ELEMENTS_SET_FORWARD};
    REQUIRE( larr_std.size() == ELEMENTS_COUNT );
    REQUIRE( larr_abc.size() == ELEMENTS_COUNT );
    REQUIRE( larr_std.capacity() == ELEMENTS_CAPACITY );
    REQUIRE( larr_abc.capacity() == ELEMENTS_CAPACITY );
    larr_std.resize(ELEMENTS_COUNT);
    larr_abc.resize(ELEMENTS_COUNT);
    REQUIRE( larr_std.size() == ELEMENTS_COUNT );
    REQUIRE( larr_abc.size() == ELEMENTS_COUNT );
    REQUIRE( larr_std.capacity() == ELEMENTS_CAPACITY );
    REQUIRE( larr_abc.capacity() == ELEMENTS_CAPACITY );
    larr_std.clear();
    larr_abc.clear();
    REQUIRE( larr_std.empty() );
    REQUIRE( larr_abc.empty() );
    REQUIRE( larr_std.size() == 0 );
    REQUIRE( larr_abc.size() == 0 );
    REQUIRE( larr_std.capacity() == ELEMENTS_CAPACITY );
    REQUIRE( larr_abc.capacity() == ELEMENTS_CAPACITY );
    larr_std.shrink_to_fit();
    larr_abc.shrink_to_fit();
    REQUIRE( larr_std.capacity() == 1 );
    REQUIRE( larr_abc.capacity() == 1 );
  }
  SECTION( "size increasing, default value" ) {
    t_vector test_vec{ELEMENTS_SET_FORWARD};
    t_linarray_std larr_std{ELEMENTS_SET_FORWARD};
    t_linarray_abc larr_abc{ELEMENTS_SET_FORWARD};
    test_vec.resize( ELEMENTS_COUNT*2 );
    larr_std.resize( ELEMENTS_COUNT*2 );
    larr_abc.resize( ELEMENTS_COUNT*2 );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_std, test_vec ) );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_abc, test_vec ) );
  }
  SECTION( "size increasing, custom value" ) {
    t_vector test_vec{ELEMENTS_SET_FORWARD};
    t_linarray_std larr_std{ELEMENTS_SET_FORWARD};
    t_linarray_abc larr_abc{ELEMENTS_SET_FORWARD};
    test_vec.resize( ELEMENTS_COUNT*2, CUSTOM_VALUE );
    larr_std.resize( ELEMENTS_COUNT*2, CUSTOM_VALUE );
    larr_abc.resize( ELEMENTS_COUNT*2, CUSTOM_VALUE );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_std, test_vec ) );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_abc, test_vec ) );
  }
  SECTION( "size decreasing" ) {
    t_vector test_vec{ELEMENTS_SET_FORWARD};
    t_linarray_std larr_std{ELEMENTS_SET_FORWARD};
    t_linarray_abc larr_abc{ELEMENTS_SET_FORWARD};
    test_vec.resize( ELEMENTS_COUNT/2 );
    larr_std.resize( ELEMENTS_COUNT/2 );
    larr_abc.resize( ELEMENTS_COUNT/2 );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_std, test_vec ) );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_abc, test_vec ) );
  }
  SECTION( "size decreasing with unused value argument" ) {
    t_vector test_vec{ELEMENTS_SET_FORWARD};
    t_linarray_std larr_std{ELEMENTS_SET_FORWARD};
    t_linarray_abc larr_abc{ELEMENTS_SET_FORWARD};
    test_vec.resize( ELEMENTS_COUNT/2, CUSTOM_VALUE );
    larr_std.resize( ELEMENTS_COUNT/2, CUSTOM_VALUE );
    larr_abc.resize( ELEMENTS_COUNT/2, CUSTOM_VALUE );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_std, test_vec ) );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_abc, test_vec ) );
  }
}

TEST_CASE( "elements management", "[manage]" ) {
  SECTION( "value copy insertion" ) {
    IntElement custval(CUSTOM_VALUE);
    t_vector test_vec{ELEMENTS_SET_FORWARD};
    t_linarray_std larr_std{ELEMENTS_SET_FORWARD};
    t_linarray_abc larr_abc{ELEMENTS_SET_FORWARD};
    test_vec.insert( test_vec.cbegin(), ELEMENTS_COUNT, CUSTOM_VALUE );
    larr_std.insert( ELEMENTS_COUNT, custval, larr_std.cbegin() );
    larr_abc.insert( ELEMENTS_COUNT, custval, larr_abc.cbegin() );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_std, test_vec ) );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_abc, test_vec ) );
    test_vec.insert( test_vec.cend(), ELEMENTS_COUNT, CUSTOM_VALUE );
    larr_std.insert( ELEMENTS_COUNT, custval, larr_std.cend() );
    larr_abc.insert( ELEMENTS_COUNT, custval, larr_abc.cend() );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_std, test_vec ) );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_abc, test_vec ) );
  }
  SECTION( "range copy insertion" ) {
    t_vector range_vec{ELEMENTS_SET_BACKWARD};
    t_vector test_vec{ELEMENTS_SET_FORWARD};
    t_linarray_std larr_std{ELEMENTS_SET_FORWARD};
    t_linarray_abc larr_abc{ELEMENTS_SET_FORWARD};
    test_vec.insert( test_vec.cbegin(), range_vec.cbegin(), range_vec.cend() );
    larr_std.insert( range_vec.cbegin(), range_vec.cend(), larr_std.cbegin() );
    larr_abc.insert( range_vec.cbegin(), range_vec.cend(), larr_abc.cbegin() );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_std, test_vec ) );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_abc, test_vec ) );
    test_vec.insert( test_vec.cend(), range_vec.cbegin(), range_vec.cend() );
    larr_std.insert( range_vec.cbegin(), range_vec.cend(), larr_std.cend() );
    larr_abc.insert( range_vec.cbegin(), range_vec.cend(), larr_abc.cend() );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_std, test_vec ) );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_abc, test_vec ) );
  }
  SECTION( "emplace, default constructor" ) {
    t_vector test_vec{ELEMENTS_SET_FORWARD};
    t_linarray_std larr_std{ELEMENTS_SET_FORWARD};
    t_linarray_abc larr_abc{ELEMENTS_SET_FORWARD};
    test_vec.emplace( test_vec.cbegin() );
    larr_std.emplace( larr_std.cbegin() );
    larr_abc.emplace( larr_abc.cbegin() );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_std, test_vec ) );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_abc, test_vec ) );
    test_vec.emplace( test_vec.cend() );
    larr_std.emplace( larr_std.cend() );
    larr_abc.emplace( larr_abc.cend() );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_std, test_vec ) );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_abc, test_vec ) );
  }
  SECTION( "emplace, copy constructor" ) {
    t_vector test_vec{ELEMENTS_SET_FORWARD};
    t_linarray_std larr_std{ELEMENTS_SET_FORWARD};
    t_linarray_abc larr_abc{ELEMENTS_SET_FORWARD};
    test_vec.emplace( test_vec.cbegin(), CUSTOM_VALUE );
    larr_std.emplace( larr_std.cbegin(), CUSTOM_VALUE );
    larr_abc.emplace( larr_abc.cbegin(), CUSTOM_VALUE );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_std, test_vec ) );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_abc, test_vec ) );
    test_vec.emplace( test_vec.cend(), CUSTOM_VALUE );
    larr_std.emplace( larr_std.cend(), CUSTOM_VALUE );
    larr_abc.emplace( larr_abc.cend(), CUSTOM_VALUE );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_std, test_vec ) );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_abc, test_vec ) );
  }
  SECTION( "erasing elements" ) {
    t_vector test_vec{ELEMENTS_SET_FORWARD};
    t_linarray_std larr_std{ELEMENTS_SET_FORWARD};
    t_linarray_abc larr_abc{ELEMENTS_SET_FORWARD};
    test_vec.erase( test_vec.cbegin() );
    larr_std.erase( larr_std.cbegin() );
    larr_abc.erase( larr_abc.cbegin() );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_std, test_vec ) );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_abc, test_vec ) );
    test_vec.erase( test_vec.cend()-1 );
    larr_std.erase( larr_std.cend()-1 );
    larr_abc.erase( larr_abc.cend()-1 );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_std, test_vec ) );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_abc, test_vec ) );
    test_vec.erase( test_vec.cbegin()+1, test_vec.cend()-2 );
    larr_std.erase( larr_std.cbegin()+1, larr_std.cend()-2 );
    larr_abc.erase( larr_abc.cbegin()+1, larr_abc.cend()-2 );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_std, test_vec ) );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_abc, test_vec ) );
  }
  SECTION( "common management" ) {
    t_vector test_vec{ELEMENTS_SET_FORWARD};
    t_linarray_std larr_std{ELEMENTS_SET_FORWARD};
    t_linarray_abc larr_abc{ELEMENTS_SET_FORWARD};
    test_vec.push_back( CUSTOM_VALUE );
    larr_std.push_back( CUSTOM_VALUE );
    larr_abc.push_back( CUSTOM_VALUE );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_std, test_vec ) );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_abc, test_vec ) );
    test_vec.pop_back();
    larr_std.pop_back();
    larr_abc.pop_back();
    REQUIRE( IS_EQUAL_CONTAINERS( larr_std, test_vec ) );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_abc, test_vec ) );
    test_vec.emplace_back( CUSTOM_VALUE );
    larr_std.emplace_back( CUSTOM_VALUE );
    larr_abc.emplace_back( CUSTOM_VALUE );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_std, test_vec ) );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_abc, test_vec ) );
  }
}

TEST_CASE( "sorting", "[sort]" ) {
  const size_type sort_count = 100000;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, sort_count-1);

  t_vector vec_forward, vec_backward, vec_shuffled;
  for (size_type i = 1; i <= sort_count; ++i) {
    vec_forward.push_back(i);
    vec_backward.push_back(sort_count-i);
    vec_shuffled.push_back(dis(gen));
  }

  t_linarray_std larrstd_forward( vec_forward.cbegin(), vec_forward.cend() );
  t_linarray_std larrstd_backward( vec_backward.cbegin(), vec_backward.cend() );
  t_linarray_std larrstd_shuffled( vec_shuffled.cbegin(), vec_shuffled.cend() );

  t_linarray_abc larrabc_forward( vec_forward.cbegin(), vec_forward.cend() );
  t_linarray_abc larrabc_backward( vec_backward.cbegin(), vec_backward.cend() );
  t_linarray_abc larrabc_shuffled( vec_shuffled.cbegin(), vec_shuffled.cend() );

  /* ====================================================================== */

  SECTION( "forward order, ascending" ) {
    t_vector test_vec(vec_forward);
    t_linarray_std larr_std(larrstd_forward);
    t_linarray_abc larr_abc(larrabc_forward);

    // test_vec
    const auto test_vec_std_time = time_measure::execution(
      std::sort<t_vector::iterator>,
      test_vec.begin(), test_vec.end()
    );
    test_vec = vec_forward;
    const auto test_vec_my_time = time_measure::execution(
      custom::heap_sort<t_vector::iterator>,
      test_vec.begin(), test_vec.end()
    );

    // larr_std
    const auto larr_std_std_time = time_measure::execution(
      std::sort<t_linarray_std::iterator>,
      larr_std.begin(), larr_std.end()
    );
    larr_std = larrstd_forward;
    const auto larr_std_my_time = time_measure::execution(
      custom::heap_sort<t_linarray_std::iterator>,
      larr_std.begin(), larr_std.end()
    );

    // larr_abc
    const auto larr_abc_std_time = time_measure::execution(
      std::sort<t_linarray_abc::iterator>,
      larr_abc.begin(), larr_abc.end()
    );
    larr_abc = larrabc_forward;
    const auto larr_abc_my_time = time_measure::execution(
      custom::heap_sort<t_linarray_abc::iterator>,
      larr_abc.begin(), larr_abc.end()
    );

    REQUIRE( IS_EQUAL_CONTAINERS( larr_std, test_vec ) );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_abc, test_vec ) );

    std::cout << "forward order, ascending:"
      << "\n  std::vector"
      << "\n    std::sort: " << test_vec_std_time
      << "\n    custom::heap_sort: " << test_vec_my_time
      << "\n  linarray (std::allocator)"
      << "\n    std::sort: " << larr_std_std_time
      << "\n    custom::heap_sort: " << larr_std_my_time
      << "\n  linarray (abc_allocator)"
      << "\n    std::sort: " << larr_abc_std_time
      << "\n    custom::heap_sort: " << larr_abc_my_time
      << "\n" << std::endl;
  }
  SECTION( "forward order, descending" ) {
    t_vector test_vec(vec_forward);
    t_linarray_std larr_std(larrstd_forward);
    t_linarray_abc larr_abc(larrabc_forward);

    // test_vec
    const auto test_vec_std_time = time_measure::execution(
      std::sort<t_vector::reverse_iterator>,
      test_vec.rbegin(), test_vec.rend()
    );
    test_vec = vec_forward;
    const auto test_vec_my_time = time_measure::execution(
      custom::heap_sort<t_vector::reverse_iterator>,
      test_vec.rbegin(), test_vec.rend()
    );

    // larr_std
    const auto larr_std_std_time = time_measure::execution(
      std::sort<t_linarray_std::reverse_iterator>,
      larr_std.rbegin(), larr_std.rend()
    );
    larr_std = larrstd_forward;
    const auto larr_std_my_time = time_measure::execution(
      custom::heap_sort<t_linarray_std::reverse_iterator>,
      larr_std.rbegin(), larr_std.rend()
    );

    // larr_abc
    const auto larr_abc_std_time = time_measure::execution(
      std::sort<t_linarray_abc::reverse_iterator>,
      larr_abc.rbegin(), larr_abc.rend()
    );
    larr_abc = larrabc_forward;
    const auto larr_abc_my_time = time_measure::execution(
      custom::heap_sort<t_linarray_abc::reverse_iterator>,
      larr_abc.rbegin(), larr_abc.rend()
    );

    REQUIRE( IS_EQUAL_CONTAINERS( larr_std, test_vec ) );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_abc, test_vec ) );

    std::cout << "forward order, descending:"
      << "\n  std::vector"
      << "\n    std::sort: " << test_vec_std_time
      << "\n    custom::heap_sort: " << test_vec_my_time
      << "\n  linarray (std::allocator)"
      << "\n    std::sort: " << larr_std_std_time
      << "\n    custom::heap_sort: " << larr_std_my_time
      << "\n  linarray (abc_allocator)"
      << "\n    std::sort: " << larr_abc_std_time
      << "\n    custom::heap_sort: " << larr_abc_my_time
      << "\n" << std::endl;
  }

  /* ====================================================================== */

  SECTION( "backward order, ascending" ) {
    t_vector test_vec(vec_backward);
    t_linarray_std larr_std(larrstd_backward);
    t_linarray_abc larr_abc(larrabc_backward);

    // test_vec
    const auto test_vec_std_time = time_measure::execution(
      std::sort<t_vector::iterator>,
      test_vec.begin(), test_vec.end()
    );
    test_vec = vec_backward;
    const auto test_vec_my_time = time_measure::execution(
      custom::heap_sort<t_vector::iterator>,
      test_vec.begin(), test_vec.end()
    );

    // larr_std
    const auto larr_std_std_time = time_measure::execution(
      std::sort<t_linarray_std::iterator>,
      larr_std.begin(), larr_std.end()
    );
    larr_std = larrstd_backward;
    const auto larr_std_my_time = time_measure::execution(
      custom::heap_sort<t_linarray_std::iterator>,
      larr_std.begin(), larr_std.end()
    );

    // larr_abc
    const auto larr_abc_std_time = time_measure::execution(
      std::sort<t_linarray_abc::iterator>,
      larr_abc.begin(), larr_abc.end()
    );
    larr_abc = larrabc_backward;
    const auto larr_abc_my_time = time_measure::execution(
      custom::heap_sort<t_linarray_abc::iterator>,
      larr_abc.begin(), larr_abc.end()
    );

    REQUIRE( IS_EQUAL_CONTAINERS( larr_std, test_vec ) );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_abc, test_vec ) );

    std::cout << "backward order, ascending:"
      << "\n  std::vector"
      << "\n    std::sort: " << test_vec_std_time
      << "\n    custom::heap_sort: " << test_vec_my_time
      << "\n  linarray (std::allocator)"
      << "\n    std::sort: " << larr_std_std_time
      << "\n    custom::heap_sort: " << larr_std_my_time
      << "\n  linarray (abc_allocator)"
      << "\n    std::sort: " << larr_abc_std_time
      << "\n    custom::heap_sort: " << larr_abc_my_time
      << "\n" << std::endl;
  }
  SECTION( "backward order, descending" ) {
    t_vector test_vec(vec_backward);
    t_linarray_std larr_std(larrstd_backward);
    t_linarray_abc larr_abc(larrabc_backward);

    // test_vec
    const auto test_vec_std_time = time_measure::execution(
      std::sort<t_vector::reverse_iterator>,
      test_vec.rbegin(), test_vec.rend()
    );
    test_vec = vec_backward;
    const auto test_vec_my_time = time_measure::execution(
      custom::heap_sort<t_vector::reverse_iterator>,
      test_vec.rbegin(), test_vec.rend()
    );

    // larr_std
    const auto larr_std_std_time = time_measure::execution(
      std::sort<t_linarray_std::reverse_iterator>,
      larr_std.rbegin(), larr_std.rend()
    );
    larr_std = larrstd_backward;
    const auto larr_std_my_time = time_measure::execution(
      custom::heap_sort<t_linarray_std::reverse_iterator>,
      larr_std.rbegin(), larr_std.rend()
    );

    // larr_abc
    const auto larr_abc_std_time = time_measure::execution(
      std::sort<t_linarray_abc::reverse_iterator>,
      larr_abc.rbegin(), larr_abc.rend()
    );
    larr_abc = larrabc_backward;
    const auto larr_abc_my_time = time_measure::execution(
      custom::heap_sort<t_linarray_abc::reverse_iterator>,
      larr_abc.rbegin(), larr_abc.rend()
    );

    REQUIRE( IS_EQUAL_CONTAINERS( larr_std, test_vec ) );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_abc, test_vec ) );

    std::cout << "backward order, descending:"
      << "\n  std::vector"
      << "\n    std::sort: " << test_vec_std_time
      << "\n    custom::heap_sort: " << test_vec_my_time
      << "\n  linarray (std::allocator)"
      << "\n    std::sort: " << larr_std_std_time
      << "\n    custom::heap_sort: " << larr_std_my_time
      << "\n  linarray (abc_allocator)"
      << "\n    std::sort: " << larr_abc_std_time
      << "\n    custom::heap_sort: " << larr_abc_my_time
      << "\n" << std::endl;
  }

  /* ====================================================================== */

  SECTION( "random order, ascending" ) {
    t_vector test_vec(vec_shuffled);
    t_linarray_std larr_std(larrstd_shuffled);
    t_linarray_abc larr_abc(larrabc_shuffled);

    // test_vec
    const auto test_vec_std_time = time_measure::execution(
      std::sort<t_vector::iterator>,
      test_vec.begin(), test_vec.end()
    );
    test_vec = vec_shuffled;
    const auto test_vec_my_time = time_measure::execution(
      custom::heap_sort<t_vector::iterator>,
      test_vec.begin(), test_vec.end()
    );

    // larr_std
    const auto larr_std_std_time = time_measure::execution(
      std::sort<t_linarray_std::iterator>,
      larr_std.begin(), larr_std.end()
    );
    larr_std = larrstd_shuffled;
    const auto larr_std_my_time = time_measure::execution(
      custom::heap_sort<t_linarray_std::iterator>,
      larr_std.begin(), larr_std.end()
    );

    // larr_abc
    const auto larr_abc_std_time = time_measure::execution(
      std::sort<t_linarray_abc::iterator>,
      larr_abc.begin(), larr_abc.end()
    );
    larr_abc = larrabc_shuffled;
    const auto larr_abc_my_time = time_measure::execution(
      custom::heap_sort<t_linarray_abc::iterator>,
      larr_abc.begin(), larr_abc.end()
    );

    REQUIRE( IS_EQUAL_CONTAINERS( larr_std, test_vec ) );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_abc, test_vec ) );

    std::cout << "random order, ascending:"
      << "\n  std::vector"
      << "\n    std::sort: " << test_vec_std_time
      << "\n    custom::heap_sort: " << test_vec_my_time
      << "\n  linarray (std::allocator)"
      << "\n    std::sort: " << larr_std_std_time
      << "\n    custom::heap_sort: " << larr_std_my_time
      << "\n  linarray (abc_allocator)"
      << "\n    std::sort: " << larr_abc_std_time
      << "\n    custom::heap_sort: " << larr_abc_my_time
      << "\n" << std::endl;
  }
  SECTION( "random order, descending" ) {
    t_vector test_vec(vec_shuffled);
    t_linarray_std larr_std(larrstd_shuffled);
    t_linarray_abc larr_abc(larrabc_shuffled);

    // test_vec
    const auto test_vec_std_time = time_measure::execution(
      std::sort<t_vector::reverse_iterator>,
      test_vec.rbegin(), test_vec.rend()
    );
    test_vec = vec_shuffled;
    const auto test_vec_my_time = time_measure::execution(
      custom::heap_sort<t_vector::reverse_iterator>,
      test_vec.rbegin(), test_vec.rend()
    );

    // larr_std
    const auto larr_std_std_time = time_measure::execution(
      std::sort<t_linarray_std::reverse_iterator>,
      larr_std.rbegin(), larr_std.rend()
    );
    larr_std = larrstd_shuffled;
    const auto larr_std_my_time = time_measure::execution(
      custom::heap_sort<t_linarray_std::reverse_iterator>,
      larr_std.rbegin(), larr_std.rend()
    );

    // larr_abc
    const auto larr_abc_std_time = time_measure::execution(
      std::sort<t_linarray_abc::reverse_iterator>,
      larr_abc.rbegin(), larr_abc.rend()
    );
    larr_abc = larrabc_shuffled;
    const auto larr_abc_my_time = time_measure::execution(
      custom::heap_sort<t_linarray_abc::reverse_iterator>,
      larr_abc.rbegin(), larr_abc.rend()
    );

    REQUIRE( IS_EQUAL_CONTAINERS( larr_std, test_vec ) );
    REQUIRE( IS_EQUAL_CONTAINERS( larr_abc, test_vec ) );

    std::cout << "random order, descending:"
      << "\n  std::vector"
      << "\n    std::sort: " << test_vec_std_time
      << "\n    custom::heap_sort: " << test_vec_my_time
      << "\n  linarray (std::allocator)"
      << "\n    std::sort: " << larr_std_std_time
      << "\n    custom::heap_sort: " << larr_std_my_time
      << "\n  linarray (abc_allocator)"
      << "\n    std::sort: " << larr_abc_std_time
      << "\n    custom::heap_sort: " << larr_abc_my_time
      << "\n" << std::endl;
  }
}


