/// \file data_manager.h
/// Contains declaration of Data_Manager class used in GEMSFIT for
/// storing experimental data
//
/// \class Data_Manager data_manager.h
///  The data manager class, retrieves and stores data from
///  the GEMSFIT input file as well as experimental data from specified databse.
//
// Copyright (C) 2013 G.D.Miron, D.Kulik
// <GEMS Development Team, mailto:gems2.support@psi.ch>
//
// This file is part of the GEMSFIT2 code for parameterization of thermodynamic
// data and models <http://gems.web.psi.ch/GEMSFIT/>
//
// GEMSIFT2 is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.

// GEMSFIT2 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with GEMSFIT2 code. If not, see <http://www.gnu.org/licenses/>.
//-------------------------------------------------------------------
//


/**
 *	@file data_manager.h
 *
 *	@brief this header file contains defitions of the data manager class,
 *	which retrieves and stores data from the GEMSFIT input file as well as
 *	experimental data form local EJDB database.
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
#include "node.h"
#include "ejdb.h"
#include <algorithm>
#include "optimization.h"

//#ifdef USE_MPI
#include "omp.h"
//#endif

using namespace std;


// The Data_Manager class retrieves and stores optimization-specific data from the GEMSFIT input file
// as well as measurement data from EJDB local database.
class Data_Manager : public TNode
{
    public:
        /**
        * Constructor for the Data_Manager class.
        * Function reads parameters for database connection from the input file,
        * creates a vector of pointers to instances of the measurement data struct object (samples),
        * then it retrieves measurement data from EJDB local databse files.
        *
        * @author DM
        * @date 06.11.2012
        */
        Data_Manager( );
        /**
        * Constructor for the Data_Manager class.
        * Used in the gemsfit2 -init mode
        * @author DM
        * @date 08.05.2013
        */
        Data_Manager( int );

        /**
        * Destructor of the Data_Manager class. This functions deletes frees the memory
        * reserved for the measurement data struct creates by the constructor of Data_Manager.
        *
        * @author DM
        * @date 06.11.2012
        */
        virtual ~Data_Manager( );

        typedef vector<int>     int_v;
        typedef vector<double>  double_v;
        typedef vector<string>  string_v;
        typedef vector<bool>    bool_v;

        vector<TNode*> NodT;

        /// JSON object for building the target function
        string DataTarget;

        int MPI; /// number of paralele threads

        double LimitOfDetection; /// Experimetal limit of detection
        double minimum_value; /// Minimum measured avlue

        std::vector<int> TP_pairs[2]; /// Stores unique TP pairs of the experiments. TP_pairs[0] temperature; TP_pairs[1] pressure.

        // structure holding one experimental set composed of more samples (experimental runs)

            struct samples                  /// data for experimental samples
            {
                int idsample;
                string sample;              /// ID/name of this experimental sample
                string expdataset;          /// ID/name of set of experimental data
                double sT;                  /// temperature of experimental sample
                string Tunit;
                double sP;                  /// pressure of experimental sample
                string Punit;
                double sV;                  /// (system) volume of experimental sample
                string Vunit;

                struct Uconstraints
                {
                    string type;
                    string name;
                    double Qnt;
                };
                vector<Uconstraints*> U_KC;

                struct Lconstraints
                {
                    string type;
                    string name;
                    double Qnt;
                };
                vector<Lconstraints*> L_KC;

                // defines bulk composition of chemical system for this experiment 2nd level in EJDB
                struct components
                {
                    string comp;         /// formula defining PCO stoichiometry (GEM formula syntax)
                    double Qnt;         /// quantity (to be added to system bulk composition)
                    double Qerror;       /// error (uncertainty) of quantity in the same units
                    string Qunit;        /// units of measurement of quantity { 'g' or 'mol' (default) or … }
                };
                vector<components*> sbcomp;

                struct phases                         /// data for phases characterised (measured) in this experiment 2nd level in EJDB
                {
                    int idphase;
                    string phase;                     /// phase ID (name)
                    // measured properties of a phase 3rd level in EJDB
                    struct prop
                    {
                        string property;                /// known bulk properties of the phase / property of phase (pH, amount, volume, sarea, etc.)
                        double Qnt;                    /// value
                        double Qerror;                  /// error
                        string Qunit;                   /// units
                    };
                    vector<prop*> phprop;

                    // composition of the phases in elements
                    vector<components*> phIC;

                    vector<components*> phMF;

                    // dcomps (end member, phase component) 3rd level in EJDB
                    struct dcomps
                    {
                        string DC;               /// name of chemical dcomps (end member, phase component)
                        struct dcprop
                        {
                            string property;
                            double Qnt;                 /// measured quantity/concentration of phase dcomps
                            double Qerror;               /// error
                            string Qunit;                /// units
                        };
                        vector<dcprop*> DCprop; /// vector of dependent components porperties
                    };
                    vector<dcomps*> phDC;       /// vector of dependent components in a phase
                };
                vector<phases*> expphases;      /// vector of phases measured in one experiment
            };
            vector<samples*> experiments;         /// vector of samples from one experimental dataset


        /**
        * Writes database connection parameters in the template input file
        * @author SD
        * @date 14.03.2013
        */
        void out_db_specs_txt( bool with_comments, bool brief_mode );

        /**
        * Interprets 1 level JSON object pointed by the variable key
        * @author DM
        * @param query JSON object
        * @param key JSON object member that we want to have the value/s returned
        * @param result vector of strings representing the values of the key in the JSON object
        * @date 19.04.2013
        */
        void parse_JSON_object( string query, const char *key, vector<string> &result );

        /**
        * Interprets 2 level JSON object in the array arr pointed by the variable key
        * @author DM
        * @param data_ JSON object
        * @param arr JSON array name that we want to parse
        * @param key JSON array-object member that we want to have the value/s returned
        * @param result vector of strings representing the values of the key in the JSON object
        * @date 08.05.2013
        */
        void parse_JSON_array_object( string data_, const char *arr , const char *key, vector<string> &result );



