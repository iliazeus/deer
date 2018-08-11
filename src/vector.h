// Copyright 2018 Ilia Pozdnyakov
// This file is distributed under the MIT license.
// See the LICENSE.txt file for details.

#ifndef DEER_VECTOR_H_
#define DEER_VECTOR_H_

#include <array>
#include <cmath>
#include <cstdint>
#include <exception>

namespace deer {

template<class T, std::size_t N>
struct Vector {
  using scalar_type = T;

  using value_type = T;
  using reference = T &;
  using const_reference = const T &;
  using iterator = typename std::array<T, N>::iterator;
  using const_iterator = typename std::array<T, N>::const_iterator;
  using difference_type = std::ptrdiff_t;
  using size_type = std::size_t;

  std::array<T, N> elements_;

  constexpr reference operator[](size_type i) {
    return elements_[i];
  }
  constexpr const_reference operator[](size_type i) const {
    return elements_[i];
  }

  constexpr reference at(size_type i) {
    return elements_.at(i);
  }
  constexpr const_reference at(size_type i) const {
    return elements_.at(i);
  }

  constexpr iterator begin() {
    return elements_.begin();
  }
  constexpr const_iterator begin() const {
    return elements_.begin();
  }
  constexpr const_iterator cbegin() const {
    return elements_.cbegin();
  }

  constexpr iterator end() {
    return elements_.end();
  }
  constexpr const_iterator end() const {
    return elements_.end();
  }
  constexpr const_iterator cend() const {
    return elements_.cend();
  }

  constexpr bool empty() const { return elements_.empty(); }
  constexpr size_type size() const { return elements_.size(); }
  constexpr size_type max_size() const { return elements_.max_size(); }

  void fill(const value_type &val) {
    elements_.fill(val);
  }

  void swap(Vector &other) {
    elements_.swap(other.elements_);
  }
  friend void swap(Vector a, Vector b) {
    a.swap(b);
  }

  friend bool operator==(const Vector &a, const Vector &b) {
    return a.elements_ == b.elements_;
  }
  friend bool operator!=(const Vector &a, const Vector &b) {
    return a.elements_ != b.elements_;
  }
  friend bool operator<(const Vector &a, const Vector &b) {
    return a.elements_ < b.elements_;
  }
  friend bool operator>(const Vector &a, const Vector &b) {
    return a.elements_ > b.elements_;
  }
  friend bool operator<=(const Vector &a, const Vector &b) {
    return a.elements_ <= b.elements_;
  }
  friend bool operator>=(const Vector &a, const Vector &b) {
    return a.elements_ >= b.elements_;
  }

  // TODO(iliazeus): compile-time bounds checking
  constexpr reference x() { return elements_[0]; }
  constexpr const_reference x() const { return elements_[0]; }
  constexpr reference r() { return elements_[0]; }
  constexpr const_reference r() const { return elements_[0]; }
  constexpr reference u() { return elements_[0]; }
  constexpr const_reference u() const { return elements_[0]; }

  constexpr reference y() { return elements_[1]; }
  constexpr const_reference y() const { return elements_[1]; }
  constexpr reference g() { return elements_[1]; }
  constexpr const_reference g() const { return elements_[1]; }
  constexpr reference v() { return elements_[1]; }
  constexpr const_reference v() const { return elements_[1]; }

  constexpr reference z() { return elements_[2]; }
  constexpr const_reference z() const { return elements_[2]; }
  constexpr reference b() { return elements_[2]; }
  constexpr const_reference b() const { return elements_[2]; }

  constexpr reference t() { return elements_[3]; }
  constexpr const_reference t() const { return elements_[3]; }
  constexpr reference a() { return elements_[3]; }
  constexpr const_reference a() const { return elements_[3]; }

  Vector operator-() const {
    Vector result;
    for (size_type i = 0; i < size(); i++) {
      result.elements_[i] = -elements_[i];
    }
    return result;
  }

