/*
 * Copyright (C) 2010-2019 The ESPResSo project
 * Copyright (C) 2002,2003,2004,2005,2006,2007,2008,2009,2010
 *   Max-Planck-Institute for Polymer Research, Theory Group
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
/** \file */

#ifndef BROWNIAN_INLINE_HPP
#define BROWNIAN_INLINE_HPP

#include "random.hpp"
#include "thermostat.hpp"

extern BrownianThermostat brownian;

/** Propagate position: viscous drag driven by conservative forces.
 *  From eq. (14.39) in @cite Schlick2010.
 *  @param[in,out] p              Particle
 *  @param[in]     dt             Time interval
 */
inline void bd_drag(Particle &p, double dt) {
  // The friction tensor Z from the Eq. (14.31) of Schlick2010:
  Thermostat::GammaType local_gamma;

#ifdef BROWNIAN_PER_PARTICLE
  if (p.p.gamma >= Thermostat::GammaType{}) {
    local_gamma = p.p.gamma;
  } else
#endif
  {
    local_gamma = brownian.gamma;
  }

  bool aniso_flag; // particle anisotropy flag

#ifdef PARTICLE_ANISOTROPY
  // Particle frictional isotropy check.
  aniso_flag =
      (local_gamma[0] != local_gamma[1]) || (local_gamma[1] != local_gamma[2]);
#endif

  Utils::Vector3d force_body;
  Utils::Vector3d delta_pos_body, delta_pos_lab;

#ifdef PARTICLE_ANISOTROPY
  if (aniso_flag) {
    force_body = convert_vector_space_to_body(p, p.f.f);
  }
#endif

  for (int j = 0; j < 3; j++) {
    // Second (deterministic) term of the Eq. (14.39) of Schlick2010.
    // Only a conservative part of the force is used here
#ifdef PARTICLE_ANISOTROPY
    if (aniso_flag) {
      delta_pos_body[j] = force_body[j] * dt / (local_gamma[j]);
    } else {
#ifdef EXTERNAL_FORCES
      if (!(p.p.ext_flag & COORD_FIXED(j)))
#endif
      {
        p.r.p[j] += p.f.f[j] * dt / (local_gamma[j]);
      }
    }
#else
#ifdef EXTERNAL_FORCES
    if (!(p.p.ext_flag & COORD_FIXED(j)))
#endif
    {
      p.r.p[j] += p.f.f[j] * dt / (local_gamma);
    }
#endif // PARTICLE_ANISOTROPY
  }
#ifdef PARTICLE_ANISOTROPY
  if (aniso_flag) {
    delta_pos_lab = convert_vector_body_to_space(p, delta_pos_body);
    for (int j = 0; j < 3; j++) {
#ifdef EXTERNAL_FORCES
      if (!(p.p.ext_flag & COORD_FIXED(j)))
#endif
      {
        p.r.p[j] += delta_pos_lab[j];
      }
    }
  }
#endif // PARTICLE_ANISOTROPY
}

/** Set the terminal velocity driven by the conservative forces drag.
 *  From eq. (14.34) in @cite Schlick2010.
 *  @param[in,out] p              Particle
 *  @param[in]     dt             Time interval
 */
