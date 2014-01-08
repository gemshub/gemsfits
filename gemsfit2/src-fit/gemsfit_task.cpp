//--------------------------------------------------------------------
// $Id: gemsfit_task.cpp 789 2012-12-19 17:08:55Z kulik $
//
/// \file gemsfit_task.cpp
/// Implementation of TGfitTask class functionality - advanced
/// interface between GEM IPM and GEMSFIT node array
/// working with one DATACH structure and arrays of DATABR structures
//
// Copyright (C) 2013 G.D.Miron, D.Kulik, S.V.Dmytrieva
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

#include "gemsfit_task.h"
#include "io_arrays.h"
#include "gdatastream.h"
#include "gemsfit_iofiles.h"
#include "keywords.h"
#include "gemsfit_global_functions.h"
#include "gemsfit_dynamic_functions.h"
#include <cmath>
#include "gemsfit_target_functions.h"
#include <iomanip>


#ifndef __unix
#include <io.h>
#endif


using namespace std;

int master_counter;

istream& f_getline(istream& is, gstring& str, char delim);

//extern outField DataBR_fields[58];

TGfitTask* TGfitTask::gft;


//< Constructors for 1D arrangement of nodes
TGfitTask::TGfitTask(  )/*: anNodes(nNod)*/
{
    weighted_Tfun_sum_of_residuals = 0.0;
    for (unsigned int i=1; i <MPI+1; i++)
    {
        EXPndx.push_back(-1);COMPndx.push_back(-1);PHndx.push_back(-1);PHPndx.push_back(-1);
        iNa.push_back(0.0); iO.push_back(0.0); iH.push_back(0.0); iCl.push_back(0.0);
    }
    h_grad = false;

    // GEMSFIT logfile
    //const char path[200] = "output_GEMSFIT/SS_GEMSFIT.log";
    ofstream fout;
    fout.open(gpf->FITLogFile().c_str(), ios::app);
    if( fout.fail() )
    { cout<<"Output fileopen error"<<endl; exit(1); }

    // file containing the input parameters of the system and of the optimization class
    param_file  = gpf->OptParamFile().c_str();

    // For parameter optimization do not use printing of results
    printfile = gpf->ResultDir()+"FIT_results.csv";

    // For parameter optimization do not use parallelization of the Monte Carlo loop (=true). Instead, execute loop over measurements within objective function in parallel (=false).
//    MC_MPI = false;

    // nodes
    anNodes = experiments.size();

    for (int j=0; j<anNodes; j++)
    {
        NodT.push_back( new TNode );
    }

    // initialize nodes with the experimental data
    fout << "8. gemsfit_task.cpp line 94. Initializing nodes with the experimental data; " << endl;
    setnodes ( );

    // getting the parameters to be optimized from DCH, DBR and multi structures, and optimization settings form the input file
    fout << "9. gemsfit_task.cpp line 98. Initializing optimization structure; " << endl;
    Opti = new optimization ( );

    fout << "12. gemsfit_task.cpp line 101. Initializing the Target function structure & get_DatTarget(); " << endl;
    Tfun = new TargetFunction;
    print = new ResPrint(printfile, Opti);
    get_DataTarget ( );

    /// function in iofiles.cpp to read the logK lookup array instead of get function!
    get_logK_TPpairs ();


    // check for errors and inconsitencies of input options and parameters
    gfit_error ( );

    double temp_res;
    number_of_residuals = get_number_of_residuals();
    for (int j= 0; j <number_of_residuals; j++)
    {
        Tuckey_weights.push_back(1); // default value 1
    }

    // to have the average and minimum value calculated
    get_sum_of_residuals( temp_res);

    if (this->LimitOfDetection > (this->minimum_value/100))
        this->LimitOfDetection = this->minimum_value/100; // sets the limit of detection not more than 100 times smaller than the lowest experimental value

    print->print_header(Tfun->type, Tfun->weight, Opti->optv.size());
    fout.close();

}

void TGfitTask::gfit_error ( )
{
    for (unsigned int i=0; i<Opti->opt.size(); ++i)
    {
        if( ((Opti->Ptype[i] == "TK") || (Opti->Ptype[i] == "P")) && (NodT.size() > 1))
         {
            cout << "Error - When fitting TK or P only one system/experiment/sample si allowed. " << endl;
            exit(1);
         }
    }
}

void TGfitTask::run_optim()
{
    ofstream fout;
    fout.open(gpf->FITLogFile().c_str(), ios::app);
    if( fout.fail() )
    { cout<<"Output fileopen error"<<endl; exit(1); }

//    titration(this);

    fout << "13. gemsfit_task.cpp line 152. Initializing optimization init_optim; " << endl;
    init_optim (Opti->optv, weighted_Tfun_sum_of_residuals);

    fout.close();
}

void TGfitTask::Ainit_optim (std::vector<double> &optv_, /*int &countit,*/ double &weighted_Tfun_sum_of_residual)
{
    init_optim( optv_, weighted_Tfun_sum_of_residuals );
}

// Initialize optimization object and Run Optimization by calling build_optim
void TGfitTask::init_optim( std::vector<double> &optv_, /*int &countit,*/ double &weighted_Tfun_sum_of_residuals )
{
    // Instantiate nlopt::opt object
    if( Opti->OptAlgo.compare( "'LN_COBYLA'" ) == 0 )
    {
        nlopt::opt nlopti_( nlopt::LN_COBYLA, optv_.size() );
        build_optim( nlopti_, optv_, weighted_Tfun_sum_of_residuals );
    }
    else if( Opti->OptAlgo.compare( "'GN_ISRES'" ) == 0 )
    {
        nlopt::opt nlopti_( nlopt::GN_ISRES, optv_.size() );
        build_optim( nlopti_, optv_, weighted_Tfun_sum_of_residuals );
    }
    else if( Opti->OptAlgo.compare( "'LN_BOBYQA'" ) == 0 )
    {
        nlopt::opt nlopti_( nlopt::LN_BOBYQA, optv_.size() );
        build_optim( nlopti_, optv_, weighted_Tfun_sum_of_residuals );
    }
    else if( Opti->OptAlgo.compare( "'GN_ORIG_DIRECT'" ) == 0 )
    {
        nlopt::opt nlopti_( nlopt::GN_ORIG_DIRECT, optv_.size() );
        build_optim( nlopti_, optv_, weighted_Tfun_sum_of_residuals );
    }
    else if( Opti->OptAlgo.compare( "'GN_ORIG_DIRECT_L'" ) == 0 )
    {
        nlopt::opt nlopti_( nlopt::GN_ORIG_DIRECT_L, optv_.size() );
        build_optim( nlopti_, optv_, weighted_Tfun_sum_of_residuals );
    }
    else if( Opti->OptAlgo.compare( "'LD_MMA'" ) == 0 )
    {
        nlopt::opt nlopti_( nlopt::LD_MMA, optv_.size() );
        build_optim( nlopti_, optv_, weighted_Tfun_sum_of_residuals );
    }
    else if( Opti->OptAlgo.compare( "'LD_SLSQP'" ) == 0 )
    {
        nlopt::opt nlopti_( nlopt::LD_SLSQP, optv_.size() );
        build_optim( nlopti_, optv_, weighted_Tfun_sum_of_residuals );
    }
    else if( Opti->OptAlgo.compare( "'GD_MLSL'" ) == 0 )
    {
        nlopt::opt nlopti_( nlopt::GD_MLSL, optv_.size() );
        build_optim( nlopti_, optv_, weighted_Tfun_sum_of_residuals );
    }
    else
    {
        cout<<" Unknown optimization algorithm !!!! "<<endl;
        cout<<" ... bail out now ... "<<endl;
        exit(1);
    }


}

