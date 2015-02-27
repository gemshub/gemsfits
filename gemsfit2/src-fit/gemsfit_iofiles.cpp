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
using namespace std;
#include <cstdlib>

#include "gemsfit_iofiles.h"
#include "v_user.h"
#include "io_arrays.h"
//#include "data_manager.h"
#include "json_parse.h"
#include "keywords.h"
#include <omp.h>
#include <sstream>
#include <unistd.h>
#include <cctype>
#include <iomanip>
#ifdef buildWIN32
        #include <tcejdb/bson.h>
#else
        #include <bson.h>
#endif


void generateBson (bson &bson_task_file, TNode *node, int mode);
void make_syn_bson_object (bson &bson_task_file, const char *key, int i, int mode);

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
            cout << gpf->GEMS3LstFilePath() << endl;
            cout<<" .. ERROR occurred while reading GEMS3K input files !!! ..."<<endl;
            return 1;
        }

    bool with_comments = true;
    bool brief_mode = false;

    bson bson_task_file;

    generateBson (bson_task_file, node, gpf->KeysNdx);

    string resStr;
    stringstream os;
    os << "{\n";
    resStr = os.str();
    bson_print_raw_txt( os, bson_task_file.data, 0, BSON_OBJECT);
    os << "}";
    resStr = os.str();


    string fname = gpf->OptParamFile();
    fstream ff(fname.c_str(), ios::out/*|ios::app*/ );
    ErrorIf( !ff.good() , fname.c_str(), "OptParamFile text open error");

    ff << resStr;

    ff.close();

//    cout << resStr << endl;

//    bson_print_raw(/*stderr,*/ bson_task_file.data, 0);


cout << "Finished writing the input specification file template" << endl;
//    }

    } catch(TError& err)
      {
        cout << "Error:" << err.title.c_str() << ":" <<  err.mess.c_str() << endl;
        return 1;
      }
      catch(...)
      {
        return -1;
      }
    return 0;
}

