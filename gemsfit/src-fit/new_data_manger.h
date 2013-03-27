/*
*	 Copyright (C) 2013 by G. Dan Miron (mirondanro@yahoo.com)
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
 *	the measurement from specified databse.
 *
 *	@author G. Dan Miron
 *
 * 	@date 26.03.2013
 *
 *
 */


#ifndef NEW_DATA_MANGER_H
#define NEW_DATA_MANGER_H


#include <vector>
#include <string>
#include "ejdb.h"

using namespace std;


///	The SS_Data_Manager class retrieves and stores optimization-specific data from the SS_GEMSFIT input file as well as measurement data from a *.csv file or PostgreSQL server.
class Data_Manager
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
        Data_Manager( );

        /**
        * Destructor of the Data_Manager class. This functions deletes frees the memory reserved for the measurement data struct creates by the constructor of Data_Manager.
        *
        * @author DM
        * @date 06.11.2012
        */
        virtual ~Data_Manager( );

        typedef vector<int>     int_v;
        typedef vector<double>  double_v;
        typedef vector<string>  string_v;

        std::vector<int> TP_pairs[2]; // UNIQUE!!! TP pairs of the experiments.< Only >The unique values will be extracted form the database.
        // TP_pairs[0] temperature; TP_pairs[1] pressure

        /// structure holding one experimental set composed of more samples (experimental runs)
        struct dataset
        {
            int idset;
            string name;                // name of experimental dataset e.g. Kennedy1950
            string P_range;             // P range of the experimental runs e.g. 0.5-5 kbar
            string V_range;             // V range of the experimental runs e.g. 300-700 C
            string reference;           // reference e.g. Anderson and Burnham 1967
            string comment;             // free text

            struct sample
            {
                int idsample;
                string name;
                int sT;
                int sP;
                int sV;
                // recipe of the sample (experimental run)
                string_v comp_name;         // name of the input components xa_ in GEMS
                double_v comp_amount;       // like b vector in GEMS - here in grams or moles
                double_v comp_error;        // error in measuring the components amount
                string_v comp_units;        // g, moles - for salts

                struct phase
                {
                    int idphase;
                    string name;
                    string_v phaseprop;             // property of phase (pH, amount, volume, sarea, etc.)
                    double_v prop_value;            // value
                    double_v prop_error;            // error of the value
                    string_v prop_units;            // units of the value
                    // elements measured in a phase
                    string_v element;               // name of element in a phase
                    double_v elem_amount;           // amount of an element in a phase
                    double_v elem_error;            // error of amount
                    string_v elem_units;            // units of measurment
                };
                vector<phase*> exp_phases;      // vector of phases measured in one experiment
            };
            vector<sample*> exp_samples;         // vector of samples from one experimental dataset
        };
        vector<dataset*> exp_dataset;            // vector of experimental datasets - all measured data

        /// Handle that marks available data in the experiments to compare with calculated data
        struct Hdataset
        {
//            int idset;
//            string name;                // name of experimental dataset e.g. Kennedy1950
//            string P_range;             // P range of the experimental runs e.g. 0.5-5 kbar
//            string V_range;             // V range of the experimental runs e.g. 300-700 C
//            string reference;           // reference e.g. Anderson and Burnham 1967
//            string comment;             // free text

            struct Hsample
            {
//                int idsample;
//                string name;
//                int sT;
//                int sP;
//                int sV;
                // recipe of the sample (experimental run)
                string_v comp_name;         // name of the input components xa_ in GEMS
                double_v comp_amount;       // like b vector in GEMS - here in grams or moles
                double_v comp_error;        // error in measuring the components amount
                string_v comp_units;        // g, moles - for salts

                struct phase
                {
                    int idphase;
                    string name;
                    string_v phaseprop;             // property of phase (pH, amount, volume, sarea, etc.)
                    double_v prop_value;            // value
                    double_v prop_error;            // error of the value
                    string_v prop_units;            // units of the value
                    // elements measured in a phase
                    string_v element;               // name of element in a phase
                    double_v elem_amount;           // amount of an element in a phase
                    double_v elem_error;            // error of amount
                    string_v elem_units;            // units of measurment
                };
                vector<phase*> exp_phases;      // vector of phases measured in one experiment
            };
            vector<sample*> exp_samples;         // vector of samples from one experimental dataset
        };
        vector<dataset*> exp_dataset;            // vector of experimental datasets - all measured data



        /**
        * get PostgreSQL database connection parameters
        * @author SD
        * @date 14.03.2013
        */
        void get_db_specs_txt( );
        void out_db_specs_txt( bool with_comments, bool brief_mode );
        void define_db_specs( );

        /**
        * Read measurement data from PosgreSQL server
        * @param sysdata   pointer to measdata struct which holds all measurement data.
        * @author DM
        * @date 06.11.2012
        */
        void get_DB( /*vector<experiment *> allexp*/ );

        /**
        * Read measurement data from CSV file
        * @param sysdata   pointer to measdata struct which holds all measurement data.
        * @author DM
        * @date 06.11.2012
        */
        void get_CSV( /*vector<experiment*> allexp*/ );

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




#endif // NEW_DATA_MANGER_H
