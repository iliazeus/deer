// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#include "rgb.h"

#include <algorithm>

#include "spectrum.h"

namespace deer {

Spectrum RgbColorProfile::FromRgb(double3 rgb) const {
  auto g_width = std::min(
      wavelengths.r() - wavelengths.g(), wavelengths.g() - wavelengths.b());
  auto r_width = 2 * (wavelengths.r() - wavelengths.g()) - g_width;
  auto b_width = 2 * (wavelengths.g() - wavelengths.b()) - g_width;
  auto r_sp = Spectrum::MakeMonochrome(wavelengths.r(), r_width, rgb.r());
  auto g_sp = Spectrum::MakeMonochrome(wavelengths.g(), g_width, rgb.g());
  auto b_sp = Spectrum::MakeMonochrome(wavelengths.b(), b_width, rgb.b());
  return r_sp + g_sp + b_sp;
}

}  // namespace deer
