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
//#include "gemsfit_nested_functions.h"
#include <cmath>
#include "gemsfit_target_functions.h"
#include "gemsfit_nested_functions.h"
#include <iomanip>
#include "s_solmod.h"
#include "s_formula.h"
#include <memory>


#ifndef __unix
#include <io.h>
#endif


using namespace std;

int master_counter;

istream& f_getline(istream& is, string& str, char delim);

//extern outField DataBR_fields[58];

TGfitTask* TGfitTask::gft;


//< Constructors for 1D arrangement of nodes
TGfitTask::TGfitTask(  )/*: anNodes(nNod)*/
{
    weighted_Tfun_sum_of_residuals = 0.0;
    for ( int i=1; i <MPI+1; i++)
    {
        EXPndx.push_back(-1);COMPndx.push_back(-1);PHndx.push_back(-1);PHPndx.push_back(-1);
        iNa.push_back(0.0); iO.push_back(0.0); iH.push_back(0.0); iCl.push_back(0.0); NEFndx.push_back(-1);
        vPAndx.push_back( new vect);
    }
    h_grad = false;

    // file containing the input parameters of the system and of the optimization class
    param_file  = gpf->OptParamFile().c_str();

    // For parameter optimization do not use printing of results
    resultsfile = gpf->FITFile();

    // nodes
    anNodes = experiments.size();

    for (int j=0; j<anNodes; j++)
    {
        NodT.push_back( new TNode );
    }

    // initialize nodes with the experimental data
    gpf->flog << "06. gemsfit_task.cpp(89). Initializing nodes with the experimental data; " << endl;
    setnodes ( );  // initialization of nodes each for one experimental point (system)
    // getting the parameters to be optimized from DCH, DBR and multi structures, and optimization settings form the input file
    gpf->flog << "07. gemsfit_task.cpp(92). Initializing optimization structure; " << endl;
    Opti = new optimization ( );
    gpf->flog << "10. gemsfit_task.cpp(94). Initializing the Target function structure & get_DatTarget(); " << endl;
    Tfun = new TargetFunction;

    get_DataTarget ( );

    for (unsigned int i=0; i < experiments.size(); i++)
    {
        aTfun.push_back(*Tfun);
    }

    /// function in iofiles.cpp to read the logK lookup array instead of get function!
    get_logK_TPpairs ();
    get_Lparams_delta ();

    // check for errors and inconsitencies of input options and parameters
    gfit_error ( );

    number_of_residuals = get_number_of_residuals( );
    set_average_objfun ();

    set_weights();

    if (this->LimitOfDetection > (this->minimum_value/100))
        this->LimitOfDetection = this->minimum_value/100; // sets the limit of detection not more than 100 times smaller than the lowest experimental value

}

void TGfitTask::gfit_error ( )
{
    bool h_nestfun = false, h_param_nestfun = false;
    if (Tfun->nestfun.size() > 0)
    {
        h_nestfun = true;

    for (unsigned int i=0; i<Opti->nest_optv.opt.size(); ++i)
    {
        if ((Opti->nest_optv.Ptype[i] == "bIC") || (Opti->nest_optv.Ptype[i] == "TK") || (Opti->nest_optv.Ptype[i] == "P"))  h_param_nestfun = true;
    }

    if (!(h_nestfun == h_param_nestfun))
    {
        if (!h_param_nestfun)
        {
            cout << "Error - When using nested functions (fitting paramaters in each system node/experiment) paramaters such as bIC,  TK or P have to be marked ";
            exit(1);
        }
        if (!h_nestfun)
        {
            cout << "Error - When fitting bIC, TK or P nested functions have to be added to DataTarget, which tell the program which is the dependent value from each node/experiment ";
            exit(1);
        }
    }
    }
}

void TGfitTask::run_optim()
{
//    titration(this);
    gpf->flog << "11. gemsfit_task.cpp(152). Initializing optimization init_optim(); " << endl;
    if (Tfun->objfun.size() > 0)
    init_optim (Opti->optv, weighted_Tfun_sum_of_residuals);
    else
    {
        if (Opti->h_nestfun)
        {
            string old = Tfun->type;               // storing the old type of target function
            Tfun->type = "abs_dif";                // seeting the target function to simple abslute difference
            nestedfun(this);                             // optimizing the nested functions
            Tfun->type = old;
        }
    }
}

