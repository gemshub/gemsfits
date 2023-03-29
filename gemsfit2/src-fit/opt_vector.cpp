/// \file opt_std::vector.cpp
/// Implementation of reading adusted parameters and reaction constraints
//
// Copyright (C) 2013 G.D.Miron, D.Kulik
// <GEMS Development Team, mailto:gems2.support@psi.ch>
//
// This file is part of the GEMSFIT code for parameterization of thermodynamic
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

#include "keywords.h"
#include "gemsfit_iofiles.h"
#include "gemsfit_global_variables.h"
#include "gemsfit_target_functions.h"
#include <sstream>
#include <muParser.h>

#if defined(_UNICODE)
#include "muparserfix.h"
#else
#endif


OptParameter::OptParameter(std::vector<std::string> data, double OptBoundPrc)
{
    Jdata = data;
    OptBoundPerc = OptBoundPrc;
    mode = gpf->KeysNdx;
}

void OptParameter::Set_TPpairs(std::vector<double> TPpairs[2])
{
    TP_pairs[0] = TPpairs[0];
    TP_pairs[1] = TPpairs[1];
}

void OptParameter::Pval_to_optF(int p, const common::JsonFree& object_data, F_parameter *opt)
{
    bool UB_LB = true;

    if (!object_data.contains(keys::IV[mode])) {
        std::cout << "ERROR: Parameter \"F\"-type " << p << " has no \"IV\" defined! "<< std::endl;
        exit(1);
    }
    opt->IV = opt->opt = object_data.value(keys::IV[mode], 0.);

    if (!object_data.contains(keys::LB[mode]))
        UB_LB = false;
    else
        opt->LB = object_data.value(keys::LB[mode], 0.);

    if (!object_data.contains(keys::UB[mode]))
        UB_LB = false;
    else
        opt->UB = object_data.value(keys::UB[mode], 0.);

    if (OptBoundPerc > 0) {
        opt->LB = opt->opt-fabs(opt->opt)*OptBoundPerc/100;
        opt->UB = opt->opt+fabs(opt->opt)*OptBoundPerc/100;
    }
    opt->optNdx = -1;

    if (!UB_LB) {
        //        opt->opt = atof(out2[0].c_str());
        if (opt->opt == 0) {
            std::cout << "ERROR: If the intial value of the parameter is 0 you must specify the UB (upper) and LB (lower) bounds in JSON format { \"IV\": val, \"UB\": val, \"LB\": val}" << std::endl
                      << "for parameter: " << p << std::endl;
            exit(1);
        }
        opt->LB = opt->opt-fabs(opt->opt)*keys::bperc/100;
        opt->UB = opt->opt+fabs(opt->opt)*keys::bperc/100;
    }
}

void OptParameter::Pval_to_optL(int p, const common::JsonFree& object_data, L_parameter *opt)
{
    if (object_data.contains(keys::IV[mode]))
        opt->IV = opt->opt = object_data.value(keys::IV[mode], 0.);
    opt->L_param = object_data.value(keys::LICs[mode], vs{});
    if (opt->L_param.size() == 0) {
        std::cout << "ERROR: No linked independent component name defined! " << std::endl;
        exit(1);
    }
    opt->L_param_coef = object_data.value(keys::LCoef[mode], vd{});
    if (opt->L_param_coef.size() != opt->L_param.size()) {
        std::cout << "ERROR: Number of linked parameters is not equal with the number of link coeficients for parameter "<< p  << std::endl;
        exit(1);
    }
}

void OptParameter::Pval_to_optR(int p, const common::JsonFree& object_data, R_parameter *opt)
{
    if (!object_data.contains(keys::IV[mode])) {
        std::cout << "ERROR: Parameter \"R\"-type " << p << " has no \"IV\" defined! "<< std::endl;
        exit(1);
    }
    opt->IV = opt->opt = object_data.value(keys::IV[mode], 0.);
    opt->nC = object_data.value(keys::nC, -1);
    if (opt->nC == -1) {
        std::cout << "ERROR: Parameter \"R\"-type " << p << " has no \"nC\" defined! "<< std::endl;
        exit(1);
    }
    opt->rdc_species = object_data.value(keys::RC, vs{});
    if (opt->rdc_species.size() <1) {
        std::cout << "ERROR: Parameter \"R\"-type " << p << " has no \"RC\" defined! "<< std::endl;
        exit(1);
    }
    opt->rdc_species_coef = object_data.value(keys::Rcoef, vi{});
    if (opt->rdc_species_coef.size() != opt->rdc_species.size()) {
        std::cout << "ERROR: Number of reaction components is not equal with the number of reaction coeficients; R-type parameter: "<< opt->Pname << std::endl;
        exit(1);
    }
}

void OptParameter::Get_R_vNdx_vCoef(int ndx, vi &vNdx, vi &vCoef )
{
    vNdx  = optRP[ndx]->rdc_species_ind;
    vCoef = optRP[ndx]->rdc_species_coef;
}

void OptParameter::Get_IVparam( vd &opt, vd &UB, vd &LB )
 {
     //  F type parameters only
     for (unsigned i = 0; i <optFP.size(); i++)
     {
          opt.push_back(optFP[i]->IV);
          UB.push_back(optFP[i]->UB);
          LB.push_back(optFP[i]->LB);
          optFP[i]->optNdx = opt.size()-1;
     }
 }

