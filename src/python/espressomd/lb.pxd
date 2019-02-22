#
# Copyright (C) 2013-2018 The ESPResSo project
#
# This file is part of ESPResSo.
#
# ESPResSo is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# ESPResSo is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
from __future__ import print_function, absolute_import

include "myconfig.pxi"

from libcpp cimport bool
from libcpp.vector cimport vector
from libcpp.string cimport string
from libc cimport stdint

from .actors cimport Actor
from .utils cimport Vector3d
from .utils cimport Vector3i
from .utils cimport Vector6d
from .utils cimport Vector19d

cdef class HydrodynamicInteraction(Actor):
    pass

IF LB_GPU or LB:

    ##############################################
    #
    # extern functions and structs
    #
    ##############################################

    cdef extern from "grid_based_algorithms/lb_interface.hpp":

        void lb_lbfluid_set_tau(double c_tau) except +
        double lb_lbfluid_get_tau() except +
        void lb_lbfluid_set_density(double c_dens) except +
        double lb_lbfluid_get_density() except +
        void lb_lbfluid_set_viscosity(double c_visc) except +
        double lb_lbfluid_get_viscosity() except +
        void lb_lbfluid_set_agrid(double c_agrid) except +
        double lb_lbfluid_get_agrid() except +
        void lb_lbfluid_set_gamma_odd(double c_gamma_odd) except +
        double lb_lbfluid_get_gamma_odd() except +
        void lb_lbfluid_set_gamma_even(double c_gamma_even) except +
        double lb_lbfluid_get_gamma_even() except +
        void lb_lbfluid_set_ext_force_density(int component, const Vector3d forcedensity) except +
        const Vector3d lb_lbfluid_get_ext_force_density() except +
        void lb_lbfluid_set_bulk_viscosity(double c_bulk_visc) except +
        double lb_lbfluid_get_bulk_viscosity() except +
        void lb_lbfluid_print_vtk_velocity(string filename) except +
        void lb_lbfluid_print_vtk_velocity(string filename, vector[int] bb1, vector[int] bb2) except +
        void lb_lbfluid_print_vtk_boundary(string filename) except +
        void lb_lbfluid_print_velocity(string filename) except +
        void lb_lbfluid_print_boundary(string filename) except +
        void lb_lbfluid_save_checkpoint(string filename, int binary) except +
        void lb_lbfluid_load_checkpoint(string filename, int binary) except +
        void lb_lbfluid_set_lattice_switch(int local_lattice_switch) except +
        int lb_lbfluid_get_lattice_switch() except +
        bool lb_lbnode_is_index_valid(const Vector3i & ind) except +
        const Vector3d lb_lbnode_get_u(const Vector3i & ind, bool global_flag) except +
        void lb_lbnode_set_u(const Vector3i & ind, const Vector3d & u) except +
        double lb_lbnode_get_density(const Vector3i & ind) except +
        const Vector6d lb_lbnode_get_pi(const Vector3i & ind) except +
        const Vector6d lb_lbnode_get_pi_neq(const Vector3i & ind) except +
        const Vector19d lb_lbnode_get_pop(const Vector3i & ind) except +
        void lb_lbnode_set_pop(const Vector3i & ind, const Vector19d & populations) except +
        int lb_lbnode_get_boundary(const Vector3i & ind) except +
        stdint.uint64_t lb_lbfluid_get_rng_state() except +
        void lb_lbfluid_set_rng_state(stdint.uint64_t) except +
        void lb_lbfluid_set_kT(double) except +
        double lb_lbfluid_get_kT() except +

    cdef extern from "grid_based_algorithms/lb_particle_coupling.hpp":
        void lb_lbcoupling_set_rng_state(stdint.uint64_t)
        stdint.uint64_t lb_lbcoupling_get_rng_state() except +
        void lb_lbcoupling_set_friction(double)
        double lb_lbcoupling_get_friction() except +

    cdef extern from "grid_based_algorithms/lb_interpolation.hpp":
        const Vector3d lb_lbinterpolation_get_interpolated_velocity(const Vector3d& pos, bool global_flag)

    cdef extern from "grid_based_algorithms/lbgpu.hpp":
        int lb_lbfluid_remove_total_momentum()
        void lb_get_interpolated_velocity_gpu(double * positions, double * velocities, int length)

    ###############################################
    #
    # Wrapper-functions for access to C-pointer: Set params
    #
    ###############################################
    cdef inline python_lbfluid_set_density(p_dens, agrid):
        cdef double c_dens

        # get pointers
        if isinstance(p_dens, float) or isinstance(p_dens, int):
            c_dens = <float > p_dens * agrid * agrid * agrid
        else:
            c_dens = p_dens * agrid * agrid * agrid
        # call c-function
        lb_lbfluid_set_density(c_dens)

###############################################

    cdef inline python_lbfluid_set_viscosity(p_visc, p_agrid, p_tau):
        cdef double c_visc
        # get pointers
        if isinstance(p_visc, float) or isinstance(p_visc, int):
            c_visc = <float > p_visc * p_tau / (p_agrid * p_agrid)
        else:
            c_visc = p_visc * p_tau / (p_agrid * p_agrid)
        # call c-function
        lb_lbfluid_set_viscosity(c_visc)

