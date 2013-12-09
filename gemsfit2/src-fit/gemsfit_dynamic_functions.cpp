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
 *	@file gemsfit_dynamic_functions.cpp
 *
 *	@brief this source file contains implementations of global dynamic functions needed during specific optimization.
 *	tasks. When the user wants to dyamically adjust other system parameters during fitting. For example
 *  adjusting the NaOH or HCl amount so that the pH is the same as measured, during optimization, since there is no
 *  clear info on their amount from the experimental paper.
 *
 *	@author G. Dan Miron
 *
 * 	@date 01.12.2013
 *
 */


#include "gemsfit_dynamic_functions.h"


void titration (TGfitTask *sys)
{
    // for checking if the experiment has HCl, or NaOH and if pH is present
    vector<bool> h_HCl, h_NaOH, h_pH;
    // for keeping old amounts
    vector<double> old_HCl, old_NaOH;
    double sum_min = 0.0;


    for (int i=1; i<sys->MPI+1; i++)
    {
        h_HCl.push_back(false);
        h_NaOH.push_back(false);
        h_pH.push_back(false);
        old_HCl.push_back(0);
        old_NaOH.push_back(0);
    }

    // loop trough all experiments
//    //#ifdef USE_MPI
//        omp_set_num_threads(sys->MPI);
//        #pragma omp parallel for
//    //#endif
    for (int i = 0; i<sys->experiments.size(); i++)
    {
        int j;
        int ICndx;
        int P_id = omp_get_thread_num();
//        cout << P_id << endl;
        h_HCl[P_id] = false; h_NaOH[P_id] = false;
        // loop torugh all comp to check for titrant

        h_pH[P_id] = false;

        // gets the index of aqeous phase
        for (j = 0; j<sys->experiments[i]->expphases.size(); ++j)
        {
            if (sys->experiments[i]->expphases[j]->phase == "aq_gen")
            {
                sys->PHndx[P_id] = j;
            }
        }

        // checks if pH is given in the data and gets the index of pH in phase property vector
        if (sys->PHndx[P_id] >= 0)
            for (j = 0; j<sys->experiments[i]->expphases[sys->PHndx[P_id]]->phprop.size(); ++j)
        {
                if (sys->experiments[i]->expphases[sys->PHndx[P_id]]->phprop[j]->property == "pH")
                {
                    h_pH[P_id] = true;
                    sys->PHPndx[P_id] = j;
                }
        }

        // checks what to adjusts: either HCl or NaOH
        for (j=0; j<sys->experiments[i]->sbcomp.size(); j++)
        {
            if (sys->experiments[i]->sbcomp[j]->comp == "HCl")
            {
                h_HCl[P_id] = true;
                old_HCl[P_id] = sys->experiments[i]->sbcomp[j]->Qnt;
                sys->COMPndx[P_id] = j;
            }
            if (sys->experiments[i]->sbcomp[j]->comp == "NaOH")
            {
                h_NaOH[P_id] = true;
                old_NaOH[P_id] = sys->experiments[i]->sbcomp[j]->Qnt;
                sys->COMPndx[P_id] = j;
            }
        }

        // get initial values of titration elements
        ICndx = sys->NodT[i]->IC_name_to_xDB("Na");
        sys->iNa[P_id] = sys->NodT[i]->Get_bIC(ICndx);
        ICndx = sys->NodT[i]->IC_name_to_xDB("O");
        sys->iO[P_id] = sys->NodT[i]->Get_bIC(ICndx);
        ICndx = sys->NodT[i]->IC_name_to_xDB("H");
        sys->iH[P_id] = sys->NodT[i]->Get_bIC(ICndx);
        ICndx = sys->NodT[i]->IC_name_to_xDB("Cl");
        sys->iCl[P_id] = sys->NodT[i]->Get_bIC(ICndx);

        //adjust HCl
        if (h_HCl[P_id] && h_pH[P_id])
        {
            nlopt::opt opt_HCL(nlopt::LN_BOBYQA, 1);

            std::vector<double> x;
            x.push_back( old_HCl[P_id]);
            double minf=0;
            sys->EXPndx[P_id]=i;

            vector<double> gr;

            titfunc(x, gr, sys);

            double pH_dif = abs (sys->experiments[i]->expphases[sys->PHndx[P_id]]->phprop[sys->PHPndx[P_id]]->Qnt - sys->NodT[i]->Get_pH());

            opt_HCL.set_lower_bounds(old_HCl[P_id]-0.9/**pH_dif*/*old_HCl[P_id]);
            opt_HCL.set_upper_bounds(old_HCl[P_id]+0.9/**pH_dif*/*old_HCl[P_id]);

            opt_HCL.set_min_objective(titfunc, sys);

            opt_HCL.set_xtol_rel(1e-3);

            sys->EXPndx[P_id]=i;
            nlopt::result result = opt_HCL.optimize(x, minf);
            sys->experiments[sys->EXPndx[P_id]]->sbcomp[sys->COMPndx[P_id]]->Qnt = x[0];

            sum_min += minf;
        }

        //adjust NaOH
        if (h_NaOH[P_id]&& h_pH[P_id])
        {
            nlopt::opt opt_NaOH(nlopt::LN_BOBYQA, 1);

            std::vector<double> x;
            x.push_back( old_NaOH[P_id]);
            double minf=0;
            sys->EXPndx[P_id]=i;

            vector<double> gr;

            titfunc(x, gr, sys);

            double pH_dif = abs (sys->experiments[i]->expphases[sys->PHndx[P_id]]->phprop[sys->PHPndx[P_id]]->Qnt - sys->NodT[i]->Get_pH());

            opt_NaOH.set_lower_bounds(old_NaOH[P_id]-0.9/**pH_dif*/*old_NaOH[P_id]);
            opt_NaOH.set_upper_bounds(old_NaOH[P_id]+0.9/**pH_dif*/*old_NaOH[P_id]);

            opt_NaOH.set_min_objective(titfunc, sys);

            opt_NaOH.set_xtol_rel(1e-3);

            sys->EXPndx[P_id]=i;
            nlopt::result result = opt_NaOH.optimize(x, minf);
            sys->experiments[sys->EXPndx[P_id]]->sbcomp[sys->COMPndx[P_id]]->Qnt = x[0];

            sum_min += minf;

        }
        h_HCl[P_id] = false; h_NaOH[P_id] = false;
    }

//    cout << sum_min << endl;


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
    int P_id = omp_get_thread_num();

    DATACH* dCH = sys->NodT[sys->COMPndx[P_id]]->pCSD();

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
        new_moles_IC[i] = sys->NodT[sys->EXPndx[P_id]]->Get_bIC(i);
    }

