// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#include "spectrum.h"

#include <functional>
#include <memory>

namespace deer {

namespace {

struct LazyCompositeSpectrum : public Spectrum {
  std::function<double(double, double)> func;
  std::shared_ptr<const Spectrum> left, right;

  LazyCompositeSpectrum(std::function<double(double, double)> f,
      std::shared_ptr<const Spectrum> l,
      std::shared_ptr<const Spectrum> r)
      : func(f), left(l), right(r) {}

  double intensity(double freq) const override {
    return func(left->intensity(freq), right->intensity(freq));
  }
};

}  // namespace

std::shared_ptr<Spectrum> Spectrum::sum(
    std::shared_ptr<const Spectrum> a,
    std::shared_ptr<const Spectrum> b) {
  return std::make_shared<LazyCompositeSpectrum>(
      std::plus<double>(), a, b);
}

std::shared_ptr<Spectrum> Spectrum::difference(
    std::shared_ptr<const Spectrum> a,
    std::shared_ptr<const Spectrum> b) {
  return std::make_shared<LazyCompositeSpectrum>(
      std::minus<double>(), a, b);
}

std::shared_ptr<Spectrum> Spectrum::product(
    std::shared_ptr<const Spectrum> a,
    std::shared_ptr<const Spectrum> b) {
  return std::make_shared<LazyCompositeSpectrum>(
      std::multiplies<double>(), a, b);
}

}  // namespace deer
