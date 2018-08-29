// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#ifndef DEER_GEOMETRY_H_
#define DEER_GEOMETRY_H_

#include <array>
#include <optional>
#include <vector>

#include "optics.h"
#include "transform.h"
#include "vector.h"

namespace deer {

struct Geometry {
  virtual std::optional<RayIntersection> IntersectWithRay(
      const Ray &) const = 0;
  virtual ~Geometry() {}
};

struct XYPlaneGeometry : public Geometry {
  std::optional<RayIntersection> IntersectWithRay(
      const Ray &) const override;
};

struct UnitSphereGeometry : public Geometry {
  std::optional<RayIntersection> IntersectWithRay(
      const Ray &) const override;
};

class TrianglesGeometry : public Geometry {
 public:
  explicit TrianglesGeometry(
      const std::vector<std::array<double4, 3>> &triangles);

  std::optional<RayIntersection> IntersectWithRay(
      const Ray &) const override;

 private:
  std::vector<AffineTransform> transforms_;
};

}  // namespace deer

#endif  // DEER_GEOMETRY_H_
