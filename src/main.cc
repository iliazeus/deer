// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#include <iostream>
#include <memory>
#include <utility>

#include "file_formats/tga.h"
#include "geometry.h"
#include "optics.h"
#include "renderer.h"
#include "scene.h"
#include "spectrum.h"
#include "transform.h"

using namespace deer;

static Scene SetUpScene() {
  Scene scene;

  auto red_spectrum = std::make_shared<MonochromeSpectrum>();
  red_spectrum->peak_freq = 0;
  red_spectrum->peak_width = 0.5;
  red_spectrum->peak_height = 255;
  auto red_material = std::make_shared<Material>();
  red_material->diffusion_spectrum = red_spectrum;

  auto green_spectrum = std::make_shared<MonochromeSpectrum>();
  green_spectrum->peak_freq = 1;
  green_spectrum->peak_width = 0.5;
  green_spectrum->peak_height = 255;
  auto green_material = std::make_shared<Material>();
  green_material->diffusion_spectrum = green_spectrum;

  auto blue_spectrum = std::make_shared<MonochromeSpectrum>();
  blue_spectrum->peak_freq = 2;
  blue_spectrum->peak_width = 0.5;
  blue_spectrum->peak_height = 255;
  auto blue_material = std::make_shared<Material>();
  blue_material->diffusion_spectrum = blue_spectrum;

  auto sphere_geometry = std::make_shared<UnitSphereGeometry>();

  scene.AddObject(std::make_shared<GeometryObject>(
      sphere_geometry,
      red_material,
      AffineTransform().Translate(-3, 0, 0)));
  scene.AddObject(std::make_shared<GeometryObject>(
      sphere_geometry,
      green_material,
      AffineTransform().Translate(0, 0, 0)));
  scene.AddObject(std::make_shared<GeometryObject>(
      sphere_geometry,
      blue_material,
      AffineTransform().Translate(3, 0, 0)));

  return scene;
}

static Camera SetUpCamera() {
  Camera camera(16.0 / 9.0, 1, 1);
  camera.transform.Translate(0, 0, -5);
  return camera;
}

static RayTracer SetUpRayTracer() {
  RayTracer::Options options;
  options.image_width = 320;
  options.image_height = 180;
  options.r_freq = 0;
  options.g_freq = 1;
  options.b_freq = 2;
  return RayTracer(options);
}

static TgaImageFile::Header SetUpTgaImageFileHeader() {
  TgaImageFile::Header header;

  header.id_length = 0;
  header.color_map_type = TgaImageFile::ColorMapType::kNoColorMap;
  header.image_type = TgaImageFile::ImageType::kTrueColor;
  header.color_map_first = 0;
  header.color_map_length = 0;
  header.color_map_depth = 0;
  header.origin_x = 0;
  header.origin_y = 0;
  header.width = 320;
  header.height = 180;
  header.pixel_depth = 24;
  header.alpha_depth = 0;
  header.horizontal_direction =
      TgaImageFile::HorizontalDirection::kLeftToRight;
  header.vertical_direction =
      TgaImageFile::VerticalDirection::kTopToBottom;

  return header;
}

int main(int argc, char **argv) {
  if (argc <= 1) {
    std::cout << "Usage: " << argv[0] << " <filename>\n";
    return 0;
  }

  Scene scene = SetUpScene();
  Camera camera = SetUpCamera();
  RayTracer renderer = SetUpRayTracer();

  auto job_status = renderer.Render(scene, camera);
  auto image_data = job_status->result.get();

  TgaImageFile image_file(argv[1], std::ios::out | std::ios::binary);
  image_file.header = SetUpTgaImageFileHeader();
  image_file.image_data = TgaImageFile::RgbToBgr(std::move(image_data));
  image_file.Write();

  return 0;
}

