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
// GEMSIFT is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.

// GEMSFIT is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with GEMSFIT code. If not, see <http://www.gnu.org/licenses/>.
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

extern outField DataBR_fields[58];

TGfitTask* TGfitTask::gft;

//-------------------------------------------------------------------------
// RunGEM()
// GEM IPM calculation of equilibrium state for the iNode node
// from array NodT1. abs(Mode)) - mode of GEMS calculation (NEED_GEM_SIA or NEED_GEM_AIA)
//    if Mode is negative then the loading of primal solution from the node is forced
//    (only in SIA mode)
//  Function returns: NodeStatus code after GEM calculation
//   ( OK_GEM_AIA; OK_GEM_SIA; error codes )
//
//-------------------------------------------------------------------

long int  TGfitTask::RunGEM( long int  iNode, long int Mode )
{

bool uPrimalSol = false;
long int retCode;

  if( Mode < 0 || abs(Mode) == NEED_GEM_SIA )
	  uPrimalSol = true;
	  
// Copy data from the iNode node from array NodT0 to the work DATABR structure
   CopyWorkNodeFromArray( iNode, anNodes, NodT0 );

// GEM IPM calculation of equilibrium state in MULTI
  pCNode()->NodeStatusCH = abs(Mode);
  retCode = GEM_run( uPrimalSol );

// Copying data for node iNode back from work DATABR structure into the node array
//   if( retCode == OK_GEM_AIA ||
//       retCode == OK_GEM_PIA  )
   MoveWorkNodeToArray( iNode, anNodes, NodT0 );

   return retCode;
}

void  TGfitTask::checkGfitSamples(
    long int i, long int* nodeTypes, const char*  datachbr_file )
{
 if(nodeTypes)
   for( long int ii=0; ii<anNodes; ii++)
     if(   nodeTypes[ii]<0 || nodeTypes[ii] >= i )
        {
	  cout << anNodes << " " << nodeTypes[ii] << " i = " << i<< endl;
	Error( datachbr_file,
          "GEM_init() error: Undefined fitting condition!" );
	}
}

//-------------------------------------------------------------------
// Initialization of TNodeArray data structures. Reads in the DBR text input files and
// copying data from work DATABR structure into the node array
// (as specified in nodeTypes array, ndx index of dataBR files in
//    the dbrfiles_lst_name list).
//
//-------------------------------------------------------------------
void  TGfitTask::InitGfitTask( const char *dbrfiles_lst_name,
                  long int *nodeTypes, bool getNodT1, bool binary_f  )
{
  int i;
  gstring datachbr_fn;

  gstring curPath = ""; //current reading file path

     gstring lst_in = dbrfiles_lst_name;
     gstring Path = "";

// Get path
#ifdef IPMGEMPLUGIN
#ifdef _WIN32
      size_t pos = lst_in.rfind("\\");// HS keep this on windows
#else
      size_t pos = lst_in.rfind("/"); // HS keep this on linux
#endif
#else
      size_t pos = lst_in.rfind("\\");
      if( pos == npos )
         pos = lst_in.rfind("/");
      else
         pos = max(pos, lst_in.rfind("/") );
#endif
      if( pos < npos )
      Path = lst_in.substr(0, pos+1);

//  open file stream for the file names list file
      fstream f_lst( lst_in.c_str(), ios::in );
      ErrorIf( !f_lst.good() , lst_in.c_str(), "Fileopen error");


// Prepare for reading DBR_DAT files
     i = 0;
     while( !f_lst.eof() )  // For all DBR_DAT files listed
     {

// Reading DBR_DAT file into work DATABR structure
         if( i )  // Comma only after the first DBR_DAT file!
            f_getline( f_lst, datachbr_fn, ',');
         else
            f_getline( f_lst, datachbr_fn, ' ');

         gstring dbr_file = Path + datachbr_fn;
         curPath = dbr_file;
         if( binary_f )
         {
             GemDataStream in_br(dbr_file, ios::in|ios::binary);
             databr_from_file(in_br);
          }
         else
          {   fstream in_br(dbr_file.c_str(), ios::in );
                 ErrorIf( !in_br.good() , datachbr_fn.c_str(),
                    "DBR_DAT fileopen error");
               databr_from_text_file(in_br);
          }
         curPath = "";
// Unpacking work DATABR structure into MULTI (GEM IPM work structure): uses DATACH
//    unpackDataBr();


// Copying data from work DATABR structure into the node array
// (as specified in nodeTypes array)
           setNodeArray( i, nodeTypes  );

          i++;
     }  // end while()

    ErrorIf( i==0, datachbr_fn.c_str(), "GEM_init() error: No DBR_DAT files read!" );
    checkGfitSamples( i, nodeTypes, datachbr_fn.c_str()  );
}

