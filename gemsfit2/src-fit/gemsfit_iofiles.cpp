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
#include "data_manager.h"
#include "keywords.h"
#include <omp.h>
#include <sstream>

#include "statistics.h"
#include "optimization.h"
//#include "plot_class.h"
//using namespace opti;



// subfolder and file names default
//const char *INPUT_DIR = "input/";
//const char *OUTPUT_DIR = "output/";
//const char *RESULT_DIR = "results/";
//const char *OPT_PARAM_FILE = "gemsfit2_input.dat";
//const char *FIT_CSV_FILE = "FIT.csv";
//const char *FIT_STATISTIC = "MyFitStatistics.txt";
//const char *FIT_LOGFILE = "gemsfit2.log";

//void out_gems_fit_txt( TNode* node, bool _comment, bool brief_mode );
//void get_gems_fit_multi_txt( TNode* node );
//void get_gems_fit_DCH_txt( TNode* node );
//void get_gems_fit_DBR_txt( TNode* node );
/// if after F comes an JSON object ( F{...} )
void F_to_OP (opti_vector *op, IOJFormat Jformat, string nfild);
/// if after F comes the initial value ( F5000 )
void F_to_OP (double val, opti_vector *op, IOJFormat Jformat, string nfild);
/// if the parameter is reaction constrained ( R{...} )
void R_to_OP (opti_vector::RDc *r, IOJFormat Jformat);
/// if the parameter is linked e.g. in titration (L{...} )
void L_to_OP (opti_vector::Lp *l, IOJFormat Jformat, string nfild);

// this constructor implicitly calls another constructor
// opti_vector::opti_vector( ) which calls GEM_init() and reads in all
// GEMS3K files!  needs to be resolved!
optimization::optimization( int i )
{
    int ii;
    constraint_data = new my_constraint_data;
    optimization::define_nlopt_param();
    ii=i;
}

//// Constructor
statistics::statistics()
{
    number_of_measurements = 0;
    Weighted_Tfun_sum_of_residuals 		= 0.0;
    num_of_runs		= 0;
    number_of_parameters   = 0;
    Tfun_sum_of_residuals = 0.0;
    Abs_sum_of_residuals = 0.0; Weighted_Abs_sum_of_residuals = 0.0;
    min_res =0.0; max_res = 0.0; Weighted_min_res = 0.0; Weighted_max_res = 0.0; neg_residuals = 0; pos_residuals = 0;
    perturbator = 0.0001;
    default_stat_param();
}

// Constructor
Data_Manager::Data_Manager( int )
{
    define_db_specs();
}