// Initialize optimization object and Run Optimization
void TGfitTask::build_optim( nlopt::opt &NLopti, std::vector<double> &optv_, double &weighted_Tfun_sum_of_residuals )
{
    unsigned int i = 0;
    int j = 0;
    std::vector<double> grad;

    // GEMSFIT logfile
    //const char path[200] = "output_GEMSFIT/SS_GEMSFIT.log";
    ofstream ffout;
    ffout.open(gpf->FITLogFile().c_str(), ios::app);
    if( ffout.fail() )
    { cout<<"Output fileopen error"<<endl; exit(1); }

    ffout << " ... initializing optimization object in build_optim() ... " << endl;

//     Reset counter to zero
    master_counter = 0;

    if( Opti->OptNormParam )
    {
        Opti->normalize_params( optv_, NormParams );
        NormParams = true;
    }
    else // do not normalize parameters
    {
        Opti->optv = optv_;
    }

    // check if initial guesses have same number of elements as bound and check if initial guesses are within the bounds
    if( Opti->optv.size() != Opti->OptUpBounds.size() )
    {
        cout<<endl;
        cout<<"initial guess vector and bounds vector must have same number of elements !! "<<endl;
        cout<<"optv.size() = "<<Opti->optv.size()<<" <-> OptUbBounds.size() = "<<Opti->OptUpBounds.size()<<endl;
        cout<<" .... exiting now .... "<<endl;
            cout<<endl;
        exit(1);
    }
    for( i=0; i<Opti->OptLoBounds.size(); i++ )
    {
        if( Opti->optv[i]<Opti->OptLoBounds[i] || Opti->optv[i]>Opti->OptUpBounds[i] )
        {
            cout<<endl;
            cout<<"Initial guess value is not within given bounds!!"<<endl;
            cout<<"optv["<<i<<"] = "<<Opti->optv[i]<<" | OptLoBounds["<<i<<"] = "<<Opti->OptLoBounds[i]<<" | OptUpBounds["<<i<<"] = "<<Opti->OptUpBounds[i]<<endl;
            cout<<" .... exiting now .... "<<endl;
                cout<<endl;
            exit(1);
        }
    }

    ffout << "... assigning bounds and tollerance for optimization..." << endl;
    // assign bounds
    NLopti.set_lower_bounds( Opti->OptLoBounds );
    NLopti.set_upper_bounds( Opti->OptUpBounds );

    // specify relative tolerance tolerance on function value
    NLopti.set_xtol_rel( Opti->OptTolRel );

    // specify absolute tolerance on function value
    NLopti.set_xtol_abs( Opti->OptTolAbs );

    // maximum number of iterations
    NLopti.set_maxeval( Opti->OptMaxEval );


    /// specify objective function
    ffout << endl << "14. in gemsfit_task.cpp line 285. Setting minimizing objective function." << endl;
    NLopti.set_min_objective( Equil_objective_function_callback, this );

//        if( OptConstraints )
//        {
//            // apply inequality constraints
//            for( int j=0; j< ((int) constraint_data_v.size()); j++ )
//                stdstate.add_inequality_constraint( StdStateProp_constraint_function_callback, &constraint_data_v[j], 1e-4);
//        }

    // Set initial stepsize
    if( Opti->OptInitStep > 0 )
    {
        vector<double> inistep( Opti->optv.size(), 0. );
        for( j=0; j<(int) Opti->optv.size(); j++ )
        {
            inistep[j] = Opti->optv[j] * Opti->OptInitStep;
        }
        NLopti.set_initial_step( inistep );
    }

    ffout << "15. in gesfit_task.cpp line 306. Performing optimization."<<endl;

//    //===== For testing the objective function without oprimization =====//
//    weighted_Tfun_sum_of_residuals = Equil_objective_function_callback(Opti->optv, grad, this);

//    NLopti.set_maxeval(3);

    nlopt::result result = NLopti.optimize( Opti->optv, weighted_Tfun_sum_of_residuals );
    ffout<<"optv[0] = "<<Opti->optv[0]<<endl;
    ffout<<"size of optv = "<<Opti->optv.size()<<endl;

    ffout << "16. gemsfit_task.cpp line 317. Finished optimization; " << endl;


            // check results
                if( result < 0 )
                {
                    std::cout<<endl;
                    std::cout<<"   !!!  nlopt failed  !!!   "<<std::endl;
                    std::cout<<"   !!!  error code:   "<<result<<std::endl;
                    Opti->print_return_message( result );
                    std::cout<<endl;
                }
                else
                {
                    std::cout<<" NLopt return code: "<<result<<endl;
                    Opti->print_return_message( result );
                    string path = gpf->OutputDirPath();
                           path +=  "SS_myFIT.out";
                    ofstream fout;
                    fout.open(path.c_str(), ios::app);
                    if( fout.fail() )
                    { cout<<"Output fileopen error"<<endl; exit(1); }
                    fout<<"found minimum at <<f( ";
                    for( unsigned i=0; i<Opti->optv.size(); i++ )
                    {
                        fout<<Opti->optv[i]<<" ";
                    }
                    fout<<") = "<<weighted_Tfun_sum_of_residuals<<std::endl;
                    fout<<" after "<< master_counter <<" evaluations."<<std::endl;
                    fout.close();

                    std::cout<<"found minimum at <<f( ";
                    for( unsigned i=0; i<Opti->optv.size(); i++ )
                    {
                        std::cout<<Opti->optv[i]<<" ";
                    }
                    std::cout<<") = "<<weighted_Tfun_sum_of_residuals<<std::endl;
                }
                std::cout<<" after "<< master_counter <<" evaluations"<<std::endl;


   // copy resulting vector back to incoming optv vector (needed for printing results)
   optv_ = Opti->optv;

  // copy resulting vector back to incoming optv vector (needed for printing results)
  if( !Opti->OptNormParam )
      {
          optv_ = Opti->optv;
      }
  else
      {
          for( i=0; i<Opti->optv.size(); i++ )
              {
                  optv_[i] = Opti->optv[i] * fabs(Opti->opt[i]);
              }
      }

ffout.close();
}

