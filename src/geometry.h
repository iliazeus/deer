// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#ifndef DEER_GEOMETRY_H_
#define DEER_GEOMETRY_H_

#include "vector.h"

#include <optional>

namespace deer {

struct Ray {
  double4 origin;
  double4 direction;

  Ray(double4 o, double4 d) : origin(o), direction(d) {}
};

struct RayIntersection {
  double4 point;
  double4 normal;
};

struct Geometry {
  virtual std::optional<RayIntersection> IntersectWithRay(
      const Ray &) const = 0;
  virtual ~Geometry() {}
};

struct XYPlane : public Geometry {
  std::optional<RayIntersection> IntersectWithRay(
      const Ray &) const override;
};

struct UnitSphere : public Geometry {
  std::optional<RayIntersection> IntersectWithRay(
      const Ray &) const override;
};

}  // namespace deer

#endif  // DEER_GEOMETRY_H_
