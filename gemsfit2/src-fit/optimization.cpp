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

std::ostream& operator<<(std::ostream& stream, const optimization& data);

optimization::optimization()
{
    h_optNF = false;
    mode = gpf->KeysNdx;

    constraint_data = new my_constraint_data;

    OptTuckey = 0;
    OptInitStep = 0;

    gpf->flog << "09. optimization.cpp(48). Reading NLopt optimization settings from the input file; " << std::endl;

//    define_nlopt_param();
    get_nlopt_param();

    OptParameterCreate();
    GetParameters ();

#ifdef CHECK_LOAD
    std::fstream test_out("optimization.log", std::ios::out);
    test_out << *this;
#endif

    if (OptBoundPerc > 0.)
    {
        UB = OptUpBounds;
        LB = OptLoBounds;
    }
}

void optimization::get_nlopt_param()
{
    std::string fname;
    std::vector<std::string> out, out2;
    int mode = gpf->KeysNdx;

    fname = gpf->OptParamFile();

    std::ifstream file(fname.c_str());
    std::ostringstream tmp;
    tmp<<file.rdbuf();
    std::string s = tmp.str();

    GEMSsys = gpf->GEMS3LstFilePath();

    parse_JSON_object(s, keys::OptParameters[mode], out);
    if (out.size() == 0) {std::cout << "Error: No keyword for \""
                               <<keys::OptParameters[mode]<<"\" found in the task definition"
                               << std::endl; exit(1);}
    OptParameters = out;
    out.clear();

    parse_JSON_object(s, keys::OptNFParameters[mode], out);
    NFunParameters = out[0];
    out.clear();

    parse_JSON_object(s, keys::AddOptParameters[mode], out);
    AddOptParameters = out[0];
    out.clear();

    // Optimization settings and statistics
    parse_JSON_object(s, keys::OptSet[mode], out);

    parse_JSON_object(out[0], keys::OptDW[mode], out2);
    OptDoWhat = atoi(out2[0].c_str());
    out2.clear();

    parse_JSON_object(out[0], keys::OptPrcParamDigits[mode], out2);
    if (out2.size()>0)
        OptPrcParamDigits = atoi(out2[0].c_str());
    out2.clear();

    parse_JSON_object(out[0], keys::OptEQ[mode], out2);
    OptEquilibrium = atoi(out2[0].c_str());
    out2.clear();

    parse_JSON_object(out[0], keys::SIA[mode], out2);
    if (out2.size()>0)
        OptGemsSIA = atoi(out2[0].c_str());
    out2.clear();

    parse_JSON_object(out[0], keys::OptUW[mode], out2);
    OptUserWeight = atoi(out2[0].c_str());
    out2.clear();

    parse_JSON_object(out[0], keys::OptMixedResiduals[mode], out2);
    if (out2.size()>0)
        OptMixedSumOfResiduals = atoi(out2[0].c_str());
    out2.clear();

    parse_JSON_object(out[0], keys::OptTu[mode], out2);
    OptTuckey = atoi(out2[0].c_str());
    out2.clear();

    parse_JSON_object(out[0], keys::OptTuVal[mode], out2);
    OptTuckeyVal = atoi(out2[0].c_str());
    out2.clear();

    parse_JSON_object(out[0], keys::OptAlg[mode], out2);
     OptAlgo = out2[0].c_str();
    out2.clear();

    parse_JSON_object(out[0], keys::OptPBP[mode], out2);
    OptBoundPerc = atof(out2[0].c_str());
    out2.clear();

    parse_JSON_object(out[0], keys::OptTRel[mode], out2);
    OptTolRel = atof(out2[0].c_str());
    out2.clear();

    parse_JSON_object(out[0], keys::OptTAbs[mode], out2);
    OptTolAbs = atof(out2[0].c_str());
    out2.clear();

    parse_JSON_object(out[0], keys::OptMEv[mode], out2);
    OptMaxEval = atoi(out2[0].c_str());
    out2.clear();

    parse_JSON_object(out[0], keys::OptNormP[mode], out2);
    OptNormParam = atoi(out2[0].c_str());
    out2.clear();

    parse_JSON_object(out[0], keys::OptPer[mode], out2);
    OptPerturbator = atof(out2[0].c_str());
    out2.clear();
    out.clear();
}

