/* 
*	 Copyright (C) 2012 by Ferdinand F. Hingerl (hingerl@hotmail.com)
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


/**
 *	@file gemsfit_global_functions.cpp
 *
 *	@brief this source file contains implementations of global functions needed for  optimization. 
 *	Since the nlopt library needs a function pointer to the objective and constraint
 *  functions, these functions need to be globally accessible and can not be implemented 
 *  as member functions. This source file contains mainly these implementations of the 
 *  respective callback functions.  
 *
 *	@author Ferdinand F. Hingerl
 *
 * 	@date 09.04.2012 
 *
 */

#include <vector>
#include <numeric>
#include <math.h>
#include <string>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>


#include "gemsfit_global_functions.h"


//int master_counter = 0;
//int slave_counter = 0;

namespace opti
{

	// Create arrays with continuous locations in memory
	int **alloc_2d_int(int rows, int cols) 
	{
		int *data           = (int *)malloc(rows*cols*sizeof(int));
		int **array_2d_int  = (int **)malloc(rows*sizeof(int*));
		for( int i=0; i<rows; i++ )
		    array_2d_int[i] = &(data[cols*i]);
		return array_2d_int;
	}

	void free_2d_int (int **array_2d_int)
	{
	  free (array_2d_int[0]);
	  free (array_2d_int);
	}


	double **alloc_2d_double(int rows, int cols) 
	{
		double *data           	  = (double *)malloc(rows*cols*sizeof(double));
		double **array_2d_double  = (double **)malloc(rows*sizeof(double*));
		for( int i=0; i<rows; i++ )
		    array_2d_double[i]    = &(data[cols*i]);
	
		return array_2d_double;
	}

	void free_2d_double (double **array_2d_double)
	{
	  free (array_2d_double[0]);
	  free (array_2d_double);
	}


	float **alloc_2d_float(int rows, int cols) 
	{
		float *data           	  = (float *)malloc(rows*cols*sizeof(float));
		float **array_2d_float  = (float **)malloc(rows*sizeof(float*));
		for( int i=0; i<rows; i++ )
		    array_2d_float[i]    = &(data[cols*i]);
	
		return array_2d_float;
	}

	void free_2d_float (float **array_2d_float)
	{
	  free (array_2d_float[0]);
	  free (array_2d_float);
	}


/*
	// Signal catcher for brute force GEMSFIT termination (Ctrl+C)	
	void catch_int( int sig_num ) 
	{ 

		signal(sig_num, SIG_IGN);

		cout << " ... GEMSFIT terminated by users's brute force ... " << endl; 
	

		// brute force stop of NLOPT
		nlopt_result nlopt_force_stop( opt_actmod );

		//exit(0);
	} 
*/
	
	
	bool barrier( int NP_DC, int NPar, int MaxOrd, int NPcoef, int NComp, double Tk, double Pbar, double* aDCc, double* aIPc, long int* aIPx, double Xw )
	{
		int j = 0, k = 0;
		double Q = 1.0, U = 1.0, R = 1.0;

        for( j=0; j<(NComp-1); j++ )
        {

			R = aDCc[NP_DC*j+2] + \
	        		(aDCc[NP_DC*j+3] + aDCc[NP_DC*j+4]*Tk + aDCc[NP_DC*j+5]*Tk*Tk) * (1-Xw) + \
	        		(aDCc[NP_DC*j+6] + aDCc[NP_DC*j+7]*Tk + aDCc[NP_DC*j+8]*Tk*Tk) * (1-Xw)*(1-Xw);

			if( R < 1e-07 || R > 700.0 )
			{

/*				cout << "R["<<j<<"] = " << R << " " << "A_R = " << aDCc[NP_DC*j+2] << " | B_R = " << (aDCc[NP_DC*j+3] + aDCc[NP_DC*j+4]*Tk + aDCc[NP_DC*j+5]*Tk*Tk) << " | total m = " << (1-Xw) << " | C_R =  " << (aDCc[NP_DC*j+6] + aDCc[NP_DC*j+7]*Tk + aDCc[NP_DC*j+8]*Tk*Tk) << " total m * total M = " << (1-Xw)*(1-Xw) << endl;  
		        cout << "Xw = " << Xw << endl;		
				for( k=0; k<7; k++  )
				   	cout << aDCc[NP_DC*j+2+k] << " ";
				cout << endl;	
*/				return true;
			}
        }
/*
        for( int ip=0; ip<NPar; ip++ )
        {
                weps = 1.0; U = 1.0;
                i1 		= aIPx[MaxOrd*ip];
                i2 		= aIPx[MaxOrd*ip+1];
                if( aIPc[NPcoef*ip]>0.99 && aIPc[NPcoef*ip]<1.01 )
                {	// Temperature correction for interaction parameter
                	U = aIPc[NPcoef*ip+1] + aIPc[NPcoef*ip+2]*pow((Tk-298.15), aIPc[NPcoef*ip+3]);                 
                }
                else if( aIPc[NPcoef*ip]>1.99 && aIPc[NPcoef*ip]<2.01 )
                {	// Temperature correction for permittivity term

                 	weps = aIPc[NPcoef*ip+1] + aIPc[NPcoef*ip+2]*(Tk-298.15)*1e-02 + aIPc[NPcoef*ip+3]*(Pbar-1.)*1e-03 + aIPc[NPcoef*ip+4]*(Tk-298.15)*(Tk-298.15)*1e-04 + aIPc[NPcoef*ip+5]*(Tk-298.15)*(Pbar-1.)*1e-06 + aIPc[NPcoef*ip+6]*(Tk-298.15)*(Tk-298.15)*(Tk-298.15)*1e-08 +aIPc[NPcoef*ip+7]*(Tk-298.15)*(Tk-298.15)*(Pbar-1.)*1e-08;
                }

			if( ip==1 || ip==2 )
			{	
				if( U < -20000 || U > 20000 || weps < 1e-05 || weps > 100.0 )
				{
					cout << " U = " << U << " | weps = " << weps << endl;
					cout << " U.a0 = " << aIPc[NPcoef*ip+1] << " U.a1 = " << aIPc[NPcoef*ip+2] << " U.a3 = " << pow((Tk-298.15), aIPc[NPcoef*ip+3]) << endl;
					return true;
				}
			}	
    	}
*/
		return false;
	}



    double StdStateEquil_objective_function_callback( const std::vector<double> &opt, std::vector<double> &grad, void *obj_func_data )
    {
        // If Master process enters function callback, he sends a signal to the others processes which makes them in turn calling ActMod_objective_function_callback directly. They are waiting for this signal in the init_optim function.
#ifdef USE_MPI
        MPI_Status status;
#endif
        int continue_or_exit = 1;
        int pid_;

        vector<SS_System_Properties*> *sys = reinterpret_cast<vector<SS_System_Properties*>*>(obj_func_data);

#ifdef USE_MPI
        // Master: send only when not in Monte Carlo mode (generation of confidence intervals)
        if( !sys->at(0)->MC_MPI )
        {
            if( !pid )
            {
                for( pid_=1; pid_<p; pid_++ )
                    MPI_Send( &continue_or_exit, 1, MPI_INT, pid_, 0,  MPI_COMM_WORLD );
            }
        }
#endif

        unsigned int i;
        double sum_of_squared_residuals_allsys = 0.0;
        double sum_of_squared_residuals_sys = 0.0;

        // Rescale optimization to unconvert normalization of parameters
        if( sys->at(0)->sysprop->NormParams )
        {
            vector<double> optV( opt.size() );
            for( i=0; i<opt.size(); i++ )
            {
                optV[i] = opt[i] * abs(sys->at(0)->sysprop->init_guesses[i]);
            }
            for( i=0; i<sys->size(); i++)
            {
                // call tsolmod wrapper
    ///			ActMod_gems3k_wrap( sum_of_squared_residuals_sys, optV, sys->at(i) );
                sum_of_squared_residuals_allsys = sum_of_squared_residuals_allsys + sum_of_squared_residuals_sys;
            }
        }
        else
        {
            for( i=0; i<sys->size(); i++)
            {
                // call tsolmod wrapper
///				ActMod_gems3k_wrap( sum_of_squared_residuals_sys, opt, sys->at(i) );
                sum_of_squared_residuals_allsys = sum_of_squared_residuals_allsys + sum_of_squared_residuals_sys;
            }
        }

    return sum_of_squared_residuals_allsys;
    }


//======================================== END StandrardStateProp instance ========================================== //


