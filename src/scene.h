// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#ifndef DEER_SCENE_H_
#define DEER_SCENE_H_

#include <memory>
#include <vector>

#include "geometry.h"
#include "transform.h"

namespace deer {

class SceneObject {
 public:
  // from object space into scene space
  AffineTransform &transform() { return transform_; }
  const AffineTransform &transform() const { return transform_; }

  virtual ~SceneObject() {}

  const double4 &position() const { return transform_.matrix()[3]; }

 protected:
  AffineTransform transform_;

  explicit SceneObject(const AffineTransform &transform = {})
      : transform_(transform) {}
};

class GeometryObject : public SceneObject {
 public:
  explicit GeometryObject(std::shared_ptr<Geometry> geometry,
      const AffineTransform &transform = {})
      : SceneObject(transform)
      , geometry_(geometry) {}

 protected:
  std::shared_ptr<Geometry> geometry_;
};

class CameraObject : public SceneObject {
 public:
  explicit CameraObject(const AffineTransform &transform = {})
      : SceneObject(transform) {}
  CameraObject(double width, double height, double fov) {
    transform_.Scale(width, height, fov);
  }

  double width() const { return transform_.matrix()[0].length(); }
  double height() const { return transform_.matrix()[1].length(); }
  double fov() const { return transform_.matrix()[2].length(); }
  double4 line_of_sight() const { return transform_.matrix()[2]; }
};

class Scene {
 public:
  std::vector<std::shared_ptr<SceneObject>> &objects() { return objects_; }
  const std::vector<std::shared_ptr<SceneObject>> &objects() const {
    return objects_;
  }

 protected:
  std::vector<std::shared_ptr<SceneObject>> objects_;
};

}  // namespace deer

#endif  // DEER_SCENE_H_
