/*
*	 Copyright (C) 2012 by Ferdinand F. Hingerl (hingerl@hotmail.com)
*
*	 This file is part of the thermodynamic fitting program GEMSFIT.
*
*    GEMSFIT is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    GEMSFIT is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU  General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with GEMSFIT.  If not, see <http://www.gnu.org/licenses/>.
*/


/**
 *	@file optimization.cpp
 *
 *	@brief this source file contains implementations of the Optimization class as well as the derived ActivityModel class. 
 * Member funtions read configuration parameters needed by the optimization algorithm from the GEMSFIT input file.     
 * This file contains all function implementations needed for the optimization such as assignment of bounds and constraints, initialization of the nlopt object as well as thje functions dealing with the actual parameter optimization.    
 *
 *	@author Ferdinand F. Hingerl
 *
 * 	@date 09.04.2012 
 *
 */


#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <math.h>
#include <nlopt.hpp>

#include <omp.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/foreach.hpp>

#include "node.h"
#include "system_properties.h"
#include "opt_vector.h"
#include "optimization.h"
//#include "gemsfit_global_functions.h"
#include "gemsfit_global_functions.h"
#include "plot_class.h"

#define BIGVAL 1e20



// Define global MPI variables (declared as extern in gemsfit_global_variables.h)
#ifdef USE_MPI
int pid;
int p;
#endif
int master_counter = 0;
int slave_counter = 0;

#ifndef USE_MPI
int pid = 0;
int p = 1;
#endif



namespace opti
{

	Optimization::Optimization( const vector<double> vec_opt )
	{ 
		constraint_data = new my_constraint_data;

		Plot_ActMod = new PlotFit();

		set_nlopt_param();
	}


	Optimization::~Optimization()
	{ 
		delete constraint_data; 

		delete Plot_ActMod;
	}

	// Read in all optimization parameters from GEMSFIT_input.dat 
	void Optimization::set_nlopt_param()
	{
	}
	
	// Normalize bounds and init guess vector
	void Optimization::normalize_params( const vector<double> initguesses )
	{
	}

	// NLopt return codes
	void Optimization::print_return_message( const int result )
	{
	}	

	// virtual constraint function of base class Optimization
	double Optimization::constraint_function(const std::vector<double> &, std::vector<double> &, void *)
	{
	return 0.0;	
	}

	// Initialize optimization object and assign constraints and bounds
	void Optimization::init_optim( std::vector<double> &optv_, std::vector<System_Properties*> *sys, int &countit, double &sum_of_squares )
	{ 
	}

	void Optimization::build_optim( nlopt::opt &opt_actmod, std::vector<double> &optv_, std::vector<System_Properties*> *systems, int &countit, double &sum_of_squares )
	{
	}

	void Optimization::build_hybrid_optim( nlopt::opt &opt_hybrid_actmod, std::vector<double> &optv_, std::vector<System_Properties*> *systems, int &countit, double &sum_of_squares )
	{
	}

	// Initialize multistart optimization object and assign constraints and bounds
    void Optimization::init_optim_multistart( std::vector<double> &optv_, std::vector<System_Properties*> *systems, int &countit, double &sum_of_squares )
	{
	}

	// Print part of the fit results
	void Optimization::print_results( std::vector<double> &optv_, std::vector<System_Properties*> *systems )
	{
		if(!pid)
		{
			systems->at(0)->printfile = true;
		}
	}

// ============================ StdStateProp =========================== //

    // StdStateProp class (derived from base class Optimization)

    StdStateProp::StdStateProp( const vector<double> vec_opt ) : Optimization( vec_opt ), opt_vec(vec_opt)
    {
        // GEMSFIT logfile
        const char path[200] = "output_GEMSFIT/SS_GEMSFIT.log";
        ofstream fout;
        fout.open(path, ios::app);
        if( fout.fail() )
        { cout<<"Output fileopen error"<<endl; exit(1); }

        constraint_data = new my_constraint_data;

//		Plot_StdState = new PlotFit();

        fout << "12. in optimization.cpp line 160. Setting the NLOPT (optimization library) parameters read form SS_GEMSFIT_input.dat."<< endl << endl;
        set_nlopt_param();

        fout.close();
    }

    StdStateProp::~StdStateProp()
    {
        delete constraint_data;

//		delete Plot_StdState;
    }


    // Constraint function
    double StdStateProp::constraint_function(const std::vector<double> &x, std::vector<double> &grad, void *data)
    {
    return 0.0;
    }