void TGfitTask::Ainit_optim (std::vector<double> &optv_ /*,int &countit, double &weighted_Tfun_sum_of_residual */)
{
    init_optim( optv_, weighted_Tfun_sum_of_residuals );
}

// Initialize optimization object and Run Optimization by calling build_optim
void TGfitTask::init_optim( std::vector<double> &optv_, /*int &countit,*/ double &weighted_Tfun_sum_of_residuals )
{
    // Instantiate nlopt::opt object
    if( Opti->OptAlgo.compare( "LN_COBYLA" ) == 0 )
    {
        nlopt::opt nlopti_( nlopt::LN_COBYLA, optv_.size() );
        build_optim( nlopti_, optv_, weighted_Tfun_sum_of_residuals );
    }
    else if( Opti->OptAlgo.compare( "GN_ISRES" ) == 0 )
    {
        nlopt::opt nlopti_( nlopt::GN_ISRES, optv_.size() );
        build_optim( nlopti_, optv_, weighted_Tfun_sum_of_residuals );
    }
    else if( Opti->OptAlgo.compare( "LN_BOBYQA" ) == 0 )
    {
        nlopt::opt nlopti_( nlopt::LN_BOBYQA, optv_.size() );
        build_optim( nlopti_, optv_, weighted_Tfun_sum_of_residuals );
    }
    else if( Opti->OptAlgo.compare( "GN_ORIG_DIRECT" ) == 0 )
    {
        nlopt::opt nlopti_( nlopt::GN_ORIG_DIRECT, optv_.size() );
        build_optim( nlopti_, optv_, weighted_Tfun_sum_of_residuals );
    }
    else if( Opti->OptAlgo.compare( "GN_ORIG_DIRECT_L" ) == 0 )
    {
        nlopt::opt nlopti_( nlopt::GN_ORIG_DIRECT_L, optv_.size() );
        build_optim( nlopti_, optv_, weighted_Tfun_sum_of_residuals );
    }
    else if( Opti->OptAlgo.compare( "LD_MMA" ) == 0 )
    {
        nlopt::opt nlopti_( nlopt::LD_MMA, optv_.size() );
        build_optim( nlopti_, optv_, weighted_Tfun_sum_of_residuals );
    }
    else if( Opti->OptAlgo.compare( "LD_SLSQP" ) == 0 )
    {
        nlopt::opt nlopti_( nlopt::LD_SLSQP, optv_.size() );
        build_optim( nlopti_, optv_, weighted_Tfun_sum_of_residuals );
    }
    else if( Opti->OptAlgo.compare( "GD_MLSL" ) == 0 )
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

    gpf->flog << " ... initializing optimization object in build_optim() ... " << endl;

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

    gpf->flog << "... assigning bounds and tolerance for optimization..." << endl;
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
    gpf->flog << endl << "12. in gemsfit_task.cpp(287). Setting target (objective) function to minimize." << endl;
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

    gpf->flog << "13. gemsfit_task.cpp(308). Performing optimization."<<endl;

//    //===== For testing the objective function without oprimization =====//
//    weighted_Tfun_sum_of_residuals = Equil_objective_function_callback(Opti->optv, grad, this);

    nlopt::result result = NLopti.optimize( Opti->optv, weighted_Tfun_sum_of_residuals );
    gpf->flog << "optv[0] = "<<Opti->optv[0]<<endl;
    gpf->flog << "size of optv = "<<Opti->optv.size()<<endl;

    gpf->flog << "14. gemsfit_task.cpp(317). Finished optimization; " << endl;

    Equil_objective_function_callback(Opti->optv, grad, this);

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
        gpf->flog<<"found minimum at <<f( ";
        for( unsigned i=0; i<Opti->optv.size(); i++ )
        {
            gpf->flog<<Opti->optv[i]<<" ";
        }
        gpf->flog<<") = "<<weighted_Tfun_sum_of_residuals<<std::endl;
        gpf->flog<<" after "<< master_counter <<" evaluations."<<std::endl;

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
}

