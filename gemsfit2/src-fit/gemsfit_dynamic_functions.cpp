#include "gemsfit_dynamic_functions.h"


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
                sys->COMPndx = j;
            }
            if (sys->experiments[i]->sbcomp[j]->comp == "NaOH")
            {
                h_NaOH = true;
                old_NaOH = sys->experiments[i]->sbcomp[j]->Qnt;
                sys->COMPndx = j;
            }
        }
        //adjust HCl
        if (h_HCl)
        {
            nlopt::opt opt_HCL(nlopt::LN_COBYLA, 1);

            std::vector<double> x;
            x.push_back( old_HCl);
            double minf;
            sys->EXPndx=i;

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
            sys->EXPndx=i;
            nlopt::result result = opt_HCL.optimize(x, minf);
            sys->experiments[sys->EXPndx]->sbcomp[sys->COMPndx]->Qnt = x[0];

//            cout << "happy"<< endl;

        }

        //adjust NaOH
        if (h_NaOH)
        {
            nlopt::opt opt_NaOH(nlopt::LN_BOBYQA, 1);

            std::vector<double> x;
            x.push_back( old_NaOH);
            double minf;
            sys->EXPndx=i;

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
            sys->EXPndx=i;
            nlopt::result result = opt_NaOH.optimize(x, minf);
            sys->experiments[sys->EXPndx]->sbcomp[sys->COMPndx]->Qnt = x[0];
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
    DATACH* dCH = sys->NodT[sys->COMPndx]->pCSD();

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
        new_moles_IC[i] = sys->NodT[sys->EXPndx]->Get_bIC(i);
    }

//    P_pa = 100000 * sys->experiments[sys->EXPndx]->sP;
//    T_k = 273.15 + sys->experiments[sys->EXPndx]->sT;


    dif = sys->experiments[sys->EXPndx]->sbcomp[sys->COMPndx]->Qnt - x[0];

    if (sys->experiments[sys->EXPndx]->sbcomp[sys->COMPndx]->comp == "NaOH")
    {
//        if (experiments[n]->sbcomp[j]->Qunit == keys::molal)
//        {
//            experiments[n]->sbcomp[j]->Qnt = experiments[n]->sbcomp[j]->Qnt*h2o_kgamount*39.99710928;
//            experiments[n]->sbcomp[j]->Qunit = keys::gram;
//        }
        ICndx = sys->NodT[sys->EXPndx]->IC_name_to_xDB("Na");
        new_moles = sys->NodT[sys->EXPndx]->Get_bIC(ICndx) + 1*dif/39.99710928;
        sys->NodT[sys->EXPndx]->Set_bIC(ICndx, new_moles);
        ICndx = sys->NodT[sys->EXPndx]->IC_name_to_xDB("H");
        new_moles = sys->NodT[sys->EXPndx]->Get_bIC(ICndx) + 1*dif/39.99710928;
        sys->NodT[sys->EXPndx]->Set_bIC(ICndx, new_moles);
        ICndx = sys->NodT[sys->EXPndx]->IC_name_to_xDB("O");
        new_moles = sys->NodT[sys->EXPndx]->Get_bIC(ICndx) + 1*dif/39.99710928;
        sys->NodT[sys->EXPndx]->Set_bIC(ICndx, new_moles);

    } else
        if (sys->experiments[sys->EXPndx]->sbcomp[sys->COMPndx]->comp == "HCl")
        {
//                        if (experiments[n]->sbcomp[j]->Qunit == keys::molal)
//                        {
//                            experiments[n]->sbcomp[j]->Qnt = experiments[n]->sbcomp[j]->Qnt*h2o_kgamount*36.4611;
//                            experiments[n]->sbcomp[j]->Qunit = keys::gram;
//                        }
            ICndx =sys->NodT[sys->EXPndx]->IC_name_to_xDB("H");
            new_moles = sys->NodT[sys->EXPndx]->Get_bIC(ICndx) + 1*dif/36.4611;
            sys->NodT[sys->EXPndx]->Set_bIC(ICndx, new_moles);
            ICndx = sys->NodT[sys->EXPndx]->IC_name_to_xDB("Cl");
            double old_moles = sys->NodT[sys->EXPndx]->Get_bIC(ICndx);
            new_moles = sys->NodT[sys->EXPndx]->Get_bIC(ICndx) + 1*dif/36.4611;
            sys->NodT[sys->EXPndx]->Set_bIC(ICndx, new_moles);

        }

//    sys->NodT[sys->EXPndx]->GEM_from_MT( NodeHandle, NodeStatusCH, T_k, P_pa, new_moles_IC, xDC_up, xDC_lo );

    sys->NodT[sys->EXPndx]->Set_TK(273.15 + sys->experiments[sys->EXPndx]->sT);
    sys->NodT[sys->EXPndx]->Set_P(100000 * sys->experiments[sys->EXPndx]->sP);


    // calc equilibirum
    vector<DATABR*> dBR;
    dBR.push_back(sys->NodT[sys->EXPndx]->pCNode());

    // Asking GEM to run with automatic initial approximation
    dBR.at(0)->NodeStatusCH = NEED_GEM_AIA;

    // RUN GEMS3K
    NodeStatusCH = sys->NodT[sys->EXPndx]->GEM_run( false );

    if( NodeStatusCH == OK_GEM_AIA || NodeStatusCH == OK_GEM_SIA  )
    {
//            sys->NodT[i]->GEM_print_ipm( "GEMS3K_log.out" );   // possible debugging printout
    }
    else
    {
        // possible return status analysis, error message
//            sys->NodT[i]->GEM_print_ipm( "GEMS3K_log.out" );   // possible debugging printout
        cout<<"For experiment titration "<<sys->EXPndx+1<< endl;
        cout<<" GEMS3K did not converge properly !!!! continuing anyway ... "<<endl;
    }


    // calc residual
    double meas_pH = sys->experiments[sys->EXPndx]->expphases[0]->phprop[0]->Qnt;
    double calc_pH = sys->NodT[sys->EXPndx]->Get_pH();

    residual = abs (sys->experiments[sys->EXPndx]->expphases[0]->phprop[0]->Qnt - sys->NodT[sys->EXPndx]->Get_pH());

    return residual;
}


