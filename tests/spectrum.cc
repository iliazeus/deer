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
  auto const_sp = Spectrum::MakeConstant(10);

  // [8; 12] -> 1
  auto mono_sp1 = Spectrum::MakeMonochrome(10, 4, 3);

  // [9; 13] -> 2
  auto mono_sp2 = Spectrum::MakeMonochrome(11, 4, 2);

  auto sp1 = mono_sp1 + mono_sp2;
  EXPECT_EQ(sp1(8), 3);
  EXPECT_EQ(sp1(10), 5);
  EXPECT_EQ(sp1(15), 0);

  auto sp2 = const_sp - sp1;
  EXPECT_EQ(sp2(8), 7);
  EXPECT_EQ(sp2(10), 5);
  EXPECT_EQ(sp2(15), 10);
}

}  // namespace test

}  // namespace deer
