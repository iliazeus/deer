// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#include "tga.h"

#include <cstdint>
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <utility>

namespace deer {

namespace {

static_assert(sizeof(char) == sizeof(std::uint8_t));
static_assert(static_cast<std::uint8_t>(static_cast<char>(
    std::uint8_t{0xFF})) == std::uint8_t{0xFF});

std::uint8_t ReadUint8(std::istream &stream) {
  char val;
  stream.get(val);
  return static_cast<std::uint8_t>(val);
}

std::uint16_t ReadUint16LE(std::istream &stream) {
  uint8_t little = ReadUint8(stream);
  uint8_t big = ReadUint8(stream);
  return (std::uint16_t{big} << 8) | little;
}

void WriteUint8(std::ostream &stream, std::uint8_t val) {
  stream.put(static_cast<char>(val));
}

void WriteUint16LE(std::ostream &stream, std::uint16_t val) {
  WriteUint8(stream, static_cast<std::uint8_t>(val & 0xFF));
  WriteUint8(stream, static_cast<std::uint8_t>(val >> 8));
}

std::size_t HeaderOffset() { return 0; }
std::size_t HeaderSize() { return 18; }

std::size_t IdOffset(const TgaImageFile::Header &header) {
  return HeaderOffset() + HeaderSize();
}
std::size_t IdSize(const TgaImageFile::Header &header) {
  return header.id_length;
}

std::size_t ColorMapDataOffset(const TgaImageFile::Header &header) {
  return IdOffset(header) + IdSize(header);
}
std::size_t ColorMapDataSize(const TgaImageFile::Header &header) {
  return header.color_map_length * (std::size_t{header.color_map_depth} / 8);
}

std::size_t ImageDataOffset(const TgaImageFile::Header &header) {
  return ColorMapDataOffset(header) + ColorMapDataSize(header);
}
std::size_t ImageDataSize(const TgaImageFile::Header &header) {
  return std::size_t{header.width} * std::size_t{header.height} *
      (std::size_t{header.pixel_depth} / 8);
}

TgaImageFile::Header ReadHeaderFromStream(
    std::istream &stream) {
  TgaImageFile::Header header;

  header.id_length = ReadUint8(stream);
  header.color_map_type =
      static_cast<TgaImageFile::ColorMapType>(ReadUint8(stream));
  header.image_type =
      static_cast<TgaImageFile::ImageType>(ReadUint8(stream));
  header.color_map_first = ReadUint16LE(stream);
  header.color_map_length = ReadUint16LE(stream);
  header.color_map_depth = ReadUint8(stream);
  header.origin_x = ReadUint16LE(stream);
  header.origin_y = ReadUint16LE(stream);
  header.width = ReadUint16LE(stream);
  header.height = ReadUint16LE(stream);
  header.pixel_depth = ReadUint8(stream);

  std::uint8_t descriptor = ReadUint8(stream);
  header.alpha_depth = 0xF & descriptor;
  header.horizontal_direction =
      static_cast<TgaImageFile::HorizontalDirection>(0x10 & descriptor);
  header.vertical_direction =
      static_cast<TgaImageFile::VerticalDirection>(0x20 & descriptor);

  return header;
}

void WriteHeaderToStream(std::ostream &stream,
    const TgaImageFile::Header &header) {
  WriteUint8(stream, header.id_length);
  WriteUint8(stream, static_cast<std::uint8_t>(header.color_map_type));
  WriteUint8(stream, static_cast<std::uint8_t>(header.image_type));
  WriteUint16LE(stream, header.color_map_first);
  WriteUint16LE(stream, header.color_map_length);
  WriteUint8(stream, header.color_map_depth);
  WriteUint16LE(stream, header.origin_x);
  WriteUint16LE(stream, header.origin_y);
  WriteUint16LE(stream, header.width);
  WriteUint16LE(stream, header.height);
  WriteUint8(stream, header.pixel_depth);

  std::uint8_t descriptor = header.alpha_depth;
  descriptor |= static_cast<std::uint8_t>(header.horizontal_direction);
  descriptor |= static_cast<std::uint8_t>(header.vertical_direction);
  WriteUint8(stream, descriptor);
}

std::string ReadIdFromStream(std::istream &stream,
    const TgaImageFile::Header &header) {
  std::string id;
  id.resize(IdSize(header));
  stream.read(id.data(), IdSize(header));
  return id;
}

void WriteIdToStream(std::ostream &stream, const std::string id) {
  stream.write(id.data(), id.size());
}

std::vector<std::uint8_t> ReadColorMapDataFromStream(
    std::istream &stream, const TgaImageFile::Header &header) {
  auto data_size = ColorMapDataSize(header);
  std::vector<std::uint8_t> data(data_size);
  stream.read(reinterpret_cast<char *>(data.data()), data_size);
  return data;
}

void WriteColorMapDataToStream(std::ostream &stream,
    const std::vector<std::uint8_t> data) {
  stream.write(reinterpret_cast<const char *>(data.data()), data.size());
}

std::vector<std::uint8_t> ReadImageDataFromStream(
    std::istream &stream, const TgaImageFile::Header &header) {
  auto data_size = ImageDataSize(header);
  std::vector<std::uint8_t> data(data_size);
  stream.read(reinterpret_cast<char *>(data.data()), data_size);
  return data;
}

void WriteImageDataToStream(std::ostream &stream,
    const std::vector<std::uint8_t> data) {
  stream.write(reinterpret_cast<const char *>(data.data()), data.size());
}

}  // namespace

void TgaImageFile::ReadHeader() {
  file_.seekg(HeaderOffset());
  header = ReadHeaderFromStream(file_);
}

void TgaImageFile::WriteHeader() {
  file_.seekp(HeaderOffset());
  WriteHeaderToStream(file_, header);
}

void TgaImageFile::ReadId() {
  if (!has_read_header_) ReadHeader();
  file_.seekg(IdOffset(header));
  id = ReadIdFromStream(file_, header);
}

void TgaImageFile::WriteId() {
  if (!has_read_header_) ReadHeader();
  file_.seekp(IdOffset(header));
  WriteIdToStream(file_, id);
}

void TgaImageFile::ReadColorMapData() {
  if (!has_read_header_) ReadHeader();
  file_.seekg(ColorMapDataOffset(header));
  color_map_data = ReadColorMapDataFromStream(file_, header);
}

void TgaImageFile::WriteColorMapData() {
  if (!has_read_header_) ReadHeader();
  file_.seekp(ColorMapDataOffset(header));
  WriteColorMapDataToStream(file_, color_map_data);
}

void TgaImageFile::ReadImageData() {
  if (!has_read_header_) ReadHeader();
  file_.seekg(ImageDataOffset(header));
  image_data = ReadImageDataFromStream(file_, header);
}

void TgaImageFile::WriteImageData() {
  if (!has_read_header_) ReadHeader();
  file_.seekp(ImageDataOffset(header));
  WriteImageDataToStream(file_, image_data);
}

void TgaImageFile::ReadFromStream(std::istream &stream) {
  header = ReadHeaderFromStream(stream);
  id = ReadIdFromStream(stream, header);
  color_map_data = ReadColorMapDataFromStream(stream, header);
  image_data = ReadImageDataFromStream(stream, header);
}

void TgaImageFile::WriteToStream(std::ostream &stream) {
  WriteHeaderToStream(stream, header);
  WriteIdToStream(stream, id);
  WriteColorMapDataToStream(stream, color_map_data);
  WriteImageDataToStream(stream, image_data);
}

void TgaImageFile::Read() {
  file_.seekg(0);
  ReadFromStream(file_);
}

void TgaImageFile::Write() {
  file_.seekp(0);
  WriteToStream(file_);
}

std::vector<std::uint8_t> TgaImageFile::BgrToRgb(
    std::vector<std::uint8_t> data) {
  for (std::size_t i = 0; i < data.size() / 3; i++) {
    std::swap(data[3*i], data[3*i + 2]);
  }
  return data;
}

std::vector<std::uint8_t> TgaImageFile::RgbToBgr(
    std::vector<std::uint8_t> data) {
  for (std::size_t i = 0; i < data.size() / 3; i++) {
    std::swap(data[3*i], data[3*i + 2]);
  }
  return data;
}

std::vector<std::uint8_t> TgaImageFile::BgraToRgba(
    std::vector<std::uint8_t> data) {
  for (std::size_t i = 0; i < data.size() / 4; i++) {
    std::swap(data[4*i], data[4*i + 2]);
  }
  return data;
}

std::vector<std::uint8_t> TgaImageFile::RgbaToBgra(
    std::vector<std::uint8_t> data) {
  for (std::size_t i = 0; i < data.size() / 4; i++) {
    std::swap(data[4*i], data[4*i + 2]);
  }
  return data;
}

bool operator==(const TgaImageFile::Header &a,
                const TgaImageFile::Header &b) {
  return a.id_length == b.id_length
      && a.color_map_type == b.color_map_type
      && a.image_type == b.image_type
      && a.color_map_first == b.color_map_first
      && a.color_map_length == b.color_map_length
      && a.color_map_depth == b.color_map_depth
      && a.origin_x == b.origin_x
      && a.origin_y == b.origin_y
      && a.width == b.width
      && a.height == b.height
      && a.pixel_depth == b.pixel_depth
      && a.alpha_depth == b.alpha_depth
      && a.horizontal_direction == b.horizontal_direction
      && a.vertical_direction == b.vertical_direction;
}

}  // namespace deer