long int OptParameter::Get_Fparam(int ndx, int &Fndx, int &Pndx, double &Pval, double &Ub, double &Lb)
{
    Fndx = optFP[ndx]->Fndx;
    Pndx = optFP[ndx]->Pndx;
    Pval = optFP[ndx]->opt;
    Ub   = optFP[ndx]->UB;
    Lb   = optFP[ndx]->LB;
    return 1;
}

long int OptParameter::Get_Fparam(int ndx, int exp, int &Fndx, int &Pndx, double &Pval, double &Ub, double &Lb)
{
    Fndx = optFP[ndx]->Fndx;
    Pndx = optFP[ndx]->Pndx;
    Pval = optFP[ndx]->EVv[exp];
//    Pval = optFP[ndx]->IVv[exp];
    Ub   = optFP[ndx]->UB;
    Lb   = optFP[ndx]->LB;
    return 1;
}

double OptParameter::Get_Fparam(int ndx, int exp )
{
    if (optFP[ndx]->Pname == "TCelsius")
        return (optFP[ndx]->EVv[exp] - 273.15 );
    else
    if (optFP[ndx]->Pname == "Pbar")
        return (optFP[ndx]->EVv[exp] );
    else
        return (optFP[ndx]->EVv[exp]);
}

double OptParameter::Get_Lparam(int ndx, int exp )
{
   return (optLP[ndx]->EVv[exp]);
}

void OptParameter::Get_Lparam(int ndx, std::string &name, double &IV, double &EV )
{
    name = optLP[ndx]->Pname;
    IV   = optLP[ndx]->IV;
    EV   = optLP[ndx]->opt;
}

void OptParameter::Get_Rparam(int ndx, std::string &name, double &IV, double &EV )
{
    name = optRP[ndx]->Pname;
    IV   = optRP[ndx]->IV;
    EV   = optRP[ndx]->opt;
}

void OptParameter::Get_Fparam(int ndx, std::string &name, double &IV, double &EV )
{
    name = optFP[ndx]->Pname;
    IV   = optFP[ndx]->IV;
    EV   = optFP[ndx]->opt;
}

void OptParameter::Set_Fparam(int ndx, int exp, double Pval)
{
    optFP[ndx]->EVv[exp] = Pval;
}


// Destructor
OptParameter::~OptParameter()
{

}

// ++++++ G0  ++++++ //

Opt_G0::Opt_G0(std::vector<std::string> data, double OptBoundPrc, unsigned &p) :
    OptParameter (data, OptBoundPrc )
{
    std::vector<std::string> out;
    Ptype = "G0";
    int Rndx = -1;
    double iv=0.;
    std::string iPType, iDCN;

    for (unsigned int i = 0 ; i < Jdata.size() ; i++)
    {
        common::JsonFree object = fromJsonString(Jdata[i]);
        iPType = object.value( keys::PType[mode], std::string());
        if (iPType == "R") {
            optRP.push_back( new Opt_G0::R_parameter);
            optRP.back()->Pndx = -1;
        }
    }

    for (unsigned int i = 0 ; i < Jdata.size() ; i++)
    {
        common::JsonFree object = fromJsonString(Jdata[i]);
        iPType = object.value( keys::PType[mode], std::string());
        iDCN = object.value( keys::DCN[mode], std::string());

        if (iPType.empty()) {
            std::cout << p << "Parameter " << iDCN << " (G0) has no \"ptype\" defined! "<< std::endl;
            exit(1);
        }
        if (iPType == "F") {
            optFP.push_back( new Opt_G0::F_parameter);
            optFP.back()->Pndx = p;
            Pval_to_optF(p, object, optFP.back());
            if (iDCN.empty()) {
                std::cout << "Parameter \"F\"-type " << p << " (G0) has no \"DCN\" defined! "<< std::endl;
                exit(1);
            }
            optFP.back()->Pname = iDCN;
            p++;
        }
        else if (iPType == "R") {
            Rndx = object.value( keys::Rndx, -1);
            //            optRP.push_back( new Opt_G0::R_parameter);
            if (Rndx<0 || Rndx >= optRP.size() ) {
                std::cout << "Parameter \"R\"-type " << p << " illegal index: " << Rndx << std::endl;
                exit(1);
            }
            optRP[Rndx]->Pndx = p;
            if (iDCN.empty()) {
                std::cout << "Parameter \"F\"-type " << p << " (G0) has no \"DCN\" defined! "<< std::endl;
                exit(1);
            }
            if (optRP[Rndx]->Pname != "") {
                std::cout << "The index: "<<Rndx<< " was already defined for R-parameter: "<< optRP[Rndx]->Pname << std::endl;
                exit(1);
            }
            optRP[Rndx]->Pname = iDCN;
            Pval_to_optR(p, object, optRP[Rndx]);
            //            h_optR = true;
            p++;
        }
        else if (iPType == "S") {
            optSP.push_back( new Opt_G0::S_parameter);
            optSP.back()->Pndx = p;
            if (iDCN.empty()) {
                std::cout << "Parameter \"S\"-type " << p << " (G0) has no \"DCN\" defined! "<< std::endl;
                exit(1);
            }
            optSP.back()->Pname = iDCN;
            if (!object.contains(keys::IV[mode])) {
                std::cout << "Parameter \"S\"-type " << p << " (G0) has no \"spec\" defined! "<< std::endl;
                exit(1);
            }
            iv = object.value(keys::IV[mode], 0.);
            optSP.back()->IV = iv;
            optSP.back()->opt = iv;
            p++;
        }
        else if (iPType == "L")  {
            std::cout << "There is no option at this point to have a G0 parameter as L-Type (linked)! "<<std::endl; exit(1);
        }
        else {
            std::cout << p << "Parameter " << iDCN << " (G0) has unknown type " << iPType << " defined! "<<std::endl;
            exit(1);
        }
    }
}