//    P_pa = 100000 * sys->experiments[sys->EXPndx]->sP;
//    T_k = 273.15 + sys->experiments[sys->EXPndx]->sT;


    dif = sys->experiments[sys->EXPndx[P_id]]->sbcomp[sys->COMPndx[P_id]]->Qnt - x[0];

    // corrects NaOH
    if (sys->experiments[sys->EXPndx[P_id]]->sbcomp[sys->COMPndx[P_id]]->comp == "NaOH")
    {
//        if (experiments[n]->sbcomp[j]->Qunit == keys::molal)
//        {
//            experiments[n]->sbcomp[j]->Qnt = experiments[n]->sbcomp[j]->Qnt*h2o_kgamount*39.99710928;
//            experiments[n]->sbcomp[j]->Qunit = keys::gram;
//        }
        ICndx = sys->NodT[sys->EXPndx[P_id]]->IC_name_to_xDB("Na");
        new_moles = sys->iNa[P_id] + 1*dif/39.99710928;
        sys->NodT[sys->EXPndx[P_id]]->Set_bIC(ICndx, new_moles);
        ICndx = sys->NodT[sys->EXPndx[P_id]]->IC_name_to_xDB("H");
        new_moles = sys->iH[P_id] + 1*dif/39.99710928;
        sys->NodT[sys->EXPndx[P_id]]->Set_bIC(ICndx, new_moles);
        ICndx = sys->NodT[sys->EXPndx[P_id]]->IC_name_to_xDB("O");
        new_moles = sys->iO[P_id] + 1*dif/39.99710928;
        sys->NodT[sys->EXPndx[P_id]]->Set_bIC(ICndx, new_moles);

    } else // corrects HCl
        if (sys->experiments[sys->EXPndx[P_id]]->sbcomp[sys->COMPndx[P_id]]->comp == "HCl")
        {
//                        if (experiments[n]->sbcomp[j]->Qunit == keys::molal)
//                        {
//                            experiments[n]->sbcomp[j]->Qnt = experiments[n]->sbcomp[j]->Qnt*h2o_kgamount*36.4611;
//                            experiments[n]->sbcomp[j]->Qunit = keys::gram;
//                        }
            ICndx =sys->NodT[sys->EXPndx[P_id]]->IC_name_to_xDB("H");
            new_moles = sys->iH[P_id] + 1*dif/36.4611;
            sys->NodT[sys->EXPndx[P_id]]->Set_bIC(ICndx, new_moles);
            ICndx = sys->NodT[sys->EXPndx[P_id]]->IC_name_to_xDB("Cl");
            double old_moles = sys->NodT[sys->EXPndx[P_id]]->Get_bIC(ICndx);
            new_moles = sys->iCl[P_id] + 1*dif/36.4611;
            sys->NodT[sys->EXPndx[P_id]]->Set_bIC(ICndx, new_moles);

        }

