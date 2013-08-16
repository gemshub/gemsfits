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
#include "gemsfit_task.h"

// Wrapper around GEMS3K solver
void gems3k_wrap( double &residuals_sys, const std::vector<double> &opt, TGfitTask* sys )
{
    // Call GEMS3K and run GEM_run();

    // Temporary storage vectors
    double residuals_sys_ = 0.0;

    master_counter++;

    // Clear already stored results
    sys->computed_values_v.clear();
    sys->measured_values_v.clear();
    sys->Weighted_Tfun_residuals_v.clear();
    sys->Tfun_residuals_v.clear();
    sys->residuals_v.clear();
    sys->weights.clear();

    sys->print->print_clear();

    // going trough the adjusted parameters in Opti->Ptype and adjust with the new value
//#ifdef USE_MPI
    omp_set_num_threads(sys->MPI);
    #pragma omp parallel for
//#endif
    for (unsigned int i=0; i< sys->Opti->Ptype.size(); ++i)
    {
        // parameter is Std State Gibbs free energy
        if (sys->Opti->Ptype[i] == "G0")
        {
            adjust_G0(i, opt[i], sys);
        } else // adjust PMc
        if (sys->Opti->Ptype[i] == "LsMod")
        {
           adjust_PMc(i, opt[i], sys);
        } else // adjust DMc
        if (sys->Opti->Ptype[i] == "LsMdc")
        {
           adjust_DMc(i, opt[i], sys);
        } else // adjust bIC
        if (sys->Opti->Ptype[i] == "bIC")
        {
           adjust_bIC(i, opt[i], sys);
        } else // adjust TK
        if (sys->Opti->Ptype[i] == "TK")
        {
           adjust_TK(i, opt[i], sys);
        } else // adjust P
        if (sys->Opti->Ptype[i] == "P")
        {
           adjust_P(i, opt[i], sys);
        }

        /// if other parameters...

    } // END loop trough parameters

    /// REACTION DC
    if (sys->Opti->h_RDc)
    {
        adjust_RDc(sys);
    }
    /// Linked parameters
    if (sys->Opti->h_Lp)
    {
        adjust_Lp(sys);
    }

////#ifdef USE_MPI
    omp_set_num_threads(sys->MPI);
    #pragma omp parallel for
////#endif
    for (unsigned int i=0; i<sys->NodT.size(); ++i)
    {
        vector<DATABR*> dBR;
        dBR.push_back(sys->NodT[i]->pCNode());
        long int NodeStatusCH;

        // Asking GEM to run with automatic initial approximation
        dBR.at(0)->NodeStatusCH = NEED_GEM_AIA;

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
            cout<<"For experiment "<<i+1<< endl;
            cout<<" GEMS3K did not converge properly !!!! continuing anyway ... "<<endl;
        }
    }

    // Set the P in the node->CNode-P as in the experiments
    for (unsigned int i=0; i<sys->experiments.size(); ++i)
    {
        sys->NodT[i]->Set_TK(273.15 + sys->experiments[i]->sT);
        sys->NodT[i]->Set_P(100000 * sys->experiments[i]->sP);
    }

/*
    /// Target function
    // Loop trough all experiments
//    for (int i=0; i<sys->experiments.size(); ++i)
//    {
//        // loop trough objective function
//        for (int j=0; j<sys->Tfun->objfun.size(); ++j)
//        {
//            if ((sys->Tfun->objfun[j]->exp_phase !="NULL") && (sys->experiments[i]->expphases.size() > 0))
//            {
//                // loop trough all pahses
//                for (int p=0; p<sys->experiments[i]->expphases.size(); ++p)
//                {
//                    if ((sys->Tfun->objfun[j]->exp_elem !="NULL") && (sys->Tfun->objfun[j]->exp_property =="NULL"))
//                    {
//                        // loop trough all elements
//                        for (int e=0; e<sys->experiments[i]->expphases[p]->phcomp.size(); ++e)
//                        {
//                            if ((sys->experiments[i]->expphases[p]->phcomp[e]->comp == sys->Tfun->objfun[j]->exp_elem) && (sys->experiments[i]->expphases[p]->phase == sys->Tfun->objfun[j]->exp_phase ))
//                            {
//                                // check for unit
//                                check_unit(i, p, e, sys->Tfun->objfun[j]->exp_unit, sys );
//                                residuals_sys_ = residuals_sys_ + residual_phase_elem (i, p, e, j, sys);
//                            }
//                        }
//                    } else
//                        if ((sys->Tfun->objfun[j]->exp_property !="NULL") && (sys->experiments[i]->expphases[p]->phprop.size() > 0) && (sys->Tfun->objfun[j]->exp_dcomp == "NULL"))
//                        {
//                        // loop trough all properties
//                        for (int pp = 0; pp< sys->experiments[i]->expphases[p]->phprop.size(); ++pp)
//                        {
//                            if ((sys->experiments[i]->expphases[p]->phprop[pp]->property == sys->Tfun->objfun[j]->exp_property) && (sys->experiments[i]->expphases[p]->phase == sys->Tfun->objfun[j]->exp_phase ))
//                            {
//                                // check for unit
//                                check_prop_unit(i, p, pp, sys->Tfun->objfun[j]->exp_unit, sys );
//                                residuals_sys_ = residuals_sys_ + residual_phase_prop (i, p, pp, j, sys);
//                            }
//                        }
//                    } else
//                        if ((sys->Tfun->objfun[j]->exp_property !="NULL") && (sys->experiments[i]->expphases[p]->phdcomps.size() > 0) && (sys->Tfun->objfun[j]->exp_dcomp != "NULL"))
//                        {
//                            // loop trough all dependent components
//                            for (int dc = 0; dc< sys->experiments[i]->expphases[p]->phdcomps.size(); ++dc)
//                            {
//                                if ((sys->experiments[i]->expphases[p]->phdcomps[dc]->formula == sys->Tfun->objfun[j]->exp_dcomp) && (sys->experiments[i]->expphases[p]->phase == sys->Tfun->objfun[j]->exp_phase ))
//                                {
//                                    // loop trough all dep comp properties
//                                    for (int dcp = 0; dcp < sys->experiments[i]->expphases[p]->phdcomps[dc]->dcompprop.size(); ++dcp)
//                                    {
//                                        if (sys->experiments[i]->expphases[p]->phdcomps[dc]->dcompprop[dcp]->property == sys->Tfun->objfun[j]->exp_property)
//                                        {
////                                            cout << "yes"<<endl;
//                                            //                                    // check for unit
//                                            //                                    check_dcomp_unit(i, p, dc, dcp, sys->Tfun->objfun[j]->exp_unit, sys );
//                                            residuals_sys_ = residuals_sys_ + residual_phase_dcomp (i, p, dc, dcp, j, sys);
//                                        }
//                                    }
//                                }
//                            }
//                        }
//                }
//            }
//        }
//    }
*/

    sys->get_residuals(residuals_sys_);
    residuals_sys = residuals_sys_;
