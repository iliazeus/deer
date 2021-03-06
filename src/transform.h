// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#ifndef DEER_TRANSFORM_H_
#define DEER_TRANSFORM_H_

#include "matrix.h"
#include "vector.h"

namespace deer {

class AffineTransform {
 public:
  AffineTransform();  // id transform
  explicit AffineTransform(double4x4 matrix);
  static AffineTransform FromInverseMatrix(double4x4 inv_matrix);

  const double4x4 &matrix() const { return matrix_; }
  const double4x4 &inverse_matrix() const { return inv_matrix_; }

  static AffineTransform MakeTranslation(double x, double y, double z);
  static AffineTransform MakeTranslation(double3 v);
  static AffineTransform MakeTranslation(double4 v);

  static AffineTransform MakeScaling(double factor);
  static AffineTransform MakeScaling(double x, double y, double z);
  static AffineTransform MakeScaling(double3 v);

  static AffineTransform MakeRotationX(double angle);
  static AffineTransform MakeRotationY(double angle);
  static AffineTransform MakeRotationZ(double angle);

  AffineTransform &Compose(const AffineTransform &other);

  AffineTransform &Translate(double x, double y, double z);
  AffineTransform &Translate(double3 v);
  AffineTransform &Translate(double4 v);

  AffineTransform &SetOrigin(double x, double y, double z);
  AffineTransform &SetOrigin(double3 point);
  AffineTransform &SetOrigin(double4 point);

  AffineTransform &Scale(double factor);
  AffineTransform &Scale(double factor_x, double factor_y, double factor_z);
  AffineTransform &Scale(double3 factors);

  AffineTransform &RotateX(double angle);
  AffineTransform &RotateY(double angle);
  AffineTransform &RotateZ(double angle);

  double4 Apply(double4 v) const { return matrix_ * v; }
  double4 ApplyInverse(double4 v) const { return inv_matrix_ * v; }

 protected:
  double4x4 matrix_, inv_matrix_;

  AffineTransform(double4x4 matrix, double4x4 inv_matrix);
};

namespace test {

bool near_equal(const AffineTransform &a, const AffineTransform &b);

}  // namespace test

}  // namespace deer

#endif  // DEER_TRANSFORM_H_
