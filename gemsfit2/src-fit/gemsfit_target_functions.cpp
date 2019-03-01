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
    int ICndx, HCndx, PHndx, nIC;
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
            computed_value = IC_in_PH[ICndx]; // phase bulk composition in moles (mol)
            if (objfun.exp_unit == keys::molkg )
            {
                computed_value = computed_value / sys->NodT[i]->Ph_Mass(PHndx);
            }
            if (objfun.exp_unit == keys::molkg_dry )
            {
                HCndx = sys->NodT[i]->IC_name_to_xDB("H");
                double H_amount = IC_in_PH[HCndx];
                double H2O_mass = H_amount/2*18.02/1000; // in kg
                computed_value = computed_value / (sys->NodT[i]->Ph_Mass(PHndx)-H2O_mass);
            }
            else

            {
                // Default
                objfun.exp_unit = keys::mol;
            }
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


// calculates the residual of IC as molar ratio
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

    try {
        mu::Parser parser;
        parser.SetExpr(objfun.exp_CN);

        vector<string> varStr;
        parser.SetVarFactory(AddVariable, &varStr);
        parser.GetUsedVar();

        for (unsigned int d = 0; d < varStr.size(); d++)
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

        for (unsigned int d = 0; d < varStr.size(); d++)
        {
            parser.DefineVar(varStr[d], &varDbl[d]);
        }
        computed_value = parser.Eval();

        parser.ClearConst();
        parser.ClearVar();
    }
    catch(mu::Parser::exception_type &e)
       {
         cout << "muParser ERROR for sample " << sys->experiments[i]->sample << "\n";
         cout << "Message:  " << e.GetMsg() << "\n";
         cout << "Formula:  " << e.GetExpr() << "\n";
         cout << "Token:    " << e.GetToken() << "\n";
         if (e.GetPos()!=std::string::npos)
         cout << "Position: " << e.GetPos() << "\n";
         cout << "Errc:     " << e.GetCode() << " http://muparser.beltoforion.de/mup_error_handling.html#idErrors " <<"\n";
//            computed_value = rand() % 100 + 1;
   }


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
    long int PHndx, DC0ndx, nDCinPH, DCndx, DCndx2;
    double computed_value = 0.0, measured_value= 0.0;
    double Tfun_residual = 0.0, Weighted_Tfun_residual, weight_ = 1.0;
    double ln_gama[40], log_gama, HCl_;
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

    } else
    // Get aqueous phase pHm
    if ((objfun.exp_CN == keys::pHm)  && (ccPH == *keys::aq) && (PHndx >=0))
    {
        DCndx = sys->NodT[i]->DC_name_to_xCH("H+"); // think about a way to find H+ name in gems and not hard code it!!!!
        DCndx2 = sys->NodT[i]->DC_name_to_xCH("HCl@");
        log_gama = log10(sys->NodT[i]->Get_gDC(DCndx));
        HCl_ = sys->NodT[i]->Get_cDC(DCndx2);
        if (objfun.exp_unit == keys::_logm)
        {
            computed_value = sys->NodT[i]->Get_pH() + log_gama;
            computed_value = pow(10,-computed_value);
            computed_value = computed_value + HCl_;
            computed_value = -log10(computed_value);
        } else {
            // Default
            computed_value = sys->NodT[i]->Get_pH() + log_gama;
            computed_value = pow(10,-computed_value);
            computed_value = computed_value + HCl_;
            computed_value = -log10(computed_value);
            objfun.exp_unit = keys::_logm;
        }

    } else //Get aqueous phase Eh
    if ((objfun.exp_CN == keys::Eh) && (ccPH == *keys::aq) && (PHndx >=0))
    {
        if (objfun.exp_unit == keys::Volts)
        {
            computed_value = sys->NodT[i]->Get_Eh();
        } else
        {// Default
            computed_value = sys->NodT[i]->Get_Eh();
            objfun.exp_unit == keys::Volts;
        }
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
                if (objfun.exp_unit == keys::mol)
                    computed_value = sys->NodT[i]->Ph_Mole(PHndx);
        else
                {
                    // Default
                    computed_value = sys->NodT[i]->Ph_Mole(PHndx);
                    objfun.exp_unit = keys::mol;
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
    if ((objfun.exp_CN == keys::SI) /*&& (ccPH == *keys::aq)*/ && (PHndx >=0))  // Saturation index
    {
        double T = sys->NodT[i]->Get_TK();
        double P = sys->NodT[i]->Get_P();
        sys->NodT[i]->Set_TK(sys->experiments[i]->sT+273.15);
        sys->NodT[i]->Set_P(sys->experiments[i]->sP*1e5);
        computed_value = sys->NodT[i]->Ph_SatInd(PHndx);
        sys->NodT[i]->Set_TK(T);
        sys->NodT[i]->Set_P(P);
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
        if (objfun.exp_unit == keys::molal)
        {
            computed_value = sys->NodT[i]->Get_IC();
        } else
        if (objfun.exp_unit == keys::logm)
        {
            computed_value = log10(sys->NodT[i]->Get_IC());
        } else
        {
            // Default
            computed_value = sys->NodT[i]->Get_IC();
            objfun.exp_unit = keys::molal;
        }

    } else
    if ((objfun.exp_CN == keys::oscw) && (ccPH == *keys::aq) && (PHndx >=0))
    {
    //  computed_value = ;
        // ++++++++++ !!!!! NOT IMPLEMENTED !!!! +++++
    } else
    if (((objfun.exp_CN == keys::mChainL) ||  (objfun.exp_CN == keys::frAlIV) || (objfun.exp_CN == keys::expr) ||
         (objfun.exp_CN == keys::frAlV) || (objfun.exp_CN == keys::frAlVI) ||
         (objfun.exp_CN == keys::Rd)) || (objfun.exp_CN == keys::activityRatio) && (PHndx >=0))
    {
        vector<double> varDbl;

        if (objfun.expr == "NULL")
        {
            cout << "An expression \"expr\" is needed to calculate the phase property. " << endl;
            exit(1);
        }

        // re-name DC names
        vector<string> exprO, exprP;
        string expr =  objfun.expr;
        expr = formula_DCname_parser(objfun.expr, exprO, exprP);

//        cout << objfun.expr << endl;

        try
        {
            mu::Parser parser;
            parser.SetExpr(expr);

            vector<string> varStr;
            parser.SetVarFactory(AddVariable, &varStr);
            parser.GetUsedVar();

            int DCndx = -1;

            for (unsigned int d = 0; d < varStr.size(); d++)
            {
                for ( unsigned int ex = 0; ex < exprO.size(); ex++)
                {
                    if (varStr[d].c_str() == exprP[ex]) { DCndx = sys->NodT[i]->DC_name_to_xCH(exprO[ex].c_str()); }
                }

                if (DCndx < 0)
                { cout << "ERROR: Dependent component: " << varStr[d].c_str() << " not present in GEMS system! "; exit(1);}

                if (objfun.exp_CN == keys::activityRatio)
                {
                    varDbl.push_back(sys->NodT[i]->Get_aDC( DCndx, true ));
                }
                else
                if (objfun.exp_unit == keys::mol)
                {
                     varDbl.push_back(sys->NodT[i]->Get_nDC(DCndx));
                } else
                if (objfun.exp_unit == keys::molfrac)
                {
                     varDbl.push_back(sys->NodT[i]->Get_cDC(DCndx));
                }
                else
                    varDbl.push_back(sys->NodT[i]->Get_cDC(DCndx)); // default mol fraction
            }

            for (unsigned int d = 0; d < varStr.size(); d++)
            {
                parser.DefineVar(varStr[d], &varDbl[d]);
            }
            computed_value = parser.Eval();
        }
        catch(mu::Parser::exception_type &e)
        {
            cout << "muParser ERROR for sample " << sys->experiments[i]->sample << "\n";
            cout << "Message:  " << e.GetMsg() << "\n";
            cout << "Formula:  " << e.GetExpr() << "\n";
            cout << "Token:    " << e.GetToken() << "\n";
            if (e.GetPos()!=std::string::npos)
                cout << "Position: " << e.GetPos() << "\n";
            cout << "Errc:     " << e.GetCode() << " http://muparser.beltoforion.de/mup_error_handling.html#idErrors " <<"\n";
            //            computed_value = rand() % 100 + 1;
        }

        if ((objfun.exp_unit == keys::loga) && (objfun.exp_CN == keys::activityRatio))
        {
            computed_value = log10(computed_value);
        }

    } else
    if ((objfun.exp_CN == keys::Gex ) && (PHndx >=0))  // functionality added by DK on 03.01.2014
    {
            double Gex = 0./*, gam_dc, x_dc, test =0.*/;
//            long int jc;

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
//        Tfun_residual = Tfunction(computed_value, measured_value, sys->Tfun->type, objfun);
//        Weighted_Tfun_residual = Tfunction(computed_value, measured_value, sys->Tfun->type, objfun)*weight_;
        Tfun_residual = Tfunction(computed_value, measured_value, sys->aTfun[i].type, objfun);
        Weighted_Tfun_residual = Tfunction(computed_value, measured_value, sys->aTfun[i].type, objfun)*weight_;
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
        if (objfun.exp_unit == keys::mol)
        {
            computed_value = sys->NodT[i]->Get_nDC(DCndx); // Retrieves the current mole amount of Dependent Component.
        } else
        if ((objfun.exp_unit == keys::molfrac) || (objfun.exp_unit == keys::molal))
        {
            /// Retrieves concentration of Dependent Component in its phase
            /// in the respective concentration scale. For aqueous species, molality is returned;
            /// for gas species, mole fraction not partial pressure; for surface complexes - molality;
            /// for species in other phases - mole fraction.
            /// \param xdc is DC DBR index
            /// \return 0.0, if DC has zero amount.
            computed_value = sys->NodT[i]->Get_cDC(DCndx);// for species in other phases - mole fraction
        } else
        if ((/*sys->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qunit*/ objfun.exp_unit == keys::log_molfrac) || (objfun.exp_unit == keys::logm))
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
        else computed_value = sys->NodT[i]->Get_aDC( DCndx, true );
    } else
        if ((objfun.exp_DCP == keys::PpG) && (DCndx >=0))
    {

        if (objfun.exp_unit == keys::bar)
             computed_value = sys->NodT[i]->DC_c( DCndx ) / 100000;
        else
            // default log10bar
            computed_value = log10(sys->NodT[i]->DC_c( DCndx ) / 100000);
//         else computed_value = sys->NodT[i]->DC_c( DCndx, true );
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
    if (type == keys::dif)
    {
       Tf = computed_value - measured_value;
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
   double afValBuf[500];
   int iVal = -1;

  vector<string> *test = reinterpret_cast<vector<string> *>(pUserData);
  iVal++;
  test->push_back(a_szName);
  afValBuf[iVal] = 0;

  if (iVal>=499)
    throw mu::ParserError("Variable buffer overflow.");
  else
    return &afValBuf[iVal];
}


string formula_DCname_parser(string expr, vector<string> &exprO, vector<string> &exprP )
{
    string expr_temp, DCname;
    char op[23]= "/+-*^?<>=#!$%&|~'_()@.", opr[23]="abcdefghijklmnopqrstuv";

    expr_temp = expr;

    std::size_t found = expr.find("{"), found_ = expr.find("}"), found_op;
    while (found!=std::string::npos)
    {
        expr.erase(found, 1);
        expr.erase(found_-1,1);

        DCname = expr.substr(found, found_-found-1);
        exprO.push_back(DCname);

        // replaces the found operators with letters
        for (int i=0; i< sizeof(op); i++)
        {
            found_op = DCname.find(op[i]);
            while (found_op!=std::string::npos)
            {
                string r(1, opr[i]);
                DCname.replace(found_op, 1, r);
                found_op = DCname.find(op[i], found_op+1);
            }
        }
        // adds the parsed DC name to the vector
        exprP.push_back(DCname);
        found = expr.find("{", found);
        found_ = expr.find("}", found_);
        if( ((found_==std::string::npos) && (found!=std::string::npos)) || ((found_!=std::string::npos) && (found==std::string::npos)) )
        {
            cout << "ERROR: Missing { or } from the \"expr\""; exit(1);
        }
    }

    for (int i = 0; i < exprO.size(); i++)
    {
        found = expr.find(exprO[i]);
        while ((found!=std::string::npos) && (exprO[i] != exprP[i]))
        {
            expr.replace(found, exprO[i].size(), exprP[i]);
            found = expr.find(exprO[i]);
        }
    }

    return expr;
}