//    cout << residuals_sys << endl;

    // debug for when using global algorithm
    if (master_counter%1000 == 0)
    {
        cout << master_counter << " itterations, continuning..." << endl;
        cout << "sum of residuals: "<<residuals_sys<< endl;
        for (unsigned int i = 0; i<opt.size(); ++i)
        {
            cout<<"parameter "<<i<<" : "<<opt[i]<<endl;
        }
    }
}


double Equil_objective_function_callback( const std::vector<double> &opt, std::vector<double> &grad, void *obj_func_data )
{
    TGfitTask *sys = reinterpret_cast<TGfitTask*>(obj_func_data);

    unsigned int i;
    double sum_of_squared_residuals_allsys = 0.0;
    double sum_of_squared_residuals_sys = 0.0;

    // Rescale optimization to unconvert normalization of parameters
    if( sys->NormParams )
    {
        vector<double> optV( opt.size() );
        for( i=0; i<opt.size(); i++ )
        {
            optV[i] = opt[i] * fabs(sys->Opti->opt[i]);
        }

        /// GRadient calculation
        if (sys->Opti->OptAlgo.compare(2,1,"D") == 0)
        {
            gradient(optV, grad, sys);
        }

        // call tsolmod wrapper
        gems3k_wrap( sum_of_squared_residuals_sys, optV, sys );
        sum_of_squared_residuals_allsys = sum_of_squared_residuals_allsys + sum_of_squared_residuals_sys;
    }
    else
    {
        /// GRadient calculation
        if (sys->Opti->OptAlgo.compare(1,1,"D") == 0)
        {
            gradient(opt, grad, sys);
        }

        // call tsolmod wrapper
        gems3k_wrap( sum_of_squared_residuals_sys, opt, sys );
        sum_of_squared_residuals_allsys = sum_of_squared_residuals_allsys + sum_of_squared_residuals_sys;
    }
return sum_of_squared_residuals_allsys;
}

void gradient( vector<double> opt, vector<double> &grad, TGfitTask *sys )
{
    double residual_sys;

    double computed_up, param_up;
    double computed_lo, param_lo;
    std::vector<double> opt_scan;

    grad.clear();
    grad.resize(opt.size());

    opt_scan.resize( opt.size() );

    double delta = sys->Opti->OptPerturbator;
    for(int i=0; i< (int) opt.size(); i++ )
    {
        opt_scan = opt;

        // Central finite differences:
        opt_scan[i] = opt[i] + opt[i]*delta;
        param_up = opt[i] + opt[i]*delta;
        residual_sys = 0.;
        gems3k_wrap( residual_sys, opt_scan, sys );
//cout<<"residual_sys = "<<residual_sys<<endl;
        computed_up = residual_sys;

        opt_scan[i] = opt[i] - opt[i]*delta;
        param_lo = opt[i] - opt[i]*delta;
        residual_sys = 0.;
        gems3k_wrap( residual_sys, opt_scan, sys );
//cout<<"residual_sys = "<<residual_sys<<endl;
        computed_lo = residual_sys;

        grad[i] = (computed_up - computed_lo) / (param_up - param_lo);
    }
}

