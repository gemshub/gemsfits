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
#include <muParser.h>


/**
* Checks if the unit specified in the target function in the input file corresponds with the unit in the
* Data Manager class
* @param i      position in the experiments vector (which experiment)
* @param p      position in the expphases vector (which phase)
* @param e      position in the phcomp vector (which element)
* @param unit   unit of form the target function specified in the input file
* @param sys    pointer to data object that holds the data of the current TGfitTask struct
* @author DM
* @date 17.05.2013
*/
void check_unit(int i, int p, int e, string unit, TGfitTask *sys );
void check_unit_dcomp(int i, int p, int dc, int dcp, string unit, TGfitTask *sys );
void check_MR_unit(int i, int p, int f, string unit, TGfitTask *sys );

/**
* Checks if the unit specified in the target function in the input file corresponds with the unit in the
* Data Manager class
* @param i      position in the experiments vector (which experiment)
* @param p      position in the expphases vector (which phase)
* @param pp     position in the phprop vector (which property)
* @param unit   unit of form the target function specified in the input file
* @param sys    pointer to data object that holds the data of the current TGfitTask struct
* @author DM
* @date 17.05.2013
*/
void check_prop_unit(int i, int p, int pp, string unit, TGfitTask *sys );


/**
* Returns the residual from comparing the calculated with the measured
* elemental composition a phase
* @param i      position in the experiments vector (which experiment)
* @param p      position in the expphases vector (which phase)
* @param e      position in the phcomp vector (which element)
* @param j      position in the objfun vector
* @param sys    pointer to data object that holds the data of the current TGfitTask struct
* @author DM
* @date 17.05.2013
*/
double residual_phase_elem (int i, int p, int e, TGfitTask::TargetFunction::obj_fun &objfun, TGfitTask *sys);


/**
* Returns the residual from comparing the calculated with the measured
* element mole ratio in a phase
* @param i      position in the experiments vector (which experiment)
* @param p      position in the expphases vector (which phase)
* @param f      position in the phMR vector (which mole ratio)
* @param j      position in the objfun vector
* @param sys    pointer to data object that holds the data of the current TGfitTask struct
* @author DM
* @date 17.08.2013
*/
double residual_phase_elemMR (int i, int p, int f, TGfitTask::TargetFunction::obj_fun &objfun, TGfitTask *sys);


/**
* Returns the residual from comparing the calculated with the measured
* bulk property of a phase
* @param i      position in the experiments vector (which experiment)
* @param p      position in the expphases vector (which phase)
* @param pp     position in the phprop vector (which property)
* @param j      position in the objfun vector
* @param sys    pointer to data object that holds the data of the current TGfitTask struct
* @author DM
* @date 17.05.2013
*/
double residual_phase_prop (int i, int p, int pp, TGfitTask::TargetFunction::obj_fun &objfun, TGfitTask *sys);


/**
* Returns the residual from comparing the calculated with the measured
* speciation in a phase
* @param i      position in the experiments vector (which experiment)
* @param p      position in the expphases vector (which phase)
* @param dc     position in the phDC vector (which dcomp)
* @param dcp    position in the DCprop vector (which dcomp property)
* @param j      position in the objfun vector
* @param sys    pointer to data object that holds the data of the current TGfitTask struct
* @author DM
* @date 13.06.2013
*/
double residual_phase_dcomp (int i, int p, int dc, int dcp, TGfitTask::TargetFunction::obj_fun &objfun, TGfitTask *sys);



/**
* Returns the weight based on the weight type
* @param i      position in the experiments vector (which experiment)
* @param p      position in the expphases vector (which phase)
* @param e      position in the phIC vector (which element)
* @param j      position in the objective function vector
* @param type   type of weighting
* @param sys    pointer to data object that holds the data of the current TGfitTask struct
* @author DM
* @date 17.05.2013
*/
double weight (int i, int p, int e, TGfitTask::TargetFunction::obj_fun &objfun, string type, TGfitTask *sys);

/**
* Returns the weight based on the weight type
* @param i      position in the experiments vector (which experiment)
* @param p      position in the expphases vector (which phase)
* @param f      position in the phMR vector (which molar faction formula)
* @param j      position in the objective function vector
* @param type   type of weighting
* @param sys    pointer to data object that holds the data of the current TGfitTask struct
* @author DM
* @date 17.08.2013
*/
double weight_MR (int i, int p, int f, TGfitTask::TargetFunction::obj_fun &objfun, string type, TGfitTask *sys);

/**
* Returns the weight based on the weight type
* @param i      position in the experiments vector (which experiment)
* @param p      position in the expphases vector (which phase)
* @param pp     position in the phprop vector (which property)
* @param j      position in the objective function vector
* @param type   type of weighting
* @param sys    pointer to data object that holds the data of the current TGfitTask struct
* @author DM
* @date 17.05.2013
*/
double weight_phprop (int i, int p, int pp, TGfitTask::TargetFunction::obj_fun &objfun, string type, TGfitTask *sys);

/**
* Returns the weight based on the weight type
* @param i      position in the experiments vector (which experiment)
* @param p      position in the expphases vector (which phase)
* @param dc     position in the phDC vector (which which dcomp)
* @param dcp    position in the DCprop vector (which which dcomp property)
* @param j      position in the objective function vector
* @param type   type of weighting
* @param sys    pointer to data object that holds the data of the current TGfitTask struct
* @author DM
* @date 13.06.2013
*/
double weight_phdcomp (int i, int p, int dc, int dcp, TGfitTask::TargetFunction::obj_fun &objfun, string type, TGfitTask *sys);

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

#if defined(_UNICODE)
double* AddVariable(const wchar_t *a_szName, void *pUserData);
#else
double* AddVariable(const char *a_szName, void *pUserData);
#endif

string formula_DCname_parser (string expr, vector<string> &exprO, vector<string> &exprP );

#endif // GEMSFIT_TARGET_FUNCTIONS_H
