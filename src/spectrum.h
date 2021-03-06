// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#ifndef DEER_SPECTRUM_H_
#define DEER_SPECTRUM_H_

#include <cmath>
#include <memory>

#include "vector.h"

namespace deer {

class Spectrum {
 public:
  virtual double intensity(double wavelength) const {
    return pimpl_->intensity(wavelength);
  }
  double operator()(double wavelength) const {
    return intensity(wavelength);
  }
  double3 operator()(double3 wavelengths) const {
    return double3{
      intensity(wavelengths.r()),
      intensity(wavelengths.g()),
      intensity(wavelengths.b())
    };
  }

  Spectrum() : pimpl_(nullptr) {}
  virtual ~Spectrum() = default;

  operator bool() const { return bool{pimpl_}; }
  bool operator!() const { return !pimpl_; }

  static Spectrum MakeConstant(double value);
  static Spectrum MakeMonochrome(double wavelength,
      double width, double value);

  friend Spectrum operator+(const Spectrum &, const Spectrum &);
  friend Spectrum operator-(const Spectrum &, const Spectrum &);
  friend Spectrum operator*(const Spectrum &, const Spectrum &);

  Spectrum &operator+=(const Spectrum &other) { return *this = *this + other; }
  Spectrum &operator-=(const Spectrum &other) { return *this = *this - other; }
  Spectrum &operator*=(const Spectrum &other) { return *this = *this * other; }

  friend Spectrum operator*(const Spectrum &, const double &);
  friend Spectrum operator*(const double &d, const Spectrum &sp) {
    return sp * d;
  }
  Spectrum &operator*=(const double &d) { return *this = *this * d; }

 protected:
  explicit Spectrum(std::shared_ptr<Spectrum> pimpl) : pimpl_(pimpl) {}

 private:
  std::shared_ptr<Spectrum> pimpl_;
};

}  // namespace deer

#endif  // DEER_SPECTRUM_H_
