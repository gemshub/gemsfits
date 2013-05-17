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
 *	@file gemsfit_target_functions.h
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

void adjust_G0 (int i, double G0, TGfitTask *sys)
{

    double new_G0=G0;
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
        }
        sys->NodT[n]->Set_DC_G0(species_index, 1e+05, 298.15, G0);
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
        if (sys->experiments[i]->expphases[p]->phcomp[e]->Qunit != unit)
        {
            cout << "Unit for experiment: "<< i <<" from "<< sys->experiments[i]->expdataset << " is not implemented"<< endl;
            exit(1);
        }
    }
}


double residual_aqgen_elem (int i, int p, int e, TGfitTask *sys)
{
    const char *elem_name;
    int ICndx;
    double computed_value, measured_value;
    double residual = 0.0;

    elem_name =  sys->experiments[i]->expphases[p]->phcomp[e]->comp.c_str();
    ICndx = sys->NodT[i]->IC_name_to_xDB(elem_name);

    if (sys->experiments[i]->expphases[p]->phcomp[e]->Qunit == "loga")
    {
    computed_value = log10(sys->NodT[i]->Get_mIC(ICndx));
    } else
    {
        computed_value = sys->NodT[i]->Get_mIC(ICndx);
    }

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

    sys->computed_values_v.push_back(computed_value);
    sys->measured_values_v.push_back(measured_value);
    sys->computed_residuals_v.push_back(residual);

    return residual;
}

double residual_phase_elem (int i, int p, int e, TGfitTask *sys)
{
    const char *elem_name, *phase_name;
    int ICndx, PHndx, nIC;
    double computed_value, measured_value;
    double residual = 0.0;
    DATACH* dCH = sys->NodT[i]->pCSD();
    double* IC_in_PH;

    elem_name =  sys->experiments[i]->expphases[p]->phcomp[e]->comp.c_str();
    ICndx = sys->NodT[i]->IC_name_to_xDB(elem_name);
    phase_name = sys->experiments[i]->expphases[p]->phase.c_str();
    PHndx = sys->NodT[i]->Ph_name_to_xDB(phase_name);

    nIC = dCH->nIC;	// nr of independent components
    IC_in_PH = new double[ nIC ];
    sys->NodT[i]->Ph_BC(PHndx, IC_in_PH);

    computed_value = IC_in_PH[ICndx];
    measured_value = sys->experiments[i]->expphases[p]->phcomp[e]->bQnt;

    // check Target function type and calculate the residual
    if (sys->Tfun->type == "lsq")
    {
        residual = least_square(computed_value, measured_value);
    } else
    {
        // other type of Target functions
    }

    delete[] IC_in_PH;

    sys->computed_values_v.push_back(computed_value);
    sys->measured_values_v.push_back(measured_value);
    sys->computed_residuals_v.push_back(residual);

    return residual;
}




double least_square (double computed_value, double measured_value)
{
    double lsq = 0.0;
    lsq = pow( (computed_value - measured_value), 2) / pow(measured_value, 2);
    return lsq;
}