void TGfitTask::setnodes()
{
    unsigned int n, i, j;
    // DATACH structure content
    int nIC, nDC, nPH, ICndx, DCndx/*, PHndx*/;
    long int NodeStatusCH, NodeHandle;
    double P_pa, T_k/*, PMc*/;
    double* new_moles_IC;
    double* xDC_up;
    double* xDC_lo;
    double* Ph_surf;
    bool salt = false;
    double h2o_kgamount = 0.0;


//#ifdef USE_MPI
    omp_set_num_threads(MPI);
    #pragma omp parallel for
//#endif
    for (n=0; n<NodT.size(); ++n)
    {
        if( NodT[n]->GEM_init( gpf->GEMS3LstFilePath().c_str() ) == 1 )
        {
            cout<<" .. ERROR occurred while reading input files !!! ..."<<endl;
        }
    }

    // initialize the nodes using the input GEMS3 file
    for (n=0; n<NodT.size(); ++n)
    {
        // Getting direct access to work node DATABR structure which exchanges the
        // data with GEMS3K (already filled out by reading the DBR input file)
        DATABR* dBR = NodT[n]->pCNode();
        DATACH* dCH = NodT[n]->pCSD();

        nIC = dCH->nIC;	// nr of independent components
        nDC = dCH->nDC;	// nr of dependent components
        nPH = dCH->nPH;
        xDC_up = new double[ nDC ];  // memory leaks may be here! (these arrays must be re-created at each n)
        xDC_lo = new double[ nDC ];
        Ph_surf = new double[ nPH ];
        new_moles_IC = new double[ nIC ]; // vector for holding the moles of independent components for each experiment

        // lower and upper bounds for concentration of DC
        for( i=0; i<nDC; i++ )
        {
            xDC_up[ i ]  = 1000000.;
            xDC_lo[ i ]  = 0.;
        }

        if (experiments.at(n)->U_KC.size() > 0)
        {
            for (i=0; i<experiments.at(n)->U_KC.size(); ++i)
            {
                if (experiments.at(n)->U_KC[i]->type == keys::DC)
                {
                    DCndx = NodT[n]->DC_name_to_xDB(experiments.at(n)->U_KC[i]->name.c_str());
                    xDC_up[DCndx] = experiments.at(n)->U_KC[i]->Qnt;
                } // else phase metastability constraints
            }
        }

        if (experiments.at(n)->L_KC.size() > 0)
        {
            for (i=0; i<experiments.at(n)->L_KC.size(); ++i)
            {
                if (experiments.at(n)->L_KC[i]->type == keys::DC)
                {
                    DCndx = NodT[n]->DC_name_to_xDB(experiments.at(n)->L_KC[i]->name.c_str());
                    xDC_lo[DCndx] = experiments.at(n)->L_KC[i]->Qnt;
                } // else phase metastability constraints
            }
        }

        // Surface energy of phases -> kinetics
        for( i=0; i<nPH; i++ )
            Ph_surf[i] = 0.;

        for (i=0; i<nIC; i++) // assigining default values for all IC (1e-09 - absent component); 0 for charge.
        {
            new_moles_IC[i]=1e-09;
            if (i==nIC-1) {
                new_moles_IC[i]=0.;
            }
        }

        // if Nitt present assign two moles
//        if (NodT[n]->IC_name_to_xDB("Nit") > -1) {
//            ICndx = NodT[n]->IC_name_to_xDB("Nit");
//            new_moles_IC[ICndx]=2;
//        }

         DCndx = NodT[n]->DC_name_to_xDB("H2");
//        NodT[n]->Set_nDC(DCndx, 1e-05);

        for (j=0; j<experiments[n]->sbcomp.size(); ++j)
        {
            if (( experiments[n]->sbcomp[j]->comp == "H2O" || experiments[n]->sbcomp[j]->comp == "H2O@" ) )
            {
                if (experiments[n]->sbcomp[j]->Qunit == keys::gram)
                {
                    h2o_kgamount = experiments[n]->sbcomp[j]->Qnt/1000;
                }
                else
                {
                     cout<<" H2O is not given in grams !!!! "<<endl;
                     cout<<" ... bail out now ... "<<endl;
                     exit(1);
                }
            }
        }
// ....................................................................
// NodT[n]->  This is written using TNode functions on 05.01.2014 by DK
        // Set amounts of dependent components (GEMS3K: DBR indexing)
        // go through all components and calculate the mole amounts of the IC for the b vector in GEMS
        for (j=0; j<experiments[n]->sbcomp.size(); ++j)
        {
            DATACH* CSD;
            long int nICb, nDCb, xDCb;
            char cName[32];
            double DCm, ICm, njDC;

            CSD = NodT[n]->pCSD(); // Get the pointer to chemical system definition data structure
            nICb = CSD->nICb;
            nDCb = CSD->nDCb;
            strcpy( cName, experiments[n]->sbcomp[j]->comp.c_str() );
            // or -1 if no such name was found in the DATACH DC name list
            xDCb = NodT[n]->DC_name_to_xDB( cName ); // Returns DBR index of DC given the DC Name string
                                    // or -1 if no such name was found in the DATACH DC name list
            if( xDCb >= 0 )
            { // This is a valid DC name from the system definition and DBR file
                if (experiments[n]->sbcomp[j]->Qunit == keys::molal)
                {   // conversion from molal to mole
//                    experiments[n]->sbcomp[j]->Qnt = experiments[n]->sbcomp[j]->Qnt*h2o_kgamount;
//                    experiments[n]->sbcomp[j]->Qunit = keys::mole; // this is not good!
                    njDC = experiments[n]->sbcomp[j]->Qnt * h2o_kgamount;
                }
                else
                    njDC = experiments[n]->sbcomp[j]->Qnt;
                if(  experiments[n]->sbcomp[j]->Qunit == keys::gram )
                {   // conversion from gram to mole
                    DCm = NodT[n]->DCmm( xDCb )*1000.;  // Retrieves the molar mass of DC in g/mol.
                    njDC /= DCm;
                }
                for(i=0; i<nICb; i++)
                {   // loop over ICs
                    double aij;
                    // Retrieves the stoichiometry coefficient a[xdc][xic] of IC in the formula of DC.
                    aij = NodT[n]->DCaJI( xDCb, i );
                    if( !aij )
                       continue;
                    new_moles_IC[i] += njDC * aij;
//                    ICm = NodT[n]->ICmm( i )*1000.; // Retrieves the molar mass of IC in g/mol.
                }
// check charge!
// check salt!
            }
            else {
                // This is not a name of DC used in DBR file!
                // Here, the use of GEM formula parser is needed!
                // Until this is done, the old stuff is retained! DK 05.01.2014
//            }
//.................................................................
            if (experiments[n]->sbcomp[j]->comp == "H2O")
            {
                ICndx = NodT[n]->IC_name_to_xDB("H");
                new_moles_IC[ICndx] += 2*experiments[n]->sbcomp[j]->Qnt/18.0153 +
                        1.2344*experiments[n]->sbcomp[j]->Qnt/1000*1e-05; // adds 1e-05 moles of H2 for each kg og H2O
//                NodT[n]->Set_nDC(DCndx, experiments[n]->sbcomp[j]->bQnt/1000*1e-05);

                if (NodT[n]->IC_name_to_xDB("Nit") < 0)
                {
//                    new_moles_IC[ICndx] += 1.5*experiments[n]->sbcomp[j]->bQnt/1000*1e-05;
                    NodT[n]->Set_nDC(DCndx, experiments[n]->sbcomp[j]->Qnt/1000*1e-05);
                }
                // cout << new_moles_IC[ICndx] << endl;
                ICndx = NodT[n]->IC_name_to_xDB("O");
                new_moles_IC[ICndx] +=  experiments[n]->sbcomp[j]->Qnt/18.0153
                        /*+ 3*experiments[n]->sbcomp[j]->bQnt/1000*1e-03*/;
            }
            else if (experiments[n]->sbcomp[j]->comp == "SiO2")
            {
                if (experiments[n]->sbcomp[j]->Qunit == keys::molal)
                {
                    experiments[n]->sbcomp[j]->Qnt = experiments[n]->sbcomp[j]->Qnt*h2o_kgamount*60.08;
                    experiments[n]->sbcomp[j]->Qunit = keys::gram;
                }
                ICndx = NodT[n]->IC_name_to_xDB("Si");
                new_moles_IC[ICndx] +=  experiments[n]->sbcomp[j]->Qnt/60.08;
               // cout << new_moles_IC[ICndx]<<endl;
                ICndx = NodT[n]->IC_name_to_xDB("O");
                new_moles_IC[ICndx] +=  2*experiments[n]->sbcomp[j]->Qnt/60.08;
            }
            else if (experiments[n]->sbcomp[j]->comp == "Al2O3")
            {
                if (experiments[n]->sbcomp[j]->Qunit == keys::molal)
                {
                    experiments[n]->sbcomp[j]->Qnt = experiments[n]->sbcomp[j]->Qnt*h2o_kgamount*101.9612772;
                    experiments[n]->sbcomp[j]->Qunit = keys::gram;
                }
                ICndx = NodT[n]->IC_name_to_xDB("Al");
                new_moles_IC[ICndx] +=  2*experiments[n]->sbcomp[j]->Qnt/101.9612772;
                ICndx = NodT[n]->IC_name_to_xDB("O");
                new_moles_IC[ICndx] +=  3*experiments[n]->sbcomp[j]->Qnt/101.9612772;
            }
            else if (experiments[n]->sbcomp[j]->comp == "K2O")
            {
                if (experiments[n]->sbcomp[j]->Qunit == keys::molal)
                {
                    experiments[n]->sbcomp[j]->Qnt = experiments[n]->sbcomp[j]->Qnt*h2o_kgamount*94.19605;
                    experiments[n]->sbcomp[j]->Qunit = keys::gram;
                }
                ICndx = NodT[n]->IC_name_to_xDB("K");
                new_moles_IC[ICndx] +=  2*experiments[n]->sbcomp[j]->Qnt/94.19605;
                ICndx = NodT[n]->IC_name_to_xDB("O");
                new_moles_IC[ICndx] +=  experiments[n]->sbcomp[j]->Qnt/94.19605;
            }
            else if (experiments[n]->sbcomp[j]->comp == "Na2O")
            {
                if (experiments[n]->sbcomp[j]->Qunit == keys::molal)
                {
                    experiments[n]->sbcomp[j]->Qnt = experiments[n]->sbcomp[j]->Qnt*h2o_kgamount*61.97897;
                    experiments[n]->sbcomp[j]->Qunit = keys::gram;
                }
                ICndx = NodT[n]->IC_name_to_xDB("Na");
                new_moles_IC[ICndx] +=  2*experiments[n]->sbcomp[j]->Qnt/61.97897;
                ICndx = NodT[n]->IC_name_to_xDB("O");
                new_moles_IC[ICndx] +=  experiments[n]->sbcomp[j]->Qnt/61.97897;
            }
            else if (experiments[n]->sbcomp[j]->comp == "Al(OH)3")
            {
                if (experiments[n]->sbcomp[j]->Qunit == keys::molal)
                {
                    experiments[n]->sbcomp[j]->Qnt = experiments[n]->sbcomp[j]->Qnt*h2o_kgamount*78.0035586;
                    experiments[n]->sbcomp[j]->Qunit = keys::gram;
                }
                ICndx = NodT[n]->IC_name_to_xDB("Al");
                new_moles_IC[ICndx] +=  1*experiments[n]->sbcomp[j]->Qnt/78.0035586;
                ICndx = NodT[n]->IC_name_to_xDB("H");
                new_moles_IC[ICndx] +=  3*experiments[n]->sbcomp[j]->Qnt/78.0035586;
                ICndx = NodT[n]->IC_name_to_xDB("O");
                new_moles_IC[ICndx] +=  3*experiments[n]->sbcomp[j]->Qnt/78.0035586;
            }
            else if (experiments[n]->sbcomp[j]->comp == "AlO(OH)")
            {
                if (experiments[n]->sbcomp[j]->Qunit == keys::molal)
                {
                    experiments[n]->sbcomp[j]->Qnt = experiments[n]->sbcomp[j]->Qnt*h2o_kgamount*59.9822326;
                    experiments[n]->sbcomp[j]->Qunit = keys::gram;
                }
                ICndx = NodT[n]->IC_name_to_xDB("Al");
                new_moles_IC[ICndx] +=  1*experiments[n]->sbcomp[j]->Qnt/59.9822326;
                ICndx = NodT[n]->IC_name_to_xDB("H");
                new_moles_IC[ICndx] +=  1*experiments[n]->sbcomp[j]->Qnt/59.9822326;
                ICndx = NodT[n]->IC_name_to_xDB("O");
                new_moles_IC[ICndx] +=  2*experiments[n]->sbcomp[j]->Qnt/59.9822326;
            }
            else if (experiments[n]->sbcomp[j]->comp == "Ca(OH)2")
            {
                if (experiments[n]->sbcomp[j]->Qunit == keys::molal)
                {
                    experiments[n]->sbcomp[j]->Qnt = experiments[n]->sbcomp[j]->Qnt*h2o_kgamount*74.09268;
                    experiments[n]->sbcomp[j]->Qunit = keys::gram;
                }
                ICndx = NodT[n]->IC_name_to_xDB("Ca");
                new_moles_IC[ICndx] +=  1*experiments[n]->sbcomp[j]->Qnt/74.09268;
               // cout << new_moles_IC[ICndx]<<endl;
                ICndx = NodT[n]->IC_name_to_xDB("H");
                new_moles_IC[ICndx] +=  2*experiments[n]->sbcomp[j]->Qnt/74.09268;
               // cout << new_moles_IC[ICndx]<<endl;
                ICndx = NodT[n]->IC_name_to_xDB("O");
                //cout << new_moles_IC[ICndx]<<endl;
                new_moles_IC[ICndx] +=  2*experiments[n]->sbcomp[j]->Qnt/74.09268;
            }

//            CaCO3
//            SrCO3
//            Nit1.6O0.4
//            Nit2
//            O2

            else if (experiments[n]->sbcomp[j]->comp == "NaOH")
            {
                if (experiments[n]->sbcomp[j]->Qunit == keys::molal)
                {
                    experiments[n]->sbcomp[j]->Qnt = experiments[n]->sbcomp[j]->Qnt*h2o_kgamount*39.99710928;
                    experiments[n]->sbcomp[j]->Qunit = keys::gram;
                }
                ICndx = NodT[n]->IC_name_to_xDB("Na");
                new_moles_IC[ICndx] +=  1*experiments[n]->sbcomp[j]->Qnt/39.99710928;
                ICndx = NodT[n]->IC_name_to_xDB("H");
                new_moles_IC[ICndx] +=  1*experiments[n]->sbcomp[j]->Qnt/39.99710928;
                ICndx = NodT[n]->IC_name_to_xDB("O");
                new_moles_IC[ICndx] +=  1*experiments[n]->sbcomp[j]->Qnt/39.99710928;

                // changing the background electrolite settings
                if (!salt){
                NodT[n]->Set_PMc(0.098, 0 );
                NodT[n]->Set_PMc(3.31, 1 );
                NodT[n]->Set_PMc(3, 4 );}

                salt = true;

            }
            else if (experiments[n]->sbcomp[j]->comp == "KOH")
            {
                if (experiments[n]->sbcomp[j]->Qunit == keys::molal)
                {
                    experiments[n]->sbcomp[j]->Qnt = experiments[n]->sbcomp[j]->Qnt*h2o_kgamount*56.10564;
                    experiments[n]->sbcomp[j]->Qunit = keys::gram;
                }
                ICndx = NodT[n]->IC_name_to_xDB("K");
                new_moles_IC[ICndx] +=  1*experiments[n]->sbcomp[j]->Qnt/56.10564;
                ICndx = NodT[n]->IC_name_to_xDB("H");
                new_moles_IC[ICndx] +=  1*experiments[n]->sbcomp[j]->Qnt/56.10564;
                ICndx = NodT[n]->IC_name_to_xDB("O");
                new_moles_IC[ICndx] +=  1*experiments[n]->sbcomp[j]->Qnt/56.10564;

                // changing the bacground electrolite settings
                if (!salt){
                NodT[n]->Set_PMc(0.123, 0 );
                NodT[n]->Set_PMc(3.67, 1 );
                NodT[n]->Set_PMc(4, 4 );}

                salt = true;
            }
            else if (experiments[n]->sbcomp[j]->comp == "KCl")
            {
                if (experiments[n]->sbcomp[j]->Qunit == keys::molal)
                {
                    experiments[n]->sbcomp[j]->Qnt = experiments[n]->sbcomp[j]->Qnt*h2o_kgamount*74.5513;
                    experiments[n]->sbcomp[j]->Qunit = keys::gram;
                }
                ICndx = NodT[n]->IC_name_to_xDB("K");
                new_moles_IC[ICndx] +=  1*experiments[n]->sbcomp[j]->Qnt/74.5513;
                ICndx = NodT[n]->IC_name_to_xDB("Cl");
                new_moles_IC[ICndx] +=  1*experiments[n]->sbcomp[j]->Qnt/74.5513;

                // changing the bacground electrolite settings
                if (!salt){
                NodT[n]->Set_PMc(0.025, 0 );
                NodT[n]->Set_PMc(4.08, 1 );
                NodT[n]->Set_PMc(2, 4 );}

                salt = true;
            }
            else if (experiments[n]->sbcomp[j]->comp == "CO2")
            {
                if (experiments[n]->sbcomp[j]->Qunit == keys::molal)
                {
                    experiments[n]->sbcomp[j]->Qnt = experiments[n]->sbcomp[j]->Qnt*h2o_kgamount*44.0095;
                    experiments[n]->sbcomp[j]->Qunit = keys::gram;
                }
                ICndx = NodT[n]->IC_name_to_xDB("C");
                new_moles_IC[ICndx] +=  1*experiments[n]->sbcomp[j]->Qnt/44.0095;
                ICndx = NodT[n]->IC_name_to_xDB("O");
                new_moles_IC[ICndx] +=  2*experiments[n]->sbcomp[j]->Qnt/44.0095;
            }
            else if (experiments[n]->sbcomp[j]->comp == "HCl")
            {
                if (experiments[n]->sbcomp[j]->Qunit == keys::molal)
                {
                    experiments[n]->sbcomp[j]->Qnt = experiments[n]->sbcomp[j]->Qnt*h2o_kgamount*36.4611;
                    experiments[n]->sbcomp[j]->Qunit = keys::gram;
                }
                ICndx = NodT[n]->IC_name_to_xDB("H");
                new_moles_IC[ICndx] +=  1*experiments[n]->sbcomp[j]->Qnt/36.4611;
                ICndx = NodT[n]->IC_name_to_xDB("Cl");
                new_moles_IC[ICndx] +=  1*experiments[n]->sbcomp[j]->Qnt/36.4611;
            }
            else if (experiments[n]->sbcomp[j]->comp == "NaCl")
            {
                if (experiments[n]->sbcomp[j]->Qunit == keys::molal)
                {
                    experiments[n]->sbcomp[j]->Qnt = experiments[n]->sbcomp[j]->Qnt*h2o_kgamount*58.4430;
                    experiments[n]->sbcomp[j]->Qunit = keys::gram;
                }
                ICndx = NodT[n]->IC_name_to_xDB("Na");
                new_moles_IC[ICndx] +=  experiments[n]->sbcomp[j]->Qnt/58.4430;
                ICndx = NodT[n]->IC_name_to_xDB("Cl");
                new_moles_IC[ICndx] +=  experiments[n]->sbcomp[j]->Qnt/58.4430;

                // changing the bacground electrolite settings
                NodT[n]->Set_PMc(0.064, 0 );
                NodT[n]->Set_PMc(3.72, 1 );
                NodT[n]->Set_PMc(1, 4 );

                salt = true;
            }
                else
                {
                    cout<<" Unknown component in gemsfit_task.cpp line 768 !!!! "<<endl;
                    cout<<" ... bail out now ... "<<endl;
                    exit(1);
                }
        }
        } // for j
        if (!salt)
        {
            // use NaCl
            NodT[n]->Set_PMc(0.064, 0 );
            NodT[n]->Set_PMc(3.72, 1 );
            NodT[n]->Set_PMc(1, 4 );
        }
        salt = false;
        h2o_kgamount = 0.0;

        // ---- // ---- // Set temperature and pressure // ---- // ---- //
        P_pa = 100000 * experiments[n]->sP;
        T_k = 273.15 + experiments[n]->sT;

        // ---- // ---- // set the new amount of IC and T & P from experiment i // ---- // ---- //
        // in the future - implement a Tnode function that stes just T, P and bIC vector of amount of independent components.
        // ---- // ---- // Transfer new temperature, pressure and b-vector to GEMS3K // ---- // ---- //
//        NodT[n]->GEM_from_MT( NodeHandle, NodeStatusCH, T_k, P_pa, 0., 0., new_moles_IC, xDC_up, xDC_lo, Ph_surf );

        NodT[n]->GEM_from_MT( NodeHandle, NodeStatusCH, T_k, P_pa, new_moles_IC, xDC_up, xDC_lo );

        delete[] new_moles_IC;
        delete[] xDC_up;
        delete[] xDC_lo;
        delete[] Ph_surf;
    }  // for n

}

