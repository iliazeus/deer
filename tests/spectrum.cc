// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#include "../src/spectrum.h"

#include <gtest/gtest.h>

#include <memory>

namespace deer {

namespace test {

class SpectrumTest : public ::testing::Test {};

TEST_F(SpectrumTest, DoesArithmetic) {
  auto const_sp = std::make_shared<ConstantSpectrum>(10);

  // [8; 12] -> 1
  auto mono_sp1 = std::make_shared<MonochromeSpectrum>();
  mono_sp1->peak_wavelength = 10;
  mono_sp1->peak_width = 4;
  mono_sp1->peak_height = 3;

  // [9; 13] -> 2
  auto mono_sp2 = std::make_shared<MonochromeSpectrum>();
  mono_sp2->peak_wavelength = 11;
  mono_sp2->peak_width = 4;
  mono_sp2->peak_height = 2;

  auto sp1 = Spectrum::sum(mono_sp1, mono_sp2);
  EXPECT_EQ(sp1->intensity(8), 3);
  EXPECT_EQ(sp1->intensity(10), 5);
  EXPECT_EQ(sp1->intensity(15), 0);

  auto sp2 = Spectrum::difference(const_sp, sp1);
  EXPECT_EQ(sp2->intensity(8), 7);
  EXPECT_EQ(sp2->intensity(10), 5);
  EXPECT_EQ(sp2->intensity(15), 10);
}

}  // namespace test

}  // namespace deer