long int Opt_G0::SetIndex_param(TNode *node)
{
    // index for F type parameters
//    for (unsigned i = 0; i <optFP.size(); i++)
//    {
//        optFP[i]->Pndx = node->DC_name_to_xCH(optFP[i]->Pname.c_str());
//        if (optFP[i]->Pndx < 0)
//        {std::cout << " Unknown DC name: "<<optFP[i]->Pname<< std::endl; exit(1);}
//    }

//    // index for S type parameters
//    for (unsigned i = 0; i <optSP.size(); i++)
//    {
//        optSP[i]->Pndx = node->DC_name_to_xCH(optSP[i]->Pname.c_str());
//        if (optSP[i]->Pndx < 0)
//        {std::cout << " Unknown DC name: "<<optSP[i]->Pname<< std::endl; exit(1);}
//    }

    // index for R type parameters
    for (unsigned i = 0; i <optRP.size(); i++)
    {
//        optRP[i]->Pndx = node->DC_name_to_xCH(optRP[i]->Pname.c_str());
//        if (optRP[i]->Pndx < 0)
//        {std::cout << " Unknown DC name: "<<optRP[i]->Pname<< std::endl; exit(1);}

        for (unsigned s = 0; s < optRP[i]->rdc_species.size(); s++)
        {
            optRP[i]->rdc_species_ind.push_back(node->DC_name_to_xCH(optRP[i]->rdc_species[s].c_str()));
            if (optRP[i]->rdc_species_ind[optRP[i]->rdc_species_ind.size()-1] < 0)
            {std::cout << " Unknown DC name: "<<optRP[i]->rdc_species[optRP[i]->rdc_species_ind.size()-1]<< " in reaction (Rndx): " << i << std::endl; exit(1);}
        }
    }
    return 1;
}

long int Opt_G0::Set_logKTP(TNode *node, std::vector<double> TP_pairs[2] )
{
    double DG = 0.0;
    const double Rln = -2.302585093*8.314472;
    double RTln = 0.0;
    bool h_logK = false;

    // loop trough reactions
    for (unsigned int i = 0; i< optRP.size(); ++i)
    {
        // checks if the logK values were not already read from the input file
        if (optRP[i]->logK_TPpairs.size() == 0) h_logK = true;

        // 25 C 1 bar
        for (unsigned int k = 0; k<optRP[i]->rdc_species.size(); ++k)
        {
            DG += node->DC_G0(optRP[i]->rdc_species_ind[k], 100000, 25+273.15, false) * optRP[i]->rdc_species_coef[k];
        }
        RTln = Rln * (25+273.15);
        if (h_logK) {
        optRP[i]->dG_reaction_TP.push_back(DG);
        optRP[i]->logK_TPpairs.push_back(DG/RTln);}
        DG = 0.0;

        // loop trough TP
        for (unsigned int j = 0; j<TP_pairs[0].size(); ++j)
        {
            // loop trough rection species to calculate delta G of reaction
            for (unsigned int k = 0; k<optRP[i]->rdc_species.size(); ++k)
            {
                DG += node->DC_G0(optRP[i]->rdc_species_ind[k], TP_pairs[1][j]*100000, TP_pairs[0][j]+273.15, false)
                        * optRP[i]->rdc_species_coef[k];
            }
            RTln = Rln * (TP_pairs[0][j]+273.15);
            if (h_logK) {
            optRP[i]->dG_reaction_TP.push_back(DG);
            optRP[i]->logK_TPpairs.push_back(DG/RTln);}
            DG = 0.0;
        }
        h_logK = false;
    }
    return 1;
}

long int Opt_G0::Set_logKTP(int RPndx, double logK  )
{
    optRP[RPndx]->logK_TPpairs.push_back(logK);
    return 1;
}

long int Opt_G0::Adjust_param(TNode *node, std::vector<double> opt)
{
    // F param
    for (unsigned i = 0; i< optFP.size(); i++)
    {
        optFP[i]->opt =  opt[optFP[i]->optNdx];
        Adjust_Fparam(node, optFP[i]->Pndx, opt[optFP[i]->optNdx]);
    }

    // R param
    for (unsigned i = 0; i< optRP.size(); i++)
    {
        Adjust_Rparam( node, *optRP[i] );
    }
 return 1;
}

long int Opt_G0::Adjust_Fparam(TNode *node, int Pndx, double Pval)
{
    double new_G0=Pval;
    double new_GTP=0.0;
    double delta_G0old_G0new;
    int species_index = Pndx;

    try
    {
        delta_G0old_G0new = fabs(node->DC_G0(species_index, 1e+05, 298.15, false)) - fabs(new_G0);

        // going trough all TP pairs
        for (unsigned int j=0; j<TP_pairs[0].size(); ++j)
        {
            new_GTP = delta_G0old_G0new + node->DC_G0(species_index, TP_pairs[1][j]*100000, TP_pairs[0][j]+273.15, false);
            // Set the new G0 in GEMS
            node->Set_DC_G0(species_index, TP_pairs[1][j]*100000, TP_pairs[0][j]+273.15, new_GTP);
        }
        node->Set_DC_G0(species_index, 1e+05, 298.15, Pval);

    } catch(TError& err)
    {
       std::cout << "Error:" << err.title.c_str() << ":" <<  err.mess.c_str() << std::endl;
        return 1;
    }
    return 1;
}