    void StdStateProp::set_nlopt_param( )
    {
        string OptParamFile("SS_INPUT/SS_GEMSFIT_input.dat");
        int pos_start, pos_end;
        unsigned int i;
        ifstream param_stream;
        vector<string> data;
        string line, allparam;
        string s1("<OptAlgo>");
        string s2("<OptThreads>");
        string s3("<OptUpBounds>");
        string s4("<OptLoBounds>");
        string s5("<OptTolRel>");
        string s14("<OptTolAbs>");
        string s6("<OptMaxEval>");
        string s7("<OptUpConstraints>");
        string s8("<OptLoConstraints>");
        string s9("<OptConstraints>");
        string s10("<OptDoWhat>");
        string s11("<OptStatOnlyBestFitParam>");
        string s12("<OptStatOnlySSR>");
        string s13("<OptEqSolv>");
        string s15("<OptHybridTolRel>");
        string s16("<OptHybridTolAbs>");
        string s17("<OptHybridMaxEval>");
        string s18("<OptHybridMode>");
        string s19("<OptNmultistart>");
        string s20("<OptHybridAlgo>");
        string s21("<OptInitStep>");
        string s22("<OptScaleParam>");
        string s23("<OptNormParam>");
        string s24("<OptBoundPerc>");
        string hash("#");
        string Hp("+H");
        string Hm("-H");


        // Open configuration file for GEMSFIT run
        param_stream.open(OptParamFile.c_str());
        if( param_stream.fail() )
        {
            cout << "Opening of file "<<OptParamFile<<" failed !!"<<endl;
            exit(1);
        }
        while( getline(param_stream, line) )
        {
            data.push_back(line);
        }
        param_stream.close();
        for( i=0; i < data.size(); i++ )
        allparam += data[i];


        const char path[200] = "output_GEMSFIT/SS_GEMSFIT.log";
        ofstream fout;
        fout.open(path, ios::app);
        if( fout.fail() )
        { cout<<"Output fileopen error"<<endl; exit(1); }


        // Algorithm name
        string Algo_s, sub_Algo;
        pos_start = allparam.find(s1);
        pos_end   = allparam.find(hash,pos_start);
        Algo_s = allparam.substr((pos_start+s1.length()),(pos_end-pos_start-s1.length()));
        istringstream Algo_ss(Algo_s);
        for( i=0; i<1; i++)
        {
            Algo_ss >> sub_Algo;
            OptAlgo = sub_Algo;
        }
        fout<<"OptAlgo = "<<OptAlgo<<endl;

        // Number of threads
        string Threads_s, sub_Threads;
        pos_start = allparam.find(s2);
        pos_end   = allparam.find(hash,pos_start);
        Threads_s = allparam.substr((pos_start+s2.length()),(pos_end-pos_start-s2.length()));
        istringstream Threads_ss(Threads_s);
        for( i=0; i<1; i++)
        {
            Threads_ss >> sub_Threads;
            OptThreads = atoi(sub_Threads.c_str());
        }
        fout<<"OptThreads = "<<OptThreads<<endl;


        // Generate bounds from initial guess vector: <OptBoundPerc> gives percentage deviation
        string BP_s, sub_BP;
        pos_start = allparam.find(s24);
        pos_end   = allparam.find(hash,pos_start);
        BP_s = allparam.substr((pos_start+s24.length()),(pos_end-pos_start-s24.length()));
        istringstream BP_ss(BP_s);
        BP_ss >> sub_BP;
        if( atof(sub_BP.c_str()) < 0. )
        {
            fout<<"Note: no OptBoundPerc specified. Using user defined bounds ..."<<endl;
            OptBoundPerc = -1.;
        }
        else
        {
            OptBoundPerc = atof(sub_BP.c_str());
        }
        fout<<"OptBoundPerc = "<<OptBoundPerc<<endl;

        if( OptBoundPerc > 0. )
        {
            OptUpBounds.resize( opt_vec.size() );
            OptLoBounds.resize( opt_vec.size() );
            for( i=0; i<opt_vec.size(); i++ )
            {
                OptUpBounds[i] = opt_vec[i] + fabs( opt_vec[i]*OptBoundPerc/100. );
                OptLoBounds[i] = opt_vec[i] - fabs( opt_vec[i]*OptBoundPerc/100. );
            }
        }
        else
        {
            // Upper bounds
            string OptUpBounds_s, sub_OptUpBounds;
            pos_start = allparam.find(s3);
            pos_end   = allparam.find(s4,pos_start);
            OptUpBounds_s = allparam.substr((pos_start+s3.length()),(pos_end-pos_start-s3.length()));
            istringstream OptUpBounds_ss(OptUpBounds_s);
            do
            {
                OptUpBounds_ss >> sub_OptUpBounds;
                if( sub_OptUpBounds.empty() )
                {
                    fout<<"Note: no upper bounds specified ..."<<endl; break;
                }
                else if( !sub_OptUpBounds.compare(Hp) )
                {
                    OptUpBounds.push_back(BIGVAL);
                }
                else
                {
                    OptUpBounds.push_back(atof(sub_OptUpBounds.c_str()));
                };
            }while(OptUpBounds_ss);
            if( !OptUpBounds.empty() )
            {
                OptUpBounds.pop_back();
                fout << "OptUpBounds.size() = " << OptUpBounds.size() << endl;
                for( i=0; i< (int) OptUpBounds.size(); i++ )
                    fout<<"OptUpBounds["<<i<<"] = "<<OptUpBounds[i]<<endl;
            }

            // Lower bounds
            string OptLoBounds_s, sub_OptLoBounds;
            pos_start = allparam.find(s4);
            pos_end   = allparam.find(hash,pos_start);
            OptLoBounds_s = allparam.substr((pos_start+s4.length()),(pos_end-pos_start-s4.length()));
            istringstream OptLoBounds_ss(OptLoBounds_s);
            do
            {
                OptLoBounds_ss >> sub_OptLoBounds;
                if( sub_OptLoBounds.empty() )
                {
                    fout<<"Note: no lower bounds specified ..."<<endl;
                    break;
                }
                else if( !sub_OptLoBounds.compare(Hm) )
                {
                    OptLoBounds.push_back(-BIGVAL);
                }
                else
                {
                    OptLoBounds.push_back(atof(sub_OptLoBounds.c_str()));
                };
            }while(OptLoBounds_ss);

            if( !OptLoBounds.empty() )
            {
                OptLoBounds.pop_back();
                for( i=0; i< (int) OptLoBounds.size(); i++ )
                    fout<<"OptLoBounds["<<i<<"] = "<<OptLoBounds[i]<<endl;
            }
            // Check if all lower bounds are smaller than the corresponding upper bounds
            if( OptLoBounds.size() != OptUpBounds.size() )
            {
                cout<<endl;
                cout<<"upper and lower bounds must have same number of elements !! "<<endl;
                    cout<<"exiting now ..."<<endl;
                    cout<<endl;
                exit(1);
            }
            for( i=0; i<OptLoBounds.size(); i++ )
            {
                if( OptLoBounds[i]>OptUpBounds[i] )
                {
                    cout<<endl;
                    cout<<"Inconsistence in bounds: OptLoBounds["<<i<<"] = "<<OptLoBounds[i]<<" has a bigger value than OptUpBounds["<<i<<"] = "<<OptUpBounds[i]<<endl;
                    cout<<" !! exiting now ..."<<endl;
                        cout<<endl;
                    exit(1);
                }
            }
        }


        // Relative optimization tolerance
        string Tol_s, sub_Tol;
        pos_start = allparam.find(s5);
        pos_end   = allparam.find(hash,pos_start);
        Tol_s = allparam.substr((pos_start+s5.length()),(pos_end-pos_start-s5.length()));
        istringstream Tol_ss(Tol_s);
        for( i=0; i<1; i++)
        {
            Tol_ss >> sub_Tol;
            if( sub_Tol.empty() )
            {
                fout<<"Note: no OptTolRel specified. using default value: 1e-04 ..."<<endl;
                OptTolRel = 1e-04;
                break;
            }
            else
            {
                OptTolRel = atof(sub_Tol.c_str());
            };
        };
        fout<<"OptTolRel = "<<OptTolRel<<endl;

        // Absolute optimization tolerance
        string TolA_s, sub_TolA;
        pos_start = allparam.find(s14);
        pos_end   = allparam.find(hash,pos_start);
        TolA_s = allparam.substr((pos_start+s14.length()),(pos_end-pos_start-s14.length()));
        istringstream TolA_ss(TolA_s);
        for( i=0; i<1; i++)
        {
            TolA_ss >> sub_TolA;
            if( sub_TolA.empty() )
            {
                fout<<"Note: no OptTolAbs specified. using default value: 1e-04 ..."<<endl;
                OptTolAbs = 1e-04;
                break;
            }
            else
            {
                OptTolAbs = atof(sub_TolA.c_str());
            };
        };
        fout<<"OptTolAbs = "<<OptTolAbs<<endl;

        // Max number of evaluations
        string MaxEval_s, sub_MaxEval;
        pos_start = allparam.find(s6);
        pos_end   = allparam.find(hash,pos_start);
        MaxEval_s = allparam.substr((pos_start+s6.length()),(pos_end-pos_start-s6.length()));
        istringstream MaxEval_ss(MaxEval_s);
        for( i=0; i<1; i++)
        {
            MaxEval_ss >> sub_MaxEval;
            OptMaxEval = atoi(sub_MaxEval.c_str());
        };
        fout<<"OptMaxEval = "<<OptMaxEval<<endl;


        // Specify whether to apply constraints or no not
        string Constr_s, sub_Constr;
        pos_start = allparam.find(s9);
        pos_end   = allparam.find(hash,pos_start);
        Constr_s = allparam.substr((pos_start+s9.length()),(pos_end-pos_start-s9.length()));
        istringstream Constr_ss(Constr_s);
        for( i=0; i<1; i++)
        {
            Constr_ss >> sub_Constr;
            OptConstraints = atoi(sub_Constr.c_str());
        };
        fout<<"OptConstraints = "<<OptConstraints<<endl;

        if( OptConstraints )
        {
            // Temporary vectors holding constraints values
            vector<int> id_;
            vector<double> OptUpConstraints_;
            vector<double> OptLoConstraints_;

            // Upper constraints
            int k=1;
            string OptUpConstraints_s, sub_OptUpConstraints;
            pos_start = allparam.find(s7);
            pos_end   = allparam.find(s8,pos_start);
            OptUpConstraints_s = allparam.substr((pos_start+s7.length()),(pos_end-pos_start-s7.length()));
            istringstream OptUpConstraints_ss(OptUpConstraints_s);
            do
            {
                OptUpConstraints_ss >> sub_OptUpConstraints;
                if( sub_OptUpConstraints.empty() )
                {
                    fout<<endl;
                    fout<<"Note: no upper constraints specified ..."<<endl;
                        cout<<endl;
                    break;
                }
                else
                {
                    id_.push_back( k ); k++;
                    OptUpConstraints_.push_back( atof(sub_OptUpConstraints.c_str()) );
                };
            }while(OptUpConstraints_ss);
    //cout<<"constraint_data->OptUpConstraints[0] = "<<constraint_data->OptUpConstraints[0]<<" | constraint_data->OptUpConstraints[1] = "<<constraint_data->OptUpConstraints[1]<<endl;


            // Lower constraints
            string OptLoConstraints_s, sub_OptLoConstraints;
            pos_start = allparam.find(s8);
            pos_end   = allparam.find(hash,pos_start);
            OptLoConstraints_s = allparam.substr((pos_start+s8.length()),(pos_end-pos_start-s8.length()));
            istringstream OptLoConstraints_ss(OptLoConstraints_s);
            do
            {
                OptLoConstraints_ss >> sub_OptLoConstraints;
                if( sub_OptUpConstraints.empty() )
                {
                    fout<<endl;
                    fout<<"Note: no lower constraints specified ..."<<endl;
                    fout<<endl;
                    break;
                }
                else
                {
                    OptLoConstraints_.push_back( atof(sub_OptLoConstraints.c_str()) );
                };
            }while(OptLoConstraints_ss);
    //cout<<"constraint_data->OptLoConstraints[0] = "<<constraint_data->OptLoConstraints[0]<<" | constraint_data->OptLoConstraints[1] = "<<constraint_data->OptLoConstraints[1]<<endl;
            my_constraint_data constrdata;

    /*		for( i=0; i<id_.size(); i++)
            {
                constrdata.id = id_[i];
                constrdata.OptUpConstraints = OptUpConstraints_[i];
                constrdata.OptLoConstraints = OptLoConstraints_[i];
                constraint_data_v.push_back(constrdata);
            };
    */
            int ii = 0;
            for( i=0; i< (int) OptUpConstraints_.size(); i++)
            {
                constrdata.id = i;
                constrdata.Constraints = OptUpConstraints_[i];
                constraint_data_v.push_back(constrdata);
                ii = i;
            };
            for( i=0; i< (int) OptLoConstraints_.size(); i++)
            {
                constrdata.id = ii + 1 + i;
                constrdata.Constraints = OptLoConstraints_[i];
                constraint_data_v.push_back(constrdata);
            };

//#ifdef GEMSFIT_DEBUG
//for( i=0; i<constraint_data_v.size(); i++ )
//    cout << "Constraint vector = " << i << " " << constraint_data_v[i].Constraints << endl;
//#endif

            // Check if all lower constraints are smaller than the corresponding upper constraints
            if( OptUpConstraints_.size() != OptLoConstraints_.size() )
            {
                fout<<endl;
                fout<<"WARNING: upper and lower constraints do not have same number of elements !!"<<endl;
                fout<<"OptUpConstraints.size() = "<<OptUpConstraints_.size()<<" <-> OptLoConstraints.size() = "<<OptLoConstraints_.size()<<endl;
                fout<<"hope that's ok, proceeding now ..."<<endl;
                fout<<endl;
            }
            for( i=0; i<OptLoConstraints_.size(); i++ )
            {
                if( OptLoConstraints_[i] > OptUpConstraints_[i] )
                {
                    cout<<endl;
                    cout<<"Inconsistence in constrains: OptLoConstraints["<<i<<"] = "<<OptLoConstraints_[i]<<" has a bigger value than OptUpConstraints["<<i<<"] = "<<OptUpConstraints_[i]<<endl;
                    cout<<" .... exiting now .... "<<endl;
                        cout<<endl;
                    exit(1);
                };
            };
        }//enf if ( OptConstraints )


        // Perform optimization or only statistics
        string DoWhat_s, sub_DoWhat;
        pos_start = allparam.find(s10);
        pos_end   = allparam.find(hash,pos_start);
        DoWhat_s = allparam.substr((pos_start+s10.length()),(pos_end-pos_start-s10.length()));
        istringstream DoWhat_ss(DoWhat_s);
        for( i=0; i<1; i++)
        {
            DoWhat_ss >> sub_DoWhat;
            OptDoWhat = atoi(sub_DoWhat.c_str());
        };
        fout<<"OptDoWhat = "<<OptDoWhat<<endl;


        // If performing only statistics, provide best fit values
        string StatOnly_s, sub_StatOnly;
        pos_start = allparam.find(s11);
        pos_end   = allparam.find(hash,pos_start);
        StatOnly_s = allparam.substr((pos_start+s11.length()),(pos_end-pos_start-s11.length()));
        istringstream StatOnly_ss(StatOnly_s);
        do
        {
            StatOnly_ss >> sub_StatOnly;
            OptStatOnlyBestFitParam.push_back( atof(sub_StatOnly.c_str()) );
        }while(StatOnly_ss);
        for( i=0; i<OptStatOnlyBestFitParam.size(); i++ )
        fout<<"OptStatOnlyBestFitParam["<<i<<"] = "<<OptStatOnlyBestFitParam[i]<<endl;


        // If performing only statistics, provide SSR from fit
        string bestSSR_s, sub_bestSSR;
        pos_start = allparam.find(s12);
        pos_end   = allparam.find(hash,pos_start);
        bestSSR_s = allparam.substr((pos_start+s12.length()),(pos_end-pos_start-s12.length()));
        istringstream bestSSR_ss(bestSSR_s);
        for( i=0; i<1; i++)
        {
            bestSSR_ss >> sub_bestSSR;
            OptStatOnlySSR = atof(sub_bestSSR.c_str());
        };
        fout<<"OptStatOnlySSR = "<<OptStatOnlySSR<<endl;


        // If performing only statistics, provide SSR from fit
        string EQ_s, sub_EQ;
        pos_start = allparam.find(s13);
        pos_end   = allparam.find(hash,pos_start);
        EQ_s = allparam.substr((pos_start+s13.length()),(pos_end-pos_start-s13.length()));
        istringstream EQ_ss(EQ_s);
        EQ_ss >> sub_EQ;
        OptEquil = atof(sub_EQ.c_str());
        fout<<"OptEquil = "<<OptEquil<<endl;


        // Perform hybrid optimization
        string Hybrid_s, sub_Hybrid;
        pos_start = allparam.find(s18);
        pos_end   = allparam.find(hash,pos_start);
        Hybrid_s = allparam.substr((pos_start+s18.length()),(pos_end-pos_start-s18.length()));
        istringstream Hybrid_ss(Hybrid_s);
        Hybrid_ss >> sub_Hybrid;
        OptHybridMode = atoi(sub_Hybrid.c_str());
        fout<<"OptHybridMode = "<<OptHybridMode<<endl;


        if( OptHybridMode )
        {

            // Algorithm name
            string HAlgo_s, sub_HAlgo;
            pos_start = allparam.find(s20);
            pos_end   = allparam.find(hash,pos_start);
            HAlgo_s = allparam.substr((pos_start+s20.length()),(pos_end-pos_start-s20.length()));
            istringstream HAlgo_ss(HAlgo_s);
            HAlgo_ss >> sub_HAlgo;
            OptHybridAlgo = sub_HAlgo;
            fout<<"OptHybridAlgo = "<<OptHybridAlgo<<endl;


            // Relative hybride optimization tolerance
            string TolH_s, sub_TolH;
            pos_start = allparam.find(s15);
            pos_end   = allparam.find(hash,pos_start);
            TolH_s = allparam.substr((pos_start+s15.length()),(pos_end-pos_start-s15.length()));
            istringstream TolH_ss(TolH_s);
            for( i=0; i<1; i++)
            {
                TolH_ss >> sub_TolH;
                if( sub_Tol.empty() )
                {
                    fout<<"Note: no OptHybridTolRel specified. using default value: 1e-04 ..."<<endl;
                    OptHybridTolRel = 1e-04;
                    break;
                }
                else
                {
                    OptHybridTolRel = atof(sub_TolH.c_str());
                };
            };
            fout<<"OptHybridTolRel = "<<OptHybridTolRel<<endl;

            // Absolute hybride optimization tolerance
            string TolAH_s, sub_TolAH;
            pos_start = allparam.find(s16);
            pos_end   = allparam.find(hash,pos_start);
            TolAH_s = allparam.substr((pos_start+s16.length()),(pos_end-pos_start-s16.length()));
            istringstream TolAH_ss(TolAH_s);
            for( i=0; i<1; i++)
            {
                TolAH_ss >> sub_TolAH;
                if( sub_TolAH.empty() )
                {
                    fout<<"Note: no OptHybridTolAbs specified. using default value: 1e-04 ..."<<endl;
                    OptHybridTolAbs = 1e-04;
                    break;
                }
                else
                {
                    OptHybridTolAbs = atof(sub_TolAH.c_str());
                };
            };
            fout<<"OptHybridTolAbs = "<<OptHybridTolAbs<<endl;

            // Max number of hybride evaluations
            string MaxEvalH_s, sub_MaxEvalH;
            pos_start = allparam.find(s17);
            pos_end   = allparam.find(hash,pos_start);
            MaxEvalH_s = allparam.substr((pos_start+s17.length()),(pos_end-pos_start-s17.length()));
            istringstream MaxEvalH_ss(MaxEvalH_s);
            for( i=0; i<1; i++)
            {
                MaxEvalH_ss >> sub_MaxEvalH;
                OptHybridMaxEval = atoi(sub_MaxEvalH.c_str());
            };
            fout<<"OptHybridMaxEval = "<<OptHybridMaxEval<<endl;
        }

        // Number of multistart runs
        string NMulti_s, sub_NMulti;
        pos_start = allparam.find(s19);
        pos_end   = allparam.find(hash,pos_start);
        NMulti_s = allparam.substr((pos_start+s19.length()),(pos_end-pos_start-s19.length()));
        istringstream NMulti_ss(NMulti_s);
        NMulti_ss >> sub_NMulti;
        OptNmultistart = atoi(sub_NMulti.c_str());
        fout<<"OptNmultistart = "<<OptNmultistart<<endl;



        // Initial stepsize in percent (this percentage will be applied to all parameters)
        string InS_s, sub_InS;
        pos_start = allparam.find(s21);
        pos_end   = allparam.find(hash,pos_start);
        InS_s = allparam.substr((pos_start+s21.length()),(pos_end-pos_start-s21.length()));
        istringstream InS_ss(InS_s);
        InS_ss >> sub_InS;
        if( sub_InS.empty() )
        {
            fout<<"Note: no OptInitStep specified. using default heurustic value computed by NLOPT ..."<<endl;
            OptInitStep = -1;
        }
        else
        {
            OptInitStep = atof(sub_InS.c_str());
        }
        fout<<"OptInitStep = "<<OptInitStep<<endl;


        // Initial stepsize in percent (this percentage will be applied to all parameters)
        string SP_s, sub_SP;
        pos_start = allparam.find(s22);
        pos_end   = allparam.find(hash,pos_start);
        SP_s = allparam.substr((pos_start+s22.length()),(pos_end-pos_start-s22.length()));
        istringstream SP_ss(SP_s);
        SP_ss >> sub_SP;
        if( sub_SP.empty() )
        {
            fout<<"Note: no OptScaleParam specified. Not using scaling of parameters ..."<<endl;
            OptScaleParam = 0;
        }
        else
        {
            OptScaleParam = atof(sub_SP.c_str());
        }
        fout<<"OptScaleParam = "<<OptScaleParam<<endl;


        // Normalize bounds/fitting parameters with initial guess vector
        string NP_s, sub_NP;
        pos_start = allparam.find(s23);
        pos_end   = allparam.find(hash,pos_start);
        NP_s = allparam.substr((pos_start+s23.length()),(pos_end-pos_start-s23.length()));
        istringstream NP_ss(NP_s);
        NP_ss >> sub_NP;
        if( sub_NP.empty() )
        {
            fout<<"Note: no OptNormParam specified. Not using m of parameters ..."<<endl;
            OptNormParam = 0;
        }
        else
        {
            OptNormParam = atoi(sub_NP.c_str());
        }
        fout<<"OptNormParam = "<<OptNormParam<<endl;

        fout<<"End set_nlopt_param()." << endl << endl;

        param_stream.close();
        fout.close();

    }//set_nlopt_param()

