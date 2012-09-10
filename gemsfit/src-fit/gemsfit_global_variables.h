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
 *	@file gemsfit_global_variables.h
 *
 *	@brief this header file contains definitions of global variables especially 
 *  needed for parallel program execution. 
 *
 *	@author Ferdinand F. Hingerl
 *
 * 	@date 09.04.2012 
 *
 */


#ifndef _global_variables_h_
#define _global_variables_h_

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

#endif

