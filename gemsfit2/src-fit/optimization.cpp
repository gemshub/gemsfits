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
 *	@file optimization.cpp
 *
 *	@brief this header file contains implementetions of optimization class functions for reading in
 *  and setting optimization specific input parameters
 *
 *	@author G. Dan Miron
 *
 * 	@date 17.05.2013
 *
 */


#include "optimization.h"
#include "json_parse.h"
#include "keywords.h"


optimization::optimization()
{
    h_optNF = false;
    mode = gpf->KeysNdx;

    constraint_data = new my_constraint_data;

    OptTuckey = 0;
    OptInitStep = 0;

    gpf->flog << "09. optimization.cpp(48). Reading NLopt optimization settings from the input file; " << endl;

//    define_nlopt_param();
    get_nlopt_param();

    OptParameterCreate();

    GetParameters ();

    if (OptBoundPerc > 0.)
    {
        UB = OptUpBounds;
        LB = OptLoBounds;
    }
}

void optimization::get_nlopt_param()
{
    string fname, str, data;
    vector<string> out;
    int mode = gpf->KeysNdx;

    fname = gpf->OptParamFile();

    std::ifstream file(fname.c_str());
    std::ostringstream tmp;
    tmp<<file.rdbuf();
    std::string s = tmp.str();

    GEMSsys = gpf->GEMS3LstFilePath();

    parse_JSON_object(s, keys::OptParameters[mode], out);
    OptParameters = out[0];
    out.clear();

    parse_JSON_object(s, keys::OptNFParameters[mode], out);
    NFunParameters = out[0];
    out.clear();

    parse_JSON_object(s, keys::OptDW[mode], out);
    OptDoWhat = atoi(out[0].c_str());
    out.clear();

    parse_JSON_object(s, keys::OptEQ[mode], out);
    OptEquilibrium = atoi(out[0].c_str());
    out.clear();

    parse_JSON_object(s, keys::OptUW[mode], out);
    OptUserWeight = atoi(out[0].c_str());
    out.clear();

    parse_JSON_object(s, keys::OptTu[mode], out);
    OptTuckey = atoi(out[0].c_str());
    out.clear();

    parse_JSON_object(s, keys::OptTuVal[mode], out);
    OptTuckeyVal = atoi(out[0].c_str());
    out.clear();

    parse_JSON_object(s, keys::OptAlg[mode], out);
    OptAlgo = out[0].c_str();
    out.clear();

    parse_JSON_object(s, keys::OptPBP[mode], out);
    OptBoundPerc = atof(out[0].c_str());
    out.clear();

    parse_JSON_object(s, keys::OptTRel[mode], out);
    OptTolRel = atof(out[0].c_str());
    out.clear();

    parse_JSON_object(s, keys::OptTAbs[mode], out);
    OptTolAbs = atof(out[0].c_str());
    out.clear();

    parse_JSON_object(s, keys::OptMEv[mode], out);
    OptMaxEval = atoi(out[0].c_str());
    out.clear();

    parse_JSON_object(s, keys::OptNormP[mode], out);
    OptNormParam = atoi(out[0].c_str());
    out.clear();

    parse_JSON_object(s, keys::OptPer[mode], out);
    OptPerturbator = atof(out[0].c_str());
    out.clear();

}