    // Normalize bounds and init guess vector
    void StdStateProp::normalize_params( const vector<double> initguesses )
    {
        unsigned int i= 0;

        // GEMSFIT logfile
        const char path[200] = "output_GEMSFIT/SS_GEMSFIT.log";
        ofstream fout;
        fout.open(path, ios::app);
        if( fout.fail() )
        { cout<<"Output fileopen error"<<endl; exit(1); }

        fout << " ... performing parameter normalization ... " << endl;


        // Normalize init guess vector
        optv.resize( initguesses.size() );
        for(i=0; i<optv.size(); i++)
        {
            optv[i] = initguesses[i] / abs(initguesses[i]);
        }

        // Normalize upper bounds vector
        for(i=0; i<OptUpBounds.size(); i++)
        {
            fout << "Init guess ["<<i<<"] = " << initguesses[i] << endl;
            fout << "Upper Bound old ["<<i<<"]= " << OptUpBounds[i] << endl;
            OptUpBounds[i] = OptUpBounds[i] / abs(initguesses[i]);
            fout << "Upper Bound new ["<<i<<"]= " << OptUpBounds[i] << endl;
        }

        // Normalize lower bounds vector
        for(i=0; i<OptLoBounds.size(); i++)
        {
            fout << "Init guess ["<<i<<"] = " << initguesses[i] << endl;
            fout << "Lower Bound old ["<<i<<"]= " << OptLoBounds[i] << endl;
            OptLoBounds[i] = OptLoBounds[i] / abs(initguesses[i]);
            fout << "Lower Bound new ["<<i<<"]= " << OptLoBounds[i] << endl;
        }

        // Normalize constraints vector
        for(i=0; i<constraint_data_v.size(); i++)
        {
            constraint_data_v[i].Constraints = constraint_data_v[i].Constraints / abs(initguesses[i]);
        }

        fout.close();
    }


    // NLopt return codes
    void StdStateProp::print_return_message( const int result )
    {
        switch (result)
        {
            case -5:
                cout<<" Halted because of a forced termination: the user called nlopt_force_stop(opt) on the optimization’s nlopt_opt object opt from the user’s objective function or constraints. "<<endl;
                break;
            case -4:
                cout<<" Halted because roundoff errors limited progress. (In this case, the optimization still typically returns a useful result.) "<<endl;
                break;
            case -3:
                cout<<" Ran out of memory. "<<endl;
                break;
            case -2:
                cout<<" Invalid arguments (e.g. lower bounds are bigger than upper bounds, an unknown algorithm was specified, etc.). "<<endl;
                break;
            case -1:
                cout<<" Generic failure code. "<<endl;
                break;
            case 1:
                cout<<" Generic success return value. "<<endl;
                break;
            case 2:
                cout<<" Optimization stopped because stopval was reached. "<<endl;
                break;
            case 3:
                cout<<" Optimization stopped because ftol_rel or ftol_abs was reached. "<<endl;
                break;
            case 4:
                cout<<" Optimization stopped because xtol_rel or xtol_abs was reached. "<<endl;
                break;
            case 5:
                cout<<" Optimization stopped because maxeval was reached. "<<endl;
                break;
            case 6:
                cout<<" Optimization stopped because maxtime was reached. "<<endl;
                break;
        }
    }// end print_return_message


    // Initialize optimization object and Run Optimization by calling build_optim
    void StdStateProp::init_optim( std::vector<double> &optv_, std::vector<SS_System_Properties*> *ss_systems, int &countit, double &sum_of_squares )
    {

        // Instantiate nlopt::opt object
        if( OptAlgo.compare( "LN_COBYLA" ) == 0 )
        {
            nlopt::opt stdstate( nlopt::LN_COBYLA, optv_.size() );
            build_optim( stdstate, optv_, ss_systems, countit, sum_of_squares );
        }
        else if( OptAlgo.compare( "GN_ISRES" ) == 0 )
        {
            nlopt::opt stdstate( nlopt::GN_ISRES, optv_.size() );
            build_optim( stdstate, optv_, ss_systems, countit, sum_of_squares );
        }
        else if( OptAlgo.compare( "LN_BOBYQA" ) == 0 )
        {
            nlopt::opt stdstate( nlopt::LN_BOBYQA, optv_.size() );
            build_optim( stdstate, optv_, ss_systems, countit, sum_of_squares );
        }
        else if( OptAlgo.compare( "GN_ORIG_DIRECT" ) == 0 )
        {
            nlopt::opt stdstate( nlopt::GN_ORIG_DIRECT, optv_.size() );
            build_optim( stdstate, optv_, ss_systems, countit, sum_of_squares );
        }
        else if( OptAlgo.compare( "GN_ORIG_DIRECT_L" ) == 0 )
        {
            nlopt::opt stdstate( nlopt::GN_ORIG_DIRECT_L, optv_.size() );
            build_optim( stdstate, optv_, ss_systems, countit, sum_of_squares );
        }
        else
        {
            cout<<" Unknown optimization algorithm !!!! "<<endl;
            cout<<" ... bail out now ... "<<endl;
            exit(1);
        }


    }


    // Initialize optimization object and Run Optimization
    void StdStateProp::build_optim( nlopt::opt &stdstate, std::vector<double> &optv_, std::vector<SS_System_Properties*> *ss_systems, int &countit, double &sum_of_squares )
    {
        unsigned int i = 0;
        int j = 0;
        std::vector<double> grad;

        // GEMSFIT logfile
        const char path[200] = "output_GEMSFIT/SS_GEMSFIT.log";
        ofstream ffout;
        ffout.open(path, ios::app);
        if( ffout.fail() )
        { cout<<"Output fileopen error"<<endl; exit(1); }

        ffout << " ... initializing optimization object in build_optim() ... " << endl;



        // Reset counter to zero
        master_counter = 0;

/*		// Do not normalize parameters
        if( ss_systems->at(0)->printfile || !OptNormParam )
        {
            optv = optv_;
        }
        else // normalize parameters
        {
            normalize_params( optv_ );
            ss_systems->at(0)->sysparam->NormParams = true;
        }*/

        // normalize parameters is OptNormParam Flag is set to yes (=1) and
        // if the program is not in printing mode (printfile = 0)
        if( OptNormParam && !ss_systems->at(0)->printfile )
        {
            normalize_params( optv_ );
            ss_systems->at(0)->sysprop->NormParams = true;
        }
        else // do not normalize parameters
        {
            optv = optv_;
        }


        // check if initial guesses have same number of elements as bound and check if initial guesses are within the bounds
        if( optv.size() != OptUpBounds.size() )
        {
            cout<<endl;
            cout<<"initial guess vector and bounds vector must have same number of elements !! "<<endl;
            cout<<"optv.size() = "<<optv.size()<<" <-> OptUbBounds.size() = "<<OptUpBounds.size()<<endl;
            cout<<" .... exiting now .... "<<endl;
                cout<<endl;
            exit(1);
        }
        for( i=0; i<OptLoBounds.size(); i++ )
        {
            if( optv[i]<OptLoBounds[i] || optv[i]>OptUpBounds[i] )
            {
                cout<<endl;
                cout<<"Initial guess value is not within given bounds!!"<<endl;
                cout<<"optv["<<i<<"] = "<<optv[i]<<" | OptLoBounds["<<i<<"] = "<<OptLoBounds[i]<<" | OptUpBounds["<<i<<"] = "<<OptUpBounds[i]<<endl;
                cout<<" .... exiting now .... "<<endl;
                    cout<<endl;
                exit(1);
            }
        }

        ffout << "... assigning bounds and tollerance for optimization..." << endl;
        // assign bounds
        stdstate.set_lower_bounds( OptLoBounds );
        stdstate.set_upper_bounds( OptUpBounds );


        // specify relative tolerance tolerance on function value
        stdstate.set_xtol_rel( OptTolRel );

        // specify absolute tolerance on function value
        stdstate.set_xtol_abs( OptTolAbs );


        // maximum number of iterations
        stdstate.set_maxeval( OptMaxEval );


        /// specify objective function
        ffout << endl << "15. in optimization.cpp line 991. Setting minimizing objective function." << endl;
//		if( !OptEquil )
//		{
//			// call wrapper for TSolMod class -> do not perform chemical equilibrium computation
//			stdstate.set_min_objective( StdState_objective_function_callback, ss_systems );
//		}
//		else if( OptEquil==1 )
//		{
            // call GEMS3K wrapper -> perform chemical equilibrium computation
            stdstate.set_min_objective( StdStateEquil_objective_function_callback, ss_systems );
////		}


//        if( OptConstraints )
//        {
//            // apply inequality constraints
//            for( int j=0; j< ((int) constraint_data_v.size()); j++ )
//                stdstate.add_inequality_constraint( StdStateProp_constraint_function_callback, &constraint_data_v[j], 1e-4);
//        }


        // Set initial stepsize
        if( OptInitStep > 0 )
        {
            vector<double> inistep( optv.size(), 0. );
            for( j=0; j<(int) optv.size(); j++ )
            {
                inistep[j] = optv[j] * OptInitStep;
            }
            stdstate.set_initial_step( inistep );
        }



        // Only the Master process runs the optimization library. The other processes only run the callback function (copying the newly generated guess values from process 0).
        int ierr;
        //ierr = MPI_Comm_rank( MPI_COMM_WORLD, &pid );
        //ierr = MPI_Comm_size( MPI_COMM_WORLD, &p );
        int continue_or_exit;

        ffout << "xx. in optimization.cpp line 1033. Performing optimization."<<endl;

        //===== For testing the objective function without oprimization =====//
//        sum_of_squares = StdStateEquil_objective_function_callback(optv, grad, ss_systems);


        // Perform optimization -> perform MPI call only when NOT in printing mode (function: StdStateProp::print_results()) AND NOT in Monte Carlo mode (function: Statistics::MC_confidence_interval())
//        if( !ss_systems->at(0)->printfile && !ss_systems->at(0)->MC_MPI )
//        {
//                // MASTER
//                if( !pid )
//                {
                    // run optimization -> perform MPI_Send() in global activity model callback
                    nlopt::result result = stdstate.optimize( optv, sum_of_squares );
                    ffout<<"optv[0] = "<<optv[0]<<endl;
                    ffout<<"size of optv = "<<optv.size()<<endl;


                // check results
                    if( result < 0 )
                    {
                        std::cout<<endl;
                        std::cout<<"   !!!  nlopt failed  !!!   "<<std::endl;
                        std::cout<<"   !!!  error code:   "<<result<<std::endl;
                        print_return_message( result );
                        std::cout<<endl;
                    }
                    else
                    {
                        std::cout<<" NLopt return code: "<<result<<endl;
                        print_return_message( result );
                        const char path[200] = "output_GEMSFIT/SS_myFIT.out";
                        ofstream fout;
                        fout.open(path, ios::app);
                        if( fout.fail() )
                        { cout<<"Output fileopen error"<<endl; exit(1); }
                        fout<<"pid "<<pid<<", found minimum at <<f( ";
                        for( unsigned i=0; i<optv.size(); i++ )
                        {
                            fout<<optv[i]<<" ";
                        }
                        fout<<") = "<<sum_of_squares<<std::endl;
                        fout<<" after "<< master_counter <<" evaluations."<<std::endl;
                        fout.close();

                        std::cout<<"found minimum at <<f( ";
                        for( unsigned i=0; i<optv.size(); i++ )
                        {
                            std::cout<<optv[i]<<" ";
                        }
                        std::cout<<") = "<<sum_of_squares<<std::endl;
                    }
                    std::cout<<" after "<< master_counter <<" evaluations"<<std::endl;


//                    // run hybrid optimization mode with gradient-based optimization
//                    if( OptHybridMode )
//                    {

//                        // Instantiate nlopt::opt object
//                        if( OptHybridAlgo.compare( "LN_COBYLA" ) == 0 )
//                        {
//                            nlopt::opt opt_hybrid_stdstate( nlopt::LN_COBYLA, optv_.size() );
//                            build_hybrid_optim( opt_hybrid_stdstate, optv, ss_systems, countit, sum_of_squares );
//                        }
//                        else if( OptHybridAlgo.compare( "GN_ISRES" ) == 0 )
//                        {
//                            nlopt::opt opt_hybrid_stdstate( nlopt::GN_ISRES, optv_.size() );
//                            build_hybrid_optim( opt_hybrid_stdstate, optv, ss_systems, countit, sum_of_squares );
//                        }
//                        else if( OptHybridAlgo.compare( "LN_BOBYQA" ) == 0 )
//                        {
//                            nlopt::opt opt_hybrid_stdstate( nlopt::LN_BOBYQA, optv_.size() );
//                            build_hybrid_optim( opt_hybrid_stdstate, optv, ss_systems, countit, sum_of_squares );
//                        }
//                        else if( OptHybridAlgo.compare( "GN_ORIG_DIRECT" ) == 0 )
//                        {
//                            nlopt::opt opt_hybrid_stdstate( nlopt::GN_ORIG_DIRECT, optv_.size() );
//                            build_hybrid_optim( opt_hybrid_stdstate, optv, ss_systems, countit, sum_of_squares );
//                        }
//                        else if( OptHybridAlgo.compare( "GN_ORIG_DIRECT_L" ) == 0 )
//                        {
//                            nlopt::opt opt_hybrid_stdstate( nlopt::GN_ORIG_DIRECT_L, optv_.size() );
//                            build_hybrid_optim( opt_hybrid_stdstate, optv, ss_systems, countit, sum_of_squares );
//                        }
//                        else
//                        {
//                            cout<<" Unknown optimization algorithm !!!! "<<endl;
//                            cout<<" ... bail out now ... "<<endl;
//                            exit(1);
//                        }


//                    }

//#ifdef USE_MPI
//                    // send stopping signal to all other threads when optimization finished
//                    int pid_;
//                    for( pid_=1; pid_<p; pid_++ )
//                    {
//                        continue_or_exit = 0;
//                        MPI_Send( &continue_or_exit, 1, MPI_INT, pid_, 0,  MPI_COMM_WORLD );
//                    }
//#endif

                    // copy resulting vector back to incoming optv vector (needed for printing results)
                    optv_ = optv;
//                }
//                else // SLAVES
//                {
//#ifdef USE_MPI
//                    MPI_Status status;
//                    int sender;
//                    int cont_or_exit = 0;

//                    MPI_Recv( &cont_or_exit, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status );
//                    while( cont_or_exit != 0 )
//                    {
//                        StdState_objective_function_callback( optv, grad, ss_systems );
//                        MPI_Recv( &cont_or_exit, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status );
//                        sender = status.MPI_SOURCE;
//                        if( cont_or_exit != 1 ) break;
//                    }
//#endif
//                }

//#ifdef USE_MPI
//                // Send the values generated by the master process to all other threads
//                ierr = MPI_Bcast( &sum_of_squares, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD );
//#endif
//                ffout<<"pid "<<pid<<" is out of loop ..."<<endl;

                // copy resulting vector back to incoming optv vector (needed for printing results)
                if( !OptNormParam )
                {
                    optv_ = optv;
                }
                else
                {
                    for( i=0; i<optv.size(); i++ )
                    {
                        optv_[i] = optv[i] * abs(ss_systems->at(0)->sysprop->init_guesses[i]);
                    }
                }
//        }
//        else // DO NOT PARALLELIZE LOOP OVER MEASUREMENTS IN OBJECTIVE FUNCTION if in printing mode (function: ActivityModel::print_results()) OR in Monte Carlo mode (function: Statistics::MC_confidence_interval())
//        {
//                nlopt::result result = stdstate.optimize( optv, sum_of_squares );
//                ffout<<"optv[0] = "<<optv[0]<<endl;
//                ffout<<"size of optv = "<<optv.size()<<endl;

//                // check results
//                if( result < 0 )
//                {
//                    std::cout<<endl;
//                    std::cout<<"   !!!  nlopt failed !!! "<<std::endl;
//                    std::cout<<"   !!!  error code: "<<result<<std::endl;
//                    print_return_message( result );
//                    std::cout<<endl;
//                }
//                else
//                {
//                    std::cout<<" NLopt return code: "<<result<<endl;
//                    print_return_message( result );
//                    const char path[200] = "output_GEMSFIT/myFIT.out";
//                    ofstream fout;
//                    fout.open(path, ios::app);
//                    if( fout.fail() )
//                    { cout<<"Output fileopen error"<<endl; exit(1); }
//                    fout<<"pid "<<pid<<", found minimum at <<f( ";
//                    for( unsigned i=0; i<optv.size(); i++ )
//                    {
//                        fout<<optv[i]<<" ";
//                    }
//                    fout<<") = "<<sum_of_squares<<std::endl;
//                    fout<<" after "<< master_counter <<" evaluations."<<std::endl;
//                    fout.close();

//                    std::cout<<"found minimum at <<f( ";
//                    for( unsigned i=0; i<optv.size(); i++ )
//                    {
//                        std::cout<<optv[i]<<" ";
//                    }
//                    std::cout<<") = "<<sum_of_squares<<std::endl;
//                }
//                std::cout<<" after "<< master_counter <<" evaluations"<<std::endl;

//                // copy resulting vector back to incoming optv vector (needed for printing results)
//                optv_ = optv;


//        }
        //stdstate.destroy();


    ffout.close();
    countit = master_counter;
    }



