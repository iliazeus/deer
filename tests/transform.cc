// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#include "../src/transform.h"

#include <cmath>

#include <gtest/gtest.h>

class TransformTest : public ::testing::Test {};

TEST_F(TransformTest, AffineTransformTranslate) {
  auto point = deer::double4{1, 2, 3, 1};
  auto vector = deer::double4{1, 2, 3, 0};

  auto transform = deer::AffineTransform().Translate(2, 0, 1);

  auto expected_point = deer::double4{3, 2, 4, 1};
  EXPECT_EQ(transform.Apply(point), expected_point);

  EXPECT_EQ(transform.Apply(vector), vector);
}

TEST_F(TransformTest, AffineTransformScale) {
  auto point1 = deer::double4{1, 2, 3, 1};
  auto point2 = deer::double4{2, 0, 1, 1};
  auto vector = deer::double4{1, 2, 3, 0};

  auto transform = deer::AffineTransform().Scale(2);

  EXPECT_EQ(transform.Apply(vector), 2 * vector);

  auto t_point1 = transform.Apply(point1);
  auto t_point2 = transform.Apply(point2);
  EXPECT_EQ(t_point2 - t_point1, 2 * (point2 - point1));
}

TEST_F(TransformTest, AffineTransformRotate) {
  auto point = deer::double4{1, 2, 3, 1};
  auto vector = deer::double4{1, 2, 3, 0};

  double half_pi = std::acos(0);
  auto transform = deer::AffineTransform().RotateX(half_pi);

  auto expected_point = deer::double4{1, -3, 2, 1};
  auto expected_vector = deer::double4{1, -3, 2, 0};

  EXPECT_EQ(transform.Apply(point), expected_point);
  EXPECT_EQ(transform.Apply(vector), expected_vector);
}

TEST_F(TransformTest, AffineTransformInverse) {
  auto point = deer::double4{1, 2, 3, 1};
  auto vector = deer::double4{1, 2, 3, 0};

  auto transform = deer::AffineTransform()
      .Translate(2, 3, 1)
      .RotateY(2)
      .Scale(1, 3, 2);

  bool (&near_equal)(const deer::double4 &, const deer::double4 &)
      = deer::NearEqualForTesting;

  auto p_trans = transform.ApplyInverse(transform.Apply(point));
  EXPECT_TRUE(near_equal(p_trans, point));

  auto v_trans = transform.ApplyInverse(transform.Apply(vector));
  EXPECT_TRUE(near_equal(v_trans, vector));
}