void generateBson(bson &bson_task_file,TNode *node, int mode)
{
    unsigned int Np = 0, NG0p = 0, NG0PH = 0, Nip = 0, Ncoef = 0, G0ndx=0, PMCndx = 0, DMCndx = 0, /*nIC,*//* nDC,*/ nPS, nPH; long int nDCinPH;
    int DCndx = -1;
    double temp = 0.0;
    stringstream ss; string sss, ipcn, dcipcn;
    bson_init(&bson_task_file);

    DATACH* dCH = node->pCSD();

//    nIC = dCH->nIC;	// nr of independent components
//    nDC = dCH->nDC;	// nr of dependent components
    nPS = dCH->nPS;
    nPH = dCH->nPH;

    bson_append_string(&bson_task_file, keys::DBPath[mode], "../EJDB/<database name>");
    bson_append_string(&bson_task_file, keys::DBColl[mode], "<collection name>");

    string path = "." + gpf->GEMS3LstFilePath();
    bson_append_string(&bson_task_file, keys::G3Ksys[mode], path.c_str() );
    bson_append_string(&bson_task_file, keys::DSelect[mode], "");
    bson_append_string(&bson_task_file, keys::DTarget[mode], "");

    bson_append_start_array(&bson_task_file, keys::OptParameters[mode]);

    // Write interaction parameters
    long int *LsMod = node->Get_LsMod();
    long int *LsMdc = node->Get_LsMdc();
//    bson_append_start_array(&bson_task_file, keys::PAM[mode]);

    int x=0;
    for (unsigned i = 0; i < nPH; i++)
    {

        NG0PH = dCH->nDCinPH[i];

        Nip = 0;
        ss << Np;
        sss = ss.str();
        ss.str("");
        bson_append_start_object(&bson_task_file, sss.c_str());
        {
            bson_append_string(&bson_task_file, keys::EPH[mode], node->xCH_to_PH_name(i));
            ipcn.append(node->xCH_to_PH_name(i)); ipcn.append("[");

            // G0
            bson_append_start_array(&bson_task_file, keys::G0[mode]);
            for (unsigned g = 0; g < NG0PH; g++)
            {
                ss << g;
                sss = ss.str();
                ss.str("");
                bson_append_start_object(&bson_task_file, sss.c_str());
                {
                    bson_append_string(&bson_task_file, keys::DCN[mode], node->xCH_to_DC_name(G0ndx));
                    bson_append_string(&bson_task_file, keys::PType[mode], "S");
                    bson_append_double(&bson_task_file, keys::IV[mode], node->DC_G0(G0ndx, 100000, 25 + 273.15, false));
                }
                bson_append_finish_object(&bson_task_file);
                NG0p++;
                G0ndx++;
            }
            bson_append_finish_array(&bson_task_file);
            NG0p = 0;


            if (((LsMod[i+x] > 0) || (LsMdc[i+x] > 0)) && (i < nPS))
            {
                // PMc parameters
                if ((LsMod[i+x] > 0))
                {
                    bson_append_start_array(&bson_task_file, keys::PMc[mode]);
                    for (unsigned j = 0; j < LsMod[i+x]; j++)
                    {
                        ss << Nip;
                        sss = ss.str();
                        ss.str("");
                        ipcn.append(sss); ipcn.append("|");
                        bson_append_start_object(&bson_task_file, sss.c_str());
                        {
                            // write IPC
                    //        bson_append_int(&bson_task_file, "IPndx", Nip);
                            bson_append_start_array(&bson_task_file, keys::IPCs[mode]);
                            Ncoef = 0;
                            for (unsigned k = 0; k < LsMod[i+x+2]; k++)
                            {
                                ss << Ncoef;
                                sss = ss.str();
                                ss.str("");
                                ipcn.append(sss); ipcn.append("]");
                                bson_append_start_object(&bson_task_file, sss.c_str());
                                {
                                    node->Get_PMc(temp, PMCndx);
                                    bson_append_string(&bson_task_file, keys::IPCN[mode], ipcn.c_str());
                                    bson_append_string(&bson_task_file, keys::PType[mode], "S");
                                    bson_append_double(&bson_task_file, keys::IV[mode], temp );
    //                                bson_append_int(&bson_task_file, keys::Pndx[mode], PMCndx );
                                    temp=0;
                                } // finish IPC
                                bson_append_finish_object(&bson_task_file);
                                Ncoef++;
                                PMCndx++;
                                ipcn.erase(ipcn.size()-2, ipcn.size());
                            }
                            bson_append_finish_array(&bson_task_file);
                        } // finish IParameters
                        bson_append_finish_object(&bson_task_file);
                        Nip++;
                        ipcn.erase(ipcn.size()-2, ipcn.size());
                    }
                    bson_append_finish_array(&bson_task_file);
                }

                // DMc parameters
                if (LsMdc[i+x] > 0)
                {
                    bson_append_start_array(&bson_task_file, keys::DMc[mode]);

                    DCndx = node->PhtoDC_DCH(i, nDCinPH);
                    for (unsigned k = 0; k < nDCinPH; k++)
                    {
                        ss << k;
                        sss = ss.str();
                        ss.str("");
                        bson_append_start_object(&bson_task_file, sss.c_str());
                        {
                            bson_append_string(&bson_task_file, keys::DCN[mode], node->xCH_to_DC_name(DCndx + k));
                            dcipcn.append(node->xCH_to_PH_name(i)); dcipcn.append("|");
                            dcipcn.append(node->xCH_to_DC_name(DCndx + k)); dcipcn.append("[");

                            // write IPDCoef
                    //        bson_append_int(&bson_task_file, "IPndx", Nip);
                            bson_append_start_array(&bson_task_file, keys::PDCC[mode]);
                            Ncoef = 0;
                            for (unsigned j = 0; j < LsMdc[i+x]; j++)
                            {
                                ss << Ncoef;
                                sss = ss.str();
                                ss.str("");
                                dcipcn.append(sss);dcipcn.append("]");
                                bson_append_start_object(&bson_task_file, sss.c_str());
                                {
                                    node->Get_DMc(temp, DMCndx);
                                    bson_append_string(&bson_task_file, keys::IPCN[mode], dcipcn.c_str());
                                    bson_append_string(&bson_task_file, keys::PType[mode], "S");
                                    bson_append_double(&bson_task_file, keys::IV[mode], temp );
    //                                bson_append_int(&bson_task_file, keys::Pndx[mode], DMCndx );
                                    temp=0;
                                } // finish IPDCoef
                                bson_append_finish_object(&bson_task_file);
                                Ncoef++;
                                DMCndx++;
                                dcipcn.erase(dcipcn.size()-2, dcipcn.size());
                            }
                            bson_append_finish_array(&bson_task_file);
                            dcipcn.clear();
                        }
                        bson_append_finish_object(&bson_task_file);
                    }
                    bson_append_finish_array(&bson_task_file);
                }
            }
            ipcn.clear();
            x +=2;
        } // finish object
        bson_append_finish_object(&bson_task_file);
        Np++;
    } // finish Phases array
//    bson_append_finish_array(&bson_task_file);

    // finish Parameters to Optimize
    bson_append_finish_array(&bson_task_file);


    // Nested function parameters
    bson_append_string(&bson_task_file, keys::OptNFParameters[mode], "");

    bson_append_string(&bson_task_file, keys::DatLogK[mode], "");

    bson_append_string(&bson_task_file, keys::LogK[mode], "");

    // Optimization Settings
    bson_append_start_object(&bson_task_file, keys::OptSet[mode]);
    {
        bson_append_int(&bson_task_file, keys::MPI[mode], omp_get_num_threads() * omp_get_num_procs());

        bson_append_int(&bson_task_file, keys::OptDW[mode], 1);

        bson_append_int(&bson_task_file, keys::OptEQ[mode], 1);

        bson_append_int(&bson_task_file, keys::OptUW[mode], -1);

        bson_append_string(&bson_task_file, keys::OptAlg[mode], "LN_BOBYQA");

        bson_append_double(&bson_task_file, keys::OptPBP[mode], -1);

        bson_append_double(&bson_task_file, keys::OptTRel[mode], 1e-5);

        bson_append_double(&bson_task_file, keys::OptTAbs[mode], 1e-5);

        bson_append_int(&bson_task_file, keys::OptMEv[mode], 10000);

        bson_append_int(&bson_task_file, keys::OptNormP[mode], 1);

        bson_append_double(&bson_task_file, keys::OptPer[mode], 0.0001);

         // Statistics Settings
        bson_append_double(&bson_task_file, keys::StatPer[mode], 0.0001);

        bson_append_int(&bson_task_file, keys::OptTu[mode], -1);

        bson_append_int(&bson_task_file, keys::OptTuVal[mode], 6);

        bson_append_int(&bson_task_file, keys::StatMC[mode], -1);

        bson_append_int(&bson_task_file, keys::StatMCr[mode], 100);

    } bson_append_finish_object(&bson_task_file);

    G0ndx = 0;

    bson_append_start_object(&bson_task_file, keys::DataSyn[mode]);
    {
        bson_append_start_array(&bson_task_file, keys::PhNames[mode]);
        {
            for (unsigned i = 0; i<nPH; i++)
            {
                make_syn_bson_object(bson_task_file, node->xCH_to_PH_name(i), i , mode);
                // DC
                NG0PH = dCH->nDCinPH[i];
                bson_append_start_array(&bson_task_file, keys::DcNames[mode]);
                {
                    for (unsigned j = 0; j<NG0PH; j++)
                    {
                        make_syn_bson_object(bson_task_file, node->xCH_to_DC_name(G0ndx), j, mode);
                        bson_append_finish_object(&bson_task_file);
                        G0ndx++;
                    }
                }
                bson_append_finish_array(&bson_task_file);
                // finish phase object
                bson_append_finish_object(&bson_task_file);
            }
        }
        bson_append_finish_array(&bson_task_file);

        bson_append_start_array(&bson_task_file, keys::PhPropNames[mode]);
        {
            unsigned i = 0;
            make_syn_bson_object(bson_task_file, keys::pH, i, mode); i++; bson_append_finish_object(&bson_task_file);

            make_syn_bson_object(bson_task_file, keys::pe, i, mode); i++; bson_append_finish_object(&bson_task_file);

            make_syn_bson_object(bson_task_file, keys::Eh, i, mode); i++; bson_append_finish_object(&bson_task_file);

            make_syn_bson_object(bson_task_file, keys::IS, i, mode); i++; bson_append_finish_object(&bson_task_file);

            make_syn_bson_object(bson_task_file, keys::all, i, mode); i++; bson_append_finish_object(&bson_task_file);

            make_syn_bson_object(bson_task_file, keys::pV, i, mode); i++; bson_append_finish_object(&bson_task_file);

            make_syn_bson_object(bson_task_file, keys::RHO, i, mode); i++; bson_append_finish_object(&bson_task_file);

            make_syn_bson_object(bson_task_file, keys::sArea, i, mode); i++; bson_append_finish_object(&bson_task_file);

            make_syn_bson_object(bson_task_file, keys::Gex, i, mode); i++; bson_append_finish_object(&bson_task_file);

            make_syn_bson_object(bson_task_file, keys::mChainL, i, mode); i++; bson_append_finish_object(&bson_task_file);
        }
        bson_append_finish_array(&bson_task_file);

//        bson_append_start_array(&bson_task_file, keys::DcNames[mode]);
//        {
//            for (unsigned i = 0; i<nDC; i++)
//            {
//                make_syn_bson_object(bson_task_file, node->xCH_to_DC_name(i), i, mode);
//            }
//        }
//        bson_append_finish_array(&bson_task_file);

        bson_append_start_array(&bson_task_file, keys::DcPropNames[mode]);
        {
            unsigned i = 0;
            make_syn_bson_object(bson_task_file, keys::activity, i, mode); i++; bson_append_finish_object(&bson_task_file);

            make_syn_bson_object(bson_task_file, keys::actcoef, i, mode); i++; bson_append_finish_object(&bson_task_file);
        }
    }
    bson_append_finish_object(&bson_task_file);

    bson_finish(&bson_task_file);
}

