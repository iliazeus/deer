// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#ifndef DEER_OPTICS_H_
#define DEER_OPTICS_H_

#include <optional>
#include <memory>

#include "spectrum.h"
#include "vector.h"

namespace deer {

struct Material {
  std::shared_ptr<Spectrum> diffusion_spectrum;
};

struct RayIntersection {
  double4 point;
  double4 normal;
  std::shared_ptr<Material> material = nullptr;
};

struct Ray {
  double4 origin;
  double4 direction;
  std::shared_ptr<Spectrum> spectrum = nullptr;

  Ray ReflectOff(const RayIntersection &isec) const {
    return Ray{isec.point, direction.reflect_off(isec.normal), spectrum};
  }

  // TODO(iliazeus): refraction
};

}  // namespace deer

#endif  // DEER_OPTICS_H_
