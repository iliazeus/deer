// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#ifndef DEER_GEOMETRY_H_
#define DEER_GEOMETRY_H_

#include <optional>

#include "optics.h"
#include "vector.h"

namespace deer {

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