void make_syn_bson_object (bson &bson_task_file, const char* key, int i, int mode)
{
    stringstream ss; string sss;
    ss << i;
    sss = ss.str();
    ss.str("");
    //pH
    bson_append_start_object(&bson_task_file, sss.c_str());
    {
        bson_append_string(&bson_task_file, keys::NameSys[mode], key);
        bson_append_string(&bson_task_file, keys::Syn[mode], "");

    }
}


void bson_print_raw_txt( iostream& osx, const char *data, int depth, int datatype )
{
    bson_iterator i;
    const char *key;
    int temp;
    bson_timestamp_t ts;
    char oidhex[25];
    bson scope;
    bool first = true;

    bson_iterator_from_buffer(&i, data);
    while (bson_iterator_next(&i))
    {
        bson_type t = bson_iterator_type(&i);
        if (t == 0)
          break;
        if( t == BSON_OID )
          continue;

        if(!first )
         osx <<  ",\n";
        else
         first = false;

        key = bson_iterator_key(&i);

         // before print
        bool nr = true;
        switch( datatype )
        {
         case BSON_OBJECT:
           for (temp = 0; temp <= depth; temp++)
             osx <<  "     ";
           osx << "\"" << key << "\" :   ";
           break;
         case BSON_ARRAY:
            for (temp = 0; temp <= depth; temp++)
              osx << "     ";
            for (unsigned x = 0; x <strlen(key); x++)
            {
            if (!isdigit(key[x])) nr = false;
            }
            if (!nr) osx << "\"" << key << "\" :   ";
            break;
         default:
            break;
        }

        switch (t)
        {
          // impotant datatypes
          case BSON_NULL:
               osx << "null";
              break;
          case BSON_BOOL:
               osx << ( bson_iterator_bool(&i) ?  "true": "false");
               break;
          case BSON_INT:
               osx << bson_iterator_int(&i);
               break;
          case BSON_LONG:
               osx << bson_iterator_long(&i);
               break;
          case BSON_DOUBLE:
               osx << setprecision(15) << bson_iterator_double(&i);
               break;
          case BSON_STRING:
               osx << "\"" << bson_iterator_string(&i) << "\"";
                break;

          // main constructions
          case BSON_OBJECT:
             osx << "{\n";
             bson_print_raw_txt( osx, bson_iterator_value(&i), depth + 1, BSON_OBJECT);
             for (temp = 0; temp <= depth; temp++)
               osx << "     ";
             osx << "}";
             break;
          case BSON_ARRAY:
              osx << "[\n";
              bson_print_raw_txt(osx, bson_iterator_value(&i), depth + 1, BSON_ARRAY );
               for (temp = 0; temp <= depth; temp++)
                 osx << "     ";
               osx << "]";
               break;

           // not used in GEMS data types
              case BSON_SYMBOL:
              //       os<<  "SYMBOL: " << bson_iterator_string(&i);
                     break;
              case BSON_OID:
              //       bson_oid_to_string(bson_iterator_oid(&i), oidhex);
              //       os << oidhex;
                     break;
              case BSON_DATE:
              //       char buf[100];
              //       tcdatestrhttp(bson_iterator_time_t(&i), INT_MAX, buf);
              //       os << "\"" << buf <<"\"";
                     break;
              case BSON_BINDATA:
              //       os << "BSON_BINDATA";
                     break;
              case BSON_UNDEFINED:
              //      os << "BSON_UNDEFINED";
                     break;
              case BSON_REGEX:
              //       os << "BSON_REGEX: " << bson_iterator_regex(&i);
                     break;
              case BSON_CODE:
              //       os << "BSON_CODE: " << bson_iterator_code(&i);
                     break;
              case BSON_CODEWSCOPE:
              //       os << "BSON_CODE_W_SCOPE: " << bson_iterator_code(&i);
              //       bson_iterator_code_scope(&i, &scope);
              //       os << "\n      SCOPE: ";
              //       bson_print_raw_txt( os, (const char*) &scope, 0, BSON_CODEWSCOPE);
                     break;
               case BSON_TIMESTAMP:
              //       ts = bson_iterator_timestamp(&i);
              //       os <<  "i: " << ts.i << ", t: " << ts.t;
                     break;
               default:
                     osx  << "can't print type : " << t;
        }
    }
    osx << "\n";
}

