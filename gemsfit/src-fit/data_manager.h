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


// Manager of the Measured data from different type of files


class Data_Manager
{

	public:
		// Constructor
		Data_Manager( );

		// Destructor		
		virtual ~Data_Manager( );

		// struct holding measurement data
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

		// get PostgreSQL database connection parameters
		void get_db_specs( );

		// retrieve data from PostgreSQL database
		void get_DB( measdata* sysdata );	

		// retrieve data from CSV file	// comma separated values
		void get_CSV( measdata* sysdata );
		
		// read species from database and form vectors of independent component names
		void get_ic( measdata* sysdata );

	private:

		// get measurement data from CSV file (0) or PostgreSQL database (1)
		int datasource;

		// name of CSV file containing measurement data
		string CSVfile;

		// Database connection parameters
		// database name			
		string DBname;

		// table name
		string tablename;

		// username
		string username;

		// password
		string passwd;
	
        // psql server
        string psql_server;    

};

#endif
