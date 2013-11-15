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
// along with GEMSFIT code. If not, see <http://www.gnu.org/licenses/>.
//-------------------------------------------------------------------
//

/**
 *	@file print.h
 *
 *	@brief this header file contains definitions of the printing class.
 *
 *	@author G. Dan Miron
 *
 * 	@date 17.05.2013
 *
 */



#ifndef PRINT_H
#define PRINT_H

#include <vector>
#include <string>
#include <iostream>
#include <optimization.h>
using namespace std;

class ResPrint
{
private:
    typedef vector<double>  double_v;
    typedef vector<string>  string_v;
    string_v experiment, what1, what2, unit, fucntion;
    double_v measured, computed, Weighted_Tfun_residual, residuals_v;
    string path;

    optimization* res_opti; /// points to the optimization results

public:
    //constructor
    ResPrint(string path_, optimization *opti);
    //destructor
    ~ResPrint ();

    vector<vector<double> > sensitivity;

    /**
    * Prints the header of the results CSV file
    * @param function      name of the target function
    * @param weight_       type of weighting
    * @author DM
    * @date 17.05.2013
    */
    void print_header(string function, string weight_, int size);

    /**
    * Clears the printing class variables (sotore the optimization results from the last itteration)
    * @author DM
    * @date 17.05.2013
    */
    void print_clear();

    /**
    * Sets the printing class variables with values form the optimization and target function calculation
    * @param experiment_    name of the experiment
    * @param what1_         name or property of data compared e.g. name of phase
    * @param what2_         name or property of data compared e.g. name of element/phase property
    * @param unit_          unit of the compared data
    * @param measured_      measured value
    * @param computed_      calculated value
    * @param Weighted_Tfun_residual_    value of the weighted target function residual
    * @param weight_        value of the weight
    * @author DM
    * @date 17.05.2013
    */
    void set_print(string experiment_, string what1_, string what2_, string unit_, double measured_, double computed_, double Weighted_Tfun_residual_, double weight_ );

    /**
    * Prints the results to result CSV (comma separated) file
    * @author DM
    * @date 17.05.2013
    */
    void print_result();
//    void print_result(string experiment, string what2, string unit, double measured, double computed, double residual);
};

#endif // PRINT_H
