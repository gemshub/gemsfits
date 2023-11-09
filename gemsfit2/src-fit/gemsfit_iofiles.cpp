/*
*	 Copyright (C) 2013 by Dmytriyeva S. (gems2.support@psi.ch)
*    modified G. Dan Miron
*
*	 This file is part of the thermodynamic fitting program GEMSFIT2.
*
*    GEMSFIT2 is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    GEMSFIT2 is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU  General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with GEMSFIT2.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
//using namespace std;
#include <cstdlib>

#include "gemsfit_iofiles.h"
#include "keywords.h"
#include "v_service.h"
#include "v_json.h"
#include <omp.h>
#ifdef __unix
#include <unistd.h>
#include <sys/time.h>
#else
#include <io.h>
#ifdef _MSC_VER
#include <direct.h>
#endif
#endif


common::JsonFree generateJson(TNode *node, int mode);

/// Mode GEMSFIT to generate input configuration file
int generateJConfig()
{
    try
    {
        // call GEM_init to read GEMS3K input files
        TNode* node  = new TNode();

        // call GEM_init     --> read in input files
        if( (node->GEM_init( gpf->GEMS3LstFilePath().c_str() )) == 1 )
        {
            std::cout << gpf->GEMS3LstFilePath() << std::endl;
            std::cout<<" .. ERROR occurred while reading GEMS3K input files !!! ..."<<std::endl;
            //return 1;
        }

        bool with_comments = true;
        bool brief_mode = false;
        auto json_obj = generateJson(node, gpf->KeysNdx);

        std::string fname = gpf->OptParamFile();
        std::fstream ff(fname.c_str(), std::ios::out/*|ios::app*/ );
        ErrorIf( !ff.good(), fname.c_str(), "OptParamFile text open error");

        ff << json_obj.dump();
        ff.close();
        std::cout << "Finished writing the input specification file template" << std::endl;

    } catch(TError& err)
    {
        std::cout << "Error:" << err.title.c_str() << ":" <<  err.mess.c_str() << std::endl;
        return 1;
    }
    catch(...)
    {
        return -1;
    }
    return 0;
}

int NumDigits(int x)
{
    x = abs(x);
    return (x < 10 ? 1 :
                     (x < 100 ? 2 :
                                (x < 1000 ? 3 :
                                            (x < 10000 ? 4 :
                                                         (x < 100000 ? 5 :
                                                                       (x < 1000000 ? 6 :
                                                                                      (x < 10000000 ? 7 :
                                                                                                      (x < 100000000 ? 8 :
                                                                                                                       (x < 1000000000 ? 9 :
                                                                                                                                         10)))))))));
}

enum eTableType
{
    bet0_ = -10, bet1_ = -11, bet2_ = -12, Cphi_ = -20, Lam_ = -30, Lam1_ = -31, Lam2_ = -32,
    Theta_ = -40,  Theta1_ = -41, Psi_ = -50, Psi1_ = -51, Zeta_ = -60
};

std::string getPitzerIPName(TNode *node, std::vector<long int> aIPx, long int MaxOrd, int iIP)
{
    std::string name="";
    switch( aIPx[iIP * MaxOrd + 3] )  // type of table
    {
    case bet0_:
        name += "beta0_";
        goto binary;
        break;
    case bet1_:
        name += "beta1_";
        goto binary;
        break;

    case bet2_:
        name += "beta2_";
        goto binary;
        break;
    case Cphi_:
        name += "cphi_";
        goto binary;
        break;

    case Lam_:
        name += "lambda_";
        goto binary;
        break;

    case Lam1_:
        name += "lambda_";
        goto binary;
        break;

    case Lam2_:
        name += "lambda_";
        goto binary;
        break;

    case Theta_:
        name += "theta_";
        goto binary;
        break;

    case Theta1_:
        name += "theta_";
        goto binary;
        break;

    case Psi_:
        name += "psi_";
        goto ternary;
        break;

    case Psi1_:
        name += "psi_";
        goto ternary;
        break;

    case Zeta_:
        name += "zeta_";
        goto ternary;
        break;
    default:
        break;
    }

binary:
    name += node->xCH_to_DC_name(aIPx[iIP * MaxOrd + 0]);
    name += "_";
    name += node->xCH_to_DC_name(aIPx[iIP * MaxOrd + 1]);
    return name;
ternary:
    name += node->xCH_to_DC_name(aIPx[iIP * MaxOrd + 0]);
    name += "_";
    name += node->xCH_to_DC_name(aIPx[iIP * MaxOrd + 1]);
    name += "_";
    name += node->xCH_to_DC_name(aIPx[iIP * MaxOrd + 2]);
    return name;

}

