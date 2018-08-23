// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#include <cmath>
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

  auto white_spectrum = Spectrum::MakeConstant(255);
  auto red_spectrum = Spectrum::MakeMonochrome(0, 0.5, 255);
  auto green_spectrum = Spectrum::MakeMonochrome(1, 0.5, 255);
  auto blue_spectrum = Spectrum::MakeMonochrome(2, 0.5, 255);

  auto red_material = std::make_shared<Material>();
  red_material->ambiance_spectrum = red_spectrum;
  red_material->diffusion_spectrum = red_spectrum;
  red_material->specular_spectrum = white_spectrum;
  red_material->shininess = 5;

  auto green_material = std::make_shared<Material>();
  green_material->ambiance_spectrum = green_spectrum;
  green_material->diffusion_spectrum = green_spectrum;
  green_material->specular_spectrum = white_spectrum;
  green_material->shininess = 5;

  auto blue_material = std::make_shared<Material>();
  blue_material->ambiance_spectrum = blue_spectrum;
  blue_material->diffusion_spectrum = blue_spectrum;
  blue_material->specular_spectrum = white_spectrum;
  blue_material->shininess = 5;

  auto sphere_geometry = std::make_shared<UnitSphereGeometry>();

  scene.Add(std::make_shared<GeometryObject>(
      sphere_geometry,
      red_material,
      AffineTransform().Translate(-3, 0, 0)));
  scene.Add(std::make_shared<GeometryObject>(
      sphere_geometry,
      green_material,
      AffineTransform().Translate(0, 0, 0)));
  scene.Add(std::make_shared<GeometryObject>(
      sphere_geometry,
      blue_material,
      AffineTransform().Translate(3, 0, 0)));

  auto white_material = std::make_shared<Material>();
  white_material->ambiance_spectrum = white_spectrum;
  white_material->diffusion_spectrum = white_spectrum;
  white_material->specular_spectrum = Spectrum::MakeConstant(0);
  white_material->shininess = 0;

  scene.Add(std::make_shared<GeometryObject>(
      std::make_shared<XYPlaneGeometry>(),
      white_material,
      AffineTransform().RotateX(std::acos(0)).Translate(0, -2, 0)));

  scene.ambiance_spectrum = Spectrum::MakeConstant(0.2);
  scene.Add(std::make_shared<PointLightSource>(
      double4{-3, 3, -3, 1}, Spectrum::MakeConstant(0.5)));

  return scene;
}

static Camera SetUpCamera() {
  Camera camera(16.0 / 9.0, 1, 1);
  camera.transform.Translate(0, 0, -5);
  return camera;
}

static RayTracer SetUpRayTracer() {
  RayTracer::Options options;
  options.image_width = 640;
  options.image_height = 360;
  options.r_wavelength = 0;
  options.g_wavelength = 1;
  options.b_wavelength = 2;
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
  header.width = 640;
  header.height = 360;
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

