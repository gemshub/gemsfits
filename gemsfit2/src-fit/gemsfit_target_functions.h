// Copyright (C) 2013 G.D.Miron, D.Kulik
// <GEMS Development Team, mailto:gems2.support@psi.ch>
//
// This file is part of the GEMSFIT code for parameterization of thermodynamic
// data and models <http://gems.web.psi.ch/GEMSFIT/>
//
// GEMSIFT is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.

// GEMSFIT is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with GEMSFIT code. If not, see <http://www.gnu.org/licenses/>.
//-------------------------------------------------------------------
//

/**
 *	@file gemsfit_target_functions.h
 *
 *	@brief this header file contains definitions of target functions needed during optimization.
 *  Also other functions needed for setting the new adjuste paramaters are defined here
 *
 *	@author G. Dan Miron
 *
 * 	@date 17.05.2013
 *
 */



#ifndef GEMSFIT_TARGET_FUNCTIONS_H
#define GEMSFIT_TARGET_FUNCTIONS_H

#include <vector>
#include <iostream>
#include "gemsfit_task.h"
#include "print.h"


// Set new Gibbs free energy
/**
* Sets the new G0 returned form the optimization algorithm
* @param i      position of the G0 in the parameter/optimization vector
* @param G0     the new G0 returned by the optimization algorithm
* @param sys    pointer to data object that holds the data of the current TGfitTask struct
* @author DM
* @date 17.05.2013
*/
void adjust_G0 (int i, double G0, TGfitTask *sys);

void adjust_PMc (int i, double new_PMc, TGfitTask *sys);

// Set new Gibbs free energy based on reaction constraints
/**
* Sets the new G0 of the components fixed using reaction constraints
* @param i      position of the G0 in the parameter/optimization vector
* @param G0     the new G0 returned by the optimization algorithm
* @param sys    pointer to data object that holds the data of the current TGfitTask struct
* @author DM
* @date 17.05.2013
*/
void adjust_RDc (TGfitTask *sys);

/**
* Checks if the unit specified in the target function in the input file corresponds with the unit in the
* Data Manager class
* @param i      position in the experiments vector (which experiment)
* @param p      position in the expphases vector (which pahse)
* @param e      position in the phcomp vector (which element)
* @param unit   unit of form the target function specified in the input file
* @param sys    pointer to data object that holds the data of the current TGfitTask struct
* @author DM
* @date 17.05.2013
*/
void check_unit(int i, int p, int e, string unit, TGfitTask *sys );

/**
* Checks if the unit specified in the target function in the input file corresponds with the unit in the
* Data Manager class
* @param i      position in the experiments vector (which experiment)
* @param p      position in the expphases vector (which pahse)
* @param pp     position in the phprop vector (which property)
* @param unit   unit of form the target function specified in the input file
* @param sys    pointer to data object that holds the data of the current TGfitTask struct
* @author DM
* @date 17.05.2013
*/
void check_ph_unit(int i, int p, int pp, string unit, TGfitTask *sys );

/**
* Returns the residual from comparing the calculated with the meaured
* the elemental composition in the aqueous phase
* @param i      position in the experiments vector (which experiment)
* @param p      position in the expphases vector (which pahse)
* @param e      position in the phcomp vector (which element)
* @param sys    pointer to data object that holds the data of the current TGfitTask struct
* @author DM
* @date 17.05.2013
*/
double residual_aqgen_elem (int i, int p, int e, TGfitTask *sys);

/**
* Returns the residual from comparing the calculated with the meaured
* the elemental composition in the aqueous phase
* @param i      position in the experiments vector (which experiment)
* @param p      position in the expphases vector (which pahse)
* @param pp     position in the phprop vector (which property)
* @param j      position in the objfun vector
* @param sys    pointer to data object that holds the data of the current TGfitTask struct
* @author DM
* @date 17.05.2013
*/
double residual_aqgen_prop (int i, int p, int pp, int j, TGfitTask *sys);

/**
* Returns the residual from comparing the calculated with the meaured
* the elemental composition a phase
* @param i      position in the experiments vector (which experiment)
* @param p      position in the expphases vector (which pahse)
* @param e      position in the phcomp vector (which element)
* @param sys    pointer to data object that holds the data of the current TGfitTask struct
* @author DM
* @date 17.05.2013
*/
double residual_phase_elem (int i, int p, int e, TGfitTask *sys);

/**
* Returns the residual from comparing the calculated with the meaured
* the elemental composition a phase
* @param i      position in the experiments vector (which experiment)
* @param p      position in the expphases vector (which pahse)
* @param pp     position in the phprop vector (which property)
* @param j      position in the objfun vector
* @param sys    pointer to data object that holds the data of the current TGfitTask struct
* @author DM
* @date 17.05.2013
*/
double residual_phase_prop (int i, int p, int pp, int j, TGfitTask *sys);

/**
* Returns the weight based on the wight type
* @param i      position in the experiments vector (which experiment)
* @param p      position in the expphases vector (which pahse)
* @param e      position in the phcomp vector (which element)
* @param type   type of weighting
* @param sys    pointer to data object that holds the data of the current TGfitTask struct
* @author DM
* @date 17.05.2013
*/
double weight (int i, int p, int e, string type, TGfitTask *sys);

/**
* Returns the weight based on the wight type
* @param i      position in the experiments vector (which experiment)
* @param p      position in the expphases vector (which pahse)
* @param pp     position in the phprop vector (which property)
* @param type   type of weighting
* @param sys    pointer to data object that holds the data of the current TGfitTask struct
* @author DM
* @date 17.05.2013
*/
double weight_phprop (int i, int p, int pp, string type, TGfitTask *sys);


/**
* Returns the least square ( sqrt[(calculated-measured)^2 /measured^2] )
* @param computed_value
* @param measured_value
* @author DM
* @date 17.05.2013
*/
double least_square (double computed_value, double measured_value);




#endif // GEMSFIT_TARGET_FUNCTIONS_H
