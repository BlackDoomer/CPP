#pragma once

#include <cmath>
#include <limits>

class complex_t {
  protected:
    bool isequal( double a, double b ) {
      return ( std::abs(a - b) <= std::numeric_limits<double>::epsilon() *
        std::max( std::abs(a), std::abs(b) ) );
    }

  public:
    double real;
    double imag;

    complex_t( double re = 0.0, double im = 0.0 ) {
      real = re;
      imag = im;
    }

    virtual complex_t& operator= ( const complex_t& num ) {
      real = num.real;
      imag = num.imag;
      return *this;
    }

    virtual bool operator== ( const complex_t& num ) {
      return isequal( real, num.real ) && isequal( imag, num.imag );
    }

    virtual complex_t operator+ ( const complex_t& num ) {
      return complex_t( real + num.real, imag + num.imag );
    }

    virtual complex_t operator- ( const complex_t& num ) {
      return complex_t( real - num.real, imag - num.imag );
    }

    virtual complex_t operator* ( const complex_t& num ) {
      return complex_t(
        real*num.real - imag*num.imag,
        real*num.imag + imag*num.real
      );
    }

    virtual complex_t operator/ ( const complex_t& num ) {
      double divisor = num.real*num.real + num.imag*num.imag;
      return complex_t(
        (real*num.real + imag*num.imag) / divisor,
        (imag*num.real - real*num.imag) / divisor
      );
    }

    virtual double abs() {
      return std::sqrt( real*real + imag*imag );
    }

    virtual complex_t pow( double power ) {
      double p_abs = std::pow( abs(), power );
      double p_angle = power * std::atan2( imag, real );
      return complex_t(
        p_abs * std::cos(p_angle),
        p_abs * std::sin(p_angle)
      );
    }

    bool operator!= ( const complex_t& num ) {
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
    complex_t to_xy() {
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

    bool operator== ( const complex_polar_t& num ) {
      return isequal( radius, num.radius ) &&
             isequal( std::sin(angle), std::sin(num.angle) ) &&
             isequal( std::cos(angle), std::cos(num.angle) );
    }

    complex_polar_t operator+ ( const complex_polar_t& num ) {
      return complex_polar_t( to_xy() + const_cast<complex_polar_t&>(num).to_xy() );
    }

    complex_polar_t operator- ( const complex_polar_t& num ) {
      return complex_polar_t( to_xy() - const_cast<complex_polar_t&>(num).to_xy() );
    }

    complex_polar_t operator* ( const complex_polar_t& num ) {
      return complex_polar_t( radius*num.radius, angle+num.angle );
    }

    complex_polar_t operator/ ( const complex_polar_t& num ) {
      return complex_polar_t( radius/num.radius, angle-num.angle );
    }

    double abs() {
      return radius;
    }

    complex_t pow( double power ) {
      double p_rad = std::pow( radius, power );
      double p_ang = angle * power;
      return complex_polar_t(
        p_rad * std::cos(p_ang),
        p_rad * std::sin(p_ang)
      );
    }
};
