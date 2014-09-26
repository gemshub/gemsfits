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
 *	@brief this file contains implementations of target functions needed during optimization.
 *  Also other functions needed for setting the newly adjusted paramaters are implemented here.
 *
 *	@author G. Dan Miron
 *
 * 	@date 17.05.2013
 *
 */



#include "gemsfit_target_functions.h"
#include "keywords.h"
#include <boost/lexical_cast.hpp>


/// Adust FUNCTIONS
void adjust_G0 (int i, double G0, TGfitTask *sys)
{

    double new_G0=G0;
    double new_GTP=0.0;
    double delta_G0old_G0new;
    int species_index = sys->Opti->Pindex[i];
    // going trough all nodes
    for (unsigned int n=0; n<sys->NodT.size(); ++n)
    {
        delta_G0old_G0new = fabs(sys->NodT[n]->DC_G0(species_index, 1e+05, 298.15, false)) - fabs(new_G0);
        // going trough all TP pairs
        for (unsigned int j=0; j<sys->TP_pairs[0].size(); ++j)
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
    // 0_param
    if (fabs(new_PMc) <9e-8)
        new_PMc = 0;
    for (unsigned int n=0; n<sys->NodT.size(); ++n)
    {
        sys->NodT[n]->Set_PMc(new_PMc, index_PMc );
//        double xxx;
//        sys->NodT[n]->Get_PMc( xxx, index_PMc);
//        cout << xxx;
    }
}

void adjust_DMc (int i, double new_DMc, TGfitTask *sys)
{
    int index_DMc = sys->Opti->Pindex[i];
    // 0_param
    if (fabs(new_DMc) <9e-8)
        new_DMc = 0;
    for (unsigned int n=0; n<sys->NodT.size(); ++n)
    {
        sys->NodT[n]->Set_DMc(new_DMc, index_DMc );
    }
}

void adjust_bIC (int i, double new_bIC, TGfitTask *sys)
{
    int index_bIC = sys->Opti->Pindex[i];
    for (unsigned int n=0; n<sys->NodT.size(); ++n)
    {
        sys->NodT[n]->Set_bIC(index_bIC, new_bIC );
    }
}

void adjust_bIC (int i, int exp, double new_bIC, TGfitTask *sys)
{
    int index_bIC = i;
    sys->NodT[exp]->Set_bIC(index_bIC, new_bIC );
}

void adjust_TK ( double new_TK, TGfitTask *sys)
{
//    if (fabs(new_PMc) <9e-8)
//        new_PMc = 0;
    for (unsigned int n=0; n<sys->NodT.size(); ++n)
    {
        sys->NodT[n]->Set_TK(new_TK );
        sys->NodT[n]->Set_mLook(0); // activate interpolation of themrodynamic properties lookup array
    }
}

void adjust_P (double new_P, TGfitTask *sys)
{
//    if (fabs(new_PMc) <9e-8)
//        new_PMc = 0;
    for (unsigned int n=0; n<sys->NodT.size(); ++n)
    {
        sys->NodT[n]->Set_P(new_P );
        sys->NodT[n]->Set_mLook(0); // activate interpolation of themrodynamic properties lookup array
    }
}

void adjust_RDc (TGfitTask *sys)
{
    // going trough all nodes
    for (unsigned int n=0; n<sys->NodT.size(); ++n)
    {
        for (unsigned int i=0; i < sys->Opti->reactions.size(); ++i )
        {
            // if there are NO logK read form the input file the reaction consttraints are adjusted based on the initial values logK of the reaction
            if (sys->Opti->reactions[i]->logK_TPpairs.size() == 0)
            {
                double new_G0 =0.0;
                double delta_G = 0.0;
                int species_index = sys->Opti->reactions[i]->DCndx; /*sys->Opti->reactions[i]->rdc_species_ind[sys->Opti->reactions[i]->rdc_species_ind.size()-1];*/

                // for standard sate at 25 C and 1 bar
                for (unsigned int j=0; j < sys->Opti->reactions[i]->rdc_species.size()-1; ++j ) // calculates DG without the last species which is the constrained one
                {
                    delta_G += sys->Opti->reactions[i]->rdc_species_coef[j] * sys->NodT[n]->DC_G0(sys->Opti->reactions[i]->rdc_species_ind[j], 1e+05, 298.15, false);
                }

                new_G0 = sys->Opti->reactions[i]->dG_reaction_TP[0] - delta_G;
                delta_G = 0.0;

                sys->Opti->reactions[i]->std_gibbs = new_G0;
                sys->NodT[n]->Set_DC_G0(species_index,1*100000, 25+273.15, new_G0);

                // for TP points

                // for all TP pairs
                for (unsigned int j=0; j<sys->TP_pairs[0].size(); j++) // loops trough all unique TP_pairs
                {
                    for (unsigned int k=0; k < sys->Opti->reactions[i]->rdc_species.size()-1; ++k ) // calculates DG without the last species which is the constrained one
                    {
                        delta_G += sys->Opti->reactions[i]->rdc_species_coef[k]
                                * sys->NodT[n]->DC_G0(sys->Opti->reactions[i]->rdc_species_ind[k], sys->TP_pairs[1][j]*100000, sys->TP_pairs[0][j]+273.15, false);
                    }

                     new_G0 = sys->Opti->reactions[i]->dG_reaction_TP[j+1] - delta_G;
                     delta_G = 0.0;

                     sys->NodT[n]->Set_DC_G0(species_index, sys->TP_pairs[1][j]*100000, sys->TP_pairs[0][j]+273.15, new_G0);

                }
            } else // if there are logK values in the input file
            {
                double new_G0 =0.0;
                double delta_G = 0.0;
                const double Rln = -2.302585093*8.314472; // 8.3144621(75)
                int species_index = sys->Opti->reactions[i]->DCndx;

                // for standard sate at 25 C and 1 bar
                for (unsigned int j=0; j < sys->Opti->reactions[i]->rdc_species.size()/*-1*/; ++j ) // calculates DG without the last species which is the constrained one
                {
                    if (sys->Opti->reactions[i]->rdc_species[j] != sys->Opti->reactions[i]->DCn )
                    delta_G += sys->Opti->reactions[i]->rdc_species_coef[j] * sys->NodT[n]->DC_G0(sys->Opti->reactions[i]->rdc_species_ind[j], 1e+05, 298.15, false);
                }


                new_G0 = (Rln*298.15*sys->Opti->reactions[i]->logK_TPpairs[0]) - delta_G;
                delta_G = 0.0;

                sys->Opti->reactions[i]->std_gibbs = new_G0;
                sys->NodT[n]->Set_DC_G0(species_index,1*100000, 25+273.15, new_G0);

                // for TP points

                // for all TP pairs
                for (unsigned int j=0; j<sys->TP_pairs[0].size(); j++) // loops trough all unique TP_pairs
                {
                    for (unsigned int k=0; k < sys->Opti->reactions[i]->rdc_species.size()/*-1*/; ++k ) // calculates DG without the last species which is the constrained one
                    {
                        if (sys->Opti->reactions[i]->rdc_species[k] != sys->Opti->reactions[i]->DCn )
                        delta_G += sys->Opti->reactions[i]->rdc_species_coef[k]
                                * sys->NodT[n]->DC_G0(sys->Opti->reactions[i]->rdc_species_ind[k], sys->TP_pairs[1][j]*100000, sys->TP_pairs[0][j]+273.15, false);
                    }

                    if (sys->Opti->reactions[i]->logK_TPpairs.size() > 1) // more than just 25 deg C 1 bar experimental data
                    new_G0 = (Rln*(sys->TP_pairs[0][j]+273.15)*sys->Opti->reactions[i]->logK_TPpairs[j+1]) - delta_G; // j = 0 the 25 C 1 bar
                    else new_G0 = (Rln*(sys->TP_pairs[0][j]+273.15)*sys->Opti->reactions[i]->logK_TPpairs[j]) - delta_G;
                    delta_G = 0.0;

                    sys->NodT[n]->Set_DC_G0(species_index, sys->TP_pairs[1][j]*100000, sys->TP_pairs[0][j]+273.15, new_G0);

                }
            }
        }
    }
}

void adjust_Lp (TGfitTask *sys, optimization::nested optv, int exp)
{
    int P_id = omp_get_thread_num();
    exp = sys->EXPndx[P_id];
    for (unsigned int i=0; i < optv.Lparams.size(); ++i )
    {
        double param_change=0, delta = 0, new_param =0;
        int LP_index = optv.Lparams[i]->index;

        if (sys->Opti->nest_optv.Ptype[i] == "bIC")
        {
            for (unsigned int j=0; j < optv.Lparams[i]->L_param.size(); ++j )
            {
                delta += optv.Lparams[i]->L_coef[j] * sys->NodT[exp]->Get_bIC(optv.Lparams[i]->L_param_ind[j]); // C=(a*A + b*B); delta = (a*newA + b*newB)
            }
            param_change =  delta - optv.Lparams[i]->delta[exp];                                                // param_change = (a*newA + b*newB) - (inita*A + initb*B);
            if (param_change != 0)
            {
                new_param = /*sys->NodT[exp]->Get_bIC(LP_index)*/optv.Lparams[i]->i_val[exp] + param_change;        // C = initC + param_change;
                sys->NodT[exp]->Set_bIC(LP_index, new_param);
                optv.Lparams[i]->e_val[exp] = new_param;
            }
        }
    }
}



/// Unit check FUNCTIONS
void check_unit(int i, int p, int e, string unit, TGfitTask *sys )
{
    if (sys->experiments[i]->expphases[p]->phIC[e]->Qunit != unit)
    {
        if (unit == keys::logm)
        {
            // molal to log(molal)
            if (sys->experiments[i]->expphases[p]->phIC[e]->Qunit == keys::molal)
            {
                double error_perc = sys->experiments[i]->expphases[p]->phIC[e]->Qerror * 100 / sys->experiments[i]->expphases[p]->phIC[e]->Qnt;
                sys->experiments[i]->expphases[p]->phIC[e]->Qnt = log10(sys->experiments[i]->expphases[p]->phIC[e]->Qnt);
                sys->experiments[i]->expphases[p]->phIC[e]->Qerror = fabs(sys->experiments[i]->expphases[p]->phIC[e]->Qnt * error_perc / 100);
                sys->experiments[i]->expphases[p]->phIC[e]->Qunit = keys::logm;
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
        if (sys->experiments[i]->expphases[p]->phIC[e]->Qunit != unit)
        {
            cout << "Unit for experiment: "<< i <<" from "<< sys->experiments[i]->expdataset << " is not implemented"<< endl;
            exit(1);
        }
    }
}

void check_MR_unit(int i, int p, int f, string unit, TGfitTask *sys )
{
    if (sys->experiments[i]->expphases[p]->phMR[f]->Qunit != unit)
    {
        if (unit == keys::log_molratio)
        {
            // MR molal/molal to MR log(molal/molal)
            if (sys->experiments[i]->expphases[p]->phMR[f]->Qunit == keys::molratio)
            {
                double error_perc = sys->experiments[i]->expphases[p]->phMR[f]->Qerror * 100 / sys->experiments[i]->expphases[p]->phMR[f]->Qnt;
                sys->experiments[i]->expphases[p]->phMR[f]->Qnt = log10(sys->experiments[i]->expphases[p]->phMR[f]->Qnt);
                sys->experiments[i]->expphases[p]->phMR[f]->Qerror = fabs(sys->experiments[i]->expphases[p]->phMR[f]->Qnt * error_perc / 100);
                sys->experiments[i]->expphases[p]->phMR[f]->Qunit = keys::log_molratio;
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
        if (sys->experiments[i]->expphases[p]->phMR[f]->Qunit != unit)
        {
            cout << "Unit for experiment: "<< i <<" from "<< sys->experiments[i]->expdataset << " is not implemented"<< endl;
            exit(1);
        }
    }

}


void check_unit_dcomp(int i, int p, int dc, int dcp, string unit, TGfitTask *sys )
{
    if (sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qunit != unit)
    {
        if (unit == keys::log_molfrac)
        {
            if (sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qunit == keys::molfrac)
            {
                double error_perc = sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qerror * 100 / sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qnt;
                sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qnt = log10(sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qnt);
                sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qerror = fabs(sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qnt * error_perc / 100);
                sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qunit = keys::log_molfrac;
            }
            else
            if (sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qunit == keys::ln_molfrac)
            {
                double error_perc = sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qerror * 100 / sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qnt;
                sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qnt = sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qnt / 2.302585093;
                sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qerror = fabs(sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qnt * error_perc / 100);
                sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qunit = keys::log_molfrac;

            }
            else
            {
                cout << "Unit for experiment: "<< i <<" from "<< sys->experiments[i]->expdataset << " is not implemented"<< endl;
                exit(1);
            }

        }
        else
        if (unit == keys::ln_molfrac)
        {
            if (sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qunit == keys::molfrac)
            {
                double error_perc = sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qerror * 100 / sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qnt;
                sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qnt = log(sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qnt);
                sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qerror = fabs(sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qnt * error_perc / 100);
                sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qunit = keys::ln_molfrac;
            }
            else
            if (sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qunit == keys::log_molfrac)
            {
                double error_perc = sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qerror * 100 / sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qnt;
                sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qnt = sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qnt * 2.302585093;
                sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qerror = fabs(sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qnt * error_perc / 100);
                sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qunit = keys::ln_molfrac;
            }
            else
            {
                cout << "Unit for experiment: "<< i <<" from "<< sys->experiments[i]->expdataset << " is not implemented"<< endl;
                exit(1);
            }
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
                double error_perc = sys->experiments[i]->expphases[p]->phprop[pp]->Qerror * 100 / sys->experiments[i]->expphases[p]->phprop[pp]->Qnt;
                sys->experiments[i]->expphases[p]->phprop[pp]->Qnt = -log10(sys->experiments[i]->expphases[p]->phprop[pp]->Qnt);
                sys->experiments[i]->expphases[p]->phprop[pp]->Qerror = fabs(sys->experiments[i]->expphases[p]->phprop[pp]->Qnt * error_perc / 100);
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
                    double error_perc = sys->experiments[i]->expphases[p]->phprop[pp]->Qerror * 100 / sys->experiments[i]->expphases[p]->phprop[pp]->Qnt;
                    sys->experiments[i]->expphases[p]->phprop[pp]->Qnt = pow(10,-(sys->experiments[i]->expphases[p]->phprop[pp]->Qnt));
                    sys->experiments[i]->expphases[p]->phprop[pp]->Qerror = fabs(sys->experiments[i]->expphases[p]->phprop[pp]->Qnt * error_perc / 100);
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
                        double error_perc = sys->experiments[i]->expphases[p]->phprop[pp]->Qerror * 100 / sys->experiments[i]->expphases[p]->phprop[pp]->Qnt;
                        sys->experiments[i]->expphases[p]->phprop[pp]->Qnt = sys->experiments[i]->expphases[p]->phprop[pp]->Qnt / 1000;
                        sys->experiments[i]->expphases[p]->phprop[pp]->Qerror = fabs(sys->experiments[i]->expphases[p]->phprop[pp]->Qnt * error_perc / 100);
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
                            double error_perc = sys->experiments[i]->expphases[p]->phprop[pp]->Qerror * 100 / sys->experiments[i]->expphases[p]->phprop[pp]->Qnt;
                            sys->experiments[i]->expphases[p]->phprop[pp]->Qnt = sys->experiments[i]->expphases[p]->phprop[pp]->Qnt * 1000;
                            sys->experiments[i]->expphases[p]->phprop[pp]->Qerror = fabs(sys->experiments[i]->expphases[p]->phprop[pp]->Qnt * error_perc / 100);
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
                                double error_perc = sys->experiments[i]->expphases[p]->phprop[pp]->Qerror * 100 / sys->experiments[i]->expphases[p]->phprop[pp]->Qnt;
                                sys->experiments[i]->expphases[p]->phprop[pp]->Qnt = sys->experiments[i]->expphases[p]->phprop[pp]->Qnt * 1e06;
                                sys->experiments[i]->expphases[p]->phprop[pp]->Qerror = fabs(sys->experiments[i]->expphases[p]->phprop[pp]->Qnt * error_perc / 100);
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
                                    double error_perc = sys->experiments[i]->expphases[p]->phprop[pp]->Qerror * 100 / sys->experiments[i]->expphases[p]->phprop[pp]->Qnt;
                                    sys->experiments[i]->expphases[p]->phprop[pp]->Qnt = sys->experiments[i]->expphases[p]->phprop[pp]->Qnt / 1e06;
                                    sys->experiments[i]->expphases[p]->phprop[pp]->Qerror = fabs(sys->experiments[i]->expphases[p]->phprop[pp]->Qnt * error_perc / 100);
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
// Calculates the residual of phase independent components (element composition)
double residual_phase_elem (int i, int p, int e, TGfitTask::TargetFunction::obj_fun &objfun, TGfitTask *sys)
{
    const char *elem_name, *phase_name;
    int ICndx, PHndx, nIC;
    double computed_value, measured_value;
    double Tfun_residual = 0.0, Weighted_Tfun_residual = 0.0, weight_ = 1.0;
    DATACH* dCH = sys->NodT[i]->pCSD();
    double* IC_in_PH;
    char ccPH;

    elem_name =  objfun.exp_CN.c_str();
    ICndx = sys->NodT[i]->IC_name_to_xDB(elem_name);
    phase_name = objfun.exp_phase.c_str();
    PHndx = sys->NodT[i]->Ph_name_to_xDB(phase_name);

    nIC = dCH->nIC;	// nr of independent components
    IC_in_PH = new double[ nIC ];
    sys->NodT[i]->Ph_BC(PHndx, IC_in_PH);
    ccPH = sys->NodT[i]->xCH_to_ccPH(PHndx);

    // Get composition of aqueous phase
    if ((ccPH == *keys::aq) && (PHndx >=0) && (ICndx >=0))
    {
        if (objfun.exp_unit == keys::logm)
        {
            double molal_= sys->NodT[i]->Get_mIC(ICndx);
        computed_value = log10(molal_);
        } else
        {
            // Default
            computed_value = sys->NodT[i]->Get_mIC(ICndx); // in mol/Kg
            objfun.exp_unit == keys::molal;
        }
    } else // other than aqueous phase
        if ((ccPH != *keys::aq) && (PHndx >=0) && (ICndx >=0))
        {
            // Default
            computed_value = IC_in_PH[ICndx]; // phase bulk composition in moles (mol)
            objfun.exp_unit = keys::mole;
        } else
        { if (PHndx < 0)
             {
                 cout << "Error: "<< phase_name <<" is not present in the GEMS3K CSD files "; exit(1);
             } else
          if (ICndx < 0)
             {
                 cout << "Error: "<< elem_name <<" is not present in the " <<phase_name; exit(1);
             } else
               cout << "Error in target functions line 481 "; exit(1);
        }


    if ((p >= 0) && (e >= 0))
    {
        measured_value = sys->experiments[i]->expphases[p]->phIC[e]->Qnt;

        // Error handeling due to possible nonphisical parameters

        if ((computed_value < sys->LimitOfDetection) && (computed_value > 0))
        {
    //        cout << measured_value <<" / " <<computed_value<<" = " << measured_value / computed_value << endl;
            computed_value = rand() % 100 + 1;
        }

        if ((objfun.exp_unit == keys::logm) && ((computed_value < log10(sys->LimitOfDetection))))
        {
            computed_value = rand() % 100 + 1;
        }

        // check Target function type and calculate the Tfun_residual
        weight_ = weight(i, p, e, objfun, sys->Tfun->weight, sys) * objfun.TuWeight * objfun.weight;
        Tfun_residual = Tfunction(computed_value, measured_value, sys->Tfun->type, objfun);
        Weighted_Tfun_residual = Tfunction(computed_value, measured_value, sys->Tfun->type, objfun)*weight_;
    } else
    {
        measured_value = 0.0;
        weight_ = 1;
        Tfun_residual = 0.0;
        Weighted_Tfun_residual = 0.0;
    }

    if ((PHndx >=0) && (ICndx >=0))
    objfun.isComputed = true;

    delete[] IC_in_PH;

    sys->set_results(objfun, computed_value, measured_value, Weighted_Tfun_residual, Tfun_residual, weight_);


    return Weighted_Tfun_residual;
}


// calculates the residual of IC as molar fraction
double residual_phase_elemMR (int i, int p, int f, TGfitTask::TargetFunction::obj_fun &objfun, TGfitTask *sys)
{
    const char *elem_name, *phase_name;
    int ICndx, PHndx, nIC;
    double computed_value = 0.0, measured_value = 0.0, computed_nom = 0.0, computed_denom = 0.0;
    double Tfun_residual = 0.0, Weighted_Tfun_residual = 0.0, weight_ = 1.0;
    DATACH* dCH = sys->NodT[i]->pCSD();
    double* IC_in_PH;
    vector<string> nom, denom;
    char ccPH;


    phase_name = objfun.exp_phase.c_str();
    PHndx = sys->NodT[i]->Ph_name_to_xDB(phase_name);

    nIC = dCH->nIC;	// nr of independent components
    IC_in_PH = new double[ nIC ];
    sys->NodT[i]->Ph_BC(PHndx, IC_in_PH);
    ccPH = sys->NodT[i]->xCH_to_ccPH(PHndx);

    if (PHndx < 0)
    {
        cout << "Error: "<< phase_name <<" is not present in the GEMS3K CSD files "; exit(1);
    }

    vector<double> varDbl;

    mu::Parser parser;
    parser.SetExpr(objfun.exp_CN);

    vector<string> varStr;
    parser.SetVarFactory(AddVariable, &varStr);
    parser.GetUsedVar();

    for (int d = 0; d < varStr.size(); d++)
    {
        if ((ccPH == *keys::aq) && (PHndx >=0))
        {
        ICndx = sys->NodT[i]->IC_name_to_xDB(varStr[d].c_str());
        varDbl.push_back(sys->NodT[i]->Get_mIC(ICndx));
        } else
        {
            ICndx = sys->NodT[i]->IC_name_to_xDB(varStr[d].c_str());
            varDbl.push_back(IC_in_PH[ICndx]);
        }
    }

    for (int d = 0; d < varStr.size(); d++)
    {
        parser.DefineVar(varStr[d], &varDbl[d]);
    }
    computed_value = parser.Eval();

    parser.ClearConst();
    parser.ClearVar();


//    computed_value = computed_nom / computed_denom;
    if (objfun.exp_unit == keys::log_molratio)
    {
        computed_value = log10(computed_value);
    }

    if ((p >= 0) && (f >= 0))
    {
        measured_value = sys->experiments[i]->expphases[p]->phMR[f]->Qnt;
        // check Target function type and calculate the Tfun_residual
        weight_ = weight_MR(i, p, f, objfun, sys->Tfun->weight, sys) * objfun.TuWeight * objfun.weight;
        Tfun_residual = Tfunction(computed_value, measured_value, sys->Tfun->type, objfun);
        Weighted_Tfun_residual = Tfunction(computed_value, measured_value, sys->Tfun->type, objfun)*weight_;
    }
    else
    {
        measured_value = 0.0;
        weight_ = 1;
        Tfun_residual = 0.0;
        Weighted_Tfun_residual = 0.0;
    }
    if (PHndx >=0)
    objfun.isComputed = true;

    delete[] IC_in_PH;

    sys->set_results( objfun, computed_value, measured_value, Weighted_Tfun_residual, Tfun_residual, weight_);

    return Weighted_Tfun_residual;
}

// calculates residual for phase properties
double residual_phase_prop (int i, int p, int pp, TGfitTask::TargetFunction::obj_fun &objfun, TGfitTask *sys)
{
    const char *phase_name;
    long int PHndx, DC0ndx, nDCinPH;
    double computed_value = 0.0, measured_value= 0.0;
    double Tfun_residual = 0.0, Weighted_Tfun_residual, weight_ = 1.0;
    double ln_gama[40];
    char ccPH;
//    DATACH* dCH = sys->NodT[i]->pCSD();

    phase_name = objfun.exp_phase.c_str();
    PHndx = sys->NodT[i]->Ph_name_to_xDB(phase_name);
    ccPH = sys->NodT[i]->xCH_to_ccPH(PHndx);
// gpf->fout << "i=" << i << " p=" << p << " pp=" << pp << " j=" << j << " : ";
    // Get aqueous phase pH
    if ((objfun.exp_CN == keys::pH)  && (ccPH == *keys::aq) && (PHndx >=0))
    {
        if (objfun.exp_unit == keys::_loga)
        {
        computed_value = sys->NodT[i]->Get_pH();
        } else {
            // Default
            computed_value = sys->NodT[i]->Get_pH();
            objfun.exp_unit = keys::_loga;
        }

    } else //Get aqueous phase Eh
    if ((objfun.exp_CN == keys::Eh) && (ccPH == *keys::aq) && (PHndx >=0))
    {
        // Default
        computed_value = sys->NodT[i]->Get_Eh();
    } else // Get phase amount
    if ((objfun.exp_CN == keys::Qnt) && (PHndx >=0))
    {
        if (objfun.exp_unit == keys::gram)
        {
            computed_value = sys->NodT[i]->Ph_Mass(PHndx) * 1000;
        } else
            if (objfun.exp_unit == keys::kgram)
                computed_value = sys->NodT[i]->Ph_Mass(PHndx);
        else
                if (objfun.exp_unit == keys::mole)
                    computed_value = sys->NodT[i]->Ph_Mole(PHndx);
        else
                {
                    // Default
                    computed_value = sys->NodT[i]->Ph_Mole(PHndx);
                    objfun.exp_unit = keys::mole;
                }

    } else // Get phase volume
    if ((objfun.exp_CN == keys::pV) && (PHndx >=0))
    {
        if (objfun.exp_unit == keys::cm3)
        {
            computed_value = sys->NodT[i]->Ph_Volume(PHndx) * 1e06;
        } else
        if (objfun.exp_unit == keys::m3)
        {
            computed_value = sys->NodT[i]->Ph_Volume(PHndx);
        } else
        {
            // Default
            computed_value = sys->NodT[i]->Ph_Volume(PHndx);
            objfun.exp_unit = keys::m3;
        }

    } else
    if ((objfun.exp_CN == keys::RHO) && (PHndx >=0))
    {
        if (objfun.exp_unit == keys::g_cm3)
        {
            computed_value = (sys->NodT[i]->Ph_Mass(PHndx) * 1000) / (sys->NodT[i]->Ph_Volume(PHndx) * 1e06);
        } else
        {
            // Default
            computed_value = (sys->NodT[i]->Ph_Mass(PHndx) * 1000) / (sys->NodT[i]->Ph_Volume(PHndx) * 1e06);
            objfun.exp_unit = keys::g_cm3;
        }
    } else
    if ((objfun.exp_CN == keys::pe) && (ccPH == *keys::aq) && (PHndx >=0))
    {
        if (objfun.exp_unit == keys::_loga)
        {
            computed_value = sys->NodT[i]->Get_pe();
        } else
        {
            // Default
            computed_value = sys->NodT[i]->Get_pe();
            objfun.exp_unit == keys::_loga;
        }
    } else
    if ((objfun.exp_CN == keys::IS) && (ccPH == *keys::aq) && (PHndx >=0))
    {
        // Default
        computed_value = sys->NodT[i]->Get_IC();

    } else
    if ((objfun.exp_CN == keys::oscw) && (ccPH == *keys::aq) && (PHndx >=0))
    {
    //  computed_value = ;
        // ++++++++++ !!!!! NOT IMPLEMENTED !!!! +++++
    } else
    if ((objfun.exp_CN == keys::mChainL) && (PHndx >=0))
    {
        vector<double> varDbl;

        if (objfun.expr == "NULL")
        {
            cout << "An expression \"expr\" is needed to calculate the Chain Length. " << endl;
            exit(1);
        }

        mu::Parser parser;
        parser.SetExpr(objfun.expr);

        vector<string> varStr;
        parser.SetVarFactory(AddVariable, &varStr);
        parser.GetUsedVar();

        int DCndx = -1;

        for (int d = 0; d < varStr.size(); d++)
        {
            DCndx = sys->NodT[i]->DC_name_to_xCH(varStr[d].c_str());
            varDbl.push_back(sys->NodT[i]->Get_cDC(DCndx));
        }

        for (int d = 0; d < varStr.size(); d++)
        {
            parser.DefineVar(varStr[d], &varDbl[d]);
        }
        computed_value = parser.Eval();

    } else
    if ((objfun.exp_CN == keys::Gex ) && (PHndx >=0))  // functionality added by DK on 03.01.2014
    {
            double Gex = 0., gam_dc, x_dc, test =0.;
            long int jc;

            computed_value = 0.;
            DC0ndx = sys->NodT[i]->PhtoDC_DBR( PHndx, nDCinPH );
//          n_ph = sys->NodT[i]->Ph_Mole(PHndx);
            if( nDCinPH > 1 )
            {
//               for( jc = DC0ndx; jc<DC0ndx+nDCinPH; jc++ )
//               {
//                  gam_dc = sys->NodT[i]->Get_gDC( jc );  // activity coeff.
//                  x_dc = sys->NodT[i]->Get_cDC( jc );  // concentration (mole fr.)
//                  // n_dc = sys->NodT[i]->Get_nDC(DCndx);
//                  Gex += log( gam_dc ) * x_dc;

//               }

               TMulti *multi = sys->NodT[i]->pMulti();
               TSolMod *sol = multi->pTSolMod(PHndx);
               sol->Get_lnGamma(ln_gama);
               for (int g=0; g<nDCinPH; g++)
               {
                   Gex+= ln_gama[g]*sys->NodT[i]->Get_cDC( g+DC0ndx );
               }
               // Default
               computed_value = Gex * 8.31451 * sys->NodT[i]->cTK(); // in J/mol
               objfun.exp_unit = keys::J_mol;
            }
//            if (sys->experiments[i]->expphases[p]->phprop[pp]->Qunit == keys::kJ_mol)
                                               // so far Gex only in J/mol
    }
    else
    { if (PHndx < 0)
         {
             cout << "Error: "<< phase_name <<" is not present in the GEMS3K CSD files "; exit(1);
         } else cout << "Error in target functions line 857 "; exit(1);
    }

    if ((p >= 0) && (pp >= 0))
    {
        measured_value = sys->experiments[i]->expphases[p]->phprop[pp]->Qnt;

        // Error handling due to possible non-physical parameters
        if ((computed_value < sys->LimitOfDetection) && (computed_value > 0))
        {
    //        cout << measured_value <<" / " <<computed_value<<" = " << measured_value / computed_value << endl;
            computed_value = rand() % 100 + 1;
        }

        // check Target function type and calculate the Tfun_residual
        weight_ = weight_phprop(i, p, pp, objfun, sys->Tfun->weight, sys)  * objfun.TuWeight * objfun.weight;
        Tfun_residual = Tfunction(computed_value, measured_value, sys->Tfun->type, objfun);
        Weighted_Tfun_residual = Tfunction(computed_value, measured_value, sys->Tfun->type, objfun)*weight_;
    }
    else
    {
        measured_value = 0.0;
        weight_ = 1;
        Tfun_residual = 0.0;
        Weighted_Tfun_residual = 0.0;
    }
    if (PHndx >=0)
    objfun.isComputed = true;


    sys->set_results( objfun, computed_value, measured_value, Weighted_Tfun_residual, Tfun_residual, weight_);


    return Weighted_Tfun_residual;
}


// residual for phase dependent components
double residual_phase_dcomp (int i, int p, int dc, int dcp, TGfitTask::TargetFunction::obj_fun &objfun, TGfitTask *sys)
{
    const char/* *phase_name,*/ *dcomp_name;
    int /* PHndx,*/ DCndx;
    double computed_value, measured_value;
    double Tfun_residual = 0.0, Weighted_Tfun_residual, weight_ = 1.0;
//    DATACH* dCH = sys->NodT[i]->pCSD();

//    phase_name = sys->experiments[i]->expphases[p]->phase.c_str();
    dcomp_name = objfun.exp_CN.c_str();
//    PHndx = sys->NodT[i]->Ph_name_to_xDB(phase_name);
    DCndx = sys->NodT[i]->DC_name_to_xCH(dcomp_name);

    if ((objfun.exp_DCP == keys::Qnt) && (DCndx >=0))
    {
        if (objfun.exp_unit == keys::mole)
        {
        computed_value = sys->NodT[i]->Get_nDC(DCndx); // Retrieves the current mole amount of Dependent Component.
        } else
        if (objfun.exp_unit == keys::molfrac)
        {
            /// Retrieves concentration of Dependent Component in its phase
            /// in the respective concentration scale. For aqueous species, molality is returned;
            /// for gas species, mole fraction not partial pressure; for surface complexes - molality;
            /// for species in other phases - mole fraction.
            /// \param xdc is DC DBR index
            /// \return 0.0, if DC has zero amount.
            computed_value = sys->NodT[i]->Get_cDC(DCndx);// for species in other phases - mole fraction
        } else
        if (/*sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qunit*/ objfun.exp_unit == keys::log_molfrac)
            computed_value = log10(sys->NodT[i]->Get_cDC(DCndx));
        else
        if (/*sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qunit*/ objfun.exp_unit== keys::ln_molfrac)
            computed_value = log(sys->NodT[i]->Get_cDC(DCndx));
        else
        {
            computed_value = sys->NodT[i]->Get_cDC(DCndx);
            objfun.exp_unit = keys::molfrac;
        }
    } else
    if ((objfun.exp_DCP == keys::actcoef) && (DCndx >=0))
    {
        computed_value = sys->NodT[i]->Get_gDC(DCndx);
    } else
    if ((objfun.exp_DCP == keys::activity) && (DCndx >=0))
    {

        if (objfun.exp_unit == keys::loga)
            computed_value = sys->NodT[i]->Get_aDC( DCndx, false );
        else computed_value = sys->NodT[i]->Get_aDC( DCndx, false );;
    } else
    { if (DCndx < 0)
         {
             cout << "Error: "<< dcomp_name <<" is not present in the GEMS3K CSD files "; exit(1);
         } else cout << "Error in target functions line 869 "; exit(1);
    }

    if ((p >= 0) && (dc >= 0) && (dcp >= 0))
    {
    measured_value = sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qnt;

    // check Target function type and calculate the Tfun_residual
    weight_ = weight_phdcomp(i, p, dc, dcp, objfun, sys->Tfun->weight, sys) * objfun.TuWeight * objfun.weight;
    Tfun_residual = Tfunction(computed_value, measured_value, sys->Tfun->type, objfun);
    Weighted_Tfun_residual = Tfunction(computed_value, measured_value, sys->Tfun->type, objfun)*weight_;
    } else
    {
        measured_value = 0.0;
        weight_ = 1;
        Tfun_residual = 0.0;
        Weighted_Tfun_residual = 0.0;
    }

    if (DCndx >=0)
    objfun.isComputed = true;

    sys->set_results( objfun, computed_value, measured_value, Weighted_Tfun_residual, Tfun_residual, weight_);


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
    } else
    if (type == keys::abs_dif)
    {
       Tf = fabs(computed_value - measured_value);
    } else
    {
      // other type of Target functions
    }

    return Tf;
}

double weight (int i, int p, int e, TGfitTask::TargetFunction::obj_fun &objfun, string type, TGfitTask *sys)
{
    if (type == keys::inverr)
    {
        return 1/(sys->experiments[i]->expphases[p]->phIC[e]->Qerror);
    } else

    if (type == keys::inverr2)
    {
        return 1/(pow(sys->experiments[i]->expphases[p]->phIC[e]->Qerror,2));
    } else

    if (type == keys::inverr3)
    {
        return 1/(pow(sys->experiments[i]->expphases[p]->phIC[e]->Qnt,2));
    } else
    if (type == keys::inverr_norm)
    {
        return 1/(pow((sys->experiments[i]->expphases[p]->phIC[e]->Qerror/objfun.meas_average),2));
    } else
        return 1;
}

double weight_MR (int i, int p, int f, TGfitTask::TargetFunction::obj_fun &objfun, string type, TGfitTask *sys)
{
    if (type == keys::inverr)
    {
        return 1/(sys->experiments[i]->expphases[p]->phMR[f]->Qerror);
    } else

    if (type == keys::inverr2)
    {
        return 1/(pow(sys->experiments[i]->expphases[p]->phMR[f]->Qerror,2));
    } else

    if (type == keys::inverr3)
    {
        return 1/(pow(sys->experiments[i]->expphases[p]->phMR[f]->Qnt,2));
    } else
    if (type == keys::inverr_norm)
    {
        return 1/(pow((sys->experiments[i]->expphases[p]->phMR[f]->Qerror/objfun.meas_average),2));
    } else
        return 1;
}

double weight_phprop (int i, int p, int pp, TGfitTask::TargetFunction::obj_fun &objfun, string type, TGfitTask *sys)
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
        return 1/(pow(sys->experiments[i]->expphases[p]->phprop[pp]->Qnt,2));
    } else
    if (type == keys::inverr_norm)
    {
        return 1/(pow((sys->experiments[i]->expphases[p]->phprop[pp]->Qerror/objfun.meas_average),2));
    } else
        return 1;
}

double weight_phdcomp (int i, int p, int dc, int dcp, TGfitTask::TargetFunction::obj_fun &objfun, string type, TGfitTask *sys)
{
    if (type == keys::inverr)
    {
        return 1/(sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qerror);
    } else

    if (type == keys::inverr2)
    {
        return 1/(pow(sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qerror,2));
    } else
    if (type == keys::inverr3)
    {
        return 1/(pow(sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qnt,2));
    } else
    if (type == keys::inverr_norm)
    {
        return 1/(pow((sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qerror/objfun.meas_average),2));
    } else
        return 1;
}

double* AddVariable(const char *a_szName, void *pUserData)
{
   double afValBuf[100];
   int iVal = -1;

  vector<string> *test = reinterpret_cast<vector<string> *>(pUserData);
  iVal++;
  test->push_back(a_szName);
  afValBuf[iVal] = 0;

  if (iVal>=99)
    throw mu::ParserError("Variable buffer overflow.");
  else
    return &afValBuf[iVal];
}