void TGfitTask::get_DataTarget ( )
{
    vector<string> out, out2;
    parse_JSON_array_object(DataTarget, "OFUN", "OPH", out);
    out.clear();


    parse_JSON_object(DataTarget, keys::Target, out);
    Tfun->name = out[0]; // Name of target function
    out.clear();

    parse_JSON_object(DataTarget, keys::TT, out);
    Tfun->type = out[0]; // Type of the target function
    out.clear();

    parse_JSON_object(DataTarget, keys::WT, out);
    Tfun->weight = out[0]; // Weight of target function
    out.clear();

    parse_JSON_object(DataTarget, keys::OFUN, out);
    for (unsigned int i = 0 ; i < out.size() ; i++)
    {
        Tfun->objfun.push_back(new TargetFunction::obj_fun); // initializing
        Tfun->objfun[i]->exp_phase = "NULL";
        Tfun->objfun[i]->exp_CT = "NULL";
        Tfun->objfun[i]->exp_CN = "NULL";
        Tfun->objfun[i]->exp_unit = "NULL";
        Tfun->objfun[i]->exp_DCP = "NULL";
        Tfun->objfun[i]->meas_average = 0.0;
    }
    out.clear();
    int j=0;
    int jj=0;

    parse_JSON_array_object(DataTarget, keys::OFUN, keys::EPH, out);
    for (unsigned int i = 0 ; i < out.size() ; i++)
    {
        Tfun->objfun[i]->exp_phase = out[i];
    }
    out.clear();

    parse_JSON_array_object(DataTarget, keys::OFUN, keys::CT, out);
    parse_JSON_array_object(DataTarget, keys::OFUN, keys::DCP, out2);
    for (unsigned int i = 0 ; i < out.size() ; i++)
    {
        Tfun->objfun[i]->exp_CT = out[i];
        if ((out[i] == keys::DC) && (out2.size() > 0))
        {
            Tfun->objfun[i]->exp_DCP = out2[j];
            ++j;
        }
    }
    out.clear();
    out2.clear();

    parse_JSON_array_object(DataTarget, keys::OFUN, keys::CN, out);
    for (unsigned int i = 0 ; i < out.size() ; i++)
    {
        Tfun->objfun[i]->exp_CN = out[i];
    }
    out.clear();

    parse_JSON_array_object(DataTarget, keys::OFUN, keys::Qunit, out);
    for (unsigned int i = 0 ; i < out.size() ; i++)
    {
        Tfun->objfun[i]->exp_unit = out[i];
    }
    out.clear();

}