    void StdStateProp::build_hybrid_optim( nlopt::opt &opt_hybrid_stdstate, std::vector<double> &optv_, std::vector<SS_System_Properties*> *ss_systems, int &countit, double &sum_of_squares )
    {
        unsigned int i = 0;
        int j = 0;
        std::vector<double> grad;

        // GEMSFIT logfile
        const char path[200] = "output_GEMSFIT/GEMSFIT.log";
        ofstream ffout;
        ffout.open(path, ios::app);
        if( ffout.fail() )
        { cout<<"Output fileopen error"<<endl; exit(1); }

        ffout << " ... initializing hybrid optimization procedure ... " << endl;


        // Reset counter to zero
        master_counter = 0;

        optv = optv_;


        // check if initial guesses have same number of elements as bound and check if initial guesses are within the bounds
        if( optv.size() != OptUpBounds.size() )
        {
            cout<<endl;
            cout<<"initial guess vector and bounds vector must have same number of elements !! "<<endl;
            cout<<"optv.size() = "<<optv.size()<<" <-> OptUbBounds.size() = "<<OptUpBounds.size()<<endl;
            cout<<" .... exiting now .... "<<endl;
                cout<<endl;
            exit(1);
        }
        for( i=0; i<OptLoBounds.size(); i++ )
        {
            if( optv[i]<OptLoBounds[i] || optv[i]>OptUpBounds[i] )
            {
                cout<<endl;
                cout<<"Initial guess value is not within given bounds!!"<<endl;
                cout<<"optv["<<i<<"] = "<<optv[i]<<" | OptLoBounds["<<i<<"] = "<<OptLoBounds[i]<<" | OptUpBounds["<<i<<"] = "<<OptUpBounds[i]<<endl;
                cout<<" .... exiting now .... "<<endl;
                    cout<<endl;
                exit(1);
            }
        }


        // assign bounds
        opt_hybrid_stdstate.set_lower_bounds( OptLoBounds );
        opt_hybrid_stdstate.set_upper_bounds( OptUpBounds );


        // specify relative tolerance tolerance on function value
        opt_hybrid_stdstate.set_xtol_rel( OptHybridTolRel );

        // specify absolute tolerance on function value
        opt_hybrid_stdstate.set_xtol_abs( OptHybridTolAbs );


        // maximum number of iterations
        opt_hybrid_stdstate.set_maxeval( OptHybridMaxEval );


        /// specify objective function
//        if( !OptEquil )
//        {
//            // call wrapper for TSolMod class -> do not perform chemical equilibrium computation
//            opt_hybrid_stdstate.set_min_objective( StdState_objective_function_callback, ss_systems );
//        }
//        else if( OptEquil==1 )
//        {
//            // call GEMS3K wrapper -> perform chemical equilibrium computation
//            opt_hybrid_stdstate.set_min_objective( StdStateEquil_objective_function_callback, ss_systems );
//        }


//        if( OptConstraints )
//        {
//            // apply inequality constraints
//            for( j=0; j< ((int) constraint_data_v.size()); j++ )
//                opt_hybrid_stdstate.add_inequality_constraint( StdState_constraint_function_callback, &constraint_data_v[j], 1e-4);
//        }


        // Set initial stepsize
        if( OptInitStep > 0 )
        {
            vector<double> inistep( optv.size(), 0. );
            for( j=0; j<(int) optv.size(); j++ )
            {
                inistep[j] = optv[j] * OptInitStep;
            }
            opt_hybrid_stdstate.set_initial_step( inistep );
        }


        // run optimization
        nlopt::result result = opt_hybrid_stdstate.optimize( optv, sum_of_squares );
        ffout<<"optv[0] = "<<optv[0]<<endl;
        ffout<<"size of optv = "<<optv.size()<<endl;


        // check results
        if( result < 0 )
        {
            std::cout<<endl;
            std::cout<<"   !!!  nlopt failed  !!!   "<<std::endl;
            std::cout<<"   !!!  error code:   "<<result<<std::endl;
            print_return_message( result );
            std::cout<<endl;
        }
        else
        {
            std::cout<<" NLopt return code: "<<result<<endl;
            print_return_message( result );
            const char path[200] = "output_GEMSFIT/myFIT.out";
            ofstream fout;
            fout.open(path, ios::app);
            if( fout.fail() )
            { cout<<"Output fileopen error"<<endl; exit(1); }
            fout<<"pid "<<pid<<", found minimum at <<f( ";
            for( unsigned i=0; i<optv.size(); i++ )
            {
                fout<<optv[i]<<" ";
            }
            fout<<") = "<<sum_of_squares<<std::endl;
            fout<<" after "<< master_counter <<" evaluations."<<std::endl;
            fout.close();

            std::cout<<"found minimum at <<f( ";
            for( unsigned i=0; i<optv.size(); i++ )
            {
                std::cout<<optv[i]<<" ";
            }
            std::cout<<") = "<<sum_of_squares<<std::endl;
        }
        std::cout<<" after "<< master_counter <<" evaluations"<<std::endl;

        // copy resulting vector back to incoming optv vector (needed for printing results)
        optv_ = optv;;

    countit = master_counter;
    ffout.close();
    }



// ============================ END StdStateProp =========================== //

	// // // //  
	// ActivityModel class (derived from base class Optimization)

	ActivityModel::ActivityModel( const vector<double> vec_opt ) : Optimization( vec_opt ), opt_vec(vec_opt)
	{ 	
		constraint_data = new my_constraint_data;

		Plot_ActMod = new PlotFit();

		set_nlopt_param();
	}

	ActivityModel::~ActivityModel()
	{ 
		delete constraint_data;
 
		delete Plot_ActMod;
 	}


	// Constraint function
	double ActivityModel::constraint_function(const std::vector<double> &x, std::vector<double> &grad, void *data)
	{
	return 0.0;	
	}


