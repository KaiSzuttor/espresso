/*
  Copyright (C) 2010,2011,2012,2013,2014 The ESPResSo project
  Copyright (C) 2009,2010 
    Max-Planck-Institute for Polymer Research, Theory Group
  
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
#ifndef _POISEUILLE_FORCE_H
#define _POISEUILLE_FORCE_H

/** \file poiseuille_force.hpp
*/

#include "utils.hpp"
#include "interaction_data.hpp"
#include "particle_data.hpp"

#ifdef POISEUILLE_FORCE

int poiseuille_force_set_params(int part_type_a, int part_type_b,
    double diameter, double viscosity, double v_max,
    double cw);

void add_poiseuille_force_pair_force(Particle *p1, Particle *p2, IA_parameters *ia_params, double dist, double force[3]);

double poiseuille_profile_velocity(double v_max, double z, double cw);
#endif

#endif
