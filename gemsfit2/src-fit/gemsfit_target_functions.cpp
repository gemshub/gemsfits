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
 *	@file gemsfit_target_functions.cpp
 *
 *	@brief this header file contains implementetions of target functions needed during optimization.
 *  Also other functions needed for setting the new adjuste paramaters are implemented here
 *
 *	@author G. Dan Miron
 *
 * 	@date 17.05.2013
 *
 */



#include "gemsfit_target_functions.h"
#include "keywords.h"


/// Adust FUNCTIONS
void adjust_G0 (int i, double G0, TGfitTask *sys)
{

    double new_G0=G0;
    double new_GTP=0.0;
    double delta_G0old_G0new;
    int species_index = sys->Opti->Pindex[i];
    // going trough all nodes
    for (int n=0; n<sys->NodT.size(); ++n)
    {
        delta_G0old_G0new = fabs(sys->NodT[n]->DC_G0(species_index, 1e+05, 298.15, false)) - fabs(new_G0);
        // going trough all TP pairs
        for (int j=0; j<sys->TP_pairs[0].size(); ++j)
        {
            new_GTP = delta_G0old_G0new + sys->NodT[n]->DC_G0(species_index, sys->TP_pairs[1][j]*100000, sys->TP_pairs[0][j]+273.15, false);
            // Set the new G0 in GEMS
            sys->NodT[n]->Set_DC_G0(species_index, sys->TP_pairs[1][j]*100000, sys->TP_pairs[0][j]+273.15, new_GTP);
        }
        sys->NodT[n]->Set_DC_G0(species_index, 1e+05, 298.15, G0);
    }
//    cout << sys->NodT[0]->DC_G0(species_index, 1*100000, 25+273.15, false)  << endl;
}

void adjust_PMc (int i, double new_PMc, TGfitTask *sys)
{
    int index_PMc = sys->Opti->Pindex[i];
    for (int n=0; n<sys->NodT.size(); ++n)
    {
        sys->NodT[n]->Set_PMc(new_PMc, index_PMc );
    }
}

void adjust_DMc (int i, double new_DMc, TGfitTask *sys)
{
    int index_DMc = sys->Opti->Pindex[i];
    for (int n=0; n<sys->NodT.size(); ++n)
    {
        sys->NodT[n]->Set_DMc(new_DMc, index_DMc );
    }
}

void adjust_bIC (int i, double new_bIC, TGfitTask *sys)
{
    int index_bIC = sys->Opti->Pindex[i];
    for (int n=0; n<sys->NodT.size(); ++n)
    {
        sys->NodT[n]->Set_bIC(index_bIC, new_bIC );
    }
}

void adjust_TK (int i, double new_TK, TGfitTask *sys)
{
    for (int n=0; n<sys->NodT.size(); ++n)
    {
        sys->NodT[n]->Set_TK(new_TK );
        sys->NodT[n]->Set_mLook(0); // activate interpolation of themrodynamic properties lookup array
    }
}

void adjust_P (int i, double new_P, TGfitTask *sys)
{
    for (int n=0; n<sys->NodT.size(); ++n)
    {
        sys->NodT[n]->Set_TK(new_P );
        sys->NodT[n]->Set_mLook(0); // activate interpolation of themrodynamic properties lookup array
    }
}

