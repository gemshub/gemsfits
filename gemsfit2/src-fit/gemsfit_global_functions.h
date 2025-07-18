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
 *	@file gemsfit_global_functions.h
 *
 *	@brief this header file contains definitions of global functions needed during optimization.
 *	Since the nlopt library needs a function pointer to the objective and constraint
 *  functions, these functions need to be globally accessible and can not be implemented
 *  as member functions. This header lists mainly these implementations of the
 *  respective callback functions.
 *
 *	@author G. Dan Miron
 *
 * 	@date 08.05.2013
 *
 */


#ifndef GEMSFIT_GLOBAL_FUNCTIONS_H
#define GEMSFIT_GLOBAL_FUNCTIONS_H


#include <vector>
#include <iostream>
#include "gemsfit_task.h"
#include <GEMS3K/node.h>

// callback function for GEMS3K wrapper
/**
* Callback function for the constraint functions referring to the GEMS3 solver. The arguments of this function must not be
* modified since the nlopt library expects exactly this format.
* @param opt             optimization vector
* @param grad            vector holding function gradients (can be NULL)
* @param obj_func_data   a general data object will holds all the data needed by the onbejctive function. The data will be cat to the necessary identity within the function.
* @author DM
* @date 16.11.2012
*/
double Equil_objective_function_callback( const std::vector<double> &opt, std::vector<double> &grad, void *obj_func_data );

// GEMS3K wrapper
/**
* Call to the GEMS3K wrapper. The arguments of this function must not be
* modified since the nlopt library expects exactly this format.
* @param opt      optimization vector
* @param residual sum of squared residuals being returned from the function
* @param sys      data object that holds the data of the current TGfitTask struct
* @author DM
* @date 19.11.2012
*/
void gems3k_wrap( double &residual, const std::vector<double> &opt, TGfitTask *sys );

void tsolmod_wrap( double &residual, const std::vector<double> &opt, TGfitTask *sys );

/**
* Calculates the gradient as d_sum_of_Tfun_residuals / d_parameter
* @param opt      optimization vector
* @param grad     vector holding the gradient
* @param sys      data object that holds the data of the current TGfitTask struct
* @author DM
* @date 10.06.2013
*/
void gradient(std::vector<double> optn, std::vector<double> &grad, TGfitTask *sys );

/**
* Gets the median form a vector of values
* @param absresiduals      vector of residuals
* @author DM
* @date 18.12.2014
*/
double median( std::vector<double> absresiduals);

/**
* Calculates the Tuckey weight for each resiual and adds it in the Tucke_weights vector
* @param sys      data object that holds the data of the current TGfitTask struct
* @author DM
* @date 18.12.2014
*/
void set_Tuckey_weight_global (TGfitTask *sys);
void set_Tuckey_weight_global_norm (TGfitTask *sys);
void set_Tuckey_weight_objfun (TGfitTask *sys);
void set_Tuckey_weight_objfun_norm (TGfitTask *sys);
void set_Tuckey_weight_objfun_norm2 (TGfitTask *sys);
//ADD! void Tuckey_exp_objfun (TGfitTask *sys);



#endif // GEMSFIT_GLOBAL_FUNCTIONS_H
