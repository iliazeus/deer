// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#include <array>
#include <cmath>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include "file_formats/tga.h"
#include "geometry.h"
#include "optics.h"
#include "renderer.h"
#include "rgb.h"
#include "scene.h"
#include "spectrum.h"
#include "transform.h"

using namespace deer;

static RgbColorProfile SetUpColorProfile() {
  RgbColorProfile profile;
  profile.wavelengths = double3{2, 1, 0};
  profile.min_intensities = double3{0, 0, 0};
  profile.max_intensities = double3{1, 1, 1};
  return profile;
}

static std::shared_ptr<Geometry> MakePyramid(int n_vert = 8) {
  std::vector<std::array<double4, 3>> triangles;
  const double PI = std::acos(-1);
  double4 peak{0, 1, 0, 1};
  for (int i = 0; i < n_vert; i++) {
    double alpha1 = PI*2 * i / n_vert;
    double alpha2 = PI*2 * (i+1) / n_vert;
    double4 a1 = double4{std::cos(alpha1), 0, std::sin(alpha1), 1};
    double4 a2 = double4{std::cos(alpha2), 0, std::sin(alpha2), 1};
    triangles.push_back({peak, a1, a2});
  }
  return std::make_shared<TrianglesGeometry>(std::move(triangles));
}

static Scene SetUpScene() {
  Scene scene;

  auto white_spectrum = Spectrum::MakeConstant(1);
  auto red_spectrum = Spectrum::MakeMonochrome(2, 0.5, 1);
  auto green_spectrum = Spectrum::MakeMonochrome(1, 0.5, 1);
  auto blue_spectrum = Spectrum::MakeMonochrome(0, 0.5, 1);

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

  auto white_material = std::make_shared<Material>();
  white_material->ambiance_spectrum = white_spectrum;
  white_material->diffusion_spectrum = white_spectrum;
  white_material->specular_spectrum = Spectrum::MakeConstant(0);
  white_material->shininess = 0;

  auto sphere_geometry = std::make_shared<UnitSphereGeometry>();
  auto pyramid_geometry = MakePyramid();

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

  scene.Add(std::make_shared<GeometryObject>(
      pyramid_geometry,
      white_material,
      AffineTransform().Scale(1.5, 4, 1.5).Translate(-1.5, -2, -1)));

  scene.Add(std::make_shared<GeometryObject>(
      std::make_shared<XYPlaneGeometry>(),
      white_material,
      AffineTransform().RotateX(std::acos(0)).Translate(0, -2, 0)));

  scene.ambiance_spectrum = Spectrum::MakeConstant(0.2);
  scene.Add(std::make_shared<PointLightSource>(
      double4{-5, 3, -5, 1}, Spectrum::MakeConstant(0.5)));
  scene.Add(std::make_shared<PointLightSource>(
      double4{5, 3, -5, 1}, Spectrum::MakeConstant(0.5)));

  return scene;
}

static Camera SetUpCamera() {
  Camera camera(16.0 / 9.0, 1, 2);
  camera.transform.Translate(0, 0, -10);
  return camera;
}

static RayTracer SetUpRayTracer() {
  RayTracer::Options options;
  options.image_width = 640;
  options.image_height = 360;
  options.color_profile = SetUpColorProfile();
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