###############################################

    cdef inline python_lbfluid_set_agrid(p_agrid):
        cdef double c_agrid
        # get pointers
        c_agrid = p_agrid
        # call c-function
        lb_lbfluid_set_agrid(c_agrid)

###############################################

    cdef inline python_lbfluid_set_bulk_viscosity(p_bvisc, p_agrid, p_tau):
        cdef double c_bvisc
        # get pointers
        if isinstance(p_bvisc, float) or isinstance(p_bvisc, int):
            c_bvisc = <float > p_bvisc * p_tau / (p_agrid * p_agrid)
        else:
            c_bvisc = p_bvisc * p_tau / (p_agrid * p_agrid)
        # call c-function
        lb_lbfluid_set_bulk_viscosity(c_bvisc)

###############################################

    cdef inline python_lbfluid_set_friction(p_friction):
        cdef double c_friction
        # get pointers
        if isinstance(p_friction, float) or isinstance(p_friction, int):
            c_friction = <float > p_friction
        else:
            c_friction = p_friction
        # call c-function
        lb_lbcoupling_set_friction(c_friction)

    cdef inline python_lbfluid_set_gamma_odd(gamma_odd):
        cdef double c_gamma_odd
        # get pointers
        if isinstance(gamma_odd, float) or isinstance(gamma_odd, int):
            c_gamma_odd = <float > gamma_odd
        else:
            c_gamma_odd = gamma_odd
        # call c-function
        lb_lbfluid_set_gamma_odd(c_gamma_odd)

    cdef inline python_lbfluid_set_gamma_even(gamma_even):
        cdef double c_gamma_even
        # get pointers
        if isinstance(gamma_even, float) or isinstance(gamma_even, int):
            c_gamma_even = <float > gamma_even
        else:
            c_gamma_even = gamma_even
        # call c-function
        lb_lbfluid_set_gamma_even(c_gamma_even)

###############################################

    cdef inline python_lbfluid_set_ext_force_density(p_ext_force_density, p_agrid, p_tau):

        cdef Vector3d c_ext_force_density
        # get pointers
        # unit conversion MD -> LB
        c_ext_force_density[
            0] = p_ext_force_density[
                0] * p_agrid * p_agrid * p_tau * p_tau
        c_ext_force_density[
            1] = p_ext_force_density[
                1] * p_agrid * p_agrid * p_tau * p_tau
        c_ext_force_density[
            2] = p_ext_force_density[
                2] * p_agrid * p_agrid * p_tau * p_tau
        # call c-function
        lb_lbfluid_set_ext_force_density(0, c_ext_force_density)


###############################################
#
# Wrapper-functions for access to C-pointer: Get params
#
###############################################
    cdef inline python_lbfluid_get_density(p_dens, agrid):
        cdef double c_dens
        # call c-function
        c_dens = lb_lbfluid_get_density()
        if isinstance(p_dens, float) or isinstance(p_dens, int):
            p_dens = <double > c_dens / agrid / agrid / agrid
        else:
            p_dens = c_dens / agrid / agrid / agrid

###############################################
    cdef inline python_lbfluid_get_viscosity(p_visc, p_agrid, p_tau):
        cdef double c_visc
        # call c-function
        c_visc = lb_lbfluid_get_viscosity()
        if isinstance(p_visc, float) or isinstance(p_visc, int):
            p_visc = <double > c_visc / p_tau * (p_agrid * p_agrid)
        else:
            p_visc = c_visc / p_tau * (p_agrid * p_agrid)

###############################################
    cdef inline python_lbfluid_get_agrid(p_agrid):
        cdef double c_agrid
        # call c-function
        c_agrid = lb_lbfluid_get_agrid()
        p_agrid = <double > c_agrid

###############################################
    cdef inline python_lbfluid_get_bulk_viscosity(p_bvisc, p_agrid, p_tau):
        cdef double c_bvisc
        # call c-function
        c_bvisc = lb_lbfluid_get_bulk_viscosity()
        if isinstance(p_bvisc, float) or isinstance(p_bvisc, int):
            p_bvisc = <double > c_bvisc / p_tau * (p_agrid * p_agrid)
        else:
            p_bvisc = c_bvisc / p_tau * (p_agrid * p_agrid)

###############################################
    cdef inline python_lbfluid_get_friction(p_friction):
        cdef double c_friction
        # call c-function
        c_friction = lb_lbcoupling_get_friction()
        if isinstance(p_friction, float) or isinstance(p_friction, int):
            p_fricition = <double > c_friction
        else:
            p_friction = c_friction

###############################################

    cdef inline python_lbfluid_get_ext_force_density(p_ext_force_density, p_agrid, p_tau):

        cdef Vector3d c_ext_force_density
        # call c-function
        c_ext_force_density = lb_lbfluid_get_ext_force_density()
        # unit conversion LB -> MD
        p_ext_force_density[
            0] = c_ext_force_density[
                0] / p_agrid / p_agrid / p_tau / p_tau
        p_ext_force_density[
            1] = c_ext_force_density[
                1] / p_agrid / p_agrid / p_tau / p_tau
        p_ext_force_density[
            2] = c_ext_force_density[
                2] / p_agrid / p_agrid / p_tau / p_tau
