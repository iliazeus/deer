// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details

#ifndef DEER_RENDERER_H_
#define DEER_RENDERER_H_

#include <cstdint>
#include <future>
#include <limits>
#include <memory>
#include <vector>

#include "scene.h"
#include "spectrum.h"

namespace deer {

class Renderer {
 public:
  struct JobStatus {
    std::future<std::vector<std::uint8_t>> result;  // RGB bytes
    double amount_done;  // from 0.0 to 1.0
    // TODO(iliazeus): 'error' field
  };

  virtual std::shared_ptr<JobStatus> Render(const Scene &,
                                            const CameraObject &) = 0;
  virtual ~Renderer() {}
};

class RayTracer : public Renderer {
 public:
  struct Options {
    std::size_t image_width, image_height;
    double r_freq, g_freq, b_freq;
  };
  const Options options;

  explicit RayTracer(const Options &opts) : options(opts) {}

  std::shared_ptr<JobStatus> Render(const Scene &,
                                    const CameraObject &) override;
};

}  // namespace deer

#endif  // DEER_RENDERER_H_
