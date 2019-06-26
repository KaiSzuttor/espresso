/*
  Copyright (C) 2016-2018 The ESPResSo project

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
#ifndef OBSERVABLES_CYLINDRICALLBVELOCITYPROFILE_HPP
#define OBSERVABLES_CYLINDRICALLBVELOCITYPROFILE_HPP

#include "CylindricalLBProfileObservable.hpp"
#include <utils/tensor.hpp>

namespace Observables {
class CylindricalLBVelocityProfile : public CylindricalLBProfileObservable {
public:
  using CylindricalLBProfileObservable::CylindricalLBProfileObservable;
  Utils::Tensor<double> operator()() const override;
  std::size_t n_values() const override {
    return 3 * n_r_bins * n_phi_bins * n_z_bins;
  }
};

} // Namespace Observables

#endif
