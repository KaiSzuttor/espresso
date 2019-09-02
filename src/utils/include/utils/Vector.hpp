/*
  Copyright (C) 2014-2018 The ESPResSo project

  This file is part of ESPResSo.

  ESPResSo is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ESPResSo is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <algorithm>
#include <cassert>
#include <cmath>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <numeric>
#include <vector>

#include "utils/Array.hpp"

namespace Utils {

template <typename T, std::size_t N> struct VectorExpression {
  T &operator~() { return static_cast<T &>(*this); }
  const T &operator~() const { return static_cast<const T &>(*this); }
};

template <typename LHS, typename RHS, std::size_t N>
class SumExpression : public VectorExpression<SumExpression<LHS, RHS, N>, N> {
  const LHS &m_lhs;
  const RHS &m_rhs;

public:
  SumExpression(LHS const &lhs, RHS const &rhs) : m_lhs(lhs), m_rhs(rhs) {}
  auto operator[](std::size_t i) const { return m_lhs[i] + m_rhs[i]; }
};

template <typename LHS, typename RHS, std::size_t N>
class DiffExpression : public VectorExpression<DiffExpression<LHS, RHS, N>, N> {
  const LHS &m_lhs;
  const RHS &m_rhs;

public:
  DiffExpression(LHS const &lhs, RHS const &rhs) : m_lhs(lhs), m_rhs(rhs) {}
  auto operator[](std::size_t i) const { return m_lhs[i] - m_rhs[i]; }
};

template <typename LHS, typename RHS, std::size_t N>
class HadamardExpression
    : public VectorExpression<HadamardExpression<LHS, RHS, N>, N> {
  const LHS &m_lhs;
  const RHS &m_rhs;

public:
  HadamardExpression(LHS const &lhs, RHS const &rhs) : m_lhs(lhs), m_rhs(rhs) {}
  auto operator[](std::size_t i) const { return m_lhs[i] * m_rhs[i]; }
};

template <typename LHS, typename RHS, std::size_t N>
class ScalarMultExpression
    : public VectorExpression<ScalarMultExpression<LHS, RHS, N>, N> {
  const LHS &m_lhs;
  const RHS &m_rhs;

public:
  ScalarMultExpression(LHS const &lhs, RHS const &rhs)
      : m_lhs(lhs), m_rhs(rhs) {}
  auto operator[](std::size_t i) const { return m_lhs * m_rhs[i]; }
};

template <typename LHS, typename RHS, std::size_t N>
class ScalarDivExpression
    : public VectorExpression<ScalarDivExpression<LHS, RHS, N>, N> {
  const LHS &m_lhs;
  const RHS &m_rhs;

public:
  ScalarDivExpression(LHS const &lhs, RHS const &rhs)
      : m_lhs(lhs), m_rhs(rhs) {}
  auto operator[](std::size_t i) const { return m_lhs[i] / m_rhs; }
};

template <typename T, std::size_t N>
class MinusExpression : public VectorExpression<MinusExpression<T, N>, N> {
  const T &m_lhs;

public:
  MinusExpression(T const &lhs) : m_lhs(lhs) {}
  auto operator[](std::size_t i) const { return -m_lhs[i]; }
};

template <typename T, std::size_t N>
class NotExpression : public VectorExpression<NotExpression<T, N>, N> {
  const T &m_lhs;

public:
  NotExpression(T const &lhs) : m_lhs(lhs) {}
  auto operator[](std::size_t i) const { return not(m_lhs[i]); }
};

template <typename T, std::size_t N>
class SqrtExpression : public VectorExpression<SqrtExpression<T, N>, N> {
  const T &m_lhs;

public:
  SqrtExpression(T const &lhs) : m_lhs(lhs) {}
  auto operator[](std::size_t i) const { return std::sqrt(m_lhs[i]); }
};

template <typename L, typename R, std::size_t N>
inline auto operator+(VectorExpression<L, N> const &lhs,
                      VectorExpression<R, N> const &rhs) {
  return SumExpression<L, R, N>(~lhs, ~rhs);
}

template <typename L, typename R, std::size_t N>
inline auto operator-(VectorExpression<L, N> const &lhs,
                      VectorExpression<R, N> const &rhs) {
  return DiffExpression<L, R, N>(~lhs, ~rhs);
}

template <typename T, std::size_t N>
inline auto operator-(VectorExpression<T, N> const &lhs) {
  return MinusExpression<T, N>(~lhs);
}

template <typename T, std::size_t N>
inline auto operator!(VectorExpression<T, N> const &lhs) {
  return NotExpression<T, N>(~lhs);
}

/* Scalar multiplication */
template <typename E, typename Scalar, std::size_t N,
          typename =
              std::enable_if_t<std::is_arithmetic<std::decay_t<Scalar>>::value>>
