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
  Utils::Matrix<int, 3, 3> mat{{1, 2, 3, 4, 5, 6, 7, 8, 9}};

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