common::JsonFree make_syn_object(const char* key, int mode)
{
    common::JsonFree object;
    object[keys::NameSys[mode]] = key;
    object[keys::Syn[mode]] = "";
    return object;
}

common::JsonFree make_UP_LB(double temp, int mode, const std::string& key1,
                            const std::string& val1,const std::string& val2)
{
    common::JsonFree object;
    object[key1] = val1;
    object[keys::PType[mode]] = val2;
    object[keys::IV[mode]] = temp;
    object[keys::UB[mode]] = temp;
    object[keys::LB[mode]] = temp;
    return object;
}

common::JsonFree generateJson(TNode *node, int mode)
{
    common::JsonFree object;

    unsigned int Np = 0, NG0p = 0, NG0PH = 0, Nip = 0, Ncoef = 0, G0ndx=0, ICndx=0,
            PMCndx = 0, DMCndx = 0,/* nIC, nDC,*/ nPS, nPH;
    long int nDCinPH;
    int DCndx = -1;
    double temp = 0.0;
    std::string ipcn, dcipcn;

    DATACH* dCH = node->pCSD();
    nPS = dCH->nPS;
    nPH = dCH->nPH;

    object[keys::DBPath[mode]] = "../EJDB/<database name>";
    object[keys::DBColl[mode]] = "<collection name>";

    std::string path = "." + gpf->GEMS3LstFilePath();
#ifdef buildWIN32
    std::replace( path.begin(), path.end(), '\\', '/');
#endif
    object[keys::G3Ksys[mode]] = path;
    object["comment"] = "Instead of current text, write here a short comment for the fitting task.";

    // Optimization Settings
    common::JsonFree opt_object;
    opt_object[keys::MPI[mode]] = (omp_get_num_threads() * omp_get_num_procs()-1);
    opt_object[keys::OptDW[mode]] = 1;
    opt_object[keys::OptPrcParamDigits[mode]] =  -1;
    opt_object[keys::OptEQ[mode]] =  1;
    opt_object[keys::SIA[mode]] = -1;
    opt_object[keys::OptUW[mode]] = -1;
    opt_object[keys::OptMixedResiduals[mode]] = -1;
    opt_object[keys::OptAlg[mode]] = "LN_BOBYQA";
    opt_object[keys::OptPBP[mode]] = -1;
    opt_object[keys::OptTRel[mode]] = 1e-5;
    opt_object[keys::OptTAbs[mode]] = 1e-5;
    opt_object[keys::OptMEv[mode]] = 10000;
    opt_object[keys::OptNormP[mode]] = 1;
    opt_object[keys::OptPer[mode]] = 0.0001;
    // Statistics Settings
    opt_object[keys::StatPer[mode]] = 0.0001;
    opt_object[keys::OptTu[mode]] = -1;
    opt_object[keys::OptTuVal[mode]] = 6;
    opt_object[keys::StatMC[mode]] = -1;
    opt_object[keys::StatMCr[mode]] = 100;
    object[keys::OptSet[mode]] = opt_object;

    // DataSelect
    common::JsonFree dsel_object;
    //dsel_object[keys::Info[mode]] = "What data to use in the fit? For example, provide a list of expdatasets in usedatasets.";
    dsel_object[keys::usedatasets] = common::JsonFree::array();
    dsel_object[keys::skipsamples] = common::JsonFree::array();
    dsel_object[keys::sT] = common::JsonFree::array();
    dsel_object[keys::sP] = common::JsonFree::array();
    object[keys::DSelect[mode]] = dsel_object;

    // DataTarget
    common::JsonFree dtar_object;
    //dtar_object[keys::Info[mode]] = "What data to compare. In OFUN provide a list of properties to use in the fit. Use ADDOUT for additional aoutput in the results table. NFUN is used to adjust properties of each sample nested withn the golobal fit.";
    dtar_object[keys::Target] = "lsqFit";
    dtar_object[keys::TT] =  "lsq";
    dtar_object[keys::WT] = "inverr3";
    dtar_object[keys::OFUN] = common::JsonFree::array();
    dtar_object[keys::NFUN] = common::JsonFree::array();
    dtar_object[ keys::ADDOUT] = common::JsonFree::array();
    object[keys::DTarget[mode]] = dtar_object;

    // Nested function parameters
    object[keys::OptNFParameters[mode]]="";
    // start additional params
    auto arr = common::JsonFree::array();
    //    for (unsigned i = 0; i < nIC; i++)
    //    {
    //        common::JsonFree it_object;
    //        it_object[keys::ICN[mode]] = node->xCH_to_IC_name(ICndx);
    //        it_object[keys::PType[mode]] = "S";
    //        it_object[keys::IV[mode]] = node->Get_bIC(ICndx);
    //        ICndx++;
    //        arr.push_back(std::move(it_object));
    //    }
    object[keys::AddOptParameters[mode]][keys::bIC[mode]]=arr;

    //std::string info = keys::OptParameters[mode];
    //std::string info_ = "info@"+info;
    //object[info_]="G0: Standard Gibbs energy at 25 C 1 bar; PMc: Non-ideal phase interaction parameters. DMC: parameters of multi-site (sublattice) model. To fit change parameter type from S (set) to F (fit). Adjust the UB upper and LB lower bound accordingly";

    auto ph_arr = common::JsonFree::array();
    // Write interaction parameters
    long int *LsMod = node->Get_LsMod();
    long int *LsMdc = node->Get_LsMdc();
    int x=0;
    for (unsigned i = 0; i < nPH; i++)
    {
        NG0PH = dCH->nDCinPH[i];
        Nip = 0;

        auto ph_obj = common::JsonFree::object();
        {
            ipcn.append(node->xCH_to_PH_name(i));
            ph_obj[keys::EPH[mode]] = ipcn;
            ipcn.append("[");

            // G0
            auto g0_arr = common::JsonFree::array();
            for (unsigned g = 0; g < NG0PH; g++)
            {
                temp = node->DC_G0(G0ndx, 100000, 25 + 273.15, false);
                g0_arr.push_back(make_UP_LB( temp, mode, keys::DCN[mode], node->xCH_to_DC_name(G0ndx), "S"));
                G0ndx++;
            }
            ph_obj[keys::G0[mode]] = g0_arr;

            if (((LsMod[i+x] > 0) || (LsMdc[i+x] > 0)) && (i < nPS))
            {
                long int ip_IPx=0; long int ip_IPc=0; long int ip_DCc=0;
                std::vector<long int> aIPx;
                node->Get_IPc_IPx_DCc_indices(ip_IPx,ip_IPc, ip_DCc, i );
                node->Get_aIPx(aIPx,ip_IPx, i);
                // PMc parameters
                if ((LsMod[i+x] > 0))
                {
                    auto PMc_arr = common::JsonFree::array();
                    for (unsigned j = 0; j < LsMod[i+x]; j++)
                    {
                        ipcn.append(std::to_string(Nip)); ipcn.append("|");
                        auto PMc_object = common::JsonFree::object();
                        {
                            // Get name of IP
                            std::string sMod;
                            node->Get_sMod(i, sMod);
                            if (sMod.compare(0,1,"Z")==0)
                            {
                                std::string IPName = getPitzerIPName(node, aIPx,LsMod[(i)*3+1], Nip);
                                PMc_object[keys::IPName[mode]] = IPName;
                            }
                            // write IPC
                            // PMc_object["IPndx"] = Nip;
                            auto pmc_arr = common::JsonFree::array();
                            Ncoef = 0;
                            for (unsigned k = 0; k < LsMod[i+x+2]; k++)
                            {
                                node->Get_PMc(temp, PMCndx);
                                pmc_arr.push_back(make_UP_LB( temp, mode, keys::IPCN[mode], ipcn+std::to_string(Ncoef)+"]", "S"));
                                temp=0;
                                PMCndx++;
                                Ncoef++;
                            }
                            PMc_object[keys::IPCs[mode]] = pmc_arr;
                        } // finish IParameters
                        PMc_arr.push_back(std::move(PMc_object));
                        ipcn.erase(ipcn.size()-(1+NumDigits(Nip)), ipcn.size());
                        Nip++;
                    }
                    ph_obj[keys::PMc[mode]] = PMc_arr;
                }

                // DMc parameters
                if (LsMdc[i+x] > 0)
                {
                    auto DMc_arr = common::JsonFree::array();
                    DCndx = node->PhtoDC_DCH(i, nDCinPH);
                    for (unsigned k = 0; k < nDCinPH; k++)
                    {
                        auto DMc_object = common::JsonFree::object();
                        {
                            DMc_object[keys::DCN[mode]] = node->xCH_to_DC_name(DCndx + k);
                            dcipcn.append(node->xCH_to_PH_name(i)); dcipcn.append("|");
                            dcipcn.append(node->xCH_to_DC_name(DCndx + k)); dcipcn.append("[");

                            // write IPDCoef
                            //        DMc_object["IPndx"] = Nip;
                            auto dmc_arr = common::JsonFree::array();
                            Ncoef = 0;
                            for (unsigned j = 0; j < LsMdc[i+x]; j++)
                            {
                                node->Get_DMc(temp, DMCndx);
                                dmc_arr.push_back(make_UP_LB( temp, mode, keys::IPCN[mode], dcipcn+std::to_string(Ncoef)+"]", "S"));
                                temp=0;
                                Ncoef++;
                                DMCndx++;
                            }
                            dcipcn.clear();
                            DMc_object[keys::PDCC[mode]] = dmc_arr;
                        }
                        DMc_arr.push_back(std::move(DMc_object));
                    }
                    ph_obj[keys::DMc[mode]] = DMc_arr;
                }
            }
            ipcn.clear();
            x +=2;
        } // finish object

        ph_arr.push_back(std::move(ph_obj));
        Np++;
    } // finish Phases array
    // finish Parameters to Optimize
    object[keys::OptParameters[mode]] = ph_arr;

    object[keys::DatLogK[mode]] = "";
    object[keys::LogK[mode]] = "";

    G0ndx = 0;
    auto DataSyn_obj = common::JsonFree::object();
    {
        auto ph_arr = common::JsonFree::array();
        {
            for (unsigned i = 0; i<nPH; i++)
            {
                auto syn_obj =  make_syn_object(node->xCH_to_PH_name(i), mode);
                // DC
                NG0PH = dCH->nDCinPH[i];
                auto syn_arr = common::JsonFree::array();
                {
                    for (unsigned j = 0; j<NG0PH; j++)
                    {
                        syn_arr.push_back(make_syn_object(node->xCH_to_DC_name(G0ndx), mode));
                        G0ndx++;
                    }
                }
                syn_obj[keys::DcNames[mode]] = syn_arr;
                ph_arr.push_back(std::move(syn_obj));
            }
        }
        DataSyn_obj[keys::PhNames[mode]] = ph_arr;

        auto syn2_arr = common::JsonFree::array();
        {
            syn2_arr.push_back(make_syn_object(keys::pH, mode));
            syn2_arr.push_back(make_syn_object(keys::pHm, mode));
            syn2_arr.push_back(make_syn_object(keys::pe, mode));
            syn2_arr.push_back(make_syn_object(keys::Eh, mode));
            syn2_arr.push_back(make_syn_object(keys::IS, mode));
            syn2_arr.push_back(make_syn_object(keys::SI, mode));
            syn2_arr.push_back(make_syn_object(keys::all, mode));
            syn2_arr.push_back(make_syn_object(keys::pV, mode));
            syn2_arr.push_back(make_syn_object(keys::RHO, mode));
            syn2_arr.push_back(make_syn_object(keys::sArea, mode));
            syn2_arr.push_back(make_syn_object(keys::Gex, mode));
            syn2_arr.push_back(make_syn_object(keys::mChainL, mode));
            syn2_arr.push_back(make_syn_object(keys::expr, mode));
            syn2_arr.push_back(make_syn_object(keys::activityRatio, mode));
            syn2_arr.push_back(make_syn_object(keys::frAlIV, mode));
            syn2_arr.push_back(make_syn_object(keys::frAlV, mode));
            syn2_arr.push_back(make_syn_object(keys::frAlVI, mode));
            syn2_arr.push_back(make_syn_object(keys::Rd, mode));
        }
        DataSyn_obj[keys::PhPropNames[mode]] = syn2_arr;

        //        auto syn4_arr = common::JsonFree::array();
        //        {
        //            for (unsigned i = 0; i<nDC; i++)
        //                syn4_arr.push_back(make_syn_object(node->xCH_to_DC_name(i), mode));
        //        }
        //        DataSyn_obj[keys::DcNames[mode]] = syn4_arr;

        auto syn3_arr = common::JsonFree::array();
        {
            syn3_arr.push_back(make_syn_object(keys::activity, mode));
            syn3_arr.push_back(make_syn_object(keys::actcoef, mode));
        }
        DataSyn_obj[keys::DcPropNames[mode]] = syn3_arr;
    }
    object[keys::DataSyn[mode]] = DataSyn_obj;
    return object;
}

