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

// Definition of TGfitTask class
class TGfitTask : public Data_Manager
{
protected:

    long int anNodes;  ///< Number of allocated nodes (samples)

    struct parameters /// structure holding the target function information
    {
        std::string Ptype;
        std::string Pfittype;
        std::string Pname;
        double Ival;
        double Fval;
        double CSS;
        double mc95;
        double mcSTDEV;
        double mcMEAN;
        std::vector<double> mcconfi;
        std::vector<double> correl;
    };

    std::vector<std::shared_ptr<parameters>> fitparam;

private:

    // initialize optimization
    /**
    * Initializes the NLopt obeject and the optimization task
    * @author DM
    * @param optv_ std::vector of optimized parameters
    * @param weighted_Tfun_sum_of_residuals
    * @date 06.05.2013
    */
    virtual void init_optim(std::vector<double> &optv_, /*int &countit,*/ double &weighted_Tfun_sum_of_residuals );

    /**
    * initialize optimization object and assign constraints and bounds.
    * performs the optimization.
    * @param NLopti			nlopt optimization object
    * @param optv_              optimization std::vector
    * @param weighted_Tfun_sum_of_residuals		sum of squared residuals
    * @author DM
    * @date 07.05.2013
    */
    virtual void build_optim( nlopt::opt &NLopti, std::vector<double> &optv_, /*std::std::vector<System_Properties*> *systems, int &countit,*/ double &weighted_Tfun_sum_of_residuals );

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

    void set_print_param();

    /**
    * Gets the value of the Log of the equilibrium constant for the marked reactions in the input file based on the G0 values in the used database
    * @author DM
    * @date 01.04.2013
    */
    void set_logK_TPpairs();

    void set_logK_TPpairs(std::vector<std::string> logK);

    /**
    * Gets the initial value of (a+b) form c=(a+b) for each experiment where c is a linked paramater to a and b with coeficients 1 and 1. also set initial values and bounds.
    * @author DM
    * @date 03.02.2014
    */
    void get_Lparams_delta();

    void set_fixed_parameters();

    /// the GEMSFIT configuration file (fixed to SS_GEMSFIT_input.dat)
    std::string param_file;

    /// Printing Flag: if this flag is set to one, the result of the optimization will be printed to file (via optimization.cpp)
    std::string resultsfile;

public:

    void nestedpH();

    std::shared_ptr<optimization> Opti; ///< pointer to optimization

    void test();

    double weighted_Tfun_sum_of_residuals;

    double _init_residuals_sys = 0.0;
    double _init_Weighted_Abs_sum_of_residuals = 0.0;

    int mode;

    int mLook;

    /// normalize parameters flag
    bool NormParams;

    static TGfitTask* gft;   ///< static pointer to this class

    // indexes used in the dynamic functions//
    std::vector<int> EXPndx, COMPndx, PHndx, PHPndx, PAndx, NEFndx;
    struct vect
    {
       std::vector<unsigned int> ndx;
    };
    std::vector<std::shared_ptr<vect>> vPAndx, vEAndx; // keeps the indexes of the parameters (optPF, optPL, etc) and std::vector optNFParam when paralelized

    // true if gradient method is used
    bool h_grad;
    // titration IC initial values in moles //
    std::vector<double>  iNa, iO, iH, iCl;

    struct TargetFunction /// structure holding the target function information
    {
        std::string name; /// target function name
        std::string type; /// target function type
        std::string weight; /// type of weight

        struct obj_fun /// structure holding the information about the data to compare from the experiments
        {
            std::string exp_phase;
            std::string exp_CT;
            std::string exp_CN;
            std::string exp_unit;
            std::string exp_DCP;
            double meas_average = 0;
            std::string Ptype;
            std::string Otype;
            std::vector<std::string> Tformula;
            std::string expr;
            std::vector<std::string> Telem;
            std::vector<int> Helem;
            double TuWeight;
            double weight;
            double sT = -1.0;
            double sP = -1.0;
            struct rslts
            {
                double input_value=-1.0;
                double measured_value;
                double computed_value;
                double error_value = 0.0;
                double residual;
                double weight;
                double Tfun_residual;
                double WTfun_residual;
            };
            rslts results;
            bool isComputed;
            bool isProp = false;
            double SumWTFun /*= 0.0*/;
            unsigned int count = 0;
        };
        std::vector<obj_fun> objfun;
        std::vector<obj_fun> nestfun;
        std::vector<obj_fun> addout;
    };

    std::shared_ptr<TargetFunction> Tfun; /// pointer to target function structure

    std::vector<TargetFunction> aTfun;

    struct FunctionLogK
    {
        std::string Ftype;
        std::vector<double> Fcoef;
        unsigned int Rndx;
    };

    std::vector<FunctionLogK> FlogK;