// will go away after implementing way to read logK's from the input file
void TGfitTask::get_logK_TPpairs()
{
    double DG = 0.0;
    const double Rln = -2.302585093*8.314472;
    double RTln = 0.0;
    bool h_logK = false;

    // loop trough reactions
    for (unsigned int i = 0; i< this->Opti->reactions.size(); ++i)
    {
        // checks if not the logK values were not already read from the input file
        if (this->Opti->reactions[i]->logK_TPpairs.size() == 0) h_logK = true;

        // 25 C 1 bar
        for (unsigned int k = 0; k<this->Opti->reactions[i]->rdc_species.size(); ++k)
        {
            DG += this->NodT[0]->DC_G0(this->Opti->reactions[i]->rdc_species_ind[k], 100000, 25+273.15, false) * this->Opti->reactions[i]->rdc_species_coef[k];
        }
        RTln = Rln * (25+273.15);
        if (h_logK) {
        this->Opti->reactions[i]->dG_reaction_TP.push_back(DG);
        this->Opti->reactions[i]->logK_TPpairs.push_back(DG/RTln);}
        DG = 0.0;

        // loop trough TP
        for (unsigned int j = 0; j<this->TP_pairs[0].size(); ++j)
        {
            // loop trough rection species to calculate delta G of reaction
            for (unsigned int k = 0; k<this->Opti->reactions[i]->rdc_species.size(); ++k)
            {
                DG += this->NodT[0]->DC_G0(this->Opti->reactions[i]->rdc_species_ind[k], this->TP_pairs[1][j]*100000, this->TP_pairs[0][j]+273.15, false) * this->Opti->reactions[i]->rdc_species_coef[k];
            }
            RTln = Rln * (this->TP_pairs[0][j]+273.15);
            if (h_logK) {
            this->Opti->reactions[i]->dG_reaction_TP.push_back(DG);
            this->Opti->reactions[i]->logK_TPpairs.push_back(DG/RTln);}
            DG = 0.0;
        }
        h_logK = false;
    }
}

