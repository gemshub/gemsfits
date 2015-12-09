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
 *	@file gemsfit_nested_functions.cpp
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


#include "gemsfit_nested_functions.h"
#include "gemsfit_target_functions.h"

void nestedfun (TGfitTask *sys)
{
    // loop over the nested functions
        #ifdef useomp
            omp_set_num_threads(sys->MPI);
            #pragma omp parallel for schedule(dynamic)
        #endif
    for  (unsigned int i = 0; i<sys->experiments.size(); i++)
    {
        vector <double> x, UB, LB;
        int Pndx = -1, Fndx = -1; double Pval = 0.0, Ub = 0.0, Lb = 0.0;
        int bounds = 0;

        for (unsigned int j = 0; j<sys->Tfun->nestfun.size(); j++)
        {
//            string param_type = sys->Tfun->nestfun[j].Ptype;
            int P_id = omp_get_thread_num();
            int count = 0;
            double test_residual = sys->get_residual (i, sys->aTfun[i].nestfun[j], count);
            if (test_residual != 0.0)
            {
                sys->EXPndx[P_id]=i;
                sys->NEFndx[P_id]=j;

                for (unsigned e = 0; e < sys->Opti->optNFParam.size(); e++) // loops trough optNFParam
                {
                    for (unsigned p = 0; p < sys->Opti->optNFParam[e]->Get_optFPsize(); p++ )
                    {
                        sys->Opti->optNFParam[e]->Get_Fparam(p, i, Fndx, Pndx, Pval, Ub, Lb);
                        if (Fndx == j) // checks if the nested function parameters point to the curent NFUN with the Fndx
                        {
                            if (((sys->Tfun->nestfun[sys->NEFndx[P_id]].exp_CN == "pH") || (sys->Tfun->nestfun[sys->NEFndx[P_id]].exp_CN == "pHm") )&& (sys->Tfun->nestfun[j].Telem.size() > 0))
                            {
//                                for (unsigned int bi=0; bi<sizeof(sys->bICv[sys->EXPndx[P_id]]); bi++)
//                                {
//                                    sys->NodT[i]->Set_bIC( bi, sys->bICv[i][bi] );
//                                }
//                                // RUN GEMS3K to rested the bulk composition to initial values
//                                sys->experiments[sys->EXPndx[P_id]]->sT = sys->NodT[sys->EXPndx[P_id]]->Get_TK() - 273.15;
//                                if (sys->experiments[sys->EXPndx[P_id]]->sP > 0)
//                                sys->experiments[sys->EXPndx[P_id]]->sP = sys->NodT[sys->EXPndx[P_id]]->Get_P() / 100000;

//                                //    cout << sys->NodT[sys->EXPndx[P_id]]->Get_P() / 100000 << endl;
//                                // claculate equilibrium
//                                sys->NodT[sys->EXPndx[P_id]]->Set_TK(273.15 + sys->experiments[sys->EXPndx[P_id]]->sT);
//                                sys->NodT[sys->EXPndx[P_id]]->Set_P(100000 * sys->experiments[sys->EXPndx[P_id]]->sP);

//                                vector<DATABR*> dBR;
//                                dBR.push_back(sys->NodT[sys->EXPndx[P_id]]->pCNode());

//                                // Asking GEM to run with automatic initial approximation
//                                dBR.at(0)->NodeStatusCH = NEED_GEM_AIA;

//                                // RUN GEMS3K
//                                sys->NodT[sys->EXPndx[P_id]]->GEM_run( false );

                                if (isTitration(sys, i, j, Pndx))
                                {
                                    x.push_back(Pval);
                                    UB.push_back(Ub);
                                    LB.push_back(Lb);
                                    sys->vPAndx[P_id]->ndx.push_back(p);
                                    sys->vEAndx[P_id]->ndx.push_back(e);
                                    bounds = 1;
                                }
                            } else
                            {
                                if (sys->Tfun->nestfun[sys->NEFndx[P_id]].exp_DCP == "PpG" )  bounds = 1;
                                x.push_back(Pval);
                                UB.push_back(Ub);
                                LB.push_back(Lb);
                                sys->vPAndx[P_id]->ndx.push_back(p);
                                sys->vEAndx[P_id]->ndx.push_back(e);
                            }
                        }
                    }
                }

                nlopt::opt opt(nlopt::LN_BOBYQA, x.size());

                double minf=0;

                opt.set_lower_bounds(LB);
                opt.set_upper_bounds(UB);

                opt.set_min_objective(nestminfunc, sys);

                if (bounds == 0)
                {
                opt.set_xtol_rel(1e-10); // take out these settings into the NFUN of the task file
                opt.set_xtol_abs(1e-10);}
                if (bounds == 1)
                {
                opt.set_stopval(0.0001);
                opt.set_maxeval( 1000 );}

//                vector<double> inistep( x.size(), 0. );
//                for( int xx=0; xx<(int) x.size(); xx++ )
//                {
//                    inistep[xx] = x[xx] * 0.00001;
//                }
//                opt.set_initial_step( inistep );

//                cout << "before: "<< sys->experiments[sys->EXPndx[P_id]]->sample << endl;
                if (x.size() == 0 )
                {
                    cout << "Error setting the NFUN parameters! " << endl; exit(1);
                }

                try
                {
                nlopt::result result = opt.optimize(x, minf);
//                cout << "Nested: " << i << " " << sys->aTfun[sys->EXPndx[P_id]].nestfun[sys->NEFndx[P_id]].count << endl;
                sys->aTfun[sys->EXPndx[P_id]].nestfun[sys->NEFndx[P_id]].count = 0;
                }
                catch (nlopt::roundoff_limited &rf )
                                        {
//                                            cout << "Error in NESTED "<< endl;
                                        }

//                double xx = minf;

                // Store result parameters
    //            sys->experiments[sys->EXPndx[P_id]]->sbcomp[sys->COMPndx[P_id]]->Qnt = x[0];
                vector<double> grad;
                nestminfunc ( x, grad, sys );

                for (unsigned int k = 0; k < sys->vPAndx[P_id]->ndx.size(); k++)
                {
//                    sys->Opti->optNFP.e_opt[sys->vPAndx[P_id]->ndx[k]]->val[sys->EXPndx[P_id]] = x[k];
                    sys->Opti->optNFParam[sys->vEAndx[P_id]->ndx[k]]->Set_Fparam(sys->vPAndx[P_id]->ndx[k], i, x[k]);
                }

                x.clear(); UB.clear(); LB.clear(); sys->PAndx.clear(); sys->vPAndx[P_id]->ndx.clear();
                sys->vEAndx[P_id]->ndx.clear();

//                cout << "after: "<<i << endl;
            }
        }
    }
}

