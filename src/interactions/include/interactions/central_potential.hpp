#ifndef SRC_INTERACTIONS_INCLUDE_INTERACTIONS_CENTRAL_POTENTIAL_HPP
#define SRC_INTERACTIONS_INCLUDE_INTERACTIONS_CENTRAL_POTENTIAL_HPP

namespace Interactions {

template <class Inter> struct CentralPotential {
  using value_type = typename Inter::value_type;
  using vector_type = typename Inter::vector_type;

  value_type cut;
  value_type offset;
  value_type shift;
  Inter inter;

  CentralPotential(value_type cutoff, value_type offset, value_type shift)
      : cut(cutoff), offset(offset), shift(shift){};

  bool in_range(value_type r) const {
    auto const r_eff = r - offset;
    return r_eff <= cut and r_eff > 0.0;
  }

  value_type energy(value_type r) const {
    return in_range(r) ? inter.energy(r - offset) + shift : value_type{};
  }

  vector_type force(value_type r, vector_type const &r12) const {
    return in_range(r) ? inter.force(r - offset, r12) : vector_type{};
  }

  value_type cutoff() const { return cut; }
};

} // namespace Interactions

#endif // SRC_INTERACTIONS_INCLUDE_INTERACTIONS_CENTRAL_POTENTIAL_HPP
