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
#include <cmath>
#include "gemsfit_target_functions.h"


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
    MC_MPI = false;

    // nodes
    anNodes = experiments.size();

//    get_nodes(experiments.size());

    for (int j=0; j<anNodes; j++)
    {
        NodT.push_back( new TNode );
    }

    // initialize nodes with the experimental data
    fout << "8. gemsfit_task.cpp line 88. Initializing nodes with the experimental data; " << endl;
    setnodes ( );

    // getting the parameters to be optimized from DCH, DBR and multi structures, and optimization settings form the input file
    fout << "9. gemsfit_task.cpp line 92. Initializing optimization structure; " << endl;
    Opti = new optimization ( );

    fout << "12. gemsfit_task.cpp line 95. Initializing the Target function structure & get_DatTarget(); " << endl;
    Tfun = new TargetFunction;
    print = new ResPrint(printfile, Opti);
    get_DataTarget ( );

    get_logK_TPpairs ();


    // check for errors and inconsitencies of input options and parameters
    gfit_error ( );

    fout.close();

}

void TGfitTask::gfit_error ( )
{
    for (int i=0; i<Opti->opt.size(); ++i)
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

    fout << "13. gemsfit_task.cpp line 111. Initializing optimization init_optim; " << endl;
    init_optim (Opti->optv, weighted_Tfun_sum_of_residuals);

    fout.close();
}

void TGfitTask::get_logK_TPpairs()
{
    double DG = 0.0;
    const double Rln = -2.302585093*8.314472;
    double RTln = 0.0;
    // loop trough reactions
    for (int i = 0; i< this->Opti->reactions.size(); ++i)
    {
//        this->Opti->reactions[i]->logK_TPpairs.resize(this->TP_pairs[0].size());
        // loop trough TP
        for (int j = 0; j<this->TP_pairs[0].size(); ++j)
        {
            // loop trough rection species to calculated delta G of reaction
            for (int k = 0; k<this->Opti->reactions[i]->rdc_species.size(); ++k)
            {
                DG += this->NodT[0]->DC_G0(this->Opti->reactions[i]->rdc_species_ind[k], this->TP_pairs[1][j]*100000, this->TP_pairs[0][j]+273.15, false) * this->Opti->reactions[i]->rdc_species_coef[k];
            }
            RTln = Rln * this->TP_pairs[0][j]+273.15;
            this->Opti->reactions[i]->logK_TPpairs.push_back(DG/RTln);
            DG = 0.0;
        }
    }
}

void TGfitTask::get_DataTarget ( )
{
    vector<string> out;
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
        Tfun->objfun[i]->exp_elem = "NULL";
        Tfun->objfun[i]->exp_property = "NULL";
        Tfun->objfun[i]->exp_unit = "NULL";
        Tfun->objfun[i]->exp_dcomp ="NULL";

    }
    out.clear();
    int j=0;
    int jj=0;

    parse_JSON_array_object(DataTarget, keys::OFUN, keys::EPH, out);
    for (unsigned int i = 0 ; i < out.size() ; i++)
    {
        Tfun->objfun[i]->exp_phase = out[i];
        ++j;
    }
    out.clear();

    parse_JSON_array_object(DataTarget, keys::OFUN, keys::EN, out);
    for (unsigned int i = 0 ; i < out.size() ; i++)
    {
        Tfun->objfun[i]->exp_elem = out[i];
    }
    out.clear();

    parse_JSON_array_object(DataTarget, keys::OFUN, keys::Eunit, out);
    for (unsigned int i = 0 ; i < out.size() ; i++)
    {
        Tfun->objfun[i]->exp_unit = out[i];
    }
    out.clear();

    jj = j;

    parse_JSON_array_object(DataTarget, keys::OFUN, keys::PPH, out);
    for (unsigned int i = j ; i < out.size()+j ; i++)
    {
        Tfun->objfun[i]->exp_phase = out[i-j];
        ++jj;
    }
    out.clear();

    parse_JSON_array_object(DataTarget, keys::OFUN, keys::EP, out);
    for (unsigned int i = j ; i < out.size()+j ; i++)
    {
        Tfun->objfun[i]->exp_property = out[i-j];
    }
    out.clear();

    parse_JSON_array_object(DataTarget, keys::OFUN, keys::PEunit, out);
    for (unsigned int i = j ; i < out.size()+j ; i++)
    {
        Tfun->objfun[i]->exp_unit = out[i-j];
    }
    out.clear();


    parse_JSON_array_object(DataTarget, keys::OFUN, keys::DCPH, out);
    for (unsigned int i = jj ; i < out.size()+jj ; i++)
    {
        Tfun->objfun[i]->exp_phase = out[i-jj];
    }
    out.clear();

    parse_JSON_array_object(DataTarget, keys::OFUN, keys::DC, out);
    for (unsigned int i = jj ; i < out.size()+jj ; i++)
    {
        Tfun->objfun[i]->exp_dcomp = out[i-jj];
    }
    out.clear();

    parse_JSON_array_object(DataTarget, keys::OFUN, keys::DCP, out);
    for (unsigned int i = jj ; i < out.size()+jj ; i++)
    {
        Tfun->objfun[i]->exp_property = out[i-jj];
    }
    out.clear();

    parse_JSON_array_object(DataTarget, keys::OFUN, keys::DCPunit, out);
    for (unsigned int i = jj ; i < out.size()+jj ; i++)
    {
        Tfun->objfun[i]->exp_unit = out[i-jj];
    }
    out.clear();
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
void TGfitTask::build_optim( nlopt::opt &NLopti, std::vector<double> &optv_, /*std::vector<SS_System_Properties*> *ss_systems, int &countit,*/ double &weighted_Tfun_sum_of_residuals )
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
        Opti->normalize_params( optv_ );
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
    ffout << endl << "14. in gemsfit_task.cpp line 351. Setting minimizing objective function." << endl;
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

    // Only the Master process runs the optimization library. The other processes only run the callback function (copying the newly generated guess values from process 0).
    int ierr;
    //ierr = MPI_Comm_rank( MPI_COMM_WORLD, &pid );
    //ierr = MPI_Comm_size( MPI_COMM_WORLD, &p );
    int continue_or_exit;

    ffout << "15. in gesfit_task.cpp line 378. Performing optimization."<<endl;

