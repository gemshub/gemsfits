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
 *	@file gemsfit_global_functions.cpp
 *
 *	@brief this source file contains implementations of global functions needed for  optimization.
 *	Since the nlopt library needs a function pointer to the objective and constraint
 *  functions, these functions need to be globally accessible and can not be implemented
 *  as member functions. This source file contains mainly these implementations of the
 *  respective callback functions.
 *
 *	@author G. Dan Miron
 *
 * 	@date 08.05.2013
 *
 */


#include "gemsfit_global_functions.h"
#include "gemsfit_target_functions.h"
#include "gemsfit_nested_functions.h"
#include "gemsfit_task.h"
#include <iomanip>

double Equil_objective_function_callback( const std::vector<double> &opt, std::vector<double> &grad, void *obj_func_data )
{
    TGfitTask *sys = reinterpret_cast<TGfitTask*>(obj_func_data);

    unsigned int i;
    double sum_of_squared_residuals_allsys = 0.0;
    double sum_of_squared_residuals_sys = 0.0;


    // Rescale optimization to unconvert normalization of parameters
    if( sys->NormParams )
    {

        /// GRadient calculation
        if (sys->Opti->OptAlgo.compare(1,1,"D") == 0)
        {
            gradient(opt, grad, sys);
        }

        vector<double> optV( opt.size() );
        for( i=0; i<opt.size(); i++ )
        {
            if (opt[i] != 0)
            optV[i] = opt[i] * fabs(sys->Opti->optv_0[i]);
        }
        sys->h_grad = false;

        // call tsolmod wrapper
        if (sys->Opti->OptEquilibrium)
        gems3k_wrap( sum_of_squared_residuals_sys, optV, sys );
        else tsolmod_wrap( sum_of_squared_residuals_sys, optV, sys );
        sum_of_squared_residuals_allsys = sum_of_squared_residuals_allsys + sum_of_squared_residuals_sys;
    }
    else
    { // if parameters were not normalized
        /// GRadient calculation
        if (sys->Opti->OptAlgo.compare(1,1,"D") == 0)
        {
            gradient(opt, grad, sys);
        }
        sys->h_grad = false;

        // call tsolmod wrapper
        if (sys->Opti->OptEquilibrium)
        gems3k_wrap( sum_of_squared_residuals_sys, opt, sys );
        else tsolmod_wrap( sum_of_squared_residuals_sys, opt, sys );
        sum_of_squared_residuals_allsys = sum_of_squared_residuals_allsys + sum_of_squared_residuals_sys;
    }

return sum_of_squared_residuals_allsys;
}


