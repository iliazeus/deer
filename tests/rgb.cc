// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#include "../src/rgb.h"

#include <cstdint>

#include <gtest/gtest.h>

#include "../src/spectrum.h"
#include "../src/vector.h"

namespace deer {

namespace test {

class RgbColorProfileTest : public ::testing::Test {};

TEST_F(RgbColorProfileTest, ToRgbWorks) {
  RgbColorProfile profile;
  profile.wavelengths = double3{2, 1, 0};
  profile.min_intensities = double3{0, 0, 0};
  profile.max_intensities = double3{1, 1, 1};

  auto red_spectrum = Spectrum::MakeMonochrome(2, 1, 1);
  auto white_spectrum = Spectrum::MakeConstant(1);
  auto yellow_spectrum = white_spectrum - red_spectrum;

  auto red_bytes = byte3{255, 0, 0};
  auto white_bytes = byte3{255, 255, 255};
  auto yellow_bytes = byte3{0, 255, 255};

  EXPECT_EQ(profile.ToRgbBytes(red_spectrum), red_bytes);
  EXPECT_EQ(profile.ToRgbBytes(white_spectrum), white_bytes);
  EXPECT_EQ(profile.ToRgbBytes(yellow_spectrum), yellow_bytes);
}

TEST_F(RgbColorProfileTest, FromRgbWorks) {
  RgbColorProfile profile;
  profile.wavelengths = double3{2, 1, 0};
  profile.min_intensities = double3{0, 0, 0};
  profile.max_intensities = double3{1, 1, 1};

  auto bytes = byte3{123, 45, 67};
  EXPECT_EQ(profile.ToRgbBytes(profile.FromRgbBytes(bytes)), bytes);
}

}  // namespace test

}  // namespace deer
