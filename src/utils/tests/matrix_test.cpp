/*
 * Copyright (C) 2018-2019 The ESPResSo project
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

#define BOOST_TEST_MODULE Utils::Mat test
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <utils/Vector.hpp>
#include <utils/matrix.hpp>

BOOST_AUTO_TEST_CASE(matrix) {
  Utils::Matrix<int, 2, 2> mat2{{8, 2}, {3, 4}};
  BOOST_CHECK((mat2(0, 0) == 8));
  BOOST_CHECK((mat2(1, 0) == 3));
  BOOST_CHECK((mat2(0, 1) == 2));
  BOOST_CHECK((mat2(1, 1) == 4));

  Utils::Matrix<int, 3, 3> mat{1, 2, 3, 4, 5, 6, 7, 8, 9};

  Utils::Vector3i diagonal = boost::qvm::diag(mat);
  BOOST_CHECK((diagonal == Utils::Vector3i{1, 5, 9}));

  Utils::Vector3i row = boost::qvm::row<0>(mat);
  BOOST_CHECK((row == Utils::Vector3i{1, 2, 3}));

  Utils::Vector3i col = boost::qvm::col<0>(mat);
  BOOST_CHECK((col == Utils::Vector3i{1, 4, 7}));

  Utils::Matrix<int, 3, 3> diag_mat = boost::qvm::diag_mat(diagonal);
  BOOST_CHECK((diag_mat(0, 0) = 1));
  BOOST_CHECK((*(diag_mat.data()) == 1));
}

BOOST_AUTO_TEST_CASE(identity_matrix) {
  auto const id_mat = Utils::Matrix<int, 2, 2>::identity();
  BOOST_CHECK((id_mat(0, 0) == 1));
  BOOST_CHECK((id_mat(1, 1) == 1));
  BOOST_CHECK((id_mat(0, 1) == 0));
  BOOST_CHECK((id_mat(1, 0) == 0));
}

BOOST_AUTO_TEST_CASE(type_deduction) {
  static_assert(
      std::is_same<boost::qvm::deduce_vec2<Utils::Matrix<int, 2, 2>,
                                           Utils::Vector<int, 2>, 2>::type,
                   Utils::Vector<int, 2>>::value,
      "");
  static_assert(
      std::is_same<boost::qvm::deduce_vec2<Utils::Matrix<int, 2, 2>,
                                           Utils::Vector<double, 2>, 2>::type,
                   Utils::Vector<double, 2>>::value,
      "");
}

BOOST_AUTO_TEST_CASE(matrix_vector) {
  // trivial identity multiplication
  auto const id_mat = Utils::Matrix<int, 2, 2>::identity();
  auto const vec = Utils::Vector<int, 2>{2, 3};
  auto const res = id_mat * vec;
  BOOST_CHECK((res == vec));
  auto const res2 = id_mat.transposed() * vec;
  BOOST_CHECK((res2 == vec));

  // non-trivial multiplication
  auto const mat2 = Utils::Matrix<int, 2, 2>{{1, 2}, {3, 4}};
  auto const vec2 = Utils::Vector<int, 2>{7, 8};
  auto const res_vec = mat2 * vec2;
  BOOST_CHECK((res_vec[0] == 23));
  BOOST_CHECK((res_vec[1] == 53));
}

BOOST_AUTO_TEST_CASE(transposed) {
  auto const mat = Utils::Matrix<int, 2, 2>{{1, 2}, {3, 4}};
  auto const id_mat_T = mat.transposed();
  BOOST_CHECK((id_mat_T(1, 0) == 2) and (mat(1, 0) == 3));
  BOOST_CHECK((id_mat_T(0, 1) == 3) and (mat(0, 1) == 2));
}

BOOST_AUTO_TEST_CASE(shape) {
  auto const mat = Utils::Matrix<int, 2, 3>{{1, 2}, {3, 4}, {5, 6}};
  BOOST_CHECK((mat.shape().first == 2));
  BOOST_CHECK((mat.shape().second == 3));
  BOOST_CHECK((mat.transposed().shape().first == 3));
  BOOST_CHECK((mat.transposed().shape().second == 2));
}