void optimization::OptParameterCreate ()
{
    unsigned  p = 0;
    vector<string> out, out2, out3, outPMc, outDMc;

    OptParameter* myOPT = 0;

    // call GEM_init to read GEMS3K input files
    TNode* node  = new TNode();

    // call GEM_init     --> read in input files
    if( (node->GEM_init( GEMSsys.c_str() )) == 1 )
       {
           cout << GEMSsys << endl;
           cout<<" .. ERROR occurred while reading GEMS3K input files !!! ..."<<endl;
           exit(1);
       }

    //OFUN parameters
    // NFUN parameters
    parse_JSON_object(OptParameters, keys::G0[mode], out);
    if (out.size() > 0)
    {
        Opt_G0* myPT = new Opt_G0( out, OptBoundPerc, p );
        myPT->SetIndex_param(node);
        myOPT = (Opt_G0*)myPT;
    }
    if(myOPT) { optParam.push_back( myOPT ); myOPT = 0; };
    out.clear(); p=0;

    parse_JSON_object(OptParameters, keys::PAM[mode], out);
    if (out.size() > 0)
    {
        for (unsigned i = 0; i < out.size(); i++)
        {
            // PMc
            parse_JSON_object(out[i], keys::PMc[mode], out2);
            for (unsigned j = 0; j < out2.size(); j++)
            {
                parse_JSON_object(out2[j], keys::IPCs[mode], out3);
                outPMc.insert(outPMc.end(), out3.begin(), out3.end());
                out3.clear();
            }
            out2.clear();

            // DMc
            parse_JSON_object(out[i], keys::DMc[mode], out2);
            for (unsigned j = 0; j < out2.size(); j++)
            {
                parse_JSON_object(out2[j], keys::PDCC[mode], out3);
                outDMc.insert(outDMc.end(), out3.begin(), out3.end());
                out3.clear();
            }
            out2.clear();
        }

        // PMc
        if (outPMc.size() > 0)
        {
            Opt_PMc* myPT = new Opt_PMc( outPMc, OptBoundPerc, p );
//            myPT->SetIndex_param(node);
            myOPT = (Opt_PMc*)myPT;
        }
        if(myOPT) { optParam.push_back( myOPT ); myOPT = 0; };
        out.clear(); p=0;

        // DMc
        if (outPMc.size() > 0)
        {
            Opt_DMc* myPT = new Opt_DMc( outDMc, OptBoundPerc, p );
//            myPT->SetIndex_param(node);
            myOPT = (Opt_DMc*)myPT;
        }
        if(myOPT) { optParam.push_back( myOPT ); myOPT = 0; };
        out.clear(); p=0;
    }
    out.clear();


    // NFUN parameters
    parse_JSON_object(NFunParameters, "bIC", out);
    if (out.size() > 0)
    {
        Opt_bIC* myPT = new Opt_bIC( out, OptBoundPerc, p );
        myPT->SetIndex_param(node);
        myOPT = (Opt_bIC*)myPT;
    }
    if(myOPT) { optNFParam.push_back( myOPT ); myOPT = 0; };
    out.clear(); p=0;

    parse_JSON_object(NFunParameters, "TK", out);
    if (out.size() > 0)
    {
        Opt_Tk* myPT = new Opt_Tk( out, OptBoundPerc, p );
        myPT->SetIndex_param( );
        myOPT = (Opt_Tk*)myPT;
    }
    if(myOPT) { optNFParam.push_back( myOPT ); myOPT = 0; };
    out.clear(); p=0;

    parse_JSON_object(NFunParameters, "P", out);
    if (out.size() > 0)
    {
        Opt_P* myPT = new Opt_P( out, OptBoundPerc, p );
        myPT->SetIndex_param( );
        myOPT = (Opt_P*)myPT;
    }
    if(myOPT) { optNFParam.push_back( myOPT ); myOPT = 0; };
    out.clear(); p=0;
}

void optimization::GetParameters ()
{
    vector<double> opt_, UB_, LB_;
    for (unsigned i = 0; i <optParam.size(); i++)
    {
        optParam[i]->Get_IVparam(opt_, UB_, LB_);
    }

    optv0 = opt_; UB = UB_; LB=LB_;

    optv_0 = optv0;
    optv = optv0;

    OptUpBounds = UB;
    OptLoBounds = LB;
}

//void optimization::get_OptParameters ( )
//{
//    vector<string> out, out2;
//    int p = -1;
//    optimization::F_parameter optF; optimization::R_parameter optR;
//    /*optimization::L_parameter optL;*/ optimization::S_parameter optS;
//    optF.Pclass =""; optF.UB = 0.0;
//    optF.LB = 0.0; optF.opt = 0.0; optF.Pindex = -1;
//    optR.DCn=""; optR.DCndx = -1; optR.Ival = 0.0;
//    optR.nC = 0; optR.std_gibbs = 0.0;
//    optS.Pclass = ""; optS.Pindex = -1; optS.Pval = 0.0;

//    // call GEM_init to read GEMS3K input files
//    TNode* node  = new TNode();

