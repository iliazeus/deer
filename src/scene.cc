// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#include "scene.h"

#include <optional>

#include "geometry.h"
#include "matrix.h"
#include "optics.h"
#include "vector.h"

namespace deer {

std::optional<RayIntersection> Scene::TraceRay(const Ray &ray) const {
  std::optional<RayIntersection> isec = {};
  double len2;
  for (const auto &object : objects_) {
    auto current_isec = object->IntersectWithRay(ray);
    if (!current_isec) continue;
    double current_len2 = length2(current_isec->point - ray.origin);
    if (!isec || current_len2 < len2) {
      isec = current_isec;
      len2 = current_len2;
    }
  }
  return isec;
}

}  // namespace deer
