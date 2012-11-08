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
 *	@file data_manager.h
 *
 *	@brief this header file contains defitions of the data manager classes,
 *	which retrieves and stores data from the GEMSFIT input file as well as
 *	the measurement data file or PostgreSQL server. 
 *
 *	@author Ferdinand F. Hingerl, G. Dan Miron
 *
 * 	@date 09.04.2012
 *
 *
 */

#ifndef _data_manager_h_
#define _data_manager_h_


#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <iterator>
#include <limits>
#include <iomanip>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/find.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/find_format.hpp>

using namespace std;
using namespace boost;

///	The SS_Data_Manager class retrieves and stores optimization-specific data from the SS_GEMSFIT input file as well as measurement data from a *.csv file or PostgreSQL server.
class SS_Data_Manager
{

    public:
        /**
        * Constructor for the Data_Manager class.
        * Function reads parameters for database connection from SS_GEMSFIT_input.dat,
        * creates a pointer to an instance of the measurement data struct object (measdata),
        * then it retrieves measurement data from the PosgreSQL server or a CSV file.
        *
        * @author FFH modified DM
        * @date 06.11.2012
        */
        SS_Data_Manager( );

        /**
        * Destructor of the Data_Manager class. This functions deletes frees the memory reserved for the measurement data struct creates by the constructor of Data_Manager.
        *
        * @author DM
        * @date 06.11.2012
        */
        virtual ~SS_Data_Manager( );

        /// struct holding 1 experiment data
        struct experiment
        {
            typedef vector<int>     int_v;
            typedef vector<double>  double_v;
            typedef vector<string>  string_v;

            int id_exp;
            string name;
            string_v component_name;       // name of the input components xa_ in GEMS
            string_v units;                // g, moles - for salts
            double_v component_amount;     // like b vector in GEMS - here in grams or moles
            // vector<string> elements - for withc solubility is measured
            int_v sol_is_measured;         // for each element if solubility is measured experimentally = true, this way we know for which solubilities to ask GEMS to provide.
            double_v solubility;           // related to components, if not measured = 0;
            string_v sol_units;            // log molality
            double_v error_sol;            // error in the measured solubility
            int TC;                        // temperature in C
            int PG;                        // pressure in bars
            string reference;              // reference name or code?

        };


        vector<experiment*> alldata;       // vector with pointers to the all experiments

        /**
        * get PostgreSQL database connection parameters
        * @author DM
        * @date 06.11.2012
        */
        void get_db_specs( );

        /**
        * Read measurement data from PosgreSQL server
        * @param sysdata   pointer to measdata struct which holds all measurement data.
        * @author DM
        * @date 06.11.2012
        */
        void get_DB( vector<experiment*> alldata );

        /**
        * Read measurement data from CSV file
        * @param sysdata   pointer to measdata struct which holds all measurement data.
        * @author DM
        * @date 06.11.2012
        */
        void get_CSV( vector<experiment*> alldata );

        /**
        * read species from database and form vectors of independent component names
        * @param sysdata   pointer to measdata struct which holds all measurement data.
        * @author DM
        * @date 06.11.2012
        */
        void get_ic( vector<experiment*> alldata );

    private:

        /// get measurement data from CSV file (0) or PostgreSQL database (1)
        int datasource;

        /// name of CSV file containing measurement data
        string CSVfile;

        // Database connection parameters
        /// PostgreSQL database: database name
        string DBname;

        /// PostgreSQL database: table name with solubility experiments
        string tablename;

        /// PostgreSQL database: table name with reaction dependent species
        string RDCtablename; // to be implemented

        /// PostgreSQL database: username
        string username;

        /// PostgreSQL database: password
        string passwd;

        /// PostgreSQL database: URL of psql server
        string psql_server;

};

///	The Data_Manager class retrieves and stores optimization-specific data from the GEMSFIT input file as well as measurement data from a *.csv file or PostgreSQL server. 
class Data_Manager
{

	public:
		/** 
		* Constructor for the Data_Manager class. 
		* Function reads parameters for database connection from GEMSFIT_input.dat,
		* creates a pointer to an instance of the measurement data struct object (measdata),
		* then it retrieves measurement data from the PosgreSQL server or a CSV file.
		* Finally, the species from database are retrived and form vectors of independent component names
		*
		* @author FFH
		* @date 09.04.2012
		*/	
		Data_Manager( );

		/**
		* Destructor of the Data_Manager class. This functions deletes frees the memory reserved for the measurement data struct creates by the constructor of Data_Manager.		
		*
		* @author FFH
		* @date 09.04.2012
		*/	
		virtual ~Data_Manager( );

		/// struct holding measurement data
		struct measdata
		{
			typedef vector<int>     int_v;
			typedef vector<double>  double_v;
			typedef vector<string>  string_v;
			int_v 		ende;
			double_v 	temperature;
			double_v 	pressure;
			double_v 	molality_1;
			double_v 	molality_2;
			int_v 		code;
			double_v 	val;
			double_v 	other_1;
			double_v 	other_2;
			double_v 	other_3;
			double_v 	other_4;
			double_v 	error;
			string_v 	species_1_1;
			string_v 	species_1_2;
			string_v 	species_1_3;
			string_v 	species_1_4;
			string_v 	species_2_1;
			string_v 	species_2_2;
			string_v 	species_2_3;
			string_v 	species_2_4;
			int_v 		stoic_1_1;	
			int_v 		stoic_1_2;	
			int_v 		stoic_1_3;	
			int_v 		stoic_1_4;	
			int_v 		stoic_2_1;	
			int_v 		stoic_2_2;	
			int_v 		stoic_2_3;	
			int_v 		stoic_2_4;	
			string_v 	id;

			// vectors of independent component names
			string_v	ic_1;
			string_v	ic_2;
			string_v	ic_3;
			string_v	ic_4;
		};

		measdata* sysdata;

		/** 
		* get PostgreSQL database connection parameters
		* @author FFH
		* @date 09.04.2012
		*/	
		void get_db_specs( );

		/** 
		* Read measurement data from PosgreSQL server
		* @author FFH
		* @param sysdata   pointer to measdata struct which holds all measurement data.
		* @date 09.04.2012
		*/	
		void get_DB( measdata* sysdata );	

		/** 
		* Read measurement data from CSV file
		* @author FFH
		* @param sysdata   pointer to measdata struct which holds all measurement data.
		* @date 09.04.2012
		*/	
		void get_CSV( measdata* sysdata );
		
		/** 
		* read species from database and form vectors of independent component names
		* @author FFH
		* @param sysdata   pointer to measdata struct which holds all measurement data.
		* @date 09.04.2012
		*/	
		void get_ic( measdata* sysdata );

	private:

		/// get measurement data from CSV file (0) or PostgreSQL database (1)
		int datasource;

		/// name of CSV file containing measurement data
		string CSVfile;

		// Database connection parameters
		/// PostgreSQL database: database name			
		string DBname;

		/// PostgreSQL database: table name
		string tablename;

		/// PostgreSQL database: username
		string username;

		/// PostgreSQL database: password
		string passwd;
	
        /// PostgreSQL database: URL of psql server
        string psql_server;    

};

#endif