//----------------------------------------------------------------
// TGfitPath  class implementation
//----------------------------------------------------------------
//----- subfolder and default file names  ------------------------
string INPUT_DIR = "input/";
string OUTPUT_DIR = "output/";
string RESULT_DIR = "results/";
//const char *OPT_PARAM_FILE = "gemsfit2_input.dat";
string FIT_CSV_FILE = "fit-results.csv";
string FIT_NFUN_FILE = "fit-inverse-results.csv";
string FIT_QQ_FILE = "qq-plot-data.csv";
string FIT_SENS_FILE = "meas-data-sensitivity.csv";
string FIT_MC_FILE = "mc-results.csv";
string FIT_PARAM_FILE = "fit-params.csv";
string FIT_STATISTIC = "sum-statistics.csv";
string FIT_LOGFILE = "gemsfit2.log";

TGfitPath::TGfitPath(int c, char *v[]):
        argc(c), argv(v)
{
    optParamFilePath = "";
    gems3LstFilePath = "";

    char cur_dir[300];
        // let's try to find resources by path of the executable
    getcwd(cur_dir, 300);
    cout << cur_dir << endl;
    for (int ii = 1; ii < argc; ii++)
     cout << ii << " arg " << argv[ii] << endl;

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
        cout << " USAGE: \n"
                "   gemsfit2  -help \n"
                "   gemsfit2  -run      <path to gemsfit2 input file> [ <path to GEMS3K input file list *-dat.lst> ] \n"
                "   gemsfit2  -init     <path to GEMS3K input file list *-dat.lst> [ <init file template name> ] \n\n"
                " WHERE: \n"
                "   -run:   runs the program with the settings from the input file \n"
                "   -init:  writes a template input file using the exported GEMS3K system files \n"
                "   -help:  displays this help for command-line options." << endl;
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

        string dir;

        if( optParamFile.empty() )
        {
            optParamFile = optParamFilePath;
            optParamFile += "00/gfin00_";
            dir = "00/";
            int pos = 0;
            string input_file = gems3LstFilePath, new_input;
            do
            {
                new_input = input_file.substr(pos+1, input_file.size());
                pos = new_input.find("/");
                input_file = new_input;
            } while (pos >-1);

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
            string name;
            string ext;
            u_splitpath( optParamFile, optParamFilePath, name, ext );
        }

        string path_init = optParamFilePath + dir;
        string path_run, path;

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
                flog << "GEMSFIT2: Start" << endl;
                flog << "optParamFile = " << optParamFile << endl;
                flog << "fitFile = " << fitFile << endl;
                flog << "fitLogFile = " << fitLogFile << endl;
                flog << "gems3LstFilePath = " << gems3LstFilePath << endl;
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

cout << "GEMSFIT2: Start" << endl;
cout << "optParamFile = " << optParamFile << endl;
cout << "fitFile = " << fitFile << endl;
cout << "fitLogFile = " << fitLogFile << endl;
cout << "gems3LstFilePath = " << gems3LstFilePath << endl;
    }
 }

TGfitPath::~TGfitPath()
{}

void TGfitPath::deleteOutputDir(const char *dir)
{
}


void TGfitPath::makeOutputDir(const char *dir)
{
}

TGfitPath *gpf;


// ----------- End of  visor.cpp ----------------------------