inline auto operator*(VectorExpression<E, N> const &a, Scalar const &b) {
  return ScalarMultExpression<Scalar, E, N>(b, ~a);
}

template <typename E, typename Scalar, std::size_t N,
          typename =
              std::enable_if_t<std::is_arithmetic<std::decay_t<Scalar>>::value>>
inline auto operator*(Scalar const &b, VectorExpression<E, N> const &a) {
  return a * b;
}

/* Inner product */
template <typename E1, typename E2, std::size_t N>
auto operator*(VectorExpression<E1, N> const &a,
               VectorExpression<E2, N> const &b) {
  auto res = (~a)[0] * (~b)[0];
  for (int i = 1; i < N; ++i) {
    res += (~a)[i] * (~b)[i];
  }
  return res;
}

/* Scalar division */
template <typename E, typename Scalar, std::size_t N,
          typename =
              std::enable_if_t<std::is_arithmetic<std::decay_t<Scalar>>::value>>
inline auto operator/(VectorExpression<E, N> const &a, Scalar const &b) {
  return ScalarDivExpression<E, Scalar, N>(~a, b);
}

/* Componentwise square root */
template <typename E, std::size_t N>
auto sqrt(VectorExpression<E, N> const &a) {
  return SqrtExpression<E, N>(~a);
}

template <typename T, std::size_t N>
class Vector : public Array<T, N>, public VectorExpression<Vector<T, N>, N> {
  using Base = Array<T, N>;

public:
  using Base::Base;
  using Array<T, N>::at;
  using Array<T, N>::operator[];
  using Array<T, N>::front;
  using Array<T, N>::back;
  using Array<T, N>::data;
  using Array<T, N>::begin;
  using Array<T, N>::cbegin;
  using Array<T, N>::end;
  using Array<T, N>::cend;
  using Array<T, N>::empty;
  using Array<T, N>::size;
  using Array<T, N>::max_size;
  using Array<T, N>::fill;
  using Array<T, N>::broadcast;
  Vector() = default;
  Vector(Vector const &) = default;

  template <typename E> Vector(VectorExpression<E, N> const &e) {
    for (std::size_t i = 0; i < N; ++i) {
      operator[](i) = (~e)[i];
    }
  }

  Vector &operator=(Vector const &) = default;

  template <typename E> Vector &operator=(VectorExpression<E, N> const &e) {
    for (std::size_t i = 0; i < N; ++i) {
      operator[](i) = (~e)[i];
    }
    return *this;
  }

  void swap(Vector &rhs) { std::swap_ranges(begin(), end(), rhs.begin()); }

private:
  constexpr void copy_init(T const *first, T const *last) {
    auto it = begin();
    while (first != last) {
      *it++ = *first++;
    }
  }

public:
  explicit constexpr Vector(T const (&v)[N]) : Base() {
    copy_init(std::begin(v), std::end(v));
  }

  constexpr Vector(std::initializer_list<T> v) : Base() {
    if (N != v.size()) {
      throw std::length_error(
          "Construction of Vector from Container of wrong length.");
    }

    copy_init(v.begin(), v.end());
  }