inline void bd_drag_vel(Particle &p, double dt) {
  // The friction tensor Z from the eq. (14.31) of Schlick2010:
  Thermostat::GammaType local_gamma;

#ifdef BROWNIAN_PER_PARTICLE
  if (p.p.gamma >= Thermostat::GammaType{}) {
    local_gamma = p.p.gamma;
  } else
#endif
  {
    local_gamma = brownian.gamma;
  }

  bool aniso_flag; // particle anisotropy flag

#ifdef PARTICLE_ANISOTROPY
  // Particle frictional isotropy check.
  aniso_flag =
      (local_gamma[0] != local_gamma[1]) || (local_gamma[1] != local_gamma[2]);
#endif

  Utils::Vector3d force_body;
  Utils::Vector3d vel_body, vel_lab;

#ifdef PARTICLE_ANISOTROPY
  if (aniso_flag) {
    force_body = convert_vector_space_to_body(p, p.f.f);
  }
#endif

  for (int j = 0; j < 3; j++) {
    // First (deterministic) term of the eq. (14.34) of Schlick2010 taking
    // into account eq. (14.35). Only conservative part of the force is used
    // here. NOTE: velocity is assigned here and propagated by thermal part
    // further on top of it
#ifdef PARTICLE_ANISOTROPY
    if (aniso_flag) {
      vel_body[j] = force_body[j] / (local_gamma[j]);
    } else {
#ifdef EXTERNAL_FORCES
      if (!(p.p.ext_flag & COORD_FIXED(j)))
#endif
      {
        p.m.v[j] = p.f.f[j] / (local_gamma[j]);
      }
#ifdef EXTERNAL_FORCES
      else {
        p.m.v[j] = 0.0;
      }
#endif
    }
#else // PARTICLE_ANISOTROPY
#ifdef EXTERNAL_FORCES
    if (!(p.p.ext_flag & COORD_FIXED(j)))
#endif
    {
      p.m.v[j] = p.f.f[j] / (local_gamma);
    }
#endif // PARTICLE_ANISOTROPY
  }

#ifdef PARTICLE_ANISOTROPY
  if (aniso_flag) {
    vel_lab = convert_vector_body_to_space(p, vel_body);
    for (int j = 0; j < 3; j++) {
#ifdef EXTERNAL_FORCES
      if (!(p.p.ext_flag & COORD_FIXED(j)))
#endif
      {
        p.m.v[j] = vel_lab[j];
      }
#ifdef EXTERNAL_FORCES
      else {
        p.m.v[j] = 0.0;
      }
#endif
    }
  }
#endif // PARTICLE_ANISOTROPY
}

/** Propagate the positions: random walk part.
 *  From eq. (14.37) in @cite Schlick2010.
 *  @param[in,out] p          Particle
 *  @param[in] dt             Time interval
 */
void bd_random_walk(Particle &p, double dt) {
  // skip the translation thermalizing for virtual sites unless enabled
  extern bool thermo_virtual;
  if (p.p.is_virtual && !thermo_virtual)
    return;
  // first, set defaults
  Thermostat::GammaType brown_sigma_pos_inv = brownian.sigma_pos_inv;

  // Override defaults if per-particle values for T and gamma are given
#ifdef BROWNIAN_PER_PARTICLE
  auto const constexpr brown_temp_coeff = 2.0;

  if (p.p.gamma >= Thermostat::GammaType{}) {
    // Is a particle-specific temperature also specified?
    if (p.p.T >= 0.) {
      if (p.p.T > 0.0) {
        brown_sigma_pos_inv = sqrt(p.p.gamma / (brown_temp_coeff * p.p.T));
      } else {
        // just an indication of the infinity
        brown_sigma_pos_inv = brownian.gammatype_nan;
      }
    } else
        // default temperature but particle-specific gamma
        if (temperature > 0.0) {
      brown_sigma_pos_inv = sqrt(p.p.gamma / (brown_temp_coeff * temperature));
    } else {
      brown_sigma_pos_inv = brownian.gammatype_nan;
    }
  } // particle-specific gamma
  else {
    // No particle-specific gamma, but is there particle-specific temperature
    if (p.p.T >= 0.) {
      if (p.p.T > 0.0) {
        brown_sigma_pos_inv = sqrt(brownian.gamma / (brown_temp_coeff * p.p.T));
      } else {
        // just an indication of the infinity
        brown_sigma_pos_inv = brownian.gammatype_nan;
      }
    } else {
      // default values for both
      brown_sigma_pos_inv = brownian.sigma_pos_inv;
    }
  }
#endif /* BROWNIAN_PER_PARTICLE */

  bool aniso_flag; // particle anisotropy flag

#ifdef PARTICLE_ANISOTROPY
  // Particle frictional isotropy check.
  aniso_flag = (brown_sigma_pos_inv[0] != brown_sigma_pos_inv[1]) ||
               (brown_sigma_pos_inv[1] != brown_sigma_pos_inv[2]);
#else
  aniso_flag = false;
#endif // PARTICLE_ANISOTROPY

  Utils::Vector3d delta_pos_body, delta_pos_lab;

  // Eq. (14.37) is factored by the Gaussian noise (12.22) with its squared
  // magnitude defined in the second eq. (14.38), Schlick2010.
  Utils::Vector3d noise = Random::v_noise_g<RNGSalt::BROWNIAN_WALK>(
      brownian.rng_counter->value(), p.p.identity);
  for (int j = 0; j < 3; j++) {
#ifdef EXTERNAL_FORCES
    if (!(p.p.ext_flag & COORD_FIXED(j)))
#endif
    {
#ifndef PARTICLE_ANISOTROPY
      if (brown_sigma_pos_inv > 0.0) {
        delta_pos_body[j] = (1.0 / brown_sigma_pos_inv) * sqrt(dt) * noise[j];
      } else {
        delta_pos_body[j] = 0.0;
      }
#else
      if (brown_sigma_pos_inv[j] > 0.0) {
        delta_pos_body[j] =
            (1.0 / brown_sigma_pos_inv[j]) * sqrt(dt) * noise[j];
      } else {
        delta_pos_body[j] = 0.0;
      }
#endif // PARTICLE_ANISOTROPY
    }
  }

#ifdef PARTICLE_ANISOTROPY
  if (aniso_flag) {
    delta_pos_lab = convert_vector_body_to_space(p, delta_pos_body);
  }
#endif

  for (int j = 0; j < 3; j++) {
#ifdef EXTERNAL_FORCES
    if (!(p.p.ext_flag & COORD_FIXED(j)))
#endif
    {
      p.r.p[j] += aniso_flag ? delta_pos_lab[j] : delta_pos_body[j];
    }
  }
}

