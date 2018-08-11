// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#ifndef DEER_SCENE_H_
#define DEER_SCENE_H_

#include <memory>
#include <vector>

#include "transform.h"

namespace deer {

class SceneObject {
 public:
  // from object space into scene space
  AffineTransform &transform() { return transform_; }
  const AffineTransform &transform() const { return transform_; }

  virtual ~SceneObject() {}

 protected:
  AffineTransform transform_;

  SceneObject();
  SceneObject(const AffineTransform &transform);
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
