// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#include "../src/scene.h"

#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include "../src/geometry.h"
#include "../src/optics.h"
#include "../src/spectrum.h"
#include "../src/transform.h"

namespace deer {

namespace test {

class SceneTest : public ::testing::Test {};

TEST_F(SceneTest, StroresThings) {
  Scene scene;

  EXPECT_EQ(scene.objects().size(), 0u);
  EXPECT_EQ(scene.cameras().size(), 0u);
  EXPECT_EQ(scene.point_light_sources().size(), 0u);

  auto object = std::make_shared<GeometryObject>(nullptr, nullptr);
  scene.Add(object);
  auto camera = std::make_shared<Camera>();
  scene.Add(camera);
  auto point_light_source = std::make_shared<PointLightSource>(
      double4::zero(), Spectrum::MakeConstant(0));
  scene.Add(point_light_source);

  EXPECT_EQ(scene.objects().size(), 1u);
  EXPECT_EQ(scene.cameras().size(), 1u);
  EXPECT_EQ(scene.point_light_sources().size(), 1u);

  scene.Remove(object);
  scene.Remove(camera);
  scene.Remove(point_light_source);

  EXPECT_EQ(scene.objects().size(), 0u);
  EXPECT_EQ(scene.cameras().size(), 0u);
  EXPECT_EQ(scene.point_light_sources().size(), 0u);
}

TEST_F(SceneTest, TracesRays) {
  Scene scene;

  auto geometry = std::make_shared<UnitSphereGeometry>();

  auto material1 = std::make_shared<Material>();
  auto object1 = std::make_shared<GeometryObject>(
      geometry, material1, AffineTransform().Translate(0, 0, 0).Scale(1));
  scene.Add(object1);

  auto material2 = std::make_shared<Material>();
  auto object2 = std::make_shared<GeometryObject>(
      geometry, material2, AffineTransform().Translate(0, 0, 3).Scale(3));
  scene.Add(object2);

  auto ray1 = Ray{double4{0, 0, -5, 1}, double4{0.1, 0.1, 1, 0}};
  auto isec1 = scene.TraceRay(ray1);
  EXPECT_TRUE(isec1.has_value());
  EXPECT_EQ(isec1->material, material1);

  auto ray2 = Ray{double4{1.5, 0, -5, 1}, double4{0, 0, 1, 0}};
  auto isec2 = scene.TraceRay(ray2);
  EXPECT_TRUE(isec2.has_value());
  EXPECT_EQ(isec2->material, material2);

  auto ray0 = Ray{double4{0, 0, -5, 1}, double4{1, 1, 1, 0}};
  auto isec0 = scene.TraceRay(ray0);
  EXPECT_FALSE(isec0.has_value());
}

}  // namespace test

}  // namespace deer
