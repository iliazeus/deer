// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#include "spectrum.h"

#include <functional>
#include <memory>

namespace deer {

namespace {

struct ConstantSpectrum : public Spectrum {
  double value = 0;
  double intensity(double wavelength) const override { return value; }
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

template<class Op>
struct BinaryOperatorSpectrum : public Spectrum {
  Spectrum left, right;
  double intensity(double wavelength) const override {
    return Op()(left(wavelength), right(wavelength));
  }
};

struct MultipliedSpectrum : public Spectrum {
  Spectrum spectrum;
  double times;
  double intensity(double wavelength) const override {
    return spectrum(wavelength) * times;
  }
};

}  // namespace

Spectrum Spectrum::MakeConstant(double value) {
  ConstantSpectrum impl;
  impl.value = value;
  return Spectrum(std::make_shared<decltype(impl)>(impl));
}

Spectrum Spectrum::MakeMonochrome(double wl, double width, double height) {
  MonochromeSpectrum impl;
  impl.peak_wavelength = wl;
  impl.peak_width = width;
  impl.peak_height = height;
  return Spectrum(std::make_shared<decltype(impl)>(impl));
}

Spectrum operator+(const Spectrum &a, const Spectrum &b) {
  BinaryOperatorSpectrum<std::plus<double>> impl;
  impl.left = a;
  impl.right = b;
  return Spectrum(std::make_shared<decltype(impl)>(impl));
}

Spectrum operator-(const Spectrum &a, const Spectrum &b) {
  BinaryOperatorSpectrum<std::minus<double>> impl;
  impl.left = a;
  impl.right = b;
  return Spectrum(std::make_shared<decltype(impl)>(impl));
}

Spectrum operator*(const Spectrum &a, const Spectrum &b) {
  BinaryOperatorSpectrum<std::multiplies<double>> impl;
  impl.left = a;
  impl.right = b;
  return Spectrum(std::make_shared<decltype(impl)>(impl));
}

Spectrum operator*(const Spectrum &sp, const double &d) {
  MultipliedSpectrum impl;
  impl.spectrum = sp;
  impl.times = d;
  return Spectrum(std::make_shared<decltype(impl)>(impl));
}

}  // namespace deer
