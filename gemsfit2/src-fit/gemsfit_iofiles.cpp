/*
*	 Copyright (C) 2013 by Dmytriyeva S. (gems2.support@psi.ch)
*
*	 This file is part of the thermodynamic fitting program GEMSFIT.
*
*    GEMSFIT is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    GEMSFIT is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU  General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with GEMSFIT.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
using namespace std;
#include <cstdlib>

#include "gemsfit_iofiles.h"
#include "v_user.h"
#include "io_arrays.h"
#include "data_manager.h"
#include "keywords.h"

//#include "fit_statistics.h"
//#include "optimization.h"
//#include "plot_class.h"
//using namespace opti;

// subfolder and file names default
const char *INPUT_DIR = "input/";
const char *OUTPUT_DIR = "output/";
const char *RESULT_DIR = "results/";
const char *OPT_PARAM_FILE = "gemsfit_input.dat";
const char *FIT_CSV_FILE = "FIT.csv";
const char *FIT_STATISTIC = "MyFitStatistics.txt";
const char *FIT_LOGFILE = "log_gemsfit.log";

//void out_gems_fit_txt( TNode* node, bool _comment, bool brief_mode );
//void get_gems_fit_multi_txt( TNode* node );
//void get_gems_fit_DCH_txt( TNode* node );
//void get_gems_fit_DBR_txt( TNode* node );
void F_to_OP (opti_vector *op, IOJFormat Jformat, string nfild);
void F_to_OP (double val, opti_vector *op, IOJFormat Jformat, string nfild);
void R_to_OP (opti_vector::RDc *r, IOJFormat Jformat, string nfild);

//StdStateProp::StdStateProp()
//{
//    constraint_data = new my_constraint_data;
//    define_nlopt_param();
//}

//// Constructor
//PlotFit::PlotFit(int)
//{
//    // allocate dynamic memory for my_plotting_info struct
//    plotting_info = new my_plotting_info;
//    // Populate member variables
//    define_plotfit_vars();
//}

//// Constructor
//StdStatistics::StdStatistics():PlotFit(1)
//{
//    number_of_measurements = 0;
//    sum_of_squares 		= 0;
//    num_of_runs		= 0;
//    number_of_parameters   = 0;
//    define_plotfit_vars();
//}

// Constructor
Data_Manager::Data_Manager( int )
{
    define_db_specs();
}

/// Mode GEMSFIT to generate input configuration file
int generateConfig()
{
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

    // Writting to the data
    cout << "Start writting"<< endl;
    Data_Manager *data_meas = new Data_Manager(1);
    data_meas->out_db_specs_txt(with_comments, brief_mode);

    out_gems_fit_txt( node, with_comments, brief_mode );


//    // Create instance of StdStateProp class derived from base class Optimization
//    StdStateProp *gibbs = new StdStateProp();
//    gibbs->out_nlopt_param_txt(with_comments, brief_mode);

//    StdStatistics *stat= new StdStatistics();
//    stat->out_stat_param_txt(with_comments, brief_mode);
//    stat->out_plotfit_vars_txt(with_comments, brief_mode);

    /* test reading
    cout << "Start testing"<< endl;
    data_meas->get_db_specs_txt();
    gibbs->set_nlopt_param_txt();
    stat->std_get_stat_param_txt();
    stat->set_plotfit_vars_txt();
    get_gems_fit_multi_txt( node ); */


//    get_gems_fit_DCH_txt( node );
//    get_gems_fit_DBR_txt( node );
    cout << "Finish " << endl;

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
        ff << "\n\n#########################################################################" << endl;
        ff << "#>>>>>>>>>>>>>>> Parameters to Fit section >>>>>>>>>>>>>>>>>>>>>>>>>>>>>#" << endl;
        ff << "#########################################################################" << endl;
    }

    TPrintArrays  prarCH(30, DataCH_dynamic_fields, ff);

    if( _comment )
        ff << "\n# ICNL: List of Independent Component names (<=4 characters per name) [nIC]";
    prarCH.writeArray(  "ICNL", CSD->ICNL[0], CSD->nIC, MaxICN );
    if( _comment )
            ff << "\n# DCNL: Name list of Dependent Components (<=16 characters per name) [nDC]";
    prarCH.writeArray(  "DCNL", CSD->DCNL[0], CSD->nDC, MaxDCN );
    if( _comment )
            ff << "\n# PHNL: List of Phase names (<=16 characters per name) [nPH]";
    prarCH.writeArray(  "PHNL", CSD->PHNL[0], CSD->nPH, MaxPHN );
    prarCH.writeArrayF(  f_ccPH, CSD->ccPH, CSD->nPH, 1L,_comment, brief_mode );
    prarCH.writeArray(  f_nDCinPH, CSD->nDCinPH, CSD->nPH, -1L,_comment, brief_mode);
    prarCH.writeArray(  f_TKval, CSD->TKval, CSD->nTp, -1L,_comment, brief_mode );
    prarCH.writeArray(  f_Pval, CSD->Pval, CSD->nPp,  -1L,_comment, brief_mode );
    vector <double> xG0;
    ff << "\n \n# G0: Look-up array for DC molar Gibbs energy function g(T,P), J/mol at 298.15K and 1 bar \n";
    ff << "<G0>" << endl;
    for (int i=0; i<CSD->nDC*node->gridTP(); ++i)
    {
        xG0.push_back(CSD->G0[i]);
        ff << CSD->G0[i] << endl;
        i += node->gridTP()-1;
    }

    if(_comment )
    {
        ff << "\n\n#########################################################################" << endl;
        ff << "#>>>>>>>>>> Input for fitting non-ideality parametres >>>>>>>>>>>>>>>>>>#" << endl;
        ff << "#########################################################################" << endl;
    }
    TPrintArrays  prarIPM( 70, MULTI_dynamic_fields, ff);
    MULTI* pmp = node->pMulti()->GetPM();

    if( pmp->FIs > 0 && pmp->Ls > 0 )
    {
      if( _comment )
         ff << "\n\n## (4) Initial data for multicomponent phases (see DCH file for dimension nPHs)";
      prarIPM.writeArrayF(  f_sMod, pmp->sMod[0], pmp->FIs, 6L, _comment, brief_mode );

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
            ff << "\n\n# PMc: Tables (in TSolMod convention) of interaction parameter coefficients  for non-ideal solutions";
        prarIPM.writeArray(  "PMc", pmp->PMc,  LsModSum);
       }

       prarIPM.writeArray(  f_LsMdc, pmp->LsMdc, pmp->FIs*3, 3L, _comment, brief_mode);
       if(LsMdcSum )
       {   if( _comment )
              ff << "\n\n# DMc: Tables (in TSolMod convention) of  parameter coefficients for dependent components";
        prarIPM.writeArray(  "DMc", pmp->DMc,  LsMdcSum);
       }
       if(LsMsnSum )
       {   if( _comment )
              ff << "\n\n# MoiSN:  end member moiety / site multiplicity number tables (in TSolMod convention) ";
        prarIPM.writeArray(  "MoiSN", pmp->MoiSN,  LsMsnSum);
       }
    } // sMod
    prarIPM.writeArray(  f_fDQF, pmp->fDQF,  pmp->L, -1L, _comment, brief_mode);


    if( _comment )
       ff << "\n\n# This part is taken from *DBR.dat\n";

    // from *DBR.dat
    TPrintArrays  prar(f_bSP+1/*52*/, DataBR_fields, ff);

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

    if(_comment )
    {
        ff << "\n\n#########################################################################" << endl;
        ff << "#>>>>>>>>>>>>>>> Data to compare section >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>#" << endl;
        ff << "#########################################################################" << endl;
    }

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
}

