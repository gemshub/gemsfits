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
#include "gemsfit_task.h"


// Wrapper around GEMS3K solver
void gems3k_wrap( double &residuals_sys, const std::vector<double> &opt, TGfitTask* sys )
{
    // Call GEMS3K and run GEM_run();

    // Temporary storage vectors
    vector<double> computed_values_temp;
    vector<double> measured_values_temp;
    vector<double> computed_residuals_temp, computed_leastsquare_temp;
    double residuals_sys_ = 0.0;


    // Clear already stored results
    sys->computed_values_v.clear();
    sys->measured_values_v.clear();
    sys->computed_residuals_v.clear();

    // going trough the adjusted parameters in Opti->Ptype and adjust with the new value
    for (int i=0; i< sys->Opti->Ptype.size(); ++i)
    {
        // parameter is Std State Gibbs free energy
        if (sys->Opti->Ptype[i] == "G0")
        {
            adjust_G0(i, opt[i], sys);

        } // END if G0

        /// if other parameters...

    } // END loop trough parameters

    /// REACTION DC
    if (sys->Opti->h_RDc)
    {
        adjust_RDc(sys);
    }

    // Asking GEM to run with automatic initial approximation after changing the G0, if not it will always take the values of the exp form which the 0000dbr file is made.
    for (int i; i<sys->NodT.size(); ++i)
    {
        DATABR* dBR = sys->NodT[i]->pCNode();
        DATACH* dCH = sys->NodT[i]->pCSD();
        long int NodeStatusCH;
        dBR->NodeStatusCH = NEED_GEM_AIA;

        // PERFORM TEST RUN WITH DBR file
        NodeStatusCH = sys->NodT[i]->GEM_run( false );

    #ifdef GEMSFIT_DEBUG
    cout << " NodeStatusCH = "<<NodeStatusCH<<endl;
    cout << "   node temperature from dbr file: " << sys->NodT[i]->cTK() << endl;
    cout << "   node pressure from dbr file: " << sys->NodT[i]->cP() << endl;
    #endif
        if( NodeStatusCH == OK_GEM_AIA || NodeStatusCH == OK_GEM_SIA  )
        {
            sys->NodT[i]->GEM_print_ipm( "SS_GEMS3K_log.out" );
        }
        else
        {
            // possible return status analysis, error message
            sys->NodT[i]->GEM_print_ipm( "SS_GEMS3K_log.out" );   // possible debugging printout
            cout<<" GEMS3K did not converge properly !!!! continuing anyway ... "<<endl;
        }
    }

    cout << "end" << endl;

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
                                if (sys->experiments[i]->expphases[p]->phase == "aq_gen")
                                {
                                    residuals_sys_ = residuals_sys_ + residual_aqgen_elem (i, p, e, sys);
                                } else // other phase than aq_gen
                                {
                                    residuals_sys_ = residuals_sys_ + residual_phase_elem (i, p, e, sys);
                                }



                            }
                        }
                    }
                }
            }
        }
    }

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


void adjust_G0 (int i, double G0, TGfitTask *sys)
{

    double new_G0=G0-444;
    double new_GTP=0.0;
    double delta_G0old_G0new;
    int species_index = sys->Opti->Pindex[i];
    // going trough all nodes
    for (int n=0; n<sys->NodT.size(); ++n)
    {
        delta_G0old_G0new = sys->NodT[n]->DC_G0(species_index, 1e+05, 298.15, false) - new_G0;
        // going trough all TP pairs
        for (int j=0; j<sys->TP_pairs[0].size(); ++j)
        {
            new_GTP = delta_G0old_G0new + sys->NodT[n]->DC_G0(species_index, sys->TP_pairs[1][j]*100000, sys->TP_pairs[0][j]+273.15, false);
            // Set the new G0 in GEMS
            sys->NodT[n]->Set_DC_G0(species_index, sys->TP_pairs[1][j]*100000, sys->TP_pairs[0][j]+273.15, new_GTP);
            // cout << temp_v[j] << endl;
        }
        sys->NodT[n]->Set_DC_G0(species_index, 1e+05, 298.15, G0-444);
    }
}

