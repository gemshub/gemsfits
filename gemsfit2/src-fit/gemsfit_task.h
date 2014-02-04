//-------------------------------------------------------------------
// $Id: gemsft_task.h 789 2012-12-19 17:08:55Z kulik $
/// \file gemsft_task.h
/// Contains declaration of TGFitTask class implementing an advanced
/// interface for GEMSFIT.
//
/// \class TGfitTask gemsft_task.h
/// Implements an advanced C/C++ interface with GEMS3K for the
/// development of GEMSFIT. Uses TNodeArray class as analogy.
/// Works with DATACH and an array of DATABR structures; uses TNode class
//
// Copyright (C) 2013 G.D.Miron, D.Kulik
// <GEMS Development Team, mailto:gems2.support@psi.ch>
//
// This file is part of the GEMSFIT code for parameterization of thermodynamic
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

#ifndef _GEMSFT_TASK_H
#define _GEMSFT_TASK_H

#include "data_manager.h"
#include "opt_vector.h"
#include "optimization.h"
#include "gemsfit_global_variables.h"
#include <nlopt.hpp>
#include "print.h"

// Definition of TGfitTask class
class TGfitTask : public Data_Manager
{
protected:

    long int anNodes;  ///< Number of allocated nodes (samples)

private:

    // initialize optimization
    /**
    * Initializes the NLopt obeject and the optimization task
    * @author DM
    * @param optv_ vector of optimized parameters
    * @param weighted_Tfun_sum_of_residuals
    * @date 06.05.2013
    */
    virtual void init_optim(vector<double> &optv_, /*int &countit,*/ double &weighted_Tfun_sum_of_residuals );

    /**
    * initialize optimization object and assign constraints and bounds.
    * performs the optimization.
    * @param NLopti			nlopt optimization object
    * @param optv_              optimization vector
    * @param weighted_Tfun_sum_of_residuals		sum of squared residuals
    * @author DM
    * @date 07.05.2013
    */
    virtual void build_optim( nlopt::opt &NLopti, std::vector<double> &optv_, /*std::vector<System_Properties*> *systems, int &countit,*/ double &weighted_Tfun_sum_of_residuals );

    /**
    * Reads the target function options form the input file and adds them to the TargetFunction structure
    * @author DM
    * @date 07.05.2013
    */
    void get_DataTarget ( );

    /**
    * Checks for parameter inconsistencies with type of experimental data and other parameters
    * @author DM
    * @date 04.07.2013
    */
    void gfit_error ( );

    /**
    * Sets the GEMS nodes with T, P, composition and solution parameters based on the data in the experiments data manager class
    * @author DM
    * @date 01.05.2013
    */
    void  setnodes( );

    /**
    * Gets the value of the Log of the equilibrium constant for the marked reactions in the input file based on the G0 values in the used database
    * @author DM
    * @date 01.04.2013
    */
    void get_logK_TPpairs();

    /**
    * Gets the initial value of (a+b) form c=(a+b) for each experiment where c is a linked paramater to a and b with coeficients 1 and 1. also set initial values and bounds.
    * @author DM
    * @date 03.02.2014
    */
    void get_Lparams_delta();


    /// the GEMSFIT configuration file (fixed to SS_GEMSFIT_input.dat)
    string param_file;

    /// Printing Flag: if this flag is set to one, the result of the optimization will be printed to file (via optimization.cpp)
    string resultsfile;
public:

    optimization *Opti; ///< pointer to optimization



    void test();

//    /// Monte Carlo flag: if true, then the MPI commands within the objective function call will not be executed. Instead, the loop over Monte Carlo runs is parallelized
//    bool MC_MPI;

    double weighted_Tfun_sum_of_residuals;

    /// normalize parameters flag
    bool NormParams;

    static TGfitTask* gft;   ///< static pointer to this class

    ResPrint* print;

    // indexes used in the dynamic functions//
    vector<int> EXPndx, COMPndx, PHndx, PHPndx, PAndx;
    int DYFndx;
    // true if gradient method is used
    bool h_grad;
    // titration IC initial values in moles //
    vector<double>  iNa, iO, iH, iCl;

    struct TargetFunction /// structure holding the target function information
    {
        string name; /// target function name
        string type; /// target function type
        string weight; /// type of weight

        struct obj_fun /// structure holding the information about the data to compare from the experiments
        {
            string exp_phase;
            string exp_CT;
            string exp_CN;
            string exp_unit;
            string exp_DCP;
            double meas_average;
            string param_type;
            vector<string> Tformula;
            vector<string> Telem;
        };
        vector<obj_fun*> objfun;
        vector<obj_fun*> objfunold;
        vector<obj_fun*> nestfun;
    };

    TargetFunction* Tfun; /// pointer to target function structure

    /// Computed values for Monte Carlo confidence interval generation
    double_v computed_values_v;
    /// Measurement values for Monte Carlo confidence interval generation
    double_v measured_values_v;

    /// Computed residuals
    double_v residuals_v; // measured - calculated residuals
    double mean_reisdulas;
//    double_v relative_residuals_v; // 100*(measured-calculated)/measured in %
    double_v Weighted_Tfun_residuals_v; // Target function "residuals"
    double_v Tfun_residuals_v;
    double_v weights; // weights
    double_v Tuckey_weights; // hoding the Tuckey weights

    int number_of_residuals;


   TGfitTask ();   ///< Constructor

   ~TGfitTask ();      /// Destructor

   // initialize and runs optimization
   /**
   * Initializez and runs the optimization
   * @author DM
   * @date 01.07.2013
   */
   void run_optim ();


   /**
   * Gets the sum of residuals based on the Objective and target functions. This value is minimized by the fitter.
   * @author DM
   * @date 24.07.2013
   */
   void get_sum_of_residuals ( double &residual);

   /**
   * Gets the residual from one experiment (exp index) and based on one comparison option (obj index in Tfun->objfun[obj])
   * @param exp index of experiment
   * @param obj index of objective function what to compare
   * @author DM
   * @date 30.01.2014
   */
   double get_residual (int exp, int obj);

   /**
   * Adds the Monte Carlo Scatter to the measured values
   * @param scatter vector of scatter values
   * @author DM
   * @date 30.10.2013
   */
   void add_MC_scatter(vector<double> scatter);

   /**
   * Counts the number of residuals basesd on the current what to compare
   * @author DM
   * @date 30.10.2013
   */
   int get_number_of_residuals(TargetFunction *Tfun );

   void Ainit_optim (std::vector<double> &optv_);

   /**
   * Stores the values for each experiment after runing GEMS
   * @author DM
   * @param computed computed value
   * @param measured measured value
   * @param Weighted_Tfun_residual value of the weighted target function residual
   * @param Tfun_residual value of the target function residual unweighted
   * @param weight value of the weight
   * @date 01.07.2013
   */
   void set_residuals (double computed, double measured, double Weighted_Tfun_residual, double Tfun_residual, double weight );

};

#endif   // _GEMSFT_TASK_H

// end gemsft_task