//-------------------------------------------------------------------
// setGfitTaskData()
// Copying data from work DATABR structure into the node array
// (as specified in nodeTypes array, ndx index of dataBR files in
//    the ipmfiles_lst_name list).
//
//-------------------------------------------------------------------
void  TGfitTask::setGfitTaskData( long int ndx, long int* nodeTypes  )
{
   for( long int ii=0; ii<anNodes; ii++)
            if(  (!nodeTypes && ndx==0) ||
              ( nodeTypes && (nodeTypes[ii] == ndx/*i+1*/ )) )
                  {    pCNode()->NodeHandle = ndx/*(i+1)*/;
                       MoveWorkNodeToArray(ii, anNodes, NodT0);
                       CopyWorkNodeFromArray(ii, anNodes,NodT0);
//                       MoveWorkNodeToArray(ii, anNodes, NodT1);
//                       CopyWorkNodeFromArray(ii, anNodes,NodT1);
                   }
}


//---------------------------------------------------------//

void TGfitTask::allocMemory()
{
	long int ii;

// alloc memory for data bidge structures
// did in constructor TNode::allocMemory();

// alloc memory for all nodes at current time point
    NodT0 = new  DATABRPTR[anNodes];
    for(  ii=0; ii<anNodes; ii++ )
//        NodT0[ii] = 0;
/*
// alloc memory for all nodes at previous time point
    NodT1 = new  DATABRPTR[anNodes];
    for(  ii=0; ii<anNodes; ii++ )
        NodT1[ii] = 0;
*/
// alloc memory for the work array of node types
    tcNode = new char[anNodes];
    for(  ii=0; ii<anNodes; ii++ )
        tcNode[ii] = normal;    
        
// alloc memory for the work array of IA indicators
    iaNode = new bool[anNodes];
    for(  ii=0; ii<anNodes; ii++ )
        iaNode[ii] = true;
// grid ?    
}

void TGfitTask::freeMemory()
{
	long int ii;

   if( anNodes )
   { if( NodT0 )
       for(  ii=0; ii<anNodes; ii++ )
        if( NodT0[ii] )
           NodT0[ii] = databr_free(NodT0[ii]);
     delete[]  NodT0;
     NodT0 = 0;

     for (ii =0; ii<NodT.size(); ++ii)
     {
         delete NodT[ii];
     }
/*
     if( NodT1 )
       for(  ii=0; ii<anNodes; ii++ )
        if( NodT1[ii] )
          NodT1[ii] = databr_free(NodT1[ii]);
     delete[]  NodT1;
     NodT1 = 0;
*/
   }

//  if( grid )
//     delete[] grid;
  if( tcNode)
     delete[] tcNode;
  if( iaNode )
	 delete[] iaNode; 
}

//< Constructors for 1D arrangement of nodes
TGfitTask::TGfitTask(  )/*: anNodes(nNod)*/
{
    sum_of_squares = 0.0;
    // GEMSFIT logfile
    //const char path[200] = "output_GEMSFIT/SS_GEMSFIT.log";
    ofstream fout;
    fout.open(gpf->FITLogFile().c_str(), ios::app);
    if( fout.fail() )
    { cout<<"Output fileopen error"<<endl; exit(1); }


    // file containing the input parameters of the system and of the optimization class
    param_file  = gpf->OptParamFile().c_str();

    // For parameter optimization do not use printing of results
    printfile = false;

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
    fout << "8. gemsfit_task.cpp line 300. Initializing nodes with the experimental data; " << endl;
    setnodes ( );

    // getting the parameters to be optimized from DCH, DBR and multi structures, and optimization settings form the input file
    fout << "9. gemsfit_task.cpp line 304. Initializing optimization structure; " << endl;
    Opti = new optimization ( );

    fout << "12. gemsfit_task.cpp line 307. Initializing the Target function structure & get_DatTarget(); " << endl;
    Tfun = new TargetFunction;
    get_DatTarget ( );

    fout << "13. gemsfit_task.cpp line 311. Initializing optimization init_optim; " << endl;
    init_optim (Opti->optv, sum_of_squares);

}