double nestminfunc ( const std::vector<double> &opt, std::vector<double> &grad, void *obj_func_data )
{
    TGfitTask *sys = reinterpret_cast<TGfitTask*>(obj_func_data);
    double residual = 0.0;
    int P_id = omp_get_thread_num();
    long int NodeStatusCH;
//    bool notP = true, notT=true;
    int op = 0;

    /// error think about a way to store the indexes of the nfun parameters

    for (unsigned e = 0; e < sys->vEAndx[P_id]->ndx.size(); e++) // loops trough OptParameter vector
    {
        for (unsigned j = 0; j < sys->Opti->optNFParam[e]->Get_optFPsize(); j++) // loops torugh optPF vector
        {
            if ((sys->vPAndx[P_id]->ndx[op] == j) && (sys->vEAndx[P_id]->ndx[e] == e))
            {
                sys->Opti->optNFParam[sys->vEAndx[P_id]->ndx[e]]->Set_Fparam(sys->vPAndx[P_id]->ndx[op], sys->EXPndx[P_id], opt[op]);
                op++;
            }
        }
    }

    op = 0;
    for (unsigned e = 0; e < sys->vEAndx[P_id]->ndx.size(); e++) // loops trough OptParameter vector
    {
        for (unsigned j = 0; j < sys->Opti->optNFParam[e]->Get_optFPsize(); j++) // loops torugh optPF vector
        {
            if ((sys->vPAndx[P_id]->ndx[op] == j) && (sys->vEAndx[P_id]->ndx[e] == e))
            {
                int Pindex;
                Pindex = sys->Opti->optNFParam[sys->vEAndx[P_id]->ndx[e]]->Get_FPndx(sys->vPAndx[P_id]->ndx[op] );
                sys->Opti->optNFParam[sys->vEAndx[P_id]->ndx[e]]->Adjust_Fparam(sys->NodT[sys->EXPndx[P_id]], Pindex, opt[op]);
                op++;
            }
        }
    }

    for (unsigned e = 0; e < sys->vEAndx[P_id]->ndx.size(); e++) // loops trough OptParameter vector
    {
    sys->Opti->optNFParam[sys->vEAndx[P_id]->ndx[e]]->Adjust_Lparam(sys->NodT[sys->EXPndx[P_id]], sys->EXPndx[P_id] );
    }


    sys->experiments[sys->EXPndx[P_id]]->sT = sys->NodT[sys->EXPndx[P_id]]->Get_TK() - 273.15;
    if (sys->experiments[sys->EXPndx[P_id]]->sP > 0)
    sys->experiments[sys->EXPndx[P_id]]->sP = sys->NodT[sys->EXPndx[P_id]]->Get_P() / 100000;

//    cout << sys->NodT[sys->EXPndx[P_id]]->Get_P() / 100000 << endl;
    // claculate equilibrium
    sys->NodT[sys->EXPndx[P_id]]->Set_TK(273.15 + sys->experiments[sys->EXPndx[P_id]]->sT);
    sys->NodT[sys->EXPndx[P_id]]->Set_P(100000 * sys->experiments[sys->EXPndx[P_id]]->sP);

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
        cout<<"For experiment nested sample "<<sys->experiments[sys->EXPndx[P_id]]->sample <<" dataset "<<sys->experiments[sys->EXPndx[P_id]]->expdataset<< endl;
        cout<<" GEMS3K did not converge properly !!!! continuing anyway ... "<<endl;
    }

