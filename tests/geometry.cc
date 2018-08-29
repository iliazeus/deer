// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details

#include "../src/geometry.h"

#include <iostream>
#include <optional>

#include <gtest/gtest.h>

#include "../src/optics.h"
#include "../src/vector.h"

namespace deer {

namespace test {

static bool near_equal(const RayIntersection &a, const RayIntersection &b) {
  if (!near_equal(a.point, b.point)) {
    return false;
  }
  if (dot(a.normal, b.normal) / (length(a.normal)*length(b.normal)) < 0.99) {
    return false;
  }
  return true;
}

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


class TrianglesGeometryTetrahedronTest : public ::testing::Test {
 protected:
  TrianglesGeometry tetrahedron_ = TrianglesGeometry({
    {double4{0, 0, 0, 1}, double4{1, 0, 0, 1}, double4{0, 1, 0, 1}},
    {double4{0, 0, 0, 1}, double4{0, 1, 0, 1}, double4{0, 0, 1, 1}},
    {double4{0, 0, 0, 1}, double4{0, 0, 1, 1}, double4{1, 0, 0, 1}},
    {double4{1, 0, 0, 1}, double4{0, 1, 0, 1}, double4{0, 0, 1, 1}},
  });
};

TEST_F(TrianglesGeometryTetrahedronTest, IntersectWithRayWorks) {
  auto ray1 = Ray{double4{-3, 0.25, 0.25, 1}, double4{1, 0, 0, 0}};
  auto expected_isec1 =
      RayIntersection{double4{0, 0.25, 0.25, 1}, double4{-1, 0, 0, 0}};
  auto isec1 = tetrahedron_.IntersectWithRay(ray1);
  EXPECT_TRUE(isec1.has_value());
  EXPECT_TRUE(near_equal(expected_isec1, *isec1));

  auto ray2 = Ray{double4{3, 3, 3, 1}, double4{-1, -1, -1, 0}};
  auto expected_isec2 =
      RayIntersection{double4{1/3.0, 1/3.0, 1/3.0, 1},
      double4{1/3.0, 1/3.0, 1/3.0, 0}};
  auto isec2 = tetrahedron_.IntersectWithRay(ray2);
  EXPECT_TRUE(isec2.has_value());
  EXPECT_TRUE(near_equal(expected_isec2, *isec2));
}

}  // namespace test

}  // namespace deer