/// Mode GEMSFIT to generate input configuration file
int generateConfig()
{
   string YN;
   try
   {
     // call GEM_init to read GEMS3K input files
     TNode* node  = new TNode();

     // call GEM_init     --> read in input files
     if( (node->GEM_init( gpf->GEMS3LstFilePath().c_str() )) == 1 )
        {
            cout<<" .. ERROR occurred while reading GEMS3K input files !!! ..."<<endl;
            return 1;
        }

    bool with_comments = true;
    bool brief_mode = false;

    // Writing to the data
    cout << "Start writing the input specification file template"<< endl;
    Data_Manager *data_meas = new Data_Manager(1);

    if ( access( gpf->OptParamFile().c_str(), 0 ) == 0 )
    {
        cout << gpf->OptParamFile() <<" exists. Do you want to overwrite it? write yes or no: ";
        cin >> YN;
        if ((YN == "yes") || (YN == "Yes") || (YN == "YES") || (YN == "yEs") || (YN == "yeS"))
        {
            goto overwrite;
        } else
        {
            cout << "Please, enter the new file name: ";
            cin >> YN;
            gpf->OptParamFileRename(YN.c_str());
            goto overwrite;
        }
   }  //  else
//    {
    overwrite:
        // Writing Data sources section
    data_meas->out_db_specs_txt(with_comments, brief_mode);

cout << "start writing out_gems_fit_txt()" << endl;
        // Writing Parameters to Fit section &
    out_gems_fit_txt( node, with_comments, brief_mode );

cout << "start writing out_nlopt_param_txt()" << endl;
    // Create instance of optimization class derived from base class Optimization
    optimization opti( 1 );
        // Writing Optimization Methods section
    opti.out_nlopt_param_txt(with_comments, brief_mode);

cout << "start writing out_stat_param_txt()" << endl;
    statistics stat;
        // Writing statistics section
    stat.out_stat_param_txt(with_comments, brief_mode);

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

//extern outField DataCH_static_fields[14];
extern outField DataCH_dynamic_fields[30];
extern outField MULTI_dynamic_fields[70];
extern outField DataBR_fields[f_lga+1/*58*/];

void out_gems_fit_txt( TNode* node, bool _comment, bool brief_mode )
{
    DATACH* CSD = node->pCSD();
    DATABR* CNode = node->pCNode();

    string fname = gpf->OptParamFile();
    fstream ff(fname.c_str(), ios::out|ios::app );
    ErrorIf( !ff.good() , fname.c_str(), "OptParamFile text open error");

    if(_comment )
    {
        ff << "\n#  GEM input parameters to fit can be marked by preceding them with the F, L, and R letter "
              "\n#    in <TKval>, <Pval>, <G0>,  <PMc>, <DMc>, <fDQF>, <bIC> ... data object entries, see below."
              "\n#    Values without markup will not be modified by GEMSFIT2 routines, just used in GEM runs.\n"
              "\n#  Mark with F the parameter value you want to fit independently of other parameters. "
              "\n#    Shorthand option: F<initval> - initial value with default (10%) upper- and lower boundaries."
              "\n#    Example:  ... F-833444 ... (<initval> can be modified, if necessary)."
              "\n#    Full JSON-style markup syntax for the independently adjusted parameter: as in this example"
              "\n#      for fitting G0(298) of SiO2@ "
              "\n#      F{ \"IV\" :-833444, \"UB\" : -800000, \"LB\" : -900000 } "
              "\n#      where:"
              "\n#         \"IV\": initial value; "
              "\n#         \"UB\": upper boundary; "
              "\n#         \"LB\": lower bundary (in this case, all in J/mol). \n"
              "\n#  Mark with R the G0(298) value of a Dependent Component, which depends on G0 of other DCs"
              "\n#     via a reaction constraint, by copy-pasting the following template in place of G0(298) value,"
              "\n#     and editing it as desired:"
              "\n#      R{ \"IV\" : -36819, \"Ref\" : \"SUPCRT92\", \"order\" : \"1\", \"nC\" : 4, \"rDC\" : \"KOH@\","
              "\n#      \"RC\" : [ \"K+\", \"H2O@\", \"H+\", \"KOH@\" ], \"Rcoef\" : [ -1, -1, 1, 1 ] }"
              "\n#     Here,  \"IV\": initial value; "
              "\n#         \"Ref\": bibliographic reference (optional);"
              "\n#         \"order\": reaction order | 1, 2, ... |, important for many reaction with common species;"
              "\n#         \"nC\": numer of components (species) involved in the reaction;"
              "\n#         \"DC\": name of Dependent Component whose properties are constrained with this reaction; "
              "\n#         \"RC\": list [ ] of names of all components (species) involved in reaction (comma-separated);"
              "\n#         \"Rcoef\": array [ ] of reaction stoichiometry coeficients (comma-separated), "
              "\n#           in the same order as in the \"RC\" list. This example describes a reaction: "
              "\n#           K+ + H2O@ = H+ + KOH@ in which G0(KOH@) follows variations of G0(K+) induced by the fitting, "
              "\n#           such that the logK of this reaction at (T,P) remains constant. \n"
              "\n#  Mark with L the bIC (system bulk composition) value of Independent Component, "
              "\n#     which depends on bIC of other elements via a titration constraint,"
              "\n#     by copy-pasting the following template in place of the bIC value, and editing as desired:"
              "\n#      L{ \"LE\" :\"H\", \"IV\" :113.016746705914, \"LEs\" :[\"S\", \"Cl\"], \"Lcoef\" :[2,1]}"
              "\n#     In this example: the amount of H is linked to S and Cl by titration with H2SO4 and HCl, "
              "\n#         with stoichiometry coeficients of 2 and 1, respectively."
              "\n#        \"LE\": linked element "
              "\n#        \"IV\": initial value "
              "\n#        \"LEs\": the elements linked to "
              "\n#        \"Lcoef\": linkage coefficients (stoichiometry coefficients) "
              "\n#         Whenever the bIC values of S or Cl are varied as (independent) titration parameters,"
              "\n#         the bIC value of H will be adjusted to titrate the system either with H2SO4 or with HCl."
           << endl;
    }

    TPrintArrays  prarCH(30, DataCH_dynamic_fields, ff);

    if( _comment )
        ff << "\n# ICNL: List of Independent Component names (for readability; no need to modify)";
    prarCH.writeArray(  "ICNL", CSD->ICNL[0], CSD->nIC, MaxICN );
    if( _comment )
            ff << "\n# DCNL: List of Dependent Component names (for readability; no need to modify)";
    prarCH.writeArray(  "DCNL", CSD->DCNL[0], CSD->nDC, MaxDCN );
    if( _comment )
            ff << "\n# PHNL: List of Phase names (for readability; no need to modify)";
    prarCH.writeArray(  "PHNL", CSD->PHNL[0], CSD->nPH, MaxPHN );
//    prarCH.writeArrayF(  f_ccPH, CSD->ccPH, CSD->nPH, 1L,_comment, brief_mode );
//    prarCH.writeArray(  f_nDCinPH, CSD->nDCinPH, CSD->nPH, -1L,_comment, brief_mode);
//    prarCH.writeArray(  f_TKval, CSD->TKval, CSD->nTp, -1L,_comment, brief_mode );
//    prarCH.writeArray(  f_Pval, CSD->Pval, CSD->nPp,  -1L,_comment, brief_mode );
    vector <double> xG0;
    ff << "\n \n# G0: Look-up array for DC Gibbs energy function g(T,P), J/mol at 298.15K and 1 bar \n";
    ff << "<G0>" << endl;
    for (int i=0; i<CSD->nDC*node->gridTP(); ++i)
    {
        xG0.push_back(CSD->G0[i]);
        ff << CSD->G0[i] << endl;
        i += node->gridTP()-1;
    }

    ff << "\n \n# logK: Look-up array for logK at T * P * nr reactions. "
          "\n#    If at least one G0 parameter is marked as \'R\' (reaction-constrained)"
          "\n#    and the list below is left commented out, then logK values for all T,P pairs and reactions"
          "\n#    will be calculated based on the initial values of all parameters, and this logK array"
          "\n#    will be used throughout the fitting process. " << endl;
    ff << "#<logK>" << endl;

    if(_comment )
    {
        ff << "\n\n#########################################################################" << endl;
        ff << "#>>>>>>>>>>>>>>> Input for fitting GEM input parameters >>>>>>>>>>>>>>>>#" << endl;
        ff << "#########################################################################" << endl;
    }
    TPrintArrays  prarIPM( 70, MULTI_dynamic_fields, ff);
    MULTI* pmp = node->pMulti()->GetPM();

    if( pmp->FIs > 0 && pmp->Ls > 0 )
    {
      if( _comment )
         ff << "\n# Initial data for multicomponent phases (fragment of GEMS3K *IMP.dat input file)"
               "\n#    (see the DCH file for the dimension nPHs)";
      prarIPM.writeArrayF(  f_sMod, pmp->sMod[0], pmp->FIs, 8L, _comment, brief_mode );

    long int LsModSum;
    long int LsIPxSum;
    long int LsMdcSum;
    long int LsMsnSum;
    long int LsSitSum;
    node->pMulti()->getLsModsum( LsModSum, LsIPxSum );
    node->pMulti()->getLsMdcsum( LsMdcSum, LsMsnSum, LsSitSum );

     prarIPM.writeArray(  f_LsMod, pmp->LsMod, pmp->FIs*3, 3L, _comment, brief_mode);

      if(LsIPxSum )
      {
       if( _comment )
          ff << "\n\n# IPxPH: Index lists (in TSolMod convention) for interaction parameters of non-ideal solutions";
       prarIPM.writeArray(  "IPxPH", pmp->IPx,  LsIPxSum);
      }
      if(LsModSum )
       {
         if( _comment )
            ff << "\n\n# PMc: Tables (in TSolMod convention) of interaction parameter coefficients for non-ideal solutions";
        prarIPM.writeArray(  "PMc", pmp->PMc,  LsModSum);
       }

       prarIPM.writeArray(  f_LsMdc, pmp->LsMdc, pmp->FIs*3, 3L, _comment, brief_mode);
       if(LsMdcSum )
       {   if( _comment )
              ff << "\n\n# DMc: Tables (in TSolMod convention) of  parameter coefficients for Dependent Components";
        prarIPM.writeArray(  "DMc", pmp->DMc,  LsMdcSum);
       }
       if(LsMsnSum )
       {   if( _comment )
              ff << "\n\n# MoiSN:  End member moiety / site multiplicity number tables (in TSolMod convention) ";
        prarIPM.writeArray(  "MoiSN", pmp->MoiSN,  LsMsnSum);
       }
    } // sMod
    prarIPM.writeArray(  f_fDQF, pmp->fDQF,  pmp->L, -1L, _comment, brief_mode);


    if( _comment )
       ff << "\n\n# This part for the system composition data is taken from the *DBR.dat file\n";

    // from *DBR.dat
    TPrintArrays  prar(f_bSP+1/*52*/, DataBR_fields, ff);

    ff<< "\n# For fitting T and P parameters in thermobarometry application, give the upper and lower values"
         "\n#   corresponding to the interpolation range that you selected when exporting GEMS3K input files ";

    prar.writeField(f_TK, CNode->TK, _comment, brief_mode  );
    prar.writeField(f_P, CNode->P, _comment, brief_mode  );
    if( _comment )
     {   ff << "\n\n## (4) Data for Independent Components";
         prar.writeArray(  NULL, CSD->ICNL[0], CSD->nIC, MaxICN );
     }
    prar.writeArray(  f_bIC,  CNode->bIC, CSD->nICb, -1L,_comment, brief_mode );
    if( _comment )
    {    ff << "\n\n## (5) Data for Dependent Components";
         prar.writeArray(  NULL, CSD->DCNL[0], CSD->nDC, MaxDCN );
     }
    prar.writeArray(  f_xDC,  CNode->xDC, CSD->nDCb, -1L,_comment, brief_mode  );
    prar.writeArray(  f_dll,  CNode->dll, CSD->nDCb, -1L,_comment, brief_mode  );
    prar.writeArray(  f_dul,  CNode->dul, CSD->nDCb, -1L,_comment, brief_mode  );
    if( _comment )
    {    ff << "\n\n## (6) Data for Phases";
          prar.writeArray(  NULL, CSD->PHNL[0], CSD->nPH, MaxPHN );
    }
    prar.writeArray(  f_aPH,  CNode->aPH, CSD->nPHb, -1L,_comment, brief_mode );

    prar.writeField(f_Vs, CNode->Vs, _comment, brief_mode  );
    prar.writeField(f_Ms, CNode->Ms, _comment, brief_mode  );
    prar.writeField(f_Gs, CNode->Gs, _comment, brief_mode  );
    if( CSD->ccPH[0] == PH_AQUEL )
    {
       prar.writeField(f_IS, CNode->IC, _comment, brief_mode  );
       prar.writeField(f_pH, CNode->pH, _comment, brief_mode  );
       prar.writeField(f_pe, CNode->pe, _comment, brief_mode  );
       prar.writeField(f_Eh, CNode->Eh, _comment, brief_mode  );
    }
    if( _comment )
     {
         ff << "\n\n## (4) Data for Independent Components";
         prar.writeArray(  NULL, CSD->ICNL[0], CSD->nIC, MaxICN );
     }
    prar.writeArray(  f_uIC,  CNode->uIC, CSD->nICb, -1L,_comment, brief_mode );
    prar.writeArray(  f_bSP,  CNode->bSP, CSD->nICb, -1L,_comment, brief_mode );

    if( _comment )
    {    ff << "\n\n## (5) Data for Dependent Components";
         prar.writeArray(  NULL, CSD->DCNL[0], CSD->nDC, MaxDCN );
    }
    //prar.writeArray(  f_xDC,  CNode->xDC, CSD->nDCb, -1L,_comment, brief_mode  );
    prar.writeArray(  f_gam,  CNode->gam, CSD->nDCb, -1L,_comment, brief_mode  );

    if( _comment )
    {    ff << "\n\n## (6) Data for Phases";
          prar.writeArray(  NULL, CSD->PHNL[0], CSD->nPH, MaxPHN );
    }
    prar.writeArray(  f_xPH,  CNode->xPH, CSD->nPHb, -1L,_comment, brief_mode );
    prar.writeArray(  f_vPS,  CNode->vPS, CSD->nPSb, -1L,_comment, brief_mode );
    prar.writeArray(  f_mPS,  CNode->mPS, CSD->nPSb, -1L,_comment, brief_mode );
    prar.writeArray(  f_xPA,  CNode->xPA, CSD->nPSb, -1L,_comment, brief_mode );

    if(!brief_mode || prar.getAlws( f_bPS ))
    {  if( _comment )
       {
            ff << DataBR_fields[f_bPS].comment.c_str();
        prar.writeArray(  NULL, CSD->ICNL[0], CSD->nIC, MaxICN );
        }
       prar.writeArray(  f_bPS,  CNode->bPS, CSD->nPSb*CSD->nICb, CSD->nICb,false, brief_mode );
    }
    ff.close();
}

/// Reading part of the MULTI structure from OptParamFile text file
void get_gems_fit_multi_txt(TNode* node, opti_vector *op )
{
    // open file for reading
    string fname = gpf->OptParamFile();
    fstream ff(fname.c_str(), ios::in );
    ErrorIf( !ff.good() , fname, "OptParamFile Fileopen error");

    TReadArrays  rddar(70, MULTI_dynamic_fields, ff);
    MULTI* pmp = node->pMulti()->GetPM();
    vector<IOJFormat> vFormats;

    long int nfild = rddar.findNextNotAll();
    while( nfild >=0 )
        {
          switch( nfild )
          {
          case f_sMod: if( !pmp->sMod )
                          Error( "Error", "Array sMod is not used in this problem");
                        rddar.readArray( "sMod" , pmp->sMod[0], pmp->FIs, 8 );
                        break;
          case f_LsMod:{ if( !pmp->LsMod )
                          Error( "Error", "Array LsMod is not used in this problem");
                        rddar.readArray( "LsMod" , pmp->LsMod, pmp->FIs*3) ;
                        long int LsModSum;
                        long int LsIPxSum;
                        node->pMulti()->getLsModsum( LsModSum, LsIPxSum );
                        if(LsIPxSum )
                        { rddar.readNext( "IPxPH");
                          if(!pmp->IPx )
                            pmp->IPx = new long int[LsIPxSum];
                          rddar.readArray( "IPxPH", pmp->IPx,  LsIPxSum);
                        }
                        if(LsModSum )
                        { rddar.readNext( "PMc");
                          if(!pmp->PMc )
                            pmp->PMc = new double[LsModSum];
                          rddar.readFormatArray( "PMc", pmp->PMc,  LsModSum, vFormats ); // expect params to fit here
                        }
                        break;
                       }
                case f_LsMdc: { if( !pmp->LsMdc )
                             Error( "Error", "Array LsMdc not used in this problem");
                          rddar.readArray( "LsMdc" , pmp->LsMdc, pmp->FIs*3 );
                          long int LsMdcSum;
                          long int LsMsnSum;
                          long int LsSitSum;
                          node->pMulti()->getLsMdcsum( LsMdcSum,LsMsnSum, LsSitSum );
                          if(LsMdcSum )
                          { rddar.readNext( "DMc");
                            if(!pmp->DMc )
                               pmp->DMc = new double[LsMdcSum];
                          rddar.readFormatArray( "DMc", pmp->DMc,  LsMdcSum, vFormats );
                          }
                        if(LsMsnSum )
                        { rddar.readNext( "MoiSN");
                          if(!pmp->MoiSN )
                             pmp->MoiSN = new double[LsMsnSum];
                        rddar.readArray( "MoiSN", pmp->MoiSN,  LsMsnSum);
                        }
                        break;
                      }
                case f_fDQF: rddar.readFormatArray( "fDQF", pmp->fDQF,  pmp->L, vFormats );
                        break;
          }

          if( !vFormats.empty() )
          {
              string fPMc = "PMc";
              string fDMc = "DMc";
              string ffDQF = "fDQF";
              // Here write your code for input parameters
              for(unsigned int ii=0; ii<vFormats.size(); ii++ )
              {
cout << "Parameter: " << MULTI_dynamic_fields[nfild].name << " Type " << vFormats[ii].type <<  " Index " << vFormats[ii].index;
cout << " : " << vFormats[ii].format << endl;
                  if ((vFormats[ii].type == ft_F))
                  {
                      if (vFormats[ii].format.size() >1)
                      {   // after F comes a JSON object
                          switch( nfild )
                          {
                             case f_LsMod:  // interaction parameters     (PMc table)
gpf->fout << "    Parameter: " << fPMc << " Type " << vFormats[ii].type <<  " Index " << vFormats[ii].index;
                                 F_to_OP(op, vFormats[ii], fPMc );
                                 break;
                             case f_LsMdc:  // phase component parameters (DMc table)
gpf->fout << "    Parameter: " << fDMc << " Type " << vFormats[ii].type <<  " Index " << vFormats[ii].index;
                                 F_to_OP(op, vFormats[ii], fDMc );
                                 break;
                             default:
gpf->fout << "    Parameter: " << MULTI_dynamic_fields[nfild].name << " Type " << vFormats[ii].type <<  " Index " << vFormats[ii].index;
                                 F_to_OP(op, vFormats[ii], MULTI_dynamic_fields[nfild].name );
                          }
                      } else
                      {   // after F comes the initial value
                          switch( nfild )
                          {
                          case f_sMod:
                                       break;
                          case f_LsMod:{ if( !pmp->LsMod )
                                            Error( "Error", "Arrays LsMod and PMc are not used in this GEM system definition!");
                                            long int LsModSum;
                                            long int LsIPxSum;
                                            node->pMulti()->getLsModsum( LsModSum, LsIPxSum );
                                            if(LsIPxSum )
                                            {
                                            }
                                            if(LsModSum )
gpf->fout << "    Parameter: " << fPMc << " Type " << vFormats[ii].type <<  " Index " << vFormats[ii].index;
                                                F_to_OP(pmp->PMc[vFormats[ii].index], op, vFormats[ii], fPMc );
                                        break;
                                       }
                          case f_LsMdc:{ if( !pmp->LsMdc )
                                            Error( "Error", "Array LsMdc and DMc are not used in this GEM system definition!");
                                            long int LsMdcSum;
                                            long int LsMsnSum;
                                            long int LsSitSum;
                                            node->pMulti()->getLsMdcsum( LsMdcSum,LsMsnSum, LsSitSum );
                                            if(LsMdcSum )
gpf->fout << "    Parameter: " << fDMc << " Type " << vFormats[ii].type <<  " Index " << vFormats[ii].index;
                                            F_to_OP(pmp->DMc[vFormats[ii].index], op, vFormats[ii], fDMc );
                                        break;
                                        }
                          case f_fDQF:
gpf->fout << "    Parameter: " << MULTI_dynamic_fields[nfild].name << " Type " << vFormats[ii].type <<  " Index " << vFormats[ii].index;
                               F_to_OP(pmp->fDQF[vFormats[ii].index], op, vFormats[ii], MULTI_dynamic_fields[nfild].name );
                                        break;
                          }
                      }
                  }
gpf->fout << " : " << vFormats[ii].format << endl;
              }
              vFormats.clear();
          }
          nfild = rddar.findNextNotAll();
        }
}

/// Reading part dataCH structure from OptParamFile text file
void get_gems_fit_DCH_txt(TNode* node, opti_vector* op )
{
    // open file for reading
    string fname = gpf->OptParamFile();
    fstream ff(fname.c_str(), ios::in );
    ErrorIf( !ff.good() , fname, "OptParamFile Fileopen error");

    TReadArrays  rddar(30, DataCH_dynamic_fields, ff);
    DATACH* CSD = node->pCSD();
    vector<IOJFormat> vFormats;
    int nr_reac = 0;
//    double logK[9];

    long int nfild = rddar.findNextNotAll();
    while( nfild >=0 )
      {
        switch( nfild )
        {
        case f_ICNL: rddar.readArray( "ICNL", CSD->ICNL[0], CSD->nIC, MaxICN );
                break;
        case f_DCNL: rddar.readArray( "DCNL", CSD->DCNL[0], CSD->nDC, MaxDCN );
                break;
        case f_PHNL: rddar.readArray( "PHNL", CSD->PHNL[0], CSD->nPH, MaxPHN );
                break;
//        case f_ccPH: rddar.readArray( "ccPH", CSD->ccPH, CSD->nPH, 1 );
//                break;
//        case f_nDCinPH: rddar.readArray( "nDCinPH", CSD->nDCinPH, CSD->nPH);
//                break;
//        case f_TKval: rddar.readArray( "TKval", CSD->TKval, CSD->nTp );
//                break;
//        case f_Pval: rddar.readArray( "Pval", CSD->Pval, CSD->nPp );
//                  break;
        case f_G0: rddar.readFormatArray( "G0", CSD->G0, CSD->nDC*node->gridTP(), vFormats );
                        for (unsigned int i=0; i<vFormats.size(); ++i)
                        {
                            if (vFormats[i].type == ft_R )
                            {
                                ++nr_reac;
                                op->reactions.push_back(new opti_vector::RDc);
                            }
                        }
                  break;
                        // not working until dealing on how to get the logK look-up array into the input file
//        case f_logK: rddar.readFormatArray( "logK", logK, nr_reac*1/**node->gridTP()*/, vFormats );
//                  break;
         }

        if( !vFormats.empty() )
        {
            int nr = 0;
            // Hear you must write your code
            for(unsigned  int ii=0; ii<vFormats.size(); ii++ )
            {
cout << "Parameter: " << DataCH_dynamic_fields[nfild].name << " " << node->xCH_to_DC_name(vFormats[ii].index)<< " Type " << vFormats[ii].type;
cout << " : " << vFormats[ii].format << endl;
                if ((vFormats[ii].type == ft_F))
                {
                    if (vFormats[ii].format.size() > 1)
                    {
                        // after F comes a JSON object
                        F_to_OP(op, vFormats[ii], DataCH_dynamic_fields[nfild].name );
                    } else
                    {
                        // after F comes initial value
                        switch( nfild )
                        {
                        // Case G0
                        case f_G0: F_to_OP(CSD->G0[vFormats[ii].index], op, vFormats[ii], DataCH_dynamic_fields[nfild].name );
                                  break;
                        // add other parameters
                         }
                    }
                }
                if (vFormats[ii].type == ft_R)
                {
//                    op->reactions.push_back(new opti_vector::RDc);
                    op->h_RDc = true;
                    vector<string> out;
                    Data_Manager *temp = new Data_Manager(1);
                    temp->parse_JSON_object(vFormats[ii].format, keys::order, out);
                    nr = atoi(out.at(0).c_str())-1;
                    out.clear();

                    R_to_OP(op->reactions[nr], vFormats[ii] ); // , DataCH_dynamic_fields[nfild].name );

                }
            }
            vFormats.clear();
        }
        nfild = rddar.findNextNotAll();
    }

    unsigned int size = CSD->nTp, sizep = 0;
    for (int i=0; i<CSD->nPp; ++i)
    {
        if (CSD->Pval[i] != 0)
            ++sizep;
    }
    size = size*sizep;


    if (op->h_RDc)
    {
        vector<string> data;
        string line, allparam;
        string LogK_s;
        string sub_LogK;
        int pos_start, pos_end;
        unsigned int i;
        ifstream param_stream;
        string f3("<logK>");
        string f4("#");

        param_stream.open(fname.c_str());
        if( param_stream.fail() )
        {
            cout << "Opening of file "<<fname<<" failed !!"<<endl;
            exit(1);
        }
        while( getline(param_stream, line) )
        {
            data.push_back(line);
        }
        param_stream.close();
        for( i=0; i < data.size(); i++ )
        allparam += data[i];

            pos_start = allparam.find(f3);
            pos_end   = allparam.find(f4,pos_start);
            LogK_s = allparam.substr((pos_start+f3.length()),(pos_end-pos_start-f3.length()));
            istringstream LogK_ss(LogK_s);
            istringstream test_ss(LogK_s);
            vector <double> test;
            do
            {
             test_ss >> sub_LogK;
            test.push_back(atof(sub_LogK.c_str()));
            }while(test_ss);
            test.pop_back();

            if ((test.size() != size*nr_reac) && (test.size() != 0))
            {
                cout << "Number of logk's doesn't correspond to number of T*P*reactions! " << endl;
                cout << "You need " << size*nr_reac << " <logK> entries in the input file, you have " << test.size() << endl;
                exit(1);
            }

            if (test.size() != 0)
            do
            {
                for (int i = 0; i<nr_reac; ++i)
                {
                    for (unsigned int j=0; j<size; ++j)
                    {
                        LogK_ss >> sub_LogK;
                        op->reactions[i]->logK_TPpairs.push_back(atof(sub_LogK.c_str()));
                    }
                }
                LogK_ss >> sub_LogK;
            }while(LogK_ss);
//            charges.pop_back();
    }



}


/// Reading part dataBR structure from OptParamFile text file
void get_gems_fit_DBR_txt(TNode* node , opti_vector *op)
{
    // open file for reading
    string fname = gpf->OptParamFile();
    fstream ff(fname.c_str(), ios::in );
    ErrorIf( !ff.good() , fname, "OptParamFile Fileopen error");

    TReadArrays  rdar(f_bSP+1/*52*/, DataBR_fields, ff);
    DATACH* CSD = node->pCSD();
    DATABR* CNode = node->pCNode();
    vector<IOJFormat> vFormats;

    long int nfild = rdar.findNextNotAll();
    while( nfild >=0 )
      {
        switch( nfild )
        {
        case f_TK: rdar.readFormatArray( "TK",  &CNode->TK, 1, vFormats); // i
                break;
        case f_P: rdar.readFormatArray( "P",  &CNode->P, 1, vFormats);  // i
                break;
        case f_Vs: rdar.readFormatArray( "Vs", &CNode->Vs, 1, vFormats);
                break;
        case f_Ms: rdar.readFormatArray( "Ms",  &CNode->Ms, 1, vFormats);
                break;
        case f_Hs: rdar.readFormatArray( "Hs",  &CNode->Hs, 1, vFormats);
                break;
        case f_Gs: rdar.readFormatArray( "Gs",  &CNode->Gs, 1, vFormats);
                 break;
        case f_IS: rdar.readFormatArray( "IS",  &CNode->IC, 1, vFormats);
                break;
        case f_pH: rdar.readFormatArray( "pH",  &CNode->pH, 1, vFormats);
                break;
        case f_pe: rdar.readFormatArray( "pe",  &CNode->pe, 1, vFormats);
                break;
        case f_Eh: rdar.readFormatArray( "Eh",  &CNode->Eh, 1, vFormats);
                break;
        case f_bIC: rdar.readFormatArray( "bIC",  CNode->bIC, CSD->nICb, vFormats ); // i
                break;
        case f_uIC: rdar.readFormatArray( "uIC",  CNode->uIC, CSD->nICb, vFormats );
                break;
        case f_xDC: rdar.readFormatArray( "xDC",  CNode->xDC, CSD->nDCb, vFormats );
                break;
        case f_gam: rdar.readFormatArray( "gam",  CNode->gam, CSD->nDCb, vFormats );
                break;
        case f_dll: rdar.readFormatArray( "dll",  CNode->dll, CSD->nDCb, vFormats );  // i
                break;
        case f_dul: rdar.readFormatArray( "dul",  CNode->dul, CSD->nDCb, vFormats );  // i
                break;
        case f_aPH: rdar.readFormatArray( "aPH",  CNode->aPH, CSD->nPHb, vFormats );  // i
                break;
        case f_xPH: rdar.readFormatArray( "xPH",  CNode->xPH, CSD->nPHb, vFormats );
                break;
        case f_vPS: rdar.readFormatArray( "vPS",  CNode->vPS, CSD->nPSb, vFormats );
                break;
        case f_mPS: rdar.readFormatArray( "mPS",  CNode->mPS, CSD->nPSb, vFormats );
                break;
        case f_bPS: rdar.readFormatArray( "bPS",  CNode->bPS, CSD->nPSb*CSD->nICb, vFormats );
                break;
        case f_xPA: rdar.readFormatArray( "xPA",  CNode->xPA, CSD->nPSb, vFormats );
                break;
        case f_bSP: rdar.readFormatArray( "bSP",  CNode->bSP, CSD->nICb, vFormats );
               break;
         }
        if( !vFormats.empty() )
        {
            int nl = 0;
            // Here you must write your code
            for(unsigned  int ii=0; ii<vFormats.size(); ii++ )
            {
cout<< "Parameter: " << DataBR_fields[nfild].name << " Type " << vFormats[ii].type << " Index " << vFormats[ii].index << endl;
cout<< vFormats[ii].format << endl;
                if ((vFormats[ii].type == ft_F))
                {
                    if (vFormats[ii].format.size() > 1)
                    {             // after F comes a JSON object
                        F_to_OP(op, vFormats[ii], DataBR_fields[nfild].name );
                    } else
                    {              // after F comes initial value
                        switch( nfild )
                        {
                            case f_TK: F_to_OP(CNode->TK, op, vFormats[ii], DataBR_fields[nfild].name );
                                      break;
                            case f_P: F_to_OP(CNode->P, op, vFormats[ii], DataBR_fields[nfild].name );
                                      break;
                            case f_bIC: F_to_OP(CNode->bIC[vFormats[ii].index], op, vFormats[ii], DataBR_fields[nfild].name ); // i
                                      break;
                        // add other parameters
                        }
                    }
                }
                if (vFormats[ii].type == ft_L)
                {
                    op->Lparams.push_back(new opti_vector::Lp);
                    op->h_Lp = true;
                    L_to_OP(op->Lparams[nl], vFormats[ii], DataBR_fields[nfild].name );
                    nl++;
                }
            }
            vFormats.clear();
        }
        nfild = rdar.findNextNotAll();
        }
}

//----------------------------------------------------------------
// TGfitPath  class implementation
//----------------------------------------------------------------
//----- subfolder and default file names  ------------------------
const char *INPUT_DIR = "input/";
const char *OUTPUT_DIR = "output/";
const char *RESULT_DIR = "results/";
const char *OPT_PARAM_FILE = "gemsfit2_input.dat";
const char *FIT_CSV_FILE = "FIT.csv";
const char *FIT_STATISTIC = "MyFitStatistics.txt";
const char *FIT_LOGFILE = "gemsfit2.log";

TGfitPath::TGfitPath(int c, char *v[]):
        argc(c), argv(v)
{
    optParamFilePath = "";
    gems3LstFilePath = "";

//#ifdef __unix
//        optParamFilePath = getenv("HOME");
//#else
//        optParamFilePath = ".";
//#endif

    // parsing options -init, -run, -conf if given

    int iinit = 0;		// index of -init option
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
            if (argc <= irun + 1)
                Error("Wrong options", "Wrong argument for option -run");
            optParamFile = argv[irun + 1];
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

        if (iconf != 0)  // needs reconsideration
        {
            if (argc <= iconf + 1)
                Error("Wrong options", "Wrong argument for option -conf");
            optParamFile = argv[iconf + 1];
        }

        if( optParamFile.empty() )
        {
            optParamFile = optParamFilePath;
            optParamFile += "/";
            optParamFile += OPT_PARAM_FILE;
        }
        else
        {
            string name;
            string ext;
            u_splitpath( optParamFile, optParamFilePath, name, ext );
        }

        string path = optParamFilePath;
        string path_ = resultDir+"FIT_results.csv";
        switch( mode )
        {
            case INIT_:
                // set up default paths
                // later these paths and filenames can be read from config file
                inputDir = path + INPUT_DIR;
                outputDir = path + OUTPUT_DIR;
                resultDir = path + RESULT_DIR;
                fitFile = resultDir+FIT_CSV_FILE;
                fitStatistics = outputDir+FIT_STATISTIC;
                fitLogFile = outputDir+FIT_LOGFILE;
                break;
            case RUN_:
                path += "/";
                // set up default paths
                // later these paths and filenames can be read from config file
                inputDir = path + INPUT_DIR;
                outputDir = path + OUTPUT_DIR;
                resultDir = path + RESULT_DIR;
                fitFile = resultDir+FIT_CSV_FILE;
                fitStatistics = outputDir+FIT_STATISTIC;
                fitLogFile = outputDir+FIT_LOGFILE;
                // GEMSFIT logfile (usually log_gemsfit.log)
                fout.open( fitLogFile.c_str(), ios::out | ios::trunc );
                if( fout.fail() )
                { cout<<"Logfile fileopen error"<<endl;
                   exit(1); }
                // GEMSFIT logfile
                fout_.open(path_.c_str(), ios::out | ios::trunc );
                if( fout_.fail() )
                { cout<<"Output fileopen error"<<endl;
                   exit(1); }
                fout << "GEMSFIT2: Start" << endl;
                fout << "optParamFile = " << optParamFile << endl;
                fout << "fitFile = " << fitFile << endl;
                fout << "fitLogFile = " << fitLogFile << endl;
                fout << "gems3LstFilePath = " << gems3LstFilePath << endl;
                fout.close();
                break;
            default:
                // set up default paths
                // later these paths and filenames can be read from config file
                inputDir = path + INPUT_DIR;
                outputDir = path + OUTPUT_DIR;
                resultDir = path + RESULT_DIR;
                fitFile = resultDir+FIT_CSV_FILE;
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

outField Data_Manager_fields[9] =
{
    { "MPI", 0, 0, 1, "\n# MPI: Number of threads for parallelization\n" },
    { "DataDB",  0, 0, 1, "\n# DataDB: EJDB database path (please, edit to put the actual path)\n" },
    { "DataCollection",  0, 0, 1, "\n# DataCollection: database collection name (please, edit to put the actual name)\n" },
    { "DataSource",  1, 0, 1, "\n# DataSource: get experimental data from the EJDB format database"
      "\n#     (default:0, no other sources yet implemented in GEMSFIT2)\n" },

    { "DataSelect", 0, 0, 1, "# DataSelect: query for obtaining the experimental data from the database."
      "\n# Options: "
//      "\n#    \'all\': select all data"
      "\n#    \'{ ... }\': script in JSON format (in braces) describing what to select. "
      "\n#      \"sample\": [...]: list of comma-separated names of samples, "
      "\n#         or empty string \"\" to select all samples."
      "\n#      \"usedataset\": [...]: list of comma-separated names of experimental datasets to be used, "
      "\n#         or empty string \"\" to select all available datasets."
      "\n#      \"skipdataset\": [...]: list of comma-separated names of experimental datasets to be skipped, "
      "\n#         or empty string \"\" to not skip (in this case usedataset will apply)."
//      "\n#      \"skipsample\": [...]: list of comma-separated names of samples to be skipped, "
//      "\n#         or empty string "" to not skip (in this case, sample list will apply)."
      "\n#      \"sT\": [...]: list of comma-separated minimum and maximum temperature, C, "
      "\n#         or empty string \"\" to select data for all available temperatures. "
      "\n#      \"sP\": [...]: list of comma-separated minimum and maximum pressures, bar, "
      "\n#         or empty string \"\" to select data for all available pressures.  "
      "\n#      Example: "
      "\n#      \'{ \"sample\" : \"\", "
      "\n#        \"usedataset\" : [\"CH04\", \"CH04D\"],"
      "\n#        \"skipdataset\" : [\"CH03\", \"CH03D\"],"
      "\n#        \"sT\" : [100, 1000],"
      "\n#        \"sP\" : [1, 2500] }\'"
      "\n#      Copy the example and paste it below, remove comment symbols (#), and edit as necessary.\n"
    },

    { "DataTarget",  0, 0, 1, "# DataTarget: Target function for parameter fitting, described in JSON style"
      "\n#     as text in single quotes and braces: \'{ ... }\' "
      "\n#  \"Target\":  name of the target (objective) function (optional)"
      "\n#  \"TT\":  type of the target function as sum of the following terms: "
      "\n#     \"lsq\":       w*(measured-simulated)^2;"
      "\n#     \"lsq-norm\":  w*(measured/avgm-simulated/avgm)^2;"
      "\n#      ... "
      "\n#      here, avgm is the arithmetic mean of measured values."
      "\n#  \"WT\":  weighting scheme for samples, one of the following options:"
      "\n#       empty string \"\": w = 1 (equal weights);"
      "\n#      \"inverr\": w=1/error; \"inverr2\": w=1/error^2; \"inverr3\": w=1/measured^2;"
      "\n#      \"inverr_norm\": w=1/(error/avgm)^2; "
      "\n#       ... "
      "\n#  \"OFUN\":  objective function, a list [] of terms {} for measured properties to compare"
      "\n#     with their computed counterparts. Each term can contain property-value pairs:"
      "\n#      \"EPH\": for what phase from the experiments"
      "\n#      \"CT\":  for the type of data to compare:"
      "\n#          \"IC\" for independent component; \"DC\" for dependent component; "
      "\n#          \"prop\" for phase property; \"MR\" for mole fraction; "
      "\n#      \"CN\":  the name/formula of the data to compare. e.g. \"Al\" if \"CT\" is \"IC\","
      "\n#          or \"pH\" if \"CT\" is \"prop\", or \"K/(Na+K)\" if \"CT\" is \"MR\" "
      "\n#      \"DCP\": used only if \"CT\" is \"DC\", to represent the name of dependent component property:"
      "\n#          \"Q\" for amount; \"@coef\" for activity coeficient"
      "\n#       \"unit\":  units of measurement (override those given in the database for this value):"
      "\n#          \"molal\":  mol/(kg H2O), \"loga\": log(molal), \"-loga\": negated log(molal);"
      "\n#          \"g\"; \"kg\"; \"cm3\"; \"m3\"; \"molfrac\": mole fraction; J/mol for Gex "
      "\n#           ..."
      "\n#       (conversions will be performed automatically)."
      "\n#     The comparison options are: "
      "\n#       aqueous phase (\"aq_gen\") elemental composition in \"molal\" or \"loga\" "
      "\n#       aqueous phase (\"aq_gen\") properties (\"prop\"): \"pH\" in \"-loga\" (or \"molal\" "
      "\n#          - molality concentration of H+); \"Q\" - mass in \"g\" or \"kg\"  "
      "\n#       other phases composition as element bulk amount in moles (\"mol\") "
      "\n#          or to /Si molar ratio (\"Simolfrac\")"
      "\n#       other phases properties (\"prop\"): \"Q\" - mass in \"g\" or \"kg\"; "
      "\n#          \"pV\" - volume in \"cm3\" or \"m3\";  \"Eh\" - volume in \"Volts\"; "
      "\n#          \"Gex\"  - excess Gibbs energy of mixing (in \"J/mol\"). "
      "\n#       dependent components (\"DC\") properties: \"Q\" - amount in \"mol\"; "
      "\n#          \"@coef\" - activity coefictient "
      "\n#           ..."
      "\n#  Example:"
      "\n#  \'{ \"Target\": \"name\", \"TT\": \"lsq\", \"WT\": \"inverr\", \"OFUN\":"
      "\n#      ["
      "\n#         { \"EPH\": \"aq_gen\", \"CT\": \"IC\", \"CN\": \"Si\", \"unit\": \"molal\" },"
      "\n#         { \"EPH\": \"aq_gen\", \"CT\": \"IC\", \"CN\": \"Al\", \"unit\": \"molal\" },"
      "\n#         { \"EPH\": \"aq_gen\", \"CT\": \"prop\", \"CN\": \"pH\", \"unit\": \"-loga\" },"
      "\n#         { \"EPH\": \"aq_gen\", \"CT\": \"DC\", \"CN\": \"SiO2@\", \"DCP\" : \"Q\", \"unit\": \"mol\"}"
      "\n#      ]"
      "\n#   }\'"
      "\n#  Copy the example, paste it below, remove the comment symbols (#), and edit as necessary. "
      "\n#     Remove the unnecessary rows {...}\n"

    },
    { "SystemFiles",  0, 0, 1, "\n# SystemFiles: path to the list of GEMS3K input files (also used for this template file)\n" },
//    { "RecipeFiles",  0, 0, 1, "\n# RecipeFiles: Comment"},
    { "LimitOfDetection",  0, 0, 1, "\n# LimitOfDetection: Limit of detection of the measured values."
      "\n#     Ensures that wrongly computed output values due to non-physical input parameter values are ignored.\n" }
};

typedef enum { /// Field index into outField structure
    f_MPI = 0,
    f_DataDB,
    f_DataCollection,
    f_DataSource,
    f_DataSelect,
    f_DataTarget,
    f_SystemFiles,
//    f_RecipeFiles,
    f_LimitOfDetection
} Data_Manager_FIELDS;

/// Set up default values for structure
void Data_Manager::define_db_specs( )
{
   MPI = omp_get_num_threads() * omp_get_num_procs();
   datasource = 0;
   DBname = "./test_input/experimentsDB";
   collection= "experiments";
   DataSelect ="{...}";
   DataTarget = "{[...]}";
   LimitOfDetection = 1e-06;

}

/// Writes structure to the GEMSFIT2 input specification file
void Data_Manager::out_db_specs_txt( bool with_comments, bool brief_mode )
{
    string fname = gpf->OptParamFile();
    fstream ff(fname.c_str(), ios::out/*|ios::app*/ );
    ErrorIf( !ff.good() , fname.c_str(), "OptParamFile text open error");

    TPrintArrays  prar(10, Data_Manager_fields, ff);
    // define data that must be written

    if (datasource == 0)
    {
        prar.setAlws( f_MPI );
        prar.setAlws( f_DataDB );
        prar.setAlws( f_DataCollection );
        prar.setAlws( f_DataSelect );
        prar.setAlws( f_LimitOfDetection );
    }

    if(with_comments )
    {
        ff << "#########################################################################" << endl;
        ff << "#>>>>>>>>>>> GEMSFIT2 input specification file template >>>>>>>>>>>>>>>>#" << endl;
        ff << "#########################################################################" << endl;
        ff << "#  Character '#' means a comment line ";
    }

    prar.writeField(f_MPI, (long int)MPI, with_comments, brief_mode  );

    if(with_comments )
    {
        ff << "\n\n#########################################################################" << endl;
        ff << "#>>>>>>>>>>>>>>>>>>>>>>>> Data Sources section >>>>>>>>>>>>>>>>>>>>>>>>>#" << endl;
        ff << "#########################################################################";
    }
    prar.writeField(f_DataSource, (long int)datasource, with_comments, brief_mode  );
    prar.writeField(f_DataDB, DBname, with_comments, brief_mode  );
    prar.writeField(f_DataCollection, collection, with_comments, brief_mode  );
    prar.writeField(f_SystemFiles, gpf->GEMS3LstFilePath(), with_comments, brief_mode  );
//    prar.writeField(f_RecipeFiles, "", with_comments, brief_mode  );

    if(with_comments )
    {
        ff << "\n\n#########################################################################" << endl;
        ff << "#>>>>>>>>>>>>>>>>>>>>>>> DataSelect section >>>>>>>>>>>>>>>>>>>>>>>>>>>>#" << endl;
        ff << "#########################################################################" << endl;
    }
    prar.writeField(f_DataSelect, DataSelect, with_comments, brief_mode );

    if(with_comments )
    {
        ff << "\n\n#########################################################################" << endl;
        ff << "#>>>>>>>>>>>>>>>>>>>>>>> DataTarget section >>>>>>>>>>>>>>>>>>>>>>>>>>>>#" << endl;
        ff << "#########################################################################" << endl;
    }
    prar.writeField(f_DataTarget, DataTarget, with_comments, brief_mode );
    prar.writeField(f_LimitOfDetection, (double)LimitOfDetection, with_comments, brief_mode  );

    if(with_comments )
    {
        ff << "\n\n#########################################################################" << endl;
        ff << "#>>>>>>>>>>>>>>>>>>>>>> ParameterMarkup section >>>>>>>>>>>>>>>>>>>>>>>>#" << endl;
        ff << "#########################################################################" << endl;
    }
    ff.close();
}

// get database connection parameters
void Data_Manager::get_db_specs_txt( )
{
    // open file for reading
    string inputstr, result;
    string fname = gpf->OptParamFile();
    fstream ff(fname.c_str(), ios::in );
    ErrorIf( !ff.good() , fname, "OptParamFile Fileopen error");

    TReadArrays  rdar(8, Data_Manager_fields, ff);

    long int nfild = rdar.findNextNotAll();
    while( nfild >=0 )
        {
          switch( nfild )
          {
            case f_MPI: rdar.readArray( "MPI",  &MPI, 1 );
                    break;
            case f_DataDB: rdar.readArray( "DataDB",  DBname );
                    break;
            case f_DataCollection: rdar.readArray( "DataCollection",  collection );
                    break;
            case f_DataSource: rdar.readArray( "DataSource",  &datasource, 1);
                    break;
            case f_LimitOfDetection: rdar.readArray( "LimitOfDetection",  &LimitOfDetection, 1);
                    break;
            case f_DataSelect: rdar.readArray( "DataSelect",  DataSelect );
                    break;
            case f_DataTarget: rdar.readArray( "DataTarget",  DataTarget );
                    break;
            case f_SystemFiles: rdar.readArray( "SystemFiles",  inputstr );
                    remove_copy(inputstr.begin(), inputstr.end(), std::back_inserter(result), '\'');
                    inputstr = result;
                    result.clear();
                    gpf->setGEMS3LstFilePath(inputstr.c_str() );
                    break;
 //           case f_RecipeFiles: rdar.readArray( "RecipeFiles",  inputstr );
 //                   gpf->setGEMS3RecipeFilePath(inputstr.c_str());
 //                   break;
          }
          nfild = rdar.findNextNotAll();
        }

    // define data that must be read
    if (datasource == 0)
    {
        rdar.setAlws( f_MPI );
        rdar.setAlws( f_DataDB );
        rdar.setAlws( f_DataCollection );
        rdar.setAlws( f_DataSelect );
        rdar.setAlws( f_LimitOfDetection );
    }

    // testing read
        string ret = rdar.testRead();
        if( !ret.empty() )
         { ret += " - fields must be read from OptParamFile structure";
           Error( "Error", ret);
         }

    // removing inverted comma
        remove_copy(DBname.begin(), DBname.end(), std::back_inserter(result), '\'');
        DBname = result;
        result.clear();

        remove_copy(collection.begin(), collection.end(), std::back_inserter(result), '\'');
        collection = result;
}

//-----------------------------------------------------------------------------------------

outField statistics_fields[4] =
{
    { "StatMCruns",  0, 0, 1, "\n# StatMCruns: number of Monte Carlo runs for confidence interval generation"},
    { "StatSensitivity",  0, 0, 1, "\n# StatSensitivity: number of evaluations points per parameter for sensitivity evaluation"},
    { "StatMCbool",  0, 0, 1, "\n# StatMCbool: perform Monte Carlo runs -> yes (1) | no (0)"},
    { "StatPerturbator", 0, 0, 1, "\n# StatPerturbator: used for calculating sensitivities by central diference, see ref [2]"}
};

typedef enum {  /// Field index into outField structure
    f_StatMCruns= 0,
    f_StatSensitivity,
    f_StatMCbool,
    f_StatPerturbator
} statistics_FIELDS;


///// Set up default values for structure
void statistics::default_stat_param()
{
  num_of_MC_runs = 10;
  sensitivity_points = 50;
  MCbool =  0;
  perturbator = 0.0001;
}

///// Writes  structure to  the GEMSFIT configuration file
void statistics::out_stat_param_txt( bool with_comments, bool brief_mode )
{
    string fname = gpf->OptParamFile();
    fstream ff(fname.c_str(), ios::out|ios::app);
    ErrorIf( !ff.good() , fname.c_str(), "OptParamFile text open error");

    TPrintArrays  prar(4, statistics_fields, ff);
    prar.writeField(f_StatMCruns, (long int)num_of_MC_runs, with_comments, brief_mode  );
    prar.writeField(f_StatSensitivity, (long int)sensitivity_points, with_comments, brief_mode  );
    prar.writeField(f_StatMCbool, (long int)MCbool, with_comments, brief_mode  );
    prar.writeField(f_StatPerturbator, (double)perturbator, with_comments, brief_mode  );

    if(with_comments )
    {
        ff << endl << endl;
        ff << "#########################################################################" << endl;
        ff << "#>>>>>>>>>> end of GEMSFIT2 input specification file template >>>>>>>>>>#" << endl;
        ff << "#########################################################################" << endl;
    }

    ff.close();
}

//// Read statistical input specifications from configurator
void statistics::get_stat_param_txt( )
{
    // open file for reading
    string fname = gpf->OptParamFile();
    fstream ff(fname.c_str(), ios::in );
    ErrorIf( !ff.good() , fname, "OptParamFile Fileopen error");

    TReadArrays  rdar(4, statistics_fields, ff);

    long int nfild = rdar.findNextNotAll();
    while( nfild >=0 )
        {
          switch( nfild )
          {
           case f_StatMCruns: rdar.readArray( "StatMCruns",  &num_of_MC_runs, 1 );
                   break;
           case f_StatSensitivity: rdar.readArray( "StatSensitivity",  &sensitivity_points, 1 );
                   break;
           case f_StatMCbool:
                   { int bb;
                    rdar.readArray( "StatMCbool",  &bb, 1 );
                    MCbool = (bool)bb;
                   }
                   break;
           case f_StatPerturbator: rdar.readArray( "StatPerturbator",  &perturbator, 1 );
                  break;
          }
          nfild = rdar.findNextNotAll();
        }

    // testing read
        string ret = rdar.testRead();
        if( !ret.empty() )
         { ret += " - fields must be read from OptParamFile structure";
           Error( "Error", ret);
         }
}


////-------------------------------------------------------------------------------------------------

outField optimization_fields[25] =
{
    { "OptAlgo",  0, 0, 1, "\n# OptAlgo: specify algorithm: GN_ISRES | GN_ORIG_DIRECT | GN_ORIG_DIRECT_L | LN_COBYLA | LN_BOBYQA "},
    { "OptThreads",  0, 0, 1, "\n# OptThreads: Comment"},
    { "OptUpBounds",  0, 0, 1, "\n# OptUpBounds: Comment"},
    { "OptLoBounds",  0, 0, 1, "\n# OptLoBounds: Comment"},
    { "OptTolRel",  0, 0, 1, "\n# OptTolRel: stopping criterion -> specify relative tolerance (default = 1e-04) of function value"},
    { "OptMaxEval",  0, 0, 1, "\n# OptMaxEval: specify max number of evaluations"},
    { "OptUpConstraints",  0, 0, 1, "\n# OptUpConstraints: specify parameter vectors for constraint function"},
    { "OptLoConstraints",  0, 0, 1, "\n# OptLoConstraints: Comment"},
    { "OptConstraints",  0, 0, 1, "\n# OptConstraints:  Optimization: apply constraints (1=yes, 0=no)"},
    { "OptDoWhat",  0, 0, 1, "\n# OptDoWhat: perform optimization and statistics (0); only optimization with basic Statistics (1);"
                             "\n#            only Statistics (2) with initial guesses as best fit parametters"},
    { "OptTitration",  0, 0, 1, "\n# OptTitration: Adjusts the computed pH by changing NaOH or HCl amount to match the measured pH"
                                "\n#               read from the database for each experiment"},
    { "OptTuckey",  0, 0, 1, "\n# OptTuckey: 0 -> Use Tuckey Biweight. Value > 0 will be the number of iterations for re-weighting. "},
    { "OptEqSolv",  0, 0, 1, "\n# OptEqSolv: Comment"},
    { "OptTolAbs",  0, 0, 1, "\n# OptTolAbs: stopping criterion -> specify absolute tolerance (default = 1e-04) of function value"},
    { "OptHybridTolRel",  0, 0, 1, "\n# OptHybridTolRel: Comment"},
    { "OptHybridTolAbs",  0, 0, 1, "\n# OptHybridTolAbs: Comment"},
    { "OptHybridMaxEval",  0, 0, 1, "\n# OptHybridMaxEval: Comment"},
    { "OptHybridMode",  0, 0, 1, "\n# OptHybridMode: Comment"},
    { "OptNmultistart",  0, 0, 1, "\n# OptNmultistart: Comment"},
    { "OptPerturbator",  0, 0, 1, "\n# OptPerturbator: The delta/difference used to to calculate the d(function_value)/d(parameter_value) gradient"},
    { "OptInitStep",  0, 0, 1, "\n# OptInitStep: specify initial stepsize for local minimizers \n"
                               "\n#              (factor will be multiplied to all optimization parameters); 0 => use default"},
    { "OptScaleParam",  0, 0, 1, "\n# OptScaleParam: Comment"},
    { "OptNormParam",  0, 0, 1, "\n# OptNormParam: Normalize bounds/constraints/fitting parameters with the initial guess vector"},
    { "OptBoundPerc",  0, 0, 1, "\n# OptBoundPerc: Generate bounds from initial guess vector: specify percentage deviation\n"
                                "\n#               (user-specific, user-defined bounds when set to -1)"}
};

typedef enum {  /// Field index into outField structure
    f_OptAlgo= 0,
    f_OptThreads,
    f_OptUpBounds,
    f_OptLoBounds,
    f_OptTolRel,
    f_OptMaxEval,
    f_OptUpConstraints,
    f_OptLoConstraints,
    f_OptConstraints,
    f_OptDoWhat,
    f_OptTitration,
    f_OptTuckey,
    f_OptEqSolv,
    f_OptTolAbs,
    f_OptHybridTolRel,
    f_OptHybridTolAbs,
    f_OptHybridMaxEval,
    f_OptHybridMode,
    f_OptNmultistart,
    f_OptPerturbator,
    f_OptInitStep,
    f_OptScaleParam,
    f_OptNormParam,
    f_OptBoundPerc

} optimization_FIELDS;


///// Set up default values for structure
void optimization::define_nlopt_param( )
{
    OptAlgo = "LN_BOBYQA";
    OptBoundPerc = -1.0;
    OptTolRel = 1e-6;
    OptTolAbs = 1e-6;
    OptMaxEval = 500000;
    OptDoWhat = 0;
    OptTuckey = 0;
    OptTitration = 0;
    OptNormParam = 1;
    OptPerturbator = 0.0001;
    OptInitStep = 0;

}

///// Writes  structure to  the GEMSFIT configuration file
void optimization::out_nlopt_param_txt( bool with_comments, bool brief_mode )
{
    string fname = gpf->OptParamFile();
    fstream ff(fname.c_str(), ios::out|ios::app );
    ErrorIf( !ff.good() , fname.c_str(), "OptParamFile text open error");

    vector<double> OptUpConstraints_;
    vector<double> OptLoConstraints_;
// cout << "point 1" << endl;
    TPrintArrays  prar(24, optimization_fields, ff);
// cout << "point 2" << endl;
    if(with_comments )
    {
        ff << "\n\n#########################################################################" << endl;
        ff << "#>>>>>>>>>>>>>>>> OptimizationMethods section >>>>>>>>>>>>>>>>>>>>>>>>>>#" << endl;
        ff << "#########################################################################" << endl;
    }

    prar.writeField( f_OptDoWhat,  (long int)OptDoWhat, with_comments, brief_mode);
    prar.writeField( f_OptTuckey,  (long int)OptTuckey, with_comments, brief_mode);
    prar.writeField( f_OptTitration,  (long int)OptTitration, with_comments, brief_mode);
    prar.writeField( f_OptAlgo,  OptAlgo, with_comments, brief_mode );
    prar.writeField( f_OptBoundPerc,  OptBoundPerc, with_comments, brief_mode );
    prar.writeField( f_OptTolRel,  OptTolRel, with_comments, brief_mode);
    prar.writeField( f_OptTolAbs,  OptTolAbs, with_comments, brief_mode);
    prar.writeField( f_OptMaxEval,  (long int)OptMaxEval, with_comments, brief_mode);
    prar.writeField( f_OptInitStep,  OptInitStep, with_comments, brief_mode);
    prar.writeField( f_OptNormParam,  (long int)OptNormParam, with_comments, brief_mode);
    prar.writeField( f_OptPerturbator,  OptPerturbator, with_comments, brief_mode);

    ff.close();
}

//// populate nlopt instance: set bounds, constraints, stopping criteria
void optimization::get_nlopt_param_txt(vector<double> optv)
{
    // open file for reading
//    int i;
    string fname = gpf->OptParamFile();
    fstream ff(fname.c_str(), ios::in );
    ErrorIf( !ff.good() , fname, "OptParamFile Fileopen error");

    TReadArrays  rdar(24, optimization_fields, ff);

    long int nfild = rdar.findNextNotAll();
    while( nfild >=0 )
        {
          switch( nfild )
          {
          case f_OptAlgo: rdar.readArray( "OptAlgo",  OptAlgo );
                   break;
          case f_OptBoundPerc: rdar.readArray( "OptBoundPerc",  &OptBoundPerc, 1 );
                  if( OptBoundPerc < 0.) OptBoundPerc = -1;
                 break;
          case f_OptTolRel: rdar.readArray( "OptTolRel",  &OptTolRel, 1);
                   break;
          case f_OptTolAbs: rdar.readArray( "OptTolAbs",  &OptTolAbs, 1);
                  break;
          case f_OptMaxEval: rdar.readArray( "OptMaxEval",  &OptMaxEval, 1);
                  break;
          case f_OptDoWhat: rdar.readArray( "OptDoWhat",  &OptDoWhat, 1);
                  break;
          case f_OptTuckey: rdar.readArray( "OptTuckey",  &OptTuckey, 1);
                  break;
          case f_OptTitration: rdar.readArray( "OptTitration",  &OptTitration, 1);
                  break;
          case f_OptInitStep: rdar.readArray( "OptInitStep",  &OptInitStep, 1);
                  break;
          case f_OptPerturbator: rdar.readArray( "OptPerturbator",  &OptPerturbator, 1);
                  break;
          case f_OptNormParam:{
                int bb;
                rdar.readArray( "OptNormParam",  &bb, 1);
                OptNormParam = bb;
                 }
                  break;
          }
          nfild = rdar.findNextNotAll();
        }

    if( OptBoundPerc > 0. )
    {
        OptUpBounds.resize( optv.size() );
        OptLoBounds.resize( optv.size() );
        for(unsigned int i=0; i<optv.size(); i++ )
        {
            OptUpBounds[i] = optv[i] + fabs( optv[i]*OptBoundPerc/100. );
            OptLoBounds[i] = optv[i] - fabs( optv[i]*OptBoundPerc/100. );
        }
    }

        // testing read
        string ret = rdar.testRead();
        if( !ret.empty() )
         { ret += " - fields must be read from OptParamFile structure";
           Error( "Error", ret);
         }
}

////-------------------------------------------------------------------------------------------------


void F_to_OP (opti_vector *op, IOJFormat Jformat, string nfild)
{
    vector<string> out;
    Data_Manager *temp = new Data_Manager(1);
    temp->parse_JSON_object(Jformat.format, keys::IV, out);

    op->opt.push_back( atof(out.at(0).c_str()) );
    op->optv0.push_back( atof(out.at(0).c_str()) );
    out.clear();

    temp->parse_JSON_object(Jformat.format, keys::UB, out);
    op->UB.push_back( atof(out.at(0).c_str()) );
    out.clear();

    temp->parse_JSON_object(Jformat.format, keys::LB, out);
    op->LB.push_back( atof(out.at(0).c_str()) );
    out.clear();

    op->Ptype.push_back( nfild );
    op->Pindex.push_back( Jformat.index );
}

void F_to_OP (double val, opti_vector *op, IOJFormat Jformat, string nfild)
{
    op->opt.push_back(val);
    op->optv0.push_back(val);
    if (val > 0)
    {
        op->LB.push_back(val-val*keys::bperc/100);
        op->UB.push_back(val+val*keys::bperc/100);
    } else
    {
        op->UB.push_back(val-val*keys::bperc/100);
        op->LB.push_back(val+val*keys::bperc/100);
    }

    op->Ptype.push_back( nfild );
    op->Pindex.push_back( Jformat.index );
}

void R_to_OP (opti_vector::RDc *r, IOJFormat Jformat /* , string nfild */ )
{
    vector<string> out;
    Data_Manager *temp = new Data_Manager(1);

    temp->parse_JSON_object(Jformat.format, keys::IV, out);
    r->IV = atof(out.at(0).c_str());
    out.clear();

    temp->parse_JSON_object(Jformat.format, keys::Ref, out);
    r->Ref = out.at(0);
    out.clear();

//    temp->parse_JSON_object(Jformat.format, keys::logK, out);
//    r->logK = atof(out.at(0).c_str());
//    out.clear();

    temp->parse_JSON_object(Jformat.format, keys::nC, out);
    r->nC = atoi(out.at(0).c_str());
    out.clear();

    temp->parse_JSON_object(Jformat.format, keys::rDC, out);
    r->Dc_name = out.at(0);
    out.clear();

    temp->parse_JSON_object(Jformat.format, keys::RC, out);
    for (unsigned int i = 0 ; i < out.size() ; i++)
    {
        r->rdc_species.push_back( out.at(i) );
    }
    out.clear();

    temp->parse_JSON_object(Jformat.format, keys::Rcoef, out);
    if (out.size() != r->rdc_species.size())
    {
        cout << "ERROR: Number of reaction components is not equal with the number of reaction coeficients" << endl;
        exit(1);
    }
    for (unsigned int i = 0 ; i < out.size() ; i++)
    {
        r->rdc_species_coef.push_back( atof(out.at(i).c_str()) );
    }

    out.clear();
}


void L_to_OP (opti_vector::Lp *l, IOJFormat Jformat, string nfild)
{
    vector<string> out;
    Data_Manager *temp = new Data_Manager(1);

    l->type = nfild ;
    l->index = Jformat.index ;

    temp->parse_JSON_object(Jformat.format, keys::IV, out);
    l->IV = atof(out.at(0).c_str());
    out.clear();

    temp->parse_JSON_object(Jformat.format, keys::LE, out);
    l->name = out.at(0);
    out.clear();

    temp->parse_JSON_object(Jformat.format, keys::LEs, out);
    for (unsigned int i = 0 ; i < out.size() ; i++)
    {
        l->L_param.push_back( out.at(i) );
    }
    out.clear();

    temp->parse_JSON_object(Jformat.format, keys::Lcoef, out);
    if (out.size() != l->L_param.size())
    {
        cout << "ERROR: Number og linked parameters is not equal with the number of link coeficients" << endl;
        exit(1);
    }
    for (unsigned int i = 0 ; i < out.size() ; i++)
    {
        l->L_coef.push_back( atof(out.at(i).c_str()) );
    }

    out.clear();

}

// ----------- End of  visor.cpp ----------------------------
