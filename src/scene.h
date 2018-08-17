// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#ifndef DEER_SCENE_H_
#define DEER_SCENE_H_

#include <algorithm>
#include <memory>
#include <optional>
#include <vector>

#include "geometry.h"
#include "optics.h"
#include "transform.h"

namespace deer {

class SceneObject {
 public:
  // Transforms from object coords to scene coords.
  AffineTransform transform;

  virtual ~SceneObject() {}

  const double4 &position() const { return transform.matrix()[3]; }

  virtual std::optional<RayIntersection> IntersectWithRay(
      const Ray &) const = 0;

 protected:
  explicit SceneObject(const AffineTransform &t = {})
      : transform(t) {}
};

class GeometryObject : public SceneObject {
 public:
  explicit GeometryObject(std::shared_ptr<Geometry> geometry,
                          std::shared_ptr<Material> material,
                          const AffineTransform &transform = {})
      : SceneObject(transform)
      , geometry_(geometry)
      , material_(material) {}

  std::optional<RayIntersection> IntersectWithRay(
      const Ray &ray) const override {
    auto object_space_ray = Ray{
      transform.ApplyInverse(ray.origin),
      transform.ApplyInverse(ray.direction)
    };
    auto result = geometry_->IntersectWithRay(object_space_ray);
    if (result) {
      result->point = transform.Apply(result->point);
      result->normal = transform.Apply(result->normal);
      result->material = material_;
    }
    return result;
  }

 protected:
  std::shared_ptr<Geometry> geometry_;
  std::shared_ptr<Material> material_;
};

class Camera {
 public:
  explicit Camera(const AffineTransform &t = {}) : transform(t) {}
  Camera(double width, double height, double focal_length) {
    transform.Scale(width, height, focal_length);
  }

  const double4 &position() const { return transform.matrix()[3]; }
  double width() const { return transform.matrix()[0].length(); }
  double height() const { return transform.matrix()[1].length(); }
  double focal_length() const { return transform.matrix()[2].length(); }
  double4 line_of_sight() const { return transform.matrix()[2]; }

  AffineTransform transform;
};

class Scene {
 public:
  std::shared_ptr<Spectrum> sky_spectrum =
      std::make_shared<ConstantSpectrum>(0);
  std::shared_ptr<Spectrum> ambiance_spectrum =
      std::make_shared<ConstantSpectrum>(1);

  const std::vector<std::shared_ptr<SceneObject>> &objects() const {
    return objects_;
  }
  void AddObject(std::shared_ptr<SceneObject> object) {
    objects_.push_back(object);
  }
  void RemoveObject(std::shared_ptr<SceneObject> object) {
    objects_.erase(std::find(objects_.begin(), objects_.end(), object));
  }

  const std::vector<std::shared_ptr<Camera>> &cameras() const {
    return cameras_;
  }
  void AddCamera(std::shared_ptr<Camera> camera) {
    cameras_.push_back(camera);
  }
  void RemoveCamera(std::shared_ptr<Camera> camera) {
    cameras_.erase(std::find(cameras_.begin(), cameras_.end(), camera));
  }

 private:
  std::vector<std::shared_ptr<SceneObject>> objects_;
  std::vector<std::shared_ptr<Camera>> cameras_;
};

}  // namespace deer

#endif  // DEER_SCENE_H_
