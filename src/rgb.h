// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#ifndef DEER_RGB_H_
#define DEER_RGB_H_

#include <algorithm>
#include <cstdint>

#include "spectrum.h"
#include "vector.h"

namespace deer {

struct RgbColorProfile {
  double3 wavelengths;
  double3 min_intensities;
  double3 max_intensities;

  Spectrum FromRgb(double3 rgb) const;
  Spectrum FromRgbBytes(byte3 rgb) const {
    return FromRgb(double3{rgb.r()/255.0, rgb.g()/255.0, rgb.b()/255.0});
  }

  double3 ToRgb(const Spectrum &spectrum) const {
    return (spectrum(wavelengths) - min_intensities)
        .clamp(min_intensities, max_intensities)
        / (max_intensities - min_intensities);
  }
  byte3 ToRgbBytes(double3 rgb) const {
    return byte3{
      static_cast<std::uint8_t>(rgb.r() * 255),
      static_cast<std::uint8_t>(rgb.g() * 255),
      static_cast<std::uint8_t>(rgb.b() * 255)
    };
  }
  byte3 ToRgbBytes(const Spectrum &spectrum) const {
    return ToRgbBytes(ToRgb(spectrum));
  }
};

}  // namespace deer

#endif  // DEER_RGB_H_
