/*
 * Copyright (C) 2010-2020 The ESPResSo project
 *
 * This file is part of ESPResSo.
 *
 * ESPResSo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ESPResSo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SRC_UTILS_INCLUDE_UTILS_MATRIX_HPP
#define SRC_UTILS_INCLUDE_UTILS_MATRIX_HPP

#include "utils/Array.hpp"
#include "utils/Vector.hpp"
#include "utils/flatten.hpp"

#include <array>

// These includes need to come first due to ADL reasons.
// clang-format off
#include <boost/qvm/mat_operations.hpp>
#include <boost/qvm/vec_mat_operations.hpp>
#include <boost/qvm/vec_operations.hpp>
// clang-format on

#include <boost/qvm/deduce_mat.hpp>
#include <boost/qvm/deduce_scalar.hpp>
#include <boost/qvm/deduce_vec.hpp>
#include <boost/qvm/map_mat_mat.hpp>
#include <boost/qvm/map_mat_vec.hpp>
#include <boost/qvm/map_vec_mat.hpp>
#include <boost/qvm/mat.hpp>
#include <boost/qvm/mat_access.hpp>
#include <boost/qvm/mat_traits.hpp>

namespace Utils {

template <typename T, std::size_t Rows, std::size_t Cols> struct Matrix {
  using container = Utils::Array<T, Cols * Rows>;
  using pointer = typename container::pointer;
  using const_pointer = typename container::const_pointer;
  using iterator = typename container::iterator;
  using const_iterator = typename container::const_iterator;
  using value_type = typename container::value_type;
  using reference = typename container::reference;
  using const_reference = typename container::const_reference;

  container m_data;

  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive &ar, const unsigned int version) {
    ar &m_data;
  }

  Matrix() = default;
  Matrix(std::initializer_list<T> init_list) {
    assert(init_list.size() == Rows * Cols);
    std::copy(init_list.begin(), init_list.end(), begin());
  }
  Matrix(std::initializer_list<std::initializer_list<T>> init_list) {
    assert(init_list.size() == Cols);
    std::array<T, Rows * Cols> tmp;
    Utils::flatten(init_list, tmp.begin());
    std::copy(tmp.begin(), tmp.end(), begin());
  }
  constexpr value_type operator()(std::size_t row, std::size_t col) const {
    assert(row >= 0 and row < Rows);
    assert(col >= 0 and col < Cols);
    return m_data[Cols * row + col];
  }
  constexpr reference operator()(std::size_t row, std::size_t col) {
    assert(row >= 0 and row < Rows);
    assert(col >= 0 and col < Cols);
    return m_data[Cols * row + col];
  }
  constexpr pointer data() { return m_data.data(); }
  constexpr const_pointer data() const noexcept { return m_data.data(); }
  constexpr iterator begin() noexcept { return m_data.begin(); };
  constexpr const_iterator begin() const noexcept { return m_data.begin(); };
  constexpr iterator end() noexcept { return m_data.end(); };
  constexpr const_iterator end() const noexcept { return m_data.end(); };

  static Matrix<T, Rows, Cols> identity() {
    static_assert(Rows == Cols,
                  "Identity matrix only defined for square matrices.");
    return boost::qvm::identity_mat<T, Rows>();
  }

  static Matrix<T, Rows, Cols> diagonal(Utils::Vector<T, Rows> const &v) {
    static_assert(Rows == Cols, "Diagonal matrix has to be a square matrix.");
    return boost::qvm::diag_mat(v);
  }

  Matrix<T, Cols, Rows> transposed() const {
    return boost::qvm::transposed(*this);
  }

  constexpr std::pair<std::size_t, std::size_t> shape() const noexcept {
    return {Rows, Cols};
  }
};

using boost::qvm::operator*;
using boost::qvm::operator+;
using boost::qvm::operator+=;
using boost::qvm::operator==;

/**
 * @brief Flatten a matrix to a linear vector.
 *
 * @param m Input Matrix
 * @return Flat vector with elements of the matrix.
 */
template <class T, size_t N, size_t M>
Vector<T, N * M> flatten(Matrix<T, N, M> const &m) {
  Vector<T, N * M> ret;

  for (size_t i = 0; i < N; i++)
    for (size_t j = 0; j < M; j++)
      ret[i * M + j] = m(j, i);

  return ret;
}

/**
 * @brief Meta function to turns a Matrix<T, 1, 1> into T.
 */
template <typename T, std::size_t M, std::size_t N>
struct decay_to_scalar<Matrix<T, M, N>> {
  using type = Matrix<T, M, N>;
};

template <typename T> struct decay_to_scalar<Matrix<T, 1, 1>> {
  using type = T;
};

} // namespace Utils

namespace boost {
namespace qvm {

template <typename T, std::size_t Rows, std::size_t Cols>
struct mat_traits<Utils::Matrix<T, Rows, Cols>> {
  using mat_type = typename Utils::Matrix<T, Rows, Cols>;
  static int const rows = Rows;
  static int const cols = Cols;
  using scalar_type = T;

  template <std::size_t R, std::size_t C>
  static inline scalar_type read_element(mat_type const &m) {
    static_assert(R >= 0 and R < Rows, "Invalid row index.");
    static_assert(C >= 0 and C < Cols, "Invalid column index.");
    return m(R, C);
  }

  template <std::size_t R, std::size_t C>
  static inline scalar_type &write_element(mat_type &m) {
    static_assert(R >= 0 and R < Rows, "Invalid row index.");
    static_assert(C >= 0 and C < Cols, "Invalid column index.");
    return m(R, C);
  }

  static inline scalar_type read_element_idx(std::size_t r, std::size_t c,
                                             mat_type const &m) {
    assert(r >= 0 and r < Rows);
    assert(c >= 0 and c < Cols);
    return m(r, c);
  }
  static inline scalar_type &write_element_idx(std::size_t r, std::size_t c,
                                               mat_type &m) {
    assert(r >= 0 and r < Rows);
    assert(c >= 0 and c < Cols);
    return m(r, c);
  }
};

template <typename T, typename U>
struct deduce_vec2<Utils::Matrix<T, 2, 2>, Utils::Vector<U, 2>, 2> {
  using type = Utils::Vector<std::common_type_t<T, U>, 2>;
};

template <typename T, typename U>
struct deduce_vec2<Utils::Matrix<T, 3, 3>, Utils::Vector<U, 3>, 3> {
  using type = Utils::Vector<std::common_type_t<T, U>, 3>;
};

template <typename T, typename U>
struct deduce_vec2<Utils::Matrix<T, 4, 4>, Utils::Vector<U, 4>, 4> {
  using type = Utils::Vector<std::common_type_t<T, U>, 4>;
};

template <typename T> struct deduce_mat<Utils::Matrix<T, 3, 4>, 3, 4> {
  using type = Utils::Matrix<T, 3, 4>;
};

template <typename T> struct deduce_mat<Utils::Matrix<T, 4, 3>, 4, 3> {
  using type = Utils::Matrix<T, 4, 3>;
};

template <typename T, typename U>
struct deduce_mat2<Utils::Matrix<T, 4, 3>, Utils::Matrix<U, 4, 3>, 4, 3> {
  using type = Utils::Matrix<std::common_type_t<T, U>, 4, 3>;
};

template <typename T, typename U>
struct deduce_scalar<Utils::Matrix<T, 3, 3>, Utils::Matrix<U, 3, 3>> {
  using type = std::common_type_t<T, U>;
};

} // namespace qvm
} // namespace boost

#endif