	// Wrapper loaded by "objective_function". Talks to GEMS' node class 
	void ActMod_tsolmod_wrap( double &sum_of_squared_residuals_sys, const vector<double> &opt, System_Properties* sys )
	{

			// Signal catcher for brute force GEMSFIT termination (Ctrl+C)
			//signal(SIGINT, opti::catch_int); 

			if( !pid )
			{
				master_counter++;
			}	
			if( pid == 1 )
			{	
				slave_counter++;
			}


			// Temporary storage vectors
			vector<double> computed_values_temp;
			vector<double> measured_values_temp;
			vector<double> computed_residuals_temp;

			// Clear already stored results
			sys->computed_values_v.clear();
			sys->measured_values_v.clear();
			sys->computed_residuals_v.clear();		


			//cout<< " now in ActMod_gems_wrap ..."<<endl;
			long NodeStatusCH, NodeHandle, ncomps;
			long NPar, NPcoef, MaxOrd, NComp, NP_DC, index_phase, dc_id_1, dc_id_2, DC_start_index;
			long index_phase_aIPx, index_phase_aIPc, index_phase_aDCc, index_H2O;
			double sum_of_squared_residuals_sys_ = 0.0, sum_M, computed_value, measured_value;
            long dc_id_1_1=0, dc_id_1_2=0, dc_id_2_1=0, dc_id_2_2=0;
			unsigned i, k, l;
			int j, start, step;
			long int ion_index1, ion_index2;
			vector<double> ln_gamma;		
			double residual = 0., P_pa, epsW, rhoW, meas_data_type, V0_ion1, V0_ion2, app_mol_vol = 0., apparent_molar_volume, dens_sol, mol_wt_electrolyte, stst_molvol_electrolyte; 
			double Rhow[5] 	 = 	{0.0, 0.0, 0.0, 0.0, 0.0};
			double Epsw[5] 	 = 	{0.0, 0.0, 0.0, 0.0, 0.0};
			int StoicIons[4] =  {1, 1, 0, 0};	
			double sum_aM 	 = 	0.0;
			char input_system_file_list_name[256];
			int ierr;

			// DATACH structure content
			int nIC=0, nDC=0, nPH=0;	

			// MPI can not work with STL containers, therefore copy values to dynamic array
			double* opt_ = new double[opt.size()];
			for( i=0; i<opt.size(); i++)
				opt_[i] = opt.at(i);	

			// Synchronize all threads with the updated guess values from the master thread -> perform MPI call only when NOT in printing mode (function: ActivityModel::print_results()) AND NOT in Monte Carlo mode (function: Statistics::MC_confidence_interval())				

	#ifdef GEMSFIT_DEBUG
			cout<<"pid "<<pid<<", inActMod_tsolmod_wrap, sys->printfile = "<<sys->printfile<<", sys->MC_MPI = "<<sys->MC_MPI<<endl;
	#endif

			if( !sys->printfile && !sys->MC_MPI )
			{	
	#ifdef GEMSFIT_DEBUG
				cout<<"pid "<<pid<<", inActMod_tsolmod_wrap, sys->printfile = "<<sys->printfile<<", sys->MC_MPI = "<<sys->MC_MPI<<endl;			
	#endif
	#ifdef USE_MPI
				ierr = MPI_Bcast( &opt_[0],opt.size(),MPI_DOUBLE,0,MPI_COMM_WORLD ); 
	#endif		
			}


	#ifdef GEMSFIT_DEBUG
			cout<<"pid "<<pid<<",  inActMod_tsolmod_wrap, after MPI_Bcast() ... "<<endl;
	#endif
	

			// instantiate SolutionData struct needed for creating an instance of TSolmod subclasses
			SolutionData sd;	

			sd.NSublat   = 0;        // number of sublattices nS
			sd.NMoiet    = 0;        // number of moieties nM
			sd.Mod_Code  = 'J';      // Code of the mixing model
			sd.Mix_Code  = 'I';      // Code for specific EoS mixing rule
			sd.DC_Codes  = NULL;     // DC class codes for species -> NSpecies
			sd.TP_Code   = NULL;     // Codes for TP correction methods for species ->NSpecies
			sd.arMoiSN   = NULL;     // End member moiety- site multiplicity number tables -> NSpecies x NSublat x NMoiet
			sd.arSitFr   = NULL;     // Tables of sublattice site fractions for moieties -> NSublat x NMoiet
			sd.arGEX     = NULL;     // Reciprocal energies, Darken terms, pure fugacities -> NSpecies
			sd.arPparc   = NULL;     // Partial pressures -> NSpecies  
			sd.arVol     = NULL;     // molar volumes of end-members (species) cm3/mol ->NSpecies
			sd.aphVOL    = NULL;     // phase volumes, cm3/mol (now obsolete) !!!!!!! check usage!
		   
			sd.NSpecies	= sys->species.size()-1; // subtract 1 to become consistent with non-vector counting
		
            sd.MaxOrder = sys->sysparam->cols_aIPx;
			sd.NPcoefs	= sys->sysparam->cols_aIPc;
			sd.NParams	= sys->sysparam->rows_aIPc;
			sd.NPperDC	= sys->sysparam->cols_aDCc;

			//long int ncomps = (long int) sd.NSpecies;


			// Assign new guesses to aIPc/aDCc vectors
			// loop over fit_ind_opt (position of parameter in opt vector) and assign to corresponding parameter of system (fit_ind_sys)
			long int len = (long int) sys->sysparam->aIPc.size();        
			vector<double> aipc;
			aipc.resize( len );
			len = (long int) sys->sysparam->aDCc.size();
			vector<double> adcc;
			adcc.resize( len );

			len = (long int) sys->sysparam->aIPc.size();        
			double* aripc = new (nothrow) double[ len ];
			if( !aripc ) { cout<<"Ferdl: optimization.cpp: aripc: ERROR MEMORY COULD NOT BE ALLOCATED !! "<<endl; exit(1); }
			len = (long int) sys->sysparam->aIPx.size();        
			long int* aripx = new (nothrow) long int[ len ];
			if( !aripx ) { cout<<"Ferdl: optimization.cpp: aripx: ERROR MEMORY COULD NOT BE ALLOCATED !! "<<endl; exit(1); }
			len = (long int) sys->sysparam->aDCc.size();        
			double* ardcc = new (nothrow) double[ len ];
			if( !ardcc ) { cout<<"Ferdl: optimization.cpp: ardcc: ERROR MEMORY COULD NOT BE ALLOCATED !! "<<endl; exit(1); }
			for( i=0; i<sys->sysparam->aIPc.size(); i++)
			{
				aripc[i] = sys->sysparam->aIPc[i];
			}
			for( i=0; i<sys->sysparam->aIPx.size(); i++)
			{
				aripx[i] = sys->sysparam->aIPx[i];
			}
			for( i=0; i<sys->sysparam->aDCc.size(); i++)
			{
				ardcc[i] = sys->sysparam->aDCc[i];
			}

			for( i=0; i<sys->fit_ind_opt.size(); i++ )
			{
				// fit_ind_sys(i) gives position in opt vector that correspond to the position in the parameter a**c_fit_ind (first looping over aIPc_ind and then over aDCc_fit_ind vector)
				if( i<(sys->sysparam->aIPc_fit_ind.size()) )
				{
					aripc[sys->sysparam->aIPc_fit_ind[ sys->fit_ind_sys[i]] ] = opt_[sys->fit_ind_opt[i]];
					aipc[sys->sysparam->aIPc_fit_ind[ sys->fit_ind_sys[i]] ] = opt_[sys->fit_ind_opt[i]];
				}else
				{
					ardcc[sys->sysparam->aDCc_fit_ind[ (sys->fit_ind_sys[i] - sys->sysparam->aIPc_fit_ind.size())] ] = opt_[sys->fit_ind_opt[i]];
					adcc[sys->sysparam->aDCc_fit_ind[ (sys->fit_ind_sys[i] - sys->sysparam->aIPc_fit_ind.size())] ] = opt_[sys->fit_ind_opt[i]];
				}
			}


			sd.arIPc = aripc;
			sd.arDCc = ardcc;
			sd.arIPx = aripx;


// ATTENTION
			// SET R and Q of anion = cation !!!! ! ! ! ! !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ! ! ! ! !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ! ! ! ! !
	//		sd.arDCc[2*sd.NPperDC]     = sd.arDCc[0]; // Q    CaCl2
	//		sd.arDCc[2*sd.NPperDC+2]   = sd.arDCc[2]; // R    CaCl2

			// system Ca Na Cl SO4 OH H H2O@
			// sd.arDCc[2*sd.NPperDC]     = sd.arDCc[1*sd.NPperDC+0] * 1.1135;    // Q    NaCl
			// sd.arDCc[2*sd.NPperDC+2]   = sd.arDCc[1*sd.NPperDC+2] * 1.1751;    // R    NaCl

			// system Ca K Na Cl SO4 OH H H2O@
	//		sd.arDCc[9*sd.NPperDC]     = sd.arDCc[6*sd.NPperDC+0] * 1.1135;    // Q    NaCl
	//		sd.arDCc[9*sd.NPperDC+2]   = sd.arDCc[6*sd.NPperDC+2] * 1.1751;    // R    NaCl


   // 		sd.arDCc[sd.NPperDC]     = sd.arDCc[0]; // * 2.5142;    // Q    CaCl3
   //		sd.arDCc[sd.NPperDC+2]   = sd.arDCc[2]; // * 3.9866;    // R    CaCl2

			//for( i=0; i<sd.NPperDC; i++ )
			//{
				//sd.arDCc[sd.NPperDC+i] = sd.arDCc[i];
				//adcc[sd.NPperDC+i] = adcc[i];			
			//}
			// SET R and Q of anion = cation !!!!! ! ! ! ! !!!!!!!!!!!!!!!!!!!!!!!!!!!!! ! ! ! ! !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ! ! ! ! !
// ATTENTION

	
#ifdef GEMSFIT_DEBUG
for( i=0; i<sys->sysparam->aDCc.size(); i++)
{
	cout<<"sd.arDCc["<<i<<"] = "<<sd.arDCc[i]<<" "<<" | ardcc = "<<ardcc[i]<<endl;
}
cout<<endl;	

for( i=0; i<sys->sysparam->aIPc.size(); i++)
{
	cout<<"sd.arIPc = "<<sd.arIPc[i]<<" | aripc = "<<aripc[i]<<endl;
}
cout<<endl;
#endif
				// get system file list name
				strcpy(input_system_file_list_name, sys->system_name.c_str());

				// call GEM_init to read GEMS3K input files and to get RhoW and EpsW vectors
				// deu
				TNode* node  = new TNode();
	
				// call GEM_init     --> read in input files
				if( (node->GEM_init( input_system_file_list_name )) == 1 )
				{
					cout<<" .. ERROR occurred while reading input files !!! ..."<<endl;
					exit(1);
				}

                
                index_phase = node->Ph_name_to_xCH( sys->phase_name.c_str( ) );

                DC_start_index = node->Phx_to_DCx( index_phase );

                string H2O("H2O@");
                index_H2O   = node->DC_name_to_xCH( H2O.c_str() );
    
                sd.NSpecies = index_H2O - DC_start_index + 1;
                ncomps = sd.NSpecies;


		// ---- // ---- // ---- // CENTRAL LOOP OVER MEASUREMENTS // ---- // ---- // ---- //

				double* lngam   = new (nothrow) double[ ncomps ];
				if( !lngam ) { cout<<"Ferdl: optimization.cpp: lngam: ERROR MEMORY COULD NOT BE ALLOCATED !! "<<endl; exit(1); }
				for( i=0; i<sd.NSpecies; i++ )
				{
					lngam[i]    = 1.0;
				}
				sd.arlnGam      = lngam;

				double* aZ    = new (nothrow) double[ ncomps ];
					if( !aZ ) { cout<<"Ferdl: optimization.cpp: aZ: ERROR MEMORY COULD NOT BE ALLOCATED !! "<<endl; exit(1); }
				for( i=0; i<ncomps; i++ )	
				{	aZ[i] = sys->charges[i]; }

				double* aM    = new (nothrow) double[ ncomps ];
				if( !aM ) { cout<<"Ferdl: optimization.cpp: aM: ERROR MEMORY COULD NOT BE ALLOCATED !! "<<endl; exit(1); }
				double* arwx  = new (nothrow) double[ ncomps ];
				if( !arwx ) { cout<<"Ferdl: optimization.cpp: arwx: ERROR MEMORY COULD NOT BE ALLOCATED !! "<<endl; exit(1); }


				// MPI LOOP : loop over measurements
				if( !sys->printfile || !sys->MC_MPI )
				{
					start = pid;
					step = p;
				}
				else // Don not use MPI parallelization if in "print" or "MC" mode
				{
					start = 0;
					step = 1;			
				}
		 	

				for( i = start; i < sys->sysdata->ende.size(); i += step )
				{
				        for( k=0; k<ncomps; k++ )
						{
							arwx[k] = 1e-9;
							aM[k] = 1e-7;
						}
						aM[ncomps-1] = 55.5084215;

						StoicIons[0] = sys->sysdata->stoic_1_1.at(i);
						StoicIons[1] = sys->sysdata->stoic_1_2.at(i);						
                        StoicIons[2] = sys->sysdata->stoic_2_1.at(i);
						StoicIons[3] = sys->sysdata->stoic_2_2.at(i);						
                    

						// copy new concentrations over to aM vector (direct input to activity model constructor)
						dc_id_1_1 = node->DC_name_to_xDB( sys->sysdata->species_1_1.at(i).c_str() );
				 		dc_id_1_2 = node->DC_name_to_xDB( sys->sysdata->species_1_2.at(i).c_str() );
						dc_id_2_1 = node->DC_name_to_xDB( sys->sysdata->species_2_1.at(i).c_str() );
				 		dc_id_2_2 = node->DC_name_to_xDB( sys->sysdata->species_2_2.at(i).c_str() );
                            
#ifdef GEMSFIT_DEBUG
cout << "tsolmod wrapper: dc_id_1_1 = "<<dc_id_1_1<<endl;
cout << "tsolmod wrapper: dc_id_1_2 = "<<dc_id_1_2<<endl;
cout << "tsolmod wrapper: dc_id_2_1 = "<<dc_id_2_1<<endl;
cout << "tsolmod wrapper: dc_id_2_2 = "<<dc_id_2_2<<endl;
#endif

						aM[ dc_id_1_1 ]   = sys->sysdata->molality_1.at(i) * StoicIons[0];
						aM[ dc_id_1_2 ]   = sys->sysdata->molality_1.at(i) * StoicIons[1];
                        
                        if( dc_id_2_1 > -1 )
    						aM[ dc_id_2_1 ]   = sys->sysdata->molality_2.at(i) * StoicIons[2];

                        if( dc_id_2_2 > -1 )
    						aM[ dc_id_2_2 ]   = sys->sysdata->molality_2.at(i) * StoicIons[3];

						sum_aM = 0.;
                        for( k=0; k<ncomps; k++ )
						{    sum_aM += aM[k];
//							cout << "aM["<<k<<"] = "<<aM[k]<<endl;
						}

                        for( k=0; k<ncomps; k++ )
                        {
							arwx[k] = aM[k] / sum_aM;        
//							cout << "arwx["<<k<<"] = "<<arwx[k]<<endl;
						}	

#ifdef GEMSFIT_DEBUG
for( k=0; k<ncomps; k++ )
    cout << "tsolmod wrapper: aM["<<k<<"] = "<<aM[k]<<endl;
cout << "tsolmod wrapper: sys->sysdata->molality_1.at("<<i<<") = "<<sys->sysdata->molality_1.at(i)<<endl;
#endif

						// fill up measurement point specific PTX data
						sd.arWx  = arwx;
						sd.P_bar = sys->sysdata->pressure.at(i);
						sd.T_k   = sys->sysdata->temperature.at(i);


						// Call barrier function to check if new guesses are within the constraint limits
						bool jumper = false;

						//jumper = barrier( sd.NPperDC, sd.NParams, sd.MaxOrder, sd.NPcoefs, ncomps, sd.T_k, sd.P_bar, ardcc, aripc, aripx, sd.arWx[ncomps-1] );

						
						if( !jumper )
						{


							P_pa	=	sd.P_bar * 100000;
							if( P_pa == 0. )
							{	
								P_pa = node->get_Ppa_sat( sd.T_k );
							}		
							epsW 	= 	node->EpsH2Ow( P_pa, sd.T_k );
	#ifdef GEMSFIT_DEBUG
	cout << "P_pa = " << P_pa << endl;
	cout << "epsW = " << epsW << endl;
	#endif
							if( epsW == 0 )
							{
								cout<<" !!!!    Calling node->EpsH2Ow returned 0 !!!!"<<endl;
								cout << " !!!!    P [pascal] = " << P_pa << endl;
								cout << " !!!!    T [Kelvin] = " << sd.T_k << endl;
								exit(1);
							}
							rhoW 	= 	node->DenH2Ow( P_pa, sd.T_k );
							if( rhoW==0 )
							{
								std::cout<<" !!!!    Calling node->DenH2Ow returned 0 !!!!"<<std::endl;
								cout << " !!!!    P [pascal] = " << P_pa << endl;
								cout << " !!!!    T [Kelvin] = " << sd.T_k << endl;
								exit(1);
							}		
							Rhow[0]	=	rhoW/1000;
							Epsw[0]	=  	epsW;


							// create instance of activity model class and retrieve activity coefficients
							TSolMod* activity_model_point = NULL;

							if( sys->activity_model == 1 )
							{
//								activity_model_point = new TELVIS( &sd, aM, aZ, Rhow, Epsw );
							}		
							else if( sys->activity_model == 2 )
							{
								activity_model_point = new TPitzer( &sd, aM, aZ, Rhow, Epsw ); 				}
							else if( sys->activity_model == 3 )
							{
								activity_model_point = new TEUNIQUAC( &sd, aM, aZ, Rhow, Epsw ); 
							}
							else if( sys->activity_model == 4 )
							{
								activity_model_point = new TSIT( &sd, aM, aZ, Rhow, Epsw ); 
							}
							else
							{
								cout<<" The activity model you entered is not implemented ! "<<endl;
								cout<<" Sorry, bailing out now ... "<<endl;
								exit(1);
							}


							// Correct SolMod instance to the new pressure and temperature
							activity_model_point->PTparam();

							meas_data_type = sys->sysdata->code.at(i);

							// Compute activity coefficient at new T-P-X
							activity_model_point->MixMod();

	#ifdef GEMSFIT_DEBUG
	cout << " tsolmod wrapper: sd.arlnGam[0] = " << sd.arlnGam[0] <<endl;
	#endif

			// ------------------------------------------------------------------------ //
			// code = 0 : activity coefficient of solute 
							if( meas_data_type < 0.1 )
							{
								computed_value = exp( (sd.arlnGam[ dc_id_1_1  ]*StoicIons[0] + sd.arlnGam[ dc_id_1_2  ]*StoicIons[1]) / (StoicIons[0]+StoicIons[1]) );
								measured_value = sys->sysdata->val.at(i);

								residual = pow( (computed_value - measured_value), 2) / measured_value; 

	#ifdef GEMSFIT_DEBUG
		std::cout<<" tsolmod wrapper: m = "<<sys->sysdata->molality_1.at(i)<<endl;
		std::cout<<" tsolmod wrapper: sd.arlnGam[ dc_id_1_1 ] = "<<sd.arlnGam[ dc_id_1_1 ]<<endl;
		std::cout<<" tsolmod wrapper: sd.arlnGam[ dc_id_1_2 ] = "<<sd.arlnGam[ dc_id_1_2 ]<<endl;
		std::cout<<"mean ln_gamma = "<<(sd.arlnGam[ dc_id_1_1  ]*sys->sysdata->stoic_1_1.at(i) + sd.arlnGam[ dc_id_1_2  ]*sys->sysdata->stoic_1_2.at(i))/(sys->sysdata->stoic_1_1.at(i)+sys->sysdata->stoic_1_2.at(i))<<std::endl;
		std::cout<<"ln meas = "<<log(sys->sysdata->val.at(i))<<std::endl;
		std::cout<<"computed_value = "<<computed_value<<std::endl;
		std::cout<<"measured_value = "<<measured_value<<std::endl;	
	#endif


							}
			// ------------------------------------------------------------------------ //		
			// code = 1 : natural log of activity coefficient of solute 
							else if( meas_data_type > 0.9 && meas_data_type < 1.1 )
							{		
	#ifdef GEMSFIT_DEBUG
		std::cout<<"mean ln_gamma = "<<(sd.arlnGam[ dc_id_1_1 ] * StoicIons[0] + sd.arlnGam[ dc_id_1_2  ] * StoicIons[1])/(StoicIons[0]+StoicIons[1])<<std::endl;
		std::cout<<"meas = "<<sys->sysdata->val.at(i)<<std::endl;
	#endif

								computed_value = exp( (sd.arlnGam[ dc_id_1_1  ] * StoicIons[0] + sd.arlnGam[ dc_id_1_2  ] * StoicIons[1])/(StoicIons[0]+StoicIons[1]) );
								measured_value = exp(sys->sysdata->val.at(i));

								residual = pow( (computed_value - measured_value), 2) / measured_value; 
							} 
			// ------------------------------------------------------------------------ //
			// code = 2 : osmotic coefficient of solvent -> compute by integration of the Bjeruum relation for multi-eletrolyte solutions
							else if( meas_data_type > 1.9 && meas_data_type < 2.1 )
							{	
	std::cout<<"Before Bjerrum:	mean ln_gamma = "<<(sd.arlnGam[ dc_id_1_1 ] * StoicIons[0] + sd.arlnGam[ dc_id_1_2  ] * StoicIons[1])/(StoicIons[0]+StoicIons[1])<<" |  aM[0] = "<<aM[0]<<std::endl;

#ifdef GEMSFIT_DEBUG
	std::cout<<"Before Bjerrum:	aM[0] = "<<aM[0]<<" | aM[1] = "<<aM[1]<<std::endl;
	std::cout<<"Before Bjerrum:	sd.arlnGam[0] = "<<sd.arlnGam[0]<<" | sd.arlnGam[1] = "<<sd.arlnGam[1]<<std::endl;
#endif

								double sum_M = 0.0;
							   	double osm_int = 0.0;
				                for( k=0; k<(ncomps-1); k++ )
								{    
									sum_M += aM[k];
								}

								// k : number of species to consider	
								
								SolutionData sdc = sd;
								double species_gamma_id=0.0, species_conc_id=0.0;

								for( species_gamma_id=0; species_gamma_id<2; species_gamma_id++ )
								{
									for( species_conc_id=0; species_conc_id<2; species_conc_id++ )
									{
										osm_int += Osm_coeff_Bjerrum( species_gamma_id, species_conc_id, sdc, aM, aZ, Rhow, Epsw );
									}
								}			

								computed_value = 1 + osm_int / sum_M; 
								//computed_value = 1 + osm_int / aM[0]; 

#ifdef GEMSFIT_DEBUG

	std::cout<<"After Bjerrum: mean ln_gamma = "<<(sd.arlnGam[ dc_id_1_1 ] * StoicIons[0] + sd.arlnGam[ dc_id_1_2  ] * StoicIons[1])/(StoicIons[0]+StoicIons[1])<<std::endl;
	std::cout<<"After Bjerrum:	aM[0] = "<<aM[0]<<" | aM[1] = "<<aM[1]<<std::endl;
	std::cout<<"After Bjerrum:	sd.arlnGam[0] = "<<sd.arlnGam[0]<<" | sd.arlnGam[1] = "<<sd.arlnGam[1]<<std::endl;
	std::cout<<"osm coeff meas = "<<sys->sysdata->val.at(i)<<std::endl;
	std::cout<<"osm coeff calc = "<<computed_value<<std::endl;

#endif
//exit(1);
								measured_value = sys->sysdata->val.at(i);

								residual = pow( (computed_value - measured_value), 2) / measured_value; 
			
							}
			// ------------------------------------------------------------------------ //
			// Apparent molar volumes 
							else if( meas_data_type > 2.9 && meas_data_type < 3.1 )
							{	
	#ifdef GEMSFIT_DEBUG
	cout<<endl;
	cout<<"in iteration = "<<i<<" | aM[0] = "<<aM[0]<<" | arwx[0] = "<<arwx[0]<<" | P_pa = "<<P_pa<<" | Tk = "<<sd.T_k<<" | Rhow[0] = "<<Rhow[0]<<endl;	
	#endif
								app_mol_vol = App_molar_volume( sd, aM, aZ, Rhow, Epsw, node, StoicIons );
				
								// double DC_V0(const long int xCH, const double P, const double Tk);
								ion_index1 = node->DC_name_to_xCH(sys->species[ dc_id_1_1 ].c_str());
								ion_index2 = node->DC_name_to_xCH(sys->species[ dc_id_1_2 ].c_str());
								V0_ion1 = 1e5 * node->DC_V0( ion_index1, P_pa, sd.T_k );
								V0_ion2 = 1e5 * node->DC_V0( ion_index2, P_pa, sd.T_k );
		                            // standard state molar volume of electrolyte at P-T
								stst_molvol_electrolyte = V0_ion1 * StoicIons[0] + V0_ion2 * StoicIons[1];
								apparent_molar_volume = 10*(stst_molvol_electrolyte + app_mol_vol);
								computed_value = apparent_molar_volume;
	#ifdef GEMSFIT_DEBUG
	std::cout<<"stst_molvol_electrolyte = "<<stst_molvol_electrolyte<<std::endl;
	std::cout<<"apparent_molar_volume = "<<apparent_molar_volume<<std::endl;			
	std::cout<<"meas = "<<sys->sysdata->val.at(i)<<std::endl;
	#endif
								measured_value = sys->sysdata->val.at(i);

								residual = pow( (computed_value - measured_value), 2) / measured_value; 
							}	
			// ------------------------------------------------------------------------ //
			// Density of solution 
							else if( meas_data_type > 3.9 && meas_data_type < 4.1 )
							{
								mol_wt_electrolyte = sys->sysdata->other_2.at(i);
							
								app_mol_vol = App_molar_volume( sd, aM, aZ, Rhow, Epsw, node, StoicIons );

								ion_index1 = node->DC_name_to_xCH(sys->species[ dc_id_1_1 ].c_str());
								ion_index2 = node->DC_name_to_xCH(sys->species[ dc_id_1_2 ].c_str());			
								V0_ion1 = 1e5 * node->DC_V0( ion_index1, P_pa, sd.T_k ); //DC_V0 gives units J/Pa -> multiply with 1e5 to get J/bar
								V0_ion2 = 1e5 * node->DC_V0( ion_index2, P_pa, sd.T_k );			
								// standard state molar volume of electrolyte at P-T
								stst_molvol_electrolyte = V0_ion1 * StoicIons[0] + V0_ion2 * StoicIons[1];

								// form expression for density from apparent molar volume of electrolyte
								rhoW = rhoW/1000.0;
								apparent_molar_volume = stst_molvol_electrolyte + app_mol_vol;
								dens_sol = (1000.0 + (aM[0] / StoicIons[0])*mol_wt_electrolyte)/(aM[0]*10*apparent_molar_volume + 1000.0/rhoW);
								computed_value = dens_sol;
	#ifdef GEMSFIT_DEBUG			
	std::cout<<"stst_molvol_electrolyte = "<<stst_molvol_electrolyte<<std::endl;
	std::cout<<"dens_sol = "<<dens_sol<<std::endl;
	std::cout<<"meas = "<<sys->sysdata->val.at(i)<<std::endl;
	#endif	
								measured_value = sys->sysdata->val.at(i);
							
								residual = pow( (computed_value - measured_value), 2) / measured_value; 
							}
			// ------------------------------------------------------------------------ //
			// Apparent molar volumes from analytical Gibbs excess function derivative
							else if( meas_data_type > 9.9 && meas_data_type < 10.1 )
							{
								sum_M = 0.0;

								mol_wt_electrolyte = sys->sysdata->other_2.at(i);

								ion_index1 = node->DC_name_to_xCH(sys->species[ dc_id_1_1 ].c_str());
								ion_index2 = node->DC_name_to_xCH(sys->species[ dc_id_1_2 ].c_str());			
								V0_ion1 = 1e5 * node->DC_V0( ion_index1, P_pa, sd.T_k ); //DC_V0 gives units J/Pa -> multiply with 1e5 to get J/bar
								V0_ion2 = 1e5 * node->DC_V0( ion_index2, P_pa, sd.T_k );			
								// standard state molar volume of electrolyte at P-T
								stst_molvol_electrolyte = 10 * ( V0_ion1 * StoicIons[0] + V0_ion2 * StoicIons[1] );

								for( l=0; l<(ncomps-1); l++ )
								{
									sum_M += aM[l];
								}

								app_mol_vol = stst_molvol_electrolyte + 10 * (Get_Vex_from_actmod( sd, aM, aZ, Rhow, Epsw, node, StoicIons )/1000) / sum_M;
							
								computed_value = app_mol_vol;
	//#ifdef GEMSFIT_DEBUG			
	std::cout<<"stst_molvol_electrolyte = "<<stst_molvol_electrolyte<<std::endl;
	std::cout<<"app_mol_vol = "<<computed_value<<std::endl;
	std::cout<<"meas = "<<sys->sysdata->val.at(i)<<std::endl;
	//#endif	
								measured_value = sys->sysdata->val.at(i);
							
								residual = pow( (computed_value - measured_value), 2) / measured_value; 
							}
			// ------------------------------------------------------------------------ //
			// Solution densities from analytical Gibbs excess function derivative
							else if( meas_data_type > 10.9 && meas_data_type < 11.1 )
							{
								sum_M = 0.0;

								mol_wt_electrolyte = sys->sysdata->other_2.at(i);

								ion_index1 = node->DC_name_to_xCH(sys->species[ dc_id_1_1 ].c_str());
								ion_index2 = node->DC_name_to_xCH(sys->species[ dc_id_1_2 ].c_str());			
								V0_ion1 = 1e5 * node->DC_V0( ion_index1, P_pa, sd.T_k ); //DC_V0 gives units J/Pa -> multiply with 1e5 to get J/bar
								V0_ion2 = 1e5 * node->DC_V0( ion_index2, P_pa, sd.T_k );			
								// standard state molar volume of electrolyte at P-T
									stst_molvol_electrolyte = 10 * ( V0_ion1 * StoicIons[0] + V0_ion2 * StoicIons[1] );

								for( l=0; l<(ncomps-1); l++ )
								{
									sum_M += aM[l];
								}

								app_mol_vol = stst_molvol_electrolyte + (Get_Vex_from_actmod( sd, aM, aZ, Rhow, Epsw, node, StoicIons )/1000. ) / sum_M;

								dens_sol = (1000.0 + (aM[0] / StoicIons[0])*mol_wt_electrolyte)/(aM[0]*10*app_mol_vol + 1000.0/rhoW);
								computed_value = dens_sol;
	#ifdef GEMSFIT_DEBUG			
	std::cout<<"stst_molvol_electrolyte = "<<stst_molvol_electrolyte<<std::endl;
	std::cout<<"dens_sol = "<<dens_sol<<std::endl;
	std::cout<<"meas = "<<sys->sysdata->val.at(i)<<std::endl;
	#endif	
								measured_value = sys->sysdata->val.at(i);
							
								residual = pow( (computed_value - measured_value), 2) / measured_value; 
							}
	#ifdef GEMSFIT_DEBUG	
	std::cout<<"residual = "<<residual<<" | i = " <<i<<std::endl;
	#endif
	

			// ------------------------------------------------------------------------ //
	/*						// Print measured and computed values to file myFIT_PLOT.out
							if( sys->printfile )
							{
								const char path[200] = "output_GEMSFIT/myFIT_PLOT.out";
								ofstream fout;
								fout.open(path, ios::app);						
								if( fout.fail() )
								{ cout<<"Output fileopen error"<<endl; exit(1); }
								fout << right << setw(20) << sys->sysdata->molality_1.at(i) << right << setw(20) << computed_value << right << setw(20) << measured_value << right << setw(20) << sys->sysdata->species_1_1.at(i) << right << setw(20) << sys->sysdata->species_1_2.at(i) << right << setw(20) << sys->sysdata->code.at(i) << endl;
								fout.close();
							}	
	*/

							// Store computed and measured values for Monte Carlo confidence interval generation
							computed_values_temp.push_back(computed_value);
							measured_values_temp.push_back(measured_value);

							// Store residuals for statistical analysis
							computed_residuals_temp.push_back(measured_value - computed_value);

							// Sum of squared residuals
							sum_of_squared_residuals_sys_ = sum_of_squared_residuals_sys_ + residual *  100 ;

							delete activity_model_point;
					// ------------------------------------------------------------------------ //

					}
					else // if jumper == 1
					{
						sum_of_squared_residuals_sys_ = 1e20;
					}

					
				} // END for loop over measurement points


				// Print measured and computed values to file myFIT_PLOT.out
				if( sys->printfile )
				{
#ifdef BOOST_MPI		
					ofstream fout;
					ostringstream pb; 
					pb << proc_id_boost;
					string out_fit_plot("./output_GEMSFIT")
					out_fit_plot += "_" + pb.str() + "/myFIT_PLOT.out";
					fout.open( out_fit_plot.c_str(), ios::app );
#endif

#ifndef BOOST_MPI
					const char path[200] = "output_GEMSFIT/myFIT_PLOT.out";
					ofstream fout;
					fout.open(path, ios::app);						
#endif
					if( fout.fail() )
					{ cout<<"Output fileopen error"<<endl; exit(1); }

					for( k=0; k< (int) computed_values_temp.size(); k++ )
					{
						fout << sys->sysdata->temperature.at(k) << ";" << sys->sysdata->molality_1.at(k) << ";" << computed_values_temp.at(k) << ";" << measured_values_temp.at(k) << ";" << sys->sysdata->species_1_1.at(k) << ";" << sys->sysdata->species_1_2.at(k) << ";" << (100.0-(computed_values_temp.at(k)/measured_values_temp.at(k))*100.0) << endl;
					}

#ifdef GEMSFIT_DEBUG
					for( i=0; i<sys->sysparam->aDCc.size(); i++)
					{
						fout<<"sd.arDCc["<<i<<"] = "<<sd.arDCc[i]<<" "<<" | ardcc = "<<ardcc[i]<<endl;
					}
					fout<<endl;	

					for( i=0; i<sys->sysparam->aIPc.size(); i++)
					{
						fout<<"sd.arIPc = "<<sd.arIPc[i]<<" | aripc = "<<aripc[i]<<endl;
					}
					fout<<endl;
#endif



					fout.close();
				}	



				// Store computed values and actual measured values (could be less than sys->sysdata->val) for Monte Carlo confidence interval generation
				sys->computed_values_v = computed_values_temp;
				sys->measured_values_v = measured_values_temp;


				// Store residuals for statistical analysis
				sys->computed_residuals_v = computed_residuals_temp;

#ifdef GEMSFIT_DEBUG	
std::cout<<"pid "<<pid<<" : sum_of_squared_residuals_sys_ = "<<sum_of_squared_residuals_sys_<<std::endl;
#endif

				// Collect all residuals from all threads -> perform MPI call only when NOT in printing mode (function: ActivityModel::print_results()) OR NOT in Monte Carlo mode (function: Statistics::MC_confidence_interval())
				if( !sys->printfile && !sys->MC_MPI )
				{	
		#ifdef USE_MPI
					ierr = MPI_Reduce( &sum_of_squared_residuals_sys_, &sum_of_squared_residuals_sys, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD); 
		#endif

		#ifndef USE_MPI
					sum_of_squared_residuals_sys = sum_of_squared_residuals_sys_;
		#endif
				}else
				{
					sum_of_squared_residuals_sys = sum_of_squared_residuals_sys_;
				}		
	

			// free dynamic memory
			delete node;
			delete[] lngam;
			delete[] aZ;
			delete[] aM;
			delete[] arwx;


		// free dynamic memoryvector<double> aDCc, vector<double> aIPc, vector<long int> aIPx, double X
		delete[] opt_;
		delete[] ardcc;
		delete[] aripx;
		delete[] aripc;

//cout << " counter: "<< master_counter << " | SSR: " << sum_of_squared_residuals_sys << endl;

		if( !pid ) 
		{
#ifdef BOOST_MPI		
			ofstream fout;
			ostringstream pb; 
			pb << proc_id_boost;
			string out_fit_plot("./output_GEMSFIT")
			out_fit_plot += "_" + pb.str() + "/fitting_stream.out";
			fout.open( out_fit_plot.c_str(), ios::app );
#endif

#ifndef BOOST_MPI
			const char path[200] = "output_GEMSFIT/fitting_stream.out";
			ofstream fout;
			fout.open(path, ios::app);						
#endif

			if( fout.fail() )
			{ cout<<"Output fileopen error"<<endl; exit(1); }
			fout << master_counter << ";"  << sum_of_squared_residuals_sys;
			for( i=0; i<opt.size(); i++)
				fout << ";" << opt.at(i);
			fout << endl;	
			fout.close();
		}
    } // END tsolmod_wrap()
// ------------------------------------------------------------------------ //