void optimization::OptParameterCreate ()
{
    unsigned  p = 0;
    std::vector<std::string> out, out2, out3, outG0, outPMc, outDMc;

    OptParameter* myOPT = 0;

    // call GEM_init to read GEMS3K input files
    TNode* node  = new TNode();

    try
    {
        // call GEM_init     --> read in input files
        if( (node->GEM_init( GEMSsys.c_str() )) == 1 )
        {
            std::cout << GEMSsys << std::endl;
            std::cout<<" .. ERROR occurred while reading GEMS3K input files !!! ..."<<std::endl;
            exit(1);
        }
    }
    catch(TError& err)
    {
        if( !GEMSsys.empty() )
            std::cout << "GEMS3K input : file " << GEMSsys << std::endl;
        std::cout << err.title.c_str() << "  : " << err.mess.c_str() << std::endl;
    }


    //OFUN parameters
    // NFUN parameters
//    parse_JSON_object(OptParameters, keys::G0[mode], out);
//    if (out.size() > 0)
//    {
//        Opt_G0* myPT = new Opt_G0( out, OptBoundPerc, p );
//        myPT->SetIndex_param(node);
//        myOPT = (Opt_G0*)myPT;
//    }
//    if(myOPT) { optParam.push_back( myOPT ); myOPT = 0; };
//    out.clear(); p=0;

//    parse_JSON_object(OptParameters, keys::PAM[mode], out);



    if (OptParameters.size() > 0)
    {
        for (unsigned i = 0; i < OptParameters.size(); i++)
        {
            // G0
            parse_JSON_object(OptParameters[i], keys::G0[mode], out2);
            outG0.insert(outG0.end(), out2.begin(), out2.end());
            out2.clear();

            // PMc
            parse_JSON_object(OptParameters[i], keys::PMc[mode], out2);
            for (unsigned j = 0; j < out2.size(); j++)
            {
                parse_JSON_object(out2[j], keys::IPCs[mode], out3);
                outPMc.insert(outPMc.end(), out3.begin(), out3.end());
                out3.clear();
            }
            out2.clear();

            // DMc
            parse_JSON_object(OptParameters[i], keys::DMc[mode], out2);
            for (unsigned j = 0; j < out2.size(); j++)
            {
                parse_JSON_object(out2[j], keys::PDCC[mode], out3);
                outDMc.insert(outDMc.end(), out3.begin(), out3.end());
                out3.clear();
            }
            out2.clear();
        }

        if (outG0.size() > 0)
        {
            Opt_G0* myPT = new Opt_G0( outG0, OptBoundPerc, p );
            myPT->SetIndex_param(node);
            myOPT = (Opt_G0*)myPT;
        }
        if(myOPT) { optParam.push_back( myOPT ); myOPT = 0; };
         p=0;

        // PMc
        if (outPMc.size() > 0)
        {
            Opt_PMc* myPT = new Opt_PMc( outPMc, OptBoundPerc, p );
//            myPT->SetIndex_param(node);
            myOPT = (Opt_PMc*)myPT;
        }
        if(myOPT) { optParam.push_back( myOPT ); myOPT = 0; };
         p=0;

        // DMc
        if (outPMc.size() > 0)
        {
            Opt_DMc* myPT = new Opt_DMc( outDMc, OptBoundPerc, p );
//            myPT->SetIndex_param(node);
            myOPT = (Opt_DMc*)myPT;
        }
        if(myOPT) { optParam.push_back( myOPT ); myOPT = 0; };
         p=0;
    }
    out.clear();

    // NFUN parameters
    if (NFunParameters != "")
    {
        parse_JSON_object(NFunParameters, keys::bIC[mode], out);
        if (out.size() > 0)
        {
            Opt_bIC* myPT = new Opt_bIC( out, OptBoundPerc, p );
            myPT->SetIndex_param(node);
            myOPT = (Opt_bIC*)myPT;
        }
        if(myOPT) { optNFParam.push_back( myOPT ); myOPT = 0; };
        out.clear(); p=0;

        parse_JSON_object(NFunParameters, keys::TK[mode], out);
        if (out.size() > 0)
        {
            Opt_Tk* myPT = new Opt_Tk( out, OptBoundPerc, p );
            myPT->SetIndex_param(node);
            myOPT = (Opt_Tk*)myPT;
        }
        if(myOPT) { optNFParam.push_back( myOPT ); myOPT = 0; };
        out.clear(); p=0;

        parse_JSON_object(NFunParameters, keys::Pb[mode], out);
        if (out.size() > 0)
        {
            Opt_P* myPT = new Opt_P( out, OptBoundPerc, p );
            myPT->SetIndex_param(node);
            myOPT = (Opt_P*)myPT;
        }
        if(myOPT) { optNFParam.push_back( myOPT ); myOPT = 0; };
        out.clear(); p=0;
    }
    out.clear();

    // Additional parameters
    if (AddOptParameters != "")
    {
        parse_JSON_object(AddOptParameters, keys::bIC[mode], out);
        if (out.size() > 0)
        {
            Opt_bIC* myPT = new Opt_bIC( out, OptBoundPerc, p, false );
            myPT->SetIndex_param(node);
            myOPT = (Opt_bIC*)myPT;
        }
        if(myOPT) { optParam.push_back( myOPT ); myOPT = 0; };
        out.clear(); p=0;
    }
}

