#include "config.hpp"
#include "grid.hpp"
#include "lattice.hpp"
#include "lb_interface.hpp"
#include "utils/Vector.hpp"

#if defined(LB) || defined(LB_GPU)

namespace {
template <typename Op>
void lattice_interpolation(const Vector3i &ind, Vector<6, double> const &delta,
                           Op &&op) {
  auto const agrid = lb_lbfluid_get_agrid();
  Vector3i neighbor_ind;
  for (int z = 0; z < 2; z++) {
    for (int y = 0; y < 2; y++) {
      for (int x = 0; x < 2; x++) {
        neighbor_ind[0] = ind[0] + x;
        neighbor_ind[1] = ind[1] + y;
        neighbor_ind[2] = ind[2] + z;
        for (int i = 0; i < 3; ++i) {
          if (neighbor_ind[i] == box_l[i] / agrid)
            neighbor_ind[i] = 0;
        }
        auto const w = delta[3 * x + 0] * delta[3 * y + 1] * delta[3 * z + 2];
        op(neighbor_ind, w);
      }
    }
  }
}
} // namespace

const Vector3d
lb_lbinterpolation_get_interpolated_velocity(const Vector3d &pos, bool global=false) {
  Vector6d delta{};
  Vector3i ind{};

  std::tie(ind, delta) =
      Lattice::map_position_to_lattice_global(pos, lb_lbfluid_get_agrid());

  Vector3d u_interpolated{};
  lattice_interpolation(
      ind, delta, [&u_interpolated, &global](const Vector3i &neighbor_ind, double w) {
        auto const u_node = lb_lbnode_get_u(neighbor_ind, global);
        u_interpolated += w * u_node;
      });
  return u_interpolated;
}

namespace {
template <typename Op>
void old_lattice_interpolation(Lattice const &lattice, Vector3d const &pos,
                           Op &&op) {
  Vector<8, std::size_t> node_index;
  Vector6d delta{};

  /* determine elementary lattice cell surrounding the particle
     and the relative position of the particle in this cell */
  std::tie(node_index, delta) = lattice.map_position_to_lattice(pos);

  for (int z = 0; z < 2; z++) {
    for (int y = 0; y < 2; y++) {
      for (int x = 0; x < 2; x++) {
        auto &index = node_index[(z * 2 + y) * 2 + x];
        auto const w = delta[3 * x + 0] * delta[3 * y + 1] * delta[3 * z + 2];

        op(index, w);
      }
    }
  }
}
} // namespace

void lb_lbinterpolation_add_force_density(const Vector3d &pos,
                                          const Vector3d &force_density) {
  auto const lattice = lb_lbfluid_get_lattice();
  old_lattice_interpolation(
      lattice, pos, [&force_density](Lattice::index_t index, double w) {
        lb_lbnode_add_force_density(index, w*force_density);
      });
}

#endif