	// Wrapper around GEMS3K solver
	void ActMod_gems3k_wrap( double &sum_of_squared_residuals_sys, const std::vector<double> &opt, System_Properties* sys )
	{
		// Call GEMS3K and run GEM_run();
#ifdef GEMSFIT_DEBUG
cout<<"in ActMod_gems3k_wrap ..."<<endl;
#endif
		if( !pid )
		{
			master_counter++;
		}	
		if( pid == 1 )
		{	
			slave_counter++;
		}


		// Temporary storage vectors
		vector<double> computed_values_temp;
		vector<double> measured_values_temp;
		vector<double> computed_residuals_temp;

		// Clear already stored results
		sys->computed_values_v.clear();
		sys->measured_values_v.clear();
		sys->computed_residuals_v.clear();		

		long NPar, NPcoef, MaxOrd, NComp, NP_DC, index_phase, dc_id_1, dc_id_2;
		long index_phase_aIPx, index_phase_aIPc, index_phase_aDCc;
		double residual, sum_of_squared_residuals_sys_ = 0.0, rhoW, computed_value;
		double P_pa, meas_data_type, V0_ion1, V0_ion2, app_mol_vol, apparent_molar_volume, dens_sol, mol_wt_electrolyte, stst_molvol_electrolyte; 
		unsigned i, k;
		int j, start, step, ierr;
		long int ion_index1, ion_index2, NodeStatusCH, NodeHandle;
		vector<double> ln_gamma;
		char input_system_file_list_name[256];

		// DATACH structure content
		int nIC, nDC, nPH;	
		double* bIC;
		double* xDC_up;
		double* xDC_lo;
		double* Ph_surf;


		// MPI can not work with STL containers, therefore copy values to dynamic array
		double* opt_ = new double[ opt.size() ];
		for( i=0; i<opt.size(); i++)
			opt_[i] = opt.at(i);	

		// Synchronize all threads with the updated guess values from the master thread -> perform MPI call only when NOT in printing mode (function: ActivityModel::print_results()) AND NOT in Monte Carlo mode (function: Statistics::MC_confidence_interval())				

#ifdef GEMSFIT_DEBUG
		cout<<"pid "<<pid<<", inActMod_gems3k_wrap, sys->printfile = "<<sys->printfile<<", sys->MC_MPI = "<<sys->MC_MPI<<endl;
#endif
		if( !sys->printfile && !sys->MC_MPI )
		{	
#ifdef GEMSFIT_DEBUG
			cout<<"pid "<<pid<<", inActMod_gems3k_wrap, sys->printfile = "<<sys->printfile<<", sys->MC_MPI = "<<sys->MC_MPI<<endl;			
#endif

#ifdef USE_MPI
			ierr = MPI_Bcast( &opt_[0],opt.size(),MPI_DOUBLE,0,MPI_COMM_WORLD ); 
#endif
		}

#ifdef GEMSFIT_DEBUG
		cout<<"pid "<<pid<<",  inActMod_gems3k_wrap, after MPI_Bcast() ... "<<endl;
#endif
		   
		long ncomps = sys->species.size()-1; // subtract 1 to become consistent with non-vector counting
		long MaxOrder_fit = sys->sysparam->cols_aIPx;
		long NPcoefs_fit  = sys->sysparam->cols_aIPc;
		long NParams_fit  = sys->sysparam->rows_aIPc;
		long NPperDC_fit  = sys->sysparam->cols_aDCc;
		ln_gamma.resize( ncomps );	
		long int len = (long int) sys->sysparam->aIPc.size();        
		vector<double> aipc; 
		aipc.resize( len );
		vector<int> aipx;
		aipx.resize( len );
		len = (long int) sys->sysparam->aDCc.size();        
		vector<double> adcc;
		adcc.resize( len );
		double T_k;

		
		// Assign new guesses to aIPc/aDCc vectors
		// loop over fit_ind_opt (position of parameter in opt vector) and assign to corresponding parameter of system (fit_ind_sys)

		for( i=0; i<sys->sysparam->aIPc.size(); i++)
		{
			aipc[i] = sys->sysparam->aIPc[i];
		}
		for( i=0; i<sys->sysparam->aIPx.size(); i++)
		{
			aipx[i] = sys->sysparam->aIPx[i];
		}
		for( i=0; i<sys->sysparam->aDCc.size(); i++)
		{
			adcc[i] = sys->sysparam->aDCc[i];
		}

		for( i=0; i<sys->fit_ind_opt.size(); i++ )
		{
			// fit_ind_sys(i) gives position in opt vector that correspond to the position in the parameter a**c_fit_ind (first looping over aIPc_ind and then over aDCc_fit_ind vector)
			if( i<(sys->sysparam->aIPc_fit_ind.size()) )
			{
				aipc[sys->sysparam->aIPc_fit_ind[ sys->fit_ind_sys[i]] ] = opt_[sys->fit_ind_opt[i]];
			}else
			{
				adcc[sys->sysparam->aDCc_fit_ind[ (sys->fit_ind_sys[i] - sys->sysparam->aIPc_fit_ind.size())] ] = opt_[sys->fit_ind_opt[i]];
			}
		}


#ifdef GEMSFIT_DEBUG
		for( i=0; i<aipc.size(); i++ )
			cout<<"GEMS3K wrapper:	aipc["<<i<<"] = "<<aipc[i]<<endl;

		for( i=0; i<adcc.size(); i++ )
			cout<<"GEMS3K wrapper:	adcc["<<i<<"] = "<<adcc[i]<<endl;
#endif


		// get system file list name
		strcpy(input_system_file_list_name, sys->system_name.c_str());

		// call GEM_init to read GEMS3K input files 
		TNode* node  = new TNode();

		// call GEM_init     --> read in input files
		if( (node->GEM_init( input_system_file_list_name )) == 1 )
		{
			cout<<" .. ERROR occurred while reading input files !!! ..."<<endl;
		}

		// Getting direct access to work node DATABR structure which exchanges the
		// data with GEMS3K (already filled out by reading the DBR input file)
		DATABR* dBR = node->pCNode(); 

		DATACH* dCH = node->pCSD();

		nIC = dCH->nIC;	// Ca Cl O H e
		nDC = dCH->nDC;	// Ca+2 Cl- OH- H+ H2O@
		nPH = dCH->nPH;	// ELVIS Gas Portlandite	

		bIC = new double[ nIC ];
		xDC_up = new double[ nDC ];
		xDC_lo = new double[ nDC ];
		Ph_surf = new double[ nPH ];


		// B vector of molar amounts of independent components
		// element corresponding to ion 1
		bIC[0] = 0.0; 
		// element corresponding to ion	2			
		bIC[1] = 0.0; 
		// hydrogen
		bIC[2] = 111.016746657646;
		// oxygen
		bIC[3] = 55.5083933288231;
		// charge
		bIC[4] = 0.;

		// lower and upper bounds for concentration of DC
		for( i=0; i<nDC; i++ )
		{
			xDC_up[ i ]  = 1000000.;
			xDC_lo[ i ]  = 0.;
		}

		// Surface energy of phases -> kinetics
		for( i=0; i<nPH; i++ )
			Ph_surf[i] = 0.;
		


		// Asking GEM to run with automatic initial approximation 
		dBR->NodeStatusCH = NEED_GEM_AIA;


		// PERFORM TEST RUN WITH DBR file
		NodeStatusCH = node->GEM_run( false );

#ifdef GEMSFIT_DEBUG
cout << " NodeStatusCH = "<<NodeStatusCH<<endl;
cout << "   node temperature from dbr file: " << node->cTK() << endl;
cout << "   node pressure from dbr file: " << node->cP() << endl;
#endif
		if( NodeStatusCH == OK_GEM_AIA || NodeStatusCH == OK_GEM_SIA  )
		{    
			node->GEM_print_ipm( "GEMS3K_log.out" );
		}
		else 
		{
			// possible return status analysis, error message
			node->GEM_print_ipm( "GEMS3K_log.out" );   // possible debugging printout
			cout<<" GEMS3K did not converge properly !!!! continuing anyway ... "<<endl;
		}



		// Get index of phase of interest
		index_phase = node->Ph_name_to_xCH( sys->phase_name.c_str() );

		// Get parameter array dimension as specified in GEMS3K input file
		node->Get_NPar_NPcoef_MaxOrd_NComp_NP_DC ( NPar, NPcoef, MaxOrd, NComp, NP_DC, index_phase );

		// Get indices of parameter start position as given in GEMS3K input file
		node->Get_IPc_IPx_DCc_indices( index_phase_aIPx, index_phase_aIPc, index_phase_aDCc, index_phase );

		// Set aIPc parameter array
		node->Set_aIPc( aipc, index_phase_aIPc, index_phase );

		// Set aDCc parameter array
		node->Set_aDCc( adcc, index_phase_aDCc, index_phase );

#ifdef GEMSFIT_DEBUG
cout<<"gems3 wrap line 636"<<endl;
#endif

// ---- // ---- // ---- // CENTRAL LOOP OVER MEASUREMENTS // ---- // ---- // ---- //

		double* lngam   = new (nothrow) double[ nDC ];
		if( !lngam ) { cout<<"Ferdl: optimization.cpp: lngam: ERROR MEMORY COULD NOT BE ALLOCATED !! "<<endl; exit(1); }
		for( i=0; i<nDC; i++ )
		{
			lngam[i]    = 1.0;
		}


		if( !sys->printfile || !sys->MC_MPI )
		{
			start = pid;
			step = p;
		}
		else // Don not use MPI parallelization if in "print" or "MC" mode
		{
			start = 0;
			step = 1;			
		}

// ########################################################################################### //
//	- - - // - - -	// MPI LOOP : loop over measurements //	- - - // - - - // - - - // 
// ########################################################################################### //	
		for( i = start; i < sys->sysdata->ende.size() ; i += step )
		{


				// Set amount of dependent components (GEMS3K: DBR indexing)
				// vector of dependent components which contains the differences in concentration compared to the B vector

#ifdef GEMSFIT_DEBUG
cout << " ------------------------ new measurement value --------------------------------------- " << endl;
cout<<" sys->sysdata->species_1_1.at("<<i<<") = "<< sys->sysdata->species_1_1.at(i) <<endl;
cout<<" sys->sysdata->species_1_2.at("<<i<<") = "<< sys->sysdata->species_1_2.at(i) <<endl;
#endif
 				dc_id_1 = node->DC_name_to_xDB( sys->sysdata->species_1_1.at(i).c_str() );
		 		dc_id_2 = node->DC_name_to_xDB( sys->sysdata->species_1_2.at(i).c_str() );

#ifdef GEMSFIT_DEBUG
cout<<"gems3k wrap: dc_id_1 = "<<dc_id_1<<", dc_id_2 = "<<dc_id_2<<endl;
#endif

				// Set concentrations of independent components
				// element corresponding to ion 1
				bIC[0] = sys->sysdata->molality_1.at(i)*sys->sysdata->stoic_1_1.at(i);
				// element corresponding to ion	2			
				bIC[1] = sys->sysdata->molality_1.at(i)*sys->sysdata->stoic_1_2.at(i);

#ifdef GEMSFIT_DEBUG
cout<<" bIC["<<0<<"] = "<< bIC[0] <<endl;
cout<<" bIC["<<1<<"] = "<< bIC[1] <<endl;
#endif


				// FOR DEBUGGING
      				node->GEM_write_dbr( "dbr_before_GEM_from_MT.out", false, true );


				// Set temperature and pressure	
				P_pa = 100000 * sys->sysdata->pressure.at(i);
				
				// Transfer new temperature, pressure and b-vector to GEMS3K
				T_k = sys->sysdata->temperature.at(i);
#ifdef GEMSFIT_DEBUG
cout << " P_pa = " << P_pa << endl;
cout << " T_k  = " << T_k  << endl;
#endif
				node->GEM_from_MT( NodeHandle, NodeStatusCH, T_k, P_pa, 0., 0., bIC, xDC_up, xDC_lo, Ph_surf );


				// FOR DEBUGGING
      				node->GEM_write_dbr( "dbr_before_GEM_run.out", false, true );

                                // Asking GEM to run with automatic initial approximation
                                dBR->NodeStatusCH = NEED_GEM_AIA;
 	
				// RUN GEMS3K AND RETRIEVE ACTIVITY COEFFICIENTS
                                NodeStatusCH = node->GEM_run( false );

#ifdef GEMSFIT_DEBUG
cout<<" NodeStatusCH = "<<NodeStatusCH<<endl;
cout<<" NodeHandle = "<<NodeHandle<<endl;
cout << "Ion 1     Get_nDC  " << node->Get_nDC( 0 ) <<  " DC_n  " << node->DC_n( 0 ) << endl;
cout << "Ion 2     Get_nDC  " << node->Get_nDC( 1 ) <<  " DC_n  " << node->DC_n( 1 ) << endl;
cout << " Node temperature = " << node->cTK() << endl;
cout << " Node pressure = " << node->cP() << endl;
#endif

				// FOR DEBUGGING
      				node->GEM_write_dbr( "dbr_after_GEM_run.out", false, true );


				if( NodeStatusCH == OK_GEM_AIA || NodeStatusCH == OK_GEM_SIA  )
				{    
					node->GEM_print_ipm( "GEMS3K_log.out" );   // possible debugging printout
				}
				else 
				{
					// possible return status analysis, error message
					node->GEM_print_ipm( "GEMS3K_log.out" );   // possible debugging printout
					cout<<" GEMS3K did not converge properly !!!! continuing anyway ... "<<endl;
				}


				// find nature of datapoint
				meas_data_type = sys->sysdata->code.at(i);


				if( meas_data_type > 0.9 && meas_data_type < 1.1 )
				{		
					// Natural logarithm of activity coefficients -> ???? wrong function !!!!
					ln_gamma[0] = node->DC_lng( dc_id_1 );
					ln_gamma[1] = node->DC_lng( dc_id_2 );

#ifdef GEMSFIT_DEBUG
for( k=0;k<(ncomps-1); k++ )
{
cout<<"ln_gamma = "<<ln_gamma[k]<<endl;
}
std::cout<<"mean ln_gamma = "<<(ln_gamma[0]*sys->sysdata->stoic_1_1.at(i) + ln_gamma[1]*sys->sysdata->stoic_1_2.at(i))/(sys->sysdata->stoic_1_1.at(i)+sys->sysdata->stoic_1_2.at(i))<<std::endl;
std::cout<<"meas = "<<sys->sysdata->val.at(i)<<std::endl;
#endif

					computed_value = (ln_gamma[0]*sys->sysdata->stoic_1_1.at(i) + ln_gamma[1]*sys->sysdata->stoic_1_2.at(i))/(sys->sysdata->stoic_1_1.at(i)+sys->sysdata->stoic_1_2.at(i));
					
					residual = pow( (computed_value - sys->sysdata->val.at(i)), 2) / sys->sysdata->val.at(i); 
				} 
				else if( meas_data_type < 0.1 )
				{
					// Activity coefficients
					ln_gamma[0] = node->DC_lng( dc_id_1 );
					ln_gamma[1] = node->DC_lng( dc_id_2 );


#ifdef GEMSFIT_DEBUG
std::cout<<"m = "<<sys->sysdata->molality_1.at(i)<<endl;
std::cout<<"mean ln_gamma = "<<(ln_gamma[0]*sys->sysdata->stoic_1_1.at(i) + ln_gamma[1]*sys->sysdata->stoic_1_2.at(i))/(sys->sysdata->stoic_1_1.at(i)+sys->sysdata->stoic_1_2.at(i))<<std::endl;
std::cout<<"ln meas = "<<log(sys->sysdata->val.at(i))<<std::endl;
#endif

						computed_value = exp( (ln_gamma[0]*sys->sysdata->stoic_1_1.at(i) + ln_gamma[1]*sys->sysdata->stoic_1_2.at(i)) / (sys->sysdata->stoic_1_1.at(i)+sys->sysdata->stoic_1_2.at(i)) );
						double ln_mean_gam_m = sys->sysdata->val.at(i);

						if( sys->printfile )
						{
							const char path[200] = "output_GEMSFIT/myFIT_PLOT.out";
							ofstream fout;
							fout.open(path, ios::app);						
							if( fout.fail() )
							{ cout<<"Output fileopen error"<<endl; exit(1); }
							fout << right << setw(20) << sys->sysdata->molality_1.at(i) << right << setw(20) << computed_value << right << setw(20) << ln_mean_gam_m << endl;
							fout.close();
						}	

					residual = pow( (computed_value - sys->sysdata->val.at(i)), 2) / sys->sysdata->val.at(i); 
				}
				else if( meas_data_type > 2.9 && meas_data_type < 3.1 )
				{	
					// Apparent molar volumes
					//app_mol_vol = App_molar_volume( activity_model_point, node );
					
					// double DC_V0(const long int xCH, const double P, const double Tk);
					ion_index1 = node->DC_name_to_xCH( sys->species[0].c_str() );
					ion_index2 = node->DC_name_to_xCH( sys->species[1].c_str() );
					V0_ion1 = 1e5 * node->DC_V0( ion_index1, (100000 * sys->sysdata->pressure.at(i)), sys->sysdata->temperature.at(i) );
					V0_ion2 = 1e5 * node->DC_V0( ion_index2, (100000 * sys->sysdata->pressure.at(i)), sys->sysdata->temperature.at(i) );
					// standard state molar volume of electrolyte at P-T
					stst_molvol_electrolyte = V0_ion1 * sys->sysdata->stoic_1_1.at(i) + V0_ion2 * sys->sysdata->stoic_1_2.at(i);
					apparent_molar_volume = 10*(stst_molvol_electrolyte + app_mol_vol);
					computed_value = apparent_molar_volume;
#ifdef GEMSFIT_DEBUG
std::cout<<"stst_molvol_electrolyte = "<<stst_molvol_electrolyte<<std::endl;
std::cout<<"apparent_molar_volume = "<<apparent_molar_volume<<std::endl;			
std::cout<<"meas = "<<sys->sysdata->val.at(i)<<std::endl;
#endif
					residual = pow( (computed_value - sys->sysdata->val.at(i)), 2) / sys->sysdata->val.at(i); 
				}
				else if( meas_data_type > 3.9 && meas_data_type < 4.1 )
				{	
					// Density of solution
					// Get rhow
					rhoW 	= 	node->DenH2Ow( P_pa, sys->sysdata->temperature.at(i) );
					if( rhoW==0 )
					{
						std::cout<<"Calling node->DenH2Ow returned 0 !!!!"<<std::endl;
						exit(1);
					}		

					mol_wt_electrolyte = sys->sysdata->other_2.at(i);
				//	app_mol_vol = activity_model_point->App_molar_volume();			
					//app_mol_vol = App_molar_volume( activity_model_point, node );
					// double DC_V0(const long int xCH, const double P, const double Tk);
					ion_index1 = node->DC_name_to_xCH(sys->species[0].c_str());
					ion_index2 = node->DC_name_to_xCH(sys->species[1].c_str());			
					V0_ion1 = 1e5 * node->DC_V0( ion_index1, (100000 * sys->sysdata->pressure.at(i)), sys->sysdata->temperature.at(i) ); //DC_V0 gives units J/Pa -> multiply with 1e5 to get J/bar
					V0_ion2 = 1e5 * node->DC_V0( ion_index2, (100000 * sys->sysdata->pressure.at(i)), sys->sysdata->temperature.at(i) );			
					// standard state molar volume of electrolyte at P-T
					stst_molvol_electrolyte = V0_ion1 * sys->sysdata->stoic_1_1.at(i) + V0_ion2 * sys->sysdata->stoic_1_2.at(i);

					// form expression for density from apparent molar volume of electrolyte
					rhoW = rhoW/1000.0;
					apparent_molar_volume = stst_molvol_electrolyte + app_mol_vol;
					dens_sol = (1000.0 + node->DC_n( 0 )*mol_wt_electrolyte)/(node->DC_n( 0 )*10*apparent_molar_volume + 1000.0/rhoW);
					computed_value = dens_sol;
#ifdef GEMSFIT_DEBUG			
std::cout<<"stst_molvol_electrolyte = "<<stst_molvol_electrolyte<<std::endl;
std::cout<<"dens_sol = "<<dens_sol<<std::endl;
std::cout<<"meas = "<<sys->sysdata->val.at(i)<<std::endl;
#endif
					residual = pow( (computed_value - sys->sysdata->val.at(i)), 2) / sys->sysdata->val.at(i); 
				}
						#ifdef GEMSFIT_DEBUG	
						std::cout<<"residual = "<<residual<<std::endl;
						#endif

				// Store computed and measured values for Monte Carlo confidence interval generation
				computed_values_temp.push_back(computed_value);
				measured_values_temp.push_back(sys->sysdata->val.at(i));

				// Store residuals for statistical analysis
				computed_residuals_temp.push_back(sys->sysdata->val.at(i) - computed_value);

				// Sum of squares
				sum_of_squared_residuals_sys_ = sum_of_squared_residuals_sys_ + residual;


		} // END for loop over measurement points
// ########################################################################################### //
//	- - - // - - -	// END MPI LOOP : loop over measurements //	- - - // - - - // - - - // 
// ########################################################################################### //	

		// Store computed values and actual measured values (could be less than sys->sysdata->val) for Monte Carlo confidence interval generation
		sys->computed_values_v = computed_values_temp;
		sys->measured_values_v = measured_values_temp;


		// Store residuals for statistical analysis
		sys->computed_residuals_v = computed_residuals_temp;


#ifdef GEMSFIT_DEBUG	
std::cout<<"pid "<<pid<<" : sum_of_squared_residuals_sys_ = "<<sum_of_squared_residuals_sys_<<std::endl;
#endif

		// Collect all residuals from all threads -> perform MPI call only when NOT in printing mode (function: ActivityModel::print_results()) OR NOT in Monte Carlo mode (function: Statistics::MC_confidence_interval())
		if( !sys->printfile && !sys->MC_MPI )
		{	
#ifdef GEMSFIT_DEBUG
			cout<<"pid "<<pid<<", inActMod_tsolmod_wrap, line 407, sys->printfile = "<<sys->printfile<<", sys->MC_MPI = "<<sys->MC_MPI<<endl;			
#endif

#ifdef USE_MPI
			ierr = MPI_Reduce( &sum_of_squared_residuals_sys_, &sum_of_squared_residuals_sys, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD); 
#endif

#ifndef USE_MPI
			sum_of_squared_residuals_sys = sum_of_squared_residuals_sys_;
#endif


#ifdef GEMSFIT_DEBUG
			cout<<"pid "<<pid<<", inActMod_tsolmod_wrap, line 409, sys->printfile = "<<sys->printfile<<", sys->MC_MPI = "<<sys->MC_MPI<<endl;			
#endif
		}else
		{
			sum_of_squared_residuals_sys = sum_of_squared_residuals_sys_;
		}		
	
		// free dynamic memory
		delete node;
		delete[] opt_;
		delete[] lngam;
		delete[] bIC;
		delete[] xDC_up;
		delete[] xDC_lo;
		delete[] Ph_surf;

	}