//    // call GEM_init     --> read in input files
//    if( (node->GEM_init( GEMSsys.c_str() )) == 1 )
//       {
//           cout << GEMSsys << endl;
//           cout<<" .. ERROR occurred while reading GEMS3K input files !!! ..."<<endl;
//           exit(1);
//       }

//    // G0 of dependent components
//    parse_JSON_object(OptParameters, "G0", out);
//    for (unsigned int i = 0 ; i < out.size() ; i++)
//    {
//        p++;
//        parse_JSON_object(out[i], keys::ptype, out2);

//        if (out2.size() !=1) { cout << "Parameter " << p+1 << " has no \"ptype\" defined! "<< endl; exit(1); }
//        // independent optimized parameters
//        if (out2[0] == "F")
//        {
//            out2.clear();
//            optF.Pclass = "G0";
//            Pval_to_optF (optPF.size(), out[i], optF);

//            parse_JSON_object(out[i], keys::DCN, out2);
//            if (out2.size() !=1) { cout << "Parameter \"F\"-type " << optPF.size()+1 << " has no \"DCN\" defined! "<< endl; exit(1); }
//            optF.Pindex = node->DC_name_to_xCH(out2[0].c_str());

//            optPF.push_back(optF);

//        }
//        if (out2[0] == "R")
//        {
//            out2.clear();
//            optR.Pclass = "G0";
//            Pval_to_optR (optPR.size(), out[i], optR);
//            optR.DCndx = node->DC_name_to_xCH(optR.DCn.c_str());

//            optPR.push_back(optR);

//            optR.rdc_species.clear();
//            optR.rdc_species_coef.clear();

//            h_optR = true;
//        }
//        if (out2[0] == "S")
//        {
//            out2.clear();
//            optS.Pclass = "G0";
//            parse_JSON_object(out[i], keys::DCN, out2);
//            if (out2.size() !=1) { cout << "Parameter \"S\"-type " << optPS.size()+1 << " has no \"DCN\" defined! "<< endl; exit(1); }
//            optS.Pindex = node->DC_name_to_xCH(out2[0].c_str());
//            out2.clear();
//            parse_JSON_object(out[i], keys::spec, out2);
//            if (out2.size() !=1) { cout << "Parameter \"S\"-type " << optPS.size()+1 << " has no \"spec\" initial value defined! "<< endl; exit(1); }
//            optS.Pval = atof(out2[0].c_str());

//            optPS.push_back(optS);


//        }
//        out2.clear();
//    }
//    out.clear();



//    DATACH* dCH = node->pCSD();
//    long int PHndx = -1, IPndx = -1, IPCndx = -1, index = 0, DCndx = -1, nDC = dCH->nDC, nDCPH = 0, nDCPH_=0, DCndx_ = -1;
//    long int *LsMod = node->Get_LsMod();
//    long int *LsMdc = node->Get_LsMdc();

//    // PMc
//    parse_JSON_object(OptParameters, "PMc", out);
//    for (unsigned int i = 0 ; i < out.size() ; i++)
//    {
//        p++;
//        parse_JSON_object(out[i], keys::EPH, out2);
//        if (out2.size() !=1) { cout << "Parameter PMc " << i << " has no \"EPH\" defined! "<< endl; exit(1); }
//        PHndx = node->Ph_name_to_xCH(out2[0].c_str());
//        out2.clear();
//        parse_JSON_object(out[i], "IPndx", out2);
//        if (out2.size() !=1) { cout << "Parameter PMc " << i << " has no \"IPndx\" defined! "<< endl; exit(1); }
//        IPndx = atoi(out2[0].c_str());
//        out2.clear();
//        parse_JSON_object(out[i], "IPCndx", out2);
//        if (out2.size() !=1) { cout << "Parameter PMc " << i << " has no \"IPCndx\" defined! "<< endl; exit(1); }
//        IPCndx = atoi(out2[0].c_str());
//        out2.clear();

//        // Function to determine the parameter index
//        int x=0;
//        for (unsigned j = 0; j <= PHndx; j++)
//        {
//            index += LsMod[x+j]*LsMod[x+j+2];
//            x +=2;
//        }
//        index = index - ( (LsMod[PHndx*3] - IPndx - 1)*LsMod[PHndx*3+2] + LsMod[PHndx*3+2] - IPCndx ); // gets the PMc index

