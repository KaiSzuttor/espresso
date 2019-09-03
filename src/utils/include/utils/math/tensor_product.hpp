/*
Copyright (C) 2010-2018 The ESPResSo project

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
#ifndef UTILS_MATH_TENSOR_PRODUCT_HPP
#define UTILS_MATH_TENSOR_PRODUCT_HPP

#include "utils/Vector.hpp"

#include <algorithm>

namespace Utils {
template <typename T, size_t N, size_t M>
Vector<Vector<T, M>, N> tensor_product(const Vector<T, N> &x,
                                       const Vector<T, M> &y) {
  Vector<Vector<T, M>, N> ret;

  std::transform(x.begin(), x.end(), ret.begin(),
                 [&y](const T &x_i) { return x_i * y; });

  return ret;
}

template<std::size_t N, std::size_t M, typename E1, typename E2>
auto tensor_product(VectorExpression<E1, N> const &x, VectorExpression<E2, M> const &y) {
  using R = decltype(std::declval<typename E1::value_type>() * std::declval<typename E2::value_type>());
  Vector<Vector<R, M>, N> ret;
  for (int i=0; i<N; ++i) {
    ret[i] = (~x)[i] * (~y);
  }
  return ret;
}

/*
 * @brief Overload if left operand is scalar.
 */
template <typename T, size_t N>
Vector<T, N> tensor_product(const T &x, const Vector<T, N> &y) {
  return x * y;
}
} // namespace Utils

#endif