	double ActMod_objective_function_callback( const std::vector<double> &opt, std::vector<double> &grad, void *obj_func_data )
	{
		// If Master process enters function callback, he sends a signal to the others processes which makes them in turn calling ActMod_objective_function_callback directly. They are waiting for this signal in the init_optim function.
#ifdef USE_MPI
		MPI_Status status;
#endif
		int continue_or_exit = 1;
		int pid_;

#ifdef GEMSFIT_DEBUG
ofstream param_residual_out;
param_residual_out.open("opt_vector_in_callback.dat", ios::app);

param_residual_out << " opt = " << endl;
for( int i=0; i<opt.size(); i++)
{
	param_residual_out << opt.at(i) <<" ";
}	
param_residual_out << endl;
param_residual_out.close();
#endif


		vector<System_Properties*> *sys = reinterpret_cast<vector<System_Properties*>*>(obj_func_data);

#ifdef USE_MPI
		// Master: send only when not in Monte Carlo mode (generation of confidence intervals)
		if( !sys->at(0)->MC_MPI )
		{
			if( !pid ) 
			{	
				for( pid_=1; pid_<p; pid_++ )
					MPI_Send( &continue_or_exit, 1, MPI_INT, pid_, 0,  MPI_COMM_WORLD );  
			}
		}
#endif

		unsigned int i;
		double sum_of_squared_residuals_allsys = 0.0;
		double sum_of_squared_residuals_sys = 0.0;

		
		// Rescale optimization to unconvert normalization of parameters
		if( sys->at(0)->sysparam->NormParams )
		{	
			vector<double> optV( opt.size() );
			for( i=0; i<opt.size(); i++ )
			{
				optV[i] = opt[i] * abs(sys->at(0)->sysparam->init_guesses[i]);
			}
		    for( i=0; i<sys->size(); i++)
		   	{
				// call tsolmod wrapper
				ActMod_tsolmod_wrap( sum_of_squared_residuals_sys, optV, sys->at(i) );
				sum_of_squared_residuals_allsys = sum_of_squared_residuals_allsys + sum_of_squared_residuals_sys;
			}
		}
		else
		{			
		    for( i=0; i<sys->size(); i++)
		   	{
				// call tsolmod wrapper
				ActMod_tsolmod_wrap( sum_of_squared_residuals_sys, opt, sys->at(i) );
				sum_of_squared_residuals_allsys = sum_of_squared_residuals_allsys + sum_of_squared_residuals_sys;
			}
		}

	return sum_of_squared_residuals_allsys;
	}



