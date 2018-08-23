// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#include "../src/matrix.h"

#include <gtest/gtest.h>

namespace deer {

namespace test {

class MatrixTest : public ::testing::Test {};

TEST_F(MatrixTest, IsContainer) {
  int3x3 m = {
    int3{1, 2, 3},
    int3{4, 5, 6},
    int3{7, 8, 9},
  };
  EXPECT_EQ(m.size(), 3u);
  {
    auto expected = int3{4, 5, 6};
    EXPECT_EQ(m[1], expected);
  }
  {
    auto it = m.begin();
    EXPECT_EQ(*it, m[0]);
    ++it;
    EXPECT_EQ(*it, m[1]);
    ++it;
    EXPECT_EQ(*it, m[2]);
    ++it;
    EXPECT_EQ(it, m.end());
  }
}

TEST_F(MatrixTest, DoesArithmetic) {
  int2x2 m1 = {int2{1, 2}, int2{3, 4}};
  int2x2 m2 = int2x2::id();
  {
    auto expected = int2x2{int2{2, 2}, int2{3, 5}};
    EXPECT_EQ(m1 + m2, expected);
  }
  {
    auto expected = int2x2{int2{0, 2}, int2{3, 3}};
    EXPECT_EQ(m1 - m2, expected);
  }
  {
    auto expected = int2x2{int2{3, 6}, int2{9, 12}};
    EXPECT_EQ(m1 * 3, expected);
    EXPECT_EQ(3 * m1, expected);
  }
  {
    auto expected = int2x2{int2{-1, -2}, int2{-3, -4}};
    EXPECT_EQ(-1 * m1, expected);
    EXPECT_EQ(-m1, expected);
  }
  {
    auto expected = int2x2{int2{0, 1}, int2{1, 2}};
    EXPECT_EQ(m1 / 2, expected);
  }
}

TEST_F(MatrixTest, DoesMultiplication) {
  int2x2 m1 = {int2{1, 2}, int2{3, 4}};
  int2x2 m2 = {int2{1, 3}, int2{2, 4}};
  int2x2 m3 = {int2{1, 1}, int2{1, 1}};

  EXPECT_EQ(m1 * int2x2::zero(), int2x2::zero());

  EXPECT_EQ(m1 * int2x2::id(), m1);
  EXPECT_EQ(int2x2::id() * m1, m1);

  EXPECT_EQ((m1 * m2) * m3, m1 * (m2 * m3));

  {
    auto a = transpose(m1 * m2);
    auto b = transpose(m2) * transpose(m1);
    EXPECT_EQ(a, b);
  }
}

TEST_F(MatrixTest, DoesOtherOps) {
  int2x2 m1 = {int2{1, 2}, int2{3, 4}};
  {
    auto expected = int2x2{int2{1, 3}, int2{2, 4}};
    EXPECT_EQ(m1.transpose(), expected);
  }
  EXPECT_EQ(m1.trace(), 5);
  EXPECT_EQ(m1.det(), -2);
}

TEST_F(MatrixTest, Test2x2) {
  double2x2 a = {double2{1, 2}, double2{3, 4}};
  double2x2 b = {double2{1, 3}, double2{2, 4}};
  double2x2 ab = a * b;
  double2x2 ba = b * a;

  EXPECT_EQ(ab * inverse(b), a);
  EXPECT_EQ(ab.inverse(), b.inverse() * a.inverse());
  EXPECT_EQ(double2x2::id().inverse(), double2x2::id());

  EXPECT_EQ(ab.det(), ba.det());
  EXPECT_EQ(ab.det(), a.det() * b.det());
}

TEST_F(MatrixTest, Test3x3) {
  double3x3 a = {
    double3{1, 1, 1},
    double3{1, 2, 3},
    double3{1, 0, 1}
  };
  double3x3 b = {
    double3{1, 1, 0},
    double3{1, 0, 1},
    double3{0, 1, 1}
  };
  double3x3 ab = a * b;
  double3x3 ba = b * a;

  EXPECT_EQ(ab * inverse(b), a);
  EXPECT_EQ(ab.inverse(), b.inverse() * a.inverse());
  EXPECT_EQ(double3x3::id().inverse(), double3x3::id());

  EXPECT_EQ(ab.det(), ba.det());
  EXPECT_EQ(ab.det(), a.det() * b.det());
}

TEST_F(MatrixTest, Test4x4) {
  double4x4 a = {
    double4{1, 2, 4, 8},
    double4{0, 2, 0, 0},
    double4{0, 0, 4, 0},
    double4{0, 0, 0, 8}
  };
  double4x4 b = {
    double4{1, 1, 1, 0},
    double4{1, 1, 0, 1},
    double4{1, 0, 1, 1},
    double4{0, 1, 1, 1}
  };
  double4x4 ab = a * b;
  double4x4 ba = b * a;

  bool (&near_equal)(const double4x4 &, const double4x4 &)
      = NearEqualForTesting;

  EXPECT_TRUE(near_equal(ab * inverse(b), a));
  EXPECT_TRUE(near_equal(ab.inverse(), b.inverse() * a.inverse()));
  EXPECT_EQ(double4x4::id().inverse(), double4x4::id());

  EXPECT_EQ(ab.det(), ba.det());
  EXPECT_EQ(ab.det(), a.det() * b.det());
}

}  // namespace test

}  // namespace deer
