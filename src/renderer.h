// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details

#ifndef DEER_RENDERER_H_
#define DEER_RENDERER_H_

#include <cstdint>
#include <future>
#include <memory>
#include <vector>

#include "scene.h"

namespace deer {

class Renderer {
 public:
  struct JobStatus {
    std::future<std::vector<std::uint8_t>> result;
    double amount_done;  // from 0.0 to 1.0
    // TODO(iliazeus): 'error' field
  };

  virtual std::shared_ptr<JobStatus> Render(const Scene &,
                                            const Camera &) = 0;
  virtual ~Renderer() {}
};

}  // namespace deer

#endif  // DEER_RENDERER_H_
