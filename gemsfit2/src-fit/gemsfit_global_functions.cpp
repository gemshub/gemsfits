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
    { // if parameters were not normalized
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

    // going trough the adjusted parameters in Opti->Ptype and adjusts them with the new value
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

    titration (sys);
//    cout << "finished titration correction"<< endl;

////#ifdef USE_MPI
    omp_set_num_threads(sys->MPI);
    #pragma omp parallel for
////#endif
    // +++ Calculating equilibrium with GEMS3K +++ //
    for (unsigned int i=0; i<sys->NodT.size(); ++i)
    {
        vector<DATABR*> dBR;
        dBR.push_back(sys->NodT[i]->pCNode());
        long int NodeStatusCH;

            // Set the P in the node->CNode-P as in the experiments to avoind problem due to Psat notation as 0
        for (unsigned int i=0; i<sys->experiments.size(); ++i)
        {
            sys->NodT[i]->Set_TK(273.15 + sys->experiments[i]->sT);
            sys->NodT[i]->Set_P(100000 * sys->experiments[i]->sP);
        }

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

    // Set the P in the node->CNode-P as in the experiments to avoind problem due to Psat notation as 0
//    for (unsigned int i=0; i<sys->experiments.size(); ++i)
//    {
//        sys->NodT[i]->Set_TK(273.15 + sys->experiments[i]->sT);
//        sys->NodT[i]->Set_P(100000 * sys->experiments[i]->sP);
//    }

    sys->get_sum_of_residuals(residuals_sys_);
    residuals_sys = residuals_sys_;

//    vector<string> nom, denom;
//    string name_MR = "Al+Si+Cu/Na+K+O";
//    interpretMR(&nom,&denom, name_MR);
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


void titration (TGfitTask *sys)
{
    bool h_HCl = false, h_NaOH = false;
    int i, j;
    double old_HCl, old_NaOH;

    // loop trough all experiments
    for (i = 0; i<sys->experiments.size(); i++)
    {
        h_HCl = false; h_NaOH = false;
        // loop torugh all comp to check for titrant
        for (j=0; j<sys->experiments[i]->sbcomp.size(); j++)
        {
            if (sys->experiments[i]->sbcomp[j]->comp == "HCl")
            {
                h_HCl = true;
                old_HCl = sys->experiments[i]->sbcomp[j]->Qnt;
                sys->tit_nr = j;
            }
            if (sys->experiments[i]->sbcomp[j]->comp == "NaOH")
            {
                h_NaOH = true;
                old_NaOH = sys->experiments[i]->sbcomp[j]->Qnt;
                sys->tit_nr = j;
            }
        }
        //adjust HCl
        if (h_HCl)
        {
            nlopt::opt opt_HCL(nlopt::LN_COBYLA, 1);

            std::vector<double> x;
            x.push_back( old_HCl);
            double minf;
            sys->exp_nr=i;

            vector<double> gr;

            titfunc(x, gr, sys);

            double pH_dif = abs (sys->experiments[i]->expphases[0]->phprop[0]->Qnt - sys->NodT[i]->Get_pH());

            opt_HCL.set_lower_bounds(old_HCl-0.1*pH_dif*old_HCl);
            opt_HCL.set_upper_bounds(old_HCl+0.1*pH_dif*old_HCl);

            opt_HCL.set_min_objective(titfunc, sys);

            opt_HCL.set_xtol_rel(1e-3);

//            std::vector<double> x;
//            x.push_back( old_HCl);
//            double minf;
            sys->exp_nr=i;
            nlopt::result result = opt_HCL.optimize(x, minf);
            sys->experiments[sys->exp_nr]->sbcomp[sys->tit_nr]->Qnt = x[0];

//            cout << "happy"<< endl;

        }

        //adjust HCl
        if (h_NaOH)
        {
            nlopt::opt opt_NaOH(nlopt::LN_BOBYQA, 1);

            std::vector<double> x;
            x.push_back( old_NaOH);
            double minf;
            sys->exp_nr=i;

            vector<double> gr;

            titfunc(x, gr, sys);

            double pH_dif = abs (sys->experiments[i]->expphases[0]->phprop[0]->Qnt - sys->NodT[i]->Get_pH());

            opt_NaOH.set_lower_bounds(old_NaOH-0.1*pH_dif*old_NaOH);
            opt_NaOH.set_upper_bounds(old_NaOH+0.1*pH_dif*old_NaOH);

            opt_NaOH.set_min_objective(titfunc, sys);

            opt_NaOH.set_xtol_rel(1e-4);

//            std::vector<double> x;
//            x.push_back( old_NaOH);
//            double minf;
            sys->exp_nr=i;
            nlopt::result result = opt_NaOH.optimize(x, minf);
            sys->experiments[sys->exp_nr]->sbcomp[sys->tit_nr]->Qnt = x[0];

        }



    }

}

double titfunc(const std::vector<double> &x, std::vector<double> &grad, void *obj_func_data)
{
    TGfitTask *sys = reinterpret_cast<TGfitTask*>(obj_func_data);
    double dif = 0.0, new_moles = 0.0, residual = 0.0;

    // DATACH structure content
    int i, nIC, nDC, nPH, ICndx, DCndx/*, PHndx*/;
    long int NodeStatusCH, NodeHandle;
    double P_pa, T_k/*, PMc*/;
    double* new_moles_IC;
    double* xDC_up;
    double* xDC_lo;
    double* Ph_surf;

    // adjust titrant

    // Getting direct access to work node DATABR structure which exchanges the
    // data with GEMS3K (already filled out by reading the DBR input file)
//    DATABR* dBR = sys->NodT[sys->tit_nr]->pCNode();
    DATACH* dCH = sys->NodT[sys->tit_nr]->pCSD();

    nIC = dCH->nIC;	// nr of independent components
    nDC = dCH->nDC;	// nr of dependent components
    nPH = dCH->nPH;
    xDC_up = new double[ nDC ];
    xDC_lo = new double[ nDC ];
    Ph_surf = new double[ nPH ];
    new_moles_IC = new double [ nIC ]; // vector for holding the moles of independent components for each experiment

    // lower and upper bounds for concentration of DC
    for( i=0; i<nDC; i++ )
    {
        xDC_up[ i ]  = 1000000.;
        xDC_lo[ i ]  = 0.;
    }


    for ( i=0; i<nIC; i++)
    {
        new_moles_IC[i] = sys->NodT[sys->exp_nr]->Get_bIC(i);
    }

//    P_pa = 100000 * sys->experiments[sys->exp_nr]->sP;
//    T_k = 273.15 + sys->experiments[sys->exp_nr]->sT;


    dif = sys->experiments[sys->exp_nr]->sbcomp[sys->tit_nr]->Qnt - x[0];

    if (sys->experiments[sys->exp_nr]->sbcomp[sys->tit_nr]->comp == "NaOH")
    {
//        if (experiments[n]->sbcomp[j]->Qunit == keys::molal)
//        {
//            experiments[n]->sbcomp[j]->Qnt = experiments[n]->sbcomp[j]->Qnt*h2o_kgamount*39.99710928;
//            experiments[n]->sbcomp[j]->Qunit = keys::gram;
//        }
        ICndx = sys->NodT[sys->exp_nr]->IC_name_to_xDB("Na");
        new_moles = sys->NodT[sys->exp_nr]->Get_bIC(ICndx) + 1*dif/39.99710928;
        sys->NodT[sys->exp_nr]->Set_bIC(ICndx, new_moles);
        ICndx = sys->NodT[sys->exp_nr]->IC_name_to_xDB("H");
        new_moles = sys->NodT[sys->exp_nr]->Get_bIC(ICndx) + 1*dif/39.99710928;
        sys->NodT[sys->exp_nr]->Set_bIC(ICndx, new_moles);
        ICndx = sys->NodT[sys->exp_nr]->IC_name_to_xDB("O");
        new_moles = sys->NodT[sys->exp_nr]->Get_bIC(ICndx) + 1*dif/39.99710928;
        sys->NodT[sys->exp_nr]->Set_bIC(ICndx, new_moles);

    } else
        if (sys->experiments[sys->exp_nr]->sbcomp[sys->tit_nr]->comp == "HCl")
        {
//                        if (experiments[n]->sbcomp[j]->Qunit == keys::molal)
//                        {
//                            experiments[n]->sbcomp[j]->Qnt = experiments[n]->sbcomp[j]->Qnt*h2o_kgamount*36.4611;
//                            experiments[n]->sbcomp[j]->Qunit = keys::gram;
//                        }
            ICndx =sys->NodT[sys->exp_nr]->IC_name_to_xDB("H");
            new_moles = sys->NodT[sys->exp_nr]->Get_bIC(ICndx) + 1*dif/36.4611;
            sys->NodT[sys->exp_nr]->Set_bIC(ICndx, new_moles);
            ICndx = sys->NodT[sys->exp_nr]->IC_name_to_xDB("Cl");
            double old_moles = sys->NodT[sys->exp_nr]->Get_bIC(ICndx);
            new_moles = sys->NodT[sys->exp_nr]->Get_bIC(ICndx) + 1*dif/36.4611;
            sys->NodT[sys->exp_nr]->Set_bIC(ICndx, new_moles);

        }

//    sys->NodT[sys->exp_nr]->GEM_from_MT( NodeHandle, NodeStatusCH, T_k, P_pa, new_moles_IC, xDC_up, xDC_lo );

    sys->NodT[sys->exp_nr]->Set_TK(273.15 + sys->experiments[sys->exp_nr]->sT);
    sys->NodT[sys->exp_nr]->Set_P(100000 * sys->experiments[sys->exp_nr]->sP);


    // calc equilibirum
    vector<DATABR*> dBR;
    dBR.push_back(sys->NodT[sys->exp_nr]->pCNode());

    // Asking GEM to run with automatic initial approximation
    dBR.at(0)->NodeStatusCH = NEED_GEM_AIA;

    // RUN GEMS3K
    NodeStatusCH = sys->NodT[sys->exp_nr]->GEM_run( false );

    if( NodeStatusCH == OK_GEM_AIA || NodeStatusCH == OK_GEM_SIA  )
    {
//            sys->NodT[i]->GEM_print_ipm( "GEMS3K_log.out" );   // possible debugging printout
    }
    else
    {
        // possible return status analysis, error message
//            sys->NodT[i]->GEM_print_ipm( "GEMS3K_log.out" );   // possible debugging printout
        cout<<"For experiment titration "<<sys->exp_nr+1<< endl;
        cout<<" GEMS3K did not converge properly !!!! continuing anyway ... "<<endl;
    }


    // calc residual
    double meas_pH = sys->experiments[sys->exp_nr]->expphases[0]->phprop[0]->Qnt;
    double calc_pH = sys->NodT[sys->exp_nr]->Get_pH();

    residual = abs (sys->experiments[sys->exp_nr]->expphases[0]->phprop[0]->Qnt - sys->NodT[sys->exp_nr]->Get_pH());

    return residual;


}