// Wrapper around GEMS3K solver
void gems3k_wrap( double &residuals_sys, const std::vector<double> &opt, TGfitTask* sys )
{
    // Call GEMS3K and run GEM_run();
    double test_residual = 0.0;
    int count;

//    cout << "in global" << endl;

    // Temporary storage vectors
    master_counter++;

    if (sys->Opti->OptTuckey == 5)
    {
        set_Tuckey_weight_global(sys);
    } else
        if (sys->Opti->OptTuckey == 6)
    {
        set_Tuckey_weight_objfun(sys);
    }

    // Clear already stored results
    sys->computed_values_v.clear();
    sys->measured_values_v.clear();
    sys->Weighted_Tfun_residuals_v.clear();
    sys->Tfun_residuals_v.clear();
    sys->residuals_v.clear();
    sys->weights.clear();

//    sys->print->print_clear();

//adjust parameters
//    cout << " befores set params " << endl;
#ifdef useomp
    omp_set_num_threads(sys->MPI);
    #pragma omp parallel for schedule(static) /*schedule(dynamic)*/
#endif
    for (unsigned i=0; i<sys->NodT.size(); ++i)
    {
        for (unsigned e=0; e < sys->Opti->optParam.size(); e++)
        {
            sys->Opti->optParam[e]->Adjust_param(sys->NodT[i], opt);
        }
//        cout<<"adjusting parameters for: "<< i+1 <<" sample "<< sys->experiments[i]->sample << endl;
    }

    /// NESTED FUNCTION
    if (sys->Opti->h_optNF)
    {
        string old = sys->Tfun->type;               // storing the old type of target function
        sys->Tfun->type = "abs_dif";                // seeting the target function to simple abslute difference
        for (int i = 0; i < sys->NodT.size(); i++)
        {
            sys->aTfun[i].type = "abs_dif";
        }
        nestedfun(sys);                             // optimizing the nested functions
        sys->Tfun->type = old;
        for (int i = 0; i < sys->NodT.size(); i++)
        {
            sys->aTfun[i].type = old;
        }
    }

#ifdef useomp
    omp_set_num_threads(sys->MPI);
#ifdef buildWIN32
    #pragma omp parallel for schedule(static)
#else
    #pragma omp parallel for schedule(dynamic)
#endif
#endif
    // +++ Calculating equilibrium with GEMS3K +++ //
    for (unsigned int i=0; i<sys->NodT.size(); ++i)
    {
        vector<DATABR*> dBR;
        dBR.push_back(sys->NodT[i]->pCNode());
        long int NodeStatusCH;

            // Set the P in the node->CNode-P as in the experiments to avoid problem due to Psat notation as 0
        for (unsigned int e=0; e<sys->experiments.size(); ++e)
        {
            sys->NodT[e]->Set_TK(273.15 + sys->experiments[e]->sT);
            sys->NodT[e]->Set_P(100000 * sys->experiments[e]->sP);
        }

        // Asking GEM to run with automatic initial approximation
        dBR.at(0)->NodeStatusCH = NEED_GEM_AIA;

        // Asking GEM to run with smart initial approximation
        if (sys->Opti->OptGemsSIA == 1)
            dBR.at(0)->NodeStatusCH = NEED_GEM_SIA;

//         cout<<"Before run node: "<< i+1 <<" sample "<< sys->experiments[i]->sample << endl;

        // RUN GEMS3K
        NodeStatusCH = sys->NodT[i]->GEM_run( false );

        if( NodeStatusCH == OK_GEM_AIA || NodeStatusCH == OK_GEM_SIA  )
        {
//            sys->NodT[i]->GEM_print_ipm( "GEMS3K_log.out" );   // possible debugging printout
        }
        else
        {
            // possible return status analysis, error message
//            sys->NodT[i]->GEM_print_ipm( "GEMS3K_log.out" );   // possible debugging printout
            cout<<"For node: "<< i+1 <<" sample "<< sys->experiments[i]->sample << endl;
            cout<<" GEMS3K did not converge properly !!!! continuing anyway ... "<<endl;
        }


        for (unsigned j = 0; j< sys->Tfun->objfun.size(); ++j)
        {
            // Set the P in the node->CNode-P as in the experiments to avoid problem due to Psat notation as 0
            string compare_type = sys->aTfun[i].objfun[j].exp_DCP;
            if (compare_type == "activity")
            {
                 sys->NodT[i]->Set_TK(273.15 + sys->experiments[i]->sT);
                 sys->NodT[i]->Set_P(100000 * sys->experiments[i]->sP);
            }

            test_residual += sys->get_residual(i, sys->aTfun[i].objfun[j], count);
        }

        for (unsigned j = 0; j< sys->Tfun->addout.size(); ++j)
        {
            // Set the P in the node->CNode-P as in the experiments to avoid problem due to Psat notation as 0
            string compare_type = sys->aTfun[i].addout[j].exp_DCP;
            if (compare_type == "activity")
            {
                 sys->NodT[i]->Set_TK(273.15 + sys->experiments[i]->sT);
                 sys->NodT[i]->Set_P(100000 * sys->experiments[i]->sP);
            }
        }
    }

    if (sys->Opti->OptTuckey == 1)
    {
//        residuals_sys = sys->get_sum_of_residuals( );
        set_Tuckey_weight_objfun(sys);
    } else
    if (sys->Opti->OptTuckey == 2)
    {
//        residuals_sys = sys->get_sum_of_residuals( );
        set_Tuckey_weight_objfun_norm(sys);
    } else
    if (sys->Opti->OptTuckey == 3)
    {
//        residuals_sys = sys->get_sum_of_residuals( );
        set_Tuckey_weight_objfun_norm2(sys);
    } else
    if (sys->Opti->OptTuckey == 4)
    {
//        residuals_sys = sys->get_sum_of_residuals( );
        set_Tuckey_weight_global(sys);
    } else
    if (sys->Opti->OptTuckey == 5)
    {
//        residuals_sys = sys->get_sum_of_residuals( );
        set_Tuckey_weight_global(sys);
    }


    residuals_sys = sys->get_sum_of_residuals( );
    double Weighted_Abs_sum_of_residuals = 0.0;

    for (unsigned j = 0; j < sys->Tfun->objfun.size(); j++)
    {
        for (unsigned int i = 0; i < sys->aTfun.size(); i++)
        {
            if (sys->aTfun[i].objfun[j].isComputed)
            {
                sys->computed_values_v.push_back(sys->aTfun[i].objfun[j].results.computed_value );
                sys->measured_values_v.push_back(sys->aTfun[i].objfun[j].results.measured_value );
                sys->residuals_v.push_back(sys->aTfun[i].objfun[j].results.residual );
                sys->weights.push_back(sys->aTfun[i].objfun[j].results.weight );
                sys->Tfun_residuals_v.push_back(sys->aTfun[i].objfun[j].results.Tfun_residual );
                sys->Weighted_Tfun_residuals_v.push_back(sys->aTfun[i].objfun[j].results.WTfun_residual );
                Weighted_Abs_sum_of_residuals = fabs(sys->aTfun[i].objfun[j].results.residual)*sys->aTfun[i].objfun[j].results.weight;
            }
        }
    }

    if (master_counter == 1)
    {
        sys->_init_residuals_sys = residuals_sys;
        sys->_init_Weighted_Abs_sum_of_residuals = Weighted_Abs_sum_of_residuals;
    }

    if (sys->Opti->OptMixedSumOfResiduals)
    {
        double norm_residuals_sys = residuals_sys/sys->_init_residuals_sys;
        double norm_Weighted_Abs_sum_of_residuals = Weighted_Abs_sum_of_residuals/sys->_init_Weighted_Abs_sum_of_residuals;
        residuals_sys = norm_residuals_sys+norm_Weighted_Abs_sum_of_residuals;
    }

//    residuals_sys = test_residual;

    // debug for when using global algorithm
    if (master_counter%1000 == 0)
    {
        cout << master_counter << " iterations, continuing..." << endl;
        cout << "sum of residuals: "<<residuals_sys<< endl;
        for (unsigned int i = 0; i<opt.size(); ++i)
        {
            cout<<"parameter "<<i<<" : "<<opt[i]<<endl;
        }
    }
    gpf->flog << "~ m.count.= " << master_counter << " sum.res.= " << setprecision(15) << residuals_sys << endl;
    cout << "~ m.count.= " << master_counter << " sum.res.= " << setprecision(15) << residuals_sys << endl;

}