void adjust_RDc (TGfitTask *sys)
{
    // going trough all nodes
    for (int n=0; n<sys->NodT.size(); ++n)
    {
        for (int i=0; i < sys->Opti->reactions.size(); ++i )
        {
            double new_G0=0;
            double delta_G=0;
            double R=8.314472;
            double delta_G0old_G0new;
            int species_index = sys->Opti->reactions[i]->rdc_species_ind[sys->Opti->reactions[i]->rdc_species_ind.size()-1];

            for (int j=0; j < sys->Opti->reactions[i]->rdc_species.size()-1; ++j ) // calculates DG without the last species which is the constrained one
            {
                delta_G += sys->Opti->reactions[i]->rdc_species_coef[j] * sys->NodT[n]->DC_G0(sys->Opti->reactions[i]->rdc_species_ind[j], 1e+05, 298.15, false);
            }

            new_G0 = (-R*298.15*2.302585093*sys->Opti->reactions[i]->logK) - delta_G;
            delta_G0old_G0new = sys->NodT[n]->DC_G0(species_index, 1e+05, 298.15, false) - new_G0;
            sys->NodT[n]->Set_DC_G0(species_index,1*100000, 25+273.15, new_G0);
            sys->Opti->reactions[i]->std_gibbs = new_G0;

            for (int j=0; j<sys->TP_pairs[0].size(); j++) // loops trough all unique TP_pairs
            {
                new_G0 = delta_G0old_G0new + sys->NodT[n]->DC_G0(species_index, sys->TP_pairs[1][j]*100000, sys->TP_pairs[0][j]+273.15, false);
                // Set the new G0 in GEMS
                sys->NodT[n]->Set_DC_G0(species_index, sys->TP_pairs[1][j]*100000, sys->TP_pairs[0][j]+273.15, new_G0);
                // cout << temp_v[j] << endl;
            }
        }
    }
}

void check_unit(int i, int p, int e, string unit, TGfitTask *sys )
{
    if (sys->experiments[i]->expphases[p]->phcomp[e]->Qunit != unit)
    {
        if (unit == "loga")
        {
            // molal to log(molal)
            if (sys->experiments[i]->expphases[p]->phcomp[e]->Qunit == "molal")
            {
                sys->experiments[i]->expphases[p]->phcomp[e]->bQnt = log10(sys->experiments[i]->expphases[p]->phcomp[e]->bQnt);
                sys->experiments[i]->expphases[p]->phcomp[e]->Qunit = "loga";
            }
            else
            {
                cout << "Unit for experiment: "<< i <<" from "<< sys->experiments[i]->expdataset << " is not implemented"<< endl;
                exit(1);
            }
        }
    }
    else
    {
        cout << "Unit for experiment: "<< i <<" from "<< sys->experiments[i]->expdataset << " is not implemented"<< endl;
        exit(1);
    }
}


double residual_aqgen_elem (int i, int p, int e, TGfitTask *sys)
{
    const char *elem_name;
    int ICndx = sys->NodT[i]->IC_name_to_xDB(elem_name);
    double computed_value, measured_value;
    double residual = 0.0;

    computed_value = sys->NodT[i]->Get_mIC(ICndx);
    measured_value = sys->experiments[i]->expphases[p]->phcomp[e]->bQnt;

    // check Target function type and calculate the residual
    if (sys->Tfun->type == "lsq")
    {
        residual = least_square(computed_value, measured_value);
    } else
    {
        // other type of Target functions
    }

    /// add weighting!!!
    /// to be implemented


    return residual;
}

double least_square (double computed_value, double measured_value)
{
    double lsq = 0.0;
    lsq = pow( (computed_value - measured_value), 2) / pow(measured_value, 2);
    return lsq;
}

