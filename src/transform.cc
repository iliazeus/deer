// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#include "transform.h"

#include <cmath>

#include "matrix.h"
#include "vector.h"

namespace deer {

AffineTransform::AffineTransform()
    : matrix_(double4x4::id())
    , inv_matrix_(double4x4::id()) {}

AffineTransform::AffineTransform(double4x4 matrix, double4x4 inv_matrix)
    : matrix_(matrix)
    , inv_matrix_(inv_matrix) {}

AffineTransform::AffineTransform(double4x4 matrix)
    : matrix_(matrix)
    , inv_matrix_(matrix.inverse()) {}

AffineTransform AffineTransform::FromInverseMatrix(double4x4 inv_matrix) {
  return AffineTransform(inv_matrix.inverse(), inv_matrix);
}

AffineTransform &AffineTransform::Compose(const AffineTransform &other) {
  matrix_ = other.matrix_ * matrix_;
  inv_matrix_ = inv_matrix_ * other.inv_matrix_;
  return *this;
}

// TODO(iliazeus): optimize these (without matrix multiplication)
AffineTransform &AffineTransform::Translate(double4 v) {
  return Compose(MakeTranslation(v));
}

AffineTransform &AffineTransform::Translate(double x, double y, double z) {
  return Translate(double4{x, y, z, 0});
}

AffineTransform &AffineTransform::Translate(double3 v) {
  return Translate(double4{v[0], v[1], v[2], 0});
}

AffineTransform &AffineTransform::SetOrigin(double4 p) {
  matrix_[3] = p;
  inv_matrix_[3] = -p; inv_matrix_[3][3] = 1;
  return *this;
}

AffineTransform &AffineTransform::SetOrigin(double3 p) {
  return SetOrigin(double4{p[0], p[1], p[2], 1});
}

AffineTransform &AffineTransform::SetOrigin(double x, double y, double z) {
  return SetOrigin(double4{x, y, z, 1});
}

AffineTransform &AffineTransform::Scale(double3 v) {
  return Compose(MakeScaling(v));
}

AffineTransform &AffineTransform::Scale(double factor) {
  return Scale(double3{factor, factor, factor});
}

AffineTransform &AffineTransform::Scale(double x, double y, double z) {
  return Scale(double3{x, y, z});
}

AffineTransform &AffineTransform::RotateX(double angle) {
  return Compose(MakeRotationX(angle));
}

AffineTransform &AffineTransform::RotateY(double angle) {
  return Compose(MakeRotationY(angle));
}

AffineTransform &AffineTransform::RotateZ(double angle) {
  return Compose(MakeRotationZ(angle));
}

AffineTransform AffineTransform::MakeTranslation(double4 v) {
  AffineTransform result;
  result.matrix_[3] += v;
  result.inv_matrix_[3] -= v;
  return result;
}

AffineTransform AffineTransform::MakeTranslation(double3 v) {
  return MakeTranslation(double4{v[0], v[1], v[2], 0});
}

AffineTransform AffineTransform::MakeTranslation(
    double x, double y, double z) {
  return MakeTranslation(double4{x, y, z, 0});
}

AffineTransform AffineTransform::MakeScaling(double3 factors) {
  AffineTransform result;
  result.matrix_[0][0] = factors[0];
  result.matrix_[1][1] = factors[1];
  result.matrix_[2][2] = factors[2];
  result.inv_matrix_[0][0] = 1 / factors[0];
  result.inv_matrix_[1][1] = 1 / factors[1];
  result.inv_matrix_[2][2] = 1 / factors[2];
  return result;
}

AffineTransform AffineTransform::MakeScaling(double x, double y, double z) {
  return MakeScaling(double3{x, y, z});
}

AffineTransform AffineTransform::MakeScaling(double factor) {
  return MakeScaling(double3{factor, factor, factor});
}

AffineTransform AffineTransform::MakeRotationZ(double angle) {
  AffineTransform result;
  result.matrix_[0][0] = std::cos(angle);
  result.matrix_[0][1] = std::sin(angle);
  result.matrix_[1][0] = -std::sin(angle);
  result.matrix_[1][1] = std::cos(angle);
  result.inv_matrix_[0][0] = std::cos(-angle);
  result.inv_matrix_[0][1] = std::sin(-angle);
  result.inv_matrix_[1][0] = -std::sin(-angle);
  result.inv_matrix_[1][1] = std::cos(-angle);
  return result;
}

AffineTransform AffineTransform::MakeRotationY(double angle) {
  AffineTransform result;
  result.matrix_[2][2] = std::cos(angle);
  result.matrix_[2][1] = std::sin(angle);
  result.matrix_[1][2] = -std::sin(angle);
  result.matrix_[1][1] = std::cos(angle);
  result.inv_matrix_[2][2] = std::cos(-angle);
  result.inv_matrix_[2][1] = std::sin(-angle);
  result.inv_matrix_[1][2] = -std::sin(-angle);
  result.inv_matrix_[1][1] = std::cos(-angle);
  return result;
}

AffineTransform AffineTransform::MakeRotationX(double angle) {
  AffineTransform result;
  result.matrix_[1][1] = std::cos(angle);
  result.matrix_[1][2] = std::sin(angle);
  result.matrix_[2][1] = -std::sin(angle);
  result.matrix_[2][2] = std::cos(angle);
  result.inv_matrix_[1][1] = std::cos(-angle);
  result.inv_matrix_[1][2] = std::sin(-angle);
  result.inv_matrix_[2][1] = -std::sin(-angle);
  result.inv_matrix_[2][2] = std::cos(-angle);
  return result;
}

namespace test {

bool near_equal(const AffineTransform &a, const AffineTransform &b) {
  return near_equal(a.matrix(), b.matrix())
      && near_equal(a.inverse_matrix(), b.inverse_matrix());
}

}  // namespace test

}  // namespace deer