void gradient( vector<double> optn, vector<double> &grad, TGfitTask *sys )
{
    double residual_sys;

    double computed_up, param_up;
    double computed_lo, param_lo;
    std::vector<double> opt_scan, opt;

    sys->h_grad = true;

    for(unsigned i=0; i<optn.size(); i++ )
    {
        opt.push_back(optn[i] * fabs(sys->Opti->optv_0[i]));
    }

    grad.clear();
    grad.resize(opt.size());

    opt_scan.resize( opt.size() );

    double delta = sys->Opti->OptPerturbator;
    for(int i=0; i< (int) opt.size(); i++ )
    {
        opt_scan = opt;

        // Central finite differences:
        opt_scan[i] = opt[i] + opt[i]*delta;
        param_up = optn[i] + optn[i]*delta;
        residual_sys = 0.;
        gems3k_wrap( residual_sys, opt_scan, sys );
//cout<<"residual_sys = "<<residual_sys<<endl;
        computed_up = residual_sys;

        opt_scan[i] = opt[i] - opt[i]*delta;
        param_lo = optn[i] - optn[i]*delta;
        residual_sys = 0.;
        gems3k_wrap( residual_sys, opt_scan, sys );
//cout<<"residual_sys = "<<residual_sys<<endl;
        computed_lo = residual_sys;

        grad[i] = (computed_up - computed_lo) / (param_up - param_lo);
    }
}

