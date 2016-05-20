/// \file opt_vector.cpp
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


#include "opt_vector.h"
#include "gemsfit_iofiles.h"
#include "gemsfit_global_variables.h"
#include "keywords.h"
#include "json_parse.h"
#include <sstream>


OptParameter::OptParameter(vector<string> data, double OptBoundPrc)
{
    Jdata = data;
    OptBoundPerc = OptBoundPrc;
    mode = gpf->KeysNdx;
}

void OptParameter::Set_TPpairs(vector<double> TPpairs[2])
{
    TP_pairs[0] = TPpairs[0];
    TP_pairs[1] = TPpairs[1];
}

void OptParameter::Pval_to_optF (int p, string data, F_parameter *opt)
{
    vector<string> out2, out3;
    bool UB_LB = true;

//    parse_JSON_object(data, keys::spec, out2);
//    if (out2.size() !=1) { cout << "ERROR: Parameter \"F\"-type " << p << " has no \"spec\" or initial value defined! "<< endl; exit(1); }
//    if (atof(out2[0].c_str()) == 0)
//    {
        parse_JSON_object(data, keys::IV[mode], out3);
        if (out3.size() !=1) { cout << "ERROR: Parameter \"F\"-type " << p << " has no \"IV\" defined! "<< endl; exit(1); }
        opt->opt = atof(out3[0].c_str());
        opt->IV = atof(out3[0].c_str());
        out3.clear();

        parse_JSON_object(data, keys::LB[mode], out3);
        if (out3.size() !=1) { UB_LB = false; }
        else opt->LB = atof(out3[0].c_str());
        out3.clear();

        parse_JSON_object(data, keys::UB[mode], out3);
        if (out3.size() !=1) { UB_LB = false; }
        else opt->UB = atof(out3[0].c_str());
        out3.clear();

        if (OptBoundPerc > 0)
        {
            opt->LB = opt->opt-fabs(opt->opt)*OptBoundPerc/100;
            opt->UB = opt->opt+fabs(opt->opt)*OptBoundPerc/100;
        }

        opt->optNdx = -1;

//    } else
        if (!UB_LB)
    {
//        opt->opt = atof(out2[0].c_str());
        if (opt->opt == 0)
        {
            cout << "ERROR: If the intial value of the parameter is 0 you must specify the UB (upper) and LB (lower) bounds in JSON format { \"IV\": val, \"UB\": val, \"LB\": val}" << endl
                    << "for parameter: " << p << endl;
            exit(1);
        }
        opt->LB = opt->opt-fabs(opt->opt)*keys::bperc/100;
        opt->UB = opt->opt+fabs(opt->opt)*keys::bperc/100;
    }
}

void OptParameter::Pval_to_optL (int p, string data, L_parameter *opt)
{
    vector<string> /*out2,*/ out3;

//    parse_JSON_object(data, keys::spec, out2);
//    if (out2.size() !=1) { cout << "ERROR: Parameter \"L\"-type " << p << " has no \"spec\" defined! "<< endl; exit(1); }

    parse_JSON_object(data, keys::IV[mode], out3);
    opt->IV = atof(out3.at(0).c_str());
    opt->opt = atof(out3.at(0).c_str());
    out3.clear();

//    parse_JSON_object(out, keys::ICN, out3);
//    opt->Pname = out3.at(0);
//    out3.clear();

    parse_JSON_object(data, keys::LICs[mode], out3);
    if (out3.size() == 0)
    { cout << "ERROR: No linked independent component name defined! " << endl; exit(1);}
    for (unsigned int i = 0 ; i < out3.size() ; i++)
    {
        opt->L_param.push_back( out3.at(i) );
    }
    out3.clear();

    parse_JSON_object(data, keys::LCoef[mode], out3);
    if (out3.size() != opt->L_param.size())
    {
        cout << "ERROR: Number of linked parameters is not equal with the number of link coeficients for parameter "<< p  << endl;
        exit(1);
    }
    for (unsigned int i = 0 ; i < out3.size() ; i++)
    {
        opt->L_param_coef.push_back( atof(out3.at(i).c_str()) );
    }

    out3.clear();

}