void TGfitTask::get_DatTarget ( )
{
    vector<string> out;
    parse_JSON_array_object(DatTarget, "OFUN", "OPH", out);
    out.clear();


    parse_JSON_object(DatTarget, Target, out);
    Tfun->name = out[0]; // Name of target function
    out.clear();

    parse_JSON_object(DatTarget, TT, out);
    Tfun->type = out[0]; // Type of the target function
    out.clear();

    parse_JSON_object(DatTarget, WT, out);
    Tfun->weight = out[0]; // Weight of target function
    out.clear();

    parse_JSON_object(DatTarget, OFUN, out);
    for (unsigned int i = 0 ; i < out.size() ; i++)
    {
        Tfun->objfun.push_back(new TargetFunction::obj_fun); // initializing
        Tfun->objfun[i]->exp_phase = "NULL";
        Tfun->objfun[i]->exp_elem = "NULL";
        Tfun->objfun[i]->exp_property = "NULL";
        Tfun->objfun[i]->exp_unit = "NULL";
    }
    out.clear();
    int j=0;

    parse_JSON_array_object(DatTarget, OFUN, EPH, out);
    for (unsigned int i = 0 ; i < out.size() ; i++)
    {
        Tfun->objfun[i]->exp_phase = out[i];
        ++j;
    }
    out.clear();

    parse_JSON_array_object(DatTarget, OFUN, EN, out);
    for (unsigned int i = 0 ; i < out.size() ; i++)
    {
        Tfun->objfun[i]->exp_elem = out[i];
    }
    out.clear();

    parse_JSON_array_object(DatTarget, OFUN, Eunit, out);
    for (unsigned int i = 0 ; i < out.size() ; i++)
    {
        Tfun->objfun[i]->exp_unit = out[i];
    }
    out.clear();

    parse_JSON_array_object(DatTarget, OFUN, PPH, out);
    for (unsigned int i = j ; i < out.size()+j ; i++)
    {
        Tfun->objfun[i]->exp_phase = out[i-j];
    }
    out.clear();

    parse_JSON_array_object(DatTarget, OFUN, EP, out);
    for (unsigned int i = j ; i < out.size()+j ; i++)
    {
        Tfun->objfun[i]->exp_property = out[i-j];
    }
    out.clear();

    parse_JSON_array_object(DatTarget, OFUN, PEunit, out);
    for (unsigned int i = j ; i < out.size()+j ; i++)
    {
        Tfun->objfun[i]->exp_unit = out[i-j];
    }
    out.clear();



}


// Initialize optimization object and Run Optimization by calling build_optim
void TGfitTask::init_optim( std::vector<double> &optv_, /*int &countit,*/ double &sum_of_squares )
{
    // Instantiate nlopt::opt object
    if( Opti->OptAlgo.compare( "'LN_COBYLA'" ) == 0 )
    {
        nlopt::opt nlopti_( nlopt::LN_COBYLA, optv_.size() );
        build_optim( nlopti_, optv_, sum_of_squares );
    }
    else if( Opti->OptAlgo.compare( "'GN_ISRES'" ) == 0 )
    {
        nlopt::opt nlopti_( nlopt::GN_ISRES, optv_.size() );
        build_optim( nlopti_, optv_, sum_of_squares );
    }
    else if( Opti->OptAlgo.compare( "'LN_BOBYQA'" ) == 0 )
    {
        nlopt::opt nlopti_( nlopt::LN_BOBYQA, optv_.size() );
        build_optim( nlopti_, optv_, sum_of_squares );
    }
    else if( Opti->OptAlgo.compare( "'GN_ORIG_DIRECT'" ) == 0 )
    {
        nlopt::opt nlopti_( nlopt::GN_ORIG_DIRECT, optv_.size() );
        build_optim( nlopti_, optv_, sum_of_squares );
    }
    else if( Opti->OptAlgo.compare( "'GN_ORIG_DIRECT_L'" ) == 0 )
    {
        nlopt::opt nlopti_( nlopt::GN_ORIG_DIRECT_L, optv_.size() );
        build_optim( nlopti_, optv_, sum_of_squares );
    }
    else
    {
        cout<<" Unknown optimization algorithm !!!! "<<endl;
        cout<<" ... bail out now ... "<<endl;
        exit(1);
    }


}


// Initialize optimization object and Run Optimization
void TGfitTask::build_optim( nlopt::opt &NLopti, std::vector<double> &optv_, /*std::vector<SS_System_Properties*> *ss_systems, int &countit,*/ double &sum_of_squares )
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

    if( Opti->OptNormParam && !printfile )
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
    ffout << endl << "14. in gemsfit_task.cpp line 487. Setting minimizing objective function." << endl;
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

    ffout << "15. in gesfit_task.cpp line 514. Performing optimization."<<endl;