//        /**
//        * Read measurement data from CSV file
//        * @param sysdata   pointer to measdata struct which holds all measurement data.
//        * @author DM
//        * @date 06.11.2012
//        */
//        void get_CSV( /*vector<experiment*> allexp*/ );

    private:

        /// get measurement data from EJDB (default) (0) CSV file (1) or PostgreSQL database (2)
        int datasource;

        /// name of CSV file containing measurement data
        string CSVfile;

        // Database connection parameters
        /// PostgreSQL/EJDB database: database name
        string DBname;

        /// PostgreSQL/EJDB database: table/colection name with solubility experiments
        string collection;

        /// PostgreSQL database: username
        string username;

        /// PostgreSQL database: password
        string passwd;

        /// PostgreSQL database: URL of psql server
        string psql_server;

        /// EJDB data selection querie
        string DataSelect;

        /**
        * get database connection parameters
        * @author SD
        * @date 14.03.2013
        */
        void get_db_specs_txt( );
        void define_db_specs( );

        /**
        * Read measurement data from EJDB database
        * @author DM
        * @date 19.04.2013
        */
        void get_EJDB( );

        /**
        * Returns JSON string from input file
        * @author DM
        * @param key string pointing to the position in the input file of the JSON string e.g <DatSelect>.
        * @date 19.04.2013
        */
        string readin_JSON (string key );

        /**
        * Puts the in BSON format (e.g. returned from an EJDB query) into Data Manager class
        * @author DM
        * @param *f
        * @param *data BSON object
        * @param pos position of the BSON object in the vector experiments
        * @date 19.04.2013
        */
        void bson_to_Data_Manager(FILE *f, const char *data, int pos);

        /**
        * Gest the distinct T-P pairs form the experimental data
        * @author DM
        * @date 19.04.2013
        */
        void get_distinct_TP( );

};




#endif // NEW_DATA_MANGER_H