void TGfitTask::setnodes()
{
    unsigned int n, i, j;
    // DATACH structure content
    unsigned int nIC, nDC, nPH, ICndx, DCndx/*, PHndx*/;
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
            cout<< gpf->GEMS3LstFilePath().c_str() << endl;
            cout<<" .. ERROR occurred while reading input files !!! ..."<<endl;
        }
    }

    // initialize the nodes using the input GEMS3 file
    for (n=0; n<NodT.size(); ++n)
    {
        // Getting direct access to work node DATABR structure which exchanges the
        // data with GEMS3K (already filled out by reading the DBR input file)
//        DATABR* dBR = NodT[n]->pCNode();
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

        // Surface areas of phases -> kinetics
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
            long int nICb, /*nDCb,*/ xDCb;
            char cName[2*IC_RKLEN+1];
            double DCm, ICm, njDC;

        try{
            nICb = dCH->nICb;
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
                // This is not a name of DC used in DBR file. The GEMS formula parser is used
                //  (implemented by SD on 4.03.2014)
                TFormula aFo;
                auto_ptr<double> A( new double[nIC]);
                auto_ptr<char> SB1( new char[nIC*IC_RKLEN]);

                // Set up SB1 - copy of ICNL
                for( i=0; i<nIC; i++ )
                {
                    memset( cName, ' ', IC_RKLEN );
                    strncpy( cName, dCH->ICNL[i], strlen(dCH->ICNL[i]) /*MaxICN*/ );
                    cName[IC_RKLEN] = 0;
                    aFo.fixup_ics( cName );
                    memcpy( SB1.get()+i*IC_RKLEN, cName, MAXICNAME+MAXSYMB );
                }
                // Get the formula of the composition (e.g. SiO2)
                strcpy( cName, experiments[n]->sbcomp[j]->comp.c_str() );
                aFo.SetFormula( cName );
                aFo.Stm_line( nIC, A.get(), SB1.get(), NULL );

                if (experiments[n]->sbcomp[j]->Qunit == keys::molal)
                    {   // conversion from molal to mole
    //                    experiments[n]->sbcomp[j]->Qnt = experiments[n]->sbcomp[j]->Qnt*h2o_kgamount;
    //                    experiments[n]->sbcomp[j]->Qunit = keys::mole; // this is not good!
                        njDC = experiments[n]->sbcomp[j]->Qnt * h2o_kgamount;
                    }
                else
                        njDC = experiments[n]->sbcomp[j]->Qnt;
                DCm = 0.;  // zero off molar mass of the formula
                for(i=0; i<nIC; i++)
                    {   // loop over ICs
                        double aij;
                        // Retrieves the stoichiometry coefficient a[xdc][xic] of IC in the formula of DC.
                        aij = A.get()[i];
                        if( !aij )
                           continue;
//                        new_moles_IC[i] += njDC * aij;
                        ICm = NodT[n]->ICmm( i )*1000.; // Retrieves the molar mass of IC in g/mol.
                        DCm += ICm * aij;
                    }
                if(  experiments[n]->sbcomp[j]->Qunit == keys::gram )
                    {   // conversion from grams to moles
                        njDC /= DCm;
                    }
                for(i=0; i<nIC; i++)  // Adding to bulk composition of the system
                    {   // loop over ICs
                        double aij;
                        aij = A.get()[i];
                        if( !aij )
                           continue;
                        new_moles_IC[i] += njDC * aij;
                    }
           }
     } catch( TError& err )
     {
       cout << "Error in setting the input composition of experimental system:" << endl;
       cout << "expsample: " << experiments[n]->sample.c_str() << " expdataset: " <<
            experiments[n]->expdataset.c_str() << " formula: "
            <<   experiments[n]->sbcomp[j]->comp.c_str() << endl;
       cout<< err.title << err.mess << endl;
       exit(1);
     }
        } // for j
//        if (!salt)
//        {
//            // use NaCl
//            NodT[n]->Set_PMc(0.064, 0 );
//            NodT[n]->Set_PMc(3.72, 1 );
//            NodT[n]->Set_PMc(1, 4 );
//        }
//        salt = false;
        h2o_kgamount = 0.0;

        // ---- // ---- // Set temperature and pressure // ---- // ---- //
        P_pa = 100000 * experiments[n]->sP;
        T_k = 273.15 + experiments[n]->sT;
        NodeHandle = n;
        // ---- // ---- // set the new amount of IC and T & P from experiment i // ---- // ---- //
        // in the future - implement a Tnode function that stes just T, P and bIC vector of amount of independent components.
        // ---- // ---- // Transfer new temperature, pressure and b-vector to GEMS3K // ---- // ---- //
