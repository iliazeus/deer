// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#ifndef DEER_TGA_H_
#define DEER_TGA_H_

#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace deer {

// Represents TGA version 1.0 image file.
// Refer to tga.txt for file format description.
class TgaImageFile {
 public:
  enum struct ColorMapType : std::uint8_t {
    kNoColorMap = 0,
    kHasColorMap = 1,
  };
  enum struct ImageType : std::uint8_t {
    kNoImageData = 0,
    kColorMapped = 1,
    kTrueColor = 2,
    kGrayscale = 3,
    kRleColorMapped = 9,
    kRleTrueColor = 10,
    kRleGrayscale = 11,
  };
  enum struct HorizontalDirection : std::uint8_t {
    kLeftToRight = 0,
    kRightToLeft = 0x10,
  };
  enum struct VerticalDirection : std::uint8_t {
    kBottomToTop = 0,
    kTopToBottom = 0x20,
  };

  struct Header {
    std::uint8_t id_length;
    ColorMapType color_map_type;
    ImageType image_type;
    std::uint16_t color_map_first;
    std::uint16_t color_map_length;
    std::uint8_t color_map_depth;
    std::uint16_t origin_x;
    std::uint16_t origin_y;
    std::uint16_t width;
    std::uint16_t height;
    std::uint8_t pixel_depth;
    std::uint8_t alpha_depth;
    HorizontalDirection horizontal_direction;
    VerticalDirection vertical_direction;
  };

  Header header;
  std::string id;
  std::vector<std::uint8_t> color_map_data;
  std::vector<std::uint8_t> image_data;

  TgaImageFile() = default;
  TgaImageFile(std::string path) : file_(path) {}
  TgaImageFile(std::string path,
               std::ios_base::openmode mode)
      : file_(path, mode) {}
  TgaImageFile(std::fstream &&file) : file_(std::move(file)) {}

  TgaImageFile(const TgaImageFile &) = delete;
  TgaImageFile &operator=(const TgaImageFile &) = delete;
  TgaImageFile(TgaImageFile &&) = default;
  TgaImageFile &operator=(TgaImageFile &&) = default;

  ~TgaImageFile() = default;

  void Read();
  void ReadHeader();
  void ReadId();
  void ReadColorMapData();
  void ReadImageData();
  
  void Write();
  void WriteHeader();
  void WriteId();
  void WriteColorMapData();
  void WriteImageData();

  void ReadFromStream(std::istream &stream);
  void WriteToStream(std::ostream &stream);

  // TODO(iliazeus): consider implementing an RLE encoder/decoder

  // 24-bit TGA files store colors as BGR
  static std::vector<std::uint8_t> BgrToRgb(std::vector<std::uint8_t>);
  static std::vector<std::uint8_t> RgbToBgr(std::vector<std::uint8_t>);

  // 32-bit TGA files store colors as BGRA
  static std::vector<std::uint8_t> BgraToRgba(std::vector<std::uint8_t>);
  static std::vector<std::uint8_t> RgbaToBgra(std::vector<std::uint8_t>);

 private:
  std::fstream file_;
  bool has_read_header_ = false;
};

}  // namespace deer

#endif  // DEER_TGA_H_
