// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#include "geometry.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <vector>

#include "optics.h"
#include "transform.h"
#include "vector.h"

namespace deer {


std::optional<RayIntersection> XYPlaneGeometry::IntersectWithRay(
    const Ray &ray) const {
  double d = ray.origin.z() * ray.direction.z();
  if (d >= 0) return {};

  double4 r = ray.direction * ray.origin.z() / ray.direction.z();
  double n = ray.origin.z() > 0 ? 1 : -1;
  return RayIntersection{ray.origin - r, double4{0, 0, n, 0}};
}


std::optional<RayIntersection> UnitSphereGeometry::IntersectWithRay(
    const Ray &ray) const {
  double4 r = ray.origin - double4{0, 0, 0, 1};
  double4 d = ray.direction;

  // solving a*alpha^2 + b*alpha + c = 0
  // where r + alpha*d is our intersection point

  double a = d.x()*d.x() + d.y()*d.y() + d.z()*d.z();
  double b = 2 * (r.x()*d.x() + r.y()*d.y() + r.z()*d.z());
  double c = r.x()*r.x() + r.y()*r.y() + r.z()*r.z() - 1;

  // we need only the positive roots
  if (b > 0 && c > 0) return {};

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


TrianglesGeometry::TrianglesGeometry(
    const std::vector<std::array<double4, 3>> &triangles) {
  for (const auto &triangle : triangles) {
    double4 a = triangle[0];
    double4 ab = triangle[1] - triangle[0];
    double4 ac = triangle[2] - triangle[0];
    double4 n = cross(ab, ac);
    transforms_.emplace_back(double4x4{ab, ac, n, a});
  }
}

std::optional<RayIntersection> TrianglesGeometry::IntersectWithRay(
    const Ray &ray) const {
  std::optional<RayIntersection> isec;
  double len2;

  for (const auto &t: transforms_) {
    Ray t_ray = {t.ApplyInverse(ray.origin), t.ApplyInverse(ray.direction)};

    double d = t_ray.origin.z() * t_ray.direction.z();
    if (d >= 0) continue;

    double4 r = t_ray.direction * t_ray.origin.z() / t_ray.direction.z();
    double n = t_ray.origin.z() > 0 ? 1 : -1;
    RayIntersection t_isec{t_ray.origin - r, double4{0, 0, n, 0}};

    if (t_isec.point.x() < 0 || t_isec.point.y() < 0) continue;
    if (t_isec.point.x() + t_isec.point.y() > 1) continue;
    double t_len2 = length2(t.Apply(r));
    if (isec && t_len2 > len2) continue;

    isec = {t.Apply(t_isec.point), t.Apply(t_isec.normal)};
    len2 = t_len2;
  }

  return isec;
}


}  // namespace deer