	void ActivityModel::set_nlopt_param( )
	{
		string OptParamFile("GEMSFIT_input.dat");
		int pos_start, pos_end;
		unsigned int i;
	 	ifstream param_stream;
		vector<string> data;
		string line, allparam;
		string s1("<OptAlgo>");
		string s2("<OptThreads>");
		string s3("<OptUpBounds>");
		string s4("<OptLoBounds>");
		string s5("<OptTolRel>");
		string s14("<OptTolAbs>");
		string s6("<OptMaxEval>");
		string s7("<OptUpConstraints>");
		string s8("<OptLoConstraints>");
		string s9("<OptConstraints>");
		string s10("<OptDoWhat>");
		string s11("<OptStatOnlyBestFitParam>");
		string s12("<OptStatOnlySSR>");
		string s13("<OptEqSolv>");
		string s15("<OptHybridTolRel>");
		string s16("<OptHybridTolAbs>");
		string s17("<OptHybridMaxEval>");
		string s18("<OptHybridMode>");
		string s19("<OptNmultistart>");
		string s20("<OptHybridAlgo>");
		string s21("<OptInitStep>");
		string s22("<OptScaleParam>");
		string s23("<OptNormParam>");
		string s24("<OptBoundPerc>");
		string hash("#");
		string Hp("+H");
		string Hm("-H");


		// Open configuration file for GEMSFIT run
		param_stream.open(OptParamFile.c_str());	
		if( param_stream.fail() )
		{
			cout << "Opening of file "<<OptParamFile<<" failed !!"<<endl;
			exit(1);
		}
		while( getline(param_stream, line) ) 
		{
			data.push_back(line);
		} 
		param_stream.close();
		for( i=0; i < data.size(); i++ )
		allparam += data[i];


		const char path[200] = "output_GEMSFIT/GEMSFIT.log";
		ofstream fout;
		fout.open(path, ios::app);						
		if( fout.fail() )
		{ cout<<"Output fileopen error"<<endl; exit(1); }


		// Algorithm name
 		string Algo_s, sub_Algo;
		pos_start = allparam.find(s1);
		pos_end   = allparam.find(hash,pos_start);
		Algo_s = allparam.substr((pos_start+s1.length()),(pos_end-pos_start-s1.length()));
		istringstream Algo_ss(Algo_s);
		for( i=0; i<1; i++)
		{
		    Algo_ss >> sub_Algo;
			OptAlgo = sub_Algo;
		}	
		fout<<"OptAlgo = "<<OptAlgo<<endl;

		// Number of threads
		string Threads_s, sub_Threads;
		pos_start = allparam.find(s2);
		pos_end   = allparam.find(hash,pos_start);
		Threads_s = allparam.substr((pos_start+s2.length()),(pos_end-pos_start-s2.length()));
		istringstream Threads_ss(Threads_s);
		for( i=0; i<1; i++)
		{
		    Threads_ss >> sub_Threads;
			OptThreads = atoi(sub_Threads.c_str());
		}	
		fout<<"OptThreads = "<<OptThreads<<endl;


		// Generate bounds from initial guess vector: <OptBoundPerc> gives percentage deviation 
		string BP_s, sub_BP;
		pos_start = allparam.find(s24);
		pos_end   = allparam.find(hash,pos_start);
		BP_s = allparam.substr((pos_start+s24.length()),(pos_end-pos_start-s24.length()));
		istringstream BP_ss(BP_s);
		BP_ss >> sub_BP;
		if( atof(sub_BP.c_str()) < 0. )
		{
			fout<<"Note: no OptBoundPerc specified. Using user defined bounds ..."<<endl;
			OptBoundPerc = -1.; 
		}
		else
		{
			OptBoundPerc = atof(sub_BP.c_str());
		}
		fout<<"OptBoundPerc = "<<OptBoundPerc<<endl;

		if( OptBoundPerc > 0. )
		{
			OptUpBounds.resize( opt_vec.size() );
			OptLoBounds.resize( opt_vec.size() );
			for( i=0; i<opt_vec.size(); i++ )
			{
				OptUpBounds[i] = opt_vec[i] + fabs( opt_vec[i]*OptBoundPerc/100. ); 
				OptLoBounds[i] = opt_vec[i] - fabs( opt_vec[i]*OptBoundPerc/100. );
			}
		}
		else
		{
			// Upper bounds
			string OptUpBounds_s, sub_OptUpBounds;
			pos_start = allparam.find(s3);
			pos_end   = allparam.find(s4,pos_start);
			OptUpBounds_s = allparam.substr((pos_start+s3.length()),(pos_end-pos_start-s3.length()));
			istringstream OptUpBounds_ss(OptUpBounds_s);
			do
			{
				OptUpBounds_ss >> sub_OptUpBounds;
				if( sub_OptUpBounds.empty() )
				{ 
					fout<<"Note: no upper bounds specified ..."<<endl; break; 
				}
				else if( !sub_OptUpBounds.compare(Hp) )
				{
					OptUpBounds.push_back(BIGVAL);				
				}
				else
				{
					OptUpBounds.push_back(atof(sub_OptUpBounds.c_str()));
				};
			}while(OptUpBounds_ss);	
			if( !OptUpBounds.empty() )
			{ 
				OptUpBounds.pop_back();
				fout << "OptUpBounds.size() = " << OptUpBounds.size() << endl;
				for( i=0; i< (int) OptUpBounds.size(); i++ )
					fout<<"OptUpBounds["<<i<<"] = "<<OptUpBounds[i]<<endl;
			}		

			// Lower bounds
			string OptLoBounds_s, sub_OptLoBounds;
			pos_start = allparam.find(s4);
			pos_end   = allparam.find(hash,pos_start);
			OptLoBounds_s = allparam.substr((pos_start+s4.length()),(pos_end-pos_start-s4.length()));
			istringstream OptLoBounds_ss(OptLoBounds_s);
			do
			{
				OptLoBounds_ss >> sub_OptLoBounds;
				if( sub_OptLoBounds.empty() )
				{ 
					fout<<"Note: no lower bounds specified ..."<<endl; 
					break; 
				}
				else if( !sub_OptLoBounds.compare(Hm) )
				{
					OptLoBounds.push_back(-BIGVAL);				
				}
				else
				{
					OptLoBounds.push_back(atof(sub_OptLoBounds.c_str()));
				};
			}while(OptLoBounds_ss);	

			if( !OptLoBounds.empty() )
			{	
				OptLoBounds.pop_back();
				for( i=0; i< (int) OptLoBounds.size(); i++ )
					fout<<"OptLoBounds["<<i<<"] = "<<OptLoBounds[i]<<endl;
			}
			// Check if all lower bounds are smaller than the corresponding upper bounds
			if( OptLoBounds.size() != OptUpBounds.size() )
			{ 	
				cout<<endl;
				cout<<"upper and lower bounds must have same number of elements !! "<<endl;
				   	cout<<"exiting now ..."<<endl;
				   	cout<<endl;	
				exit(1); 
			}		
			for( i=0; i<OptLoBounds.size(); i++ )
			{
				if( OptLoBounds[i]>OptUpBounds[i] )
				{
					cout<<endl;
					cout<<"Inconsistence in bounds: OptLoBounds["<<i<<"] = "<<OptLoBounds[i]<<" has a bigger value than OptUpBounds["<<i<<"] = "<<OptUpBounds[i]<<endl;
					cout<<" !! exiting now ..."<<endl;
					    cout<<endl;	
					exit(1);
				}
			}
		}


		// Relative optimization tolerance
		string Tol_s, sub_Tol;
		pos_start = allparam.find(s5);
		pos_end   = allparam.find(hash,pos_start);
		Tol_s = allparam.substr((pos_start+s5.length()),(pos_end-pos_start-s5.length()));
		istringstream Tol_ss(Tol_s);
		for( i=0; i<1; i++)
		{
		    Tol_ss >> sub_Tol;
			if( sub_Tol.empty() )
			{
				fout<<"Note: no OptTolRel specified. using default value: 1e-04 ..."<<endl;
				OptTolRel = 1e-04; 
				break; 				
			}
			else
			{
				OptTolRel = atof(sub_Tol.c_str());
			};
		};	
		fout<<"OptTolRel = "<<OptTolRel<<endl;

		// Absolute optimization tolerance
		string TolA_s, sub_TolA;
		pos_start = allparam.find(s14);
		pos_end   = allparam.find(hash,pos_start);
		TolA_s = allparam.substr((pos_start+s14.length()),(pos_end-pos_start-s14.length()));
		istringstream TolA_ss(TolA_s);
		for( i=0; i<1; i++)
		{
		    TolA_ss >> sub_TolA;
			if( sub_TolA.empty() )
			{
				fout<<"Note: no OptTolAbs specified. using default value: 1e-04 ..."<<endl;
				OptTolAbs = 1e-04; 
				break; 				
			}
			else
			{
				OptTolAbs = atof(sub_TolA.c_str());
			};
		};	
		fout<<"OptTolAbs = "<<OptTolAbs<<endl;

		// Max number of evaluations
		string MaxEval_s, sub_MaxEval;
		pos_start = allparam.find(s6);
		pos_end   = allparam.find(hash,pos_start);
		MaxEval_s = allparam.substr((pos_start+s6.length()),(pos_end-pos_start-s6.length()));
		istringstream MaxEval_ss(MaxEval_s);
		for( i=0; i<1; i++)
		{
		    MaxEval_ss >> sub_MaxEval;
			OptMaxEval = atoi(sub_MaxEval.c_str());
		};	
		fout<<"OptMaxEval = "<<OptMaxEval<<endl;


		// Specify whether to apply constraints or no not
		string Constr_s, sub_Constr;
		pos_start = allparam.find(s9);
		pos_end   = allparam.find(hash,pos_start);
		Constr_s = allparam.substr((pos_start+s9.length()),(pos_end-pos_start-s9.length()));
		istringstream Constr_ss(Constr_s);
		for( i=0; i<1; i++)
		{
		    Constr_ss >> sub_Constr;
			OptConstraints = atoi(sub_Constr.c_str());
		};	
		fout<<"OptConstraints = "<<OptConstraints<<endl;
		
		if( OptConstraints )
		{
			// Temporary vectors holding constraints values
			vector<int> id_;
			vector<double> OptUpConstraints_;
			vector<double> OptLoConstraints_;

			// Upper constraints
			int k=1;
			string OptUpConstraints_s, sub_OptUpConstraints;
			pos_start = allparam.find(s7);
			pos_end   = allparam.find(s8,pos_start);
			OptUpConstraints_s = allparam.substr((pos_start+s7.length()),(pos_end-pos_start-s7.length()));
			istringstream OptUpConstraints_ss(OptUpConstraints_s);
			do
			{
			    OptUpConstraints_ss >> sub_OptUpConstraints;
				if( sub_OptUpConstraints.empty() )
				{ 
					fout<<endl;
					fout<<"Note: no upper constraints specified ..."<<endl;
				        cout<<endl;	
					break; 
				}
				else
				{
					id_.push_back( k ); k++;
					OptUpConstraints_.push_back( atof(sub_OptUpConstraints.c_str()) );
				};
			}while(OptUpConstraints_ss);	
	//cout<<"constraint_data->OptUpConstraints[0] = "<<constraint_data->OptUpConstraints[0]<<" | constraint_data->OptUpConstraints[1] = "<<constraint_data->OptUpConstraints[1]<<endl;


			// Lower constraints
			string OptLoConstraints_s, sub_OptLoConstraints;
			pos_start = allparam.find(s8);
			pos_end   = allparam.find(hash,pos_start);
			OptLoConstraints_s = allparam.substr((pos_start+s8.length()),(pos_end-pos_start-s8.length()));
			istringstream OptLoConstraints_ss(OptLoConstraints_s);
			do
			{
			    OptLoConstraints_ss >> sub_OptLoConstraints;
				if( sub_OptUpConstraints.empty() )
				{ 
					fout<<endl;
					fout<<"Note: no lower constraints specified ..."<<endl; 
					fout<<endl;
					break; 
				}
				else
				{
					OptLoConstraints_.push_back( atof(sub_OptLoConstraints.c_str()) );
				};
			}while(OptLoConstraints_ss);	
	//cout<<"constraint_data->OptLoConstraints[0] = "<<constraint_data->OptLoConstraints[0]<<" | constraint_data->OptLoConstraints[1] = "<<constraint_data->OptLoConstraints[1]<<endl;
			my_constraint_data constrdata;

	/*		for( i=0; i<id_.size(); i++)	
			{
				constrdata.id = id_[i]; 
				constrdata.OptUpConstraints = OptUpConstraints_[i];
				constrdata.OptLoConstraints = OptLoConstraints_[i];
				constraint_data_v.push_back(constrdata);
			};
	*/
			int ii = 0;
			for( i=0; i< (int) OptUpConstraints_.size(); i++)	
			{
				constrdata.id = i; 
				constrdata.Constraints = OptUpConstraints_[i];
				constraint_data_v.push_back(constrdata);
				ii = i;
			};
			for( i=0; i< (int) OptLoConstraints_.size(); i++)	
			{
				constrdata.id = ii + 1 + i; 
				constrdata.Constraints = OptLoConstraints_[i];
				constraint_data_v.push_back(constrdata);
			};

#ifdef GEMSFIT_DEBUG
for( i=0; i<constraint_data_v.size(); i++ )
	cout << "Constraint vector = " << i << " " << constraint_data_v[i].Constraints << endl;
#endif

			// Check if all lower constraints are smaller than the corresponding upper constraints
			if( OptUpConstraints_.size() != OptLoConstraints_.size() )
			{ 
				fout<<endl;
				fout<<"WARNING: upper and lower constraints do not have same number of elements !!"<<endl; 
				fout<<"OptUpConstraints.size() = "<<OptUpConstraints_.size()<<" <-> OptLoConstraints.size() = "<<OptLoConstraints_.size()<<endl;
				fout<<"hope that's ok, proceeding now ..."<<endl; 
				fout<<endl;
			}		
			for( i=0; i<OptLoConstraints_.size(); i++ )
			{
				if( OptLoConstraints_[i] > OptUpConstraints_[i] )
				{	
					cout<<endl;
					cout<<"Inconsistence in constrains: OptLoConstraints["<<i<<"] = "<<OptLoConstraints_[i]<<" has a bigger value than OptUpConstraints["<<i<<"] = "<<OptUpConstraints_[i]<<endl;
					cout<<" .... exiting now .... "<<endl;
				        cout<<endl;	
					exit(1);
				};
			};
		}//enf if ( OptConstraints )


		// Perform optimization or only statistics
		string DoWhat_s, sub_DoWhat;
		pos_start = allparam.find(s10);
		pos_end   = allparam.find(hash,pos_start);
		DoWhat_s = allparam.substr((pos_start+s10.length()),(pos_end-pos_start-s10.length()));
		istringstream DoWhat_ss(DoWhat_s);
		for( i=0; i<1; i++)
		{
		    DoWhat_ss >> sub_DoWhat;
			OptDoWhat = atoi(sub_DoWhat.c_str());
		};	
		fout<<"OptDoWhat = "<<OptDoWhat<<endl;


		// If performing only statistics, provide best fit values
		string StatOnly_s, sub_StatOnly;
		pos_start = allparam.find(s11);
		pos_end   = allparam.find(hash,pos_start);
		StatOnly_s = allparam.substr((pos_start+s11.length()),(pos_end-pos_start-s11.length()));
		istringstream StatOnly_ss(StatOnly_s);
		do
		{
			StatOnly_ss >> sub_StatOnly;
			OptStatOnlyBestFitParam.push_back( atof(sub_StatOnly.c_str()) );
		}while(StatOnly_ss);	
		for( i=0; i<OptStatOnlyBestFitParam.size(); i++ )
		fout<<"OptStatOnlyBestFitParam["<<i<<"] = "<<OptStatOnlyBestFitParam[i]<<endl;


		// If performing only statistics, provide SSR from fit
		string bestSSR_s, sub_bestSSR;
		pos_start = allparam.find(s12);
		pos_end   = allparam.find(hash,pos_start);
		bestSSR_s = allparam.substr((pos_start+s12.length()),(pos_end-pos_start-s12.length()));
		istringstream bestSSR_ss(bestSSR_s);
		for( i=0; i<1; i++)
		{
		    bestSSR_ss >> sub_bestSSR;
			OptStatOnlySSR = atof(sub_bestSSR.c_str());
		};	
		fout<<"OptStatOnlySSR = "<<OptStatOnlySSR<<endl;


		// If performing only statistics, provide SSR from fit
		string EQ_s, sub_EQ;
		pos_start = allparam.find(s13);
		pos_end   = allparam.find(hash,pos_start);
		EQ_s = allparam.substr((pos_start+s13.length()),(pos_end-pos_start-s13.length()));
		istringstream EQ_ss(EQ_s);
	    EQ_ss >> sub_EQ;
		OptEquil = atof(sub_EQ.c_str());
		fout<<"OptEquil = "<<OptEquil<<endl;


		// Perform hybrid optimization
		string Hybrid_s, sub_Hybrid;
		pos_start = allparam.find(s18);
		pos_end   = allparam.find(hash,pos_start);
		Hybrid_s = allparam.substr((pos_start+s18.length()),(pos_end-pos_start-s18.length()));
		istringstream Hybrid_ss(Hybrid_s);
	    Hybrid_ss >> sub_Hybrid;
		OptHybridMode = atoi(sub_Hybrid.c_str());
		fout<<"OptHybridMode = "<<OptHybridMode<<endl;


		if( OptHybridMode ) 
		{

			// Algorithm name
	 		string HAlgo_s, sub_HAlgo;
			pos_start = allparam.find(s20);
			pos_end   = allparam.find(hash,pos_start);
			HAlgo_s = allparam.substr((pos_start+s20.length()),(pos_end-pos_start-s20.length()));
			istringstream HAlgo_ss(HAlgo_s);
			HAlgo_ss >> sub_HAlgo;
			OptHybridAlgo = sub_HAlgo;
			fout<<"OptHybridAlgo = "<<OptHybridAlgo<<endl;


			// Relative hybride optimization tolerance
			string TolH_s, sub_TolH;
			pos_start = allparam.find(s15);
			pos_end   = allparam.find(hash,pos_start);
			TolH_s = allparam.substr((pos_start+s15.length()),(pos_end-pos_start-s15.length()));
			istringstream TolH_ss(TolH_s);
			for( i=0; i<1; i++)
			{
				TolH_ss >> sub_TolH;
				if( sub_Tol.empty() )
				{
					fout<<"Note: no OptHybridTolRel specified. using default value: 1e-04 ..."<<endl;
					OptHybridTolRel = 1e-04; 
					break; 				
				}
				else
				{
					OptHybridTolRel = atof(sub_TolH.c_str());
				};
			};	
			fout<<"OptHybridTolRel = "<<OptHybridTolRel<<endl;

			// Absolute hybride optimization tolerance
			string TolAH_s, sub_TolAH;
			pos_start = allparam.find(s16);
			pos_end   = allparam.find(hash,pos_start);
			TolAH_s = allparam.substr((pos_start+s16.length()),(pos_end-pos_start-s16.length()));
			istringstream TolAH_ss(TolAH_s);
			for( i=0; i<1; i++)
			{
				TolAH_ss >> sub_TolAH;
				if( sub_TolAH.empty() )
				{
					fout<<"Note: no OptHybridTolAbs specified. using default value: 1e-04 ..."<<endl;
					OptHybridTolAbs = 1e-04; 
					break; 				
				}
				else
				{
					OptHybridTolAbs = atof(sub_TolAH.c_str());
				};
			};	
			fout<<"OptHybridTolAbs = "<<OptHybridTolAbs<<endl;

			// Max number of hybride evaluations
			string MaxEvalH_s, sub_MaxEvalH;
			pos_start = allparam.find(s17);
			pos_end   = allparam.find(hash,pos_start);
			MaxEvalH_s = allparam.substr((pos_start+s17.length()),(pos_end-pos_start-s17.length()));
			istringstream MaxEvalH_ss(MaxEvalH_s);
			for( i=0; i<1; i++)
			{
				MaxEvalH_ss >> sub_MaxEvalH;
				OptHybridMaxEval = atoi(sub_MaxEvalH.c_str());
			};	
			fout<<"OptHybridMaxEval = "<<OptHybridMaxEval<<endl;
		}

		// Number of multistart runs
		string NMulti_s, sub_NMulti;
		pos_start = allparam.find(s19);
		pos_end   = allparam.find(hash,pos_start);
		NMulti_s = allparam.substr((pos_start+s19.length()),(pos_end-pos_start-s19.length()));
		istringstream NMulti_ss(NMulti_s);
		NMulti_ss >> sub_NMulti;
		OptNmultistart = atoi(sub_NMulti.c_str());	
		fout<<"OptNmultistart = "<<OptNmultistart<<endl;



		// Initial stepsize in percent (this percentage will be applied to all parameters)
		string InS_s, sub_InS;
		pos_start = allparam.find(s21);
		pos_end   = allparam.find(hash,pos_start);
		InS_s = allparam.substr((pos_start+s21.length()),(pos_end-pos_start-s21.length()));
		istringstream InS_ss(InS_s);
		InS_ss >> sub_InS;
		if( sub_InS.empty() )
		{
			fout<<"Note: no OptInitStep specified. using default heurustic value computed by NLOPT ..."<<endl;
			OptInitStep = -1; 
		}
		else
		{
			OptInitStep = atof(sub_InS.c_str());
		}
		fout<<"OptInitStep = "<<OptInitStep<<endl;


		// Initial stepsize in percent (this percentage will be applied to all parameters)
		string SP_s, sub_SP;
		pos_start = allparam.find(s22);
		pos_end   = allparam.find(hash,pos_start);
		SP_s = allparam.substr((pos_start+s22.length()),(pos_end-pos_start-s22.length()));
		istringstream SP_ss(SP_s);
		SP_ss >> sub_SP;
		if( sub_SP.empty() )
		{
			fout<<"Note: no OptScaleParam specified. Not using scaling of parameters ..."<<endl;
			OptScaleParam = 0; 
		}
		else
		{
			OptScaleParam = atof(sub_SP.c_str());
		}
		fout<<"OptScaleParam = "<<OptScaleParam<<endl;


		// Normalize bounds/fitting parameters with initial guess vector
		string NP_s, sub_NP;
		pos_start = allparam.find(s23);
		pos_end   = allparam.find(hash,pos_start);
		NP_s = allparam.substr((pos_start+s23.length()),(pos_end-pos_start-s23.length()));
		istringstream NP_ss(NP_s);
		NP_ss >> sub_NP;
		if( sub_NP.empty() )
		{
			fout<<"Note: no OptNormParam specified. Not using m of parameters ..."<<endl;
			OptNormParam = 0; 
		}
		else
		{
			OptNormParam = atoi(sub_NP.c_str());
		}
		fout<<"OptNormParam = "<<OptNormParam<<endl;

		param_stream.close();
		fout.close();

	}//set_nlopt_param() 


	
	// Normalize bounds and init guess vector
	void ActivityModel::normalize_params( const vector<double> initguesses )
	{
		unsigned int i= 0;

		// GEMSFIT logfile
		const char path[200] = "output_GEMSFIT/GEMSFIT.log";
		ofstream fout;
		fout.open(path, ios::app);						
		if( fout.fail() )
		{ cout<<"Output fileopen error"<<endl; exit(1); }

		fout << " ... performing parameter normalization ... " << endl;
		

		// Normalize init guess vector
		optv.resize( initguesses.size() );
		for(i=0; i<optv.size(); i++)
		{
			optv[i] = initguesses[i] / abs(initguesses[i]); 
		}

		// Normalize upper bounds vector
		for(i=0; i<OptUpBounds.size(); i++)
		{
			fout << "Init guess ["<<i<<"] = " << initguesses[i] << endl;
			fout << "Upper Bound old ["<<i<<"]= " << OptUpBounds[i] << endl;
			OptUpBounds[i] = OptUpBounds[i] / abs(initguesses[i]);
			fout << "Upper Bound new ["<<i<<"]= " << OptUpBounds[i] << endl;
		}
		
		// Normalize lower bounds vector
		for(i=0; i<OptLoBounds.size(); i++)
		{
			fout << "Init guess ["<<i<<"] = " << initguesses[i] << endl;
			fout << "Lower Bound old ["<<i<<"]= " << OptLoBounds[i] << endl;
			OptLoBounds[i] = OptLoBounds[i] / abs(initguesses[i]); 
			fout << "Lower Bound new ["<<i<<"]= " << OptLoBounds[i] << endl;
		}

		// Normalize constraints vector
		for(i=0; i<constraint_data_v.size(); i++)
		{
			constraint_data_v[i].Constraints = constraint_data_v[i].Constraints / abs(initguesses[i]); 
		}
		
		fout.close();
	}


