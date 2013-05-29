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

    if (master_counter%10000 == 0)
    {
        cout << master_counter << " itterations, continuning..." << endl;
    }

    // Clear already stored results
    sys->computed_values_v.clear();
    sys->measured_values_v.clear();
    sys->computed_residuals_v.clear();

    sys->print->print_clear();


    // going trough the adjusted parameters in Opti->Ptype and adjust with the new value
//#ifdef USE_MPI
//    omp_set_num_threads(sys->MPI);
//    #pragma omp parallel for
//#endif
    for (int i=0; i< sys->Opti->Ptype.size(); ++i)
    {
        // parameter is Std State Gibbs free energy
        if (sys->Opti->Ptype[i] == "G0")
        {
            adjust_G0(i, opt[i], sys);

        } // END if G0
        else
        if (sys->Opti->Ptype[i] == "LsMod")
        {
           adjust_PMc(i, opt[i], sys);
        }

        /// if other parameters...

    } // END loop trough parameters

    /// REACTION DC
    if (sys->Opti->h_RDc)
    {
        adjust_RDc(sys);
    }

//     cout << sys->NodT[0]->DC_G0(5, 1*100000, 22+273.15, false)  << endl;
    // Equilibrium calculation
//    cout << sys->NodT[0]->Get_mIC(4) << endl;

//#ifdef USE_MPI
    omp_set_num_threads(sys->MPI);
    #pragma omp parallel for
//#endif
    for (int i=0; i<sys->NodT.size(); ++i)
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


//    long int NodeStatusCH = sys->NodT[0]->GEM_run( false );
//    cout << sys->NodT[0]->DC_G0(5, 1*100000, 22+273.15, false)  << endl;
//    cout << sys->NodT[0]->Get_mIC(4) << endl;
//    cout << "end" << endl;



    /// Target function
    // Loop trough all experiments
    for (int i=0; i<sys->experiments.size(); ++i)
    {
        // loop trough objective function
        for (int j=0; j<sys->Tfun->objfun.size(); ++j)
        {
            if ((sys->Tfun->objfun[j]->exp_phase !="NULL") && (sys->experiments[i]->expphases.size() > 0))
            {
                // loop trough all pahses
                for (int p=0; p<sys->experiments[i]->expphases.size(); ++p)
                {
                    if (sys->Tfun->objfun[j]->exp_elem !="NULL")
                    {
                        // loop trough all elements
                        for (int e=0; e<sys->experiments[i]->expphases[p]->phcomp.size(); ++e)
                        {
                            if (sys->experiments[i]->expphases[p]->phcomp[e]->comp == sys->Tfun->objfun[j]->exp_elem)
                            {
                                // check for unit
                                check_unit(i, p, e, sys->Tfun->objfun[j]->exp_unit, sys );
                                if ((sys->experiments[i]->expphases[p]->phase == "aq_gen") && (sys->Tfun->objfun[j]->exp_phase == "aq_gen"))
                                {
                                    if (sys->Tfun->weight.size() > 0)
                                    {
                                       residuals_sys_ = residuals_sys_ + residual_aqgen_elem (i, p, e, sys)*weight(i, p, e, sys->Tfun->weight, sys);
                                    } else residuals_sys_ = residuals_sys_ + residual_aqgen_elem (i, p, e, sys);
                                } else // other phase than aq_gen
                                {
                                    if ((sys->Tfun->objfun[j]->exp_phase != "aq_gen") && (sys->experiments[i]->expphases[p]->phase != "aq_gen"))
                                    {
                                    residuals_sys_ = residuals_sys_ + residual_phase_elem (i, p, e, sys);
                                    }
                                }
                            }
                        }
                    } else
                    if ((sys->Tfun->objfun[j]->exp_property !="NULL") && (sys->experiments[i]->expphases[p]->phprop.size() > 0))
                    {
                        // loop trough all properties
                        for (int pp = 0; pp< sys->experiments[i]->expphases[p]->phprop.size(); ++pp)
                        {
                            if (sys->experiments[i]->expphases[p]->phprop[pp]->property == sys->Tfun->objfun[j]->exp_property)
                            {
                                // check for unit
                                check_ph_unit(i, p, pp, sys->Tfun->objfun[j]->exp_unit, sys );
                                if ((sys->experiments[i]->expphases[p]->phase == "aq_gen") && (sys->Tfun->objfun[j]->exp_phase == "aq_gen"))
                                {
                                    if (sys->Tfun->weight.size() > 0)
                                    {
                                       residuals_sys_ = residuals_sys_ + residual_aqgen_prop (i, p, pp, j, sys)*weight_phprop(i, p, pp, sys->Tfun->weight, sys);
                                    } else residuals_sys_ = residuals_sys_ + residual_aqgen_prop (i, p, pp, j, sys);
                                } else // other phase than aq_gen
                                {
                                    if ((sys->Tfun->objfun[j]->exp_phase != "aq_gen") && (sys->experiments[i]->expphases[p]->phase != "aq_gen"))
                                    {
                                        if (sys->Tfun->weight.size() > 0)
                                        {
                                           residuals_sys_ = residuals_sys_ + residual_phase_prop (i, p, pp, j, sys)*weight_phprop(i, p, pp, sys->Tfun->weight, sys);
                                        } else residuals_sys_ = residuals_sys_ + residual_phase_prop (i, p, pp, j, sys);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    residuals_sys = residuals_sys_;
}




double Equil_objective_function_callback( const std::vector<double> &opt, std::vector<double> &grad, void *obj_func_data )
{
    // If Master process enters function callback, he sends a signal to the others processes which makes them in turn calling ActMod_objective_function_callback directly. They are waiting for this signal in the init_optim function.
    int continue_or_exit = 1;
    int pid_;

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
            optV[i] = opt[i] * abs(sys->Opti->opt[i]);
        }
        // call tsolmod wrapper
        gems3k_wrap( sum_of_squared_residuals_sys, optV, sys );
        sum_of_squared_residuals_allsys = sum_of_squared_residuals_allsys + sum_of_squared_residuals_sys;
    }
    else
    {
    // call tsolmod wrapper
    gems3k_wrap( sum_of_squared_residuals_sys, opt, sys );
    sum_of_squared_residuals_allsys = sum_of_squared_residuals_allsys + sum_of_squared_residuals_sys;
    }
return sum_of_squared_residuals_allsys;
}

