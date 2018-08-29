// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details

#include "../src/geometry.h"

#include <optional>

#include <gtest/gtest.h>

#include "../src/optics.h"
#include "../src/vector.h"

namespace deer {

namespace test {

static bool (&near_equal)(const double4 &, const double4 &)
    = NearEqualForTesting;

class XYPlaneGeometryTest : public ::testing::Test {
 protected:
  XYPlaneGeometry plane_;
};

TEST_F(XYPlaneGeometryTest, IntersectWithRayWorks) {
  auto ray = Ray{double4{1, 2, 3, 1}, double4{3, 2, -1, 0}};

  auto result = plane_.IntersectWithRay(ray).value();

  auto expected_normal_direction = double4{0, 0, 1, 0};
  auto expected_intersection_point = double4{10, 8, 0, 1};

  EXPECT_EQ(result.point, expected_intersection_point);
  EXPECT_EQ(result.normal.project_onto(expected_normal_direction),
      result.normal);

  ray.direction *= -1;
  EXPECT_FALSE(plane_.IntersectWithRay(ray).has_value());
}


class UnitSphereGeometryTest : public ::testing::Test {
 protected:
  UnitSphereGeometry sphere_;
};

TEST_F(UnitSphereGeometryTest, IntersectWithRayWorks) {
  auto inner_ray = Ray{double4{0, 0, 0, 1}, double4{1, 2, 3, 0}};
  auto outer_ray = Ray{double4{1, 2, 3, 1}, double4{-1, -2, -3, 0}};

  auto inner_result = sphere_.IntersectWithRay(inner_ray).value();
  auto outer_result = sphere_.IntersectWithRay(outer_ray).value();

  EXPECT_TRUE(near_equal(inner_result.point, outer_result.point));
  EXPECT_TRUE(near_equal(inner_result.normal, -1 * outer_result.normal));

  inner_ray.direction *= -1;
  EXPECT_TRUE(sphere_.IntersectWithRay(inner_ray).has_value());
  outer_ray.direction *= -1;
  EXPECT_FALSE(sphere_.IntersectWithRay(outer_ray).has_value());
}

}  // namespace test

}  // namespace deer
