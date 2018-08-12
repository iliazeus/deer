// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#include "geometry.h"

#include <algorithm>
#include <cmath>
#include <optional>

#include "vector.h"

namespace deer {


std::optional<RayIntersection> XYPlane::IntersectWithRay(
    const Ray &ray) const {
  double d = ray.origin.z() * ray.direction.z();
  if (d >= 0) return {};

  double4 r = ray.direction * ray.origin.z() / ray.direction.z();
  double n = ray.origin.z() > 0 ? 1 : -1;
  return RayIntersection{ray.origin - r, double4{0, 0, n, 0}};
}


std::optional<RayIntersection> UnitSphere::IntersectWithRay(
    const Ray &ray) const {
  double4 r = ray.origin - double4{0, 0, 0, 1};
  double4 d = ray.direction;

  double a = d.x()*d.x() + d.y()*d.y() + d.z()*d.z();
  double b = 2 * (r.x()*d.x() + r.y()*d.y() + r.z()*d.z());
  double c = r.x()*r.x() + r.y()*r.y() + r.z()*r.z() - 1;

  // solving a*alpha^2 + b*alpha + c = 0
  // where r + alpha*d is our intersection point
  double discriminant = b*b - 4*a*c;
  if (discriminant < 0) return {};
  double alpha1 = (-b - std::sqrt(discriminant)) / (2 * a);
  double alpha2 = (-b + std::sqrt(discriminant)) / (2 * a);

  double alpha;

  // we want the closest intersection "in front" of the origin
  if (alpha1 >= 0 && alpha2 >= 0) alpha = std::min(alpha1, alpha2);
  else if (alpha1 >= 0) alpha = alpha1;
  else if (alpha2 >= 0) alpha = alpha2;
  else return {};

  double4 isec_point = ray.origin + alpha * d;
  double4 isec_normal = r + alpha * d;
  // we want an inner normal if we are inside the sphere
  if (r.length2() < 1) isec_normal *= -1;

  return RayIntersection{isec_point, isec_normal};
}


}  // namespace deer
