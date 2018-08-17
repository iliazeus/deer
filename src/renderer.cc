// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#include "renderer.h"

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

std::shared_ptr<Spectrum> TraceRay(const RayTracer &tracer,
                                   const Scene &scene,
                                   const Ray &ray) {
  // TODO(iliazeus): a whole bunch of proper rendering

  // Find a closest (if any) intersection.
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

  // If no intersection found, then we hit the sky.
  if (!isec) return scene.sky_spectrum;

  // Currently, only ambiance lighting is implemented.
  return Spectrum::product(isec->material->diffusion_spectrum,
      scene.ambiance_spectrum);
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
        result.push_back(spectrum->intensity(tracer.options.r_freq));
        result.push_back(spectrum->intensity(tracer.options.g_freq));
        result.push_back(spectrum->intensity(tracer.options.b_freq));
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
    result.push_back(spectrum->intensity(tracer.options.r_freq));
    result.push_back(spectrum->intensity(tracer.options.g_freq));
    result.push_back(spectrum->intensity(tracer.options.b_freq));
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
