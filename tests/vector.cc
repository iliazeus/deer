// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license
// See the LICENSE.txt file for details.

#include "../src/vector.h"

#include <gtest/gtest.h>

class VectorTest : public ::testing::Test {};

TEST_F(VectorTest, IsContainer) {
  deer::int3 v{1, 2, 3};
  EXPECT_EQ(v.size(), 3ull);
  EXPECT_EQ(v[1], 2);
  {
    auto it = v.begin();
    ++it; ++it; ++it;
    EXPECT_EQ(it, v.end());
  }
}

TEST_F(VectorTest, DoesArithmetic) {
  deer::int3 v{1, 2, 3};
  deer::int3 w{0, 1, -1};
  {
    auto expected = deer::int3{1, 3, 2};
    EXPECT_EQ(v + w, expected);
  }
  {
    auto expected = deer::int3{1, 1, 4};
    EXPECT_EQ(v - w, expected);
  }
  {
    auto expected = deer::int3{0, 2, -3};
    EXPECT_EQ(v * w, expected);
  }
}

TEST_F(VectorTest, DoesVectorOps) {
  deer::int3 v{1, 2, 3};
  deer::int3 w{0, 1, -1};
  {
    auto expected = deer::int3{2, 4, 6};
    EXPECT_EQ(v * 2, expected);
  }
  {
    auto expected = deer::int3{0, 2, -2};
    EXPECT_EQ(2 * w, expected);
  }
  {
    auto expected = deer::int3{0, 1, 1};
    EXPECT_EQ(v / 2, expected);
  }
  EXPECT_EQ(v.length2(), 14);
  {
    auto actual = dot(v, w);
    EXPECT_EQ(actual, -1);
  }
  {
    auto expected = deer::int3{-5, 1, 1};
    EXPECT_EQ(deer::cross(v, w), expected);
  }
}