// Main function that calculates the sum of residuals
void TGfitTask::get_sum_of_residuals( double &residuals)
{
    double average = 0.0, min = 1e-05;
    residuals = 0.0;

    int tk=0;
    // loop trough objective function
    for (unsigned int j=0; j<Tfun->objfun.size(); ++j)
    {
        int count = 0;
    /// Target function
    // Loop through all experiments
    for (unsigned int i=0; i<this->experiments.size(); ++i)
    {
            if ((this->Tfun->objfun[j]->exp_phase !="NULL") && (this->experiments[i]->expphases.size() > 0))
            {
                // loop trough all phases
                for (unsigned int p=0; p<this->experiments[i]->expphases.size(); ++p)
                {
                    if ((Tfun->objfun[j]->exp_CT == keys::IC) /*&& (Tfun->objfun[j]->exp_property =="NULL")*/)
                    {
                        // loop trough all elements
                        for (unsigned int e=0; e<this->experiments[i]->expphases[p]->phIC.size(); ++e)
                        {
                            if ((this->experiments[i]->expphases[p]->phIC[e]->comp == this->Tfun->objfun[j]->exp_CN) && (this->experiments[i]->expphases[p]->phase == this->Tfun->objfun[j]->exp_phase ))
                            {
                                // check for unit
                                check_unit(i, p, e, Tfun->objfun[j]->exp_unit, this );
                                average = average + this->experiments[i]->expphases[p]->phIC[e]->Qnt;
                                if (this->experiments[i]->expphases[p]->phIC[e]->Qnt < min)
                                    min = this->experiments[i]->expphases[p]->phIC[e]->Qnt;
                                residuals = residuals + residual_phase_elem (i, p, e, j, this) * this->Tuckey_weights[tk];
                                tk++;
                                ++count;
                            }
                        }
                    } else
                        if ((Tfun->objfun[j]->exp_CT == keys::MR) /*&& (Tfun->objfun[j]->exp_property =="NULL")*/)
                        {
                            // loop trough all elements
                            for (unsigned int f=0; f<this->experiments[i]->expphases[p]->phMR.size(); ++f)
                            {
                                if ((this->experiments[i]->expphases[p]->phMR[f]->comp == this->Tfun->objfun[j]->exp_CN) && (this->experiments[i]->expphases[p]->phase == this->Tfun->objfun[j]->exp_phase ))
                                {
                                    // check for unit
//                                    check_unit(i, p, e, Tfun->objfun[j]->exp_unit, this );
                                    average = average + this->experiments[i]->expphases[p]->phMR[f]->Qnt;
                                    if (this->experiments[i]->expphases[p]->phMR[f]->Qnt < min)
                                        min = this->experiments[i]->expphases[p]->phMR[f]->Qnt;
                                    residuals = residuals + residual_phase_elemMR (i, p, f, j, this) * this->Tuckey_weights[tk];
                                    tk++;
                                    ++count;
                                }
                            }
                        } else

                        if ((Tfun->objfun[j]->exp_CT == keys::property) && (this->experiments[i]->expphases[p]->phprop.size() > 0) /*&& (this->Tfun->objfun[j]->exp_dcomp == "NULL")*/)
                        {
                        // loop trough all properties
                        for (unsigned int pp = 0; pp< this->experiments[i]->expphases[p]->phprop.size(); ++pp)
                        {
                            if ((this->experiments[i]->expphases[p]->phprop[pp]->property == Tfun->objfun[j]->exp_CN) && (this->experiments[i]->expphases[p]->phase == this->Tfun->objfun[j]->exp_phase ))
                            {
                                // check for unit
                                check_prop_unit(i, p, pp, Tfun->objfun[j]->exp_unit, this );
                                average = average + this->experiments[i]->expphases[p]->phprop[pp]->Qnt;
                                if (this->experiments[i]->expphases[p]->phprop[pp]->Qnt < min)
                                    min = this->experiments[i]->expphases[p]->phprop[pp]->Qnt;
                                residuals = residuals + residual_phase_prop (i, p, pp, j, this) * this->Tuckey_weights[tk];
                                tk++;
                                ++count;
                            }
                        }
                    } else
                        if (/*(Tfun->objfun[j]->exp_property !="NULL") &&*/ (this->experiments[i]->expphases[p]->phDC.size() > 0) && (Tfun->objfun[j]->exp_CT == keys::DC))
                        {
                            // loop trough all dependent components
                            for (unsigned int dc = 0; dc< this->experiments[i]->expphases[p]->phDC.size(); ++dc)
                            {
                                if ((this->experiments[i]->expphases[p]->phDC[dc]->DC == Tfun->objfun[j]->exp_CN) && (this->experiments[i]->expphases[p]->phase == this->Tfun->objfun[j]->exp_phase ))
                                {
                                    // loop trough all dep comp properties
                                    for (unsigned int dcp = 0; dcp < this->experiments[i]->expphases[p]->phDC[dc]->DCprop.size(); ++dcp)
                                    {
                                        if (this->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->property == Tfun->objfun[j]->exp_DCP)
                                        {
//                                            cout << "yes"<<endl;
                                            //                                    // check for unit
                                            //                                    check_dcomp_unit(i, p, dc, dcp, sys->Tfun->objfun[j]->exp_unit, sys );
                                            average = average + this->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qnt;
                                            residuals = residuals + residual_phase_dcomp (i, p, dc, dcp, j, this) * this->Tuckey_weights[tk];
                                            tk++;
                                            ++count;
                                        }
                                    }
                                }
                            }
                        }
                }
            }
        }
    if (count > 0)
    {
        average = average / count;
    }
    Tfun->objfun[j]->meas_average = average;
    this->minimum_value = min;
    average = 0.0;
    }
}