void adjust_RDc (TGfitTask *sys)
{
    // going trough all nodes
//#ifdef USE_MPI
    omp_set_num_threads(sys->MPI);
    #pragma omp parallel for
//#endif
    for (int n=0; n<sys->NodT.size(); ++n)
    {
        for (int i=0; i < sys->Opti->reactions.size(); ++i )
        {
            double new_G0=0;
            double delta_G=0;
            double R=8.314472;
            double delta_G0old_G0new;
            int species_index = sys->Opti->reactions[i]->rdc_species_ind[sys->Opti->reactions[i]->rdc_species_ind.size()-1];

            // for standard sate at 25 C and 1 bar
            for (int j=0; j < sys->Opti->reactions[i]->rdc_species.size()-1; ++j ) // calculates DG without the last species which is the constrained one
            {
                delta_G += sys->Opti->reactions[i]->rdc_species_coef[j] * sys->NodT[n]->DC_G0(sys->Opti->reactions[i]->rdc_species_ind[j], 1e+05, 298.15, false);
            }

            new_G0 = (-R*298.15*2.302585093*sys->Opti->reactions[i]->logK) - delta_G;
            sys->Opti->reactions[i]->std_gibbs = new_G0;
//             put absolute - check if correct
            // M1
            delta_G0old_G0new = fabs(sys->NodT[n]->DC_G0(species_index, 1e+05, 298.15, false)) - fabs(new_G0);
            // M1
            sys->NodT[n]->Set_DC_G0(species_index,1*100000, 25+273.15, new_G0);

            // for all TP pairs
            for (int j=0; j<sys->TP_pairs[0].size(); j++) // loops trough all unique TP_pairs
            {
                // loop torugh reaction species except the last which is the dependent one
// M2
//                for (int k=0; k < sys->Opti->reactions[i]->rdc_species.size()-1; ++k ) // calculates DG without the last species which is the constrained one
//                 {
//                     delta_G += sys->Opti->reactions[i]->rdc_species_coef[k] * sys->NodT[n]->DC_G0(sys->Opti->reactions[i]->rdc_species_ind[k], sys->TP_pairs[1][j]*100000, sys->TP_pairs[0][j]+273.15, false);
//                 }
//                 new_G0 = (-R*sys->TP_pairs[0][j]+273.15*2.302585093*sys->Opti->reactions[i]->logK_TPpairs[j]) - delta_G;
//                 sys->NodT[n]->Set_DC_G0(species_index, sys->TP_pairs[1][j]*100000, sys->TP_pairs[0][j]+273.15, new_G0);
// M2
                 // M1
                new_G0 = delta_G0old_G0new + sys->NodT[n]->DC_G0(species_index, sys->TP_pairs[1][j]*100000, sys->TP_pairs[0][j]+273.15, false);
                // Set the new G0 in GEMS
                sys->NodT[n]->Set_DC_G0(species_index, sys->TP_pairs[1][j]*100000, sys->TP_pairs[0][j]+273.15, new_G0);
                 // M1
//                // cout << temp_v[j] << endl;
            }
        }
    }
}

void adjust_Lp (TGfitTask *sys)
{
    // going trough all nodes
//#ifdef USE_MPI
    omp_set_num_threads(sys->MPI);
    #pragma omp parallel for
//#endif
    for (int n=0; n<sys->NodT.size(); ++n)
    {
        for (int i=0; i < sys->Opti->Lparams.size(); ++i )
        {
            double new_param=0;
            int LP_index = sys->Opti->Lparams[i]->index;

            for (int j=0; j < sys->Opti->Lparams[i]->L_param.size()-1; ++j )
            {
                new_param += sys->Opti->Lparams[i]->L_coef[j] * sys->NodT[n]->Get_bIC(sys->Opti->Lparams[i]->L_param_ind[j]);
            }

            new_param += sys->NodT[n]->Get_bIC(LP_index);
            sys->NodT[n]->Set_bIC(LP_index, new_param);
            sys->Opti->Lparams[i]->EV = new_param;
        }
    }
}

