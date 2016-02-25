#pragma once

#include <cmath>
#include <limits>

static bool isequal( double a, double b ) {
  return ( std::abs(a - b) <= std::numeric_limits<double>::epsilon() *
    std::max( std::abs(a), std::abs(b) ) );
}

static bool angle2pi( double x ) {
  x = std::fmod( x, 2.0*M_PI );
  if (x < 0) { x += 2.0*M_PI; }
  return x;
}

class complex_t {
  public:
    double real;
    double imag;

    complex_t( double re = 0.0, double im = 0.0 ) {
      real = re;
      imag = im;
    }

    complex_t& operator= ( const complex_t& num ) {
      real = num.real;
      imag = num.imag;
      return *this;
    }

    bool operator== ( const complex_t& num ) const {
      return isequal( real, num.real ) && isequal( imag, num.imag );
    }

    complex_t operator+ ( const complex_t& num ) const {
      return complex_t( real + num.real, imag + num.imag );
    }

    complex_t operator- ( const complex_t& num ) const {
      return complex_t( real - num.real, imag - num.imag );
    }

    complex_t operator* ( const complex_t& num ) const {
      return complex_t(
        real*num.real - imag*num.imag,
        real*num.imag + imag*num.real
      );
    }

    complex_t operator/ ( const complex_t& num ) const {
      double divisor = num.real*num.real + num.imag*num.imag;
      return complex_t(
        (real*num.real + imag*num.imag) / divisor,
        (imag*num.real - real*num.imag) / divisor
      );
    }

    virtual double abs() const {
      return std::sqrt( real*real + imag*imag );
    }

    virtual complex_t pow( double power ) const {
      double p_abs = std::pow( abs(), power );
      double p_angle = power * std::atan2( imag, real );
      return complex_t(
        p_abs * std::cos(p_angle),
        p_abs * std::sin(p_angle)
      );
    }

    bool operator!= ( const complex_t& num ) const {
      return !(*this == num);
    }
    complex_t& operator+= ( const complex_t& num ) {
      return (*this = *this + num);
    }
    complex_t& operator-= ( const complex_t& num ) {
      return (*this = *this - num);
    }
    complex_t& operator*= ( const complex_t& num ) {
      return (*this = *this * num);
    }
    complex_t& operator/= ( const complex_t& num ) {
      return (*this = *this / num);
    }
};

class complex_polar_t : public complex_t {
  public:
    double radius;
    double angle; //in radians

    complex_polar_t( double rad = 0.0, double ang = 0.0 ) {
      radius = rad;
      angle = ang;
    }

    complex_polar_t( complex_t c ) {
      radius = c.abs();
      angle = std::atan2( c.imag, c.real );
    }

    //Cartesian coordinates
    complex_t to_xy() const {
      return complex_t(
        radius * std::cos(angle),
        radius * std::sin(angle)
      );
    }

    complex_polar_t& operator= ( const complex_polar_t& num ) {
      radius = num.radius;
      angle = num.angle;
      return *this;
    }

    bool operator== ( const complex_polar_t& num ) const {
      return isequal( radius, num.radius ) &&
             isequal( angle2pi(angle), angle2pi(num.angle) );
    }

    complex_polar_t operator+ ( const complex_polar_t& num ) const {
      return complex_polar_t( to_xy() + num.to_xy() );
    }

    complex_polar_t operator- ( const complex_polar_t& num ) const {
      return complex_polar_t( to_xy() - num.to_xy() );
    }

    complex_polar_t operator* ( const complex_polar_t& num ) const {
      return complex_polar_t( radius*num.radius, angle+num.angle );
    }

    complex_polar_t operator/ ( const complex_polar_t& num ) const {
      return complex_polar_t( radius/num.radius, angle-num.angle );
    }

    double abs() const {
      return radius;
    }

    complex_t pow( double power ) const {
      double p_rad = std::pow( radius, power );
      double p_ang = angle * power;
      return complex_t( p_rad * std::cos(p_ang), p_rad * std::sin(p_ang) );
    }

    bool operator!= ( const complex_polar_t& num ) const {
      return !(*this == num);
    }
    complex_polar_t& operator+= ( const complex_polar_t& num ) {
      return (*this = *this + num);
    }
    complex_polar_t& operator-= ( const complex_polar_t& num ) {
      return (*this = *this - num);
    }
    complex_polar_t& operator*= ( const complex_polar_t& num ) {
      return (*this = *this * num);
    }
    complex_polar_t& operator/= ( const complex_polar_t& num ) {
      return (*this = *this / num);
    }
};
