// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license
// See the LICENSE.txt file for details.

#ifndef DEER_MATRIX_H_
#define DEER_MATRIX_H_

#include <cstdint>
#include <array>
#include <exception>
#include <type_traits>

#include "vector.h"

namespace deer {

// Column-first square number matrix
template<class T, std::size_t N>
struct Matrix {
  using scalar_type = T;

  using value_type = Vector<T, N>;
  using reference = Vector<T, N> &;
  using const_reference = const Vector<T, N> &;
  using iterator = typename std::array<Vector<T, N>, N>::iterator;
  using const_iterator = typename std::array<Vector<T, N>, N>::const_iterator;
  using difference_type = std::ptrdiff_t;
  using size_type = std::size_t;

  std::array<Vector<T, N>, N> vectors_;

  constexpr reference operator[](size_type i) {
    return vectors_[i];
  }
  constexpr const_reference operator[](size_type i) const {
    return vectors_[i];
  }

  constexpr reference at(size_type i) {
    return vectors_.at(i);
  }
  constexpr const_reference at(size_type i) const {
    return vectors_.at(i);
  }

  constexpr iterator begin() {
    return vectors_.begin();
  }
  constexpr const_iterator begin() const {
    return vectors_.begin();
  }
  constexpr const_iterator cbegin() const {
    return vectors_.cbegin();
  }

  constexpr iterator end() {
    return vectors_.end();
  }
  constexpr const_iterator end() const {
    return vectors_.end();
  }
  constexpr const_iterator cend() const {
    return vectors_.cend();
  }

  constexpr bool empty() const { return vectors_.empty(); }
  constexpr size_type size() const { return vectors_.size(); }
  constexpr size_type max_size() const { return vectors_.max_size(); }

  void swap(Matrix &other) {
    vectors_.swap(other.vectors_);
  }
  friend void swap(Matrix a, Matrix b) {
    a.swap(b);
  }

  friend bool operator==(const Matrix &a, const Matrix &b) {
    return a.vectors_ == b.vectors_;
  }
  friend bool operator!=(const Matrix &a, const Matrix &b) {
    return a.vectors_ != b.vectors_;
  }
  friend bool operator<(const Matrix &a, const Matrix &b) {
    return a.vectors_ < b.vectors_;
  }
  friend bool operator>(const Matrix &a, const Matrix &b) {
    return a.vectors_ > b.vectors_;
  }
  friend bool operator<=(const Matrix &a, const Matrix &b) {
    return a.vectors_ <= b.vectors_;
  }
  friend bool operator>=(const Matrix &a, const Matrix &b) {
    return a.vectors_ >= b.vectors_;
  }

  Matrix operator-() const {
    Matrix result;
    for (size_type i = 0; i < size(); i++) {
      result.vectors_[i] = -vectors_[i];
    }
    return result;
  }

  Matrix &operator+=(const Matrix &other) {
    for (size_type i = 0; i < size(); i++) {
      vectors_[i] += other.vectors_[i];
    }
    return *this;
  }
  Matrix &operator-=(const Matrix &other) {
    for (size_type i = 0; i < size(); i++) {
      vectors_[i] -= other.vectors_[i];
    }
    return *this;
  }

  friend Matrix operator+(const Matrix &a, const Matrix &b) {
    Matrix result = a;
    result += b;
    return result;
  }
  friend Matrix operator-(const Matrix &a, const Matrix &b) {
    Matrix result = a;
    result -= b;
    return result;
  }

  Matrix &operator*=(const scalar_type &alpha) {
    for (size_type i = 0; i < size(); i++) {
      vectors_[i] *= alpha;
    }
    return *this;
  }
  Matrix &operator/=(const scalar_type &alpha) {
    for (size_type i = 0; i < size(); i++) {
      vectors_[i] /= alpha;
    }
    return *this;
  }

  friend Matrix operator*(const Matrix &v, const scalar_type &alpha) {
    Matrix result = v;
    result *= alpha;
    return result;
  }
  friend Matrix operator/(const Matrix &v, const scalar_type &alpha) {
    Matrix result = v;
    result /= alpha;
    return result;
  }

  friend Matrix operator*(const scalar_type &alpha, const Matrix &v) {
    Matrix result = v;
    result *= alpha;
    return result;
  }

  friend Vector<T, N> operator*(const Matrix &m, const Vector<T, N> &v) {
    Vector<T, N> result;
    for (size_type i = 0; i < N; i++) {
      result[i] = 0;
      for (size_type j = 0; j < N; j++) {
        result[i] += m[j][i] * v[j];
      }
    }
    return result;
  }