void OptParameter::Pval_to_optR (int p, string data, R_parameter *opt)
{
    vector<string> /*out2,*/ out3;

//    parse_JSON_object(data, keys::spec, out2);
//    if (out2.size() !=1) { cout << "ERROR: Parameter \"R\"-type " << p << " has no \"spec\" defined! "<< endl; exit(1); }
////    Data_Manager *temp = new Data_Manager(1);

    parse_JSON_object(data, keys::IV[mode], out3);
    if (out3.size() !=1) { cout << "ERROR: Parameter \"R\"-type " << p << " has no \"IV\" defined! "<< endl; exit(1); }
    opt->IV = atof(out3.at(0).c_str()); opt->opt = opt->IV;
    out3.clear();

//    parse_JSON_object(out2[0], keys::Ref, out3);
//    opt.Ref = out3.at(0);
//    out3.clear();

//    temp->parse_JSON_object(Jformat.format, keys::logK, out);
//    r->logK = atof(out.at(0).c_str());
//    out.clear();

    parse_JSON_object(data, keys::nC, out3);
    if (out3.size() !=1) { cout << "ERROR: Parameter \"R\"-type " << p << " has no \"nC\" defined! "<< endl; exit(1); }
    opt->nC = atoi(out3.at(0).c_str());
    out3.clear();

//    parse_JSON_object(data, keys::DCN, out3);
//    if (out3.size() !=1) { cout << "ERROR: Parameter \"R\"-type " << p << " has no \"DCN\" defined! "<< endl; exit(1); }
//    opt->DCn = out3.at(0);
//    out3.clear();

    parse_JSON_object(data, keys::RC, out3);
    if (out3.size() <1) { cout << "ERROR: Parameter \"R\"-type " << p << " has no \"RC\" defined! "<< endl; exit(1); }
    for (unsigned int i = 0 ; i < out3.size() ; i++)
    {
        opt->rdc_species.push_back( out3.at(i) );
    }
    out3.clear();

    parse_JSON_object(data, keys::Rcoef, out3);
    if (out3.size() != opt->rdc_species.size())
    {
        cout << "ERROR: Number of reaction components is not equal with the number of reaction coeficients; R-type parameter: "<< opt->Pname << endl;
        exit(1);
    }
    for (unsigned int i = 0 ; i < out3.size() ; i++)
    {
        opt->rdc_species_coef.push_back( atof(out3.at(i).c_str()) );
    }

    out3.clear();
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

void OptParameter::Get_Lparam(int ndx, string &name, double &IV, double &EV )
{
    name = optLP[ndx]->Pname;
    IV   = optLP[ndx]->IV;
    EV   = optLP[ndx]->opt;
}

void OptParameter::Get_Rparam(int ndx, string &name, double &IV, double &EV )
{
    name = optRP[ndx]->Pname;
    IV   = optRP[ndx]->IV;
    EV   = optRP[ndx]->opt;
}

void OptParameter::Get_Fparam(int ndx, string &name, double &IV, double &EV )
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

Opt_G0::Opt_G0(vector<string> data, double OptBoundPrc, unsigned &p) :
    OptParameter (data, OptBoundPrc )
{
    vector<string> out;
    Ptype = "G0";
    int Rndx = -1;

    for (unsigned int i = 0 ; i < Jdata.size() ; i++)
    {
        parse_JSON_object(Jdata[i], keys::PType[mode], out);
        if (out[0] == "R")
        {
            optRP.push_back( new Opt_G0::R_parameter);
            optRP[optRP.size()-1]->Pndx = -1;
        }
        out.clear();
    }

    for (unsigned int i = 0 ; i < Jdata.size() ; i++)
    {
        parse_JSON_object(Jdata[i], keys::PType[mode], out);
        if (out.size() !=1) { cout << "Parameter " << p << " (G0) has no \"ptype\" defined! "<< endl; exit(1); }
        if (out[0] == "F")
        {
            out.clear();
            optFP.push_back( new Opt_G0::F_parameter);
            optFP[optFP.size()-1]->Pndx = p;
            Pval_to_optF (p, Jdata[i], optFP[optFP.size()-1]);

            parse_JSON_object(Jdata[i], keys::DCN[mode], out);
            if (out.size() !=1) { cout << "Parameter \"F\"-type " << p << " (G0) has no \"DCN\" defined! "<< endl; exit(1); }
            optFP[optFP.size()-1]->Pname = out[0];
            p++;
        }
        if (out[0] == "R")
        {
            out.clear();
            parse_JSON_object(Jdata[i], keys::Rndx, out);
            Rndx = atoi(out[0].c_str()); out.clear();
//            optRP.push_back( new Opt_G0::R_parameter);
            optRP[Rndx]->Pndx = p;

            parse_JSON_object(Jdata[i], keys::DCN[mode], out);
            if (out.size() !=1) { cout << "Parameter \"F\"-type " << p << " (G0) has no \"DCN\" defined! "<< endl; exit(1); }
            if (optRP[Rndx]->Pname != "") { cout << "The index: "<<Rndx<< " was already defined for R-parameter: "<< optRP[Rndx]->Pname << endl; exit(1); }
            optRP[Rndx]->Pname = out[0];

            Pval_to_optR (p, Jdata[i], optRP[Rndx]);

//            h_optR = true;
            p++;
        }
        if (out[0] == "S")
        {
            out.clear();
            optSP.push_back( new Opt_G0::S_parameter);
            optSP[optSP.size()-1]->Pndx = p;

            parse_JSON_object(Jdata[i], keys::DCN[mode], out);
            if (out.size() !=1) { cout << "Parameter \"S\"-type " << p << " (G0) has no \"DCN\" defined! "<< endl; exit(1); }
            optSP[optSP.size()-1]->Pname = out[0];
            out.clear();

            parse_JSON_object(Jdata[i], keys::IV[mode], out);
            if (out.size() !=1) { cout << "Parameter \"S\"-type " << p << " (G0) has no \"spec\" defined! "<< endl; exit(1); }
            optSP[optSP.size()-1]->IV = atof(out[0].c_str()); optSP[optSP.size()-1]->opt = atof(out[0].c_str());
            p++;
        }
        if (out[0] == "L")
        {
            cout << "There is no option at this point to have a G0 parameter as L-Type (linked)! "<<endl; exit(1);
//            out.clear();
//            optLP.push_back( new Opt_bIC::L_parameter);
//            h_optL = true;
//            Pval_to_optL (p, Jdata[i], optLP[optLP.size()-1]);
        }
        out.clear();
    }
}

long int Opt_G0::SetIndex_param(TNode *node)
{
    // index for F type parameters
//    for (unsigned i = 0; i <optFP.size(); i++)
//    {
//        optFP[i]->Pndx = node->DC_name_to_xCH(optFP[i]->Pname.c_str());
//        if (optFP[i]->Pndx < 0)
//        { cout << " Unknown DC name: "<<optFP[i]->Pname<< endl; exit(1);}
//    }

//    // index for S type parameters
//    for (unsigned i = 0; i <optSP.size(); i++)
//    {
//        optSP[i]->Pndx = node->DC_name_to_xCH(optSP[i]->Pname.c_str());
//        if (optSP[i]->Pndx < 0)
//        { cout << " Unknown DC name: "<<optSP[i]->Pname<< endl; exit(1);}
//    }

    // index for R type parameters
    for (unsigned i = 0; i <optRP.size(); i++)
    {
//        optRP[i]->Pndx = node->DC_name_to_xCH(optRP[i]->Pname.c_str());
//        if (optRP[i]->Pndx < 0)
//        { cout << " Unknown DC name: "<<optRP[i]->Pname<< endl; exit(1);}

        for (unsigned s = 0; s < optRP[i]->rdc_species.size(); s++)
        {
            optRP[i]->rdc_species_ind.push_back(node->DC_name_to_xCH(optRP[i]->rdc_species[s].c_str()));
            if (optRP[i]->rdc_species_ind[optRP[i]->rdc_species_ind.size()-1] < 0)
            { cout << " Unknown DC name: "<<optRP[i]->rdc_species[optRP[i]->rdc_species_ind.size()-1]<< " in reaction (Rndx): " << i << endl; exit(1);}
        }
    }
    return 1;
}

long int Opt_G0::Set_logKTP(TNode *node, vector<double> TP_pairs[2] )
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

long int Opt_G0::Adjust_param(TNode *node, vector<double> opt)
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

    delta_G0old_G0new = fabs(node->DC_G0(species_index, 1e+05, 298.15, false)) - fabs(new_G0);

    // going trough all TP pairs
    for (unsigned int j=0; j<TP_pairs[0].size(); ++j)
    {
        new_GTP = delta_G0old_G0new + node->DC_G0(species_index, TP_pairs[1][j]*100000, TP_pairs[0][j]+273.15, false);
        // Set the new G0 in GEMS
        node->Set_DC_G0(species_index, TP_pairs[1][j]*100000, TP_pairs[0][j]+273.15, new_GTP);
    }
    node->Set_DC_G0(species_index, 1e+05, 298.15, Pval);
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

Opt_PMc::Opt_PMc(vector<string> data, double OptBoundPrc, unsigned &p) :
    OptParameter (data, OptBoundPrc )
{
    vector<string> out;
    Ptype = "PMc";
    for (unsigned int i = 0 ; i < Jdata.size() ; i++)
    {
        parse_JSON_object(Jdata[i], keys::PType[mode], out);
        if (out.size() !=1) { cout << "Parameter " << p << " (PMc) has no \"PType\" defined! "<< endl; exit(1); }
        if (out[0] == "F")
        {
            out.clear();
            optFP.push_back( new Opt_PMc::F_parameter);
            optFP[optFP.size()-1]->Pndx = i;
            parse_JSON_object(Jdata[i], keys::IPCN[mode], out);
            if (out.size() == 1) optFP[optFP.size()-1]->Pname = out[0];
            else optFP[optFP.size()-1]->Pname = "PMc";
            Pval_to_optF (p, Jdata[i], optFP[optFP.size()-1]);

            p++;
        }
//        if (out[0] == "R")
//        {

//        }
        if (out[0] == "S")
        {
            out.clear();
            optSP.push_back( new Opt_PMc::S_parameter);
            optSP[optSP.size()-1]->Pndx = i;
            optSP[optSP.size()-1]->Pname = "PMc";

            parse_JSON_object(Jdata[i], keys::IV[mode], out);
            if (out.size() !=1) { cout << "Parameter \"S\"-type " << p << " (G0) has no \"spec\" defined! "<< endl; exit(1); }
            optSP[optSP.size()-1]->IV = atof(out[0].c_str()); optSP[optSP.size()-1]->opt = atof(out[0].c_str());
            p++;
        }
        if (out[0] == "L")
        {
            cout << "There is no option at this point to have a PMc parameter as L-Type (linked)! "<<endl; exit(1);

        }
        out.clear();
    }
}

long int Opt_PMc::Adjust_param(TNode *node, vector<double> opt)
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

Opt_DMc::Opt_DMc(vector<string> data, double OptBoundPrc, unsigned &p) :
    OptParameter (data, OptBoundPrc )
{
    vector<string> out;
    Ptype = "DMc";
    for (unsigned int i = 0 ; i < Jdata.size() ; i++)
    {
        parse_JSON_object(Jdata[i], keys::PType[mode], out);
        if (out.size() !=1) { cout << "Parameter " << p << " (G0) has no \"PType\" defined! "<< endl; exit(1); }
        if (out[0] == "F")
        {
            out.clear();
            optFP.push_back( new Opt_DMc::F_parameter);
            optFP[optFP.size()-1]->Pndx = i;
            if (out.size() == 1) optFP[optFP.size()-1]->Pname = out[0];
            else optFP[optFP.size()-1]->Pname = "DMc";
            Pval_to_optF (p, Jdata[i], optFP[optFP.size()-1]);

            p++;
        }
//        if (out[0] == "R")
//        {

//        }
        if (out[0] == "S")
        {
            out.clear();
            optSP.push_back( new Opt_DMc::S_parameter);
            optSP[optSP.size()-1]->Pndx = i;
            optSP[optSP.size()-1]->Pname = "DMc";

            parse_JSON_object(Jdata[i], keys::IV[mode], out);
            if (out.size() !=1) { cout << "Parameter \"S\"-type " << p << " (G0) has no \"spec\" defined! "<< endl; exit(1); }
            optSP[optSP.size()-1]->IV = atof(out[0].c_str()); optSP[optSP.size()-1]->opt = atof(out[0].c_str());
            p++;
        }
        if (out[0] == "L")
        {
            cout << "There is no option at this point to have a DMc parameter as L-Type (linked)! "<<endl; exit(1);

        }
        out.clear();
    }
}

long int Opt_DMc::Adjust_param(TNode *node, vector<double> opt)
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
Opt_bIC::Opt_bIC(vector<string> data, double OptBoundPrc, unsigned &p) :
    OptParameter (data, OptBoundPrc )
{
    vector<string> out;
    Ptype = "bIC";
    for (unsigned int i = 0 ; i < Jdata.size() ; i++)
    {
        parse_JSON_object(Jdata[i], keys::PType[mode], out);
        if (out.size() !=1) { cout << "Parameter " << p << " (bIC) has no \"ptype\" defined! "<< endl; exit(1); }
        if (out[0] == "F")
        {
            out.clear();
            optFP.push_back( new Opt_bIC::F_parameter);
            optFP[optFP.size()-1]->Pndx = -1;

            parse_JSON_object(Jdata[i], keys::NFndx[mode], out);
            if (out.size() !=1) { cout << "Parameter \"F\"-type " << p << " (bIC) has no \"NFndx\" defined! "<< endl; exit(1); }
            optFP[optFP.size()-1]->Fndx = atoi(out[0].c_str());
            out.clear();

            Pval_to_optF (p, Jdata[i], optFP[optFP.size()-1]);

            parse_JSON_object(Jdata[i], keys::ICN[mode], out);
            if (out.size() !=1) { cout << "Parameter \"F\"-type " << p << " (bIC) has no \"ICN\" defined! "<< endl; exit(1); }
            optFP[optFP.size()-1]->Pname = out[0];
            p++;
        }
        if (out[0] == "L")
        {
            out.clear();
            optLP.push_back( new Opt_bIC::L_parameter);
            optLP[optLP.size()-1]->Pndx = -1;
            Pval_to_optL (p, Jdata[i], optLP[optLP.size()-1]);

            parse_JSON_object(Jdata[i], keys::ICN[mode], out);
            if (out.size() !=1) { cout << "Parameter \"L\"-type " << p << " (bIC) has no \"ICN\" defined! "<< endl; exit(1); }
            optLP[optLP.size()-1]->Pname = out[0];
            p++;
        }
        out.clear();
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
                cout<<endl;
                cout<<" Phase name in GEMSFIT chemical system file has no corresponding phase name in GEMS3K input file !!!! "<<endl;
                cout<<" Can not proceed ... Bailing out now ... "<<endl;
                cout<<endl;
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
        }
    }
    return 1;
}