  template <typename InputIterator>
  Vector(InputIterator first, InputIterator last) {
    if (std::distance(first, last) == N) {
      std::copy_n(first, N, begin());
    } else {
      throw std::length_error(
          "Construction of Vector from Container of wrong length.");
    }
  }

  /**
   * @brief Create a vector that has all entries set to
   *         one value.
   */
  static Vector<T, N> broadcast(T const &s) {
    Vector<T, N> ret;
    std::fill(ret.begin(), ret.end(), s);

    return ret;
  }

  std::vector<T> as_vector() const { return std::vector<T>(begin(), end()); }

  operator std::vector<T>() const { return as_vector(); }

  template <class U> explicit operator Vector<U, N>() const {
    Vector<U, N> ret;

    std::transform(begin(), end(), ret.begin(),
                   [](auto e) { return static_cast<U>(e); });

    return ret;
  }

  /*
   * @brief Normalize the vector.
   *
   * Normalize the vector by its length,
   * if not zero, otherwise the vector is unchanged.
   */

  inline Vector &normalize() {
    auto const l = norm(*this);
    if (l > T(0)) {
      for (int i = 0; i < N; i++)
        this->operator[](i) /= l;
    }

    return *this;
  }
};

template <class T> using Vector3 = Vector<T, 3>;

template <size_t N> using VectorXd = Vector<double, N>;
using Vector2d = VectorXd<2>;
using Vector3d = VectorXd<3>;
using Vector4d = VectorXd<4>;
using Vector6d = VectorXd<6>;
using Vector9d = VectorXd<9>;
using Vector19d = VectorXd<19>;

template <size_t N> using VectorXf = Vector<float, N>;
using Vector3f = VectorXf<3>;

template <size_t N> using VectorXi = Vector<int, N>;
using Vector3i = VectorXi<3>;

template <class T, size_t N, size_t M> using Matrix = Vector<Vector<T, M>, N>;

namespace detail {
template <size_t N, typename T, typename U, typename Op>
auto binary_op(Vector<T, N> const &a, Vector<U, N> const &b, Op op) {
  using std::declval;

  using R = decltype(op(declval<T>(), declval<U>()));
  Vector<R, N> ret;

  std::transform(std::begin(a), std::end(a), std::begin(b), std::begin(ret),
                 op);

  return ret;
}

template <size_t N, typename T, typename E, typename Op>
Vector<T, N> &binary_op_assign(Vector<T, N> &a, VectorExpression<E, N> const &b,
                               Op op) {
  for (int i = 0; i < N; ++i) {
    a[i] = op(a[i], (~b)[i]);
  }
  return a;
}

template <size_t N, typename T, typename Op>
constexpr bool all_of(Vector<T, N> const &a, Vector<T, N> const &b, Op op) {
  for (int i = 0; i < a.size(); i++) {
    /* Short circuit */
    if (!static_cast<bool>(op(a[i], b[i]))) {
      return false;
    }
  }

  return true;
}

template <size_t N, typename T, typename E, typename Op>
constexpr bool all_of(VectorExpression<E, N> const &a, Vector<T, N> const &b,
                      Op op) {
  for (int i = 0; i < N; i++) {
    /* Short circuit */
    if (!static_cast<bool>(op((~a)[i], b[i]))) {
      return false;
    }
  }

  return true;
}

template <size_t N, typename T, typename E, typename Op>
constexpr bool all_of(Vector<T, N> const &a, VectorExpression<E, N> const &b,
                      Op op) {
  return all_of(b, a, op);
}

} // namespace detail

template <size_t N, typename T>
constexpr bool operator==(Vector<T, N> const &a, Vector<T, N> const &b) {
  return detail::all_of(a, b, std::equal_to<T>());
}

template <size_t N, typename T, typename E>
constexpr bool operator==(Vector<T, N> const &a,
                          VectorExpression<E, N> const &b) {
  return detail::all_of(a, b, std::equal_to<T>());
}