//    double meas_pH = sys->experiments[sys->EXPndx[P_id]]->expphases[0]->phprop[0]->Qnt;
//    double calc_pH = sys->NodT[sys->EXPndx[P_id]]->Get_pH();

//    residual = abs (sys->experiments[sys->EXPndx[P_id]]->expphases[0]->phprop[0]->Qnt - sys->NodT[sys->EXPndx[P_id]]->Get_pH());

    // calculate residual
    int count = 0;
    sys->aTfun[sys->EXPndx[P_id]].nestfun[sys->NEFndx[P_id]].count++;
    residual = sys->get_residual (sys->EXPndx[P_id], sys->aTfun[sys->EXPndx[P_id]].nestfun[sys->NEFndx[P_id]], count);

    return residual;
}

bool isTitration (TGfitTask *sys, int i, int j, int p)
{
//    int P_id = omp_get_thread_num();
        // check titrant in experiment
    double residual = 0.0;
    int P_id = omp_get_thread_num();

//    for (unsigned int t=0; t<sys->Tfun->nestfun[j].Tformula.size(); t++)
//    {
//        for (unsigned int c=0; c<sys->experiments[i]->sbcomp.size(); c++)
//        {
//            if (sys->experiments[i]->sbcomp[c]->comp == sys->Tfun->nestfun[j].Tformula[t])
//            {
//                int Endx = sys->NodT[i]->IC_name_to_xDB(sys->Tfun->nestfun[j].Telem[t].c_str());
//                if (Endx == p)
//                {
//                    return true;
//                } else return false;
//            }
//        }
//    }

//    int count = 0;

    int Endx = sys->NodT[i]->IC_name_to_xDB(sys->Tfun->nestfun[j].Telem[0].c_str());
    if ((Endx == p) && (sys->aTfun[sys->EXPndx[P_id]].nestfun[sys->NEFndx[P_id]].Helem[0] == 1))
    {
        return true;
    }

    Endx = sys->NodT[i]->IC_name_to_xDB(sys->Tfun->nestfun[j].Telem[1].c_str());
    if ((Endx == p) && (sys->aTfun[sys->EXPndx[P_id]].nestfun[sys->NEFndx[P_id]].Helem[1] == 1))
    {
        return true;
    }




//    if (sys->EXPndx[P_id] == 2)
//    {
//        int xEndx = sys->NodT[i]->IC_name_to_xDB(sys->Tfun->nestfun[j].Telem[0].c_str());
//        if (xEndx == p)
//        {
//           cout << "here" << endl;
//           cout << sys->NodT[i]->Get_bIC(xEndx);
//        }
//    }

//    sys->aTfun[sys->EXPndx[P_id]].type = "dif";
//    residual = sys->get_residual (sys->EXPndx[P_id], sys->aTfun[sys->EXPndx[P_id]].nestfun[sys->NEFndx[P_id]], count);
//    sys->aTfun[sys->EXPndx[P_id]].type = "abs_dif";

//    //Base
//    if (residual < 0.0)
//    {
//        int Endx = sys->NodT[i]->IC_name_to_xDB(sys->Tfun->nestfun[j].Telem[0].c_str());
//        if (Endx == p)
//        {
//            return true;
//        } else return false;
//    }

//    // acid
//    if (residual > 0.0)
//    {
//        int Endx = sys->NodT[i]->IC_name_to_xDB(sys->Tfun->nestfun[j].Telem[1].c_str());
//        if (Endx == p)
//        {
//            return true;
//        } else return false;
//    }

    return false;
}


