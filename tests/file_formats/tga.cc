// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#include "../../src/file_formats/tga.h"

#include <cstdint>
#include <cstring>
#include <random>
#include <sstream>
#include <vector>

#include <gtest/gtest.h>

using deer::TgaImageFile;

class TgaImageFileTest : public ::testing::Test {};

static std::vector<std::uint8_t> GenerateRandomData(std::size_t size,
    int seed) {
  std::minstd_rand engine(seed);
  std::uniform_int_distribution<std::uint8_t> rand;
  std::vector<std::uint8_t> result;
  for (std::size_t i = 0; i < size; i++) result.push_back(rand(engine));
  return result;
}

static TgaImageFile GenerateTestImage(int seed) {
  TgaImageFile result;

  result.id = "test_image";

  result.header.id_length = result.id.length();
  result.header.color_map_type = TgaImageFile::ColorMapType::kHasColorMap;
  result.header.image_type = TgaImageFile::ImageType::kTrueColor;
  result.header.color_map_first = 0;
  result.header.color_map_length = 256;
  result.header.color_map_depth = 16;
  result.header.origin_x = 0;
  result.header.origin_y = 0;
  result.header.width = 320;
  result.header.height = 200;
  result.header.pixel_depth = 8;
  result.header.alpha_depth = 0;
  result.header.horizontal_direction =
      TgaImageFile::HorizontalDirection::kLeftToRight;
  result.header.vertical_direction =
      TgaImageFile::VerticalDirection::kTopToBottom;

  result.color_map_data = GenerateRandomData(256 * 2, seed);

  result.image_data = GenerateRandomData(320 * 200, seed);

  return std::move(result);
}

TEST_F(TgaImageFileTest, ReadsWhatItWrites) {
  std::stringstream stream;

  TgaImageFile output_image = GenerateTestImage(1337);
  output_image.WriteToStream(stream);

//  stream.flush();
//  stream.sync();
//  stream.seekg(0);

  TgaImageFile input_image;
  input_image.ReadFromStream(stream);

  EXPECT_EQ(output_image.header, input_image.header);
  EXPECT_EQ(output_image.id, input_image.id);
  EXPECT_EQ(output_image.color_map_data, input_image.color_map_data);
  EXPECT_EQ(output_image.image_data, input_image.image_data);
}
