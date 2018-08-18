// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license
// See the LICENSE.txt file for details.

#include "../src/vector.h"

#include <gtest/gtest.h>

namespace deer {

namespace test {

class VectorTest : public ::testing::Test {};

TEST_F(VectorTest, IsContainer) {
  int3 v{1, 2, 3};
  EXPECT_EQ(v.size(), 3ull);
  EXPECT_EQ(v[1], 2);
  {
    auto it = v.begin();
    ++it; ++it; ++it;
    EXPECT_EQ(it, v.end());
  }
}

TEST_F(VectorTest, DoesArithmetic) {
  int3 v{1, 2, 3};
  int3 w{0, 1, -1};
  {
    auto expected = int3{1, 3, 2};
    EXPECT_EQ(v + w, expected);
  }
  {
    auto expected = int3{1, 1, 4};
    EXPECT_EQ(v - w, expected);
  }
  {
    auto expected = int3{0, 2, -3};
    EXPECT_EQ(v * w, expected);
  }
}

TEST_F(VectorTest, DoesVectorOps) {
  int3 v{1, 2, 3};
  int3 w{0, 1, -1};
  {
    auto expected = int3{2, 4, 6};
    EXPECT_EQ(v * 2, expected);
  }
  {
    auto expected = int3{0, 2, -2};
    EXPECT_EQ(2 * w, expected);
  }
  {
    auto expected = int3{0, 1, 1};
    EXPECT_EQ(v / 2, expected);
  }
  EXPECT_EQ(v.length2(), 14);
  {
    auto actual = dot(v, w);
    EXPECT_EQ(actual, -1);
  }
  {
    auto expected = int3{-5, 1, 1};
    EXPECT_EQ(cross(v, w), expected);
  }
}

TEST_F(VectorTest, DoesProjectReflect) {
  int2 v{2, 2};
  int2 w{0, -1};
  {
    auto expected = int2{0, 2};
    EXPECT_EQ(v.project_onto(w), expected);
  }
  {
    auto expected = int2{2, -2};
    EXPECT_EQ(v.reflect_off(w), expected);
  }
}

}  // namespace test

}  // namespace deer