	// NLopt return codes
	void ActivityModel::print_return_message( const int result )
	{
		switch (result)
		{
			case -5:
				cout<<" Halted because of a forced termination: the user called nlopt_force_stop(opt) on the optimization’s nlopt_opt object opt from the user’s objective function or constraints. "<<endl;
				break;
			case -4:
				cout<<" Halted because roundoff errors limited progress. (In this case, the optimization still typically returns a useful result.) "<<endl;
				break;
			case -3:
				cout<<" Ran out of memory. "<<endl;
				break;
			case -2:
				cout<<" Invalid arguments (e.g. lower bounds are bigger than upper bounds, an unknown algorithm was specified, etc.). "<<endl;
				break;
			case -1:
				cout<<" Generic failure code. "<<endl;
				break;
			case 1:
				cout<<" Generic success return value. "<<endl;
				break;
			case 2:
				cout<<" Optimization stopped because stopval was reached. "<<endl;
				break;
			case 3:
				cout<<" Optimization stopped because ftol_rel or ftol_abs was reached. "<<endl;
				break;
			case 4:
				cout<<" Optimization stopped because xtol_rel or xtol_abs was reached. "<<endl;
				break;
			case 5:
				cout<<" Optimization stopped because maxeval was reached. "<<endl;
				break;
			case 6:
				cout<<" Optimization stopped because maxtime was reached. "<<endl;
				break;
		}		
	}// end print_return_message	


    void ActivityModel::init_optim_multistart( std::vector<double> &optv_, std::vector<System_Properties*> *systems, int &countit, double &sum_of_squares )
    {
        unsigned int i;
		int j;
        std::vector<double> grad;

		// GEMSFIT logfile
		const char path[200] = "output_GEMSFIT/GEMSFIT.log";
		ofstream ffout;
		ffout.open(path, ios::app);						
		if( ffout.fail() )
		{ cout<<"Output fileopen error"<<endl; exit(1); }

		ffout << " ... entering multi-start optimization routines ... " << endl;

        // Reset counter to zero
        master_counter = 0;

        // Instantiate nlopt::opt object
        nlopt::opt opt_actmod( nlopt::GN_ISRES, optv_.size() );

 		// normalize parameters is OptNormParam Flag is set to yes (=1) and 
		// if the program is not in printing mode (printfile = 0)
		if( OptNormParam && !systems->at(0)->printfile )
		{
			normalize_params( optv_ );
			systems->at(0)->sysparam->NormParams = true;
		}
		else // do not normalize parameters
		{
			optv = optv_;
		}
		

        // check if initial guesses have same number of elements as bound and check if initial guesses are within the bounds
        if( optv.size() != OptUpBounds.size() )
        {
                cout<<endl;
                cout<<"initial guess vector and bounds vector must have same number of elements !! "<<endl;
                cout<<"optv.size() = "<<optv.size()<<" <-> OptUbBounds.size() = "<<OptUpBounds.size()<<endl;
                cout<<" .... exiting now .... "<<endl;
                cout<<endl;
                exit(1);
        }
        for( i=0; i<OptLoBounds.size(); i++ )
        {
                if( optv[i]<OptLoBounds[i] || optv[i]>OptUpBounds[i] )
                {
                        cout<<endl;
                        cout<<"Initial guess value is not within given bounds!!"<<endl;
                        cout<<"optv["<<i<<"] = "<<optv[i]<<" | OptLoBounds["<<i<<"] = "<<OptLoBounds[i]<<" | OptUpBounds["<<i<<"] = "<<OptUpBounds[i]<<endl;
                        cout<<" .... exiting now .... "<<endl;
                        cout<<endl;
                        exit(1);
                };
        };


        // assign bounds
        opt_actmod.set_lower_bounds( OptLoBounds );
        opt_actmod.set_upper_bounds( OptUpBounds );


        // specify relative tolerance tolerance on function value
        opt_actmod.set_xtol_rel( OptTolRel );

        // specify absolute tolerance on function value
        opt_actmod.set_xtol_abs( OptTolAbs );


        // maximum number of iterations
        opt_actmod.set_maxeval( OptMaxEval );


        // specify objective function
        if( !OptEquil )
        {
                // call wrapper for TSolMod class -> do not perform chemical equilibrium computation
                opt_actmod.set_min_objective( ActMod_objective_function_callback, systems );
        }
        else if( OptEquil==1 )
        {
                // call GEMS3K wrapper -> perform chemical equilibrium computation
                opt_actmod.set_min_objective( ActModEquil_objective_function_callback, systems );
        }


        if( OptConstraints )
        {
                // apply inequality constraints
                for( j=0; j< ((int) constraint_data_v.size()); j++ )
                        opt_actmod.add_inequality_constraint( ActMod_constraint_function_callback, &constraint_data_v[j], 1e-4);
        }

		if( OptInitStep > 0 )
		{
			vector<double> inistep( optv.size(), 0. );
			for( j=0; j<(int) optv.size(); j++ )
			{
				inistep[j] = optv[j] * OptInitStep;
			}
			opt_actmod.set_initial_step( inistep );
		}



#ifdef BOOST_MPI

		// Initialize boost::mpi environment
		bmpi::environment env(argc, argv);
  		bmpi::communicator world;

		n_procs_boost = world.size();
		proc_id_boost = world.rank();

		if( world.rank() == 0 )
		{
		    // generate vector of normaly distributed random numbers
		    typedef boost::mt19937 RNGType;
		    RNGType rng( time(0) );
		    boost::uniform_int<> rangeint( 1, 100 );
		    boost::variate_generator< RNGType, boost::uniform_int<> > randfactor(rng, rangeint);
		}

		n_runs = ( OptNMultistart / n_procs_boost + 1 ) * n_procs_boost; 

		// Create new initial vectors and perform parameter regression
		for( int l=0; l<n_runs; l++ )	
		{

			vector<double> init_opt_randfac_all( n_procs_boost * optv.size(), 0. );
			vector<double> init_opt_randfac(optv.size(), 0.);

			if( world.rank() == 0 )
			{
				for( int k=0; k<(int) init_opt_randfac_all.size(); k++ )
					init_opt_randfac_all[ k ] = randfactor() / 100.0;
			}

			bmpi::scatter( world, init_opt_all, init_opt_randfac, 0 );	

			for(int k=0; k<(int) OptLoBounds.size(); k++)
			{
				optv[k] = (OptUpBounds[k] - OptLoBounds[k]) * init_opt_randfac[k] / 100.0 + OptLoBounds[k]; 					
			}

			
			// Create directory where outputs of multistart runs are collected
			if( world.rank() == 0 )
			{
				if ( !boost::filesystem::exists( "output_RUNS" ) )
					boost::filesystem::create_directory("output_RUNS");

				// empty new directory if it was already existing
				boost::filesystem::path fi("output_RUNS");
				if(!boost::filesystem::exists(fi) || !boost::filesystem::is_directory(fi))
				{
					std::cout<<"output_RUNS could not be created or is not a directory. Exiting now ... "<<endl;
					exit(-1);
				}
				boost::filesystem::directory_iterator dir_iter(fi), dir_end;
				for(;dir_iter != dir_end; ++dir_iter)
				{
					boost::filesystem::remove(*dir_iter);
				}

			}

			// Copy directory output_GEMSFIT into output_RUNS directory inside a new folder
			ostringstream s, pb; 
 			s << l;
			pb << proc_id_boost;
			string new_directory("./output_RUNS");
			new_directory += "/run_" + s.str(); 
			string out_fit("./output_GEMSFIT")
			out_fit += "_" + pb.str();
			boost::filesystem3::copy_directory(boost::filesystem::path( out_fit ), boost::filesystem::path( new_directory.c_str() ));

			// empty output_GEMSFIT directory
			boost::filesystem::directory_iterator dir_iter_G(out_fit), dir_end_G;
			for(;dir_iter_G != dir_end_G; ++dir_iter_G)
			{
				boost::filesystem::remove(*dir_iter_G);
			}



            // RUN OPTIMIZATION 
            nlopt::result result = opt_actmod.optimize( optv, sum_of_squares );
            ffout<<"optv[0] = "<<optv[0]<<endl;
            ffout<<"size of optv = "<<optv.size()<<endl;


            // check results
            if( result < 0 )
            {
                    std::cout<<endl;
                    std::cout<<"   !!!  nlopt failed  !!!   "<<std::endl;
                    std::cout<<"   !!!  error code:   "<<result<<std::endl;
                    print_return_message( result );
                    std::cout<<endl;
            }
            else
            {
                    std::cout<<" NLopt return code: "<<result<<endl;
                    print_return_message( result );

					ofstream fout;
					ostringstream pb; 
					pb << proc_id_boost;
					string out_fit_plot("./output_GEMSFIT")
					out_fit_plot += "_" + pb.str() + "/myFIT.out";
					fout.open( out_fit_plot.c_str(), ios::app );

                    if( fout.fail() )
                    { cout<<"Output fileopen error"<<endl; exit(1); }
                    fout<<"pid "<<pid<<", found minimum at <<f( ";
                    for( unsigned i=0; i<optv.size(); i++ )
                    {
                            fout<<optv[i]<<" ";
                    }
                    fout<<") = "<<sum_of_squares<<std::endl;
                    fout<<" after "<< master_counter <<" evaluations."<<std::endl;
                    fout.close();

                    std::cout<<"found minimum at <<f( ";
                    for( unsigned i=0; i<optv.size(); i++ )
                    {
                            std::cout<<optv[i]<<" ";
                    }
                    std::cout<<") = "<<sum_of_squares<<std::endl;
            }
            std::cout<<" after "<< master_counter <<" evaluations"<<std::endl;
		}
#endif


#ifndef BOOST_MPI
		// Run several optimization runs with random initial guesses

	    typedef boost::mt19937 RNGType;
	    RNGType rng( time(0) );
	    boost::uniform_int<> rangeint( 1, 100 );
	    boost::variate_generator< RNGType, boost::uniform_int<> > randfactor(rng, rangeint);


		for( int i=0; i<OptNmultistart; i++ )
		{

				for(int k=0; k<(int) OptLoBounds.size(); k++)
				{
					optv[k] = (OptUpBounds[k] - OptLoBounds[k]) * randfactor() / 100.0 + OptLoBounds[k]; 					
				}

				// Create directory where outputs of multistart runs are collected
				if ( !boost::filesystem::exists( "output_RUNS" ) )
					boost::filesystem::create_directory("output_RUNS");
	
				// empty directory
				boost::filesystem::path fi("output_RUNS");
				if(!boost::filesystem::exists(fi) || !boost::filesystem::is_directory(fi))
				{
					std::cout<<"output_RUNS could not be created or is not a directory. Exiting now ... "<<endl;
					exit(-1);
				}
				boost::filesystem::directory_iterator dir_iter(fi), dir_end;
				for(;dir_iter != dir_end; ++dir_iter)
				{
					boost::filesystem::remove(*dir_iter);
				}

				// Copy directory output_GEMSFIT into output_RUNS directory inside a new folder
				ostringstream s; 
     			s << i;
				string new_directory("./output_RUNS");
				new_directory += "/run_" + s.str(); 
//                boost::filesystem::copy_directory(boost::filesystem::path("./output_GEMSFIT"), boost::filesystem::path( new_directory.c_str() ));

				// empty output_GEMSFIT directory
				boost::filesystem::path outFIT("output_GEMSFIT");
				boost::filesystem::directory_iterator dir_iter_G(outFIT), dir_end_G;
				for(;dir_iter_G != dir_end_G; ++dir_iter_G)
				{
					boost::filesystem::remove(*dir_iter_G);
				}


                // run optimization 
                nlopt::result result = opt_actmod.optimize( optv, sum_of_squares );
                ffout<<"optv[0] = "<<optv[0]<<endl;
                ffout<<"size of optv = "<<optv.size()<<endl;


                // check results
                if( result < 0 )
                {
                        std::cout<<endl;
                        std::cout<<"   !!!  nlopt failed  !!!   "<<std::endl;
                        std::cout<<"   !!!  error code:   "<<result<<std::endl;
                        print_return_message( result );
                        std::cout<<endl;
                }
                else
                {
                        std::cout<<" NLopt return code: "<<result<<endl;
                        print_return_message( result );
                        const char path[200] = "output_GEMSFIT/myFIT.out";
                        ofstream fout;
                        fout.open(path, ios::app);
                        if( fout.fail() )
                        { cout<<"Output fileopen error"<<endl; exit(1); }
                        fout<<"pid "<<pid<<", found minimum at <<f( ";
                        for( unsigned i=0; i<optv.size(); i++ )
                        {
                                fout<<optv[i]<<" ";
                        }
                        fout<<") = "<<sum_of_squares<<std::endl;
                        fout<<" after "<< master_counter <<" evaluations."<<std::endl;
                        fout.close();

                        std::cout<<"found minimum at <<f( ";
                        for( unsigned i=0; i<optv.size(); i++ )
                        {
                                std::cout<<optv[i]<<" ";
                        }
                        std::cout<<") = "<<sum_of_squares<<std::endl;
                }
                std::cout<<" after "<< master_counter <<" evaluations"<<std::endl;
		}
#endif
	ffout.close();
    }


