#ifndef LB_VELOCITY_INTERPOLATION_HPP
#define LB_VELOCITY_INTERPOLATION_HPP
#include "config.hpp"
#include "utils/Vector.hpp"

#if defined(LB) || defined(LB_GPU)
/*
 * @brief Interpolate the fluid velocity.
 *
 * @param pos Position
 * @retval Interpolated velocity in MD units.
 */
const Vector3d
lb_lbinterpolation_get_interpolated_velocity(const Vector3d &pos, bool global);
/**
 * @brief Add a force density to the fluid at the given position.
 */
void lb_lbinterpolation_add_force_density(const Vector3d &p,
                                          const Vector3d &force_density);

#endif

#endif