//    sys->NodT[sys->EXPndx]->GEM_from_MT( NodeHandle, NodeStatusCH, T_k, P_pa, new_moles_IC, xDC_up, xDC_lo );

    sys->NodT[sys->EXPndx[P_id]]->Set_TK(273.15 + sys->experiments[sys->EXPndx[P_id]]->sT);
    sys->NodT[sys->EXPndx[P_id]]->Set_P(100000 * sys->experiments[sys->EXPndx[P_id]]->sP);


    // calc equilibirum
    vector<DATABR*> dBR;
    dBR.push_back(sys->NodT[sys->EXPndx[P_id]]->pCNode());

    // Asking GEM to run with automatic initial approximation
    dBR.at(0)->NodeStatusCH = NEED_GEM_AIA;

    // RUN GEMS3K
    NodeStatusCH = sys->NodT[sys->EXPndx[P_id]]->GEM_run( false );

    if( NodeStatusCH == OK_GEM_AIA || NodeStatusCH == OK_GEM_SIA  )
    {
//            sys->NodT[i]->GEM_print_ipm( "GEMS3K_log.out" );   // possible debugging printout
    }
    else
    {
        // possible return status analysis, error message
//            sys->NodT[i]->GEM_print_ipm( "GEMS3K_log.out" );   // possible debugging printout
        cout<<"For experiment titration "<<sys->EXPndx[P_id]+1<< endl;
        cout<<" GEMS3K did not converge properly !!!! continuing anyway ... "<<endl;
    }


    // calc residual
    double meas_pH = sys->experiments[sys->EXPndx[P_id]]->expphases[sys->PHndx[P_id]]->phprop[sys->PHPndx[P_id]]->Qnt;
    double calc_pH = sys->NodT[sys->EXPndx[P_id]]->Get_pH();

    residual = abs (sys->experiments[sys->EXPndx[P_id]]->expphases[sys->PHndx[P_id]]->phprop[sys->PHPndx[P_id]]->Qnt - sys->NodT[sys->EXPndx[P_id]]->Get_pH());

    return residual;
}


