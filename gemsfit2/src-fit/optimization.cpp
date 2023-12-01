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
#include "keywords.h"
#include "v_json.h"

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
    int mode = gpf->KeysNdx;

    fname = gpf->OptParamFile();

    std::ifstream file(fname.c_str());
    std::ostringstream tmp;
    tmp<<file.rdbuf();
    std::string s = tmp.str();

    GEMSsys = gpf->GEMS3LstFilePath();

    common::JsonFree db_opt_object = fromJsonString(s);
    OptParameters = db_opt_object.value( keys::OptParameters[mode], std::vector<std::string>{});
    if (OptParameters.size() == 0) {
          std::cout << "Error: No keyword for \""
          << keys::OptParameters[mode] <<"\" found in the task definition" << std::endl;
          exit(1);
    }
    NFunParameters = db_opt_object.value(keys::OptNFParameters[mode], std::string());
    AddOptParameters = db_opt_object.value(keys::AddOptParameters[mode], std::string());

    // Optimization settings and statistics
    auto& opt_set_object =  db_opt_object[keys::OptSet[mode]];
    OptDoWhat = opt_set_object.value(keys::OptDW[mode], OptDoWhat);
    OptPrcParamDigits = opt_set_object.value( keys::OptPrcParamDigits[mode], OptPrcParamDigits);
    OptEquilibrium = opt_set_object.value(keys::OptEQ[mode], OptEquilibrium);
    OptGemsSIA = opt_set_object.value(keys::SIA[mode], OptGemsSIA);
    OptUserWeight = opt_set_object.value(keys::OptUW[mode], OptUserWeight);
    OptMixedSumOfResiduals = opt_set_object.value(keys::OptMixedResiduals[mode], OptMixedSumOfResiduals);
    OptTuckey = opt_set_object.value(keys::OptTu[mode], OptTuckey);
    OptTuckeyVal = opt_set_object.value(keys::OptTuVal[mode], OptTuckeyVal);
    OptAlgo = opt_set_object.value(keys::OptAlg[mode], OptAlgo);
    OptBoundPerc = opt_set_object.value(keys::OptPBP[mode], OptBoundPerc);
    OptTolRel = opt_set_object.value(keys::OptTRel[mode], OptTolRel);
    OptTolAbs = opt_set_object.value(keys::OptTAbs[mode], OptTolAbs);
    OptMaxEval = opt_set_object.value(keys::OptMEv[mode], OptMaxEval);
    OptNormParam = opt_set_object.value(keys::OptNormP[mode], 0);
    OptPerturbator = opt_set_object.value(keys::OptPer[mode], OptPerturbator);
}

void optimization::OptParameterCreate ()
{
    unsigned  p = 0;
    std::vector<std::string> temp, outG0, outPMc, outDMc;
    OptParameter* myOPT = 0;

    // call GEM_init to read GEMS3K input files
    std::shared_ptr<TNode> node  = std::make_shared<TNode>();

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
    if (OptParameters.size() > 0)
    {
        for (unsigned i = 0; i < OptParameters.size(); i++)
        {
            common::JsonFree object = fromJsonString(OptParameters[i]);
            // G0
            temp = object.value(keys::G0[mode], std::vector<std::string>{});
            outG0.insert(outG0.end(), temp.begin(), temp.end());
            // PMc
            if( object.contains(keys::PMc[mode]) ) {
                for (const auto& element : object[keys::PMc[mode]]) {
                    temp = element->value(keys::IPCs[mode], std::vector<std::string>{});
                    outPMc.insert(outPMc.end(), temp.begin(), temp.end());
                }
            }
            // DMc
            if( object.contains(keys::DMc[mode]) ) {
                for (const auto& element : object[keys::DMc[mode]]) {
                    temp = element->value(keys::PDCC[mode], std::vector<std::string>{});
                    outDMc.insert(outDMc.end(), temp.begin(), temp.end());
                }
            }
        }

        if (outG0.size() > 0) {
            Opt_G0* myPT = new Opt_G0(outG0, OptBoundPerc, p);
            myPT->SetIndex_param(node.get());
            myOPT = (Opt_G0*)myPT;
        }
        if(myOPT) { optParam.push_back( std::shared_ptr<OptParameter>(myOPT) ); myOPT = 0; };
         p=0;

        // PMc
        if (outPMc.size() > 0) {
            Opt_PMc* myPT = new Opt_PMc(outPMc, OptBoundPerc, p);
//            myPT->SetIndex_param(node);
            myOPT = (Opt_PMc*)myPT;
        }
        if(myOPT) { optParam.push_back( std::shared_ptr<OptParameter>(myOPT) ); myOPT = 0; };
         p=0;

        // DMc
        if (outPMc.size() > 0) {
            Opt_DMc* myPT = new Opt_DMc(outDMc, OptBoundPerc, p);
//            myPT->SetIndex_param(node);
            myOPT = (Opt_DMc*)myPT;
        }
        if(myOPT) { optParam.push_back( std::shared_ptr<OptParameter>(myOPT) ); myOPT = 0; };
         p=0;
    }

    // NFUN parameters
    if (NFunParameters != "")
    {
        common::JsonFree object = fromJsonString(NFunParameters);
        temp = object.value(keys::bIC[mode], std::vector<std::string>{});
        if (temp.size() > 0) {
            Opt_bIC* myPT = new Opt_bIC(temp, OptBoundPerc, p );
            myPT->SetIndex_param(node.get());
            myOPT = (Opt_bIC*)myPT;
        }
        if(myOPT) { optNFParam.push_back( std::shared_ptr<OptParameter>(myOPT) ); myOPT = 0; };
        p=0;

        temp = object.value(keys::TK[mode], std::vector<std::string>{});
        if (temp.size() > 0) {
            Opt_Tk* myPT = new Opt_Tk( temp, OptBoundPerc, p );
            myPT->SetIndex_param(node.get());
            myOPT = (Opt_Tk*)myPT;
        }
        if(myOPT) { optNFParam.push_back( std::shared_ptr<OptParameter>(myOPT) ); myOPT = 0; };
        p=0;

        temp = object.value(keys::Pb[mode], std::vector<std::string>{});
        if (temp.size() > 0) {
            Opt_P* myPT = new Opt_P(temp, OptBoundPerc, p);
            myPT->SetIndex_param(node.get());
            myOPT = (Opt_P*)myPT;
        }
        if(myOPT) { optNFParam.push_back( std::shared_ptr<OptParameter>(myOPT) ); myOPT = 0; };
        p=0;
    }

    // Additional parameters
    if (AddOptParameters != "")
    {
        common::JsonFree object = fromJsonString(AddOptParameters);
        temp = object.value(keys::bIC[mode], std::vector<std::string>{});
        if (temp.size() > 0) {
            Opt_bIC* myPT = new Opt_bIC(temp, OptBoundPerc, p, false);
            myPT->SetIndex_param(node.get());
            myOPT = (Opt_bIC*)myPT;
        }
        if(myOPT) { optParam.push_back( std::shared_ptr<OptParameter>(myOPT) ); myOPT = 0; };
        p=0;
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