  Vector &operator+=(const Vector &other) {
    for (size_type i = 0; i < size(); i++) {
      elements_[i] += other.elements_[i];
    }
    return *this;
  }
  Vector &operator-=(const Vector &other) {
    for (size_type i = 0; i < size(); i++) {
      elements_[i] -= other.elements_[i];
    }
    return *this;
  }
  Vector &operator*=(const Vector &other) {
    for (size_type i = 0; i < size(); i++) {
      elements_[i] *= other.elements_[i];
    }
    return *this;
  }
  Vector &operator/=(const Vector &other) {
    for (size_type i = 0; i < size(); i++) {
      elements_[i] /= other.elements_[i];
    }
    return *this;
  }

  friend Vector operator+(const Vector &a, const Vector &b) {
    Vector result = a;
    result += b;
    return result;
  }
  friend Vector operator-(const Vector &a, const Vector &b) {
    Vector result = a;
    result -= b;
    return result;
  }
  friend Vector operator*(const Vector &a, const Vector &b) {
    Vector result = a;
    result *= b;
    return result;
  }
  friend Vector operator/(const Vector &a, const Vector &b) {
    Vector result = a;
    result /= b;
    return result;
  }

  Vector &operator*=(const scalar_type &alpha) {
    for (size_type i = 0; i < size(); i++) {
      elements_[i] *= alpha;
    }
    return *this;
  }
  Vector &operator/=(const scalar_type &alpha) {
    for (size_type i = 0; i < size(); i++) {
      elements_[i] /= alpha;
    }
    return *this;
  }

  friend Vector operator*(const Vector &v, const scalar_type &alpha) {
    Vector result = v;
    result *= alpha;
    return result;
  }
  friend Vector operator/(const Vector &v, const scalar_type &alpha) {
    Vector result = v;
    result /= alpha;
    return result;
  }

  friend Vector operator*(const scalar_type &alpha, const Vector &v) {
    Vector result = v;
    result *= alpha;
    return result;
  }

  scalar_type length2() const {
    scalar_type result = 0;
    for (size_type i = 0; i < size(); i++) {
      result += elements_[i] * elements_[i];
    }
    return result;
  }

  float flength() const { return std::sqrt((float) length2()); }
  double length() const { return std::sqrt((double) length2()); }
  long double llength() const { return std::sqrt((long double) length2()); }

  // TODO(iliazeus): signed volume

  static Vector zero() {
    Vector result;
    result.fill(0);
    return result;
  }
};

template<class T, std::size_t N>
T length2(const Vector<T, N> &v) {
  return v.length2();
}

template<class T, std::size_t N>
float flength(const Vector<T, N> &v) { return v.flength(); }
template<class T, std::size_t N>
double length(const Vector<T, N> &v) { return v.length(); }
template<class T, std::size_t N>
long double llength(const Vector<T, N> &v) { return v.llength(); }

template<class T, std::size_t N>
T dot(const Vector<T, N> &a, const Vector<T, N> &b) {
  T result = 0;
  for (std::size_t i = 0; i < N; i++) {
    result += a.elements_[i] * b.elements_[i];
  }
  return result;
}

template<class T>
Vector<T, 3> cross(const Vector<T, 3> &a, const Vector<T, 3> &b) {
  Vector<T, 3> result = {
    a.y() * b.z() - a.z() * b.y(),
    a.z() * b.x() - a.x() * b.z(),
    a.x() * b.y() - a.y() * b.x()
  };
  return result;
}

using int2 = Vector<int, 2>;
using int3 = Vector<int, 3>;
using int4 = Vector<int, 4>;

using float2 = Vector<float, 2>;
using float3 = Vector<float, 3>;
using float4 = Vector<float, 4>;

using double2 = Vector<double, 2>;
using double3 = Vector<double, 3>;
using double4 = Vector<double, 4>;

template<class T, std::size_t N>
bool NearEqualForTesting(
    const Vector<T, N> &a, const Vector<T, N> &b) {
  const double EPS = 1e-6;
  for (std::size_t i = 0; i < N; i++) {
    if (std::abs(a[i] - b[i]) > EPS) return false;
  }
  return true;
}

}  // namespace deer

#endif  // DEER_VECTOR_H_