//    //===== For testing the objective function without oprimization =====//
//    sum_of_squares = Equil_objective_function_callback(Opti->optv, grad, this);

    nlopt::result result = NLopti.optimize( Opti->optv, sum_of_squares );
    ffout<<"optv[0] = "<<Opti->optv[0]<<endl;
    ffout<<"size of optv = "<<Opti->optv.size()<<endl;

    ffout << "16. gemsfit_task.cpp line 523. Finished optimization; " << endl;


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
                    fout<<") = "<<sum_of_squares<<std::endl;
                    fout<<" after "<< master_counter <<" evaluations."<<std::endl;
                    fout.close();

                    std::cout<<"found minimum at <<f( ";
                    for( unsigned i=0; i<Opti->optv.size(); i++ )
                    {
                        std::cout<<Opti->optv[i]<<" ";
                    }
                    std::cout<<") = "<<sum_of_squares<<std::endl;
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
                  optv_[i] = Opti->optv[i] * abs(Opti->opt[i]);
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


void TGfitTask::get_nodes(long int nNod)
{
    NodT0 = 0;  // nodes at current time point
  //  NodT1 = 0;  // nodes at previous time point
    tcNode = 0;     // Node type codes (see DataBR.h) size anNodes+1
    iaNode = 0;
    allocMemory();
    gft = this;
    anNodes = nNod; // ???
}

TGfitTask::~TGfitTask(   )
{
   freeMemory();
}


// Copying data for node ii from node array into work DATABR structure
void TGfitTask::CopyWorkNodeFromArray( long int ii, long int nNodes, DATABRPTR* arr_BR )
{
  // from arr_BR[ii] to pCNode() structure
  if( ii < 0 || ii>= nNodes )
    return;
  // memory must be allocated before

  // mem_cpy( &pCNode()->NodeHandle, &arr_BR[ii]->NodeHandle, 6*sizeof(short));
  pCNode()->NodeHandle = arr_BR[ii]->NodeHandle;
  pCNode()->NodeTypeHY = arr_BR[ii]->NodeTypeHY;
  pCNode()->NodeTypeMT = arr_BR[ii]->NodeTypeMT;
  pCNode()->NodeStatusFMT = arr_BR[ii]->NodeStatusFMT;
  pCNode()->NodeStatusCH = arr_BR[ii]->NodeStatusCH;
  pCNode()->IterDone = arr_BR[ii]->IterDone;      //6
  // mem_cpy( &pCNode()->TK, &arr_BR[ii]->TK, 32*sizeof(double));
	pCNode()->TK = arr_BR[ii]->TK;
	pCNode()->P = arr_BR[ii]->P;
	pCNode()->Vs = arr_BR[ii]->Vs;  
	pCNode()->Vi = arr_BR[ii]->Vi;   
	pCNode()->Ms = arr_BR[ii]->Ms;   
	pCNode()->Mi = arr_BR[ii]->Mi;    
	pCNode()->Gs = arr_BR[ii]->Gs;    
	pCNode()->Hs = arr_BR[ii]->Hs; 	
	pCNode()->Hi = arr_BR[ii]->Hi;    
	pCNode()->IC = arr_BR[ii]->IC;    
	pCNode()->pH = arr_BR[ii]->pH;    
	pCNode()->pe = arr_BR[ii]->pe;     
	pCNode()->Eh = arr_BR[ii]->Eh; //13     

	pCNode()->Tm = arr_BR[ii]->Tm;    
    pCNode()->dt = arr_BR[ii]->dt;

// Dynamic data - dimensions see in DATACH.H structure
// exchange of values occurs through lists of indices, e.g. xDC, xPH
  copyValues( pCNode()->xDC, arr_BR[ii]->xDC, pCSD()->nDCb );
  copyValues( pCNode()->gam, arr_BR[ii]->gam, pCSD()->nDCb );
  if( CSD->nAalp >0 )
	  copyValues( pCNode()->aPH, arr_BR[ii]->aPH, pCSD()->nPHb );
  else  pCNode()->aPH = 0;
  copyValues( pCNode()->xPH, arr_BR[ii]->xPH, pCSD()->nPHb );
  copyValues( pCNode()->vPS, arr_BR[ii]->vPS, pCSD()->nPSb );
  copyValues( pCNode()->mPS, arr_BR[ii]->mPS, pCSD()->nPSb );

  copyValues( pCNode()->bPS, arr_BR[ii]->bPS,
                          pCSD()->nPSb*pCSD()->nICb );
  copyValues( pCNode()->xPA, arr_BR[ii]->xPA, pCSD()->nPSb );
  copyValues( pCNode()->dul, arr_BR[ii]->dul, pCSD()->nDCb );
  copyValues( pCNode()->dll, arr_BR[ii]->dll, pCSD()->nDCb );
  copyValues( pCNode()->bIC, arr_BR[ii]->bIC, pCSD()->nICb );
  copyValues( pCNode()->rMB, arr_BR[ii]->rMB, pCSD()->nICb );
  copyValues( pCNode()->uIC, arr_BR[ii]->uIC, pCSD()->nICb );
  copyValues( pCNode()->bSP, arr_BR[ii]->bSP, pCSD()->nICb );
}