//----------------------------------------------------------------
// TGfitPath  class implementation
//----------------------------------------------------------------
//----- subfolder and default file names  ------------------------
std::string INPUT_DIR = "input/";
std::string OUTPUT_DIR = "output/";
std::string RESULT_DIR = "results/";
//const char *OPT_PARAM_FILE = "gemsfit2_input.dat";
std::string FIT_CSV_FILE = "fit-results.csv";
std::string FIT_NFUN_FILE = "fit-inverse-results.csv";
std::string FIT_QQ_FILE = "qq-plot-data.csv";
std::string FIT_SENS_FILE = "meas-data-sensitivity.csv";
std::string FIT_MC_FILE = "mc-results.csv";
std::string FIT_PARAM_FILE = "fit-params.csv";
std::string FIT_STATISTIC = "sum-statistics.csv";
std::string FIT_LOGFILE = "gemsfit2.log";

TGfitPath::TGfitPath(int c, char *v[]):
    argc(c), argv(v)
{
    optParamFilePath = "";
    gems3LstFilePath = "";

    char cur_dir[300];
    // let's try to find resources by path of the executable
#ifdef __unix
    getcwd(cur_dir, 300);
#else
    _getcwd(cur_dir, 300);
#endif

    std::cout << cur_dir << std::endl;
    for (int ii = 1; ii < argc; ii++)
        std::cout << ii << " arg " << argv[ii] << std::endl;

    // parsing options -init, -run, -conf if given

    int iinit = 0;		// index of -init option
    int iinitJ = 0;
    int irun = 0;		// index of -run option
    int iconf = 0;		// index of -conf option
    int ihelp = 0;      // index of -help option

    for (int ii = 1; ii < argc; ii++)
    {
        if (strcmp(argv[ii], "-init") == 0 )
            iinit = ii;
        else if (strcmp(argv[ii], "-run") == 0 )
            irun = ii;
        else if (strcmp(argv[ii], "-conf") == 0)
            iconf = ii;
        else if (strcmp(argv[ii], "-help") == 0)
            ihelp = ii;
        else if (strcmp(argv[ii], "-initJ") == 0)
            iinitJ = ii;
    }

    if (ihelp !=0)
    {
        std::cout << " USAGE: \n"
                     "   gemsfit2  -help \n"
                     "   gemsfit2  -run      <path to gemsfit2 input file> [ <path to GEMS3K input file list *-dat.lst> ] \n"
                     "   gemsfit2  -init     <path to GEMS3K input file list *-dat.lst> [ <init file template name> ] \n\n"
                     " WHERE: \n"
                     "   -run:   runs the program with the settings from the input file \n"
                     "   -init:  writes a template input file using the exported GEMS3K system files \n"
                     "   -help:  displays this help for command-line options." << std::endl;
        mode = HELP_;
        //        return 0;
    } else
    {
        if (irun != 0)
        {
            if (argc <= irun + 2)
                Error("Wrong options", "Wrong argument for option -run");
            optParamFile = argv[irun + 2];

            KeysNdx = atoi(argv[irun + 1]);
            mode = RUN_;
        }

        if (iinit != 0)
        {
            if (argc <= iinit + 1)
                Error("Wrong options", "Wrong argument for option -init");
            gems3LstFilePath = argv[iinit+1];
            mode = INIT_;
            if (argc > iinit + 2)  // Optional: file name for the GEMSFIT2 init file template
            {
                optParamFile = optParamFilePath;
                //                optParamFile += "/";
                optParamFile += argv[iinit+2];
            }
        }

        if (iinitJ != 0)
        {
            if (argc <= iinitJ + 1)
                Error("Wrong options", "Wrong argument for option -init");
            gems3LstFilePath = argv[iinitJ+2];
            mode = INITJ_;
            if (argc > iinitJ + 3)  // Optional: file name for the GEMSFIT2 init file template
            {
                optParamFile = optParamFilePath;
                //                optParamFile += "/";
                optParamFile += argv[iinitJ+3];
            }
            KeysNdx = atoi(argv[iinitJ + 1]);
        }

        if (iconf != 0)  // needs reconsideration
        {
            if (argc <= iconf + 1)
                Error("Wrong options", "Wrong argument for option -conf");
            optParamFile = argv[iconf + 1];
        }

        std::string dir;

        if( optParamFile.empty() )
        {
            optParamFile = optParamFilePath;
            optParamFile += "work/gfin00_";
            dir = "work/";
            int pos = 0;
            std::string input_file = gems3LstFilePath, new_input;
            if(!input_file.empty())
            {
                do
                {
                    new_input = input_file.substr(pos+1, input_file.size());
                    pos = new_input.find("/");
                    input_file = new_input;
                } while (pos >-1);
            }
            pos = input_file.find("-dat.lst");
            if (pos <0)
            {
                pos = input_file.find(".lst");
                if (pos < 0)
                {
                    Error("Wrong file name", "Wrong name for the GEMS3K system file *.lst");
                }
            }


            optParamFile += input_file.substr(0, pos);
            optParamFile += ".json";
            pos = gems3LstFilePath.find(input_file.c_str());
            new_input = gems3LstFilePath.substr(0, gems3LstFilePath.size() - input_file.size());

        }
        else
        {
            std::string name;
            std::string ext;
            u_splitpath( optParamFile, optParamFilePath, name, ext );
        }

        std::string path_init = optParamFilePath + dir;
        std::string path_run, path;

        switch( mode )
        {
        case INIT_:
            // set up default paths
            // later these paths and filenames can be read from the task file
#ifdef buildWIN32
            mkdir(path_init.c_str());
#else
            mkdir(path_init.c_str(), 0775);
#endif
            outputDir = path_init + OUTPUT_DIR;
            fitsens = outputDir+FIT_SENS_FILE;
            fitFile = outputDir+FIT_CSV_FILE;
            fitnfun = outputDir+FIT_NFUN_FILE;
            fitqq = outputDir+FIT_QQ_FILE;
            fitparam = outputDir+FIT_PARAM_FILE;
            fitmc = outputDir+FIT_MC_FILE;
            fitStatistics = outputDir+FIT_STATISTIC;
            fitLogFile = outputDir+FIT_LOGFILE;
            break;
        case INITJ_:
            // set up default paths
            // later these paths and filenames can be read from the task file
#ifdef buildWIN32
            mkdir(path_init.c_str());
#else
            mkdir(path_init.c_str(), 0775);
#endif
            outputDir = path_init + OUTPUT_DIR;
            fitsens = outputDir+FIT_SENS_FILE;
            fitFile = outputDir+FIT_CSV_FILE;
            fitnfun = outputDir+FIT_NFUN_FILE;
            fitqq = outputDir+FIT_QQ_FILE;
            fitparam = outputDir+FIT_PARAM_FILE;
            fitmc = outputDir+FIT_MC_FILE;
            fitStatistics = outputDir+FIT_STATISTIC;
            fitLogFile = outputDir+FIT_LOGFILE;
            break;
        case RUN_:
            // set up default paths
            // later these paths and filenames can be read from the task file
            outputDir = path_run + OUTPUT_DIR;
            fitFile = outputDir+FIT_CSV_FILE;
            fitsens = outputDir+FIT_SENS_FILE;
            fitnfun = outputDir+FIT_NFUN_FILE;
            fitqq = outputDir+FIT_QQ_FILE;
            fitparam = outputDir+FIT_PARAM_FILE;
            fitmc = outputDir+FIT_MC_FILE;
            fitStatistics = outputDir+FIT_STATISTIC;
            fitLogFile = outputDir+FIT_LOGFILE;
            // GEMSFIT logfile
            flog << "GEMSFIT2: Start" << std::endl;
            flog << "optParamFile = " << optParamFile << std::endl;
            flog << "fitFile = " << fitFile << std::endl;
            flog << "fitLogFile = " << fitLogFile << std::endl;
            flog << "gems3LstFilePath = " << gems3LstFilePath << std::endl;
            flog.close();
            break;
        default:
            // set up default paths
            // later these paths and filenames can be read from the task file
            inputDir = path + INPUT_DIR;
            outputDir = path + OUTPUT_DIR;
            fitFile = outputDir+FIT_CSV_FILE;
            fitStatistics = outputDir+FIT_STATISTIC;
            fitLogFile = outputDir+FIT_LOGFILE;
        }

        std::cout << "GEMSFIT2: Start" << std::endl;
        std::cout << "optParamFile = " << optParamFile << std::endl;
        std::cout << "fitFile = " << fitFile << std::endl;
        std::cout << "fitLogFile = " << fitLogFile << std::endl;
        std::cout << "gems3LstFilePath = " << gems3LstFilePath << std::endl;
    }
}

TGfitPath::~TGfitPath()
{}

void TGfitPath::deleteOutputDir(const char */*dir*/)
{
}


void TGfitPath::makeOutputDir(const char */*dir*/)
{
}

TGfitPath *gpf;


// ----------- End of  visor.cpp ----------------------------
