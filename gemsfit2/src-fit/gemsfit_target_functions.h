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

/**
* Sets the new PMc returned form the optimization algorithm
* @param i          position of the PMc in the parameter/optimization vector
* @param new_PMc    the new PMc returned by the optimization algorithm
* @param sys        pointer to data object that holds the data of the current TGfitTask struct
* @author DM
* @date 17.05.2013
*/
void adjust_PMc (int i, double new_PMc, TGfitTask *sys);

/**
* Sets the new DMc returned form the optimization algorithm
* @param i          position of the DMc in the parameter/optimization vector
* @param new_DMc    the new DMc returned by the optimization algorithm
* @param sys        pointer to data object that holds the data of the current TGfitTask struct
* @author DM
* @date 04.07.2013
*/
void adjust_DMc (int i, double new_DMc, TGfitTask *sys);

/**
* Sets the new bIC returned form the optimization algorithm
* @param i          position of the bIC in the parameter/optimization vector
* @param new_bIC    the new DMc returned by the optimization algorithm
* @param sys        pointer to data object that holds the data of the current TGfitTask struct
* @author DM
* @date 04.07.2013
*/
void adjust_bIC (int i, double new_bIC, TGfitTask *sys);

/**
* Sets the new TK returned form the optimization algorithm
* @param i          position of the TK in the parameter/optimization vector
* @param new_TK     the new TK returned by the optimization algorithm
* @param sys        pointer to data object that holds the data of the current TGfitTask struct
* @author DM
* @date 04.07.2013
*/
void adjust_TK (int i, double new_TK, TGfitTask *sys);

/**
* Sets the new P returned form the optimization algorithm
* @param i          position of the P in the parameter/optimization vector
* @param new_P      the new P returned by the optimization algorithm
* @param sys        pointer to data object that holds the data of the current TGfitTask struct
* @author DM
* @date 04.07.2013
*/
void adjust_P (int i, double new_P, TGfitTask *sys);

// Set new Gibbs free energy based on reaction constraints
/**
* Sets the new G0 of the components fixed using reaction constraints
* @param sys    pointer to data object that holds the data of the current TGfitTask struct
* @author DM
* @date 17.05.2013
*/
void adjust_RDc (TGfitTask *sys);

/**
* Sets the new value of the linked element in bIC (bulk composition) vector
* @param sys    pointer to data object that holds the data of the current TGfitTask struct
* @author DM
* @date 13.06.2013
*/
void adjust_Lp (TGfitTask *sys);

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
void check_prop_unit(int i, int p, int pp, string unit, TGfitTask *sys );


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
double residual_phase_elem (int i, int p, int e, int j, TGfitTask *sys);

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
* Returns the residual from comparing the calculated with the meaured
* the elemental composition a phase
* @param i      position in the experiments vector (which experiment)
* @param p      position in the expphases vector (which pahse)
* @param dc     position in the phdcomps vector (which which dcomp)
* @param dcp    position in the dcompprop vector (which which dcomp property)
* @param j      position in the objfun vector
* @param sys    pointer to data object that holds the data of the current TGfitTask struct
* @author DM
* @date 13.06.2013
*/
double residual_phase_dcomp (int i, int p, int dc, int dcp, int j, TGfitTask *sys);

/**
* Returns the weight based on the weight type
* @param i      position in the experiments vector (which experiment)
* @param p      position in the expphases vector (which pahse)
* @param e      position in the phcomp vector (which element)
* @param j      position in the objective function vector
* @param type   type of weighting
* @param sys    pointer to data object that holds the data of the current TGfitTask struct
* @author DM
* @date 17.05.2013
*/
double weight (int i, int p, int e, int j, string type, TGfitTask *sys);

/**
* Returns the weight based on the weight type
* @param i      position in the experiments vector (which experiment)
* @param p      position in the expphases vector (which pahse)
* @param pp     position in the phprop vector (which property)
* @param j      position in the objective function vector
* @param type   type of weighting
* @param sys    pointer to data object that holds the data of the current TGfitTask struct
* @author DM
* @date 17.05.2013
*/
double weight_phprop (int i, int p, int pp, int j, string type, TGfitTask *sys);

/**
* Returns the weight based on the weight type
* @param i      position in the experiments vector (which experiment)
* @param p      position in the expphases vector (which pahse)
* @param dc     position in the phdcomps vector (which which dcomp)
* @param dcp    position in the dcompprop vector (which which dcomp property)
* @param j      position in the objective function vector
* @param type   type of weighting
* @param sys    pointer to data object that holds the data of the current TGfitTask struct
* @author DM
* @date 13.06.2013
*/
double weight_phdcomp (int i, int p, int dc, int dcp, int j, string type, TGfitTask *sys);

/**
* Returns the target function value (computed - measured)
* @param computed_value
* @param measured_value
* @param type
* @param objfun current objective function
* @author DM
* @date 17.05.2013
*/
double Tfunction (double computed_value, double measured_value, string type, TGfitTask::TargetFunction::obj_fun objfun);




#endif // GEMSFIT_TARGET_FUNCTIONS_H