void tsolmod_wrap( double &residual, const std::vector<double> &opt, TGfitTask *sys )
{

    // Clear already stored results
    sys->computed_values_v.clear();
    sys->measured_values_v.clear();
    sys->Weighted_Tfun_residuals_v.clear();
    sys->Tfun_residuals_v.clear();
    sys->residuals_v.clear();
    sys->weights.clear();

    for (unsigned i=0; i<sys->NodT.size(); ++i)
    {
        for (unsigned e=0; e < sys->Opti->optParam.size(); e++)
        {
            sys->Opti->optParam[e]->Adjust_param(sys->NodT[i], opt);
        }
    }

    // Loop trough all nodes for calculationg the pahse properties
    for (unsigned int i=0; i<sys->NodT.size(); ++i)
    {

        TMulti *multi = sys->NodT[i]->pMulti();

        unsigned int sizeFIs = multi->get_sizeFIs();

        for (unsigned j=0; j<sizeFIs; j++)
        {
        TSolMod *sol = multi->pTSolMod(j);
        multi->Access_GEM_IMP_init();
        sol->PTparam();
        sol->MixMod();
        }
    }

    if (sys->Opti->OptTuckey == 1)
    {
        residual = sys->get_sum_of_residuals( );
        set_Tuckey_weight_global(sys);
    } else
        if (sys->Opti->OptTuckey == 2)
    {
        residual = sys->get_sum_of_residuals( );
        set_Tuckey_weight_objfun(sys);
    }
    residual = sys->get_sum_of_residuals( );

    gpf->flog << "~ m.count.= " << master_counter << " sum.res.= " << residual << endl;
    cout << "~ m.count.= " << master_counter << " sum.res.= " << residual << endl;

    for (unsigned j = 0; j < sys->Tfun->objfun.size(); j++)
    {
        for (unsigned int i = 0; i < sys->aTfun.size(); i++)
        {
            if (sys->aTfun[i].objfun[j].isComputed)
            {
                sys->computed_values_v.push_back(sys->aTfun[i].objfun[j].results.computed_value );
                sys->measured_values_v.push_back(sys->aTfun[i].objfun[j].results.measured_value );
                sys->residuals_v.push_back(sys->aTfun[i].objfun[j].results.residual );
                sys->weights.push_back(sys->aTfun[i].objfun[j].results.weight );
                sys->Tfun_residuals_v.push_back(sys->aTfun[i].objfun[j].results.Tfun_residual );
                sys->Weighted_Tfun_residuals_v.push_back(sys->aTfun[i].objfun[j].results.WTfun_residual );
            }
        }
    }
}


double median(vector<double> absresiduals)
{
  double median;
  size_t size = absresiduals.size();

  sort(absresiduals.begin(), absresiduals.end());

  if (size  % 2 == 0)
  {
      median = (absresiduals[size / 2 - 1] + absresiduals[size / 2]) / 2;
  }
  else
  {
      median = absresiduals[size / 2];
  }

  return median;
}

