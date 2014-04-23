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

#include <string>
using namespace std;

// MPI variable declarations
//namespace mpispace
//{
    extern int pid;			    // Process rank
    extern int p;				// Number of processes
    extern int master_counter;
    extern int slave_counter;
//}

//    extern int sizeTP;

    extern int	n_procs_boost;
    extern int	proc_id_boost;

//    struct outField_ /// Internal descriptions of fields
//     {
//       string name; ///< name of field in structure
//       string comment;

//    };


//    outField_ Error_fields[2] =
//    {
//        { "Error_TK_param", " When fitting TK (geo-thermometry) only one system/experiment/sample si alowed at a time " },
//        { "Error_P_param","When fitting TK (geo-thermometry) only one system/experiment/sample si alowed at a time " }

//    };

//    typedef enum { /// Field index into outField structure
//        f_Error_TK_param = 0,
//        f_Error_P_param
//    } Error_FIELDS;



#endif // GEMSFIT_GLOBAL_VARIABLES_H