long int Opt_G0::Adjust_Sparam(TNode *node)
{
    for (unsigned i = 0; i < optSP.size(); i++)
    {
        Adjust_Fparam(node, optSP[i]->Pndx, optSP[i]->opt);
    }
return 1;
}

long int Opt_G0::Adjust_Rparam(TNode *node , OptParameter::R_parameter &optR)
{
    // if there are NO logK read form the input file the reaction consttraints are adjusted based on the initial values logK of the reaction
    if (optR.logK_TPpairs.size() == 0)
    {
        double new_G0 =0.0;
        double delta_G = 0.0;
        int species_index = optR.Pndx; /*sys->Opti->reactions[i]->rdc_species_ind[sys->Opti->reactions[i]->rdc_species_ind.size()-1];*/

        // for standard sate at 25 C and 1 bar
        for (unsigned int j=0; j < optR.rdc_species.size()/*-1*/; ++j ) // calculates DG without the last species which is the constrained one
        {
            if (optR.rdc_species[j] != optR.Pname )
            delta_G += optR.rdc_species_coef[j]
                    * node->DC_G0(optR.rdc_species_ind[j], 1e+05, 298.15, false);
        }

        new_G0 = optR.dG_reaction_TP[0] - delta_G;
        delta_G = 0.0;

        optR.opt = new_G0;
        node->Set_DC_G0(species_index,1*100000, 25+273.15, new_G0);

        // for TP points

        // for all TP pairs
        for (unsigned int j=0; j<TP_pairs[0].size(); j++) // loops trough all unique TP_pairs
        {
            for (unsigned int k=0; k < optR.rdc_species.size()/*-1*/; ++k ) // calculates DG without the last species which is the constrained one
            {
                if (optR.rdc_species[k] != optR.Pname )
                delta_G += optR.rdc_species_coef[k]
                        * node->DC_G0(optR.rdc_species_ind[k], TP_pairs[1][j]*100000, TP_pairs[0][j]+273.15, false);
            }

             new_G0 = optR.dG_reaction_TP[j+1] - delta_G; // j+1 for the case TP pairs don't contain 25C 1bar, dG_reaction_TP[0] <-1bar 25C
             delta_G = 0.0;

             node->Set_DC_G0(species_index, TP_pairs[1][j]*100000, TP_pairs[0][j]+273.15, new_G0);

        }
    } else // if there are logK values in the input file
    {
        double new_G0 =0.0;
        double delta_G = 0.0;
        const double Rln = -2.302585093*8.314472; // 8.3144621(75)
        int species_index = optR.Pndx;

        // for standard sate at 25 C and 1 bar
        for (unsigned int j=0; j < optR.rdc_species.size()/*-1*/; ++j ) // calculates DG without the last species which is the constrained one
        {
            if (optR.rdc_species[j] != optR.Pname )
            delta_G += optR.rdc_species_coef[j] * node->DC_G0(optR.rdc_species_ind[j], 1e+05, 298.15, false);
        }


        new_G0 = (Rln*298.15*optR.logK_TPpairs[0]) - delta_G;
        delta_G = 0.0;

        optR.opt = new_G0;
        node->Set_DC_G0(species_index, 1*100000, 25+273.15, new_G0);

        // for TP points

        // for all TP pairs
        for (unsigned int j=0; j<TP_pairs[0].size(); j++) // loops trough all unique TP_pairs
        {
            for (unsigned int k=0; k < optR.rdc_species.size()/*-1*/; ++k ) // calculates DG without the last species which is the constrained one
            {
                if (optR.rdc_species[k] != optR.Pname )
                delta_G += optR.rdc_species_coef[k]
                        * node->DC_G0(optR.rdc_species_ind[k], TP_pairs[1][j]*100000, TP_pairs[0][j]+273.15, false);
            }

            if (optR.logK_TPpairs.size() > 1) // more than just 25 deg C 1 bar experimental data
            new_G0 = (Rln*(TP_pairs[0][j]+273.15)*optR.logK_TPpairs[j+1]) - delta_G; // j = 0 the 25 C 1 bar
            else new_G0 = (Rln*(TP_pairs[0][j]+273.15)*optR.logK_TPpairs[j]) - delta_G;
            delta_G = 0.0;

            node->Set_DC_G0(species_index, TP_pairs[1][j]*100000, TP_pairs[0][j]+273.15, new_G0);

        }
    }
    return 1;
}


// Destructor
Opt_G0::~Opt_G0()
{

}


// End G0 //

// ++++++ PMc  ++++++ //

Opt_PMc::Opt_PMc(std::vector<std::string> data, double OptBoundPrc, unsigned &p) :
    OptParameter (data, OptBoundPrc )
{
    Ptype = "PMc";
    double iv=0.;
    std::string iPType, iIPCN;

    for (unsigned int i = 0 ; i < Jdata.size() ; i++)
    {
        common::JsonFree object = fromJsonString(Jdata[i]);
        iPType = object.value( keys::PType[mode], std::string());
        iIPCN = object.value( keys::IPCN[mode], std::string());

        if (iPType.empty()) {
            std::cout << "Parameter " << p << " (PMc) has no \"PType\" defined! "<< std::endl;
            exit(1);
        }

        if (iPType == "F") {
            optFP.push_back( new Opt_PMc::F_parameter);
            optFP.back()->Pndx = i;
            optFP.back()->Pname = ( iIPCN.empty() ? "PMc" : iIPCN);
            Pval_to_optF (p, object, optFP.back());
            p++;
        }
        else if (iPType == "S") {
            optSP.push_back( new Opt_PMc::S_parameter);
            optSP.back()->Pndx = i;
            optSP.back()->Pname = "PMc";
            if (!object.contains(keys::IV[mode])) {
                std::cout << "Parameter \"S\"-type " << p << " (G0) has no \"spec\" defined! "<< std::endl;
                exit(1);
            }
            iv = object.value(keys::IV[mode], 0.);
            optSP.back()->IV = iv;
            optSP.back()->opt = iv;
            p++;
        }
        else if (iPType == "L") {
           std::cout << "There is no option at this point to have a PMc parameter as L-Type (linked)! "<<std::endl;
           exit(1);
        }
        else {
            std::cout << "Parameter " << iIPCN << " (IP) has unknown type " << iPType << " defined! "<<std::endl;
            exit(1);
        }
    }
}

