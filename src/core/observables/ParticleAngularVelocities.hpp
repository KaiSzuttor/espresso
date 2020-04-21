/*
 * Copyright (C) 2010-2019 The ESPResSo project
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
#ifndef OBSERVABLES_PARTICLEANGULARVELOCITIES_HPP
#define OBSERVABLES_PARTICLEANGULARVELOCITIES_HPP

#include "PidObservable.hpp"
#include "integrate.hpp"

#include "rotation.hpp"
#include <vector>

namespace Observables {

class ParticleAngularVelocities : public PidObservable {
public:
  using PidObservable::PidObservable;

  std::vector<double>
  evaluate(Utils::Span<const Particle *const> particles) const override {
    std::vector<double> res(n_values());

#ifdef ROTATION
    size_t i = 0;
    for (auto p : particles) {
      const Utils::Vector3d omega =
          convert_vector_body_to_space(*p, p->m.omega);
      res[3 * i + 0] = omega[0];
      res[3 * i + 1] = omega[1];
      res[3 * i + 2] = omega[2];
      i++;
    }
#endif
    return res;
  }

  std::vector<size_t> shape() const override { return {ids().size(), 3}; }
};

} // namespace Observables
#endif
