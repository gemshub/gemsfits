//-------------------------------------------------------------------
// $Id: gemsft_task.h 789 2012-12-19 17:08:55Z kulik $
/// \file gemsft_task.h
/// Contains declaration of TNodeArray class implementing an advanced
/// interface for GEMSFIT.
//
/// \class TGfitTask gemsft_task.h
/// Implements an advanced C/C++ interface with GEMS3K for the
/// development of GEMSFIT. Uses TNodeArray class as analogy.
/// Works with DATACH and an array of DATABR structures; uses TNode class
//
// Copyright (C) 2013 G.D.Miron, D.Kulik
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

#ifndef _gemsft_task.h_
#define _gemsft_task.h_


#include "data_manager.h"
#include "opt_vector.h"
#include "optimization.h"
#include "gemsfit_global_variables.h"
#include <nlopt.hpp>
#include "print.h"



// These structures are needed for implementation of Random Walk and
// similar particle-based transport algorithms
//enum  PTCODE /// Codes of particle type
//{
//   DISSOLVED = 20,
//   ADVECTIVE = 21,
//   DIFFUSIVE = 22,
//   COLLOID = 23
//};


// Definition of TNodeArray class
class TGfitTask : public Data_Manager
{
protected:



    DATABR* (*NodT0);  ///< array of nodes
//    vector<samples*> experiments;

//    DATABR* (*NodT1);  ///< array of nodes for current time point

    long int anNodes;  ///< Number of allocated nodes (samples)

//    opti_vector *OParameters; ///< pointer to optimization vector class

    char* tcNode;      ///< Node type codes (see databr.h), size anNodes
    bool* iaNode;      ///< GEM IA status for all nodes (true: NEED_GEM_AIA, false: NEED_GEM_SIA)
    
    void allocMemory();
    void freeMemory();


public:

    optimization *Opti; ///< pointer to optimization

    /// the GEMSFIT configuration file (fixed to SS_GEMSFIT_input.dat)
    string param_file;

    /// Printing Flag: if this flag is set to one, the result of the optimization will be printed to file (via optimization.cpp)
    string printfile;

    /// Monte Carlo flag: if true, then the MPI commands within the objective function call will not be executed. Instead, the loop over Monte Carlo runs is parallelized
    bool MC_MPI;

    double weighted_Tfun_sum_of_residuals;

    /// normalize parameters flag
    bool NormParams;

    static TGfitTask* gft;   ///< static pointer to this class

    ResPrint* print;


    struct TargetFunction /// structure holding the target function information
    {
        string name; /// target function name
        string type; /// target function type
        string weight; /// type of weight

        struct obj_fun /// structure holding the information about the data to compare from the experiments
        {
            string exp_phase;
            string exp_elem;
            string exp_property;
            string exp_unit;
        };
        vector<obj_fun*> objfun;
    };

    TargetFunction* Tfun; /// pointer to target function structure

    /// Computed values for Monte Carlo confidence interval generation
    double_v computed_values_v;
    /// Measurement values for Monte Carlo confidence interval generation
    double_v measured_values_v;

    /// Computed residuals
    double_v residuals_v; // measured - calculated residuals
//    double_v relative_residuals_v; // 100*(measured-calculated)/measured in %
    double_v Weighted_Tfun_residuals_v; // Target function "residuals"
    double_v Tfun_residuals_v;
    double_v weights; // weights


   TGfitTask ();   ///< Constructor

   ~TGfitTask ();      /// Destructor

   void set_residuals (double computed, double measured, double Weighted_Tfun_residual, double Tfun_residual, double weight );

   // initialize optimization
   /**
   * Initializes the NLopt obeject and the optimization task
   * @author DM
   * @param optv_ vector of optimized parameters
   * @param weighted_Tfun_sum_of_residuals
   * @date 06.05.2013
   */
   virtual void init_optim(vector<double> &optv_, /*int &countit,*/ double &weighted_Tfun_sum_of_residuals );

   /**
   * initialize optimization object and assign constraints and bounds.
   * performs the optimization.
   * @param NLopti			nlopt optimization object
   * @param optv_              optimization vector
   * @param weighted_Tfun_sum_of_residuals		sum of squared residuals
   * @author DM
   * @date 07.05.2013
   */
   virtual void build_optim( nlopt::opt &NLopti, std::vector<double> &optv_, /*std::vector<System_Properties*> *systems, int &countit,*/ double &weighted_Tfun_sum_of_residuals );



   void get_DatTarget ( );


    void get_nodes( long int nNod ); ///< Constructors for 1D arrangement of nodes

    void  setnodes( );

    long int nNodes() const  /// Get total number of nodes in the node array
    { return anNodes; }

    DATABRPTR* pNodT0() const /// Get pointer to array of nodes
    { return NodT0; }

