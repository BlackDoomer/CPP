#define _USE_MATH_DEFINES
#include <cmath>

#ifndef M_PI
  #define M_PI  3.14159265358979323846
#endif

#include <complex>
#include "complex_t.hpp"

#include "catch/catch_with_main.hpp"

const double POWER_EXP = 16.73;

const double CMPL_XY_REAL1 = 15.0;
const double CMPL_XY_IMAG1 = 42.0;
const double CMPL_XY_REAL2 = 8.0;
const double CMPL_XY_IMAG2 = 1.0;

const double CMPL_PL_RAD1 = std::sqrt( 3.0*3.0 + 4.0*4.0 );
const double CMPL_PL_ANG1 = M_PI / 4.0;
const double CMPL_PL_RAD2 = std::sqrt( 23.0*23.0 + 1.0*1.0 );
const double CMPL_PL_ANG2 = -M_PI / 2.0;

const std::complex<double> cpp_cmpl_xy1 ( CMPL_XY_REAL1, CMPL_XY_IMAG1 );
const std::complex<double> cpp_cmpl_xy2 ( CMPL_XY_REAL2, CMPL_XY_IMAG2 );
const complex_t my_cmpl_xy1 ( CMPL_XY_REAL1, CMPL_XY_IMAG1 );
const complex_t my_cmpl_xy2 ( CMPL_XY_REAL2, CMPL_XY_IMAG2 );

const std::complex<double>
  cpp_cmpl_pl1 = std::polar( CMPL_PL_RAD1, CMPL_PL_ANG1 );
const std::complex<double>
  cpp_cmpl_pl2 = std::polar( CMPL_PL_RAD2, CMPL_PL_ANG2 );
const complex_polar_t my_cmpl_pl1 ( CMPL_PL_RAD1, CMPL_PL_ANG1 );
const complex_polar_t my_cmpl_pl2 ( CMPL_PL_RAD2, CMPL_PL_ANG2 );

/* ========================================================================== */

inline void REQUIRE_CMPL_EQUAL( double right_real, double right_imag, complex_t actual ) {
  REQUIRE( right_real == Approx( actual.real ) );
  REQUIRE( right_imag == Approx( actual.imag ) );
}

inline void REQUIRE_CMPL_EQUAL( double right_rad, double right_ang, complex_polar_t actual ) {
  REQUIRE( right_rad == Approx( actual.radius ) );
  REQUIRE( sin(right_ang) == Approx( sin(actual.angle) ) );
  REQUIRE( cos(right_ang) == Approx( cos(actual.angle) ) );
}

inline void REQUIRE_CMPL_EQUAL( std::complex<double> expected, complex_t actual ) {
  REQUIRE( expected.real() == Approx( actual.real ) );
  REQUIRE( expected.imag() == Approx( actual.imag ) );
}

inline void REQUIRE_CMPL_EQUAL( std::complex<double> expected, complex_polar_t actual ) {
  std::complex<double> c = std::polar( actual.radius, actual.angle );
  REQUIRE( expected.real() == Approx( c.real() ) );
  REQUIRE( expected.imag() == Approx( c.imag() ) );
}

/* ========================================================================== */
/* ========================================================================== */

TEST_CASE( "complex_t constructor", "[init]" ) {
  SECTION( "default values: 0,0" ) {
    complex_t c;
    REQUIRE_CMPL_EQUAL( 0.0, 0.0, c );
  }
  SECTION( "real value without imaginary" ) {
    complex_t c( CMPL_XY_REAL1 );
    REQUIRE_CMPL_EQUAL( CMPL_XY_REAL1, 0.0, c );
  }
  SECTION( "both values" ) {
    complex_t c( CMPL_XY_REAL1, CMPL_XY_IMAG1 );
    REQUIRE_CMPL_EQUAL( CMPL_XY_REAL1, CMPL_XY_IMAG1, c );
  }
}

TEST_CASE( "complex_polar_t constructor", "[init]" ) {
  SECTION( "default values: 0,0" ) {
    complex_polar_t c;
    REQUIRE_CMPL_EQUAL( 0.0, 0.0, c );
  }
  SECTION( "radius without angle" ) {
    complex_polar_t c( CMPL_PL_RAD1 );
    REQUIRE_CMPL_EQUAL( CMPL_PL_RAD1, 0.0, c );
  }
  SECTION( "both values" ) {
    complex_polar_t c( CMPL_PL_RAD1, CMPL_PL_ANG1 );
    REQUIRE_CMPL_EQUAL( CMPL_PL_RAD1, CMPL_PL_ANG1, c );
  }
}

/* ========================================================================== */

TEST_CASE( "complex_t assignment operator", "[init]" ) {
  complex_t c = my_cmpl_xy1;
  REQUIRE_CMPL_EQUAL( c.real, c.imag, my_cmpl_xy1 );
}

TEST_CASE( "complex_polar_t assignment operator", "[init]" ) {
  complex_polar_t c = my_cmpl_pl1;
  REQUIRE_CMPL_EQUAL( c.radius, c.angle, my_cmpl_pl1 );
}

