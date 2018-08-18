// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#include "../src/scene.h"

#include <memory>
#include <vector>

#include <gtest/gtest.h>

namespace deer {

namespace test {

class SceneTest : public ::testing::Test {};

TEST_F(SceneTest, StroresThings) {
  Scene scene;

  EXPECT_EQ(scene.objects().size(), 0u);
  EXPECT_EQ(scene.cameras().size(), 0u);
  EXPECT_EQ(scene.point_light_sources().size(), 0u);

  auto object = std::make_shared<GeometryObject>(nullptr, nullptr);
  scene.AddObject(object);
  auto camera = std::make_shared<Camera>();
  scene.AddCamera(camera);
  auto point_light_source = std::make_shared<PointLightSource>(
      double4::zero(), nullptr);
  scene.AddPointLightSource(point_light_source);

  EXPECT_EQ(scene.objects().size(), 1u);
  EXPECT_EQ(scene.cameras().size(), 1u);
  EXPECT_EQ(scene.point_light_sources().size(), 1u);

  scene.RemoveObject(object);
  scene.RemoveCamera(camera);
  scene.RemovePointLightSource(point_light_source);

  EXPECT_EQ(scene.objects().size(), 0u);
  EXPECT_EQ(scene.cameras().size(), 0u);
  EXPECT_EQ(scene.point_light_sources().size(), 0u);
}

}  // namespace test

}  // namespace deer
