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
#include "poiseuille_force.hpp"
#include "random.hpp"
#include "communication.hpp"
#include "utils.hpp" // for the PI variable
#include "integrate.hpp" //for the time_step variable
#include <math.h>

#ifdef POISEUILLE_FORCE

int poiseuille_force_set_params(int part_type_a, int part_type_b,
			    double gamma, double v_max,
			    double cw)
{
  IA_parameters *data = get_ia_param_safe(part_type_a, part_type_b);
  
  if (!data) return ES_ERROR;

  data->POISEUILLE_FORCE_gamma = gamma;
  data->POISEUILLE_FORCE_v_max = v_max;
  data->POISEUILLE_FORCE_cw = cw;

  /* broadcast interaction parameters */
  mpi_bcast_ia_params(part_type_a, part_type_b);
  
  return ES_OK;
}


double poiseuille_profile_velocity(double v_max, double z, double cw)
{
	return v_max*(1.0-4.0*z*z/(cw*cw));
}


void add_poiseuille_force_pair_force(Particle *p1, Particle *p2, IA_parameters *ia_params, double dist, double force[3])
{
  if (dist < 0.5*ia_params->POISEUILLE_FORCE_cw) {
    force[0] += ia_params->POISEUILLE_FORCE_gamma*\
                poiseuille_profile_velocity(ia_params->POISEUILLE_FORCE_v_max, dist, ia_params->POISEUILLE_FORCE_cw);
  }
}




#endif
