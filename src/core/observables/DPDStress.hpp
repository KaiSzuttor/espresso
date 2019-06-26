#ifndef OBSERVABLES_DPDSTRESS_HPP
#define OBSERVABLES_DPDSTRESS_HPP

#include "Observable.hpp"
#include "dpd.hpp"
#include "particle_data.hpp"
#include <vector>

namespace Observables {

class DPDStress : public Observable {
public:
  std::size_t n_values() const override { return 9; };
  Utils::Tensor<double> operator()() const override { return dpd_stress(); }
};

} // Namespace Observables

#endif
