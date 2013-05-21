/**
 *	@file gemsfit_global_variables.h
 *
 *	@brief this header file contains definitions of global variables especially
 *  needed for parallel program execution.
 *
 *	@author G. Dan Miron
 *
 * 	@date 08.05.2012
 *
 */



#ifndef GEMSFIT_GLOBAL_VARIABLES_H
#define GEMSFIT_GLOBAL_VARIABLES_H

// MPI variable declarations
//namespace mpispace
//{
    extern int pid;			    // Process rank
    extern int p;				// Number of processes
    extern int master_counter;
    extern int slave_counter;
//}


    extern int	n_procs_boost;
    extern int	proc_id_boost;






#endif // GEMSFIT_GLOBAL_VARIABLES_H