string Opt_bIC::Print_param( )
{
    string out;
    stringstream in;
    for (unsigned i= 0; i<optFP.size(); i++)
    {
        in << "," << optFP[i]->Pname;
    }

    for (unsigned i= 0; i<optLP.size(); i++)
    {
        in << "," << optLP[i]->Pname;
    }

    in << endl;
    out = in.str();

    return (out);
}


Opt_bIC::~Opt_bIC( )
{

}

// End bIC //

// Tk //

Opt_Tk::Opt_Tk(vector<string> data, double OptBoundPrc, unsigned &p) :
    OptParameter (data, OptBoundPrc )
{
    vector<string> out;
    Ptype = "TK";

    for (unsigned int i = 0 ; i < Jdata.size() ; i++)
    {
        parse_JSON_object(Jdata[i], keys::PType[mode], out);
        if (out.size() !=1) { cout << "Parameter " << p << " (TK) has no \"ptype\" defined! "<< endl; exit(1); }
        if (out[0] == "F")
        {
            out.clear();
            optFP.push_back( new Opt_Tk::F_parameter);

            parse_JSON_object(Jdata[i], keys::NFndx[mode], out);
            if (out.size() !=1) { cout << "Parameter \"F\"-type " << p << " (TK) has no \"NFndx\" defined! "<< endl; exit(1); }
            optFP[optFP.size()-1]->Fndx = atoi(out[0].c_str());
            out.clear();
            optFP[optFP.size()-1]->Pname = "TCelsius";

            Pval_to_optF (p, Jdata[i], optFP[optFP.size()-1]);

            p++;

        }
        if (out[0] == "L")
        {
            cout << " Tk parameters cannot be L-type! "<< endl;
            exit(1);

        }
        out.clear();
    }

}

