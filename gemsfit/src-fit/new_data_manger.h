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
        typedef vector<bool>    bool_v;

        std::vector<int> TP_pairs[2]; // UNIQUE!!! TP pairs of the experiments.< Only >The unique values will be extracted form the database.
        // TP_pairs[0] temperature; TP_pairs[1] pressure

        /// structure holding one experimental set composed of more samples (experimental runs)

            struct samples                  // data for experimental samples
            {
                int idsample;
                string sample;              // ID of this experimental sample
                string expdataset;          // ID of set of experimental data
                double sT;                  // temperature for this experimental sample
                string Tunit;
                double sP;                  // pressure for this experimental sample
                string Punit;
                double sV;                  // (system) volume for this experimental sample
                string Vunit;
                // defines bulk composition of chemical system for this experiment 2nd level in EJDB
                string_v comp_name;         // formula defining PCO stoichiometry (GEM formula syntax)
                double_v comp_bQnt;         // quantity (to be added to system bulk composition)
                double_v comp_Qerror;       // error (uncertainty) of quantity in the same units
                string_v comp_Qunit;        // units of measurement of quantity { 'g' or 'mol' (default) or … }

                struct phases                         // data for phases characterised (measured) in this experiment 2nd level in EJDB
                {
                    int idphase;
                    string phase;                     // phase ID (name)
                    // measured properties of a phase 3rd level in EJDB
                    double pQnt;                      // Quantity of this phase in the sample system
                    string_v phprop;                  // known bulk properties of the phase / property of phase (pH, amount, volume, sarea, etc.)
                    double_v phprop_Qnt;              // value
                    double_v phprop_Qerror;           // error
                    string_v phprop_Qunit;            // units
                    // elements measured in a phase 3rd level in EJDB
                    string_v phcomp;                  // name of chemical element (e.g. 'Al')
                    double_v phcomp_eQnt;             // measured quantity/concentration of element (in Qunit)
                    double_v phcomp_Qerror;           // error
                    string_v phcomp_Qunit;            // units of measurment
                    // species (end member, phase component) 3rd level in EJDB
                    string_v phspecies;               // name of chemical species (end member, phase component)
                    double_v phsp_sQnt;               // measured quantity/concentration of phase species
                    double_v phsp_Qerror;             // error
                    string_v phsp_Qunit;              // units

                };
                vector<phases*> expphases;      // vector of phases measured in one experiment
            };
            vector<samples*> experiments;         // vector of samples from one experimental dataset

        /// Handle that marks available data in the experiments to compare with calculated data
            struct Hsamples                  // data for experimental samples
            {
                int idsample;
                string sample;              // ID of this experimental sample
                string expdataset;          // ID of set of experimental data
//                double sT;                  // temperature for this experimental sample
//                string Tunit;
//                double sP;                  // pressure for this experimental sample
//                string Punit;
//                double sV;                  // (system) volume for this experimental sample
//                string Vunit;
                // defines bulk composition of chemical system for this experiment 2nd level in EJDB
                bool_v Hcomp_name;         // 0 if component not available
//                double_v comp_bQnt;         // quantity (to be added to system bulk composition)
                bool_v comp_Qerror;        // 0 if error not specified
//                string_v comp_Qunit;        // units of measurement of quantity { 'g' or 'mol' (default) or … }

                struct Hphases                         // data for phases characterised (measured) in this experiment 2nd level in EJDB
                {
                    int idphase;
                    bool phase;                       // 0 if phase not available
                    // measured properties of a phase 3rd level in EJDB
                    bool pQnt;                        // Quantity of this phase in the sample system
                    bool_v phprop;                  // known bulk properties of the phase / property of phase (pH, amount, volume, sarea, etc.)
//                    double_v phprop_Qnt;              // value
                    bool_v phprop_Qerror;           // error
//                    string_v phprop_Qunit;            // units
                    // elements measured in a phase 3rd level in EJDB
                    bool_v phcomp;                  // name of chemical element (e.g. 'Al')
//                    double_v phcomp_eQnt;             // measured quantity/concentration of element (in Qunit)
                    bool_v phcomp_Qerror;           // error
//                    string_v phcomp_Qunit;            // units of measurment
                    // species (end member, phase component) 3rd level in EJDB
                    bool_v phspecies;               // name of chemical species (end member, phase component)
//                    bool_v phsp_sQnt;               // measured quantity/concentration of phase species
                    bool_v phsp_Qerror;             // error
                    string_v phsp_Qunit;              // units

                };
                vector<Hphases*> Hexpphases;      // vector of phases measured in one experiment
            };
            vector<Hsamples*> Hexperiments;         // vector of samples from one experimental dataset



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
