// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#include "../src/matrix.h"

#include <gtest/gtest.h>

class MatrixTest : public ::testing::Test {};

TEST_F(MatrixTest, IsContainer) {
  deer::int3x3 m = {
    deer::int3{1, 2, 3},
    deer::int3{4, 5, 6},
    deer::int3{7, 8, 9},
  };
  EXPECT_EQ(m.size(), 3u);
  {
    auto expected = deer::int3{4, 5, 6};
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
  deer::int2x2 m1 = {deer::int2{1, 2}, deer::int2{3, 4}};
  deer::int2x2 m2 = deer::int2x2::id();
  {
    auto expected = deer::int2x2{deer::int2{2, 2}, deer::int2{3, 5}};
    EXPECT_EQ(m1 + m2, expected);
  }
  {
    auto expected = deer::int2x2{deer::int2{0, 2}, deer::int2{3, 3}};
    EXPECT_EQ(m1 - m2, expected);
  }
  {
    auto expected = deer::int2x2{deer::int2{3, 6}, deer::int2{9, 12}};
    EXPECT_EQ(m1 * 3, expected);
    EXPECT_EQ(3 * m1, expected);
  }
  {
    auto expected = deer::int2x2{deer::int2{-1, -2}, deer::int2{-3, -4}};
    EXPECT_EQ(-1 * m1, expected);
    EXPECT_EQ(-m1, expected);
  }
  {
    auto expected = deer::int2x2{deer::int2{0, 1}, deer::int2{1, 2}};
    EXPECT_EQ(m1 / 2, expected);
  }
}

TEST_F(MatrixTest, DoesMultiplication) {
  deer::int2x2 m1 = {deer::int2{1, 2}, deer::int2{3, 4}};
  deer::int2x2 m2 = {deer::int2{1, 3}, deer::int2{2, 4}};
  deer::int2x2 m3 = {deer::int2{1, 1}, deer::int2{1, 1}};

  EXPECT_EQ(m1 * deer::int2x2::zero(), deer::int2x2::zero());

  EXPECT_EQ(m1 * deer::int2x2::id(), m1);
  EXPECT_EQ(deer::int2x2::id() * m1, m1);

  EXPECT_EQ((m1 * m2) * m3, m1 * (m2 * m3));

  {
    auto a = deer::transpose(m1 * m2);
    auto b = deer::transpose(m2) * deer::transpose(m1);
    EXPECT_EQ(a, b);
  }
}

TEST_F(MatrixTest, DoesOtherOps) {
  deer::int2x2 m1 = {deer::int2{1, 2}, deer::int2{3, 4}};
  {
    auto expected = deer::int2x2{deer::int2{1, 3}, deer::int2{2, 4}};
    EXPECT_EQ(m1.transpose(), expected);
  }
  EXPECT_EQ(m1.trace(), 5);
  EXPECT_EQ(m1.det(), -2);
}

TEST_F(MatrixTest, Test2x2) {
  deer::double2x2 a = {deer::double2{1, 2}, deer::double2{3, 4}};
  deer::double2x2 b = {deer::double2{1, 3}, deer::double2{2, 4}};
  deer::double2x2 ab = a * b;
  deer::double2x2 ba = b * a;

  EXPECT_EQ(ab * deer::inverse(b), a);
  EXPECT_EQ(ab.inverse(), b.inverse() * a.inverse());
  EXPECT_EQ(deer::double2x2::id().inverse(), deer::double2x2::id());

  EXPECT_EQ(ab.det(), ba.det());
  EXPECT_EQ(ab.det(), a.det() * b.det());
}

TEST_F(MatrixTest, Test3x3) {
  deer::double3x3 a = {
    deer::double3{1, 1, 1},
    deer::double3{1, 2, 3},
    deer::double3{1, 0, 1}
  };
  deer::double3x3 b = {
    deer::double3{1, 1, 0},
    deer::double3{1, 0, 1},
    deer::double3{0, 1, 1}
  };
  deer::double3x3 ab = a * b;
  deer::double3x3 ba = b * a;

  EXPECT_EQ(ab * deer::inverse(b), a);
  EXPECT_EQ(ab.inverse(), b.inverse() * a.inverse());
  EXPECT_EQ(deer::double3x3::id().inverse(), deer::double3x3::id());

  EXPECT_EQ(ab.det(), ba.det());
  EXPECT_EQ(ab.det(), a.det() * b.det());
}