    /// Computed values for Monte Carlo confidence interval generation
    double_v computed_values_v;
    /// Measurement values for Monte Carlo confidence interval generation
    double_v measured_values_v;
    //double_v measured_values_e; // errors

    /// Computed residuals
    double_v residuals_v; // measured - calculated residuals
    double mean_reisdulas;
    double_v Weighted_Tfun_residuals_v; // Target function "residuals"
    double_v Tfun_residuals_v;
    double_v weights; // weights

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
   double get_sum_of_residuals( );

   /**
   * Gets the residual from one experiment (exp index) and based on one comparison option (objfun)
   * @param exp index of experiment
   * @author DM
   * @date 30.01.2014
   */
   double get_residual (int exp, TargetFunction::obj_fun &objfun, int &count);

   /**
   * Gets the ADDOUT calculated properties
   * @param exp index of experiment
   * @author DM
   * @date 25.03.2014
   */
   void get_addout_calc (int exp, TargetFunction::obj_fun &addout );

   /**
    * @brief get_addout_meas Gets addout measured property form experiments read from the database
    * @param exp index of experiment
    * @param objfun objfun object
    */
   void get_addout_meas(int exp, TGfitTask::TargetFunction::obj_fun &objfun);

   /**
    * @brief get_addout_input Gets addout input comp form experiments read from the database
    * @param exp index of experiment
    * @param objfun objfun object
    */
   void get_addout_input(int exp, TGfitTask::TargetFunction::obj_fun &objfun);

   /**
   * Adds the Monte Carlo Scatter to the measured values
   * @param scatter std::vector of scatter values
   * @author DM
   * @date 30.10.2013
   */
   void add_MC_scatter(std::vector<double> scatter);

   /**
   * Counts the number of residuals basesd on the current what to compare
   * @author DM
   * @date 30.10.2013
   */
   int get_number_of_residuals( );

   /**
    * @brief set_average_objfun Sets the average measured value for each objfun in aTfun[i].objfun[j].meas_average
    * @author DM
    * @date 30.10.2013
    */
   void set_average_objfun ();

   /**
    * @brief Ainit_optim initializes the optimization inside the Monte Carlo function
    * @param optv_ std::vector of optimized parameters
    * @author DM
    * @date 30.10.2013
    */
   void Ainit_optim (std::vector<double> &optv_);

   /**
   * @brief set_results Stores the values for each experiment after runing GEMS
   * @author DM
   * @param computed computed value
   * @param measured measured value
   * @param Weighted_Tfun_residual value of the weighted target function residual
   * @param Tfun_residual value of the target function residual unweighted
   * @param weight value of the weight
   * @date 01.07.2013
   */
   void set_results (TGfitTask::TargetFunction::obj_fun &objfun, double computed, double measured, double error, double Weighted_Tfun_residual, double Tfun_residual, double weight );

   /**
    * @brief set_weights sets the weights for each sample form the database and for each objfun (if present), default weight is 1
    * @author DM
    * @date 20.01.2014
    */
   void set_weights ();

   /**
    * @brief print_param prints current parameter values to output file
    */
   void print_param();

   /**
    * @brief print_global_results prints the results of the global fitting to fit-results.csv
    * @author DM
    * @date 20.01.2014
    */
   void print_global_results ();

   /**
    * @brief print_nested_results prints the results of nfun and inverse modeling in fit-inverse-results.csv
    * @author DM
    * @date 20.01.2014
    */
   void print_nested_results ();

   /**
    * @brief reads in the DataLogK JSON entry form the input file
    * @author DM
    * @date 20.08.2014
    */
   void get_DataLogK ( );

   /**
    * @brief calculates the logK of the reactions involning the reaction constrined parameters
    * @author DM
    * @date 20.01.2014
    */
   void calc_logK_TP( );

   /**
    * @brief calculates the logK using the T-depedence polynomial
    * @author DM
    * @date 20.01.2014
    */
   double calc_logK_dT (std::vector<double> A, double Tk, double P , int Rndx, int e);

   /**
    * @brief calculates the logK using the RHO-depedence polynomial
    * @param A
    * @param Tk
    * @param P
    * @author DM
    * @date 20.01.2014
    */
   double calc_logK_dRHOw( std::vector<double> A, double Tk , double P );

   /**
    * @brief sets the paramteres for the extended Debye-Hueckel equation (Helgeson). For mixed salts it calculates a weighted average of the parameters
    * @author DM
    * @date 14.08.2014
    */
   void set_DH_Helgeson (int n);

   double BgammaTP(int flag, TNode *node, double Gf, double EpsW);
   double Gfunction (double RhoW, double Tc, double Pbar);
   double IonsizeTP(int flag, double Gf);

};

#endif   // _GEMSFT_TASK_H

// end gemsft_task