//        parse_JSON_object(out[i], keys::ptype, out2);
//        if (out2.size() !=1) { cout << "Parameter " << p+1 << " has no \"ptype\" defined! "<< endl; exit(1); }
//        if (out2[0] == "F")
//        {
//            out2.clear();
//            optF.Pclass = "PMc";
//            Pval_to_optF (optPF.size(), out[i], optF);
//            optF.Pindex = index;
//            optPF.push_back(optF);


//        } else
//        if (out2[0] == "S")
//        {
//            out2.clear();
//            optS.Pclass = "PMc";
//            optS.Pindex = index;
//            out2.clear();
//            parse_JSON_object(out[i], keys::spec, out2);
//            if (out2.size() !=1) { cout << "Parameter \"S\"-type " << i << " has no \"spec\" initial value defined! "<< endl; exit(1); }
//            optS.Pval = atof(out2[0].c_str());
//            optPS.push_back(optS);
//        }
//        out2.clear();
//        index = 0;
//    }
//    out.clear();

//    parse_JSON_object(OptParameters, "DMc", out);
//    for (unsigned int i = 0 ; i < out.size() ; i++)
//    {
//        p++;

//        parse_JSON_object(out[i], keys::EPH, out2);
//        if (out2.size() !=1) { cout << "Parameter \"F\"-type " << optPF.size()+1 << " has no \"EPH\" defined! "<< endl; exit(1); }
//        PHndx = node->Ph_name_to_xCH(out2[0].c_str());
//        out2.clear();

//        parse_JSON_object(out[i], keys::DCN, out2);
//        if (out2.size() !=1) { cout << "Parameter \"F\"-type " << optPF.size()+1 << " has no \"DCN\" defined! "<< endl; exit(1); }
//        DCndx = node->DC_name_to_xCH(out2[0].c_str());;
//        out2.clear();

//        DCndx_ = node->PhtoDC_DCH(PHndx, nDCPH); // returns the index of the first DC in the phase and the number of DC in the pahse.

//        int x =0;
//        for (unsigned j=0; j<PHndx; j++)
//        {
//            node->PhtoDC_DCH(j, nDCPH_);
//            index += LsMdc[j+x]*nDCPH_;
//            x +=2;
//        }
//        index = index + (DCndx - DCndx_);

//        parse_JSON_object(out[i], keys::ptype, out2);
//        if (out2.size() !=1) { cout << "Parameter " << p+1 << " has no \"ptype\" defined! "<< endl; exit(1); }
//        if (out2[0] == "F")
//        {
//            out2.clear();
//            optF.Pclass = "DMc";
//            Pval_to_optF (optPF.size(), out[i], optF);
//            optF.Pindex = index;
//            optPF.push_back(optF);
//        } else
//        if (out2[0] == "S")
//        {
//            out2.clear();
//            optS.Pclass = "DMc";
//            optS.Pindex = index;
//            out2.clear();
//            parse_JSON_object(out[i], keys::spec, out2);
//            if (out2.size() !=1) { cout << "Parameter \"S\"-type " << i << " has no \"spec\" initial value defined! "<< endl; exit(1); }
//            optS.Pval = atof(out2[0].c_str());
//            optPS.push_back(optS);
//        }
//        out2.clear();
//        index = 0;
//    }

//    // bIC
//    parse_JSON_object(OptParameters, "bIC", out);
//    for (unsigned int i = 0 ; i < out.size() ; i++)
//    {
//        p++;
//        parse_JSON_object(out[i], keys::ptype, out2);
//        if (out2.size() !=1) { cout << "Parameter " << p+1 << " has no \"ptype\" defined! "<< endl; exit(1); }
//        if (out2[0] == "F")
//        {
//            out2.clear();
//            optF.Pclass = "bIC";
//            Pval_to_optF (this->optPF.size(), out[i], optF);

//            parse_JSON_object(out[i], keys::ICN, out2);
//            if (out2.size() !=1) { cout << "Parameter \"F\"-type " << this->optPF.size()+1 << " has no \"ICN\" defined! "<< endl; exit(1); }
//            optF.Pindex = node->IC_name_to_xCH(out2[0].c_str());

