// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#ifndef DEER_SCENE_H_
#define DEER_SCENE_H_

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

class CameraObject : public SceneObject {
 public:
  explicit CameraObject(const AffineTransform &transform = {})
      : SceneObject(transform) {}
  CameraObject(double width, double height, double focal_length) {
    transform.Scale(width, height, focal_length);
  }

  std::optional<RayIntersection> IntersectWithRay(
      const Ray &) const override {
    return {};
  }

  double width() const { return transform.matrix()[0].length(); }
  double height() const { return transform.matrix()[1].length(); }
  double focal_length() const { return transform.matrix()[2].length(); }
  double4 line_of_sight() const { return transform.matrix()[2]; }
};

struct Scene {
  std::vector<std::shared_ptr<SceneObject>> objects;
  std::shared_ptr<Spectrum> sky_spectrum =
      std::make_shared<ConstantSpectrum>(0);
  std::shared_ptr<Spectrum> ambiance_spectrum =
      std::make_shared<ConstantSpectrum>(1);
};

}  // namespace deer

#endif  // DEER_SCENE_H_
