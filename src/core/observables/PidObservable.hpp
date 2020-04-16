/*
 * Copyright (C) 2016-2019 The ESPResSo project
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

#ifndef OBSERVABLES_PIDOBSERVABLE_HPP
#define OBSERVABLES_PIDOBSERVABLE_HPP

#include <observables/observable.hpp>

#include "Observable.hpp"
#include "Particle.hpp"
#include "ParticleTraits.hpp"

#include <utils/Span.hpp>

#include <vector>

namespace CoreObservables {

/** %Particle-based observable.
 *
 *  Base class for observables extracting raw data from particle subsets and
 *  returning either the data or a statistic derived from it.
 */
class PidObservable : virtual public Observable {
  /** Identifiers of particles measured by this observable */
  std::vector<int> m_ids;

  virtual std::vector<double>
  evaluate(Utils::Span<const Particle *const> particles) const = 0;

public:
  explicit PidObservable(std::vector<int> ids) : m_ids(std::move(ids)) {}
  std::vector<double> operator()() const final;

  std::vector<int> &ids() { return m_ids; }
  std::vector<int> const &ids() const { return m_ids; }
};

namespace detail {
template <class T> struct shape_impl;

template <> struct shape_impl<double> {
  static std::vector<size_t> eval(size_t /* n_part */) { return {1}; }
};
template <class _, size_t N> struct shape_impl<Utils::Vector<_, N>> {
  static std::vector<size_t> eval(size_t /* n_part */) { return {N}; }
};
template <class T> struct shape_impl<std::vector<T>> {
  static std::vector<size_t> eval(size_t n_part) {
    std::vector<size_t> ret{n_part};
    boost::copy(shape_impl<T>::eval(n_part), std::back_inserter(ret));

    return ret;
  }
};

template <class OutputIterator> void flatten(double value, OutputIterator out) {
  *out++ = value;
}

template <class OutputIterator>
void flatten(Utils::Vector3d const &vec, OutputIterator out) {
  for (auto const &elem : vec) {
    flatten(elem, out);
  }
}

inline std::vector<double> flatten(Utils::Vector3d const &vec) {
  return vec.as_vector();
}

template <class T> std::vector<double> flatten(std::vector<T> const &vec) {
  std::vector<double> ret;
  for (auto const &elem : vec) {
    flatten(elem, std::back_inserter(ret));
  }
  return ret;
}

} // namespace detail

template <class ObsType> class ParticleObservable : public PidObservable {
public:
  using PidObservable::PidObservable;
  std::vector<size_t> shape() const override {
    using std::declval;

    return detail::shape_impl<decltype(declval<ObsType>()(
        declval<Utils::Span<const Particle *const>>()))>::eval(ids().size());
  }

  std::vector<double>
  evaluate(Utils::Span<const Particle *const> particles) const override {
    return detail::flatten(ObsType{}(particles));
  }
};

} // namespace CoreObservables
#endif