	// Initialize optimization object and Run Optimization
	void ActivityModel::init_optim( std::vector<double> &optv_, std::vector<System_Properties*> *systems, int &countit, double &sum_of_squares )
	{

		// Instantiate nlopt::opt object
		if( OptAlgo.compare( "LN_COBYLA" ) == 0 )
		{	
			nlopt::opt opt_actmod( nlopt::LN_COBYLA, optv_.size() ); 
			build_optim( opt_actmod, optv_, systems, countit, sum_of_squares );
		}
		else if( OptAlgo.compare( "GN_ISRES" ) == 0 )
		{	
			nlopt::opt opt_actmod( nlopt::GN_ISRES, optv_.size() ); 
			build_optim( opt_actmod, optv_, systems, countit, sum_of_squares );
		}
		else if( OptAlgo.compare( "LN_BOBYQA" ) == 0 )
		{	
			nlopt::opt opt_actmod( nlopt::LN_BOBYQA, optv_.size() ); 
			build_optim( opt_actmod, optv_, systems, countit, sum_of_squares );
		}
		else if( OptAlgo.compare( "GN_ORIG_DIRECT" ) == 0 )
		{	
			nlopt::opt opt_actmod( nlopt::GN_ORIG_DIRECT, optv_.size() ); 
			build_optim( opt_actmod, optv_, systems, countit, sum_of_squares );
		}
		else if( OptAlgo.compare( "GN_ORIG_DIRECT_L" ) == 0 )
		{	
			nlopt::opt opt_actmod( nlopt::GN_ORIG_DIRECT_L, optv_.size() ); 
			build_optim( opt_actmod, optv_, systems, countit, sum_of_squares );
		}
		else
		{
			cout<<" Unknown optimization algorithm !!!! "<<endl;
			cout<<" ... bail out now ... "<<endl;
			exit(1); 
		}		


	}


	// Initialize optimization object and Run Optimization
	void ActivityModel::build_optim( nlopt::opt &opt_actmod, std::vector<double> &optv_, std::vector<System_Properties*> *systems, int &countit, double &sum_of_squares )
	{
		unsigned int i = 0;
		int j = 0;
		std::vector<double> grad;

		// GEMSFIT logfile
		const char path[200] = "output_GEMSFIT/GEMSFIT.log";
		ofstream ffout;
		ffout.open(path, ios::app);						
		if( ffout.fail() )
		{ cout<<"Output fileopen error"<<endl; exit(1); }

		ffout << " ... initializing optimization object ... " << endl;

		

		// Reset counter to zero
		master_counter = 0;

/*		// Do not normalize parameters
		if( systems->at(0)->printfile || !OptNormParam )
		{
			optv = optv_;
		}
		else // normalize parameters
		{
			normalize_params( optv_ );
			systems->at(0)->sysparam->NormParams = true;
		}*/
		
 		// normalize parameters is OptNormParam Flag is set to yes (=1) and 
		// if the program is not in printing mode (printfile = 0)
		if( OptNormParam && !systems->at(0)->printfile )
		{
			normalize_params( optv_ );
			systems->at(0)->sysparam->NormParams = true;
		}
		else // do not normalize parameters
		{
			optv = optv_;
		}


		// check if initial guesses have same number of elements as bound and check if initial guesses are within the bounds
		if( optv.size() != OptUpBounds.size() )
		{ 
			cout<<endl;
			cout<<"initial guess vector and bounds vector must have same number of elements !! "<<endl;
			cout<<"optv.size() = "<<optv.size()<<" <-> OptUbBounds.size() = "<<OptUpBounds.size()<<endl;
			cout<<" .... exiting now .... "<<endl;
		       	cout<<endl;	
			exit(1); 
		}		
		for( i=0; i<OptLoBounds.size(); i++ )
		{
			if( optv[i]<OptLoBounds[i] || optv[i]>OptUpBounds[i] )
			{
				cout<<endl;
				cout<<"Initial guess value is not within given bounds!!"<<endl;
				cout<<"optv["<<i<<"] = "<<optv[i]<<" | OptLoBounds["<<i<<"] = "<<OptLoBounds[i]<<" | OptUpBounds["<<i<<"] = "<<OptUpBounds[i]<<endl;
				cout<<" .... exiting now .... "<<endl;
			        cout<<endl;	
				exit(1);
			}
		}

		
		// assign bounds
		opt_actmod.set_lower_bounds( OptLoBounds );
		opt_actmod.set_upper_bounds( OptUpBounds );


		// specify relative tolerance tolerance on function value
		opt_actmod.set_xtol_rel( OptTolRel );
		
		// specify absolute tolerance on function value
		opt_actmod.set_xtol_abs( OptTolAbs );


		// maximum number of iterations
		opt_actmod.set_maxeval( OptMaxEval );


		// specify objective function
		if( !OptEquil )
		{
			// call wrapper for TSolMod class -> do not perform chemical equilibrium computation				
			opt_actmod.set_min_objective( ActMod_objective_function_callback, systems );
		}
		else if( OptEquil==1 )
		{
			// call GEMS3K wrapper -> perform chemical equilibrium computation
			opt_actmod.set_min_objective( ActModEquil_objective_function_callback, systems );
		}		


		if( OptConstraints )
		{	
			// apply inequality constraints
			for( int j=0; j< ((int) constraint_data_v.size()); j++ ) 
				opt_actmod.add_inequality_constraint( ActMod_constraint_function_callback, &constraint_data_v[j], 1e-4);
		}


		// Set initial stepsize
		if( OptInitStep > 0 )
		{
			vector<double> inistep( optv.size(), 0. );
			for( j=0; j<(int) optv.size(); j++ )
			{
				inistep[j] = optv[j] * OptInitStep;
			}
			opt_actmod.set_initial_step( inistep );
		}



		// Only the Master process runs the optimization library. The other processes only run the callback function (copying the newly generated guess values from process 0).	
		int ierr;
		//ierr = MPI_Comm_rank( MPI_COMM_WORLD, &pid );
		//ierr = MPI_Comm_size( MPI_COMM_WORLD, &p );
		int continue_or_exit;
		
		// Perform optimization -> perform MPI call only when NOT in printing mode (function: ActivityModel::print_results()) AND NOT in Monte Carlo mode (function: Statistics::MC_confidence_interval())				
		if( !systems->at(0)->printfile && !systems->at(0)->MC_MPI )
		{
				// MASTER
				if( !pid )
				{
                    // run optimization -> perform MPI_Send() in global activity model callback
					nlopt::result result = opt_actmod.optimize( optv, sum_of_squares );
					ffout<<"optv[0] = "<<optv[0]<<endl;
					ffout<<"size of optv = "<<optv.size()<<endl;
			

				// check results
					if( result < 0 ) 
					{
						std::cout<<endl;
						std::cout<<"   !!!  nlopt failed  !!!   "<<std::endl;
						std::cout<<"   !!!  error code:   "<<result<<std::endl;
						print_return_message( result );
						std::cout<<endl;
					}
					else 
					{
						std::cout<<" NLopt return code: "<<result<<endl;
						print_return_message( result );
						const char path[200] = "output_GEMSFIT/myFIT.out";
						ofstream fout;
						fout.open(path, ios::app);						
						if( fout.fail() )
						{ cout<<"Output fileopen error"<<endl; exit(1); }
						fout<<"pid "<<pid<<", found minimum at <<f( "; 			
						for( unsigned i=0; i<optv.size(); i++ )
						{	
							fout<<optv[i]<<" "; 
						}
						fout<<") = "<<sum_of_squares<<std::endl;
						fout<<" after "<< master_counter <<" evaluations."<<std::endl;
						fout.close();

						std::cout<<"found minimum at <<f( "; 
						for( unsigned i=0; i<optv.size(); i++ )
						{	
							std::cout<<optv[i]<<" "; 
						}
						std::cout<<") = "<<sum_of_squares<<std::endl;
					}
					std::cout<<" after "<< master_counter <<" evaluations"<<std::endl;


					// run hybrid optimization mode with gradient-based optimization
					if( OptHybridMode )
					{

						// Instantiate nlopt::opt object
						if( OptHybridAlgo.compare( "LN_COBYLA" ) == 0 )
						{	
							nlopt::opt opt_hybrid_actmod( nlopt::LN_COBYLA, optv_.size() ); 
							build_hybrid_optim( opt_hybrid_actmod, optv, systems, countit, sum_of_squares );
						}
						else if( OptHybridAlgo.compare( "GN_ISRES" ) == 0 )
						{	
							nlopt::opt opt_hybrid_actmod( nlopt::GN_ISRES, optv_.size() ); 
							build_hybrid_optim( opt_hybrid_actmod, optv, systems, countit, sum_of_squares );
						}
						else if( OptHybridAlgo.compare( "LN_BOBYQA" ) == 0 )
						{	
							nlopt::opt opt_hybrid_actmod( nlopt::LN_BOBYQA, optv_.size() ); 
							build_hybrid_optim( opt_hybrid_actmod, optv, systems, countit, sum_of_squares );
						}
						else if( OptHybridAlgo.compare( "GN_ORIG_DIRECT" ) == 0 )
						{	
							nlopt::opt opt_hybrid_actmod( nlopt::GN_ORIG_DIRECT, optv_.size() ); 
							build_hybrid_optim( opt_hybrid_actmod, optv, systems, countit, sum_of_squares );
						}
						else if( OptHybridAlgo.compare( "GN_ORIG_DIRECT_L" ) == 0 )
						{	
							nlopt::opt opt_hybrid_actmod( nlopt::GN_ORIG_DIRECT_L, optv_.size() ); 
							build_hybrid_optim( opt_hybrid_actmod, optv, systems, countit, sum_of_squares );
						}
						else
						{
							cout<<" Unknown optimization algorithm !!!! "<<endl;
							cout<<" ... bail out now ... "<<endl;
							exit(1); 
						}		


					}

#ifdef USE_MPI
					// send stopping signal to all other threads when optimization finished
					int pid_;
					for( pid_=1; pid_<p; pid_++ )
					{
						continue_or_exit = 0;
						MPI_Send( &continue_or_exit, 1, MPI_INT, pid_, 0,  MPI_COMM_WORLD );  
					}
#endif				
			
					// copy resulting vector back to incoming optv vector (needed for printing results)
					optv_ = optv;
				}
				else // SLAVES
				{	
#ifdef USE_MPI
					MPI_Status status;
					int sender;
					int cont_or_exit = 0;

					MPI_Recv( &cont_or_exit, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status );
					while( cont_or_exit != 0 )
					{
						ActMod_objective_function_callback( optv, grad, systems );
						MPI_Recv( &cont_or_exit, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status );
						sender = status.MPI_SOURCE;				
						if( cont_or_exit != 1 ) break;
					}
#endif
				}

#ifdef USE_MPI	
				// Send the values generated by the master process to all other threads
				ierr = MPI_Bcast( &sum_of_squares, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD );
#endif		
				ffout<<"pid "<<pid<<" is out of loop ..."<<endl;

				// copy resulting vector back to incoming optv vector (needed for printing results)
				if( !OptNormParam )
				{
					optv_ = optv;
				}
				else
				{
					for( i=0; i<optv.size(); i++ )
					{
						optv_[i] = optv[i] * abs(systems->at(0)->sysparam->init_guesses[i]);
					}
				}
		} 
		else // DO NOT PARALLELIZE LOOP OVER MEASUREMENTS IN OBJECTIVE FUNCTION if in printing mode (function: ActivityModel::print_results()) OR in Monte Carlo mode (function: Statistics::MC_confidence_interval())
		{
				nlopt::result result = opt_actmod.optimize( optv, sum_of_squares );
				ffout<<"optv[0] = "<<optv[0]<<endl;
				ffout<<"size of optv = "<<optv.size()<<endl;
			
				// check results
				if( result < 0 ) 
				{
					std::cout<<endl;
					std::cout<<"   !!!  nlopt failed !!! "<<std::endl;
					std::cout<<"   !!!  error code: "<<result<<std::endl;
					print_return_message( result );
					std::cout<<endl;
				}
				else 
				{
					std::cout<<" NLopt return code: "<<result<<endl;
					print_return_message( result );
					const char path[200] = "output_GEMSFIT/myFIT.out";
					ofstream fout;
					fout.open(path, ios::app);						
					if( fout.fail() )
					{ cout<<"Output fileopen error"<<endl; exit(1); }
					fout<<"pid "<<pid<<", found minimum at <<f( "; 			
					for( unsigned i=0; i<optv.size(); i++ )
					{	
						fout<<optv[i]<<" "; 
					}
					fout<<") = "<<sum_of_squares<<std::endl;
					fout<<" after "<< master_counter <<" evaluations."<<std::endl;
					fout.close();

					std::cout<<"found minimum at <<f( "; 
					for( unsigned i=0; i<optv.size(); i++ )
					{	
						std::cout<<optv[i]<<" "; 
					}
					std::cout<<") = "<<sum_of_squares<<std::endl;
				}
				std::cout<<" after "<< master_counter <<" evaluations"<<std::endl;

				// copy resulting vector back to incoming optv vector (needed for printing results)
				optv_ = optv;				


		}
		//opt_actmod.destroy();
	
	ffout.close();
	countit = master_counter;
	}


