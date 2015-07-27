/*
  Copyright (C) 2010,2011,2012,2013,2014 The ESPResSo project
  Copyright (C) 2002,2003,2004,2005,2006,2007,2008,2009,2010
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
/** \file poiseuille_force_tcl.cpp
 *
 *  Implementation of \ref poiseuille_force_tcl.hpp
 */
#include "parser.hpp"

#ifdef POISEUILLE_FORCE
#include "poiseuille_force.hpp"
#include "interaction_data.hpp"

int tclprint_to_result_poiseuille_forceIA(Tcl_Interp *interp, int i, int j)
{
  char buffer[TCL_DOUBLE_SPACE];
  IA_parameters *data = get_ia_param(i, j);

  Tcl_AppendResult(interp, "poiseuille_force ", buffer, " ", (char *) NULL);
  Tcl_PrintDouble(interp, data->POISEUILLE_FORCE_diameter, buffer);
  Tcl_AppendResult(interp, buffer, " ", (char *) NULL);
  Tcl_PrintDouble(interp, data->POISEUILLE_FORCE_viscosity, buffer);
  Tcl_AppendResult(interp, buffer, " ", (char *) NULL);
  Tcl_PrintDouble(interp, data->POISEUILLE_FORCE_v_max, buffer);
  Tcl_AppendResult(interp, buffer, " ", (char *) NULL);
  Tcl_PrintDouble(interp, data->POISEUILLE_FORCE_cw, buffer);
  Tcl_AppendResult(interp, buffer, " ", (char *) NULL);

  return TCL_OK;
}

int tclcommand_inter_parse_poiseuille_force(Tcl_Interp * interp,
					int part_type_a, int part_type_b,
					int argc, char ** argv)
{
  double diameter, viscosity, v_max, cw;
  int change;

  if (argc < 5) {
    Tcl_AppendResult(interp, "poiseuille_force needs 4 parameters: "
		     "diameter, viscosity, v_max, cw",
		     (char *) NULL);
    return 0;
  }

  /* copy lj-cos parameters */
  if ((! ARG_IS_D(1, diameter))    ||
      (! ARG_IS_D(2, viscosity))   ||
      (! ARG_IS_D(3, v_max))   ||
      (! ARG_IS_D(4, cw))  ) {
    Tcl_AppendResult(interp, "poiseuille_force needs 4 DOUBLE parameters: "
		     "<diameter> <viscosity> <v_max> <cw>",
		     (char *) NULL);
    return 0;
  }
  change = 8;

  if (poiseuille_force_set_params(part_type_a, part_type_b, diameter, viscosity, v_max, cw) == ES_ERROR) {
    Tcl_AppendResult(interp, "particle types must be non-negative", (char *) NULL);
    return 0;
  }

  return change;
}

#endif