void optimization::GetParameters ()
{
    std::vector<double> opt_, UB_, LB_;
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


optimization::~optimization()
{
    delete constraint_data;

}

void optimization::normalize_params(const std::vector<double> initguesses , bool NormBounds)
{
    unsigned int i= 0;

    gpf->flog << " ... performing parameter normalization ... " << std::endl;

    // Normalize init guess std::vector
    optv.resize( initguesses.size() );
    for(i=0; i<optv.size(); i++)
    {
        if (optv[i] != 0)
        optv[i] = initguesses[i] / fabs(initguesses[i]);
    }

    // Normalize upper bounds std::vector
    NormBounds = false; // the bounds are normalized each time to the new param values
    if (!NormBounds)
    {
    for(i=0; i<OptUpBounds.size(); i++)
    {
        gpf->flog << "     Init guess ["<<i<<"] = " << initguesses[i] << std::endl;
        gpf->flog << "     Upper Bound old ["<<i<<"]= " << OptUpBounds[i] << std::endl;
//        if ((optv[i] != 0) && (fabs(initguesses[i]) > 9e-11))
        OptUpBounds[i] = UB[i] / fabs(initguesses[i]);
        gpf->flog << "     Upper Bound new ["<<i<<"]= " << OptUpBounds[i] << std::endl;
    }

    // Normalize lower bounds std::vector
    for(i=0; i<OptLoBounds.size(); i++)
    {
        gpf->flog << "     Init guess ["<<i<<"] = " << initguesses[i] << std::endl;
        gpf->flog << "     Lower Bound old ["<<i<<"]= " << OptLoBounds[i] << std::endl;
//        if ((optv[i] != 0) && (fabs(initguesses[i]) > 9e-11))
        OptLoBounds[i] = LB[i] / fabs(initguesses[i]);
        gpf->flog << "     Lower Bound new ["<<i<<"]= " << OptLoBounds[i] << std::endl;
    }

//     Normalize constraints std::vector
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
            std::cout<<" Halted because of a forced termination: the user called nlopt_force_stop(opt) on the optimization’s nlopt_opt object opt from the user’s objective function or constraints. "<<std::endl;
            break;
        case -4:
            std::cout<<" Halted because roundoff errors limited progress. (In this case, the optimization still typically returns a useful result.) "<<std::endl;
            break;
        case -3:
            std::cout<<" Ran out of memory. "<<std::endl;
            break;
        case -2:
            std::cout<<" Invalid arguments (e.g. lower bounds are bigger than upper bounds, an unknown algorithm was specified, etc.). "<<std::endl;
            break;
        case -1:
            std::cout<<" Generic failure code. "<<std::endl;
            break;
        case 1:
            std::cout<<" Generic success return value. "<<std::endl;
            break;
        case 2:
            std::cout<<" Optimization stopped because stopval was reached. "<<std::endl;
            break;
        case 3:
            std::cout<<" Optimization stopped because ftol_rel or ftol_abs was reached. "<<std::endl;
            break;
        case 4:
            std::cout<<" Optimization stopped because xtol_rel or xtol_abs was reached. "<<std::endl;
            break;
        case 5:
            std::cout<<" Optimization stopped because maxeval was reached. "<<std::endl;
            break;
        case 6:
            std::cout<<" Optimization stopped because maxtime was reached. "<<std::endl;
            break;
    }
}// end print_return_message

std::ostream& operator<<(std::ostream& stream, const optimization& data) {

    stream << data.OptAlgo<< "\n";
    stream << data.OptNormParam << " " << data.OptMaxEval << " " << data.OptDoWhat << " " << data.OptPrcParamDigits << " ";
    stream << data.OptEquilibrium << " " << data.OptGemsSIA << " " << data.OptTuckey << " " << data.OptTuckeyVal << " ";
    stream << data.OptTitration << " " << data.OptUserWeight << " " << data.OptMixedSumOfResiduals << "\n";
    stream << data.OptTolRel << " " << data.OptTolAbs << " " << data.OptInitStep<< " ";
    stream << data.OptBoundPerc << " " << data.OptPerturbator << " " << data.h_optNF << "\n";

    stream << "optParam\n";
    int ii=0;
    for (auto item : data.optParam) {
        stream << "parameter " << ii++ << "\n" << item << std::endl;
    }
    stream << "optNFParam\n";
    ii=0;
    for (auto item : data.optNFParam) {
        stream << "parameter " << ii++ << "\n" << item << "\n";
    }

    return stream;
}