    bool* piaNode() const /// Get pointer to IA switches for nodes
    { return iaNode; }
    
    char* ptcNode() const /// Get pointer to boundary condition codes for nodes
    { return tcNode; }
    
    /// Calls GEM IPM calculation for a node with absolute index ndx
    long int RunGEM( long int ndx, long int Mode );
        // (cleans the work node DATABR structure)

    /// Initialization of TGFitTask data structures. Reads in the DBR text input files and
    /// copying data from work DATABR structure into the node array
    ///  \param dbrfiles_lst_name  pointer to a null-terminated C string with a path to a text file
    ///                      containing the list of names of  DBR input files.
    ///                      Example: file "test-dbr.lst" with a content:    "dbr-0.dat" , "dbr-1.dat" , "dbr-2.dat"
    ///  \param nodeTypes    the initial node contents from DATABR files will be distributed among nodes in array
    ///                      according to the distribution list nodeTypes
    ///  \param getNodT1     optional parameter used only when reading multiple DBR files after modeling
    ///                      task interruption  in GEM-Selektor
    void  InitGfitTask( const char *dbrfiles_lst_name, long int *nodeTypes, bool getNodT1, bool binary_f  );

    /// Copies data from the work DATABR structure into the node ndx in
    /// the node arrays NodT0 and NodT1  (as specified in nodeTypes array)
    void  setGfitTaskData( long int ndx, long int* nodeTypes  );

   /// Test setup of properties of samples in the task
    void  checkGfitSamples( long int i, long int* nodeTypes, const char*  datachbr_file );

   //---------------------------------------------------------
   // Methods for working with node arrays (access to data from DBR)
   /// Calculate phase (carrier) mass, kg  of single component phase
   double get_mPH( long int ia, long int nodex, long int PHx );
   /// Calculate phase volume, cm3  of single component phase
   double get_vPH( long int ia, long int nodex, long int PHx );
   /// Calculate bulk compositions  of single component phase
   double get_bPH( long int ia, long int nodex, long int PHx, long int IC );

   //---------------------------------------------------------
   // Methods for working with node arrays

    ///  Copies data for a node ndx from the array of nodes anyNodeArray that
    /// contains nNodes into the work node data bridge structure
    void CopyWorkNodeFromArray( long int ndx, long int nNodes, DATABRPTR* anyNodeArray );

    ///  Moves work node data to the ndx element of the node array anyNodeArray
    /// that has nNodes. Previous contents of the ndx element will be lost,
    /// work node will be allocated new and will contain no data
    void MoveWorkNodeToArray( long int ndx, long int nNodes, DATABRPTR* anyNodeArray );

    /// Copies a node from the node array arr_From to the same place in the
    /// node array arr_To. Previous contents of the ndx element in arr_To
    /// will be lost. Uses the work node structure which will be newly
    /// allocated and contain no data afterwards
    void CopyNodeFromTo( long int ndx, long int nNodes, DATABRPTR* arr_From,
         DATABRPTR* arr_To );

     /// Writes work node (DATABR structure) to a text VTK file
//     void databr_to_vtk( fstream& ff, const char*name, double time, long cycle,
//                               long int nFields=0, long int (*Flds)[2]=0);

};

//IC node data access macroses
#define node0_bIC( nodex, ICx ) (TGfitTask::pNodT0()[(nodex)]->bIC[(ICx)])
//#define node1_bIC( nodex, ICx ) (TGfitTask::pNodT1()[(nodex)]->bIC[(ICx)])
#define node0_rMB( nodex, ICx ) (TGfitTask::pNodT0()[(nodex)]->rMB[(ICx)])
//#define node1_rMB( nodex, ICx ) (TGfitTask::pNodT1()[(nodex)]->rMB[(ICx)])
#define node0_uIC( nodex, ICx ) (TGfitTask::pNodT0()[(nodex)]->uIC[(ICx)])
//#define node1_uIC( nodex, ICx ) (TGfitTask::pNodT1()[(nodex)]->uIC[(ICx)])

//DC node data access macroses

  // amount of DC with index DCx from T0 node with index nodex
#define node0_xDC( nodex, DCx ) (TGfitTask::pNodT0()[(nodex)]->xDC[(DCx)])
  // amount of DC with index DCx from T1 node with index nodex
//#define node1_xDC( nodex, DCx ) (TGfitTask::pNodT1()[(nodex)]->xDC[(DCx)])

  // activity coefficient of DC with index DCx from T0 node with index nodex
#define node0_gam( nodex, DCx ) (TGfitTask::pNodT0()[(nodex)]->gam[(DCx)])
  // activity coefficient of DC with index DCx from T1 node with index nodex