template <size_t N, typename T, typename E>
constexpr bool operator==(VectorExpression<E, N> const &a,
                          Vector<T, N> const &b) {
  return detail::all_of(a, b, std::equal_to<T>());
}

template <size_t N, typename T>
constexpr bool operator!=(Vector<T, N> const &a, Vector<T, N> const &b) {
  return not(a == b);
}

template <size_t N, typename T>
constexpr bool operator<(Vector<T, N> const &a, Vector<T, N> const &b) {
  return detail::all_of(a, b, std::less<T>());
}

template <size_t N, typename T>
constexpr bool operator>(Vector<T, N> const &a, Vector<T, N> const &b) {
  return detail::all_of(a, b, std::greater<T>());
}

template <size_t N, typename T>
constexpr bool operator<=(Vector<T, N> const &a, Vector<T, N> const &b) {
  return detail::all_of(a, b, std::less_equal<T>());
}

template <size_t N, typename T>
constexpr bool operator>=(Vector<T, N> const &a, Vector<T, N> const &b) {
  return detail::all_of(a, b, std::greater_equal<T>());
}

template <size_t N, typename T, typename E>
Vector<T, N> &operator+=(Vector<T, N> &a, VectorExpression<E, N> const &b) {
  return detail::binary_op_assign(a, b, std::plus<T>());
}

template <size_t N, typename T, typename E>
Vector<T, N> &operator-=(Vector<T, N> &a, VectorExpression<E, N> const &b) {
  return detail::binary_op_assign(a, b, std::minus<T>());
}

template <size_t N, typename T>
Vector<T, N> &operator*=(Vector<T, N> &b, T const &a) {
  std::transform(std::begin(b), std::end(b), std::begin(b),
                 [a](T const &val) { return a * val; });
  return b;
}

template <typename E, std::size_t N>
auto norm2(VectorExpression<E, N> const &a) {
  return a * a;
}
template <typename E, std::size_t N>
auto norm(VectorExpression<E, N> const &a) {
  return std::sqrt(norm2(a));
}

template <size_t N, typename T>
Vector<T, N> &operator/=(Vector<T, N> &a, T const &b) {
  std::transform(std::begin(a), std::end(a), std::begin(a),
                 [b](T const &val) { return val / b; });
  return a;
}

template <size_t N, typename T, class U,
          class = std::enable_if_t<std::is_integral<T>::value &&
                                   std::is_integral<U>::value>>
auto operator%(Vector<T, N> const &a, Vector<U, N> const &b) {
  using std::declval;
  using R = decltype(declval<T>() % declval<U>());
  Vector<R, N> ret;

  std::transform(std::begin(a), std::end(a), std::begin(b), std::begin(ret),
                 [](T const &ai, U const &bi) { return ai % bi; });

  return ret;
}

template <typename E1, typename E2>
Vector<double, 3> vector_product(VectorExpression<E1, 3> const &a,
                                 VectorExpression<E2, 3> const &b) {
  return {(~a)[1] * (~b)[2] - (~a)[2] * (~b)[1],
          (~a)[2] * (~b)[0] - (~a)[0] * (~b)[2],
          (~a)[0] * (~b)[1] - (~a)[1] * (~b)[0]};
}

template <typename E1, typename E2, size_t N>
auto hadamard_product(VectorExpression<E1, N> const &a,
                      VectorExpression<E2, N> const &b) {
  return HadamardExpression<E1, E2, N>(~a, ~b);
}

/**
 * @brief Meta function to turns a Vector<1, T> into T.
 */
template <typename T> struct decay_to_scalar {};
template <typename T, size_t N> struct decay_to_scalar<Vector<T, N>> {
  using type = Vector<T, N>;
};

template <typename T> struct decay_to_scalar<Vector<T, 1>> { using type = T; };

} // namespace Utils
#endif