void set_Tuckey_weight_global (TGfitTask *sys)
{
    double C = 0.0;
    double median_ = 0.0;
    vector <double> abs_res;

    for (unsigned int i = 0; i < sys->aTfun.size(); i++)
    {
        for (unsigned int j = 0; j < sys->aTfun[i].objfun.size(); j++)
        {
            if (sys->aTfun[i].objfun[j].isComputed)
            {
                abs_res.push_back(fabs(sys->aTfun[i].objfun[j].results.residual));
            }

        }
    }

    median_ = median(abs_res);
    C = sys->Opti->OptTuckeyVal * median_;

//#ifdef useomp
//    omp_set_num_threads(sys->MPI);
//    #pragma omp parallel for
//#endif
    for (unsigned int i = 0; i < sys->aTfun.size(); i++)
    {
        for (unsigned int j = 0; j < sys->aTfun[i].objfun.size(); j++)
        {
            if (sys->aTfun[i].objfun[j].isComputed)
            {
                if (( C - abs_res[i]) < 0)
                    sys->aTfun[i].objfun[j].TuWeight = 0;
                else
                    sys->aTfun[i].objfun[j].TuWeight = pow((1 - pow((abs_res[i]/C), 2) ), 2);
            }
        }
    }
}

void set_Tuckey_weight_global_norm (TGfitTask *sys)
{
    double C = 0.0;
    double median_ = 0.0;
    vector <double> abs_res;

    for (unsigned int i = 0; i < sys->aTfun.size(); i++)
    {
        for (unsigned int j = 0; j < sys->aTfun[i].objfun.size(); j++)
        {
            if (sys->aTfun[i].objfun[j].isComputed)
            {
                abs_res.push_back(fabs(sys->aTfun[i].objfun[j].results.residual/sys->aTfun[i].objfun[j].results.computed_value));
            }

        }
    }

    median_ = median(abs_res);
    C = sys->Opti->OptTuckeyVal * median_;

//#ifdef useomp
//    omp_set_num_threads(sys->MPI);
//    #pragma omp parallel for
//#endif
    for (unsigned int i = 0; i < sys->aTfun.size(); i++)
    {
        for (unsigned int j = 0; j < sys->aTfun[i].objfun.size(); j++)
        {
            if (sys->aTfun[i].objfun[j].isComputed)
            {
                if (( C - abs_res[i]) < 0)
                    sys->aTfun[i].objfun[j].TuWeight = 0;
                else
                    sys->aTfun[i].objfun[j].TuWeight = pow((1 - pow((abs_res[i]/C), 2) ), 2);
            }
        }
    }
}

void set_Tuckey_weight_objfun (TGfitTask *sys)
{
    vector<double> C;
    vector<double> median_;
    vector<vector<double> > vabs_res;
    vector<double> abs_res;

    for (unsigned int j = 0; j < sys->Tfun->objfun.size(); j++)
    {
        if (sys->Tfun->objfun[j].isComputed)
        {
            for (unsigned int i = 0; i < sys->aTfun.size(); i++)
            {
                if (sys->aTfun[i].objfun[j].isComputed)
                {
                    abs_res.push_back(fabs(sys->aTfun[i].objfun[j].results.residual));
                }

            }
            if (abs_res.size()>0)
            median_.push_back(median(abs_res));
            C.push_back(sys->Opti->OptTuckeyVal * median_[j]);
            vabs_res.push_back(abs_res);
            abs_res.clear();
        } else
        {
        median_.push_back(0.0);
        C.push_back(-1000);
        }
    }

//#ifdef useomp
//    omp_set_num_threads(sys->MPI);
//    #pragma omp parallel for
//#endif
    for (unsigned int j = 0; j < sys->Tfun->objfun.size(); j++)
    {
        for (unsigned int i = 0; i < sys->aTfun.size(); i++)
        {
            if (sys->aTfun[i].objfun[j].isComputed)
            {
                double absres = fabs(sys->aTfun[i].objfun[j].results.residual);
                if (( C[j] - absres) < 0)
                    sys->aTfun[i].objfun[j].TuWeight = 0;
                else
                    sys->aTfun[i].objfun[j].TuWeight = pow((1 - pow((absres/C[j]), 2) ), 2);
            }
        }
    }
}