//        NodT[n]->GEM_from_MT( NodeHandle, NodeStatusCH, T_k, P_pa, 0., 0., new_moles_IC, xDC_up, xDC_lo, Ph_surf );

        // variant (8c) of GEM_from_MT()
        NodT[n]->GEM_from_MT( NodeHandle, NEED_GEM_AIA, T_k, P_pa, new_moles_IC, xDC_up, xDC_lo );  // bugfix DK 09.01.2014

        // Calling GEMIPM calculation
        NodeStatusCH = NodT[n]->GEM_run( true );
cout << "Node: " << NodeHandle << "  NodeStatusCH: " << NodeStatusCH << endl;

        if( ( NodeStatusCH == ERR_GEM_AIA || NodeStatusCH == ERR_GEM_SIA ||
                       NodeStatusCH ==  T_ERROR_GEM ) )
        {
             cout << "Error: GEM calculation results are not retrieved upon initializing experimental system (node) "
                  << NodeHandle << endl;
        }
        else
        {
           if( ( NodeStatusCH == BAD_GEM_AIA || NodeStatusCH == BAD_GEM_SIA  ) )
           {
              cout << "Insufficient quality of GEM solution, but GEM results are retrieved upon initializing experimental system (node) "
              << NodeHandle << endl;
           }
        }
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
        TargetFunction::obj_fun objfun;
        Tfun->objfun.push_back(objfun); // initializing
        Tfun->objfun[i].exp_phase = "NULL";
        Tfun->objfun[i].exp_CT = "NULL";
        Tfun->objfun[i].exp_CN = "NULL";
        Tfun->objfun[i].exp_unit = "NULL";
        Tfun->objfun[i].exp_DCP = "NULL";
        Tfun->objfun[i].meas_average = 0.0;
        Tfun->objfun[i].TuWeight = 1;
        Tfun->objfun[i].weight = 1;
        Tfun->objfun[i].isComputed = false;

        parse_JSON_object(out[i], keys::EPH, out2);
        if (out2.size() == 0) { cout << "Phase name has to be specified in Data Target->OFUN->EPH!"<< endl; exit(1);} // ERROR
        Tfun->objfun[i].exp_phase = out2[0];
        out2.clear();

        parse_JSON_object(out[i], keys::CT, out2);
        if (out2.size() == 0) { cout << "Type of compared property has to be specified in Data Target->OFUN->CT!"<< endl; exit(1);} // ERROR
        Tfun->objfun[i].exp_CT = out2[0];
        out2.clear();

        parse_JSON_object(out[i], keys::DCP, out2);
        if ((out2.size() == 0) && (Tfun->objfun[i].exp_CT == keys::DC)) { cout << "Name of dependent component compared property has to be specified in Data Target->OFUN->DCP!"<< endl; exit(1);} // ERROR
        if (out2.size() > 0)
        Tfun->objfun[i].exp_DCP = out2[0];
        out2.clear();

        parse_JSON_object(out[i], keys::CN, out2);
        if (out2.size() == 0) { cout << "Data Target->OFUN->CN has to be speficied!"<< endl; exit(1);} // ERROR
        Tfun->objfun[i].exp_CN = out2[0];
        out2.clear();

        parse_JSON_object(out[i], keys::Qunit, out2);
        if (out2.size() > 0)
        Tfun->objfun[i].exp_unit = out2[0];
        out2.clear();

        parse_JSON_object(out[i], keys::WT, out2);
        if (out2.size() > 0)
        Tfun->objfun[i].weight = atof(out2[0].c_str());
        out2.clear();



    }
    out.clear();

    // get DFUN
    parse_JSON_object(DataTarget, keys::NFUN, out);
    for (unsigned int i = 0 ; i < out.size() ; i++)
    {
        Opti->h_nestfun = true;
        TargetFunction::obj_fun objfun;
        Tfun->nestfun.push_back(objfun); // initializing
        Tfun->nestfun[i].exp_phase = "NULL";
        Tfun->nestfun[i].exp_CT = "NULL";
        Tfun->nestfun[i].exp_CN = "NULL";
        Tfun->nestfun[i].exp_unit = "NULL";
        Tfun->nestfun[i].exp_DCP = "NULL";
        Tfun->nestfun[i].Ptype = "NULL";
        Tfun->nestfun[i].weight = 1;
        Tfun->nestfun[i].TuWeight = 1;
        Tfun->nestfun[i].isComputed = false;

        parse_JSON_object(out[i], keys::EPH, out2);
        if (out2.size() == 0) { cout << "Phase name has to be specified in Data Target->NFUN->EPH!"<< endl; exit(1);} // ERROR
        Tfun->nestfun[i].exp_phase = out2[0];
        out2.clear();

        parse_JSON_object(out[i], keys::CT, out2);
        if (out2.size() == 0) { cout << "Type of compared property has to be specified in Data Target->NFUN->CT!"<< endl; exit(1);} // ERROR
        Tfun->nestfun[i].exp_CT = out2[0];
        out2.clear();

        parse_JSON_object(out[i], keys::DCP, out2);
        if ((out2.size() == 0) && (Tfun->objfun[i].exp_CT == keys::DC)) { cout << "Name of dependent component compared property has to be specified in Data Target->NFUN->DCP!"<< endl; exit(1);} // ERROR
        if (out2.size() > 0)
        Tfun->nestfun[i].exp_DCP = out2[0];
        out2.clear();

        parse_JSON_object(out[i], keys::CN, out2);
        if (out2.size() == 0) { cout << "Data Target->OFUN->CN has to be speficied!"<< endl; exit(1);} // ERROR
        Tfun->nestfun[i].exp_CN = out2[0];
        out2.clear();

        parse_JSON_object(out[i], keys::Qunit, out2);
        if (out2.size() > 0)
        Tfun->nestfun[i].exp_unit = out2[0];
        out2.clear();

        parse_JSON_object(out[i], keys::WT, out2);
        if (out2.size() > 0)
        Tfun->nestfun[i].weight = atof(out2[0].c_str());
        out2.clear();

        parse_JSON_object(out[i], keys::Telem, out2);
        if (out2.size() > 0)
        Tfun->nestfun[i].Telem = out2;
        out2.clear();

        parse_JSON_object(out[i], keys::Tforumla, out2);
        if (out2.size() > 0)
        Tfun->nestfun[i].Tformula = out2;
        out2.clear();

        parse_JSON_object(out[i], keys::Ptype, out2);
        if (out2.size() == 0) { cout << "Data Target->NFUN->Ptype has to be speficied!"<< endl; exit(1);} // ERROR
        Tfun->nestfun[i].Ptype = out2[0];
        out2.clear();
    }
    out.clear();


    // get DFUN
    parse_JSON_object(DataTarget, keys::ADDOUT, out);
    for (unsigned int i = 0 ; i < out.size() ; i++)
    {
        TargetFunction::obj_fun addout;
        Tfun->addout.push_back(addout); // initializing
        Tfun->addout[i].exp_phase = "NULL";
        Tfun->addout[i].exp_CT = "NULL";
        Tfun->addout[i].exp_CN = "NULL";
        Tfun->addout[i].exp_unit = "NULL";
        Tfun->addout[i].exp_DCP = "NULL";
        Tfun->addout[i].Otype = "NULL";
        Tfun->addout[i].weight = 1;
        Tfun->addout[i].TuWeight = 1;
        Tfun->addout[i].isComputed = false;

        parse_JSON_object(out[i], keys::EPH, out2);
        if (out2.size() == 0) { cout << "Phase name has to be specified in Data Target->NFUN->EPH!"<< endl; exit(1);} // ERROR
        Tfun->addout[i].exp_phase = out2[0];
        out2.clear();

        parse_JSON_object(out[i], keys::CT, out2);
        if (out2.size() == 0) { cout << "Type of compared property has to be specified in Data Target->NFUN->CT!"<< endl; exit(1);} // ERROR
        Tfun->addout[i].exp_CT = out2[0];
        out2.clear();

        parse_JSON_object(out[i], keys::DCP, out2);
        if ((out2.size() == 0) && (Tfun->objfun[i].exp_CT == keys::DC)) { cout << "Name of dependent component compared property has to be specified in Data Target->NFUN->DCP!"<< endl; exit(1);} // ERROR
        if (out2.size() > 0)
        Tfun->addout[i].exp_DCP = out2[0];
        out2.clear();

        parse_JSON_object(out[i], keys::CN, out2);
        if (out2.size() == 0) { cout << "Data Target->OFUN->CN has to be speficied!"<< endl; exit(1);} // ERROR
        Tfun->addout[i].exp_CN = out2[0];
        out2.clear();

        parse_JSON_object(out[i], keys::Qunit, out2);
        if (out2.size() > 0)
        Tfun->addout[i].exp_unit = out2[0];
        out2.clear();

        parse_JSON_object(out[i], keys::WT, out2);
        if (out2.size() > 0)
        Tfun->addout[i].weight = atof(out2[0].c_str());
        out2.clear();

        parse_JSON_object(out[i], keys::Telem, out2);
        if (out2.size() > 0)
        Tfun->addout[i].Telem = out2;
        out2.clear();

        parse_JSON_object(out[i], keys::Tforumla, out2);
        if (out2.size() > 0)
        Tfun->addout[i].Tformula = out2;
        out2.clear();

        parse_JSON_object(out[i], keys::SRC, out2);
        if (out2.size() == 0) { cout << "Data Target->ADDOUT->SRC has to be speficied!"<< endl; exit(1);} // ERROR
        Tfun->addout[i].Otype = out2[0];
        out2.clear();
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

void TGfitTask::get_Lparams_delta()
{
    double delta;
    for (unsigned int i=0; i < Opti->nest_optv.Lparams.size(); ++i )
    {
        if (Opti->nest_optv.Ptype[i] == "bIC")
            for (unsigned int e=0; e < experiments.size(); e++ )
            {
                delta=0.0;
                for (unsigned int j=0; j < Opti->nest_optv.Lparams[i]->L_param.size(); ++j )
                {
                    delta += Opti->nest_optv.Lparams[i]->L_coef[j] * NodT[e]->Get_bIC(Opti->nest_optv.Lparams[i]->L_param_ind[j]);
                }
                Opti->nest_optv.Lparams[i]->delta.push_back(delta);
                Opti->nest_optv.Lparams[i]->i_val.push_back(NodT[e]->Get_bIC(Opti->nest_optv.Lparams[i]->index));
                Opti->nest_optv.Lparams[i]->e_val.push_back(NodT[e]->Get_bIC(Opti->nest_optv.Lparams[i]->index));
            }
    }

    // stores the initial values of the linked parameter bIC, ...
    for (unsigned int i=0; i < Opti->nest_optv.Ptype.size(); ++i )
    {
        Opti->nest_optv.i_opt.push_back(new optimization::nested::ival);
        Opti->nest_optv.e_opt.push_back(new optimization::nested::ival);
        if (Opti->nest_optv.Ptype[i] == "bIC")
            for (unsigned int e=0; e < experiments.size(); e++ )
            {
                Opti->nest_optv.i_opt[i]->val.push_back(NodT[e]->Get_bIC(Opti->nest_optv.Pindex[i]));
                Opti->nest_optv.e_opt[i]->val.push_back(NodT[e]->Get_bIC(Opti->nest_optv.Pindex[i]));
            }
        else
        if (Opti->nest_optv.Ptype[i] == "TK")
            for (unsigned int e=0; e < experiments.size(); e++ )
            {
                Opti->nest_optv.i_opt[i]->val.push_back(NodT[e]->Get_TK( ));
                Opti->nest_optv.e_opt[i]->val.push_back(NodT[e]->Get_TK( ));
            }
        else
        if (Opti->nest_optv.Ptype[i] == "P")
            for (unsigned int e=0; e < experiments.size(); e++ )
            {
                Opti->nest_optv.i_opt[i]->val.push_back(NodT[e]->Get_P( ));
                Opti->nest_optv.e_opt[i]->val.push_back(NodT[e]->Get_P( ));
            }
    }
}

TGfitTask::~TGfitTask(   )
{
//   freeMemory();
}


void TGfitTask::test()
{
    mean_reisdulas = 0.0;
    for (unsigned int i=0; i<residuals_v.size(); i++)
    {
        mean_reisdulas +=residuals_v[i];
    }
    mean_reisdulas = mean_reisdulas / residuals_v.size();
}

//-----------------------End of gemsfit_task.cpp--------------------------