	double ActModEquil_objective_function_callback( const std::vector<double> &opt, std::vector<double> &grad, void *obj_func_data )
	{
		// If Master process enters function callback, he sends a signal to the others processes which makes them in turn calling ActMod_objective_function_callback directly. They are waiting for this signal in the init_optim function.
#ifdef USE_MPI
		MPI_Status status;
#endif
		int continue_or_exit = 1;
		int pid_;

		vector<System_Properties*> *sys = reinterpret_cast<vector<System_Properties*>*>(obj_func_data);

#ifdef USE_MPI
		// Master: send only when not in Monte Carlo mode (generation of confidence intervals)
		if( !sys->at(0)->MC_MPI )
		{
			if( !pid ) 
			{	
				for( pid_=1; pid_<p; pid_++ )
					MPI_Send( &continue_or_exit, 1, MPI_INT, pid_, 0,  MPI_COMM_WORLD );  
			}
		}
#endif

		unsigned int i;
		double sum_of_squared_residuals_allsys = 0.0;
		double sum_of_squared_residuals_sys = 0.0;

		// Rescale optimization to unconvert normalization of parameters
		if( sys->at(0)->sysparam->NormParams )
		{	
			vector<double> optV( opt.size() );
			for( i=0; i<opt.size(); i++ )
			{
				optV[i] = opt[i] * abs(sys->at(0)->sysparam->init_guesses[i]);
			}
		    for( i=0; i<sys->size(); i++)
		   	{
				// call tsolmod wrapper
				ActMod_gems3k_wrap( sum_of_squared_residuals_sys, optV, sys->at(i) );
				sum_of_squared_residuals_allsys = sum_of_squared_residuals_allsys + sum_of_squared_residuals_sys;
			}	
		}
		else
		{			
		    for( i=0; i<sys->size(); i++)
		   	{
				// call tsolmod wrapper
				ActMod_gems3k_wrap( sum_of_squared_residuals_sys, opt, sys->at(i) );
				sum_of_squared_residuals_allsys = sum_of_squared_residuals_allsys + sum_of_squared_residuals_sys;
			}
		}

	return sum_of_squared_residuals_allsys;
	}