long int Opt_PMc::Adjust_param(TNode *node, std::vector<double> opt)
{
    // F param
    for (unsigned i = 0; i< optFP.size(); i++)
    {
        optFP[i]->opt =  opt[optFP[i]->optNdx];
        Adjust_Fparam(node, optFP[i]->Pndx, opt[optFP[i]->optNdx]);
    }

    // R param
return 1;
}

long int Opt_PMc::Adjust_Fparam(TNode *node, int Pndx, double Pval)
{
    node->Set_PMc(Pval, Pndx);
    return 1;
}

long int Opt_PMc::Adjust_Sparam(TNode *node)
{
    for (unsigned i = 0; i < optSP.size(); i++)
    {
        Adjust_Fparam(node, optSP[i]->Pndx, optSP[i]->opt);
    }
return 1;
}


// Destructor
Opt_PMc::~Opt_PMc()
{

}


// ++++++ DMc  ++++++ //

Opt_DMc::Opt_DMc(std::vector<std::string> data, double OptBoundPrc, unsigned &p) :
    OptParameter (data, OptBoundPrc )
{
    double iv=0.;
    std::string iPType, iIPCN;
    Ptype = "DMc";

    for (unsigned int i = 0 ; i < Jdata.size() ; i++)
    {
        common::JsonFree object = fromJsonString(Jdata[i]);
        iPType = object.value( keys::PType[mode], std::string());
        iIPCN = object.value( keys::IPCN[mode], std::string());

        if (iPType.empty()) {
            std::cout << "Parameter " << p << " (G0) has no \"PType\" defined! "<< std::endl;
            exit(1);
        }

        if (iPType == "F") {
            optFP.push_back( new Opt_DMc::F_parameter);
            optFP.back()->Pndx = i;
            optFP.back()->Pname =  ( iIPCN.empty() ? "DMc" : iIPCN);
            Pval_to_optF(p, object, optFP.back());
            p++;
        }
        else if (iPType == "S") {
            optSP.push_back( new Opt_DMc::S_parameter);
            optSP.back()->Pndx = i;
            optSP.back()->Pname = "DMc";
            if (!object.contains(keys::IV[mode])) {
                std::cout << "Parameter \"S\"-type " << p << " (G0) has no \"spec\" defined! "<< std::endl;
                exit(1);
            }
            iv = object.value(keys::IV[mode], 0.);
            optSP.back()->IV = iv;
            optSP.back()->opt = iv;
            p++;
        }
        else if (iPType == "L")
        {
            std::cout << "There is no option at this point to have a DMc parameter as L-Type (linked)! "<<std::endl; exit(1);

        }
        else  {
            std::cout << "Parameter " << Jdata[i] << " (IP) has unknown type " <<  iPType<< " defined! "<<std::endl;
            exit(1);
        }
    }
}

long int Opt_DMc::Adjust_param(TNode *node, std::vector<double> opt)
{
    // F param
    for (unsigned i = 0; i< optFP.size(); i++)
    {
        optFP[i]->opt =  opt[optFP[i]->optNdx];
        Adjust_Fparam(node, optFP[i]->Pndx, opt[optFP[i]->optNdx]);
    }

    // R param
return 1;
}

long int Opt_DMc::Adjust_Fparam(TNode *node, int Pndx, double Pval)
{
    node->Set_DMc(Pval, Pndx);
    return 1;
}

long int Opt_DMc::Adjust_Sparam(TNode *node)
{
    for (unsigned i = 0; i < optSP.size(); i++)
    {
        Adjust_Fparam(node, optSP[i]->Pndx, optSP[i]->opt);
    }
return 1;
}


// Destructor
Opt_DMc::~Opt_DMc()
{

}