  friend Matrix operator*(const Matrix &a, const Matrix &b) {
    Matrix result;
    for (size_type i = 0; i < N; i++) {
      result[i] = a * b[i];
    }
    return result;
  }
  Matrix &operator*=(const Matrix &other) {
    Matrix result = *this * other;
    *this = result;
    return *this;
  }

  Matrix transpose() const;
  scalar_type trace() const;
  scalar_type det() const;
  Matrix inverse() const;

  static Matrix zero() {
    Matrix result;
    for (size_type i = 0; i < N; i++) {
      result[i].fill(0);
    }
    return result;
  }
  static Matrix id() {
    Matrix result;
    for (size_type i = 0; i < N; i++) {
      result[i].fill(0);
      result[i][i] = 1;
    }
    return result;
  }
};

template<class T, std::size_t N>
Matrix<T, N> transpose(const Matrix<T, N> &m) {
  Matrix<T, N> result;
  for (std::size_t i = 0; i < N; i++) {
    for (std::size_t j = 0; j < N; j++) {
      result[i][j] = m[j][i];
    }
  }
  return result;
}

template<class T, std::size_t N>
Matrix<T, N> Matrix<T, N>::transpose() const {
  return deer::transpose(*this);
}

template<class T, std::size_t N>
T trace(const Matrix<T, N> &m) {
  T result = 0;
  for (std::size_t i = 0; i < N; i++) {
    result += m[i][i];
  }
  return result;
}

template<class T, std::size_t N>
T Matrix<T, N>::trace() const {
  return deer::trace(*this);
}

template<class T, std::size_t N>
T det(const Matrix<T, N> &m) {
  throw std::logic_error("det() is currently only implemented"
      "for matrix sizes up to 3");
}

template<class T>
T det(const Matrix<T, 1> &m) { return m[0][0]; }

template<class T>
T det(const Matrix<T, 2> &m) {
  return m[0][0] * m[1][1] - m[0][1] * m[1][0];
}

template<class T>
T det(const Matrix<T, 3> &m) {
  return m[0][0] * m[1][1] * m[2][2]
       + m[1][0] * m[2][1] * m[0][2]
       + m[2][0] * m[0][1] * m[1][2]
       - m[0][0] * m[2][1] * m[1][2]
       - m[1][0] * m[0][1] * m[2][2]
       - m[2][0] * m[1][1] * m[0][2];
}

template<class T, std::size_t N>
T Matrix<T, N>::det() const {
  return deer::det(*this);
}

template<class T, std::size_t N>
Matrix<T, N> inverse(const Matrix<T, N> &m) {
  throw std::logic_error("inverse() is currently only implemented"
      "for matrix sizes up to 3");
}

template<class T>
Matrix<T, 1> inverse(const Matrix<T, 1> &m) {
  return Matrix<T, 1>{1 / m[0][0]};
}

template<class T>
struct Matrix<T, 2> inverse(const Matrix<T, 2> &m) {
  return Matrix<T, 2>{
    Vector<T, 2>{m[1][1], -m[0][1]},
    Vector<T, 2>{-m[1][0], m[0][0]}
  } / det(m);
}

// TODO(iliazeus): research into efficient matrix inversion methods
template<class T>
Matrix<T, 3> inverse(const Matrix<T, 3> &m) {
  // Cayley-Hamilton decomposition
  T trace_m = trace(m);
  Matrix<T, 3> m2 = m * m;
  T trace_m2 = trace(m2);
  return (((trace_m * trace_m - trace_m2) / 2) * Matrix<T, 3>::id()
      - m * trace_m + m2) / det(m);
}

template<class T, std::size_t N>
Matrix<T, N> Matrix<T, N>::inverse() const {
  return deer::inverse(*this);
}

// TODO(iliazeus): det() and inverse() for matrices of size 4

using int2x2 = Matrix<int, 2>;
using int3x3 = Matrix<int, 3>;
using int4x4 = Matrix<int, 4>;

using float2x2 = Matrix<float, 2>;
using float3x3 = Matrix<float, 3>;
using float4x4 = Matrix<float, 4>;

using double2x2 = Matrix<double, 2>;
using double3x3 = Matrix<double, 3>;
using double4x4 = Matrix<double, 4>;

}  // namespace deer

#endif  // DEER_MATRIX_H_
