// Copyright (C) 2013 G.D.Miron, D.Kulik
// <GEMS Development Team, mailto:gems2.support@psi.ch>
//
// This file is part of the GEMSFIT2 code for parameterization of thermodynamic
// data and models <http://gems.web.psi.ch/GEMSFIT/>
//
// GEMSIFT2 is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.

// GEMSFIT2 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with GEMSFIT2 code. If not, see <http://www.gnu.org/licenses/>.
//-------------------------------------------------------------------
//

/**
 *	@file gemsfit_nested_functions.h
 *
 *	@brief this header file contains definitions of dynamic functions needed during specific optimization.
 *	tasks. Wehn the user wants to dyamically adjust other system parameters during fitting. For example
 *  adjusting the NaOH or HCl amount so that the pH is the same as measured since there is no clear info
 *  of their amount from the experimental paper.
 *
 *	@author G. Dan Miron
 *
 * 	@date 01.12.2013
 *
 */


#ifndef GEMSFIT_NESTED_FUNCTIONS_H
#define GEMSFIT_NESTED_FUNCTIONS_H

#include <vector>
#include <iostream>
#include "gemsfit_task.h"

void nestedfun (TGfitTask *sys);

double nestminfunc(const std::vector<double> &opt, std::vector<double> &grad, void *obj_func_data );

bool isTitration (TGfitTask *sys, int i, int j, int p);

#endif // GEMSFIT_DYNAMIC_FUNCTIONS_H