//#define node1_gam( nodex, DCx ) (TGfitTask::pNodT1()[(nodex)]->gam[(DCx)])

  // upper constraint on amount of DC with index DCx from T0 node with index nodex
#define node0_dul( nodex, DCx ) (TGfitTask::pNodT0()[(nodex)]->dul[(DCx)])
  // upper constraint on amount of DC with index DCx from T1 node with index nodex
//#define node1_dul( nodex, DCx ) (TGfitTask::pNodT1()[(nodex)]->dul[(DCx)])

  // lower constraint on amount of DC with index DCx from T0 node with index nodex
#define node0_dll( nodex, DCx ) (TGfitTask::pNodT0()[(nodex)]->dll[(DCx)])
  // lower constraint on amount of DC with index DCx from T1 node with index nodex
//#define node1_dll( nodex, DCx ) (TGfitTask::pNodT1()[(nodex)]->dll[(DCx)])

//Phase node data access macroses
  // amount of phase with index PHx from T0 node with index nodex
#define node0_xPH( nodex, PHx ) (TGfitTask::pNodT0()[(nodex)]->xPH[(PHx)])
  // amount of phase with index PHx from T1 node with index nodex
///#define node1_xPH( nodex, PHx ) (TGfitTask::pNodT1()[(nodex)]->xPH[(PHx)])

  // volume of multicomponent phase with index PHx from T0 node with index nodex
#define node0_vPS( nodex, PHx ) (TGfitTask::pNodT0()[(nodex)]->vPS[(PHx)])
  // volume of multicomponent phase with index PHx from T1 node with index nodex
//#define node1_vPS( nodex, PHx ) (TGfitTask::pNodT1()[(nodex)]->vPS[(PHx)])

  // volume of single-component phase with index PHx from T0 node with index nodex
#define node0_vPH( nodex, PHx ) (TGfitTask::get_vPH( 0, (nodex), (PHx)))
  // volume of single-component phase with index PHx from T1 node with index nodex
//#define node1_vPH( nodex, PHx ) (TGfitTask::get_vPH( 1, (nodex), (PHx)))

  // mass of multicomponent phase with index PHx from T0 node with index nodex
#define node0_mPS( nodex, PHx ) (TGfitTask::pNodT0()[(nodex)]->mPS[(PHx)])
  // mass of multicomponent phase with index PHx from T1 node with index nodex
//#define node1_mPS( nodex, PHx ) (TGfitTask::pNodT1()[(nodex)]->mPS[(PHx)])

  // mass of single-component phase with index PHx from T0 node with index nodex
#define node0_mPH( nodex, PHx )  (TGfitTask::get_mPH( 0, (nodex), (PHx)))
  // mass of single-component phase with index PHx from T1 node with index nodex
//#define node1_mPH( nodex, PHx )  (TGfitTask::get_mPH( 1, (nodex), (PHx)))


  // amount of solvent/sorbent in phase with index PHx from T0 node with index nodex
#define node0_xPA( nodex, PHx ) (TGfitTask::pNodT0()[(nodex)]->xPA[(PHx)])
  // amount of solvent/sorbent in phase with index PHx from T1 node with index nodex
//#define node1_xPA( nodex, PHx ) (TGfitTask::pNodT1()[(nodex)]->xPA[(PHx)])

// equilibrium bulk composition of solid part of the system, moles from T0 node
#define node0_bSP( nodex, ICx ) (TGfitTask::pNodT0()[(nodex)]->bSP[(ICx)])
// equilibrium bulk composition of solid part of the system, moles from T1 node
//#define node1_bSP( nodex, ICx ) (TGfitTask::pNodT1()[(nodex)]->bSP[(ICx)])


// Phase compositions node data access macroses
// amount of independent component ICx in multi-component phase PHx in T0 node nodex
#define node0_bPS( nodex, PHx, ICx ) ( TGfitTask::pNodT0()[(nodex)]->bPS[ \
                                       (PHx)*TGfitTask::pCSD()->nICb+(ICx)])
// amount of independent component ICx in multi-component phase PHx in T1 node nodex
//#define node1_bPS( nodex, PHx, ICx ) ( TGfitTask::pNodT1()[(nodex)]->bPS[ \
//                                       (PHx)*TGfitTask::pCSD()->nICb+(ICx)])

// amount of independent component ICx in single-component phase PHx in T0 node nodex
#define node0_bPH( nodex, PHx, ICx )  (TGfitTask::get_bPH( 0, (nodex), (PHx), (ICx)))
// amount of independent component ICx in single-component phase PHx in T1 node nodex
//#define node1_bPH( nodex, PHx, ICx )  (TGfitTask::get_bPH( 1, (nodex), (PHx), (ICx)))

#endif   // _gemsft_task_

// end gemsft_task