// function for counting the total numbers of residuals that will be calculated
int TGfitTask::get_number_of_residuals( )
{
    int nr = 0;
    // loop trough objective function
    for (unsigned int j=0; j<Tfun->objfun.size(); ++j)
    {
    /// Target function
    // Loop trough all experiments
    for (unsigned int i=0; i<this->experiments.size(); ++i)
    {
            if ((this->Tfun->objfun[j]->exp_phase !="NULL") && (this->experiments[i]->expphases.size() > 0))
            {
                // loop trough all phases
                for (unsigned int p=0; p<this->experiments[i]->expphases.size(); ++p)
                {
                    if ((Tfun->objfun[j]->exp_CT == keys::IC) /*&& (Tfun->objfun[j]->exp_property =="NULL")*/)
                    {
                        // loop trough all elements
                        for (unsigned int e=0; e<this->experiments[i]->expphases[p]->phIC.size(); ++e)
                        {
                            if ((this->experiments[i]->expphases[p]->phIC[e]->comp == this->Tfun->objfun[j]->exp_CN) && (this->experiments[i]->expphases[p]->phase == this->Tfun->objfun[j]->exp_phase ))
                            {
                                ++nr;
                            }
                        }
                    } else
                        if ((Tfun->objfun[j]->exp_CT == keys::MR) /*&& (Tfun->objfun[j]->exp_property =="NULL")*/)
                        {
                            // loop trough all elements
                            for (unsigned int f=0; f<this->experiments[i]->expphases[p]->phMR.size(); ++f)
                            {
                                if ((this->experiments[i]->expphases[p]->phMR[f]->comp == this->Tfun->objfun[j]->exp_CN) && (this->experiments[i]->expphases[p]->phase == this->Tfun->objfun[j]->exp_phase ))
                                {
                                    ++nr;
                                }
                            }
                        } else

                        if ((Tfun->objfun[j]->exp_CT == keys::property) && (this->experiments[i]->expphases[p]->phprop.size() > 0) /*&& (this->Tfun->objfun[j]->exp_dcomp == "NULL")*/)
                        {
                        // loop trough all properties
                        for (unsigned int pp = 0; pp< this->experiments[i]->expphases[p]->phprop.size(); ++pp)
                        {
                            if ((this->experiments[i]->expphases[p]->phprop[pp]->property == Tfun->objfun[j]->exp_CN) && (this->experiments[i]->expphases[p]->phase == this->Tfun->objfun[j]->exp_phase ))
                            {
                                ++nr;
                            }
                        }
                    } else
                        if (/*(Tfun->objfun[j]->exp_property !="NULL") &&*/ (this->experiments[i]->expphases[p]->phDC.size() > 0) && (Tfun->objfun[j]->exp_CT == keys::DC))
                        {
                            // loop trough all dependent components
                            for (unsigned int dc = 0; dc< this->experiments[i]->expphases[p]->phDC.size(); ++dc)
                            {
                                if ((this->experiments[i]->expphases[p]->phDC[dc]->DC == Tfun->objfun[j]->exp_CN) && (this->experiments[i]->expphases[p]->phase == this->Tfun->objfun[j]->exp_phase ))
                                {
                                    // loop trough all dep comp properties
                                    for (unsigned int dcp = 0; dcp < this->experiments[i]->expphases[p]->phDC[dc]->DCprop.size(); ++dcp)
                                    {
                                        if (this->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->property == Tfun->objfun[j]->exp_DCP)
                                        {

                                            ++nr;
                                        }
                                    }
                                }
                            }
                        }
                }
            }
        }

    }
    return nr;
}