void titrationG(TGfitTask *sys)
{
    vector<bool> h_HCl, h_NaOH, h_pH;
    vector<double> old_HCl, old_NaOH;
    double sum_min = 0.0;

    for (int i=1; i<sys->MPI+1; i++)
    {
        h_HCl.push_back(false);
        h_NaOH.push_back(false);
        h_pH.push_back(false);
        old_HCl.push_back(0);
        old_NaOH.push_back(0);
    }

    // loop trough all experiments
//    //#ifdef USE_MPI
//        omp_set_num_threads(sys->MPI);
//        #pragma omp parallel for
//    //#endif
    for (int i = 0; i<sys->experiments.size(); i++)
    {
        int j;
        int P_id = omp_get_thread_num();
//        cout << P_id << endl;
        h_HCl[P_id] = false; h_NaOH[P_id] = false;
        // loop torugh all comp to check for titrant

        h_pH[P_id] = false;

        for (j = 0; j<sys->experiments[i]->expphases.size(); ++j)
        {
            if (sys->experiments[i]->expphases[j]->phase == "aq_gen")
                sys->PHndx[P_id] = j;
        }

        if (sys->PHndx[P_id] > 0)
            for (j = 0; j<sys->experiments[i]->expphases[sys->PHndx[P_id]]->phprop.size(); ++j)
        {
                if (sys->experiments[i]->expphases[sys->PHndx[P_id]]->phprop[j]->property == "pH")
                    h_pH[P_id] = true;
        }


        for (j=0; j<sys->experiments[i]->sbcomp.size(); j++)
        {
            if (sys->experiments[i]->sbcomp[j]->comp == "HCl")
            {
                h_HCl[P_id] = true;
                old_HCl[P_id] = sys->experiments[i]->sbcomp[j]->Qnt;
                sys->COMPndx[P_id] = j;
            }
            if (sys->experiments[i]->sbcomp[j]->comp == "NaOH")
            {
                h_NaOH[P_id] = true;
                old_NaOH[P_id] = sys->experiments[i]->sbcomp[j]->Qnt;
                sys->COMPndx[P_id] = j;
            }
        }

//        for (j=0; j<sys->experiments[i]->expphases[0]->phprop.size(); ++j)
//        {
//            if (j<sys->experiments[i]->expphases[0]->phprop[j])
//        }
        //adjust HCl
        if (h_HCl[P_id] && h_pH[P_id])
        {
            std::vector<double> x;
            x.push_back( old_HCl[P_id]);

            sys->EXPndx[P_id]=i;

            vector<double> gr;

            titfunc(x, gr, sys);

            double pH_dif = abs (sys->experiments[i]->expphases[0]->phprop[0]->Qnt - sys->NodT[i]->Get_pH());

            double c=min(sys->experiments[i]->expphases[0]->phprop[0]->Qnt, sys->NodT[i]->Get_pH());
            double d=max(sys->experiments[i]->expphases[0]->phprop[0]->Qnt, sys->NodT[i]->Get_pH());

            x[0] = sys->experiments[sys->EXPndx[P_id]]->sbcomp[sys->COMPndx[P_id]]->Qnt;

            x[0] = golden(x[0]+x[0]/2,x[0]-x[0]/2, c, d, sys->EXPndx[P_id], sys);

            pH_dif = abs (sys->experiments[i]->expphases[0]->phprop[0]->Qnt - sys->NodT[i]->Get_pH());

            sys->EXPndx[P_id]=i;

            sys->experiments[sys->EXPndx[P_id]]->sbcomp[sys->COMPndx[P_id]]->Qnt = x[0];

        }

        //adjust NaOH
        if (h_NaOH[P_id]&& h_pH[P_id])
        {

            std::vector<double> x;
            x.push_back( old_NaOH[P_id]);

            sys->EXPndx[P_id]=i;

            vector<double> gr;

            titfunc(x, gr, sys);

            double pH_dif = abs (sys->experiments[i]->expphases[0]->phprop[0]->Qnt - sys->NodT[i]->Get_pH());

            double c=min(sys->experiments[i]->expphases[0]->phprop[0]->Qnt, sys->NodT[i]->Get_pH());
            double d=max(sys->experiments[i]->expphases[0]->phprop[0]->Qnt, sys->NodT[i]->Get_pH());

            x[0] = sys->experiments[sys->EXPndx[P_id]]->sbcomp[sys->COMPndx[P_id]]->Qnt;

            x[0] = golden(x[0]+x[0]/2,x[0]-x[0]/2, c, d, sys->EXPndx[P_id], sys);

            sys->EXPndx[P_id]=i;

            pH_dif = abs (sys->experiments[i]->expphases[0]->phprop[0]->Qnt - sys->NodT[i]->Get_pH());

            sys->experiments[sys->EXPndx[P_id]]->sbcomp[sys->COMPndx[P_id]]->Qnt = x[0];

        }
        h_HCl[P_id] = false; h_NaOH[P_id] = false; h_pH[P_id] = false;
    }
}

double golden(double a, double b, double c, double d , int EXPndx, TGfitTask *sys)
{
    double tol = 0.0001;
    double tau = (sqrt(5)-1)/2;

    double x1 = a+(1-tau)*(b-a);
    double x2 = a+tau*(b-a);

    double f_x1 = get_pH(x1, EXPndx, sys);
    double f_x2 = get_pH(x2, EXPndx, sys);

    while (abs(c-d) > tol)
    {
        if (f_x1<f_x2)
        {
            b=x2;
            x2=x1;
            x1=a+(1-tau)*(b-a);

            f_x1 = get_pH(x1, EXPndx, sys);
            f_x2 = get_pH(x2, EXPndx, sys);
        } else
        {
            a=x1;
            x1=x2;
            x2=a+tau*(b-a);

            f_x1 = get_pH(x1, EXPndx, sys);
            f_x2 = get_pH(x2, EXPndx, sys);
        }
    }

    if (f_x1<f_x2)
    {
        get_pH(x1, EXPndx, sys);
        return x1;
    }
    else
    {
        get_pH(x2, EXPndx, sys);
        return x2;
    }
}

double get_pH(double x, int EXPndx, TGfitTask *sys)
{

    vector<double> gr, x2;
    x2.push_back(x);
    titfunc(x2, gr, sys);

    return sys->NodT[EXPndx]->Get_pH();

}