/** Determine the velocities: random walk part.
 *  From eq. (10.2.16) in @cite Pottier2010.
 *  @param[in, out] p         Particle
 *  @param[in] dt             Time interval
 */
inline void bd_random_walk_vel(Particle &p, double dt) {
  // skip the translation thermalizing for virtual sites unless enabled
  extern bool thermo_virtual;
  if (p.p.is_virtual && !thermo_virtual)
    return;
  // Just a square root of kT, see eq. (10.2.17) and comments in 2 paragraphs
  // afterwards, Pottier2010
  double brown_sigma_vel;

  // Override defaults if per-particle values for T and gamma are given
#ifdef BROWNIAN_PER_PARTICLE
  auto const constexpr brown_temp_coeff = 1.0;
  // Is a particle-specific temperature specified?
  if (p.p.T >= 0.) {
    brown_sigma_vel = sqrt(brown_temp_coeff * p.p.T);
  } else {
    brown_sigma_vel = brownian.sigma_vel;
  }
#else
  // defaults
  brown_sigma_vel = brownian.sigma_vel;
#endif /* BROWNIAN_PER_PARTICLE */

  Utils::Vector3d noise = Random::v_noise_g<RNGSalt::BROWNIAN_INC>(
      brownian.rng_counter->value(), p.identity());
  for (int j = 0; j < 3; j++) {
#ifdef EXTERNAL_FORCES
    if (!(p.p.ext_flag & COORD_FIXED(j)))
#endif
    {
      // Random (heat) velocity is added here. It is already initialized in the
      // terminal drag part. See eq. (10.2.16) taking into account eq. (10.2.18)
      // and (10.2.29), Pottier2010. Note, that the Pottier2010 units system
      // (see Eq. (10.1.1) there) has been adapted towards the ESPResSo and the
      // referenced above Schlick2010 one, which is defined by the eq. (14.31)
      // of Schlick2010. A difference is the mass factor to the friction tensor.
      // The noise is Gaussian according to the convention at p. 237 (last
      // paragraph), Pottier2010.
      p.m.v[j] += brown_sigma_vel * noise[j] / sqrt(p.p.mass);
    }
  }
}

#ifdef ROTATION

/** Propagate quaternions: viscous drag driven by conservative torques.
 *  An analogy of eq. (14.39) in @cite Schlick2010.
 *  @param[in,out] p              Particle
 *  @param[in]     dt             Time interval
 */