void set_Tuckey_weight_objfun_norm (TGfitTask *sys)
{
    vector<double> C;
    vector<double> median_;
    vector<vector<double> > vabs_res;
    vector<double> abs_res;

    for (unsigned int j = 0; j < sys->Tfun->objfun.size(); j++)
    {
        if (sys->Tfun->objfun[j].isComputed)
        {
            for (unsigned int i = 0; i < sys->aTfun.size(); i++)
            {
                if (sys->aTfun[i].objfun[j].isComputed)
                {
                    abs_res.push_back(fabs((sys->aTfun[i].objfun[j].results.residual/sys->aTfun[i].objfun[j].results.computed_value)));
                }
            }
            median_.push_back(median(abs_res));
            C.push_back(sys->Opti->OptTuckeyVal * median_[j]);
            vabs_res.push_back(abs_res);
            abs_res.clear();
        } else
        {
        median_.push_back(0.0);
        C.push_back(-1000);
        }
    }

//#ifdef useomp
//    omp_set_num_threads(sys->MPI);
//    #pragma omp parallel for
//#endif
    for (unsigned int j = 0; j < sys->Tfun->objfun.size(); j++)
    {
        if (sys->Tfun->objfun[j].isComputed)
        {
            for (unsigned int i = 0; i < sys->aTfun.size(); i++)
            {
                if (sys->aTfun[i].objfun[j].isComputed)
                {
                    double absres = fabs((sys->aTfun[i].objfun[j].results.residual/sys->aTfun[i].objfun[j].results.computed_value));
                    if (( C[j] - absres) < 0)
                        sys->aTfun[i].objfun[j].TuWeight = 0;
                    else
                        sys->aTfun[i].objfun[j].TuWeight = pow((1 - pow((absres/C[j]), 2) ), 2);
                }
            }
        }
    }
}

void set_Tuckey_weight_objfun_norm2 (TGfitTask *sys)
{
    vector<double> C;
    vector<double> median_;
    vector<vector<double> > vabs_res;
    vector<double> abs_res;

    for (unsigned int j = 0; j < sys->Tfun->objfun.size(); j++)
    {
        if (sys->Tfun->objfun[j].isComputed)
        {
            for (unsigned int i = 0; i < sys->aTfun.size(); i++)
            {
                if (sys->aTfun[i].objfun[j].isComputed)
                {
                    abs_res.push_back(fabs((sys->aTfun[i].objfun[j].results.residual/sys->aTfun[i].objfun[j].results.computed_value)));
                }
            }
            median_.push_back(median(abs_res));
            C.push_back(sys->Opti->OptTuckeyVal * median_[j]);
            vabs_res.push_back(abs_res);
            abs_res.clear();
        } else
        {
        median_.push_back(0.0);
        C.push_back(-1000);
        }
    }

//#ifdef useomp
//    omp_set_num_threads(sys->MPI);
//    #pragma omp parallel for
//#endif
    for (unsigned int j = 0; j < sys->Tfun->objfun.size(); j++)
    {
        if (sys->Tfun->objfun[j].isComputed)
        {
            for (unsigned int i = 0; i < sys->aTfun.size(); i++)
            {
                if (sys->aTfun[i].objfun[j].isComputed)
                {
                    double absres = fabs((sys->aTfun[i].objfun[j].results.residual/sys->aTfun[i].objfun[j].results.computed_value));
                    if (( C[j] - absres) < 0)
                        sys->aTfun[i].objfun[j].TuWeight = 0;
                    else
                        sys->aTfun[i].objfun[j].TuWeight = 1;
                }
            }
        }
    }
}