//            this->optPF.push_back(optF);
//        }
//        if (out2[0] == "L")
//        {
//            out2.clear();
//            optL.Pclass = "bIC";
//            Pval_to_optL (this->optPL.size(), out[i], optL);
//            optL.index = node->IC_name_to_xCH(optL.name.c_str());

//            this->optPL.push_back(optL);
//            optL.L_param.clear();
//            optL.L_coef.clear();


//        }
//        out2.clear();
//    }
//    out.clear();


//    get_optR_indexes(node, this->optPR);

//    out.clear();

//}


optimization::~optimization()
{
    delete constraint_data;

}

void optimization::normalize_params(const vector<double> initguesses , bool NormBounds)
{
    unsigned int i= 0;

    gpf->flog << " ... performing parameter normalization ... " << endl;

    // Normalize init guess vector
    optv.resize( initguesses.size() );
    for(i=0; i<optv.size(); i++)
    {
        if (optv[i] != 0)
        optv[i] = initguesses[i] / fabs(initguesses[i]);
    }

    // Normalize upper bounds vector
    NormBounds = false; // the bounds are normalized each time to the new param values
    if (!NormBounds)
    {
    for(i=0; i<OptUpBounds.size(); i++)
    {
        gpf->flog << "     Init guess ["<<i<<"] = " << initguesses[i] << endl;
        gpf->flog << "     Upper Bound old ["<<i<<"]= " << OptUpBounds[i] << endl;
//        if ((optv[i] != 0) && (fabs(initguesses[i]) > 9e-11))
        OptUpBounds[i] = UB[i] / fabs(initguesses[i]);
        gpf->flog << "     Upper Bound new ["<<i<<"]= " << OptUpBounds[i] << endl;
    }

    // Normalize lower bounds vector
    for(i=0; i<OptLoBounds.size(); i++)
    {
        gpf->flog << "     Init guess ["<<i<<"] = " << initguesses[i] << endl;
        gpf->flog << "     Lower Bound old ["<<i<<"]= " << OptLoBounds[i] << endl;
//        if ((optv[i] != 0) && (fabs(initguesses[i]) > 9e-11))
        OptLoBounds[i] = LB[i] / fabs(initguesses[i]);
        gpf->flog << "     Lower Bound new ["<<i<<"]= " << OptLoBounds[i] << endl;
    }

//     Normalize constraints vector
    for(i=0; i<constraint_data_v.size(); i++)
    {
//        if ((optv[i] != 0) && (fabs(initguesses[i]) > 9e-11))
        constraint_data_v[i].Constraints = constraint_data_v[i].Constraints / fabs(initguesses[i]);
    }
    }
}


// NLopt return codes
void optimization::print_return_message( const int result )
{
    switch (result)
    {
        case -5:
            cout<<" Halted because of a forced termination: the user called nlopt_force_stop(opt) on the optimization’s nlopt_opt object opt from the user’s objective function or constraints. "<<endl;
            break;
        case -4:
            cout<<" Halted because roundoff errors limited progress. (In this case, the optimization still typically returns a useful result.) "<<endl;
            break;
        case -3:
            cout<<" Ran out of memory. "<<endl;
            break;
        case -2:
            cout<<" Invalid arguments (e.g. lower bounds are bigger than upper bounds, an unknown algorithm was specified, etc.). "<<endl;
            break;
        case -1:
            cout<<" Generic failure code. "<<endl;
            break;
        case 1:
            cout<<" Generic success return value. "<<endl;
            break;
        case 2:
            cout<<" Optimization stopped because stopval was reached. "<<endl;
            break;
        case 3:
            cout<<" Optimization stopped because ftol_rel or ftol_abs was reached. "<<endl;
            break;
        case 4:
            cout<<" Optimization stopped because xtol_rel or xtol_abs was reached. "<<endl;
            break;
        case 5:
            cout<<" Optimization stopped because maxeval was reached. "<<endl;
            break;
        case 6:
            cout<<" Optimization stopped because maxtime was reached. "<<endl;
            break;
    }
}// end print_return_message

