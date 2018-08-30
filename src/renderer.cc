// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#include "renderer.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <future>
#include <memory>
#include <optional>
#include <queue>
#include <utility>
#include <vector>

#include "optics.h"
#include "rgb.h"
#include "spectrum.h"
#include "scene.h"
#include "transform.h"
#include "vector.h"

namespace deer {

namespace {

Ray RayThroughPixel(const RayTracer &tracer,
                    const Camera &camera,
                    std::size_t row, std::size_t col) {
  const double  image_width  = tracer.options.image_width;
  const double  image_height = tracer.options.image_height;
  const double  screen_x =    col / (image_width  / 2) - 1;
  const double  screen_y = - (row / (image_height / 2) - 1);
  const double4 camera_space_direction = double4{screen_x, screen_y, 1, 0};
  const double4 direction = camera.transform.Apply(camera_space_direction);
  return Ray{camera.position(), direction};
}

Spectrum TraceRay(const RayTracer &tracer,
                  const Scene &scene,
                  const Ray &ray) {
  // TODO(iliazeus): a whole bunch of proper rendering

  // Find a closest (if any) intersection.
  auto isec = scene.TraceRay(ray);

  // If an intersection is farther than max_distance, drop it.
  const double max_distance2 = std::pow(tracer.options.max_distance, 2);
  if (isec && length2(isec->point - ray.origin) > max_distance2) {
    isec = {};
  }

  // If no intersection found, then we hit the sky.
  if (!isec) return scene.sky_spectrum;

  const double kLightingEps = 1e-6;
  auto ambient_lighting_spectrum = scene.ambiance_spectrum;
  auto diffuse_lighting_spectrum = Spectrum::MakeConstant(0);
  auto specular_lighting_spectrum = Spectrum::MakeConstant(0);

  // Check if each of the point light sources is reachable, modifying
  // the total lighting_spectrum.
  for (const auto &source : scene.point_light_sources()) {
    const double4 ray_origin = isec->point + kLightingEps * isec->normal;
    const double4 ray_direction = source->position - ray_origin;
    const Ray ray{ray_origin, ray_direction};

    // Cast shadows
    const auto light_isec = scene.TraceRay(ray);
    if (light_isec) {
      if (length2(light_isec->point - ray_origin) < length2(ray_direction)) {
        continue;
      }
    }

    // Phong reflection model
    const double4 nn = isec->normal / length(isec->normal);
    const double4 nl = ray_direction / length(ray_direction);
    const double4 nr = -nl.reflect_off(nn);
    diffuse_lighting_spectrum += source->spectrum * dot(nn, nl);
    specular_lighting_spectrum += source->spectrum *
        std::pow(dot(nn, nr), isec->material->shininess);
  }

  const auto result_spectrum =
      isec->material->ambiance_spectrum * ambient_lighting_spectrum +
      isec->material->diffusion_spectrum * diffuse_lighting_spectrum +
      isec->material->specular_spectrum * specular_lighting_spectrum;

  return result_spectrum;
}

void RenderPixels(const RayTracer &tracer,
                  const Scene &scene,
                  const Camera &camera,
                  std::promise<std::vector<std::uint8_t>> &&result_promise,
                  std::shared_ptr<Renderer::JobStatus> job_status) {
  const std::size_t result_size =
      tracer.options.image_width * tracer.options.image_height * 3;
  std::vector<std::uint8_t> result(result_size);
  const double amount_done_per_pixel = 1.0 / result_size;
#pragma omp parallel for
  for (std::size_t row = 0; row < tracer.options.image_height; row++) {
    for (std::size_t col = 0; col < tracer.options.image_width; col++) {
      Ray ray = RayThroughPixel(tracer, camera, row, col);
      auto spectrum = TraceRay(tracer, scene, ray);
      auto rgb_bytes = tracer.options.color_profile.ToRgbBytes(spectrum);
      result[row*tracer.options.image_width*3 + col*3 + 0] = rgb_bytes[0];
      result[row*tracer.options.image_width*3 + col*3 + 1] = rgb_bytes[1];
      result[row*tracer.options.image_width*3 + col*3 + 2] = rgb_bytes[2];
      // A race condition doesn't really bother us here
      job_status->amount_done += amount_done_per_pixel;
    }
  }
  result_promise.set_value(std::move(result));
  job_status->amount_done = 1.0;
}

}  // namespace

std::shared_ptr<Renderer::JobStatus> RayTracer::Render(const Scene &scene,
    const Camera &camera) {
  auto job_status = std::make_shared<Renderer::JobStatus>();
  job_status->amount_done = 0;
  std::promise<std::vector<std::uint8_t>> result_promise;
  job_status->result = result_promise.get_future();
  std::async(std::launch::async, RenderPixels, *this,
      scene, camera, std::move(result_promise), job_status);
  return job_status;
}

}  // namespace deer
