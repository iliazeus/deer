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
#include "spectrum.h"
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

class PointLightSource {
 public:
  double4 position;
  Spectrum spectrum;
  double softness;

  PointLightSource(double4 p, Spectrum spec, double soft = 0)
      : position(p), spectrum(spec), softness(soft) {}
};

class Scene {
 public:
  Spectrum sky_spectrum = Spectrum::MakeConstant(0);
  Spectrum ambiance_spectrum = Spectrum::MakeConstant(1);

  std::optional<RayIntersection> TraceRay(const Ray &ray) const;

  const std::vector<std::shared_ptr<SceneObject>> &objects() const {
    return objects_;
  }
  void Add(std::shared_ptr<SceneObject> object) {
    objects_.push_back(object);
  }
  void Remove(std::shared_ptr<SceneObject> object) {
    objects_.erase(std::find(objects_.begin(), objects_.end(), object));
  }

  const std::vector<std::shared_ptr<Camera>> &cameras() const {
    return cameras_;
  }
  void Add(std::shared_ptr<Camera> camera) {
    cameras_.push_back(camera);
  }
  void Remove(std::shared_ptr<Camera> camera) {
    cameras_.erase(std::find(cameras_.begin(), cameras_.end(), camera));
  }

  const std::vector<std::shared_ptr<PointLightSource>>
      &point_light_sources() const {
    return point_light_sources_;
  }
  void Add(std::shared_ptr<PointLightSource> source) {
    point_light_sources_.push_back(source);
  }
  void Remove(std::shared_ptr<PointLightSource> source) {
    point_light_sources_.erase(std::find(
        point_light_sources_.begin(), point_light_sources_.end(), source));
  }

 private:
  std::vector<std::shared_ptr<SceneObject>> objects_;
  std::vector<std::shared_ptr<Camera>> cameras_;
  std::vector<std::shared_ptr<PointLightSource>> point_light_sources_;
};

}  // namespace deer

#endif  // DEER_SCENE_H_