// ++++++ bIC ++++++ //
Opt_bIC::Opt_bIC(std::vector<std::string> data, double OptBoundPrc, unsigned &p, bool isNFun) :
    OptParameter (data, OptBoundPrc )
{
    double iv=0.;
    std::string iPType, iICN;
    Ptype = "bIC";

    for (unsigned int i = 0 ; i < Jdata.size() ; i++)
    {
        common::JsonFree object = fromJsonString(Jdata[i]);
        iPType = object.value( keys::PType[mode], std::string());
        iICN = object.value( keys::ICN[mode], std::string());

        if (iPType.empty()) {
            std::cout << "Parameter " << p << " (bIC) has no \"ptype\" defined! "<< std::endl;
            exit(1);
        }
        if (!isNFun && iPType != "F" && iPType != "") {
                       std::cout << "Parameter " <<iICN << " (bIC) has not available type " << iPType << " defined! "<<std::endl;
                       exit(1);
        }

        if (iPType == "F") {
            optFP.push_back( new Opt_bIC::F_parameter);
            optFP.back()->expr = object.value(keys::expr, std::string());
            optFP.back()->Fndx = object.value(keys::NFndx[mode], -1);
            if (optFP.back()->Fndx == -1 && isNFun) {
                std::cout << "Parameter \"F\"-type " << p << " (bIC) has no \"NFndx\" defined! "<< std::endl;
                exit(1);
            }
            Pval_to_optF(p, object, optFP.back());
            if (iICN.empty() && isNFun) {
                std::cout << "Parameter \"F\"-type " << p << " (bIC) has no \"ICN\" defined! "<< std::endl;
                exit(1);
            }
            optFP.back()->Pname = iICN;
            p++;
        }
        else if (iPType == "L") {
            optLP.push_back( new Opt_bIC::L_parameter);
            optLP.back()->Pndx = -1;
            Pval_to_optL(p, object, optLP.back());
            if (iICN.empty()) {
                std::cout << "Parameter \"L\"-type " << p << " (bIC) has no \"ICN\" defined! "<< std::endl;
                exit(1);
            }
            optLP.back()->Pname = iICN;
            p++;
        }
    }
}

long int Opt_bIC::SetIndex_param(TNode *node)
{
    // index for F type parameters
    for (unsigned i = 0; i <optFP.size(); i++)
    {
        optFP[i]->Pndx = node->IC_name_to_xCH(optFP[i]->Pname.c_str());
    }

    // index for L type parameters
    int index_IC;
    for (unsigned i = 0; i < optLP.size(); i++ )
    {
        index_IC = node->IC_name_to_xCH( optLP[i]->Pname.c_str() );
        if( index_IC < 0 )
        {
            throw index_IC;
        }
        else
        {
            optLP[i]->Pndx = index_IC;
        }

        for (unsigned int j=0; j<optLP[i]->L_param.size(); ++j )
        {
            // Get form GEMS the index of to_fit_species of interest
            try
            {
                index_IC = node->IC_name_to_xCH( optLP[i]->L_param[j].c_str() );
                if( index_IC < 0 )
                {
                    throw index_IC;
                }
                else
                {
                    optLP[i]->L_param_ind.push_back(index_IC);
                }
            }
            catch( long e )
            {
               std::cout<<std::endl;
               std::cout<<" Phase name in GEMSFIT chemical system file has no corresponding phase name in GEMS3K input file !!!! "<<std::endl;
               std::cout<<" Can not proceed ... Bailing out now ... "<<std::endl;
               std::cout<<std::endl;
                exit(1);
            }
        }
    }
return 1;
}

long int Opt_bIC::SetIVvEVvDelta(TNode *node)
{
    double delta = 0.0;

    for (unsigned i = 0; i <optLP.size(); i++)
    {
        delta = 0.0;
        for (unsigned j=0; j < optLP[i]->L_param.size(); ++j )
        {
            delta += optLP[i]->L_param_coef[j] * node->Get_bIC(optLP[i]->L_param_ind[j]);
        }
        optLP[i]->delta.push_back(delta);
        optLP[i]->IVv.push_back(node->Get_bIC(optLP[i]->Pndx));
        optLP[i]->EVv.push_back(node->Get_bIC(optLP[i]->Pndx));
        //optLP[i]->IV = node->Get_bIC(optLP[i]->Pndx);
    }

    for (unsigned i = 0; i <optFP.size(); i++)
    {
        optFP[i]->IVv.push_back(node->Get_bIC(optFP[i]->Pndx));
        optFP[i]->EVv.push_back(node->Get_bIC(optFP[i]->Pndx));
    }
return 1;
}

long int Opt_bIC::Adjust_Fparam(TNode *node, int Pndx, double Pval)
{
    node->Set_bIC(Pndx, Pval );
    return 1;
}

long int Opt_bIC::Adjust_Fparam(TNode *node, int Pndx, double Pval, std::string expr)
{
    std::vector<std::string> exprO, exprP;
    std::vector<double> varDbl;
    int PHndx;

    expr = formula_DCname_parser(expr, exprO, exprP);

    try
    {
        mu::Parser parser;
#if defined(_UNICODE)
        parser.SetExpr(s2ws(expr));
        std::vector<std::wstring> varStr;
#else
        parser.SetExpr(expr);
        std::vector<std::string> varStr;
#endif

        parser.SetVarFactory(AddVariable, &varStr);
        parser.GetUsedVar();

        int DCndx = -1;

        for (unsigned int d = 0; d < varStr.size(); d++)
        {
#if defined(_UNICODE)
            if (ws2s(varStr[d]) == "value")
#else
            if (varStr[d] == "value")
#endif
            {
                varDbl.push_back(Pval);
            }

            for ( unsigned int ex = 0; ex < exprO.size(); ex++)
            {

#if defined(_UNICODE)
            if (ws2s(varStr[d]) == "phM"+exprP[ex])
#else
            if (varStr[d] == "phM"+exprP[ex])
#endif
            {
#if defined(_UNICODE)
                std::size_t found = varStr[d].find(s2ws("phM"));
#else
                std::size_t found = varStr[d].find("phM");
#endif
                if (found!=std::string::npos)
                {
                     PHndx = node->Ph_name_to_xDB(exprO[ex].c_str());
                     if (PHndx < 0)
                     {std::cout << "ERROR: Phase: " << varStr[d].c_str() << " not present in GEMS system! (bIC expression)"; exit(1);}
                     varDbl.push_back(node->Ph_Mass(PHndx)*1000);
                }
            }
            }
        }

        for (unsigned int d = 0; d < varStr.size(); d++)
        {
#if defined(_UNICODE)
            parser.DefineVar(varStr[d], &varDbl[d]);
#else
            parser.DefineVar(varStr[d], &varDbl[d]);
#endif
        }
        Pval = parser.Eval();
    }
    catch(mu::Parser::exception_type &e)
    {
//       std::cout << "muParser ERROR for sample " << sys->experiments[i]->sample << "\n";
#if defined(_UNICODE)
    std::cout << "Message:  " << ws2s(e.GetMsg()) << "\n";
    std::cout << "Formula:  " << ws2s(e.GetExpr()) << "\n";
    std::cout << "Token:    " << ws2s(e.GetToken()) << "\n";
#else
    std::cout << "Message:  " << e.GetMsg() << "\n";
    std::cout << "Formula:  " << e.GetExpr() << "\n";
    std::cout << "Token:    " << e.GetToken() << "\n";
#endif
        if (e.GetPos()!=std::string::npos)
           std::cout << "Position: " << e.GetPos() << "\n";
       std::cout << "Errc:     " << e.GetCode() << " http://muparser.beltoforion.de/mup_error_handling.html#idErrors " <<"\n";
        //            computed_value = rand() % 100 + 1;
    }
    // first calc val, then set param
    node->Set_bIC(Pndx, Pval );
    return 1;
}

