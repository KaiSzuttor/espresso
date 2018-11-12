/*
  Copyright (C) 2014-2018 The ESPResSo project

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
#ifndef ESPRESSOSYSTEMINTERFACE_H
#define ESPRESSOSYSTEMINTERFACE_H

#include <stdio.h>

#include "SystemInterface.hpp"
#include "cuda_interface.hpp"
#include "debug.hpp"

/** Syntactic sugar */
#define espressoSystemInterface EspressoSystemInterface::Instance()

class EspressoSystemInterface : public SystemInterface {
public:
  static EspressoSystemInterface &Instance() {
    if (!m_instance)
      m_instance = new EspressoSystemInterface;

    return *m_instance;
  };

  static EspressoSystemInterface *_Instance() {
    if (!m_instance)
      m_instance = new EspressoSystemInterface;

    return m_instance;
  };

  void init() override;
  void update() override;

#ifdef CUDA
  double *rGpuBegin() override { return m_r_gpu_begin; };
  double *rGpuEnd() override { return m_r_gpu_end; };
  bool hasRGpu() override { return true; };
  bool requestRGpu() override {
    m_needsRGpu = hasRGpu();
    m_splitParticleStructGpu |= m_needsRGpu;
    m_gpu |= m_needsRGpu;
    if (m_gpu)
      enableParticleCommunication();
    return m_needsRGpu;
  };
#ifdef DIPOLES
  double *dipGpuBegin() override { return m_dip_gpu_begin; };
  double *dipGpuEnd() override { return m_dip_gpu_end; };
  bool hasDipGpu() override { return true; };
  bool requestDipGpu() override {
    m_needsDipGpu = hasDipGpu();
    m_splitParticleStructGpu |= m_needsRGpu;
    m_gpu |= m_needsRGpu;
    if (m_gpu)
      enableParticleCommunication();
    return m_needsDipGpu;
  };
#endif
  double *vGpuBegin() override { return m_v_gpu_begin; };
  double *vGpuEnd() override { return m_v_gpu_end; };
  bool hasVGpu() override { return true; };
  bool requestVGpu() override {
    m_needsVGpu = hasVGpu();
    m_splitParticleStructGpu |= m_needsVGpu;
    m_gpu |= m_needsVGpu;
    if (m_gpu)
      enableParticleCommunication();
    return m_needsVGpu;
  };

  double *qGpuBegin() override { return m_q_gpu_begin; };
  double *qGpuEnd() override { return m_q_gpu_end; };
  bool hasQGpu() override { return true; };
  bool requestQGpu() override {
    m_needsQGpu = hasQGpu();
    m_splitParticleStructGpu |= m_needsQGpu;
    m_gpu |= m_needsQGpu;
    if (m_gpu)
      enableParticleCommunication();
    return m_needsQGpu;
  };

  double *directorGpuBegin() override { return m_director_gpu_begin; };
  double *directorGpuEnd() override { return m_director_gpu_end; };
  bool hasDirectorGpu() override { return true; };
  bool requestDirectorGpu() override {
    m_needsDirectorGpu = hasDirectorGpu();
    m_splitParticleStructGpu |= m_needsDirectorGpu;
    m_gpu |= m_needsDirectorGpu;
    if (m_gpu)
      enableParticleCommunication();
    return m_needsDirectorGpu;
  };

  bool requestParticleStructGpu() {
    m_needsParticleStructGpu = true;
    m_gpu |= m_needsParticleStructGpu;
    if (m_gpu)
      enableParticleCommunication();
    return true;
  };

  double *fGpuBegin() override { return gpu_get_particle_force_pointer(); };
  double *fGpuEnd() override {
    return gpu_get_particle_force_pointer() + 3 * m_gpu_npart;
  };
  double *eGpu() override { return (double *)gpu_get_energy_pointer(); };
  double *torqueGpuBegin() override {
    return (double *)gpu_get_particle_torque_pointer();
  };
  double *torqueGpuEnd() override {
    return (double *)(gpu_get_particle_torque_pointer()) + 3 * m_gpu_npart;
  };
  bool hasFGpu() override { return true; };
  bool requestFGpu() override {
    m_needsFGpu = hasFGpu();
    m_gpu |= m_needsFGpu;
    if (m_gpu)
      enableParticleCommunication();
    return m_needsFGpu;
  };

#ifdef ROTATION
  bool hasTorqueGpu() override { return true; };
  bool requestTorqueGpu() override {
    m_needsTorqueGpu = hasTorqueGpu();
    m_gpu |= m_needsTorqueGpu;
    if (m_gpu)
      enableParticleCommunication();
    return m_needsTorqueGpu;
  };
#endif

#endif

  Vector3d box() const override;

  unsigned int npart_gpu() override {
#ifdef CUDA
    return m_gpu_npart;
#else
    return 0;
#endif
  };

protected:
  static EspressoSystemInterface *m_instance;
  EspressoSystemInterface()
      : m_gpu_npart(0), m_gpu(false), m_r_gpu_begin(0), m_r_gpu_end(0),
        m_dip_gpu_begin(0), m_dip_gpu_end(0), m_v_gpu_begin(0), m_v_gpu_end(0),
        m_q_gpu_begin(0), m_q_gpu_end(0), m_director_gpu_begin(0),
        m_director_gpu_end(0), m_needsParticleStructGpu(false),
        m_splitParticleStructGpu(false){};
  virtual ~EspressoSystemInterface() {}

  void gatherParticles();
  void split_particle_struct();
#ifdef CUDA
  void enableParticleCommunication() {
    if (!gpu_get_global_particle_vars_pointer_host()->communication_enabled) {
      ESIF_TRACE(puts("gpu communication not enabled;"));
      ESIF_TRACE(puts("enableParticleCommunication"));
      gpu_init_particle_comm();
      cuda_bcast_global_part_params();
      reallocDeviceMemory(
          gpu_get_global_particle_vars_pointer_host()->number_of_particles);
    }
  };
  void reallocDeviceMemory(int n);
#endif

  int m_gpu_npart;
  bool m_gpu;

  double *m_r_gpu_begin;
  double *m_r_gpu_end;

  double *m_dip_gpu_begin;
  double *m_dip_gpu_end;

  double *m_v_gpu_begin;
  double *m_v_gpu_end;

  double *m_q_gpu_begin;
  double *m_q_gpu_end;

  double *m_director_gpu_begin;
  double *m_director_gpu_end;

  bool m_needsParticleStructGpu;
  bool m_splitParticleStructGpu;
};

/********************************************************************************************/

#endif
