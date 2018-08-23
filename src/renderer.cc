// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#include "renderer.h"

#include <cmath>
#include <cstdint>
#include <future>
#include <memory>
#include <optional>
#include <queue>
#include <vector>
#include <utility>

#include "optics.h"
#include "spectrum.h"
#include "scene.h"
#include "transform.h"
#include "vector.h"

namespace deer {

namespace {

// TODO(iliazeus): a proper parallelization
constexpr auto kLaunchPolicy = std::launch::deferred;

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

std::optional<RayIntersection> TraceRayThroughSceneObjects(
    const RayTracer &tracer, const Scene &scene, const Ray &ray) {
  std::optional<RayIntersection> isec = {};
  double len2;
  for (const auto &object : scene.objects()) {
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

std::shared_ptr<Spectrum> TraceRay(const RayTracer &tracer,
                                   const Scene &scene,
                                   const Ray &ray) {
  // TODO(iliazeus): a whole bunch of proper rendering

  // Find a closest (if any) intersection.
  auto isec = TraceRayThroughSceneObjects(tracer, scene, ray);

  // If an intersection is farther than max_distance, drop it.
  const double max_distance2 = std::pow(tracer.options.max_distance, 2);
  if (isec && length2(isec->point - ray.origin) > max_distance2) {
    isec = {};
  }

  // If no intersection found, then we hit the sky.
  if (!isec) return scene.sky_spectrum;

  const double kLightingEps = 1e-6;
  auto lighting_spectrum = scene.ambiance_spectrum;

  // Check if each of the point light sources is reachable, modifying
  // the total lighting_spectrum.
  for (const auto &source : scene.point_light_sources()) {
    const double4 ray_origin = isec->point + kLightingEps * isec->normal;
    const double4 ray_direction = source->position - ray_origin;
    const Ray ray{ray_origin, ray_direction};
    const auto light_isec = TraceRayThroughSceneObjects(tracer, scene, ray);
    if (light_isec) {
      if (length2(light_isec->point - ray_origin) < length2(ray_direction)) {
        continue;
      }
    }
    lighting_spectrum = Spectrum::sum(lighting_spectrum, source->spectrum);
  }

  const auto result_spectrum = Spectrum::product(
      isec->material->diffusion_spectrum, lighting_spectrum);

  return result_spectrum;
}

void RenderPixels(const RayTracer &tracer,
                  const Scene &scene,
                  const Camera &camera,
                  std::promise<std::vector<std::uint8_t>> &&result_promise,
                  std::shared_ptr<Renderer::JobStatus> job_status) {
  const std::size_t kMaxQueueLength = 50;

  std::vector<std::uint8_t> result;
  result.reserve(tracer.options.image_width * tracer.options.image_height);
  std::queue<std::future<std::shared_ptr<Spectrum>>> queue;

  for (std::size_t row = 0; row < tracer.options.image_height; row++) {
    for (std::size_t col = 0; col < tracer.options.image_width; col++) {
      while (queue.size() >= kMaxQueueLength) {
        queue.front().wait();
        auto spectrum = queue.front().get();
        queue.pop();
        result.push_back(spectrum->intensity(tracer.options.r_wavelength));
        result.push_back(spectrum->intensity(tracer.options.g_wavelength));
        result.push_back(spectrum->intensity(tracer.options.b_wavelength));
      }

      Ray ray = RayThroughPixel(tracer, camera, row, col);
      auto future = std::async(std::launch::deferred,
          TraceRay, tracer, scene, ray);
      queue.push(std::move(future));
    }
  }

  while (!queue.empty()) {
    queue.front().wait();
    auto spectrum = queue.front().get();
    queue.pop();
    result.push_back(spectrum->intensity(tracer.options.r_wavelength));
    result.push_back(spectrum->intensity(tracer.options.g_wavelength));
    result.push_back(spectrum->intensity(tracer.options.b_wavelength));
  }

  result_promise.set_value(std::move(result));
  job_status->amount_done = 1.0;
}

}  // namespace

std::shared_ptr<Renderer::JobStatus> RayTracer::Render(const Scene &scene,
    const Camera &camera) {
  // TODO(iliazeus): track the amount_done
  auto job_status = std::make_shared<Renderer::JobStatus>();
  job_status->amount_done = 0;
  std::promise<std::vector<std::uint8_t>> result_promise;
  job_status->result = result_promise.get_future();
  std::async(std::launch::async, RenderPixels, *this,
      scene, camera, std::move(result_promise), job_status);
  return job_status;
}

}  // namespace deer