void TGfitTask::add_MC_scatter( vector<double> scatter)
{
    double average = 0.0, min = 1e-05;
    double residuals = 0.0;
    int count = 0;
    // loop trough objective function
    for (unsigned int j=0; j<Tfun->objfun.size(); ++j)
    {
    /// Target function
    // Loop trough all experiments
    for (unsigned int i=0; i<this->experiments.size(); ++i)
    {
            if ((this->Tfun->objfun[j]->exp_phase !="NULL") && (this->experiments[i]->expphases.size() > 0))
            {
                // loop trough all phases
                for (unsigned int p=0; p<this->experiments[i]->expphases.size(); ++p)
                {
                    if ((Tfun->objfun[j]->exp_CT == keys::IC) /*&& (Tfun->objfun[j]->exp_property =="NULL")*/)
                    {
                        // loop trough all elements
                        for (unsigned int e=0; e<this->experiments[i]->expphases[p]->phIC.size(); ++e)
                        {
                            if ((this->experiments[i]->expphases[p]->phIC[e]->comp == this->Tfun->objfun[j]->exp_CN) && (this->experiments[i]->expphases[p]->phase == this->Tfun->objfun[j]->exp_phase ))
                            {
                                this->experiments[i]->expphases[p]->phIC[e]->Qnt = scatter[count];
//                                // check for unit
//                                check_unit(i, p, e, Tfun->objfun[j]->exp_unit, this );
//                                average = average + this->experiments[i]->expphases[p]->phIC[e]->Qnt;
//                                if (this->experiments[i]->expphases[p]->phIC[e]->Qnt < min)
//                                    min = this->experiments[i]->expphases[p]->phIC[e]->Qnt;
//                                residuals = residuals + residual_phase_elem (i, p, e, j, this);
                                ++count;
                            }
                        }
                    } else
                        if ((Tfun->objfun[j]->exp_CT == keys::MR) /*&& (Tfun->objfun[j]->exp_property =="NULL")*/)
                        {
                            // loop trough all elements
                            for (unsigned int f=0; f<this->experiments[i]->expphases[p]->phMR.size(); ++f)
                            {
                                if ((this->experiments[i]->expphases[p]->phMR[f]->comp == this->Tfun->objfun[j]->exp_CN) && (this->experiments[i]->expphases[p]->phase == this->Tfun->objfun[j]->exp_phase ))
                                {
                                    this->experiments[i]->expphases[p]->phMR[f]->Qnt = scatter[count];
//                                    // check for unit
////                                    check_unit(i, p, e, Tfun->objfun[j]->exp_unit, this );
//                                    average = average + this->experiments[i]->expphases[p]->phMR[f]->Qnt;
//                                    if (this->experiments[i]->expphases[p]->phMR[f]->Qnt < min)
//                                        min = this->experiments[i]->expphases[p]->phMR[f]->Qnt;
//                                    residuals = residuals + residual_phase_elemMR (i, p, f, j, this);
                                    ++count;
                                }
                            }
                        } else

                        if ((Tfun->objfun[j]->exp_CT == keys::property) && (this->experiments[i]->expphases[p]->phprop.size() > 0) /*&& (this->Tfun->objfun[j]->exp_dcomp == "NULL")*/)
                        {
                        // loop trough all properties
                        for (unsigned int pp = 0; pp< this->experiments[i]->expphases[p]->phprop.size(); ++pp)
                        {
                            if ((this->experiments[i]->expphases[p]->phprop[pp]->property == Tfun->objfun[j]->exp_CN) && (this->experiments[i]->expphases[p]->phase == this->Tfun->objfun[j]->exp_phase ))
                            {
                                this->experiments[i]->expphases[p]->phprop[pp]->Qnt = scatter[count];
//                                // check for unit
//                                check_prop_unit(i, p, pp, Tfun->objfun[j]->exp_unit, this );
//                                average = average + this->experiments[i]->expphases[p]->phprop[pp]->Qnt;
//                                if (this->experiments[i]->expphases[p]->phprop[pp]->Qnt < min)
//                                    min = this->experiments[i]->expphases[p]->phprop[pp]->Qnt;
//                                residuals = residuals + residual_phase_prop (i, p, pp, j, this);
                                ++count;
                            }
                        }
                    } else
                        if (/*(Tfun->objfun[j]->exp_property !="NULL") &&*/ (this->experiments[i]->expphases[p]->phDC.size() > 0) && (Tfun->objfun[j]->exp_CT == keys::DC))
                        {
                            // loop trough all dependent components
                            for (unsigned int dc = 0; dc< this->experiments[i]->expphases[p]->phDC.size(); ++dc)
                            {
                                if ((this->experiments[i]->expphases[p]->phDC[dc]->DC == Tfun->objfun[j]->exp_CN) && (this->experiments[i]->expphases[p]->phase == this->Tfun->objfun[j]->exp_phase ))
                                {
                                    // loop trough all dep comp properties
                                    for (unsigned int dcp = 0; dcp < this->experiments[i]->expphases[p]->phDC[dc]->DCprop.size(); ++dcp)
                                    {
                                        if (this->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->property == Tfun->objfun[j]->exp_DCP)
                                        {
                                            this->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qnt = scatter[count];
////                                            cout << "yes"<<endl;
//                                            //                                    // check for unit
//                                            //                                    check_dcomp_unit(i, p, dc, dcp, sys->Tfun->objfun[j]->exp_unit, sys );
//                                            average = average + this->experiments[i]->expphases[p]->phDC[dc]->DCprop[dcp]->Qnt;
//                                            residuals = residuals + residual_phase_dcomp (i, p, dc, dcp, j, this);
                                            ++count;
                                        }
                                    }
                                }
                            }
                        }
                }
            }
        }
//    if (count > 0)
//    {
//        average = average / count;
//    }
//    Tfun->objfun[j]->meas_average = average;
//    this->minimum_value = min;
    average = 0.0;
    }
}


TGfitTask::~TGfitTask(   )
{
//   freeMemory();
}

void TGfitTask::set_residuals (double computed, double measured, double Weighted_Tfun_residual, double Tfun_residual, double weight )
{
    residuals_v.push_back(measured-computed);
    weights.push_back(weight);
    computed_values_v.push_back(computed);
    measured_values_v.push_back(measured);
    Weighted_Tfun_residuals_v.push_back(Weighted_Tfun_residual);
    Tfun_residuals_v.push_back(Tfun_residual);
}

void TGfitTask::test()
{
    mean_reisdulas = 0.0;
    for (int i=0; i<residuals_v.size(); i++)
    {
        mean_reisdulas +=residuals_v[i];
    }
    mean_reisdulas = mean_reisdulas / residuals_v.size();
}

//-----------------------End of gemsfit_task.cpp--------------------------

