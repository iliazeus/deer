// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#ifndef DEER_SPECTRUM_H_
#define DEER_SPECTRUM_H_

#include <cmath>
#include <memory>

namespace deer {

struct Spectrum {
  virtual double intensity(double wavelength) const = 0;
  double operator()(double wavelength) const { return intensity(wavelength); }

  virtual ~Spectrum() {}

  static std::shared_ptr<Spectrum> sum(
      std::shared_ptr<const Spectrum> a,
      std::shared_ptr<const Spectrum> b);
  static std::shared_ptr<Spectrum> difference(
      std::shared_ptr<const Spectrum> a,
      std::shared_ptr<const Spectrum> b);
  static std::shared_ptr<Spectrum> product(
      std::shared_ptr<const Spectrum> a,
      std::shared_ptr<const Spectrum> b);
};

struct ConstantSpectrum : public Spectrum {
  double value = 0;
  double intensity(double wavelength) const override { return value; }

  explicit ConstantSpectrum(double v) : value(v) {}
};

struct MonochromeSpectrum : public Spectrum {
  double peak_wavelength = 0;
  double peak_width = 0;
  double peak_height = 0;
  double intensity(double wavelength) const override {
    if (std::abs(wavelength - peak_wavelength) <= peak_width / 2) {
      return peak_height;
    } else {
      return 0;
    }
  }
};

}  // namespace deer

#endif  // DEER_SPECTRUM_H_