long int Opt_bIC::Adjust_Lparam(TNode *node, int exp )
{
    // it does not loop if there are no optLP
    for (unsigned int i=0; i <optLP.size(); ++i )
    {
        double param_change=0.0, delta_ = 0.0, new_param =0.0;
        int LP_index = optLP[i]->Pndx;

        for (unsigned int j=0; j < optLP[i]->L_param.size(); ++j )
        {
            delta_ += optLP[i]->L_param_coef[j] * node->Get_bIC(optLP[i]->L_param_ind[j]); // C=(a*A + b*B); delta = (a*newA + b*newB)
        }
        param_change =  delta_ - optLP[i]->delta[exp];                                                // param_change = (a*newA + b*newB) - (inita*A + initb*B);
        if (param_change != 0)
        {
            new_param = /*node->Get_bIC(LP_index)*/optLP[i]->IVv[exp] + param_change;        // C = initC + param_change;
            node->Set_bIC(LP_index, new_param);
            optLP[i]->EVv[exp] = new_param;
            //optLP[i]->opt =  new_param;
        }
    }
    return 1;
}

long int Opt_bIC::Adjust_param(TNode *node, std::vector<double> opt)
{
    // F param
    for (unsigned i = 0; i< optFP.size(); i++)
    {
        optFP[i]->opt =  opt[optFP[i]->optNdx];
        if (optFP[i]->expr == "")
            Adjust_Fparam(node, optFP[i]->Pndx, opt[optFP[i]->optNdx]);
        else
            Adjust_Fparam(node, optFP[i]->Pndx, opt[optFP[i]->optNdx], optFP[i]->expr);
    }

    // L param
    for (unsigned i = 0; i< optLP.size(); i++)
    {
        Adjust_Lparam( node, opt[optLP[i]->optNdx] );
    }
 return 1;
}

std::string Opt_bIC::Print_param( )
{
    std::string out;
    std::stringstream in;
    for (unsigned i= 0; i<optFP.size(); i++)
    {
        in << "," << optFP[i]->Pname;
    }

    for (unsigned i= 0; i<optLP.size(); i++)
    {
        in << "," << optLP[i]->Pname;
    }

    in << std::endl;
    out = in.str();

    return (out);
}


Opt_bIC::~Opt_bIC( )
{

}

// End bIC //

// Tk //

Opt_Tk::Opt_Tk(std::vector<std::string> data, double OptBoundPrc, unsigned &p) :
    OptParameter (data, OptBoundPrc )
{
    Ptype = "TK";

    for (unsigned int i = 0 ; i < Jdata.size() ; i++)
    {
        common::JsonFree object = fromJsonString(Jdata[i]);
        auto iPType = object.value( keys::PType[mode], std::string());

        if (iPType.empty()) {
            std::cout << "Parameter " << p << " (TK) has no \"ptype\" defined! "<< std::endl;
            exit(1);
        }

        if (iPType != "S" && iPType != "F" /*&& out[0] != "R"*/ && iPType != "L") {
            std::cout << "Parameter (TK) has unknown type " << iPType<< " defined! "<<std::endl;
            exit(1);
        }

        if (iPType == "F") {
            optFP.push_back( new Opt_Tk::F_parameter);
            optFP.back()->Fndx = object.value(keys::NFndx[mode], -1);
            if (optFP.back()->Fndx == -1 ) {
                std::cout << "Parameter \"F\"-type " << p << " (TK) has no \"NFndx\" defined! "<< std::endl;
                exit(1);
            }
            optFP.back()->Pname = "TCelsius";
            Pval_to_optF (p, object, optFP.back());
            p++;

        }
        if(iPType == "L") {
            std::cout << " Tk parameters cannot be L-type! "<< std::endl;
            exit(1);
        }
    }
}

long int Opt_Tk::SetIndex_param(TNode */*node*/)
{
    // index for F type parameters
    for (unsigned i = 0; i <optFP.size(); i++)
    {
        optFP[i]->Pndx = -1;
    }
    return 1;
}