//    //===== For testing the objective function without oprimization =====//
//    weighted_Tfun_sum_of_residuals = Equil_objective_function_callback(Opti->optv, grad, this);

    print->print_header(Tfun->type, Tfun->weight);

    nlopt::result result = NLopti.optimize( Opti->optv, weighted_Tfun_sum_of_residuals );
    ffout<<"optv[0] = "<<Opti->optv[0]<<endl;
    ffout<<"size of optv = "<<Opti->optv.size()<<endl;

    ffout << "16. gemsfit_task.cpp line 389. Finished optimization; " << endl;


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
//countit = master_counter;
}


void TGfitTask::setnodes()
{
    int n, i, j;
    // DATACH structure content
    int nIC, nDC, nPH, ICndx, PHndx;
    long int NodeStatusCH, NodeHandle;
    double P_pa, T_k;
    double* new_moles_IC;
    double* xDC_up;
    double* xDC_lo;
    double* Ph_surf;

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


    // initialize the nodes with the input GEMS3 file
    for (n=0; n<NodT.size(); ++n)
    {
        // Getting direct access to work node DATABR structure which exchanges the
        // data with GEMS3K (already filled out by reading the DBR input file)
        DATABR* dBR = NodT[n]->pCNode();
        DATACH* dCH = NodT[n]->pCSD();

        nIC = dCH->nIC;	// nr of independent components
        nDC = dCH->nDC;	// nr of dependent components
        nPH = dCH->nPH;
        xDC_up = new double[ nDC ];
        xDC_lo = new double[ nDC ];
        Ph_surf = new double[ nPH ];
        new_moles_IC = new double [ nIC ]; // vector for holding the moles of independent components for each experiment

        // lower and upper bounds for concentration of DC
        for( i=0; i<nDC; i++ )
        {
            xDC_up[ i ]  = 1000000.;
            xDC_lo[ i ]  = 0.;
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
        if (NodT[n]->IC_name_to_xDB("Nit") > -1) {
            ICndx = NodT[n]->IC_name_to_xDB("Nit");
            new_moles_IC[ICndx]=4.12434406;
        }

        // Set amount of dependent components (GEMS3K: DBR indexing)
        // go trough all acomponents and calculate the mole amounts of the IC for the b vector in GEMS
        for (j=0; j<experiments[n]->sbcomp.size(); ++j)
        {
            if (experiments[n]->sbcomp[j]->comp == "H2O")
            {
                ICndx = NodT[n]->IC_name_to_xDB("H");
                new_moles_IC[ICndx] += 2*experiments[n]->sbcomp[j]->bQnt/18.01528 /*+  2.123456*experiments[n]->sbcomp[j]->bQnt/1000*1e-05*/; // adds 1e-05 moles of H2 for each kg og H2O
                if (NodT[n]->IC_name_to_xDB("Nit") < 0)
                {
                    new_moles_IC[ICndx] += 2.123456*experiments[n]->sbcomp[j]->bQnt/1000*1e-05;
                }
                // cout << new_moles_IC[ICndx] << endl;
                ICndx = NodT[n]->IC_name_to_xDB("O");
                new_moles_IC[ICndx] +=  experiments[n]->sbcomp[j]->bQnt/18.01528 /*+ 3*experiments[n]->sbcomp[j]->bQnt/1000*1e-03*/;
            }
            else if (experiments[n]->sbcomp[j]->comp == "SiO2")
            {
                ICndx = NodT[n]->IC_name_to_xDB("Si");
                new_moles_IC[ICndx] +=  experiments[n]->sbcomp[j]->bQnt/60.0843;
               // cout << new_moles_IC[ICndx]<<endl;
                ICndx = NodT[n]->IC_name_to_xDB("O");
                new_moles_IC[ICndx] +=  2*experiments[n]->sbcomp[j]->bQnt/60.0843;
            }
            else if (experiments[n]->sbcomp[j]->comp == "Al2O3")
            {
                ICndx = NodT[n]->IC_name_to_xDB("Al");
                new_moles_IC[ICndx] +=  2*experiments[n]->sbcomp[j]->bQnt/101.9612772;
                ICndx = NodT[n]->IC_name_to_xDB("O");
                new_moles_IC[ICndx] +=  3*experiments[n]->sbcomp[j]->bQnt/101.9612772;
            }
            else if (experiments[n]->sbcomp[j]->comp == "Al(OH)3")
            {
                ICndx = NodT[n]->IC_name_to_xDB("Al");
                new_moles_IC[ICndx] +=  1*experiments[n]->sbcomp[j]->bQnt/78.0035586;
                ICndx = NodT[n]->IC_name_to_xDB("H");
                new_moles_IC[ICndx] +=  3*experiments[n]->sbcomp[j]->bQnt/78.0035586;
                ICndx = NodT[n]->IC_name_to_xDB("O");
                new_moles_IC[ICndx] +=  3*experiments[n]->sbcomp[j]->bQnt/78.0035586;
            }
            else if (experiments[n]->sbcomp[j]->comp == "Ca(OH)2")
            {
                ICndx = NodT[n]->IC_name_to_xDB("Ca");
                new_moles_IC[ICndx] +=  1*experiments[n]->sbcomp[j]->bQnt/74.09268;
               // cout << new_moles_IC[ICndx]<<endl;
                ICndx = NodT[n]->IC_name_to_xDB("H");
                new_moles_IC[ICndx] +=  2*experiments[n]->sbcomp[j]->bQnt/74.09268;
               // cout << new_moles_IC[ICndx]<<endl;
                ICndx = NodT[n]->IC_name_to_xDB("O");
                //cout << new_moles_IC[ICndx]<<endl;
                new_moles_IC[ICndx] +=  2*experiments[n]->sbcomp[j]->bQnt/74.09268;
            }
            else if (experiments[n]->sbcomp[j]->comp == "NaOH")
            {
                ICndx = NodT[n]->IC_name_to_xDB("Na");
                new_moles_IC[ICndx] +=  1*experiments[n]->sbcomp[j]->bQnt/39.99710928;
                ICndx = NodT[n]->IC_name_to_xDB("H");
                new_moles_IC[ICndx] +=  1*experiments[n]->sbcomp[j]->bQnt/39.99710928;
                ICndx = NodT[n]->IC_name_to_xDB("O");
                new_moles_IC[ICndx] +=  1*experiments[n]->sbcomp[j]->bQnt/39.99710928;
            }
            else if (experiments[n]->sbcomp[j]->comp == "KOH")
            {
                ICndx = NodT[n]->IC_name_to_xDB("K");
                new_moles_IC[ICndx] +=  1*experiments[n]->sbcomp[j]->bQnt/56.10564;
                ICndx = NodT[n]->IC_name_to_xDB("H");
                new_moles_IC[ICndx] +=  1*experiments[n]->sbcomp[j]->bQnt/56.10564;
                ICndx = NodT[n]->IC_name_to_xDB("O");
                new_moles_IC[ICndx] +=  1*experiments[n]->sbcomp[j]->bQnt/56.10564;
            }
            else if (experiments[n]->sbcomp[j]->comp == "CO2")
            {
                ICndx = NodT[n]->IC_name_to_xDB("C");
                new_moles_IC[ICndx] +=  1*experiments[n]->sbcomp[j]->bQnt/44.0095;
                ICndx = NodT[n]->IC_name_to_xDB("O");
                new_moles_IC[ICndx] +=  2*experiments[n]->sbcomp[j]->bQnt/44.0095;
            }
            else if (experiments[n]->sbcomp[j]->comp == "NaCl")
            {
                ICndx = NodT[n]->IC_name_to_xDB("Na");
                new_moles_IC[ICndx] +=  experiments[n]->sbcomp[j]->bQnt;
                ICndx = NodT[n]->IC_name_to_xDB("Cl");
                new_moles_IC[ICndx] +=  experiments[n]->sbcomp[j]->bQnt;
            }
                else
                {
                    cout<<" Unknown component in gemsfit_global_function.cpp line 343 !!!! "<<endl;
                    cout<<" ... bail out now ... "<<endl;
                    exit(1);
                }
        }

        // ---- // ---- // Set temperature and pressure // ---- // ---- //
        P_pa = 100000 * experiments[n]->sP;
        T_k = 273.15 + experiments[n]->sT;

        // ---- // ---- // set the new amount of IC and T & P from experiment i // ---- // ---- //
        // in the future - implement a Tnode function that stes just T, P and bIC vector of amount of independent components.
        // ---- // ---- // Transfer new temperature, pressure and b-vector to GEMS3K // ---- // ---- //
        NodT[n]->GEM_from_MT( NodeHandle, NodeStatusCH, T_k, P_pa, 0., 0., new_moles_IC, xDC_up, xDC_lo, Ph_surf );

        delete[] new_moles_IC;
        delete[] xDC_up;
        delete[] xDC_lo;
        delete[] Ph_surf;
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

//-----------------------End of gemsfit_task.cpp--------------------------