	void ActivityModel::build_hybrid_optim( nlopt::opt &opt_hybrid_actmod, std::vector<double> &optv_, std::vector<System_Properties*> *systems, int &countit, double &sum_of_squares )
	{
		unsigned int i = 0;
		int j = 0;
		std::vector<double> grad;

		// GEMSFIT logfile
		const char path[200] = "output_GEMSFIT/GEMSFIT.log";
		ofstream ffout;
		ffout.open(path, ios::app);						
		if( ffout.fail() )
		{ cout<<"Output fileopen error"<<endl; exit(1); }

		ffout << " ... initializing hybrid optimization procedure ... " << endl;


		// Reset counter to zero
		master_counter = 0;

		optv = optv_;
		

		// check if initial guesses have same number of elements as bound and check if initial guesses are within the bounds
		if( optv.size() != OptUpBounds.size() )
		{ 
			cout<<endl;
			cout<<"initial guess vector and bounds vector must have same number of elements !! "<<endl;
			cout<<"optv.size() = "<<optv.size()<<" <-> OptUbBounds.size() = "<<OptUpBounds.size()<<endl;
			cout<<" .... exiting now .... "<<endl;
		       	cout<<endl;	
			exit(1); 
		}		
		for( i=0; i<OptLoBounds.size(); i++ )
		{
			if( optv[i]<OptLoBounds[i] || optv[i]>OptUpBounds[i] )
			{
				cout<<endl;
				cout<<"Initial guess value is not within given bounds!!"<<endl;
				cout<<"optv["<<i<<"] = "<<optv[i]<<" | OptLoBounds["<<i<<"] = "<<OptLoBounds[i]<<" | OptUpBounds["<<i<<"] = "<<OptUpBounds[i]<<endl;
				cout<<" .... exiting now .... "<<endl;
			        cout<<endl;	
				exit(1);
			}
		}

		
		// assign bounds
		opt_hybrid_actmod.set_lower_bounds( OptLoBounds );
		opt_hybrid_actmod.set_upper_bounds( OptUpBounds );


		// specify relative tolerance tolerance on function value
		opt_hybrid_actmod.set_xtol_rel( OptHybridTolRel );
		
		// specify absolute tolerance on function value
		opt_hybrid_actmod.set_xtol_abs( OptHybridTolAbs );


		// maximum number of iterations
		opt_hybrid_actmod.set_maxeval( OptHybridMaxEval );


		// specify objective function
		if( !OptEquil )
		{
			// call wrapper for TSolMod class -> do not perform chemical equilibrium computation				
			opt_hybrid_actmod.set_min_objective( ActMod_objective_function_callback, systems );
		}
		else if( OptEquil==1 )
		{
			// call GEMS3K wrapper -> perform chemical equilibrium computation
			opt_hybrid_actmod.set_min_objective( ActModEquil_objective_function_callback, systems );
		}		


		if( OptConstraints )
		{	
			// apply inequality constraints
			for( j=0; j< ((int) constraint_data_v.size()); j++ ) 
				opt_hybrid_actmod.add_inequality_constraint( ActMod_constraint_function_callback, &constraint_data_v[j], 1e-4);
		}


		// Set initial stepsize
		if( OptInitStep > 0 )
		{
			vector<double> inistep( optv.size(), 0. );
			for( j=0; j<(int) optv.size(); j++ )
			{
				inistep[j] = optv[j] * OptInitStep;
			}
			opt_hybrid_actmod.set_initial_step( inistep );
		}


		// run optimization 
		nlopt::result result = opt_hybrid_actmod.optimize( optv, sum_of_squares );
		ffout<<"optv[0] = "<<optv[0]<<endl;
		ffout<<"size of optv = "<<optv.size()<<endl;
			

		// check results
		if( result < 0 ) 
		{
			std::cout<<endl;
			std::cout<<"   !!!  nlopt failed  !!!   "<<std::endl;
			std::cout<<"   !!!  error code:   "<<result<<std::endl;
			print_return_message( result );
			std::cout<<endl;
		}
		else 
		{
			std::cout<<" NLopt return code: "<<result<<endl;
			print_return_message( result );
			const char path[200] = "output_GEMSFIT/myFIT.out";
			ofstream fout;
			fout.open(path, ios::app);						
			if( fout.fail() )
			{ cout<<"Output fileopen error"<<endl; exit(1); }
			fout<<"pid "<<pid<<", found minimum at <<f( "; 			
			for( unsigned i=0; i<optv.size(); i++ )
			{	
				fout<<optv[i]<<" "; 
			}
			fout<<") = "<<sum_of_squares<<std::endl;
			fout<<" after "<< master_counter <<" evaluations."<<std::endl;
			fout.close();

			std::cout<<"found minimum at <<f( "; 
			for( unsigned i=0; i<optv.size(); i++ )
			{	
				std::cout<<optv[i]<<" "; 
			}
			std::cout<<") = "<<sum_of_squares<<std::endl;
		}
		std::cout<<" after "<< master_counter <<" evaluations"<<std::endl;
			
		// copy resulting vector back to incoming optv vector (needed for printing results)
		optv_ = optv;;

	countit = master_counter;
	ffout.close();
	}
	

	// Print part of the fit results
	void ActivityModel::print_results( std::vector<double> &optv_, std::vector<System_Properties*> *systems )
	{

		int i, j, code;
		double residual_sys;
		double temp, pres, mola;		
    		std::vector<double> p_row;
		p_row.resize(4);
		std::vector< std::vector<double> > plot_v;
		int len = 0;
		std::vector< std::vector<double> > plot_array;
		double m, mnew = 0.; 
		int ind = 0;
		typedef std::vector<int> IntContainer;
        	typedef IntContainer::iterator IntIterator;
	
		len = (int) systems->at(0)->computed_values_v.size();

		if( !pid )
		{
			// Set printfile bool to true in order to deactivate to parallelization of the loop over measurements within the ActMod_tsolmod_wrap() function
			systems->at(0)->printfile = true;
			residual_sys = 0.;	
			ActMod_tsolmod_wrap( residual_sys, optv_, systems->at(0) );
		}	

		// In case later on parallelization of measurement over for loop is wished, set printfile bool to false
		systems->at(0)->printfile = false;
		
		bool stop;
		// check for values that want to be printed
		for( i=0; i<len; i++ )
		{
			temp = systems->at(0)->sysdata->temperature[i];
			pres = systems->at(0)->sysdata->pressure[i];
			mola = systems->at(0)->sysdata->molality_1[i];
			code = systems->at(0)->sysdata->code[i];
//cout<<"i = "<<i<<", mola = "<<mola<<", pres = "<<pres<<", temp = "<<temp<<endl;
			stop = false;
			j = 0;
			while( j<(int)  Plot_ActMod->plotting_info->print_temperatures.size() && stop == false )
			{
//cout<<" in print loop ... "<<endl;
				if( temp == Plot_ActMod->plotting_info->print_temperatures[j] && pres == Plot_ActMod->plotting_info->print_pressures[j] && mola >= Plot_ActMod->plotting_info->print_molalities[2*j] && mola <=  Plot_ActMod->plotting_info->print_molalities[2*j+1] && code == Plot_ActMod->plotting_info->print_code )
				{
					// add columns to each row	
 				    p_row[0] = mola; 
					p_row[1] = systems->at(0)->computed_values_v[i];
					p_row[2] = systems->at(0)->measured_values_v[i];
					p_row[3] = systems->at(0)->sysdata->error[i];
					stop = true;		
				}	
				j++;						
			};	
			if( stop )
			{	
				plot_v.push_back(p_row);
			}				
		}
		

//for( i=0; i< (int) plot_v.size(); i++ )		
//	cout<<"before sort: plot_v["<<i<<"][0] = "<<plot_v[i][0]<<", plot_v["<<i<<"][2] = "<<plot_v[i][2]<<endl;


		// Sort the plotting array
		IntContainer intv;

		// loop generating sorted vector
		for( i=0; i< (int) plot_v.size(); i++ )
		{
			m = 1000.0;
			// loop over unsorted vector
			for( j=0; j< (int) plot_v.size(); j++ )
			{
					if( i == 0 && plot_v[j][0] < m )
					{
						m = plot_v[j][0];
						ind = j;
					}else
					{
				        IntIterator i = find(intv.begin(), intv.end(), j);
						if( i != intv.end() )
						{ /* do nothing */ }
						else
						{
							if( plot_v[j][0] < m )
							{
								m = plot_v[j][0];
								ind = j;
							}
						}
					}
			}
			intv.push_back( ind );
			mnew = plot_v[ind][0];

			p_row[0] = plot_v[ind][0];
			p_row[1] = plot_v[ind][1];
			p_row[2] = plot_v[ind][2];
			p_row[3] = plot_v[ind][3];
			plot_array.push_back(p_row);				
		}

		// Call printing function: plot computed versus measured values
		if( !pid )
		{	
			Plot_ActMod->print_vectors_curve( 0, systems, plot_array ); 
		}
		cout<<"pid "<<pid<<" leaving ActivityModel::print_results ..."<<endl;

#ifdef USE_MPI
		MPI_Barrier(MPI_COMM_WORLD);
#endif

	}


}// namespace opti