/// Reading part MULTI structure from OptParamFile text file
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
                        rddar.readArray( "sMod" , pmp->sMod[0], pmp->FIs, 6 );
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
              // Hear you must write your code
              for( int ii=0; ii<vFormats.size(); ii++ )
              {    cout<< "Fied " << MULTI_dynamic_fields[nfild].name << " Type " << vFormats[ii].type <<
                          " Index " << vFormats[ii].index << endl;
                  cout<< vFormats[ii].format << endl;

                  if ((vFormats[ii].type == ft_F))
                  {
                      if (vFormats[ii].format.size() >1)
                      {
                          F_to_OP(op, vFormats[ii], MULTI_dynamic_fields[nfild].name );
                      } else
                      {
                          switch( nfild )
                          {
                          case f_sMod:
                                       break;
                          case f_LsMod:{ if( !pmp->LsMod )
                                          Error( "Error", "Array LsMod is not used in this problem");
//                                        rddar.readArray( "LsMod" , pmp->LsMod, pmp->FIs*3) ;
                                        long int LsModSum;
                                        long int LsIPxSum;
                                        node->pMulti()->getLsModsum( LsModSum, LsIPxSum );
                                        if(LsIPxSum )
                                        {
                                        }
                                        if(LsModSum )
                                            F_to_OP(pmp->PMc[vFormats[ii].index], op, vFormats[ii], MULTI_dynamic_fields[nfild].name );
                                        break;
                                       }
                            case f_LsMdc:
                                        break;
                            case f_fDQF:
                                        break;
                          }

                      }
                  }
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
        case f_ccPH: rddar.readArray( "ccPH", CSD->ccPH, CSD->nPH, 1 );
                break;
        case f_nDCinPH: rddar.readArray( "nDCinPH", CSD->nDCinPH, CSD->nPH);
                break;
        case f_TKval: rddar.readArray( "TKval", CSD->TKval, CSD->nTp );
                break;
        case f_Pval: rddar.readArray( "Pval", CSD->Pval, CSD->nPp );
                  break;
        case f_G0: rddar.readFormatArray( "G0", CSD->G0, CSD->nDC*node->gridTP(), vFormats );
                  break;
         }

        if( !vFormats.empty() )
        {
            int nr = 0;
            // Hear you must write your code
            for( int ii=0; ii<vFormats.size(); ii++ )
            {
                cout<< "Field " << DataCH_dynamic_fields[nfild].name << " Type " << vFormats[ii].type <<
                        " Index " << vFormats[ii].index << endl;
                cout<< vFormats[ii].format << endl;

                if ((vFormats[ii].type == ft_F))
                {
                    if (vFormats[ii].format.size() >1)
                    {
                        F_to_OP(op, vFormats[ii], DataCH_dynamic_fields[nfild].name );
                    } else
                    {
                        switch( nfild )
                        {
                        case f_G0: F_to_OP(CSD->G0[vFormats[ii].index], op, vFormats[ii], DataCH_dynamic_fields[nfild].name );
                                  break;
                         }
                    }
                }
                if (vFormats[ii].type == ft_R)
                {
                    op->reactions.push_back(new opti_vector::RDc);
                    op->h_RDc = true;
                    R_to_OP(op->reactions[nr], vFormats[ii], DataCH_dynamic_fields[nfild].name );
                    nr++;
                }
            }
            vFormats.clear();
        }

        nfild = rddar.findNextNotAll();

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
            // Hear you must write your code
            for( int ii=0; ii<vFormats.size(); ii++ )
            {    cout<< "Fied " << DataBR_fields[nfild].name << " Type " << vFormats[ii].type <<
                        " Index " << vFormats[ii].index << endl;
                cout<< vFormats[ii].format << endl;

                if ((vFormats[ii].type == ft_F) && (vFormats[ii].format.size() >1))
                {
                    F_to_OP(op, vFormats[ii], DataBR_fields[nfild].name );
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

TGfitPath::TGfitPath(int c, char *v[]):
        argc(c), argv(v)
{
    optParamFilePath = "";
    gems3LstFilePath = "";

//#ifdef __unix
//        optParamFilePath = getenv("HOME");
//#else
        optParamFilePath = ".";
//#endif

    // parsing options -init, -run, -conf if given

    int iinit = 0;		// index of -init option
    int irun = 0;		// index of -run option
    int iconf = 0;		// index of -conf option

    for (int ii = 1; ii < argc; ii++)
    {
        if (strcmp(argv[ii], "-init") == 0 )
          iinit = ii;
        else if (strcmp(argv[ii], "-run") == 0 )
            irun = ii;
                 else if (strcmp(argv[ii], "-conf") == 0)
                     iconf = ii;
    }


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
        gems3LstFilePath = argv[iinit + 1];
        mode = INIT_;
    }

    if (iconf != 0)
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
    path += "/";

    // set up default pathes
    // later this pathes and filenames can be read from config file
    inputDir = path + INPUT_DIR;
    outputDir = path + OUTPUT_DIR;
    resultDir = path + RESULT_DIR;

    fitFile = resultDir+FIT_CSV_FILE;
    fitStatistics = outputDir+FIT_STATISTIC;
    fitLogFile = outputDir+FIT_LOGFILE;

    cout << "optParamFile = " << optParamFile << endl;
    cout << "fitFile = " << fitFile << endl;
    cout << "fitLogFile = " << fitLogFile << endl;
    cout << "gems3LstFilePath = " << gems3LstFilePath << endl;
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

outField Data_Manager_fields[10] =
{
    { "DatDB",  0, 0, 1, "\n# DatDB: database path"},
    { "DatColection",  0, 0, 1, "\n# DatColection: database colection name"},
//    { "DatUsername",  0, 0, 1, "\n# DatUsername: Comment"},
//    { "DatPasswd",  0, 0, 1, "\n# DatPasswd: Comment"},
    { "DatSource",  1, 0, 1, "\n# DatSource: get measurement data from EJDB databse (default) (0). No other sources implemented in GEMSFIT2"},
//    { "DatCSVfile",  0, 0, 1, "\n# DatCSVfile: Comment"},
//    { "DatServer",  0, 0, 1, "\n# DatServer: Comment"},
    { "DatSelect", 0, 0, 1, "\n# DataSelect: query for selecting data form EJDB database in JSON format. Names of samples or expdatasets and temperature and pressure intervals. all - select all data"
      "\n { \"sample\" : \"\", "
      "\n \"expdataset\" : [\"CH04\", \"CH04D\"],"
      "\n \"sT\" : [100, 1000],"
      "\n \"sP\" : [1, 2500] \n# }"},
    { "SystemFiles",  0, 0, 1, "\n# SystemFiles: Comment"},
    { "RecipeFiles",  0, 0, 1, "\n# RecipeFiles: Comment"}
};

typedef enum {  /// Field index into outField structure
    f_DatDB = 0,
    f_DatColection,
//    f_DatUsername,
//    f_DatPasswd,
    f_DatSource,
//    f_DatCSVfile,
//    f_DatServer,
    f_DatSelect,
    f_SystemFiles,
    f_RecipeFiles
} Data_Manager_FIELDS;

/// Set up default values for structure
void Data_Manager::define_db_specs( )
{
   datasource = 0;
//   CSVfile = "DatCSVfile";
   DBname ="";
   colection="";
//   username="";
//   passwd="";
//   psql_server="";
   DataSelect ="all";

}

/// Writes  structure to  the GEMSFIT configuration file
void Data_Manager::out_db_specs_txt( bool with_comments, bool brief_mode )
{
    string fname = gpf->OptParamFile();
    fstream ff(fname.c_str(), ios::out/*|ios::app*/ );
    ErrorIf( !ff.good() , fname.c_str(), "OptParamFile text open error");

    TPrintArrays  prar(10, Data_Manager_fields, ff);
    // define data must be written

    if (datasource == 0)
    {
        prar.setAlws( f_DatDB );
        prar.setAlws( f_DatColection );
        prar.setAlws( f_DatSelect );
    }
//    else if( datasource == 1 )
//    {
//       prar.setAlws( f_DatCSVfile );
//    }
//    else if( datasource == 2 )
//    {
//        prar.setAlws( f_DatDB );
//        prar.setAlws( f_DatTable );
//        prar.setAlws( f_DatUsername );
//        prar.setAlws( f_DatPasswd );
//        prar.setAlws( f_DatServer );
//    }

    if(with_comments )
    {
        ff << "\n\n#########################################################################" << endl;
        ff << "#>>>>>>>>>>>>>>> Data sources section >>>>>>>>>>>>>>>>>>>>>>>>>>>>>#" << endl;
        ff << "#########################################################################" << endl;
    }

    prar.writeField(f_DatSource, (long int)datasource, with_comments, brief_mode  );
//    prar.writeField(f_DatCSVfile, CSVfile, with_comments, brief_mode  );
    prar.writeField(f_DatDB, DBname, with_comments, brief_mode  );
    prar.writeField(f_DatColection, colection, with_comments, brief_mode  );
//    prar.writeField(f_DatUsername, username, with_comments, brief_mode  );
//    prar.writeField(f_DatPasswd, passwd, with_comments, brief_mode  );
//    prar.writeField(f_DatServer, psql_server, with_comments, brief_mode  );
    prar.writeField(f_DatSelect, DataSelect, with_comments, brief_mode );
    prar.writeField(f_SystemFiles, gpf->GEMS3LstFilePath(), with_comments, brief_mode  );
    prar.writeField(f_RecipeFiles, "", with_comments, brief_mode  );
}

// get PostgreSQL database connection parameters
void Data_Manager::get_db_specs_txt( )
{
    // open file for reading
    string inputstr;
    string fname = gpf->OptParamFile();
    fstream ff(fname.c_str(), ios::in );
    ErrorIf( !ff.good() , fname, "OptParamFile Fileopen error");

    TReadArrays  rdar(10, Data_Manager_fields, ff);

    long int nfild = rdar.findNextNotAll();
    while( nfild >=0 )
        {
          switch( nfild )
          {
           case f_DatDB: rdar.readArray( "DatDB",  DBname );
                   break;
           case f_DatColection: rdar.readArray( "DatColection",  colection );
                   break;
//           case f_DatUsername: rdar.readArray( "DatUsername",  username);
//                   break;
//           case f_DatPasswd: rdar.readArray( "DatPasswd",  passwd );
//                   break;
           case f_DatSource: rdar.readArray( "DatSource",  &datasource, 1);
                   break;
//           case f_DatCSVfile: rdar.readArray( "DatCSVfile",  CSVfile);
//              break;
//           case f_DatServer: rdar.readArray( "DatServer",  psql_server );
              break;
          case f_DatSelect: rdar.readArray( "DatSelect",  DataSelect );
             break;
          case f_SystemFiles: rdar.readArray( "SystemFiles",  inputstr );
              gpf->setGEMS3LstFilePath(inputstr.c_str() );
             break;
          case f_RecipeFiles: rdar.readArray( "RecipeFiles",  inputstr );
              gpf->setGEMS3RecipeFilePath(inputstr.c_str());
             break;
          }
          nfild = rdar.findNextNotAll();
        }

    // define data must be read
    if (datasource == 0)
    {
        rdar.setAlws( f_DatDB );
        rdar.setAlws( f_DatColection );
        rdar.setAlws( f_DatSelect );
    }
//    else if( datasource == 1 )
//    {
//       rdar.setAlws( f_DatCSVfile );
//    }
//    else if( datasource == 2 )
//    {
//        rdar.setAlws( f_DatDB );
//        rdar.setAlws( f_DatTable );
//        rdar.setAlws( f_DatUsername );
//        rdar.setAlws( f_DatPasswd );
//        rdar.setAlws( f_DatServer );
//    }

    // testing read
        string ret = rdar.testRead();
        if( !ret.empty() )
         { ret += " - fields must be read from OptParamFile structure";
           Error( "Error", ret);
         }
}

//-------------------------------------------------------------------------------------------------

//outField StdStatistics_fields[4] =
//{
//    { "StatMCruns",  0, 0, 1, "\n# StatMCruns: Comment"},
//    { "StatSensitivity",  0, 0, 1, "\n# StatSensitivity: Comment"},
//    { "StatMCbars",  0, 0, 1, "\n# StatMCbars: Comment"},
//    { "StatMCbool",  0, 0, 1, "\n# StatMCbool: Comment"}
//};

//typedef enum {  /// Field index into outField structure
//    f_StatMCruns= 0,
//    f_StatSensitivity,
//    f_StatMCbars,
//    f_StatMCbool
//} StdStatistics_FIELDS;


///// Set up default values for structure
//void StdStatistics::default_stat_param()
//{
//  num_of_MC_runs = 10;
//  sensitivity_points = 50;
//  MC_number_of_bars = 10;
//  MCbool =  0;
//}

///// Writes  structure to  the GEMSFIT configuration file
//void StdStatistics::out_stat_param_txt( bool with_comments, bool brief_mode )
//{
//    string fname = gpf->OptParamFile();
//    fstream ff(fname.c_str(), ios::out|ios::app);
//    ErrorIf( !ff.good() , fname.c_str(), "OptParamFile text open error");

//    TPrintArrays  prar(4, StdStatistics_fields, ff);
//    prar.writeField(f_StatMCruns, (long int)num_of_MC_runs, with_comments, brief_mode  );
//    prar.writeField(f_StatSensitivity, (long int)sensitivity_points, with_comments, brief_mode  );
//    prar.writeField(f_StatMCbars, (long int)MC_number_of_bars, with_comments, brief_mode  );
//    prar.writeField(f_StatMCbool, (long int)MCbool, with_comments, brief_mode  );
//}

//// Read statistical input specifications from configurator
//void StdStatistics::std_get_stat_param_txt( )
//{
//    // open file for reading
//    string fname = gpf->OptParamFile();
//    fstream ff(fname.c_str(), ios::in );
//    ErrorIf( !ff.good() , fname, "OptParamFile Fileopen error");

//    TReadArrays  rdar(4, StdStatistics_fields, ff);

//    long int nfild = rdar.findNextNotAll();
//    while( nfild >=0 )
//        {
//          switch( nfild )
//          {
//           case f_StatMCruns: rdar.readArray( "StatMCruns",  &num_of_MC_runs, 1 );
//                   break;
//           case f_StatSensitivity: rdar.readArray( "StatSensitivity",  &sensitivity_points, 1 );
//                   break;
//           case f_StatMCbars: rdar.readArray( "StatMCbars",  &MC_number_of_bars, 1);
//                   break;
//           case f_StatMCbool:
//                   { int bb;
//                    rdar.readArray( "StatMCbool",  &bb, 1 );
//                    MCbool = (bool)bb;
//                   }
//                   break;
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


////-------------------------------------------------------------------------------------------------

//outField StdStateProp_fields[24] =
//{
//    { "OptAlgo",  0, 0, 1, "\n# OptAlgo: Comment"},
//    { "OptThreads",  0, 0, 1, "\n# OptThreads: Comment"},
//    { "OptUpBounds",  0, 0, 1, "\n# OptUpBounds: Comment"},
//    { "OptLoBounds",  0, 0, 1, "\n# OptLoBounds: Comment"},
//    { "OptTolRel",  0, 0, 1, "\n# OptTolRel: Comment"},
//    { "OptMaxEval",  0, 0, 1, "\n# OptMaxEval: Comment"},
//    { "OptUpConstraints",  0, 0, 1, "\n# OptUpConstraints: Comment"},
//    { "OptLoConstraints",  0, 0, 1, "\n# OptLoConstraints: Comment"},
//    { "OptConstraints",  0, 0, 1, "\n# OptConstraints: Comment"},
//    { "OptDoWhat",  0, 0, 1, "\n# OptDoWhat: Comment"},
//    { "OptStatOnlyBestFitParam",  0, 0, 1, "\n# OptStatOnlyBestFitParam: Comment"},
//    { "OptStatOnlySSR",  0, 0, 1, "\n# OptStatOnlySSR: Comment"},
//    { "OptEqSolv",  0, 0, 1, "\n# OptEqSolv: Comment"},
//    { "OptTolAbs",  0, 0, 1, "\n# OptTolAbs: Comment"},
//    { "OptHybridTolRel",  0, 0, 1, "\n# OptHybridTolRel: Comment"},
//    { "OptHybridTolAbs",  0, 0, 1, "\n# OptHybridTolAbs: Comment"},
//    { "OptHybridMaxEval",  0, 0, 1, "\n# OptHybridMaxEval: Comment"},
//    { "OptHybridMode",  0, 0, 1, "\n# OptHybridMode: Comment"},
//    { "OptNmultistart",  0, 0, 1, "\n# OptNmultistart: Comment"},
//    { "OptHybridAlgo",  0, 0, 1, "\n# OptHybridAlgo: Comment"},
//    { "OptInitStep",  0, 0, 1, "\n# OptInitStep: Comment"},
//    { "OptScaleParam",  0, 0, 1, "\n# OptScaleParam: Comment"},
//    { "OptNormParam",  0, 0, 1, "\n# OptNormParam: Comment"},
//    { "OptBoundPerc",  0, 0, 1, "\n# OptBoundPerc: Comment"}
//};

//typedef enum {  /// Field index into outField structure
//    f_OptAlgo= 0,
//    f_OptThreads,
//    f_OptUpBounds,
//    f_OptLoBounds,
//    f_OptTolRel,
//    f_OptMaxEval,
//    f_OptUpConstraints,
//    f_OptLoConstraints,
//    f_OptConstraints,
//    f_OptDoWhat,
//    f_OptStatOnlyBestFitParam,
//    f_OptStatOnlySSR,
//    f_OptEqSolv,
//    f_OptTolAbs,
//    f_OptHybridTolRel,
//    f_OptHybridTolAbs,
//    f_OptHybridMaxEval,
//    f_OptHybridMode,
//    f_OptNmultistart,
//    f_OptHybridAlgo,
//    f_OptInitStep,
//    f_OptScaleParam,
//    f_OptNormParam,
//    f_OptBoundPerc

//} StdStateProp_FIELDS;


///// Set up default values for structure
//void StdStateProp::define_nlopt_param( )
//{
//    OptAlgo = "LN_BOBYQA";
//    OptThreads = 1;
//    OptBoundPerc = -0.1;
//    OptUpBounds.push_back(100);
//    OptUpBounds.push_back(100);
//    OptLoBounds.push_back(-10);
//    OptLoBounds.push_back(-10);
//    OptTolRel = 1e-4;
//    OptTolAbs = 1e-4;
//    OptMaxEval = 5000;

//    OptConstraints = 0;
//    //??OptUpConstraints;
//    //??OptLoConstraints;

//     OptDoWhat = 0;
//    //OptStatOnlyBestFitParam;
//    //OptStatOnlySSR;
//    //OptEquil;
//    OptHybridMode = 0;
//    OptHybridAlgo = "LN_COBYLA";
//    OptHybridTolRel = 1e-4;
//    OptHybridTolAbs = 1e-4;
//    OptHybridMaxEval = 5000000;
//    OptNmultistart = 0;
//    OptInitStep = 0;
//    //OptScaleParam;
//    OptNormParam = 1;

//}

///// Writes  structure to  the GEMSFIT configuration file
//void StdStateProp::out_nlopt_param_txt( bool with_comments, bool brief_mode )
//{
//    string fname = gpf->OptParamFile();
//    fstream ff(fname.c_str(), ios::out|ios::app );
//    ErrorIf( !ff.good() , fname.c_str(), "OptParamFile text open error");

//    vector<double> OptUpConstraints_;
//    vector<double> OptLoConstraints_;

//    TPrintArrays  prar(24, StdStateProp_fields, ff);

//    if( OptHybridMode )
//    {
//        prar.setAlws( f_OptHybridTolRel );
//        prar.setAlws( f_OptHybridTolAbs );
//        prar.setAlws( f_OptHybridMaxEval );
//        prar.setAlws( f_OptHybridAlgo );
//    }

//    if( OptConstraints )
//    {

//        // !!!! What constraint up or low ?
//        int i;

//        for( i=0; i< (int) constraint_data_v.size(); i++)
//        {
//            OptUpConstraints_.push_back(constraint_data_v[i].Constraints);
//        };
//        for( i=0; i< (int) constraint_data_v.size(); i++)
//        {
//            OptLoConstraints_.push_back(constraint_data_v[i].Constraints);
//        };
//    }

//    if(with_comments )
//    {
//        ff << "\n\n#########################################################################" << endl;
//        ff << "#>>>>>>>>>>>>>>> Optimization Methods section >>>>>>>>>>>>>>>>>>>>>>>>>>>>>#" << endl;
//        ff << "#########################################################################" << endl;
//    }

//    prar.writeField( f_OptAlgo,  OptAlgo, with_comments, brief_mode );
//    prar.writeField( f_OptThreads,  (long int)OptThreads, with_comments, brief_mode);
//    prar.writeField( f_OptBoundPerc,  OptBoundPerc, with_comments, brief_mode );
//    prar.writeArray( f_OptUpBounds,  OptUpBounds, 2, with_comments, brief_mode);
//    prar.writeArray( f_OptLoBounds,  OptLoBounds, 2, with_comments, brief_mode );
//    prar.writeField( f_OptTolRel,  OptTolRel, with_comments, brief_mode);
//    prar.writeField( f_OptTolAbs,  OptTolAbs, with_comments, brief_mode);
//    prar.writeField( f_OptMaxEval,  (long int)OptMaxEval, with_comments, brief_mode);
//    prar.writeField( f_OptConstraints,  (long int)OptConstraints, with_comments, brief_mode);
//    prar.writeArray( f_OptUpConstraints,  OptUpConstraints_, 2, with_comments, brief_mode);
//    prar.writeArray( f_OptLoConstraints,  OptLoConstraints_, 2, with_comments, brief_mode );
//    prar.writeField( f_OptDoWhat,  (long int)OptDoWhat, with_comments, brief_mode);
//    prar.writeArray( f_OptStatOnlyBestFitParam,  OptStatOnlyBestFitParam, 2, with_comments, brief_mode);
//    prar.writeField( f_OptStatOnlySSR,  OptStatOnlySSR, with_comments, brief_mode);
//    prar.writeField( f_OptEqSolv,  (long int)OptEquil, with_comments, brief_mode);
//    prar.writeField( f_OptHybridMode,  (long int)OptHybridMode, with_comments, brief_mode);
//    prar.writeField( f_OptHybridAlgo,  OptHybridAlgo, with_comments, brief_mode );
//    prar.writeField( f_OptHybridTolRel,  OptHybridTolRel, with_comments, brief_mode);
//    prar.writeField( f_OptHybridTolAbs,  OptHybridTolAbs, with_comments, brief_mode);
//    prar.writeField( f_OptHybridMaxEval,  (long int)OptHybridMaxEval, with_comments, brief_mode);
//    prar.writeField( f_OptNmultistart,  (long int)OptNmultistart, with_comments, brief_mode);
//    prar.writeField( f_OptInitStep,  OptInitStep, with_comments, brief_mode);
//    prar.writeField( f_OptScaleParam,  OptScaleParam, with_comments, brief_mode);
//    prar.writeField( f_OptNormParam,  (long int)OptNormParam, with_comments, brief_mode);

//}

//// populate nlopt instance: set bounds, constraints, stopping criteria
//void StdStateProp::set_nlopt_param_txt( )
//{
//    // open file for reading
//    int i;
//    string fname = gpf->OptParamFile();
//    fstream ff(fname.c_str(), ios::in );
//    ErrorIf( !ff.good() , fname, "OptParamFile Fileopen error");

//    TReadArrays  rdar(24, StdStateProp_fields, ff);
//    vector<double> OptUpConstraints_;
//    vector<double> OptLoConstraints_;

//    long int nfild = rdar.findNextNotAll();
//    while( nfild >=0 )
//        {
//          switch( nfild )
//          {
//          case f_OptAlgo: rdar.readArray( "OptAlgo",  OptAlgo );
//                   break;
//          case f_OptThreads: rdar.readArray( "OptThreads",  &OptThreads, 1);
//                  break;
//          case f_OptBoundPerc: rdar.readArray( "OptBoundPerc",  &OptBoundPerc, 1 );
//                  if( OptBoundPerc < 0.) OptBoundPerc = -1;
//                 break;
//          case f_OptUpBounds: rdar.readArray( "OptUpBounds",  OptUpBounds);
//                   break;
//          case f_OptLoBounds: rdar.readArray( "OptLoBounds",  OptLoBounds );
//                   break;
//          case f_OptTolRel: rdar.readArray( "OptTolRel",  &OptTolRel, 1);
//                   break;
//          case f_OptTolAbs: rdar.readArray( "OptTolAbs",  &OptTolAbs, 1);
//                  break;
//          case f_OptMaxEval: rdar.readArray( "OptMaxEval",  &OptMaxEval, 1);
//                  break;
//          case f_OptConstraints: rdar.readArray( "OptConstraints",  &OptConstraints, 1);
//                  break;
//          case f_OptUpConstraints: rdar.readArray( "OptUpConstraints",  OptUpConstraints_);
//                   break;
//          case f_OptLoConstraints: rdar.readArray( "OptLoConstraints",  OptLoConstraints_ );
//                   break;
//          case f_OptDoWhat: rdar.readArray( "OptDoWhat",  &OptDoWhat, 1);
//                  break;
//          case f_OptStatOnlyBestFitParam: rdar.readArray( "OptStatOnlyBestFitParam",  OptStatOnlyBestFitParam);
//                   break;
//          case f_OptStatOnlySSR: rdar.readArray( "OptStatOnlySSR",  &OptStatOnlySSR, 1);
//                  break;
//          case f_OptEqSolv:
//                  {
//                    int bb;
//                    rdar.readArray( "OptEqSolv",  &bb, 1);
//                     OptEquil = bb;
//                   }  break;
//          case f_OptHybridMode: rdar.readArray( "OptHybridMode",  &OptHybridMode, 1);
//                  break;

//          case f_OptHybridAlgo: rdar.readArray( "OptHybridAlgo",  OptHybridAlgo );
//                   break;
//          case f_OptHybridTolRel: rdar.readArray( "OptHybridTolRel",  &OptHybridTolRel, 1);
//                  break;
//          case f_OptHybridTolAbs: rdar.readArray( "OptHybridTolAbs",  &OptHybridTolAbs, 1);
//                  break;
//          case f_OptHybridMaxEval: rdar.readArray( "OptHybridMaxEval",  &OptHybridMaxEval, 1);
//                  break;
//          case f_OptNmultistart: rdar.readArray( "OptNmultistart",  &OptNmultistart, 1);
//                  break;
//          case f_OptInitStep: rdar.readArray( "OptInitStep",  &OptInitStep, 1);
//                  break;
//          case f_OptScaleParam: rdar.readArray( "OptScaleParam",  &OptScaleParam, 1);
//                  break;
//          case f_OptNormParam:{
//                int bb;
//                rdar.readArray( "OptNormParam",  &bb, 1);
//                OptNormParam = bb;
//                 }
//                  break;
//          }
//          nfild = rdar.findNextNotAll();
//        }

//    if( OptBoundPerc > 0. )
//    {
//        OptUpBounds.resize( opt_vec.size() );
//        OptLoBounds.resize( opt_vec.size() );
//        for(int i=0; i<opt_vec.size(); i++ )
//        {
//            OptUpBounds[i] = opt_vec[i] + fabs( opt_vec[i]*OptBoundPerc/100. );
//            OptLoBounds[i] = opt_vec[i] - fabs( opt_vec[i]*OptBoundPerc/100. );
//        }
//    }

//    // Check if all lower bounds are smaller than the corresponding upper bounds
//    if( OptLoBounds.size() != OptUpBounds.size() )
//    {
//        cout<<endl;
//        cout<<"upper and lower bounds must have same number of elements !! "<<endl;
//            cout<<"exiting now ..."<<endl;
//            cout<<endl;
//        exit(1);
//    }
//    for( i=0; i<OptLoBounds.size(); i++ )
//    {
//        if( OptLoBounds[i]>OptUpBounds[i] )
//        {
//            cout<<endl;
//            cout<<"Inconsistence in bounds: OptLoBounds["<<i<<"] = "<<OptLoBounds[i]<<" has a bigger value than OptUpBounds["<<i<<"] = "<<OptUpBounds[i]<<endl;
//            cout<<" !! exiting now ..."<<endl;
//                cout<<endl;
//            exit(1);
//        }
//    }

//    if( OptConstraints )
//    {
//        my_constraint_data constrdata;
//        int ii = 0;

//        for( i=0; i< (int) OptUpConstraints_.size(); i++)
//        {
//            constrdata.id = i;
//            constrdata.Constraints = OptUpConstraints_[i];
//            constraint_data_v.push_back(constrdata);
//            ii = i;
//        };
//        for( i=0; i< (int) OptLoConstraints_.size(); i++)
//        {
//            constrdata.id = ii + 1 + i;
//            constrdata.Constraints = OptLoConstraints_[i];
//            constraint_data_v.push_back(constrdata);
//        };


//        // Check if all lower constraints are smaller than the corresponding upper constraints
//        if( OptUpConstraints_.size() != OptLoConstraints_.size() )
//        {
//            cout<<endl;
//            cout<<"WARNING: upper and lower constraints do not have same number of elements !!"<<endl;
//            cout<<"OptUpConstraints.size() = "<<OptUpConstraints_.size()<<" <-> OptLoConstraints.size() = "<<OptLoConstraints_.size()<<endl;
//            cout<<"hope that's ok, proceeding now ..."<<endl;
//            cout<<endl;
//        }
//        for( i=0; i<OptLoConstraints_.size(); i++ )
//        {
//            if( OptLoConstraints_[i] > OptUpConstraints_[i] )
//            {
//                cout<<endl;
//                cout<<"Inconsistence in constrains: OptLoConstraints["<<i<<"] = "<<OptLoConstraints_[i]<<" has a bigger value than OptUpConstraints["<<i<<"] = "<<OptUpConstraints_[i]<<endl;
//                cout<<" .... exiting now .... "<<endl;
//                    cout<<endl;
//                exit(1);
//            };
//        };
//    }//enf if ( OptConstraints )

//        if( OptHybridMode )
//        {
//            rdar.setAlws( f_OptHybridTolRel );
//            rdar.setAlws( f_OptHybridTolAbs );
//            rdar.setAlws( f_OptHybridMaxEval );
//            rdar.setAlws( f_OptHybridAlgo );
//        }

//        // testing read
//        string ret = rdar.testRead();
//        if( !ret.empty() )
//         { ret += " - fields must be read from OptParamFile structure";
//           Error( "Error", ret);
//         }
//}

////-------------------------------------------------------------------------------------------------

//outField PlotFit_fields[9] =
//{
//    { "PrintTemperatures",  0, 0, 1, "\n# PrintTemperatures: Comment"},
//    { "PrintPressures",  0, 0, 1, "\n# PrintPressures: Comment"},
//    { "PrintMolalities",  0, 0, 1, "\n# PrintMolalities: Comment"},
//    { "PrintFormat",  0, 0, 1, "\n# PrintFormat: Comment"},
//    { "PrintFilename",  0, 0, 1, "\n# PrintFilename: Comment"},
//    { "PrintMeasValueCode",  0, 0, 1, "\n# PrintMeasValueCode: Comment"},
//    { "PrintLabelXaxis",  0, 0, 1, "\n# PrintLabelXaxis: Comment"},
//    { "PrintLabelYaxis",  0, 0, 1, "\n# PrintLabelYaxis: Comment"},
//    { "PrintHead",  0, 0, 1, "\n# PrintHead: Comment"}
//};

//typedef enum {  /// Field index into outField structure
//    f_PrintTemperatures= 0,
//    f_PrintPressures,
//    f_PrintMolalities,
//    f_PrintFormat,
//    f_PrintFilename,
//    f_PrintMeasValueCode,
//    f_PrintLabelXaxis,
//    f_PrintLabelYaxis,
//    f_PrintHead
//} PlotFit_FIELDS;

///// Set up default values for structure
//void PlotFit::define_plotfit_vars( )
//{
//    plotting_info->print_temperatures.clear();
//    plotting_info->print_temperatures.push_back(298.15);
//    plotting_info->print_pressures.clear();
//    plotting_info->print_pressures.push_back(1);
//    plotting_info->print_molalities.clear();
//    plotting_info->print_molalities.push_back(0.0001);
//    plotting_info->print_molalities.push_back(-4.5);
//    plotting_info->print_format="eps";
//    plotting_info->print_filename="Comp_vs_Meas";
//    plotting_info->print_code=0;
//    plotting_info->print_xlabel="molality";
//    plotting_info->print_ylabel="activity coefficient";
//    plotting_info->print_head="Systems: test plot";
//}

///// Writes  structure to  the GEMSFIT configuration file
//void PlotFit::out_plotfit_vars_txt( bool with_comments, bool brief_mode )
//{
//    string fname = gpf->OptParamFile();
//    fstream ff(fname.c_str(), ios::out|ios::app );
//    ErrorIf( !ff.good() , fname.c_str(), "OptParamFile text open error");

//    TPrintArrays  prar(9, PlotFit_fields, ff);

//    if(with_comments )
//    {
//        ff << "\n\n#########################################################################" << endl;
//        ff << "#>>>>>>>>>>>>>>> Fitting Results section >>>>>>>>>>>>>>>>>>>>>>>>>>>>>#" << endl;
//        ff << "#########################################################################" << endl;
//    }

//    prar.writeArray(f_PrintTemperatures,  plotting_info->print_temperatures, 2, with_comments, brief_mode );
//    prar.writeArray(f_PrintPressures,  plotting_info->print_pressures, 2, with_comments, brief_mode );
//    prar.writeArray(f_PrintMolalities,  plotting_info->print_molalities, 2, with_comments, brief_mode );
//    prar.writeField(f_PrintFormat,  plotting_info->print_format, with_comments, brief_mode );
//    prar.writeField(f_PrintFilename,  plotting_info->print_filename, with_comments, brief_mode );
//    prar.writeField(f_PrintMeasValueCode,  (long int)plotting_info->print_code, with_comments, brief_mode );
//    prar.writeField(f_PrintLabelXaxis,  plotting_info->print_xlabel, with_comments, brief_mode );
//    prar.writeField(f_PrintLabelYaxis,  plotting_info->print_ylabel, with_comments, brief_mode );
//    prar.writeField(f_PrintHead,  plotting_info->print_head, with_comments, brief_mode );

//}

///// Function populates member variables of the Plot_Fit
///// class with data form the GEMSFIT configuration file
//void PlotFit::set_plotfit_vars_txt( )
//{
//    // open file for reading
//    string fname = gpf->OptParamFile();
//    fstream ff(fname.c_str(), ios::in );
//    ErrorIf( !ff.good() , fname, "OptParamFile Fileopen error");

//    TReadArrays  rdar(9, PlotFit_fields, ff);

//    long int nfild = rdar.findNextNotAll();
//    while( nfild >=0 )
//        {
//          switch( nfild )
//          {
//           case f_PrintTemperatures: rdar.readArray( "PrintTemperatures",  plotting_info->print_temperatures );
//                   break;
//           case f_PrintPressures: rdar.readArray( "PrintPressures",  plotting_info->print_pressures );
//                   break;
//           case f_PrintMolalities: rdar.readArray( "PrintMolalities",  plotting_info->print_molalities );
//                   break;
//          case f_PrintFormat: rdar.readArray( "PrintFormat",  plotting_info->print_format );
//                  break;
//          case f_PrintFilename: rdar.readArray( "PrintFilename",  plotting_info->print_filename );
//                  break;
//          case f_PrintMeasValueCode: rdar.readArray( "PrintMeasValueCode",  &plotting_info->print_code, 1 );
//                  break;
//          case f_PrintLabelXaxis: rdar.readArray( "PrintLabelXaxis",  plotting_info->print_xlabel );
//                  break;
//          case f_PrintLabelYaxis: rdar.readArray( "PrintLabelYaxis",  plotting_info->print_ylabel );
//                  break;
//          case f_PrintHead: rdar.readArray( "PrintHead",  plotting_info->print_head );
//                  break;
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


void F_to_OP (opti_vector *op, IOJFormat Jformat, string nfild)
{
    vector<string> out;
    Data_Manager *temp = new Data_Manager(1);
    temp->parse_JSON_object(Jformat.format, IV, out);

    op->opt.push_back( atof(out.at(0).c_str()) );
    op->optv0.push_back( atof(out.at(0).c_str()) );
    out.clear();

    temp->parse_JSON_object(Jformat.format, UB, out);
    op->UB.push_back( atof(out.at(0).c_str()) );
    out.clear();

    temp->parse_JSON_object(Jformat.format, LB, out);
    op->LB.push_back( atof(out.at(0).c_str()) );
    out.clear();

    op->Ptype.push_back( nfild );
    op->Pindex.push_back( Jformat.index );
}

void F_to_OP (double val, opti_vector *op, IOJFormat Jformat, string nfild)
{
    op->opt.push_back(val);
    op->optv0.push_back(val);
    op->LB.push_back(val-val*bperc/100);
    op->UB.push_back(val+val*bperc/100);

    op->Ptype.push_back( nfild );
    op->Pindex.push_back( Jformat.index );
}

void R_to_OP (opti_vector::RDc *r, IOJFormat Jformat, string nfild)
{
    vector<string> out;
    Data_Manager *temp = new Data_Manager(1);

    temp->parse_JSON_object(Jformat.format, IV, out);
    r->IV = atof(out.at(0).c_str());
    out.clear();

    temp->parse_JSON_object(Jformat.format, Ref, out);
    r->Ref = out.at(0);
    out.clear();

    temp->parse_JSON_object(Jformat.format, logK, out);
    r->logK = atof(out.at(0).c_str());
    out.clear();

    temp->parse_JSON_object(Jformat.format, nC, out);
    r->nC = atoi(out.at(0).c_str());
    out.clear();

    temp->parse_JSON_object(Jformat.format, species, out);
    r->Dc_name = out.at(0);
    out.clear();

    temp->parse_JSON_object(Jformat.format, RC, out);
    for (unsigned int i = 0 ; i < out.size() ; i++)
    {
        r->rdc_species.push_back( out.at(i) );
    }
    out.clear();

    temp->parse_JSON_object(Jformat.format, Rcoef, out);
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

//    op->opt.push_back( atof(out.at(0).c_str()) );
//    op->optv0.push_back( atof(out.at(0).c_str()) );
//    out.clear();

//    temp->parse_JSON_object(Jformat.format, UB, out);
//    op->UB.push_back( atof(out.at(0).c_str()) );
//    out.clear();

//    temp->parse_JSON_object(Jformat.format, LB, out);
//    op->LB.push_back( atof(out.at(0).c_str()) );
//    out.clear();

//    op->Ptype.push_back( nfild );
//    op->Pindex.push_back( Jformat.index );
}

// ----------- End of  visor.cpp ----------------------------