long int Opt_Tk::SetIndex_param( )
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

long int Opt_Tk::Adjust_Fparam (TNode *node, int Pndx, double Pval)
{
    node->Set_Tk( Pval );
    return 1;
}

string Opt_Tk::Print_param( )
{
    string out;
    stringstream in;
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

Opt_P::Opt_P(vector<string> data, double OptBoundPrc, unsigned &p) :
    OptParameter (data, OptBoundPrc )
{
    vector<string> out;
    Ptype = "P";

    for (unsigned int i = 0 ; i < Jdata.size() ; i++)
    {
        parse_JSON_object(Jdata[i], keys::PType[mode], out);
        if (out.size() !=1) { cout << "Parameter " << p << " (P) has no \"ptype\" defined! "<< endl; exit(1); }
        if (out[0] == "F")
        {
            out.clear();
            optFP.push_back( new Opt_P::F_parameter);

            parse_JSON_object(Jdata[i], keys::NFndx[mode], out);
            if (out.size() !=1) { cout << "Parameter \"F\"-type " << p << " (P) has no \"NFndx\" defined! "<< endl; exit(1); }
            optFP[optFP.size()-1]->Fndx = atoi(out[0].c_str());
            out.clear();
            optFP[optFP.size()-1]->Pname = "Pbar";

            Pval_to_optF (p, Jdata[i], optFP[optFP.size()-1]);

            p++;

        }
        if (out[0] == "L")
        {
            cout << " P parameters cannot be L-type! "<< endl;
            exit(1);

        }
        out.clear();
    }

}

long int Opt_P::SetIndex_param( )
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

long int Opt_P::Adjust_Fparam (TNode *node, int Pndx, double Pval)
{
    node->Set_P( Pval * 100000 );
    return 1;
}

string Opt_P::Print_param( )
{
    string out;
    stringstream in;
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