	double ActMod_constraint_function_callback( const std::vector<double> &opt, std::vector<double> &grad, void *constr_data )
	{
/*		typedef struct 
		{    
		    int id;
			double OptUpConstraints;
			double OptLoConstraints;
		} my_constraint_data;
*/
		typedef struct 
		{    
		    int id;
			double Constraints;
		} my_constraint_data;

		my_constraint_data *cdata = reinterpret_cast<my_constraint_data*>(constr_data);

		double TK_l = 298.0;	double TK_h = 573.0;
		double Pbar_l = 1.0;	double Pbar_h = 1000.0;
		double res = 0.; int id = 0;



		if( cdata->id < 42 )
		{
			// U
			if( cdata->id < 14 ) 
			{ 
				id = cdata->id / 2;  
				if( ! (cdata->id % 2) )
				{
					res = opt[ id*3 ] + opt[ id*3 + 1 ]*TK_l + opt[ id*3 + 2 ]*TK_l*TK_l - cdata->Constraints;
				}
				else
				{
				 	res = opt[ id*3 ] + opt[ id*3 + 1 ]*TK_h + opt[ id*3 + 2 ]*TK_h*TK_h - cdata->Constraints;
				}
			}
			else
			{ 
				// R_B
				if( cdata->id >= 14 && cdata->id < 28  )
				{
					id = (cdata->id - 14 ) / 2; 
					if( ! (cdata->id % 2) )
					{
						res = opt[ 23+id*8 ] + opt[ 23+id*8 + 1 ]*TK_l + opt[ 23+id*8 + 2 ]*TK_l*TK_l - cdata->Constraints;
					}
					else
					{
					 	res = opt[ 23+id*8 ] + opt[ 23+id*8 + 1 ]*TK_h + opt[ 23+id*8 + 2 ]*TK_h*TK_h - cdata->Constraints;
					}
				}
				// R_C
				if( cdata->id >= 28 && cdata->id < 42  )
				{
					id = (cdata->id - 28 ) / 2; 
					if( ! (cdata->id % 2) )
					{
						res = opt[ 26+id*8 ] + opt[ 26+id*8 + 1 ]*TK_l + opt[ 26+id*8 + 2 ]*TK_l*TK_l - cdata->Constraints;
					}
					else
					{
					 	res = opt[ 26+id*8 ] + opt[ 26+id*8 + 1 ]*TK_h + opt[ 26+id*8 + 2 ]*TK_h*TK_h - cdata->Constraints;
					}
				}
			}
		}
		else
		{ 
			// U
			if( cdata->id < 56 ) 
			{ 
				id = (cdata->id - 42)/ 2;
				if( ! (cdata->id % 2) )
				{
					res = -( opt[ id*3 ] + opt[ id*3 + 1 ]*TK_l + opt[ id*3 + 2 ]*TK_l*TK_l) + cdata->Constraints;
				}
				else
				{
				 	res = -(opt[ id*3 ] + opt[ id*3 + 1 ]*TK_h + opt[ id*3 + 2 ]*TK_h*TK_h) + cdata->Constraints;
				}
			}
			else
			{ 
				// R_B
				if( cdata->id >= (42+14) && cdata->id < (42+28)  )
				{
					id = (cdata->id - 42 - 14 ) / 2; 
					if( ! (cdata->id % 2) )
					{
						res = -(opt[ 23+id*8 ] + opt[ 23+id*8 + 1 ]*TK_l + opt[ 23+id*8 + 2 ]*TK_l*TK_l) + cdata->Constraints;
					}
					else
					{
					 	res = -(opt[ 23+id*8 ] + opt[ 23+id*8 + 1 ]*TK_h + opt[ 23+id*8 + 2 ]*TK_h*TK_h) + cdata->Constraints;
					}
				}
				// R_C
				if( cdata->id >= (42+28) && cdata->id < (42+42)  )
				{
					id = (cdata->id - 42 - 28 ) / 2; 
					if( ! (cdata->id % 2) )
					{
						res = -(opt[ 26+id*8 ] + opt[ 26+id*8 + 1 ]*TK_l + opt[ 26+id*8 + 2 ]*TK_l*TK_l) + cdata->Constraints;
					}
					else
					{
					 	res = -(opt[ 26+id*8 ] + opt[ 26+id*8 + 1 ]*TK_h + opt[ 26+id*8 + 2 ]*TK_h*TK_h) + cdata->Constraints;
					}
				}
			}
		}

	return res;
	}

	
	double Get_Vex_from_actmod( SolutionData &sd, double* aM, double* aZ, double* Rhow, double* Epsw, TNode* node_pointer, int* StoicIons )
	{
		int i;
		double Vex;
		double ExProps[ 7 ];
		
		vector<double> RhoW(5,0);
		vector<double> EpsW(5,0);

		TSolMod* activity_model_point = NULL;

		node_pointer->EpsArrayH2Ow( (sd.P_bar * 100000), sd.T_k, EpsW );
#ifdef GEMSFIT_DEBUG
cout << "EpsW[0] = " << EpsW[0] << endl;
cout << "EpsW[3] = " << EpsW[3] << endl;
#endif
		if( EpsW[0] == 0 )
		{
			std::cout<<" !!!!    Calling node->EpsH2Ow returned 0 !!!!"<<std::endl;
			cout << " !!!!    P [pascal] = " << (sd.P_bar * 100000) << endl;
			cout << " !!!!    T [Kelvin] = " << sd.T_k << endl;
			exit(1);
		}

		node_pointer->DensArrayH2Ow( (sd.P_bar * 100000), sd.T_k, RhoW );
		for( i=0; i<5; i++ )
		{
			RhoW[i] = RhoW[i]/1000;
		}
		if( RhoW[0]==0 )
		{
			std::cout<<" !!!!    Calling node->DenH2Ow returned 0 !!!!"<<std::endl;
			cout << " !!!!    P [pascal] = " << (sd.P_bar * 100000) << endl;
			cout << " !!!!    T [Kelvin] = " << sd.T_k << endl;
			exit(1);
		}		

		for( i=0; i<5; i++ )
		{
			Rhow[i] = RhoW[i];
			Epsw[i] = EpsW[i];
		}


//		activity_model_point = new TELVIS( &sd, aM, aZ, Rhow, Epsw );
		activity_model_point->PTparam();
		activity_model_point->MixMod();
		
		activity_model_point->ExcessProp( ExProps );

		Vex = ExProps[ 4 ];

#ifdef GEMSFIT_DEBUG
cout << "Vex = " << Vex << endl;
#endif
		delete activity_model_point;

	return Vex;	
	}