/// Unit check FUNCTIONS
void check_unit(int i, int p, int e, string unit, TGfitTask *sys )
{
    if (sys->experiments[i]->expphases[p]->phcomp[e]->Qunit != unit)
    {
        if (unit == keys::loga)
        {
            // molal to log(molal)
            if (sys->experiments[i]->expphases[p]->phcomp[e]->Qunit == keys::molal)
            {
                double error_perc = sys->experiments[i]->expphases[p]->phcomp[e]->Qerror * 100 / sys->experiments[i]->expphases[p]->phcomp[e]->bQnt;
                sys->experiments[i]->expphases[p]->phcomp[e]->bQnt = log10(sys->experiments[i]->expphases[p]->phcomp[e]->bQnt);
                sys->experiments[i]->expphases[p]->phcomp[e]->Qerror = sys->experiments[i]->expphases[p]->phcomp[e]->bQnt * error_perc / 100;
                sys->experiments[i]->expphases[p]->phcomp[e]->Qunit = keys::loga;
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
        if (sys->experiments[i]->expphases[p]->phcomp[e]->Qunit != unit)
        {
            cout << "Unit for experiment: "<< i <<" from "<< sys->experiments[i]->expdataset << " is not implemented"<< endl;
            exit(1);
        }
    }
}

void check_prop_unit(int i, int p, int pp, string unit, TGfitTask *sys )
{
    if (sys->experiments[i]->expphases[p]->phprop[pp]->Qunit != unit)
    {
        if (unit == keys::_loga)
        {
            // molal to -log(molal)
            if (sys->experiments[i]->expphases[p]->phprop[pp]->Qunit == keys::molal)
            {
                double error_perc = sys->experiments[i]->expphases[p]->phprop[pp]->Qerror * 100 / sys->experiments[i]->expphases[p]->phprop[pp]->pQnt;
                sys->experiments[i]->expphases[p]->phprop[pp]->pQnt = -log10(sys->experiments[i]->expphases[p]->phprop[pp]->pQnt);
                sys->experiments[i]->expphases[p]->phprop[pp]->Qerror = sys->experiments[i]->expphases[p]->phprop[pp]->pQnt * error_perc / 100;
                sys->experiments[i]->expphases[p]->phprop[pp]->Qunit = keys::_loga;
            }
            else
            {
                cout << "Unit for experiment: "<< i <<" from "<< sys->experiments[i]->expdataset << " is not implemented"<< endl;
                exit(1);
            }
        } else
            if (unit == keys::molal)
            {
                // -log to molal
                if (sys->experiments[i]->expphases[p]->phprop[pp]->Qunit == keys::_loga)
                {
                    double error_perc = sys->experiments[i]->expphases[p]->phprop[pp]->Qerror * 100 / sys->experiments[i]->expphases[p]->phprop[pp]->pQnt;
                    sys->experiments[i]->expphases[p]->phprop[pp]->pQnt = pow(10,-(sys->experiments[i]->expphases[p]->phprop[pp]->pQnt));
                    sys->experiments[i]->expphases[p]->phprop[pp]->Qerror = sys->experiments[i]->expphases[p]->phprop[pp]->pQnt * error_perc / 100;
                    sys->experiments[i]->expphases[p]->phprop[pp]->Qunit = keys::molal;
                }
                else
                {
                    cout << "Unit for experiment: "<< i <<" from "<< sys->experiments[i]->expdataset << " is not implemented"<< endl;
                    exit(1);
                }
            } else
                if (unit == keys::kgram)
                {
                    // g to kg
                    // log to molal
                    if (sys->experiments[i]->expphases[p]->phprop[pp]->Qunit == keys::gram)
                    {
                        double error_perc = sys->experiments[i]->expphases[p]->phprop[pp]->Qerror * 100 / sys->experiments[i]->expphases[p]->phprop[pp]->pQnt;
                        sys->experiments[i]->expphases[p]->phprop[pp]->pQnt = sys->experiments[i]->expphases[p]->phprop[pp]->pQnt / 1000;
                        sys->experiments[i]->expphases[p]->phprop[pp]->Qerror = sys->experiments[i]->expphases[p]->phprop[pp]->pQnt * error_perc / 100;
                        sys->experiments[i]->expphases[p]->phprop[pp]->Qunit = keys::kgram;
                    }
                    else
                    {
                        cout << "Unit for experiment: "<< i <<" from "<< sys->experiments[i]->expdataset << " is not implemented"<< endl;
                        exit(1);
                    }
                } else
                    if (unit == keys::gram)
                    {
                        // kg to g
                        // log to molal
                        if (sys->experiments[i]->expphases[p]->phprop[pp]->Qunit == keys::kgram)
                        {
                            double error_perc = sys->experiments[i]->expphases[p]->phprop[pp]->Qerror * 100 / sys->experiments[i]->expphases[p]->phprop[pp]->pQnt;
                            sys->experiments[i]->expphases[p]->phprop[pp]->pQnt = sys->experiments[i]->expphases[p]->phprop[pp]->pQnt * 1000;
                            sys->experiments[i]->expphases[p]->phprop[pp]->Qerror = sys->experiments[i]->expphases[p]->phprop[pp]->pQnt * error_perc / 100;
                            sys->experiments[i]->expphases[p]->phprop[pp]->Qunit = keys::gram;
                        }
                        else
                        {
                            cout << "Unit for experiment: "<< i <<" from "<< sys->experiments[i]->expdataset << " is not implemented"<< endl;
                            exit(1);
                        }
                    } else
                        if (unit == keys::cm3)
                        {
                            // m3 to cm3
                            // log to molal
                            if (sys->experiments[i]->expphases[p]->phprop[pp]->Qunit == keys::m3)
                            {
                                double error_perc = sys->experiments[i]->expphases[p]->phprop[pp]->Qerror * 100 / sys->experiments[i]->expphases[p]->phprop[pp]->pQnt;
                                sys->experiments[i]->expphases[p]->phprop[pp]->pQnt = sys->experiments[i]->expphases[p]->phprop[pp]->pQnt * 1e06;
                                sys->experiments[i]->expphases[p]->phprop[pp]->Qerror = sys->experiments[i]->expphases[p]->phprop[pp]->pQnt * error_perc / 100;
                                sys->experiments[i]->expphases[p]->phprop[pp]->Qunit = keys::cm3;
                            }
                            else
                            {
                                cout << "Unit for experiment: "<< i <<" from "<< sys->experiments[i]->expdataset << " is not implemented"<< endl;
                                exit(1);
                            }
                        } else
                            if (unit == keys::m3)
                            {
                                // kg to g
                                // log to molal
                                if (sys->experiments[i]->expphases[p]->phprop[pp]->Qunit == keys::cm3)
                                {
                                    double error_perc = sys->experiments[i]->expphases[p]->phprop[pp]->Qerror * 100 / sys->experiments[i]->expphases[p]->phprop[pp]->pQnt;
                                    sys->experiments[i]->expphases[p]->phprop[pp]->pQnt = sys->experiments[i]->expphases[p]->phprop[pp]->pQnt / 1e06;
                                    sys->experiments[i]->expphases[p]->phprop[pp]->Qerror = sys->experiments[i]->expphases[p]->phprop[pp]->pQnt * error_perc / 100;
                                    sys->experiments[i]->expphases[p]->phprop[pp]->Qunit = keys::m3;
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
        if (sys->experiments[i]->expphases[p]->phprop[pp]->Qunit != unit)
        {
            cout << "Unit for experiment: "<< i <<" from "<< sys->experiments[i]->expdataset << " is not implemented"<< endl;
            exit(1);
        }
    }
}

/// Target functions, Tfun_residual calculations
double residual_phase_elem (int i, int p, int e, int j, TGfitTask *sys)
{
    const char *elem_name, *phase_name;
    int ICndx, PHndx, nIC;
    double computed_value, measured_value;
    double Tfun_residual = 0.0, Weighted_Tfun_residual = 0.0, weight_ = 1.0;
    DATACH* dCH = sys->NodT[i]->pCSD();
    double* IC_in_PH;

    elem_name =  sys->experiments[i]->expphases[p]->phcomp[e]->comp.c_str();
    ICndx = sys->NodT[i]->IC_name_to_xDB(elem_name);
    phase_name = sys->experiments[i]->expphases[p]->phase.c_str();
    PHndx = sys->NodT[i]->Ph_name_to_xDB(phase_name);

    nIC = dCH->nIC;	// nr of independent components
    IC_in_PH = new double[ nIC ];
    sys->NodT[i]->Ph_BC(PHndx, IC_in_PH);

    // Get composition of aqueous pahse
    if ((sys->experiments[i]->expphases[p]->phase == "aq_gen") && (sys->Tfun->objfun[j]->exp_phase == "aq_gen"))
    {
        if (sys->experiments[i]->expphases[p]->phcomp[e]->Qunit == keys::loga)
        {
        computed_value = log10(sys->NodT[i]->Get_mIC(ICndx));
        } else
        {
            computed_value = sys->NodT[i]->Get_mIC(ICndx);
        }
    } else // other than aqueous phase
        if ((sys->Tfun->objfun[j]->exp_phase != "aq_gen") && (sys->experiments[i]->expphases[p]->phase != "aq_gen"))
        {
            if ((sys->Tfun->objfun[j]->exp_unit == keys::Simolfrac) && (sys->experiments[i]->expphases[p]->phcomp[e]->Qunit == keys::Simolfrac))
            {
                int Sindx = sys->NodT[i]->IC_name_to_xDB("Si");
                computed_value = IC_in_PH[ICndx]/IC_in_PH[Sindx];
            } else // phase bulk composition in moles (mol)
            computed_value = IC_in_PH[ICndx];
        } else { cout << "Error in target functions line 293 "; exit(1);}

    measured_value = sys->experiments[i]->expphases[p]->phcomp[e]->bQnt;

    // Error handeling due to possible nonphisical parameters
    if (computed_value < sys->LimitOfDetection)
    {
//        cout << measured_value <<" / " <<computed_value<<" = " << measured_value / computed_value << endl;
        computed_value = rand() % 100 + 1;
    }

    // check Target function type and calculate the Tfun_residual
    weight_ = weight(i, p, e, j, sys->Tfun->weight, sys);
    Tfun_residual = Tfunction(computed_value, measured_value, sys->Tfun->type, *sys->Tfun->objfun[j]);
    Weighted_Tfun_residual = Tfunction(computed_value, measured_value, sys->Tfun->type, *sys->Tfun->objfun[j])*weight_;

    sys->set_residuals(computed_value, measured_value, Weighted_Tfun_residual, Tfun_residual, weight_);

    delete[] IC_in_PH;

    sys->print->set_print(sys->experiments[i]->sample,sys->experiments[i]->expphases[p]->phase,sys->experiments[i]->expphases[p]->phcomp[e]->comp,sys->experiments[i]->expphases[p]->phcomp[e]->Qunit,measured_value,computed_value,Weighted_Tfun_residual, weight_ );

    return Weighted_Tfun_residual;
}

double residual_phase_prop (int i, int p, int pp, int j, TGfitTask *sys)
{
    const char *phase_name;
    int PHndx;
    double computed_value, measured_value;
    double Tfun_residual = 0.0, Weighted_Tfun_residual, weight_ = 1.0;
    DATACH* dCH = sys->NodT[i]->pCSD();

    phase_name = sys->experiments[i]->expphases[p]->phase.c_str();
    PHndx = sys->NodT[i]->Ph_name_to_xDB(phase_name);

    // Get aqueous phase pH
    if ((sys->Tfun->objfun[j]->exp_property == keys::pH) && (sys->experiments[i]->expphases[p]->phase == "aq_gen") && (sys->Tfun->objfun[j]->exp_phase == "aq_gen"))
    {
        if (sys->experiments[i]->expphases[p]->phprop[pp]->Qunit == keys::_loga)
        {
        computed_value = sys->NodT[i]->Get_pH();
        } else computed_value = pow(10,(-(sys->NodT[i]->Get_pH()))) /*sys->NodT[i]->Get_pH()*/;

    } else // Get phase amount
    if ((sys->Tfun->objfun[j]->exp_property == keys::pQnt))
    {
        if (sys->experiments[i]->expphases[p]->phprop[pp]->Qunit == keys::gram)
        {
            computed_value = sys->NodT[i]->Ph_Mass(PHndx) * 1000;
        } else computed_value = sys->NodT[i]->Ph_Mass(PHndx);

    } else // Get phase volume
    if ((sys->Tfun->objfun[j]->exp_property == keys::pV))
    {
        if (sys->experiments[i]->expphases[p]->phprop[pp]->Qunit == keys::cm3)
        {
            computed_value = sys->NodT[i]->Ph_Volume(PHndx) * 1e06;
        } else computed_value = sys->NodT[i]->Ph_Volume(PHndx);

    } else
    if (sys->Tfun->objfun[j]->exp_property == keys::RHO)
    {
        if (sys->experiments[i]->expphases[p]->phprop[pp]->Qunit == keys::g_cm3)
        {
            computed_value = (sys->NodT[i]->Ph_Mass(PHndx) * 1000) / (sys->NodT[i]->Ph_Volume(PHndx) * 1e06);
        }
    } else { cout << "Error in target functions line 427 "; exit(1);}

    measured_value = sys->experiments[i]->expphases[p]->phprop[pp]->pQnt;

    // Error handeling due to possible nonphisical parameters
    if (computed_value < sys->LimitOfDetection)
    {
//        cout << measured_value <<" / " <<computed_value<<" = " << measured_value / computed_value << endl;
        computed_value = rand() % 100 + 1;
    }

    // check Target function type and calculate the Tfun_residual
    weight_ = weight_phprop(i, p, pp, j, sys->Tfun->weight, sys);
    Tfun_residual = Tfunction(computed_value, measured_value, sys->Tfun->type, *sys->Tfun->objfun[j]);
    Weighted_Tfun_residual = Tfunction(computed_value, measured_value, sys->Tfun->type, *sys->Tfun->objfun[j])*weight_;

    sys->set_residuals(computed_value, measured_value, Weighted_Tfun_residual, Tfun_residual, weight_);
    sys->print->set_print(sys->experiments[i]->sample,sys->experiments[i]->expphases[p]->phase,sys->experiments[i]->expphases[p]->phprop[pp]->property,sys->experiments[i]->expphases[p]->phprop[pp]->Qunit,measured_value,computed_value,Weighted_Tfun_residual, weight_ );

    return Weighted_Tfun_residual;
}


double residual_phase_dcomp (int i, int p, int dc, int dcp, int j, TGfitTask *sys)
{
    const char *phase_name, *dcomp_name;
    int PHndx, DCndx;
    double computed_value, measured_value;
    double Tfun_residual = 0.0, Weighted_Tfun_residual, weight_ = 1.0;
    DATACH* dCH = sys->NodT[i]->pCSD();

    phase_name = sys->experiments[i]->expphases[p]->phase.c_str();
    dcomp_name = sys->experiments[i]->expphases[p]->phdcomps[dc]->formula.c_str();
    PHndx = sys->NodT[i]->Ph_name_to_xDB(phase_name);
    DCndx = sys->NodT[i]->DC_name_to_xCH(dcomp_name);

    if (sys->Tfun->objfun[j]->exp_property == keys::pQnt)
    {
        measured_value = sys->NodT[i]->Get_nDC(DCndx); // Retrieves the current mole amount of Dependent Component.
        if (sys->experiments[i]->expphases[p]->phdcomps[dc]->dcompprop[dcp]->Qunit == keys::molfrac)
        {
            measured_value = sys->NodT[i]->Get_cDC(DCndx);// for species in other phases - mole fraction.
        }
    } else
    if (sys->Tfun->objfun[j]->exp_property == keys::actcoef)
    {
        measured_value = sys->NodT[i]->Get_gDC(DCndx);
    } else { cout << "Error in target functions line 474 "; exit(1);}

    measured_value = sys->experiments[i]->expphases[p]->phdcomps[dc]->dcompprop[dcp]->pQnt;

    // Error handeling due to possible nonphisical parameters
//    if (computed_value < sys->LimitOfDetection)
//    {
////        cout << measured_value <<" / " <<computed_value<<" = " << measured_value / computed_value << endl;
//        computed_value = rand() % 100 + 1;
//    }

    // check Target function type and calculate the Tfun_residual
    weight_ = weight_phdcomp(i, p, dc, dcp, j, sys->Tfun->weight, sys);
    Tfun_residual = Tfunction(computed_value, measured_value, sys->Tfun->type, *sys->Tfun->objfun[j]);
    Weighted_Tfun_residual = Tfunction(computed_value, measured_value, sys->Tfun->type, *sys->Tfun->objfun[j])*weight_;

    sys->set_residuals(computed_value, measured_value, Weighted_Tfun_residual, Tfun_residual, weight_);
    sys->print->set_print(sys->experiments[i]->sample,sys->experiments[i]->expphases[p]->phdcomps[dc]->formula,sys->experiments[i]->expphases[p]->phdcomps[dc]->dcompprop[dcp]->property, sys->experiments[i]->expphases[p]->phdcomps[dc]->dcompprop[dcp]->Qunit, measured_value, computed_value, Weighted_Tfun_residual, weight_ );

    return Weighted_Tfun_residual;
}

double Tfunction (double computed_value, double measured_value, string type, TGfitTask::TargetFunction::obj_fun objfun)
{
    double Tf = 0.0;
    if (type == keys::lsq)
    {
        Tf = pow( (computed_value - measured_value), 2) /*/ pow(measured_value, 2)*/;
    }  else
    if (type == keys::lsq_norm)
    {
        Tf = pow( ((computed_value/objfun.meas_average) - (measured_value/objfun.meas_average)), 2);
    }
    else
    {
        // other type of Target functions
    }
    return Tf;
}

double weight (int i, int p, int e, int j, string type, TGfitTask *sys)
{
    if (type == keys::inverr)
    {
        return 1/(sys->experiments[i]->expphases[p]->phcomp[e]->Qerror);
    } else

    if (type == keys::inverr2)
    {
        return 1/(pow(sys->experiments[i]->expphases[p]->phcomp[e]->Qerror,2));
    } else

    if (type == keys::inverr3)
    {
        return 1/(pow(sys->experiments[i]->expphases[p]->phcomp[e]->bQnt,2));
    } else
    if (type == keys::inverr_norm)
    {
        return 1/(pow((sys->experiments[i]->expphases[p]->phcomp[e]->Qerror/sys->Tfun->objfun[j]->meas_average),2));
    } else
        return 1;
}

double weight_phprop (int i, int p, int pp, int j, string type, TGfitTask *sys)
{
    if (type == keys::inverr)
    {
        return 1/(sys->experiments[i]->expphases[p]->phprop[pp]->Qerror);
    } else

    if (type == keys::inverr2)
    {
        return 1/(pow(sys->experiments[i]->expphases[p]->phprop[pp]->Qerror,2));
    } else
    if (type == keys::inverr3)
    {
        return 1/(pow(sys->experiments[i]->expphases[p]->phprop[pp]->pQnt,2));
    } else
    if (type == keys::inverr_norm)
    {
        return 1/(pow((sys->experiments[i]->expphases[p]->phprop[pp]->Qerror/sys->Tfun->objfun[j]->meas_average),2));
    } else
        return 1;
}

double weight_phdcomp (int i, int p, int dc, int dcp, int j, string type, TGfitTask *sys)
{
    if (type == keys::inverr)
    {
        return 1/(sys->experiments[i]->expphases[p]->phdcomps[dc]->dcompprop[dcp]->Qerror);
    } else

    if (type == keys::inverr2)
    {
        return 1/(pow(sys->experiments[i]->expphases[p]->phdcomps[dc]->dcompprop[dcp]->Qerror,2));
    } else
    if (type == keys::inverr3)
    {
        return 1/(pow(sys->experiments[i]->expphases[p]->phdcomps[dc]->dcompprop[dcp]->pQnt,2));
    } else
    if (type == keys::inverr_norm)
    {
        return 1/(pow((sys->experiments[i]->expphases[p]->phdcomps[dc]->dcompprop[dcp]->Qerror/sys->Tfun->objfun[j]->meas_average),2));
    } else
        return 1;
}