/* ========================================================================== */

TEST_CASE( "complex_t equality operators", "[operator]" ) {
  SECTION( "equal" ) {
    complex_t c = my_cmpl_xy1;
    REQUIRE( c == my_cmpl_xy1 );
  }
  SECTION( "not equal" ) {
    complex_t c = my_cmpl_xy1;
    REQUIRE( c != my_cmpl_xy2 );
  }
}

TEST_CASE( "complex_polar_t equality operators", "[operator]" ) {
  SECTION( "equal" ) {
    complex_polar_t c = my_cmpl_pl1;
    c.angle -= 2.0 * M_PI;
    REQUIRE( c == my_cmpl_pl1 );
  }
  SECTION( "not equal" ) {
    complex_polar_t c = my_cmpl_pl1;
    REQUIRE( c != my_cmpl_pl2 );
  }
}

/* ========================================================================== */

TEST_CASE( "complex_t arithmetic operators", "[operator]" ) {
  SECTION( "add (+)" ) {
    complex_t c = my_cmpl_xy1;
    c += my_cmpl_xy2;
    REQUIRE_CMPL_EQUAL( cpp_cmpl_xy1 + cpp_cmpl_xy2, c );
  }
  SECTION( "subtract (-)" ) {
    complex_t c = my_cmpl_xy1;
    c -= my_cmpl_xy2;
    REQUIRE_CMPL_EQUAL( cpp_cmpl_xy1 - cpp_cmpl_xy2, c );
  }
  SECTION( "multiply (*)" ) {
    complex_t c = my_cmpl_xy1;
    c *= my_cmpl_xy2;
    REQUIRE_CMPL_EQUAL( cpp_cmpl_xy1 * cpp_cmpl_xy2, c );
  }
  SECTION( "divide (/)" ) {
    complex_t c = my_cmpl_xy1;
    c /= my_cmpl_xy2;
    REQUIRE_CMPL_EQUAL( cpp_cmpl_xy1 / cpp_cmpl_xy2, c );
  }
}

TEST_CASE( "complex_polar_t arithmetic operators", "[operator]" ) {
  SECTION( "add (+)" ) {
    complex_polar_t c = my_cmpl_pl1;
    c += my_cmpl_pl2;
    REQUIRE_CMPL_EQUAL( cpp_cmpl_pl1 + cpp_cmpl_pl2, c.to_xy() );
  }
  SECTION( "subtract (-)" ) {
    complex_polar_t c = my_cmpl_pl1;
    c -= my_cmpl_pl2;
    REQUIRE_CMPL_EQUAL( cpp_cmpl_pl1 - cpp_cmpl_pl2, c.to_xy() );
  }
  SECTION( "multiply (*)" ) {
    complex_polar_t c = my_cmpl_pl1;
    c *= my_cmpl_pl2;
    REQUIRE_CMPL_EQUAL( cpp_cmpl_pl1 * cpp_cmpl_pl2, c.to_xy() );
  }
  SECTION( "divide (/)" ) {
    complex_polar_t c = my_cmpl_pl1;
    c /= my_cmpl_pl2;
    REQUIRE_CMPL_EQUAL( cpp_cmpl_pl1 / cpp_cmpl_pl2, c.to_xy() );
  }
}

/* ========================================================================== */

TEST_CASE( "complex_t functions", "[functions]" ) {
  SECTION( "abs" ) {
    REQUIRE( std::abs(cpp_cmpl_xy1) == Approx( my_cmpl_xy1.abs() ) );
    REQUIRE( std::abs(cpp_cmpl_xy2) == Approx( my_cmpl_xy2.abs() ) );
  }
  SECTION( "pow" ) {
    REQUIRE_CMPL_EQUAL(
      std::pow(cpp_cmpl_xy1, POWER_EXP), my_cmpl_xy1.pow(POWER_EXP)
    );
    REQUIRE_CMPL_EQUAL(
      std::pow(cpp_cmpl_xy2, POWER_EXP), my_cmpl_xy2.pow(POWER_EXP)
    );
  }
}

TEST_CASE( "complex_polar_t functions", "[functions]" ) {
  SECTION( "abs" ) {
    REQUIRE( std::abs(cpp_cmpl_pl1) == Approx( my_cmpl_pl1.abs() ) );
    REQUIRE( std::abs(cpp_cmpl_pl2) == Approx( my_cmpl_pl2.abs() ) );
  }
  SECTION( "pow" ) {
    REQUIRE_CMPL_EQUAL(
      std::pow(cpp_cmpl_pl1, POWER_EXP), my_cmpl_pl1.pow(POWER_EXP)
    );
    REQUIRE_CMPL_EQUAL(
      std::pow(cpp_cmpl_pl2, POWER_EXP), my_cmpl_pl2.pow(POWER_EXP)
    );
  }
  SECTION( "to_xy" ) {
    REQUIRE_CMPL_EQUAL( cpp_cmpl_pl1, my_cmpl_pl1.to_xy() );
    REQUIRE_CMPL_EQUAL( cpp_cmpl_pl2, my_cmpl_pl2.to_xy() );
  }
}