void bd_drag_rot(Particle &p, double dt) {
  Thermostat::GammaType local_gamma;

#ifdef BROWNIAN_PER_PARTICLE
  if (p.p.gamma_rot >= Thermostat::GammaType{}) {
    local_gamma = p.p.gamma_rot;
  } else
#endif
  {
    local_gamma = brownian.gamma_rotation;
  }

  Utils::Vector3d dphi = {0.0, 0.0, 0.0};
  for (int j = 0; j < 3; j++) {
#ifdef EXTERNAL_FORCES
    if (!(p.p.ext_flag & COORD_FIXED(j)))
#endif
    {
      // only a conservative part of the torque is used here
#ifndef PARTICLE_ANISOTROPY
      dphi[j] = p.f.torque[j] * dt / (local_gamma);
#else
      dphi[j] = p.f.torque[j] * dt / (local_gamma[j]);
#endif // ROTATIONAL_INERTIA
    }
  } // j
  dphi = mask(p.p.rotation, dphi);
  double dphi_m = dphi.norm();
  if (dphi_m) {
    Utils::Vector3d dphi_u;
    dphi_u = dphi / dphi_m;
    local_rotate_particle_body(p, dphi_u, dphi_m);
  }
}

/** Set the terminal angular velocity driven by the conservative torques drag.
 *  An analogy of the 1st term of eq. (14.34) in @cite Schlick2010.
 *  @param[in,out] p              Particle
 *  @param[in]     dt             Time interval
 */
void bd_drag_vel_rot(Particle &p, double dt) {
  Thermostat::GammaType local_gamma;

#ifdef BROWNIAN_PER_PARTICLE
  if (p.p.gamma_rot >= Thermostat::GammaType{}) {
    local_gamma = p.p.gamma_rot;
  } else
#endif
  {
    local_gamma = brownian.gamma_rotation;
  }

  for (int j = 0; j < 3; j++) {
#ifdef EXTERNAL_FORCES
    if (p.p.ext_flag & COORD_FIXED(j)) {
      p.m.omega[j] = 0.0;
    } else
#endif
    {
      // only conservative part of the force is used here
      // NOTE: velocity is assigned here and propagated by thermal part further
      // on top of it
#ifndef PARTICLE_ANISOTROPY
      p.m.omega[j] = p.f.torque[j] / (local_gamma);
#else
      p.m.omega[j] = p.f.torque[j] / (local_gamma[j]);
#endif // ROTATIONAL_INERTIA
    }
  }
  p.m.omega = mask(p.p.rotation, p.m.omega);
}

/** Propagate the quaternions: random walk part.
 *  An analogy of eq. (14.37) in @cite Schlick2010.
 *  @param[in,out] p              Particle
 *  @param[in]     dt             Time interval
 */
void bd_random_walk_rot(Particle &p, double dt) {
  // first, set defaults
  Thermostat::GammaType brown_sigma_pos_inv = brownian.sigma_pos_rotation_inv;

  // Override defaults if per-particle values for T and gamma are given
#ifdef BROWNIAN_PER_PARTICLE
  auto const constexpr brown_temp_coeff = 2.0;

  if (p.p.gamma_rot >= Thermostat::GammaType{}) {
    // Is a particle-specific temperature also specified?
    if (p.p.T >= 0.) {
      if (p.p.T > 0.0) {
        brown_sigma_pos_inv = sqrt(p.p.gamma_rot / (brown_temp_coeff * p.p.T));
      } else {
        // just an indication of the infinity
        brown_sigma_pos_inv = brownian.gammatype_nan;
      }
    } else if (temperature > 0.) {
      // Default temperature but particle-specific gamma
      brown_sigma_pos_inv =
          sqrt(p.p.gamma_rot / (brown_temp_coeff * temperature));
    } else {
      // just an indication of the infinity
      brown_sigma_pos_inv = brownian.gammatype_nan;
    }
  } // particle-specific gamma
  else {
    // No particle-specific gamma, but is there particle-specific temperature
    if (p.p.T >= 0.) {
      if (p.p.T > 0.0) {
        brown_sigma_pos_inv =
            sqrt(brownian.gamma_rotation / (brown_temp_coeff * p.p.T));
      } else {
        // just an indication of the infinity
        brown_sigma_pos_inv = brownian.gammatype_nan;
      }
    } else {
      // Defaut values for both
      brown_sigma_pos_inv = brownian.sigma_pos_rotation_inv;
    }
  }
#endif /* BROWNIAN_PER_PARTICLE */

  Utils::Vector3d dphi = {0.0, 0.0, 0.0};
  Utils::Vector3d noise = Random::v_noise_g<RNGSalt::BROWNIAN_ROT_INC>(
      brownian.rng_counter->value(), p.p.identity);
  for (int j = 0; j < 3; j++) {
#ifdef EXTERNAL_FORCES
    if (!(p.p.ext_flag & COORD_FIXED(j)))
#endif
    {
#ifndef PARTICLE_ANISOTROPY
      if (brown_sigma_pos_inv > 0.0) {
        dphi[j] = noise[j] * (1.0 / brown_sigma_pos_inv) * sqrt(dt);
      } else {
        dphi[j] = 0.0;
      }
#else
      if (brown_sigma_pos_inv[j] > 0.0) {
        dphi[j] = noise[j] * (1.0 / brown_sigma_pos_inv[j]) * sqrt(dt);
      } else {
        dphi[j] = 0.0;
      }
#endif // ROTATIONAL_INERTIA
    }
  }
  dphi = mask(p.p.rotation, dphi);
  // making the algorithm to be independ on an order of the rotations
  double dphi_m = dphi.norm();
  if (dphi_m) {
    Utils::Vector3d dphi_u;
    dphi_u = dphi / dphi_m;
    local_rotate_particle_body(p, dphi_u, dphi_m);
  }
}