	double App_molar_volume( SolutionData &sd, double* aM, double* aZ, double* Rhow, double* Epsw, TNode* node_pointer, int* StoicIons )
	{
		    // add to app_molar_vol_part the stst molar volume of the electrolyte (outside ELVIS class) !!!!
		    long int j      = 0;
			long int not_used_value	= 0;
		    double m_infdil = 1.1e-6;
		    double m_el 	= node_pointer->DC_n( j ); 	// concentration of electrolyte
			double result, error;
			const double R_CONST = 8.31451;
			double app_molar_vol_part = R_CONST * (node_pointer->cTK()) * qsimp( m_infdil, m_el, j, not_used_value, 1, sd, aM, aZ, Rhow, Epsw, node_pointer, StoicIons )/m_el;

		    // partial molar excess volume
		    // double part_molar_excess_vol = R_CONST*Tk*FinDiffVol( m[j], j );
		    //cout<<"part_molar_excess_vol = "<<part_molar_excess_vol<<endl;

		return app_molar_vol_part; // To get the apparent molar volume of the solute, add the stst partial molar volume to 'app_molar_vol_part'
	}
	
/*
	double Int_OsmCoeff()
	{
		    double osm_coeff   = 0.0;
		    double m_infdil    = 1.1e-6;
		    double bjerrum     = 0.0;
		    // calc osmotic coefficient via Bjerrum relation
		    long int j = 0;

		    // bjerrum : \int_{m_k=0}^{m_k=m[j]} m_k  d log_gam(m_k)
		    bjerrum = qsimp( m_infdil, m[j], j, 0 );
	//		cout<<" integral between "<<m_infdil<<" and "<<m[j]<<" is : "<<bjerrum<<endl;
		    osm_coeff = 1 + bjerrum/m[j];
		    cout<<" Osmotic coefficient = "<<osm_coeff<<endl;
	return osm_coeff;
	}
*/

	double Osm_coeff_Bjerrum( long int species_gamma_id, long int species_conc_id, SolutionData sdc, double* aM, double* aZ, double* Rhow, double* Epsw )
	{
		double lower_bound = 1e-16;
		double upper_bound = aM[ species_conc_id ];
		double bjerrum = 0.;

	    // bjerrum : \int_{m_i=0}^{m_i=m_i_f} m_i  d log_gam(m_i)
		bjerrum = qsimp( lower_bound, upper_bound, species_gamma_id, species_conc_id, 0, sdc, aM, aZ, Rhow, Epsw, NULL, NULL );

		return bjerrum;
	}



	double FinDiff( double m_spec_current, long int species_gamma_id, long int species_conc_id, SolutionData &sdc1, double* aM, double* aZ, double* Rhow, double* Epsw  )
	{
	    double h = 1e-4;
	    double DlnactDm;
	    double m_old = aM[ species_conc_id ];

	    // Forward Finite Difference
	    double gam_1=0., gam_2=0., gam_3=0.;

cout << "FinDiff:	aM["<< species_conc_id << "] = " << m_spec_current << endl;
cout << "FinDiff:	aM[0] = " << aM[0] << endl;
cout << "FinDiff:	aM[1] = " << aM[1] << endl;


	aM[ species_conc_id ] 	= m_spec_current;
	    //aM[ 0 ] 	= m_spec_current;
	    //aM[ 1 ] 	= m_spec_current;
//		TSolMod* activity_model_point_1 = new TELVIS( &sdc1, aM, aZ, Rhow, Epsw );
//		activity_model_point_1->PTparam();
//		activity_model_point_1->MixMod();
	    gam_1 = sdc1.arlnGam[ species_gamma_id ]; 
	    //gam_1 = 0.5 * (sdc1.arlnGam[ 0 ] + sdc1.arlnGam[ 1 ]);
cout<<"FD 1 	aM["<<species_conc_id<<"] = "<< aM[ species_conc_id ] <<" | gam_1 = "<< gam_1 <<endl;
cout<<"ln_gamma mean = "<< 0.5*(sdc1.arlnGam[ 0 ] + sdc1.arlnGam[ 1 ]) << endl;
//		delete activity_model_point_1;
		

	aM[ species_conc_id ] 	= m_spec_current + h;
		//aM[ 0 ] 	= m_spec_current + h;
	    //aM[ 1 ] 	= m_spec_current + h;
//		TSolMod* activity_model_point_2 = new TELVIS( &sdc1, aM, aZ, Rhow, Epsw );
//		activity_model_point_2->PTparam();
//		activity_model_point_2->MixMod();
	    gam_2 = sdc1.arlnGam[ species_gamma_id ];
	    //gam_2 = 0.5 * (sdc1.arlnGam[ 0 ] + sdc1.arlnGam[ 1 ]);
cout<<"FD 2		aM["<<species_conc_id<<"] = "<< aM[ species_conc_id ]<<" | gam_2 = "<< gam_2 <<endl;
//		delete activity_model_point_2;


	aM[ species_conc_id ] 	= m_spec_current + h + h;
		//aM[ 0 ] 	= m_spec_current + h + h;
	    //aM[ 1 ] 	= m_spec_current + h + h;
//		TSolMod* activity_model_point_3 = new TELVIS( &sdc1, aM, aZ, Rhow, Epsw );
//		activity_model_point_3->PTparam();
//		activity_model_point_3->MixMod();
	    gam_3 = sdc1.arlnGam[ species_gamma_id ];
	    //gam_3 = 0.5 * (sdc1.arlnGam[ 0 ] + sdc1.arlnGam[ 1 ]);
cout<<"FD 3 	aM["<<species_conc_id<<"] = "<< aM[ species_conc_id ]<<" | gam_3 = "<< gam_3 <<endl;
//		delete activity_model_point_3;


	    DlnactDm = ( -3*gam_1 + 4*gam_2 - gam_3 )/( 2*h );
	    //DlnactDm = ( gam_2 - gam_1 ) / h ;

		// integrate over molality of species multiplied with the derivative of the natural logarithm of activity coefficient of species 
	    DlnactDm = m_spec_current * DlnactDm;

	    aM[ species_conc_id ] 	= m_old;

	return DlnactDm;
	}


	// partial molar excess volume of solute
	double FinDiffVol( double m_j, int j, SolutionData &sd, double* aM, double* aZ, double* Rhow, double* Epsw, TNode* node_pointer, int* StoicIons )
	{
		// Get parameter array dimension as specified in GEMS3K input file
		long index_phase;
		index_phase = 0;

		long NPar, NPcoef, MaxOrd, NComp, NP_DC;
		double Pp = 0., epsW, rhoW;
		double lnGam_high = 0., lnGam_low = 0., FinDiff_cation = 0., FinDiff_anion = 0., FinDiffVolume = 0.0;
		double P_diff = node_pointer->cP() * 0.2;	// pressure in pascal
		double P_old  = node_pointer->cP();
		int i;		

		node_pointer->Get_NPar_NPcoef_MaxOrd_NComp_NP_DC ( NPar, NPcoef, MaxOrd, NComp, NP_DC, index_phase );
		double* lngam   = new (nothrow) double[ NComp ];
		if( !lngam ) { cout<<"Ferdl: optimization.cpp: lngam: ERROR MEMORY COULD NOT BE ALLOCATED !! "<<endl; exit(1); }
		for( i=0; i<NComp; i++ )
		{
			lngam[i]    = 100.0;
		}	

		TSolMod* activity_model_point = NULL;

		aM[ j ]   = (StoicIons[0]*m_j);
		aM[ j+1 ] = (StoicIons[1]*m_j);
		
#ifdef GEMSFIT_DEBUG
cout << "m_j = " << m_j << endl;
cout << "New concentration of Ca+2 = " << node_pointer->Get_nDC(  j  ) << endl;
cout << "New concentration of Cl-  = " << node_pointer->Get_nDC( j+1 ) << endl;
#endif		

		// Cation contribution -------------------------------------------------------------
		// Cation lnGam high =========================================
		Pp = P_old + P_diff;		
		sd.P_bar = Pp;
#ifdef GEMSFIT_DEBUG
cout << " P_old = " << P_old << " Pp = " << Pp << " | sd.T_k = " << sd.T_k << endl;
#endif
		epsW 	= 	node_pointer->EpsH2Ow( Pp, sd.T_k );
#ifdef GEMSFIT_DEBUG
cout << "epsW = " << epsW << endl;
#endif
		if( epsW == 0 )
		{
			std::cout<<" !!!!    Calling node->EpsH2Ow returned 0 !!!!"<<std::endl;
			cout << " !!!!    P [pascal] = " << Pp << endl;
			cout << " !!!!    T [Kelvin] = " << sd.T_k << endl;
			exit(1);
		}
		rhoW 	= 	node_pointer->DenH2Ow( Pp, sd.T_k );
		if( rhoW==0 )
		{
			std::cout<<" !!!!    Calling node->DenH2Ow returned 0 !!!!"<<std::endl;
			cout << " !!!!    P [pascal] = " << Pp << endl;
			cout << " !!!!    T [Kelvin] = " << sd.T_k << endl;
			exit(1);
		}		
		Rhow[0]	=	rhoW/1000;
		Epsw[0]	=  	epsW;
//        activity_model_point = new TELVIS( &sd, aM, aZ, Rhow, Epsw );
		activity_model_point->PTparam();
		activity_model_point->MixMod();
		activity_model_point->Get_lnGamma( lngam );
		lnGam_high = lngam[ 0 ]; 
#ifdef GEMSFIT_DEBUG
cout << "Cation: lnGam_high = " << lnGam_high << endl;
#endif
		delete activity_model_point;

		// Cation lnGam low =========================================
		Pp			= P_old - P_diff;
		sd.P_bar = Pp;
		epsW 	= 	node_pointer->EpsH2Ow( Pp, sd.T_k );
		if( epsW == 0 )
		{
			std::cout<<" !!!!    Calling node->EpsH2Ow returned 0 !!!!"<<std::endl;
			cout << " !!!!    P [pascal] = " << Pp << endl;
			cout << " !!!!    T [Kelvin] = " << sd.T_k << endl;
			exit(1);
		}
		rhoW 	= 	node_pointer->DenH2Ow( Pp, sd.T_k );
		if( rhoW==0 )
		{
			std::cout<<" !!!!    Calling node->DenH2Ow returned 0 !!!!"<<std::endl;
			cout << " !!!!    P [pascal] = " << Pp << endl;
			cout << " !!!!    T [Kelvin] = " << sd.T_k << endl;
			exit(1);
		}		
		Rhow[0]	=	rhoW/1000;
		Epsw[0]	=  	epsW;
//		activity_model_point = new TELVIS( &sd, aM, aZ, Rhow, Epsw );
		activity_model_point->PTparam();
		activity_model_point->MixMod();
		activity_model_point->Get_lnGamma( lngam );
		lnGam_low = lngam[ j ]; 
#ifdef GEMSFIT_DEBUG
cout << "Cation: lnGam_low = " << lnGam_low << endl;
#endif
		delete activity_model_point;

		FinDiff_cation 		= (lnGam_high - lnGam_low)/(2*P_diff);
#ifdef GEMSFIT_DEBUG
cout<<"FinDiff_cation = "<<FinDiff_cation<<endl;
#endif
		
		// Anion contribution -------------------------------------------------------------
		// Anion lnGam high =========================================
		Pp = P_old + P_diff;
		sd.P_bar = Pp;
		epsW 	= 	node_pointer->EpsH2Ow( Pp, sd.T_k );
		if( epsW == 0 )
		{
			std::cout<<" !!!!    Calling node->EpsH2Ow returned 0 !!!!"<<std::endl;
			cout << " !!!!    P [pascal] = " << Pp << endl;
			cout << " !!!!    T [Kelvin] = " << sd.T_k << endl;
			exit(1);
		}
		rhoW 	= 	node_pointer->DenH2Ow( Pp, sd.T_k );
		if( rhoW==0 )
		{
			std::cout<<" !!!!    Calling node->DenH2Ow returned 0 !!!!"<<std::endl;
			cout << " !!!!    P [pascal] = " << Pp << endl;
			cout << " !!!!    T [Kelvin] = " << sd.T_k << endl;
			exit(1);
		}		
		Rhow[0]	=	rhoW/1000;
		Epsw[0]	=  	epsW;
//		activity_model_point = new TELVIS( &sd, aM, aZ, Rhow, Epsw );
		activity_model_point->PTparam();
		activity_model_point->MixMod();
		activity_model_point->Get_lnGamma( lngam );
		lnGam_high = lngam[ j+1 ]; 
		delete activity_model_point;

		// Cation lnGam low =========================================
		Pp			= P_old - P_diff;
		sd.P_bar = Pp;
		epsW 	= 	node_pointer->EpsH2Ow( Pp, sd.T_k );
		if( epsW == 0 )
		{
			std::cout<<" !!!!    Calling node->EpsH2Ow returned 0 !!!!"<<std::endl;
			cout << " !!!!    P [pascal] = " << Pp << endl;
			cout << " !!!!    T [Kelvin] = " << sd.T_k << endl;
			exit(1);
		}
		rhoW 	= 	node_pointer->DenH2Ow( Pp, sd.T_k );
		if( rhoW==0 )
		{
			std::cout<<" !!!!    Calling node->DenH2Ow returned 0 !!!!"<<std::endl;
			cout << " !!!!    P [pascal] = " << Pp << endl;
			cout << " !!!!    T [Kelvin] = " << sd.T_k << endl;
			exit(1);
		}		
		Rhow[0]	=	rhoW/1000;
		Epsw[0]	=  	epsW;
//		activity_model_point = new TELVIS( &sd, aM, aZ, Rhow, Epsw );
		activity_model_point->PTparam();
		activity_model_point->MixMod();
		activity_model_point->Get_lnGamma( lngam );
		lnGam_low = lngam[ j+1 ]; 
		delete activity_model_point;

		FinDiff_anion 		= (lnGam_high - lnGam_low)/(2*P_diff);
#ifdef GEMSFIT_DEBUG
cout<<"FinDiff_anion = "<<FinDiff_anion<<endl;
#endif

		// Sum of Finite Difference --------------------------------------------------
		//FinDiff = (stoic_cation+stoic_anion)*(stoic_cation*FinDiff_cation + stoic_anion*FinDiff_anion);
		FinDiffVolume = (StoicIons[0]*FinDiff_cation + StoicIons[1]*FinDiff_anion);

		Pp			= P_old;
		sd.P_bar = Pp;

#ifdef GEMSFIT_DEBUG
cout<<"FinDiffVolume = "<<FinDiffVolume<<endl;
#endif

		delete[] lngam;

		return FinDiffVolume;
	}


