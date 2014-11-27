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
#include <bson.h>

//#include "statistics.h"
//#include "optimization.h"

void generateBson (bson &bson_task_file, TNode *node, int mode);
void make_syn_bson_object (bson &bson_task_file, const char *key, int i, int mode);

//optimization::optimization( int i )
//{
//    int ii;
//    constraint_data = new my_constraint_data;
//    optimization::define_nlopt_param();
//    ii=i;
//}

// Constructor
//statistics::statistics()
//{
//    number_of_measurements = 0;
//    Weighted_Tfun_sum_of_residuals 		= 0.0;
//    num_of_runs		= 0;
//    number_of_parameters   = 0;
//    Tfun_sum_of_residuals = 0.0;
//    Abs_sum_of_residuals = 0.0; Weighted_Abs_sum_of_residuals = 0.0;
//    min_res =0.0; max_res = 0.0; Weighted_min_res = 0.0; Weighted_max_res = 0.0; neg_residuals = 0; pos_residuals = 0;
//    perturbator = 0.0001;
////    default_stat_param();
//}

//// Constructor
//Data_Manager::Data_Manager( int )
//{
//    define_db_specs();
//}


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
    int Np = 0, Nip = 0, Ncoef = 0, PMCndx = 0, DMCndx = 0, nIC, nDC, nPS, nPH, DCndx = -1; long int nDCinPH;
    double temp = 0.0;
    stringstream ss; string sss, ipcn, dcipcn;
    bson_init(&bson_task_file);

    DATACH* dCH = node->pCSD();

    nIC = dCH->nIC;	// nr of independent components
    nDC = dCH->nDC;	// nr of dependent components
    nPS = dCH->nPS;
    nPH = dCH->nPH;

    bson_append_int(&bson_task_file, keys::MPI[mode], omp_get_num_threads() * omp_get_num_procs());
    bson_append_string(&bson_task_file, keys::DBPath[mode], "../EJDB/<database name>");
    bson_append_string(&bson_task_file, keys::DBColl[mode], "<collection name>");

    string path = "." + gpf->GEMS3LstFilePath();
    bson_append_string(&bson_task_file, keys::G3Ksys[mode], path.c_str() );
    bson_append_string(&bson_task_file, keys::DSelect[mode], "");
    bson_append_string(&bson_task_file, keys::DTarget[mode], "");

    bson_append_start_object(&bson_task_file, keys::OptParameters[mode]);


    // Write G0 parameters
    bson_append_start_array(&bson_task_file, keys::G0[mode]);
    for (unsigned i = 0; i < nDC; i++)
    {
        ss << Np;
        sss = ss.str();
        ss.str("");
        bson_append_start_object(&bson_task_file, sss.c_str());
        {
            bson_append_string(&bson_task_file, keys::DCN_[mode], node->xCH_to_DC_name(i));
            bson_append_string(&bson_task_file, keys::PType[mode], "S");
            bson_append_double(&bson_task_file, keys::IV[mode], node->DC_G0(i, 100000, 25 + 273.15, false));
        }
        bson_append_finish_object(&bson_task_file);
        Np++;
    }
    bson_append_finish_array(&bson_task_file);
    Np = 0;

    // Write interaction parameters
    long int *LsMod = node->Get_LsMod();
    long int *LsMdc = node->Get_LsMdc();
    bson_append_start_array(&bson_task_file, keys::PAM[mode]);

    int x=0;
    for (unsigned i = 0; i < nPS; i++)
    {
        if ((LsMod[i+x] > 0) || (LsMdc[i+x] > 0))
        {
        Nip = 0;
        ss << Np;
        sss = ss.str();
        ss.str("");
        bson_append_start_object(&bson_task_file, sss.c_str());
        {
            bson_append_string(&bson_task_file, keys::EPH[mode], node->xCH_to_PH_name(i));
            ipcn.append(node->xCH_to_PH_name(i)); ipcn.append("[");

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
                        bson_append_string(&bson_task_file, keys::DCN_[mode], node->xCH_to_DC_name(DCndx + k));
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
            ipcn.clear();
        } // finish object
        bson_append_finish_object(&bson_task_file);
        Np++;
        }
        x +=2;
    } // finish PAM array
    bson_append_finish_array(&bson_task_file);

    // finish Parameters to Optimize
    bson_append_finish_object(&bson_task_file);


    // Nested function parameters
    bson_append_string(&bson_task_file, keys::OptNFParameters[mode], "");

    bson_append_string(&bson_task_file, keys::DatLogK[mode], "");

    bson_append_string(&bson_task_file, keys::LogK[mode], "");

    // Optimization Settings
    bson_append_int(&bson_task_file, keys::OptDW[mode], 0);

    bson_append_int(&bson_task_file, keys::OptEQ[mode], 1);

    bson_append_int(&bson_task_file, keys::OptUW[mode], 0);

    bson_append_string(&bson_task_file, keys::OptAlg[mode], "LN_BOBYQA");

    bson_append_double(&bson_task_file, keys::OptPBP[mode], -1);

    bson_append_double(&bson_task_file, keys::OptTRel[mode], 1e-5);

    bson_append_double(&bson_task_file, keys::OptTAbs[mode], 1e-5);

    bson_append_int(&bson_task_file, keys::OptMEv[mode], 10000);

    bson_append_int(&bson_task_file, keys::OptNormP[mode], 1);

    bson_append_double(&bson_task_file, keys::OptPer[mode], 0.0001);

    // Statistics Settings
    bson_append_int(&bson_task_file, keys::OptTu[mode], -1);

    bson_append_int(&bson_task_file, keys::OptTuVal[mode], 6);

    bson_append_int(&bson_task_file, keys::StatMC[mode], 0);

    bson_append_int(&bson_task_file, keys::StatMCr[mode], 100);

    bson_append_double(&bson_task_file, keys::StatPer[mode], 0.0001);

    bson_append_start_object(&bson_task_file, keys::DataSyn[mode]);
    {
        bson_append_start_array(&bson_task_file, keys::PhNames[mode]);
        {
            for (unsigned i = 0; i<nPH; i++)
            {
                make_syn_bson_object(bson_task_file, node->xCH_to_PH_name(i), i , mode);
            }
        }
        bson_append_finish_array(&bson_task_file);

        bson_append_start_array(&bson_task_file, keys::PhPropNames[mode]);
        {
            unsigned i = 0;
            make_syn_bson_object(bson_task_file, keys::pH, i, mode); i++;

            make_syn_bson_object(bson_task_file, keys::pe, i, mode); i++;

            make_syn_bson_object(bson_task_file, keys::Eh, i, mode); i++;

            make_syn_bson_object(bson_task_file, keys::IS, i, mode); i++;

            make_syn_bson_object(bson_task_file, keys::all, i, mode); i++;

            make_syn_bson_object(bson_task_file, keys::pV, i, mode); i++;

            make_syn_bson_object(bson_task_file, keys::RHO, i, mode); i++;

            make_syn_bson_object(bson_task_file, keys::sArea, i, mode); i++;

            make_syn_bson_object(bson_task_file, keys::Gex, i, mode); i++;

            make_syn_bson_object(bson_task_file, keys::mChainL, i, mode); i++;
        }
        bson_append_finish_array(&bson_task_file);

        bson_append_start_array(&bson_task_file, keys::DcNames[mode]);
        {
            for (unsigned i = 0; i<nDC; i++)
            {
                make_syn_bson_object(bson_task_file, node->xCH_to_DC_name(i), i, mode);
            }
        }
        bson_append_finish_array(&bson_task_file);

        bson_append_start_array(&bson_task_file, keys::DcPropNames[mode]);
        {
            unsigned i = 0;
            make_syn_bson_object(bson_task_file, keys::activity, i, mode); i++;

            make_syn_bson_object(bson_task_file, keys::actcoef, i, mode); i++;
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

    } bson_append_finish_object(&bson_task_file);
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



///// Mode GEMSFIT to generate input configuration file
//int generateConfig()
//{
//   string YN;
//   try
//   {
//     // call GEM_init to read GEMS3K input files
//     TNode* node  = new TNode();

//     // call GEM_init     --> read in input files
//     if( (node->GEM_init( gpf->GEMS3LstFilePath().c_str() )) == 1 )
//        {
//            cout << gpf->GEMS3LstFilePath() << endl;
//            cout<<" .. ERROR occurred while reading GEMS3K input files !!! ..."<<endl;
//            return 1;
//        }

//    bool with_comments = true;
//    bool brief_mode = false;

//    // Writing to the data
//    cout << "Start writing the input specification file template"<< endl;
//    Data_Manager *data_meas = new Data_Manager(1);

//    // Writing Data sources section
//    data_meas->out_db_specs_txt(with_comments, brief_mode);

//cout << "start writing out_gems_fit_txt()" << endl;
//    // Writing Parameters to Fit section &
//    out_gems_fit_txt( node, with_comments, brief_mode );

//cout << "start writing out_nlopt_param_txt()" << endl;
//    // Create instance of optimization class derived from base class Optimization
//    optimization opti(1);
//        // Writing Optimization Methods section
//    opti.out_nlopt_param_txt(with_comments, brief_mode);

//cout << "start writing out_stat_param_txt()" << endl;
//    statistics stat;
//        // Writing statistics section
//    stat.out_stat_param_txt(with_comments, brief_mode);

//cout << "Finished writing the input specification file template" << endl;
////    }

//    } catch(TError& err)
//      {
//        cout << "Error:" << err.title.c_str() << ":" <<  err.mess.c_str() << endl;
//        return 1;
//      }
//      catch(...)
//      {
//        return -1;
//      }
//    return 0;
//}

////extern outField DataCH_static_fields[14];
//extern outField DataCH_dynamic_fields[30];
//extern outField MULTI_dynamic_fields[70];
//extern outField DataBR_fields[f_lga+1/*58*/];

//outField Par_Other_fields[2] =
//{
//    { "logK",  0, 0, 1, "\n# logK: Look-up array for logK at T * P * nr reactions. "
//      "\n#    The list has to be finished with End>"
//      "\n#    If at least one G0 parameter is marked as \'R\' (reaction-constrained)"
//      "\n#    and the list below is left commented out, then logK values for all T,P pairs and reactions"
//      "\n#    will be calculated based on the initial values of all parameters, and this logK array"
//      "\n#    will be used throughout the fitting process. " },
//    { "DataLogK",  0, 0, 1, "\n# DataLogK:  "
//      "\n#      \n" }
//};

//typedef enum { /// Field index into outField structure
//    f_logK,
//    f_DataLogK
//} Par_Other_FIELDS;


//void out_gems_fit_txt( TNode* node, bool _comment, bool brief_mode )
//{
//    DATACH* CSD = node->pCSD();
//    DATABR* CNode = node->pCNode();

//    string fname = gpf->OptParamFile();
//    fstream ff(fname.c_str(), ios::out|ios::app );
//    ErrorIf( !ff.good() , fname.c_str(), "OptParamFile text open error");

//    if(_comment )
//    {
//        ff << "\n#  GEM input parameters to fit can be marked by preceding them with the F, L, and R letter "
//              "\n#    in <TKval>, <Pval>, <G0>,  <PMc>, <DMc>, <fDQF>, <bIC> ... data object entries, see below."
//              "\n#    Values without markup will not be modified by GEMSFIT2 routines, just used in GEM runs.\n"
//              "\n#  Mark with F the parameter value you want to fit independently of other parameters. "
//              "\n#    Shorthand option: F<initval> - initial value with default (10%) upper- and lower boundaries."
//              "\n#    Example:  ... F-833444 ... (<initval> can be modified, if necessary)."
//              "\n#    Full JSON-style markup syntax for the independently adjusted parameter: as in this example"
//              "\n#      for fitting G0(298) of SiO2@ "
//              "\n#      F{ \"IV\" :-833444, \"UB\" : -800000, \"LB\" : -900000 } "
//              "\n#      where:"
//              "\n#         \"IV\": initial value; "
//              "\n#         \"UB\": upper boundary; "
//              "\n#         \"LB\": lower bundary (in this case, all in J/mol). \n"
//              "\n#  Mark with R the G0(298) value of a Dependent Component, which depends on G0 of other DCs"
//              "\n#     via a reaction constraint, by copy-pasting the following template in place of G0(298) value,"
//              "\n#     and editing it as desired:"
//              "\n#      R{ \"IV\" : -36819, \"Ref\" : \"SUPCRT92\", \"order\" : \"1\", \"nC\" : 4, \"rDC\" : \"KOH@\","
//              "\n#      \"RC\" : [ \"K+\", \"H2O@\", \"H+\", \"KOH@\" ], \"Rcoef\" : [ -1, -1, 1, 1 ] }"
//              "\n#     Here,  \"IV\": initial value; "
//              "\n#         \"Ref\": bibliographic reference (optional);"
//              "\n#         \"order\": reaction order | 1, 2, ... |, important for many reaction with common species;"
//              "\n#         \"nC\": numer of components (species) involved in the reaction;"
//              "\n#         \"DC\": name of Dependent Component whose properties are constrained with this reaction; "
//              "\n#         \"RC\": list [ ] of names of all components (species) involved in reaction (comma-separated);"
//              "\n#         \"Rcoef\": array [ ] of reaction stoichiometry coeficients (comma-separated), "
//              "\n#           in the same order as in the \"RC\" list. This example describes a reaction: "
//              "\n#           K+ + H2O@ = H+ + KOH@ in which G0(KOH@) follows variations of G0(K+) induced by the fitting, "
//              "\n#           such that the logK of this reaction at (T,P) remains constant. \n"
//              "\n#  Mark with L the bIC (system bulk composition) value of Independent Component, "
//              "\n#     which depends on bIC of other elements via a titration constraint,"
//              "\n#     by copy-pasting the following template in place of the bIC value, and editing as desired:"
//              "\n#      L{ \"LE\" :\"H\", \"IV\" :113.016746705914, \"LEs\" :[\"S\", \"Cl\"], \"Lcoef\" :[2,1]}"
//              "\n#     In this example: the amount of H is linked to S and Cl by titration with H2SO4 and HCl, "
//              "\n#         with stoichiometry coeficients of 2 and 1, respectively."
//              "\n#        \"LE\": linked element "
//              "\n#        \"IV\": initial value "
//              "\n#        \"LEs\": the elements linked to "
//              "\n#        \"Lcoef\": linkage coefficients (stoichiometry coefficients) "
//              "\n#         Whenever the bIC values of S or Cl are varied as (independent) titration parameters,"
//              "\n#         the bIC value of H will be adjusted to titrate the system either with H2SO4 or with HCl."
//              "\n#  Restrictions: You can't fit G0's together with TK and P!"
//           << endl;
//    }

//    TPrintArrays  prarCH(30, DataCH_dynamic_fields, ff);

//    if( _comment )
//        ff << "\n# ICNL: List of Independent Component names (for readability; no need to modify)";
//    prarCH.writeArrayS(  "ICNL", CSD->ICNL[0], CSD->nIC, MaxICN );
//    if( _comment )
//            ff << "\n# DCNL: List of Dependent Component names (for readability; no need to modify)";
//    prarCH.writeArrayS(  "DCNL", CSD->DCNL[0], CSD->nDC, MaxDCN );
//    if( _comment )
//            ff << "\n# PHNL: List of Phase names (for readability; no need to modify)";
//    prarCH.writeArrayS(  "PHNL", CSD->PHNL[0], CSD->nPH, MaxPHN );

//    vector <double> xG0;
//    ff << "\n \n# G0: Look-up array for DC Gibbs energy function g(T,P), J/mol at 298.15K and 1 bar \n";
//    ff << "<G0>" << endl;
//    for (int i=0; i<CSD->nDC*node->gridTP(); ++i)
//    {
//        xG0.push_back(CSD->G0[i]);
//        ff << CSD->G0[i] << endl;
//        i += node->gridTP()-1;
//    }

//    TPrintArrays  par_other(2, Par_Other_fields, ff);

//    string DataLogK = "", logK = "";

//    par_other.writeField(f_DataLogK, DataLogK, _comment, brief_mode );
//    par_other.writeField(f_logK, logK, _comment, brief_mode );

//    if(_comment )
//    {
//        ff << "\n\n#########################################################################" << endl;
//        ff << "#>>>>>>>>>>>>>>> Input for fitting GEM input parameters >>>>>>>>>>>>>>>>#" << endl;
//        ff << "#########################################################################" << endl;
//    }
//    TPrintArrays  prarIPM( 70, MULTI_dynamic_fields, ff);
//    MULTI* pmp = node->pMulti()->GetPM();

//    if( pmp->FIs > 0 && pmp->Ls > 0 )
//    {
//      if( _comment )
//         ff << "\n# Initial data for multicomponent phases (fragment of GEMS3K *IMP.dat input file)"
//               "\n#    (see the DCH file for the dimension nPHs)";
//      prarIPM.writeArrayF(  f_sMod, pmp->sMod[0], pmp->FIs, 8L, _comment, brief_mode );

//    long int LsModSum;
//    long int LsIPxSum;
//    long int LsMdcSum;
//    long int LsMsnSum;
//    long int LsSitSum;
//    node->pMulti()->getLsModsum( LsModSum, LsIPxSum );
//    node->pMulti()->getLsMdcsum( LsMdcSum, LsMsnSum, LsSitSum );

//     prarIPM.writeArray(  f_LsMod, pmp->LsMod, pmp->FIs*3, 3L, _comment, brief_mode);

//      if(LsIPxSum )
//      {
//       if( _comment )
//          ff << "\n\n# IPxPH: Index lists (in TSolMod convention) for interaction parameters of non-ideal solutions";
//       prarIPM.writeArray(  "IPxPH", pmp->IPx,  LsIPxSum);
//      }
//      if(LsModSum )
//       {
//         if( _comment )
//            ff << "\n\n# PMc: Tables (in TSolMod convention) of interaction parameter coefficients for non-ideal solutions";
//        prarIPM.writeArray(  "PMc", pmp->PMc,  LsModSum);
//       }

//       prarIPM.writeArray(  f_LsMdc, pmp->LsMdc, pmp->FIs*3, 3L, _comment, brief_mode);
//       if(LsMdcSum )
//       {   if( _comment )
//              ff << "\n\n# DMc: Tables (in TSolMod convention) of  parameter coefficients for Dependent Components";
//        prarIPM.writeArray(  "DMc", pmp->DMc,  LsMdcSum);
//       }
//       if(LsMsnSum )
//       {   if( _comment )
//              ff << "\n\n# MoiSN:  End member moiety / site multiplicity number tables (in TSolMod convention) ";
//        prarIPM.writeArray(  "MoiSN", pmp->MoiSN,  LsMsnSum);
//       }
//    } // sMod
//    prarIPM.writeArray(  f_fDQF, pmp->fDQF,  pmp->L, -1L, _comment, brief_mode);


//    if( _comment )
//       ff << "\n\n# This part for the system composition data is taken from the *DBR.dat file\n";

//    // from *DBR.dat
//    TPrintArrays  prar(f_bSP+1/*52*/, DataBR_fields, ff);

//    ff<< "\n# For fitting T and P parameters in thermobarometry application, give the upper and lower values"
//         "\n#   corresponding to the interpolation range that you selected when exporting GEMS3K input files ";

//    prar.writeField(f_TK, CNode->TK, _comment, brief_mode  );
//    prar.writeField(f_P, CNode->P, _comment, brief_mode  );
//    if( _comment )
//     {   ff << "\n\n## (4) Data for Independent Components";
//         prar.writeArrayS(  NULL, CSD->ICNL[0], CSD->nIC, MaxICN );
//     }
//    prar.writeArray(  f_bIC,  CNode->bIC, CSD->nICb, -1L,_comment, brief_mode );

//    ff.close();
//}



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
                mkdir(path_init.c_str(), 0775);
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
                mkdir(path_init.c_str(), 0775);
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

//-------------------------------------------------------------------------------------------------

//outField Data_Manager_fields[8] =
//{
//    { "MPI", 0, 0, 1, "\n# MPI: Number of threads for parallelization\n" },
//    { "DataDB",  0, 0, 1, "\n# DataDB: EJDB database path (please, edit to put the actual path)\n" },
//    { "DataCollection",  0, 0, 1, "\n# DataCollection: database collection name (please, edit to put the actual name)\n" },
//    { "DataSource",  1, 0, 1, "\n# DataSource: get experimental data from the EJDB format database"
//      "\n#     (default:0, no other sources yet implemented in GEMSFIT2)\n" },

//    { "DataSelect", 0, 0, 1, "# DataSelect: query for obtaining the experimental data from the database."
//      "\n# Options: "
//      "\n#      leave empty ot { } to select all data in the database"
//      "\n#    \'{ ... }\': script in JSON format (in braces) describing what experimental data to select. "
//      "\n#      \"usedatasets\": [...]: list of comma-separated names of experimental datasets to be used, "
//      "\n#         or empty string \"\" to select all available datasets."
//      "\n#      \"skipdatasets\": [...]: list of comma-separated names of experimental datasets to be skipped, "
//      "\n#         or empty string \"\" to not skip (in this case, \"usedatasets\" will apply)."
//      "\n#      \"usesamples\": [...]: list of comma-separated names of samples, or empty string \"\" "
//      "\n#         to select all available samples. Note that samples with the same name can be present "
//      "\n#         in two or more different experimental datasets."
//      "\n#      \"skipsamples\": [...]: list of comma-separated names of samples to be skipped, "
//      "\n#         or empty string \"\" to not to skip (in this case, \"usesamples\" will apply)."
//      "\n#      \"usepairs\": [...]: list of pairs each consisting of an experimental dataset name and a list"
//      "\n#         of comma-separated sample names to be taken from this dataset only; or an empty string \"\" "
//      "\n#         for other options to apply."
//      "\n#      \"skippairs\": [...]: list of pairs each consisting of an experimental dataset name and a list"
//      "\n#         of comma-separated sample names to be skipped from this dataset only; or an empty string \"\" "
//      "\n#         for other options to apply."
//      "\n#      \"sT\": [...]: list of comma-separated minimum and maximum temperature, C, "
//      "\n#         or empty string \"\" to select data for all available temperatures. "
//      "\n#      \"sP\": [...]: list of comma-separated minimum and maximum pressures, bar, "
//      "\n#         or empty string \"\" to select data for all available pressures.  "
//      "\n#      Example: "
//      "\n#      \'{ \"usedatasets\" : [\"CH04\", \"CH04D\"],"
//      "\n#          \"skipdatasets\" : [\"CH03\", \"CH03D\"],"
//      "\n#          \"usesamples\" : \"\", "
//      "\n#          \"skipsamples\" : \"\", "
//      "\n#          \"usepairs\" : \"\", "
//      "\n#          \"skippairs\" : [ "
//      "\n#                            { \"dataset\": \"CHO4\", \"samples\": [\"01\", \"02\"]}, "
//      "\n#                            { \"dataset\": \"CHO4D\", \"samples\": [\"01\", \"02\"]}, "
//      "\n#                          ], "
//      "\n#          \"sT\" : [100, 1000],"
//      "\n#          \"sP\" : [1, 2500] }\'"
//      "\n#      Copy the example and paste it below, remove comment symbols (#), and edit as necessary."
//      "\n#      Any main query option that has no value (empty string \"\") can be left out. In this example,"
//      "\n#        the lines for \"usesamples\", \"skipsamples\", and \"usepairs\" can be left out. \n"
//    },

//    { "DataTarget",  0, 0, 1, "# DataTarget: Target function for parameter fitting, described in JSON style"
//      "\n#     as text in single quotes and braces: \'{ ... }\' "
//      "\n#  \"Target\":  name of the target (objective) function (optional)"
//      "\n#  \"TT\":  type of the target function as sum of the following terms: "
//      "\n#     \"lsq\":       w*(measured-simulated)^2;"
//      "\n#     \"lsq-norm\":  w*(measured/avgm-simulated/avgm)^2;"
//      "\n#      ... "
//      "\n#      here, avgm is the arithmetic mean of measured values."
//      "\n#  \"WT\":  weighting scheme for samples, one of the following options:"
//      "\n#       empty string \"\": w = 1 (equal weights);"
//      "\n#      \"inverr\": w=1/error; \"inverr2\": w=1/error^2; \"inverr3\": w=1/measured^2;"
//      "\n#      \"inverr_norm\": w=1/(error/avgm)^2; "
//      "\n#       ... "
//      "\n#  \"OFUN\": objective function, a list [] of terms {} for measured properties to compare"
//      "\n#     with their computed counterparts. Each term can contain property-value pairs:"
//      "\n#      \"EPH\": for what phase from the experiments"
//      "\n#      \"CT\":  for the type of data to compare:"
//      "\n#          \"IC\" for independent component; \"DC\" for dependent component; "
//      "\n#          \"prop\" for phase property; \"MR\" for mole ratio; "
//      "\n#      \"CN\":  the name/formula of the data to compare. e.g. \"Al\" if \"CT\" is \"IC\","
//      "\n#          or \"pH\" if \"CT\" is \"prop\", or \"K/(Na+K)\" if \"CT\" is \"MR\" "
//      "\n#      \"DCP\": used only if \"CT\" is \"DC\", to represent the name of dependent component property:"
//      "\n#          \"Q\" for amount in mole fraction; \"@coef\" for activity coeficient"
//      "\n#      \"WT\": weight assigned to the objective function as real number e.g. \"WT\": 100. "
//      "\n#      \"unit\":  units of measurement (override those given in the database for this value):"
//      "\n#          \"molal\":  mol/(kg H2O), \"log_molal\": log(molal), \"-loga\": negated log(activity) for pH;"
//      "\n#          \"g\"; \"kg\"; \"cm3\"; \"m3\"; \"molfrac\": mole fraction; J/mol for Gex "
//      "\n#           ... (conversions will be performed automatically).\n"
//      "\n#  \"NFUN\": nested objective function, a list [] of terms {} for measured properties to compare"
//      "\n#    with their computed counterparts for each experiment independently of \"OFUN\" for adjusting"
//      "\n#    a input property \"Ptype\" to obtain an output value given by \"EPH\", \"CT\" and \"CN\" pairs."
//      "\n#       \"Ptype\": can have a value \"bIC\" or \"T-P\". The former refers to ajusting some parts of"
//      "\n#         the bulk composition of the system representing the sample. The latter refers to adjusting"
//      "\n#         the temperature, the pressure, or both."
//      "\n#    IMPORTANT: When \"NFUN\" is specified in DataTarget section, at least one element in \"Ptype\" "
//      "\n#         property object must be marked for fitting. More than one NFUN entries with different "
//      "\n#         \"Ptype\" input properties can be used with caution.\n "
//      "\n#  \"ADDOUT\": additional output, a list [] of terms {} for additional output in the results file"
//      "\n#    The options are the same as for OFUN with an additional \"SRC\" key whose value can be"
//      "\n#    \"calc\" if the aditional output is not present in the database but can be retreived trough GEMS claculation "
//      "\n#    \"meas\" if the additional output is present in the database but was not used in the OFUN "
//      "\n#     The comparison options are: "
//      "\n#       aqueous phase (\"aq_gen\") elemental composition in \"molal\" or \"loga\" "
//      "\n#       aqueous phase (\"aq_gen\") properties (\"prop\"): \"pH\" in \"-loga\" (or \"molal\" "
//      "\n#          - molality concentration of H+); \"Q\" - mass in \"g\" or \"kg\"  "
//      "\n#       other phases composition as element bulk amount in moles (\"mol\") "
//      "\n#          or to /Si molar ratio (\"Simolfrac\")"
//      "\n#       other phases properties (\"prop\"): \"Q\" - mass in \"g\" or \"kg\"; "
//      "\n#          \"pV\" - volume in \"cm3\" or \"m3\";  \"Eh\" - volume in \"Volts\"; "
//      "\n#          \"Gex\"  - excess Gibbs energy of mixing (in \"J/mol\"). "
//      "\n#       dependent components (\"DC\") properties: \"Q\" - amount in \"mol\"; "
//      "\n#          \"@coef\" - activity coefficient < add activity and mole fraction >"
//      "\n#           ..."
//      "\n#  Example of target (objective) function definition:"
//      "\n#  \'{ \"Target\": \"name\", \"TT\": \"lsq\", \"WT\": \"inverr\", \"OFUN\":"
//      "\n#      ["
//      "\n#         { \"EPH\": \"aq_gen\", \"CT\": \"IC\", \"CN\": \"Si\", \"unit\": \"molal\" },"
//      "\n#         { \"EPH\": \"aq_gen\", \"CT\": \"IC\", \"CN\": \"Al\", \"unit\": \"molal\" },"
//      "\n#         { \"EPH\": \"aq_gen\", \"CT\": \"prop\", \"CN\": \"pH\", \"unit\": \"-loga\" },"
//      "\n#         { \"EPH\": \"aq_gen\", \"CT\": \"DC\", \"CN\": \"SiO2@\", \"DCP\" : \"Q\", \"unit\": \"mol\"}"
//      "\n#      ]"
//      "\n#   }\'"
//      "\n# "
//      "\n#  Example of a target function with a nested objective function to titrate for a given pH:"
//      "\n#  \'{ \"Target\": \"name\", \"TT\": \"lsq\", \"WT\": \"inverr\", \"OFUN\":"
//      "\n#      ["
//      "\n#         { \"EPH\": \"aq_gen\", \"CT\": \"IC\", \"CN\": \"Al\", \"unit\": \"log_molal\" },"
//      "\n#      ], \"NFUN\": "
//      "\n#      ["
//      "\n#         { \"Ptype\": \"bIC\", \"EPH\": \"aq_gen\", \"CT\": \"prop\", \"CN\": \"pH\", \"unit\": \"-loga\" } "
//      "\n#      ]"
//      "\n#   }\'"
//      "\n#  Copy one of the examples, paste it below, remove the comment symbols (#), and edit as necessary. "
//      "\n#     Remove unnecessary rows. Note that in the example for nested function, at least one element in"
//      "\n#     the \"bIC\" vector should be marked as freely adjustable parameter 'F'. \n"
//    },
//    { "SystemFiles",  0, 0, 1, "\n# SystemFiles: path to the list of GEMS3K input files (also used for this template file)\n" },
//    { "LimitOfDetection",  0, 0, 1, "\n# LimitOfDetection: Limit of detection of the measured values."
//      "\n#     Ensures that wrongly computed output values due to non-physical input parameter values are ignored.\n" }
//};

//typedef enum { /// Field index into outField structure
//    f_MPI = 0,
//    f_DataDB,
//    f_DataCollection,
//    f_DataSource,
//    f_DataSelect,
//    f_DataTarget,
//    f_SystemFiles,
//    f_LimitOfDetection,
//} Data_Manager_FIELDS;

///// Set up default values for structure
//void Data_Manager::define_db_specs( )
//{
//   MPI = omp_get_num_threads() * omp_get_num_procs();
//   datasource = 0;
//   DBname = "../EJDB/<database name>";
//   collection= "experiments";
//   DataSelect ="";
//   DataTarget = "";
//   LimitOfDetection = 1e-06;
//   DataLogK = "";

//}

///// Writes structure to the GEMSFIT2 input specification file
//void Data_Manager::out_db_specs_txt( bool with_comments, bool brief_mode )
//{
//    string fname = gpf->OptParamFile();
//    fstream ff(fname.c_str(), ios::out/*|ios::app*/ );
//    ErrorIf( !ff.good() , fname.c_str(), "OptParamFile text open error");

//    TPrintArrays  prar(8, Data_Manager_fields, ff);
//    // define data that must be written

//    if (datasource == 0)
//    {
//        prar.setAlws( f_MPI );
//        prar.setAlws( f_DataDB );
//        prar.setAlws( f_DataCollection );
//        prar.setAlws( f_DataSelect );
//        prar.setAlws( f_LimitOfDetection );
//    }

//    if(with_comments )
//    {
//        ff << "#########################################################################" << endl;
//        ff << "#>>>>>>>>>>> GEMSFIT2 input specification file template >>>>>>>>>>>>>>>>#" << endl;
//        ff << "#########################################################################" << endl;
//        ff << "#  Character '#' means a comment line ";
//    }

//    prar.writeField(f_MPI, (long int)MPI, with_comments, brief_mode  );

//    if(with_comments )
//    {
//        ff << "\n\n#########################################################################" << endl;
//        ff << "#>>>>>>>>>>>>>>>>>>>>>>>> Data Sources section >>>>>>>>>>>>>>>>>>>>>>>>>#" << endl;
//        ff << "#########################################################################";
//    }
//    prar.writeField(f_DataSource, (long int)datasource, with_comments, brief_mode  );
//    prar.writeField(f_DataDB, DBname, with_comments, brief_mode  );
//    prar.writeField(f_DataCollection, collection, with_comments, brief_mode  );
//    prar.writeField(f_SystemFiles, "."+gpf->GEMS3LstFilePath(), with_comments, brief_mode  );
////    prar.writeField(f_RecipeFiles, "", with_comments, brief_mode  );

//    if(with_comments )
//    {
//        ff << "\n\n#########################################################################" << endl;
//        ff << "#>>>>>>>>>>>>>>>>>>>>>>> DataSelect section >>>>>>>>>>>>>>>>>>>>>>>>>>>>#" << endl;
//        ff << "#########################################################################" << endl;
//    }
//    prar.writeField(f_DataSelect, DataSelect, with_comments, brief_mode );

//    if(with_comments )
//    {
//        ff << "\n\n#########################################################################" << endl;
//        ff << "#>>>>>>>>>>>>>>>>>>>>>>> DataTarget section >>>>>>>>>>>>>>>>>>>>>>>>>>>>#" << endl;
//        ff << "#########################################################################" << endl;
//    }
//    prar.writeField(f_DataTarget, DataTarget, with_comments, brief_mode );
//    prar.writeField(f_LimitOfDetection, (double)LimitOfDetection, with_comments, brief_mode  );

//    if(with_comments )
//    {
//        ff << "\n\n#########################################################################" << endl;
//        ff << "#>>>>>>>>>>>>>>>>>>>>>> ParameterMarkup section >>>>>>>>>>>>>>>>>>>>>>>>#" << endl;
//        ff << "#########################################################################" << endl;
//    }
//    ff.close();
//}

//// get database connection parameters
//void Data_Manager::get_db_specs_txt( )
//{
//    // open file for reading
//    string inputstr, result;
//    string fname = gpf->OptParamFile();
//    fstream ff(fname.c_str(), ios::in );
//    ErrorIf( !ff.good() , fname, "OptParamFile Fileopen error");

//    TReadArrays  rdar(8, Data_Manager_fields, ff);

//    long int nfild = rdar.findNextNotAll();
//    while( nfild >=0 )
//        {
//          switch( nfild )
//          {
//            case f_MPI: rdar.readArray( "MPI",  &MPI, 1 );
//                    break;
//            case f_DataDB: rdar.readArray( "DataDB",  DBname );
//                    break;
//            case f_DataCollection: rdar.readArray( "DataCollection",  collection );
//                    break;
//            case f_DataSource: rdar.readArray( "DataSource",  &datasource, 1);
//                    break;
//            case f_LimitOfDetection: rdar.readArray( "LimitOfDetection",  &LimitOfDetection, 1);
//                    break;
//            case f_DataSelect: rdar.readArray( "DataSelect",  DataSelect);
//                    break;
//            case f_DataTarget: rdar.readArray( "DataTarget",  DataTarget );
//                    break;
//            case f_SystemFiles: rdar.readArray( "SystemFiles",  inputstr );
//                    remove_copy(inputstr.begin(), inputstr.end(), std::back_inserter(result), '\'');
//                    inputstr = result;
//                    result.clear();
//                    gpf->setGEMS3LstFilePath(inputstr.c_str() );
//                    break;
//          }
//          nfild = rdar.findNextNotAll();
//        }

//    // define data that must be read
//    if (datasource == 0)
//    {
//        rdar.setAlws( f_MPI );
//        rdar.setAlws( f_DataDB );
//        rdar.setAlws( f_DataCollection );
//        rdar.setAlws( f_DataSelect );
//        rdar.setAlws( f_LimitOfDetection );
//    }

//    // testing read
//        string ret = rdar.testRead();
//        if( !ret.empty() )
//         { ret += " - fields must be read from OptParamFile structure";
//           Error( "Error", ret);
//         }

//    // removing inverted comma
//        remove_copy(DBname.begin(), DBname.end(), std::back_inserter(result), '\'');
//        DBname = result;
//        result.clear();

//        remove_copy(collection.begin(), collection.end(), std::back_inserter(result), '\'');
//        collection = result;

//    // reading other implemented fields
//    fstream fff(fname.c_str(), ios::in );
//    ErrorIf( !fff.good() , fname, "OptParamFile Fileopen error");
//    TReadArrays  par_other (2, Par_Other_fields, fff);
//    nfild = par_other.findNextNotAll();
//    while( nfild >=0 )
//        {
//          switch( nfild )
//          {
//            case f_DataLogK: par_other.readArray( "DataLogK",  DataLogK);
//                    break;
//          }
//          nfild = rdar.findNextNotAll();
//        }
//}

////-----------------------------------------------------------------------------------------

//outField statistics_fields[3] =
//{
//    { "StatMCbool",  0, 0, 1, "\n# StatMCbool: perform Monte Carlo runs -> no (0) "
//                              "\n#             yes (1) scatter added to (ideal) computed values | yes (2) scatter added to measured values "},
//    { "StatMCruns",  0, 0, 1, "\n# StatMCruns: number of Monte Carlo runs for confidence interval generation"},
//    { "StatPerturbator", 0, 0, 1, "\n# StatPerturbator: used for calculating sensitivities by central diference, see ref [2]"}
//};

//typedef enum {  /// Field index into outField structure
//    f_StatMCbool = 0,
//    f_StatMCruns,
//    f_StatPerturbator
//} statistics_FIELDS;


/////// Set up default values for structure
//void statistics::default_stat_param()
//{
//  MCbool =  0;
//  num_of_MC_runs = 100;
//  perturbator = 0.0001;
//}

/////// Writes  structure to  the GEMSFIT configuration file
//void statistics::out_stat_param_txt( bool with_comments, bool brief_mode )
//{
//    string fname = gpf->OptParamFile();
//    fstream ff(fname.c_str(), ios::out|ios::app);
//    ErrorIf( !ff.good() , fname.c_str(), "OptParamFile text open error");

//    TPrintArrays  prar(3, statistics_fields, ff);
//    prar.writeField(f_StatMCbool, (long int)MCbool, with_comments, brief_mode  );
//    prar.writeField(f_StatMCruns, (long int)num_of_MC_runs, with_comments, brief_mode  );
//    prar.writeField(f_StatPerturbator, (double)perturbator, with_comments, brief_mode  );

//    if(with_comments )
//    {
//        ff << endl << endl;
//        ff << "#########################################################################" << endl;
//        ff << "#>>>>>>>>>> end of GEMSFIT2 input specification file template >>>>>>>>>>#" << endl;
//        ff << "#########################################################################" << endl;
//    }

//    ff.close();
//}

////// Read statistical input specifications from configurator
//void statistics::get_stat_param_txt( )
//{
//    // open file for reading
//    string fname = gpf->OptParamFile();
//    fstream ff(fname.c_str(), ios::in );
//    ErrorIf( !ff.good() , fname, "OptParamFile Fileopen error");

//    TReadArrays  rdar(3, statistics_fields, ff);

//    long int nfild = rdar.findNextNotAll();
//    while( nfild >=0 )
//        {
//          switch( nfild )
//          {
//          case f_StatMCbool:
//          { /*int bb;*/
//              rdar.readArray( "StatMCbool",  &MCbool, 1 );
////              MCbool = (bool)bb;
//          }
//              break;
//          case f_StatMCruns: rdar.readArray( "StatMCruns",  &num_of_MC_runs, 1 );
//              break;
//          case f_StatPerturbator: rdar.readArray( "StatPerturbator",  &perturbator, 1 );
//              break;
//          }
//          nfild = rdar.findNextNotAll();
//        }

//    // testing read
//        string ret = rdar.testRead();
//        if( !ret.empty() )
//         { ret += " - fields must be read from OptParamFile structure";
//           Error( "Error", ret);
//         }
//}


//////-------------------------------------------------------------------------------------------------

//outField optimization_fields[15] =
//{   { "OptParameters",  0, 0, 1, "\n# OptParameters:    "},
//    { "OptNfunParameters",  0, 0, 1, "\n# OptNfunParameters:    "},
//    { "OptDoWhat",  0, 0, 1, "\n# OptDoWhat: perform optimization and statistics (0); only optimization with basic Statistics (1);"
//       "\n#            only Statistics (2) with initial guesses as best fit parametters"},
//    { "OptEquilibrium",  0, 0, 1, "\n# OptEquilibrium: (1) Use full GEMS3K to calculate thermodynamic equilibrium. (0) Use TSolMod shortcut "
//                                   "\n#                  without calculating equilibrium (only fitting activity model parameters)"},
//    { "OptUserWeight",  0, 0, 1, "\n# OptUserWeight: (1) Use the weights provided in the \"weight\" column of the database. "},
//    { "OptTuckey",  0, 0, 1, "\n# OptTuckey: (1) Use Tuckey Biweight for all data. (2) Use Tuckey Biweight for each OFUN independently. "},
//    { "OptTuckeyVal",  0, 0, 1, "\n# OptTuckeyVal: Empirical chosen value that is multiplied with the median of residuals to get the "
//      "\n# 		      weighting thereshold value C = Val * M. Default value 6. Residuals >C -> weight 0. "},
//    { "OptAlgo",  0, 0, 1, "\n# OptAlgo: specify algorithm: GN_ISRES | GN_ORIG_DIRECT | GN_ORIG_DIRECT_L | LN_COBYLA | LN_BOBYQA "},
//    { "OptBoundPerc",  0, 0, 1, "\n# OptBoundPerc: Generate bounds from initial guess vector: specify percentage deviation "
//                                "\n#               (user-specific, user-defined bounds when set to -1)"},
//    { "OptTolRel",  0, 0, 1, "\n# OptTolRel: stopping criterion -> specify relative tolerance (default = 1e-06) of function value"},
//    { "OptTolAbs",  0, 0, 1, "\n# OptTolAbs: stopping criterion -> specify absolute tolerance (default = 1e-06) of function value"},
//    { "OptMaxEval",  0, 0, 1, "\n# OptMaxEval: specify max number of evaluations"},
//    { "OptInitStep",  0, 0, 1, "\n# OptInitStep: specify initial stepsize for local minimizers "
//                               "\n#              (factor will be multiplied to all optimization parameters); 0 => use default"},
//    { "OptNormParam",  0, 0, 1, "\n# OptNormParam: Normalize bounds/constraints/fitting parameters with the initial guess vector"},
//    { "OptPerturbator",  0, 0, 1, "\n# OptPerturbator: The delta/difference used to to calculate the d(function_value)/d(parameter_value) gradient"}
//};

//typedef enum {  /// Field index into outField structure
//    f_OptParameters = 0,
//    f_OptNfunParameters,
//    f_OptDoWhat,
//    f_OptEquilibrium,
//    f_OptUserWeight,
//    f_OptTuckey,
//    f_OptTuckeyVal,
//    f_OptAlgo,
//    f_OptBoundPerc,
//    f_OptTolRel,
//    f_OptTolAbs,
//    f_OptMaxEval,
//    f_OptInitStep,
//    f_OptNormParam,
//    f_OptPerturbator,
//} optimization_FIELDS;


/////// Set up default values for structure
//void optimization::define_nlopt_param( )
//{
//    OptAlgo = "LN_BOBYQA";
//    OptBoundPerc = -1.0;
//    OptTolRel = 1e-6;
//    OptTolAbs = 1e-6;
//    OptMaxEval = 10000;
//    OptDoWhat = 0;
//    OptEquilibrium = 1;
//    OptTuckey = 0;
//    OptTuckeyVal = 6;
//    OptUserWeight = 0;
//    OptTitration = 0;
//    OptNormParam = 1;
//    OptPerturbator = 0.0001;
//    OptInitStep = 0;
//    OptParameters ="";
//    NFunParameters ="";

//}

/////// Writes  structure to  the GEMSFIT configuration file
//void optimization::out_nlopt_param_txt( bool with_comments, bool brief_mode )
//{
//    string fname = gpf->OptParamFile();
//    fstream ff(fname.c_str(), ios::out|ios::app );
//    ErrorIf( !ff.good() , fname.c_str(), "OptParamFile text open error");
//    TPrintArrays  prar(15, optimization_fields, ff);
//    if(with_comments )
//    {
//        ff << "\n\n#########################################################################" << endl;
//        ff << "#>>>>>>>>>>>>>>>> OptimizationMethods section >>>>>>>>>>>>>>>>>>>>>>>>>>#" << endl;
//        ff << "#########################################################################" << endl;
//    }

//    prar.writeField( f_OptParameters,  OptParameters, with_comments, brief_mode);
//    prar.writeField( f_OptNfunParameters,  NFunParameters, with_comments, brief_mode);
//    prar.writeField( f_OptDoWhat,  (long int)OptDoWhat, with_comments, brief_mode);
//    prar.writeField( f_OptEquilibrium,  (long int)OptEquilibrium, with_comments, brief_mode);
//    prar.writeField( f_OptUserWeight,  (long int)OptUserWeight, with_comments, brief_mode);
//    prar.writeField( f_OptTuckey,  (long int)OptTuckey, with_comments, brief_mode);
//    prar.writeField( f_OptTuckeyVal,  (long int)OptTuckeyVal, with_comments, brief_mode);
//    prar.writeField( f_OptAlgo,  OptAlgo, with_comments, brief_mode );
//    prar.writeField( f_OptBoundPerc,  OptBoundPerc, with_comments, brief_mode );
//    prar.writeField( f_OptTolRel,  OptTolRel, with_comments, brief_mode);
//    prar.writeField( f_OptTolAbs,  OptTolAbs, with_comments, brief_mode);
//    prar.writeField( f_OptMaxEval,  (long int)OptMaxEval, with_comments, brief_mode);
//    prar.writeField( f_OptInitStep,  OptInitStep, with_comments, brief_mode);
//    prar.writeField( f_OptNormParam,  (long int)OptNormParam, with_comments, brief_mode);
//    prar.writeField( f_OptPerturbator,  OptPerturbator, with_comments, brief_mode);

//    ff.close();
//}

////// populate nlopt instance: set bounds, constraints, stopping criteria
//void optimization::get_nlopt_param_txt( )
//{
//    // open file for reading
////    int i;
//    string fname = gpf->OptParamFile();
//    fstream ff(fname.c_str(), ios::in );
//    ErrorIf( !ff.good() , fname, "OptParamFile Fileopen error");

//    TReadArrays  rdar(15, optimization_fields, ff);

//    long int nfild = rdar.findNextNotAll();
//    while( nfild >=0 )
//        {
//          switch( nfild )
//          {
//          case f_OptAlgo: rdar.readArray( "OptAlgo",  OptAlgo );
//                   break;
//          case f_OptBoundPerc: rdar.readArray( "OptBoundPerc",  &OptBoundPerc, 1 );
//                  if( OptBoundPerc < 0.) OptBoundPerc = -1;
//                 break;
//          case f_OptTolRel: rdar.readArray( "OptTolRel",  &OptTolRel, 1);
//                   break;
//          case f_OptTolAbs: rdar.readArray( "OptTolAbs",  &OptTolAbs, 1);
//                  break;
//          case f_OptMaxEval: rdar.readArray( "OptMaxEval",  &OptMaxEval, 1);
//                  break;
//          case f_OptDoWhat: rdar.readArray( "OptDoWhat",  &OptDoWhat, 1);
//                  break;
//          case f_OptEquilibrium: rdar.readArray( "OptEquilibrium",  &OptEquilibrium, 1);
//                  break;
//          case f_OptUserWeight: rdar.readArray( "OptUserWeight",  &OptUserWeight, 1);
//                  break;
//          case f_OptTuckey: rdar.readArray( "OptTuckey",  &OptTuckey, 1);
//                  break;
//          case f_OptTuckeyVal: rdar.readArray( "OptTuckeyVal",  &OptTuckeyVal, 1);
//                  break;
//          case f_OptInitStep: rdar.readArray( "OptInitStep",  &OptInitStep, 1);
//                  break;
//          case f_OptPerturbator: rdar.readArray( "OptPerturbator",  &OptPerturbator, 1);
//                  break;
//          case f_OptParameters: rdar.readArray( "OptParameters",  OptParameters);
//                  break;
//          case f_OptNfunParameters: rdar.readArray( "NFunParameters",  NFunParameters);
//                  break;
//          case f_OptNormParam:{
//                int bb;
//                rdar.readArray( "OptNormParam",  &bb, 1);
//                if (bb<0)
//                OptNormParam = false;
//                else
//                    OptNormParam = true;
//                 }
//                  break;
//          }
//          nfild = rdar.findNextNotAll();
//        }

//    GEMSsys = gpf->GEMS3LstFilePath();

//        // testing read
//        string ret = rdar.testRead();
//        if( !ret.empty() )
//         { ret += " - fields must be read from OptParamFile structure";
//           Error( "Error", ret);
//         }

//}

////-------------------------------------------------------------------------------------------------



// ----------- End of  visor.cpp ----------------------------