/** Determine the angular velocities: random walk part.
 *  An analogy of eq. (10.2.16) in @cite Pottier2010.
 *  @param[in,out] p              Particle
 *  @param[in]     dt             Time interval
 */
void bd_random_walk_vel_rot(Particle &p, double dt) {
  double brown_sigma_vel;

  // Override defaults if per-particle values for T and gamma are given
#ifdef BROWNIAN_PER_PARTICLE
  auto const constexpr brown_temp_coeff = 1.0;
  // Is a particle-specific temperature specified?
  if (p.p.T >= 0.) {
    brown_sigma_vel = sqrt(brown_temp_coeff * p.p.T);
  } else {
    brown_sigma_vel = brownian.sigma_vel_rotation;
  }
#else
  // set defaults
  brown_sigma_vel = brownian.sigma_vel_rotation;
#endif /* BROWNIAN_PER_PARTICLE */

  Utils::Vector3d domega;
  Utils::Vector3d noise = Random::v_noise_g<RNGSalt::BROWNIAN_ROT_WALK>(
      brownian.rng_counter->value(), p.p.identity);
  for (int j = 0; j < 3; j++) {
#ifdef EXTERNAL_FORCES
    if (!(p.p.ext_flag & COORD_FIXED(j)))
#endif
    {
      // velocity is added here. It is already initialized in the terminal drag
      // part.
      domega[j] = brown_sigma_vel * noise[j] / sqrt(p.p.rinertia[j]);
    }
  }
  domega = mask(p.p.rotation, domega);
  p.m.omega += domega;
}
#endif // ROTATION

inline void brownian_dynamics_propagator(const ParticleRange &particles) {
  for (auto &p : particles) {
    // Don't propagate translational degrees of freedom of vs
#ifdef VIRTUAL_SITES
    extern bool thermo_virtual;
    if (!(p.p.is_virtual) or thermo_virtual)
#endif
    {
      bd_drag(p, time_step);
      bd_drag_vel(p, time_step);
      bd_random_walk(p, time_step);
      bd_random_walk_vel(p, time_step);
      /* Verlet criterion check */
      if ((p.r.p - p.l.p_old).norm2() > Utils::sqr(0.5 * skin))
        set_resort_particles(Cells::RESORT_LOCAL);
    }
#ifdef ROTATION
    if (!p.p.rotation)
      continue;
    convert_torque_to_body_frame_apply_fix(p);
    bd_drag_rot(p, time_step);
    bd_drag_vel_rot(p, time_step);
    bd_random_walk_rot(p, time_step);
    bd_random_walk_vel_rot(p, time_step);
#endif // ROTATION
  }
  sim_time += time_step;
}

#endif // BROWNIAN_INLINE_HPP