	// Numerical Integration code from Numerical recipes in C (4th edition).
	double trapzd( const double m_infdil, const double m_j, int& n, long int& species_gamma_id, long int& species_conc_id, int select, SolutionData &sdc2, double* aM, double* aZ, double* Rhow, double* Epsw, TNode* node_pointer, int* StoicIons )
	{
		    double x,tnm,sum,del = 0.0;
		    static double s;
		    int it,j = 0;

		    // select = 0: Finite differences over molality of electrolyte -> compute osmotic coefficient
		    if (select == 0)
		    {
		            if( n == 1 )
		            {
		                    return (s=0.5*(m_j-m_infdil)*( FinDiff( m_infdil, species_gamma_id, species_conc_id, sdc2, aM, aZ, Rhow, Epsw ) + FinDiff( m_j, species_gamma_id, species_conc_id, sdc2, aM, aZ, Rhow, Epsw ) ));
		            }
		            else
		            {
		                    for( it=1,j=1;j<n-1;j++ ) it <<= 1;
		                    tnm=it;
		                    del=(m_j-m_infdil)/tnm;
		    //		This is the spacing of the points to be added.
		                    x=m_infdil+0.5*del;
		                    for (sum=0.0,j=1;j<=it;j++,x+=del) sum += FinDiff( x, species_gamma_id, species_conc_id, sdc2, aM, aZ, Rhow, Epsw );
		                    s=0.5*(s+(m_j-m_infdil)*sum/tnm);
		    //		This replaces s by its refined value.
		                            return s;
		            }
		    }
		    // select = 1: Finite differences over pressure -> compute apparent molar volume
		    else if (select == 1)
		    {

		            if (n == 1)
		            {
		                    double lower_bound = FinDiffVol( m_infdil, species_conc_id, sdc2, aM, aZ, Rhow, Epsw, node_pointer, StoicIons );
		                    double upper_bound = FinDiffVol( m_j, species_conc_id, sdc2, aM, aZ, Rhow, Epsw, node_pointer, StoicIons );
		                    return (s=0.5*(m_j-m_infdil)*(lower_bound+upper_bound));
		            }
		            else
		            {
		                    for (it=1,j=1;j<n-1;j++) it <<= 1;
		                    tnm=it;
		                    del=(m_j-m_infdil)/tnm;
		    //		This is the spacing of the points to be added.
		                    x=m_infdil+0.5*del;
		                    for (sum=0.0,j=1;j<=it;j++,x+=del) sum += FinDiffVol( x, species_conc_id, sdc2, aM, aZ, Rhow, Epsw, node_pointer, StoicIons );
		                    s=0.5*(s+(m_j-m_infdil)*sum/tnm);
		    //		This replaces s by its refined value.
		                            return s;
		            }
		    }
	}


	//	Returns the integral of the function func from a to b. The parameters EPS can be set to the
	//	desired fractional accuracy and JMAX so that 2 to the power JMAX-1 is the maximum allowed
	//	number of steps. Integration is performed by Simpson’s rule.
	// Numerical Integration code from Numerical recipes in C (4th edition).
	double qsimp( const double m_infdil, const double m_j, long int& species_gamma_id, long int& species_conc_id, int select, SolutionData &sdc3, double* aM, double* aZ, double* Rhow, double* Epsw, TNode* node_pointer, int* StoicIons )
	{
		    int k;
		    double s,st,ost,os,EPS,JMAX;
		    EPS = 1.0e-5;
		    JMAX = 15;
		    ost = os = -1.0e30;

		    for( k=1;k<=JMAX;k++ )
		    {
		            st=trapzd( m_infdil, m_j, k, species_gamma_id, species_conc_id, select, sdc3, aM, aZ, Rhow, Epsw, node_pointer, StoicIons );
		            if( isnan(st) ) break;
		
					s=(4.0*st-ost)/3.0;
				    	if( k > 6 )
		            //	Avoid spurious early convergence.
		                if( fabs(s-os) < EPS*fabs(os) || (s == 0.0 && os == 0.0) ) return s;
		            os=s;
		            ost=st;
		    }
		    cout<<"Too many steps in routine qsimp"<<endl;

		    return 77777777777777777777777.0;
	}



	// Compute Law-of-Mass-Action
	void LMA_basic( vector<double> CompMol, vector<double> MeasMol, SolutionData& sd, double* aM, double* aZ, double* RhoW, double* EpsW, vector<int> Stoic, vector<int> DC_id, double Ksp, int nH2O )
	{

		typedef struct 
		{
		 	vector<double> CompMol; 
		 	vector<double> MeasMol; 
			SolutionData& sd;
			double* aM; 
			double* aZ; 
			double* RhoW; 
			double* EpsW; 
			vector<int> Stoic; 
			vector<int> DC_id;
			double Ksp; 
			int nH2O;
		} LMA_data;

		LMA_data *lma_data = NULL;

		// copy data into container
	 	lma_data->CompMol = CompMol; 
	 	lma_data->MeasMol = MeasMol; 
		lma_data->sd = sd;
		lma_data->aM = aM; 
		lma_data->aZ = aZ; 
		lma_data->RhoW = RhoW; 
		lma_data->EpsW = EpsW; 
		lma_data->Stoic = Stoic; 
		lma_data->Ksp = Ksp; 
		lma_data->nH2O = nH2O;

		// sum of squared residuals
		double SSR = 0.;

		// guess vector -> use values of measurements as initial guesses
		vector<double> optv = MeasMol;

		// initialize boundaries
		vector<double> OptLoBounds(1e-10,MeasMol.size());
		vector<double> OptUpBounds(100,MeasMol.size());

		// initialize optimization object
		nlopt::opt opt_lma( nlopt::LN_COBYLA, MeasMol.size() ); 

		// empty gradient container
		std::vector<double> grad;

		// assign bounds
		opt_lma.set_lower_bounds( OptLoBounds );
		opt_lma.set_upper_bounds( OptUpBounds );

		// specify relative tolerance tolerance on function value
		opt_lma.set_xtol_rel( 1e-6 );
		
		// specify absolute tolerance on function value
		opt_lma.set_xtol_abs( 1e-6 );

		// maximum number of iterations
		opt_lma.set_maxeval( 100000 );

		// call LMA wrapper
		opt_lma.set_min_objective( LMA_objective_function_callback, lma_data );

        // run optimization 
		nlopt::result result = opt_lma.optimize( optv, SSR );

		// check results
			if( result < 0 ) 
			{
				std::cout<<endl;
				std::cout<<"   !!!  nlopt failed  !!!   "<<std::endl;
				std::cout<<"   !!!  error code:   "<<result<<std::endl;

				switch (result)
				{
					case -5:
						cout<<" Halted because of a forced termination: the user called nlopt_force_stop(opt) on the optimization’s nlopt_opt object opt from the user’s objective function or constraints. "<<endl;
						break;
					case -4:
						cout<<" Halted because roundoff errors limited progress. (In this case, the optimization still typically returns a useful result.) "<<endl;
						break;
					case -3:
						cout<<" Ran out of memory. "<<endl;
						break;
					case -2:
						cout<<" Invalid arguments (e.g. lower bounds are bigger than upper bounds, an unknown algorithm was specified, etc.). "<<endl;
						break;
					case -1:
						cout<<" Generic failure code. "<<endl;
						break;
				}		
				std::cout<<endl;
			}
			else 
			{

				const char path[200] = "output_GEMSFIT/myFIT.out";
				ofstream fout;
				fout.open(path, ios::app);						

				if( fout.fail() )
				{ cout<<"Output fileopen error"<<endl; exit(1); }

				fout<<" LMA: NLopt return code: "<<result<<endl;

				switch (result)
				{
					case 1:
						fout<<" Generic success return value. "<<endl;
						break;
					case 2:
						fout<<" Optimization stopped because stopval was reached. "<<endl;
						break;
					case 3:
						fout<<" Optimization stopped because ftol_rel or ftol_abs was reached. "<<endl;
						break;
					case 4:
						fout<<" Optimization stopped because xtol_rel or xtol_abs was reached. "<<endl;
						break;
					case 5:
						fout<<" Optimization stopped because maxeval was reached. "<<endl;
						break;
					case 6:
						fout<<" Optimization stopped because maxtime was reached. "<<endl;
						break;
				}		

				fout<<"pid "<<pid<<", found minimum at <<f( "; 			
				for( int i=0; i< (int) optv.size(); i++ )
				{	
					fout<<optv[i]<<" "; 
				}
				fout<<") = "<< SSR <<std::endl;
				fout.close();
			}

		// copy optimized values to CompMol
		CompMol = optv;		
	}



	double LMA_objective_function_callback( const std::vector<double> &opt, std::vector<double> &grad, void *struc_data )
	{

		// 
		double water_activity = 0.;
		double computed_value = 0.;


		typedef struct 
		{
			vector<double> CompMol;
		 	vector<double> MeasMol; 
			SolutionData& sd;
			double* aM; 
			double* aZ; 
			double* RhoW; 
			double* EpsW; 
			vector<int> Stoic; 
			vector<int> DC_id; 
			double Ksp; 
			int nH2O;
		} LMA_data;

		LMA_data *lma_data = reinterpret_cast<LMA_data*>(struc_data);

		// Sum of Squared Residuals
		double SSR = 0.;

		// Put guess values into molality vector
		lma_data->aM[ lma_data->DC_id[2] ] = opt[0] * lma_data->Stoic[2];
		lma_data->aM[ lma_data->DC_id[3] ] = opt[0] * lma_data->Stoic[3];
	
		// vector of computed activity coefficients
		vector<double> comp_actcoeff;	

		// initialize instance of TSolMod subclass 
//		TSolMod* activities = new TELVIS( &lma_data->sd, lma_data->aM, lma_data->aZ, lma_data->RhoW, lma_data->EpsW );
		
		// Correct SolMod instance to the new pressure and temperature
//		activities->PTparam();

		// Compute activity coefficient at new T-P-X
//		activities->MixMod();

#ifdef GEMSFIT_DEBUG
cout << " LMA wrapper: sd.arlnGam[0] = " << sd.arlnGam[0] <<endl;
#endif

		// store computed activity coefficients 
		comp_actcoeff.push_back( exp( lma_data->sd.arlnGam[ lma_data->DC_id[2] ] ) );
		comp_actcoeff.push_back( exp( lma_data->sd.arlnGam[ lma_data->DC_id[3] ] ) );


		// compute water activity
		if( lma_data->nH2O )
		{
			// compute water activity
		}
		
		// compute solubility 
		computed_value = sqrt( lma_data->Ksp / ( exp( comp_actcoeff[0] * comp_actcoeff[1] * pow( water_activity, lma_data->nH2O ) ) ) );

		SSR = ( lma_data->MeasMol[0] - computed_value ) * ( lma_data->MeasMol[0] - computed_value );

//		delete activities;

	return SSR;
	}




}// namespace opti
