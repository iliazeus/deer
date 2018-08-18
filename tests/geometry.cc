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

class GeometryTest : public ::testing::Test {};

TEST_F(GeometryTest, XYPlaneGeometryDoesRayIntersection) {
  auto plane = XYPlaneGeometry();
  auto ray = Ray{double4{1, 2, 3, 1}, double4{3, 2, -1, 0}};

  auto result = plane.IntersectWithRay(ray).value();

  auto expected_normal_direction = double4{0, 0, 1, 0};
  auto expected_intersection_point = double4{10, 8, 0, 1};

  EXPECT_EQ(result.point, expected_intersection_point);
  EXPECT_EQ(result.normal.project_onto(expected_normal_direction),
      result.normal);

  ray.direction *= -1;
  EXPECT_FALSE(plane.IntersectWithRay(ray).has_value());
}

TEST_F(GeometryTest, UnitSphereDoesRayIntersection) {
  auto sphere = UnitSphereGeometry();
  auto inner_ray = Ray{double4{0, 0, 0, 1}, double4{1, 2, 3, 0}};
  auto outer_ray = Ray{double4{1, 2, 3, 1}, double4{-1, -2, -3, 0}};

  auto inner_result = sphere.IntersectWithRay(inner_ray).value();
  auto outer_result = sphere.IntersectWithRay(outer_ray).value();

  auto &near_equal = NearEqualForTesting<double, 4>;

  EXPECT_TRUE(near_equal(inner_result.point, outer_result.point));
  EXPECT_TRUE(near_equal(inner_result.normal, -1 * outer_result.normal));

  inner_ray.direction *= -1;
  EXPECT_TRUE(sphere.IntersectWithRay(inner_ray).has_value());
  outer_ray.direction *= -1;
  EXPECT_FALSE(sphere.IntersectWithRay(outer_ray).has_value());
}

}  // namespace test

}  // namespace deer