long int Opt_Tk::SetIVvEVvDelta( TNode *node)
{
    for (unsigned i = 0; i <optFP.size(); i++)
    {
        optFP[i]->IVv.push_back(node->Get_TK( ));
        optFP[i]->EVv.push_back(node->Get_TK());
    }
    return 1;
}

long int Opt_Tk::Adjust_Fparam (TNode *node, int /*Pndx*/, double Pval)
{
    node->Set_Tk( Pval );
    return 1;
}

std::string Opt_Tk::Print_param( )
{
    std::string out;
    std::stringstream in;
    for (unsigned i= 0; i<optFP.size(); i++)
    {
        in << "," << optFP[i]->Pname;
    }
    out = in.str();
    return (out);
}

Opt_Tk::~Opt_Tk( )
{

}

// End TK //

// P //

Opt_P::Opt_P(std::vector<std::string> data, double OptBoundPrc, unsigned &p) :
    OptParameter (data, OptBoundPrc )
{
    Ptype = "P";

    for (unsigned int i = 0 ; i < Jdata.size() ; i++)
    {
        common::JsonFree object = fromJsonString(Jdata[i]);
        auto iPType = object.value( keys::PType[mode], std::string());
        if (iPType.empty()) {
            std::cout << "Parameter " << p << " (P) has no \"ptype\" defined! "<< std::endl;
            exit(1);
        }

        if (/*iPType != "S" &&*/ iPType != "F" /*&& out[0] != "R"*/ && iPType != "L") {
            std::cout << "Parameter (Pbar) has unknown type " << iPType << " defined! "<<std::endl;
            exit(1);
        }

        if (iPType == "F") {
            optFP.push_back( new Opt_P::F_parameter);
            optFP.back()->Fndx = object.value(keys::NFndx[mode], -1);
            if (optFP.back()->Fndx == -1 ) {
                std::cout << "Parameter \"F\"-type " << p << " (P) has no \"NFndx\" defined! " << std::endl;
                exit(1);
            }
            optFP.back()->Pname = "Pbar";
            Pval_to_optF(p, object, optFP.back());
            p++;
        }
        if (iPType == "L") {
            std::cout << " P parameters cannot be L-type! "<< std::endl;
            exit(1);
        }
    }
}

long int Opt_P::SetIndex_param(TNode */*node*/)
{
    // index for F type parameters
    for (unsigned i = 0; i <optFP.size(); i++)
    {
        optFP[i]->Pndx = -1;
    }
    return 1;
}

long int Opt_P::SetIVvEVvDelta( TNode *node)
{
    for (unsigned i = 0; i <optFP.size(); i++)
    {
        optFP[i]->IVv.push_back(node->Get_P() / 100000);
        optFP[i]->EVv.push_back(node->Get_P() / 100000);
    }
    return 1;
}

long int Opt_P::Adjust_Fparam (TNode *node, int /*Pndx*/, double Pval)
{
    node->Set_P( Pval * 100000 );
    return 1;
}

std::string Opt_P::Print_param( )
{
    std::string out;
    std::stringstream in;
    for (unsigned i= 0; i<optFP.size(); i++)
    {
        in << "," << optFP[i]->Pname;
    }
    out = in.str();
    return (out);
}

Opt_P::~Opt_P( )
{

}

// End P //

std::ostream& operator<<(std::ostream& stream, const OptParameter* data) {
    stream << "F_parameter : \n";
    for (auto item : data->optFP) {
        stream << item->Pname << " " << item->IV << " " << item->opt << " "
               << item->optNdx << " " << item->Pndx << " " << item->unit << " " << item->expr << "\n";
        stream << item->Fndx << " " << item->UB << " " << item->LB << "\n";
        stream << "IVv: " << item->IVv << "\n";
        stream << "EVv: " << item->EVv << "\n";
    }
    stream << "R_parameter : \n";
    for (auto item : data->optRP) {
        stream << item->Pname << " " << item->IV << " " << item->opt << " "
               << item->optNdx << " " << item->Pndx << " " << item->unit << " " << item->expr << "\n";
        stream << item->nC << " " << item->Ref << "\n";
        stream << "IVv: " << item->IVv << "\n";
        stream << "EVv: " << item->EVv << "\n";
        stream << "logK_TPpairs: " << item->logK_TPpairs << "\n";
        stream << "dG_reaction_TP: " << item->dG_reaction_TP << "\n";
        stream << "rdc_species: " << item->rdc_species << "\n";
        stream << "rdc_species_ind: " << item->rdc_species_ind << "\n";
        stream << "rdc_species_coef: " << item->rdc_species_coef << "\n";
    }
    stream << "L_parameter : \n";
    for (auto item : data->optLP) {
        stream << item->Pname << " " << item->IV << " " << item->opt << " "
               << item->optNdx << " " << item->Pndx << " " << item->unit << " " << item->expr << "\n";
        stream << "IVv: " << item->IVv << "\n";
        stream << "EVv: " << item->EVv << "\n";
        stream << "delta: " << item->delta << "\n";
        stream << "L_param: " << item->L_param << "\n";
        stream << "L_param_ind: " << item->L_param_ind << "\n";
        stream << "L_param_coef: " << item->L_param_coef << "\n";
    }
    stream << "S_parameter : \n";
    for (auto item : data->optSP) {
        stream << item->Pname << " " << item->IV << " " << item->opt << " "
               << item->optNdx << " " << item->Pndx << " " << item->unit << " " << item->expr << "\n";
        stream << "IVv: " << item->IVv << "\n";
        stream << "EVv: " << item->EVv << "\n";
    }
    return stream;
}