// Copying data for node iNode back from work DATABR structure into the node array
void TGfitTask::MoveWorkNodeToArray( long int ii, long int nNodes, DATABRPTR* arr_BR )
{
  if( ii < 0 || ii>= nNodes )
    return;
  if( arr_BR[ii] )
  {
       arr_BR[ii] = databr_free( arr_BR[ii]);
       // delete[] arr_BR[ii];
  }
  arr_BR[ii] = pCNode();
// alloc new memory
  TNode::CNode = new DATABR;
  databr_reset( CNode, 1 );
  databr_realloc();
  // mem_set( &pCNode()->TC, 0, 32*sizeof(double));
}

void TGfitTask::CopyNodeFromTo( long int ndx, long int nNod,
                       DATABRPTR* arr_From, DATABRPTR* arr_To )
{
  if( !arr_From || !arr_To )
      return;
  CopyWorkNodeFromArray( ndx, nNod, arr_From );
  MoveWorkNodeToArray( ndx,  nNod, arr_To );
}

//---------------------------------------------------------
// Methods for working with node arrays (access to data from DBR)

// Calculate phase (carrier) mass, kg  of single component phase
double TGfitTask::get_mPH( long int ia, long int nodex, long int PHx )
{
  long int DCx = Phx_to_DCx( Ph_xDB_to_xCH(PHx) );
  double val=0.;

  if( DCx >= pCSD()->nDCs && DCx < pCSD()->nDC )
  {
    val = pCSD()->DCmm[DCx];
    if( ia == 0)
     val *= pNodT0()[nodex]->xDC[DC_xCH_to_xDB(DCx)];
//    else
//     val *= pNodT1()[nodex]->xDC[DC_xCH_to_xDB(DCx)];
  }

  return val;
}

// Calculate phase volume (in cm3) of single - component phase
double TGfitTask::get_vPH( long int ia, long int nodex, long int PHx )
{
  long int DCx = Phx_to_DCx( Ph_xDB_to_xCH(PHx) );
  double val=0.;

  if( DCx >= pCSD()->nDCs && DCx < pCSD()->nDC )
  {
     double T, P;
     if( ia == 0 )
     {
      T = pNodT0()[(nodex)]->TK;
      P = pNodT0()[(nodex)]->P;
      val = pNodT0()[nodex]->xDC[DC_xCH_to_xDB(DCx)]; // number of moles
     }
//     else
//     {
//      T = pNodT1()[(nodex)]->TK;
//      P = pNodT1()[(nodex)]->P;
//      val = pNodT1()[nodex]->xDC[DC_xCH_to_xDB(DCx)];
//     }
     val *= DC_V0( DCx, P, T );
  }
  return val;
}


// Calculate bulk compositions  of single component phase
double TGfitTask::get_bPH( long int ia, long int nodex, long int PHx, long int ICx )
{
  long int DCx = Phx_to_DCx( Ph_xDB_to_xCH(PHx) );
  double val=0.;

  if( DCx >= pCSD()->nDCs && DCx < pCSD()->nDC )
  {
    val = pCSD()->A[ pCSD()->xic[ICx] + DCx * pCSD()->nIC];
    if( ia == 0)
     val *= pNodT0()[nodex]->xDC[DC_xCH_to_xDB(DCx)];
//    else
//     val *= pNodT1()[nodex]->xDC[DC_xCH_to_xDB(DCx)];
  }

  return val;
}

//-----------------------End of gemsfit_task.cpp--------------------------

