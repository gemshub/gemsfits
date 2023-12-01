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
#include <thread>
#include <shared_mutex>
#include <algorithm>
#include "node.h"
#include "v_json.h"
#include "optimization.h"

//#ifdef USE_MPI
#include "omp.h"
//#endif


// The Data_Manager class retrieves and stores optimization-specific data from the GEMSFIT input file
// as well as measurement data from EJDB local database.
class Data_Manager : public TNode
{
    public:
        /**
        * Constructor for the Data_Manager class.
        * Function reads parameters for database connection from the input file,
        * creates a std::vector of pointers to instances of the measurement data struct object (samples),
        * then it retrieves measurement data from EJDB local databse files.
        *
        * @author DM
        * @date 06.11.2012
        */
        Data_Manager( );
//        /**
//        * Constructor for the Data_Manager class.
//        * Used in the gemsfit2 -init mode
//        * @author DM
//        * @date 08.05.2013
//        */
//        Data_Manager( int );

        /**
        * Destructor of the Data_Manager class. This functions deletes frees the memory
        * reserved for the measurement data struct creates by the constructor of Data_Manager.
        *
        * @author DM
        * @date 06.11.2012
        */
        virtual ~Data_Manager( );

        mutable std::shared_mutex select_query_mutex;

        typedef std::vector<int>     int_v;
        typedef std::vector<double>  double_v;
        typedef std::vector<std::string>  string_v;
        typedef std::vector<bool>    bool_v;

        std::vector<std::shared_ptr<TNode>> NodT;
        std::vector<std::shared_ptr<double[]>> bICv;

        /// JSON object for building the target function
        std::string DataTarget;
        std::string DataLogK;
        std::vector<std::string> LogK;
        std::string DataSyn;
        std::string OptSet;
        double h_datasetlist;

        int MPI = 1; /// number of paralell threads

        double LimitOfDetection; /// Experimetal limit of detection
        double minimum_value; /// Minimum measured avlue

        std::vector<double> TP_pairs[2]; /// Stores unique TP pairs of the experiments. TP_pairs[0] temperature; TP_pairs[1] pressure.

        // structure holding one experimental set composed of more samples (experimental runs)

            struct samples                  /// data for experimental samples
            {
                int idsample;
                std::string sample;              /// ID/name of this experimental sample
                std::string expdataset;          /// ID/name of set of experimental data
                std::string Type;                /// mineral assemblage
                double weight;
                double sT;                  /// temperature of experimental sample
                std::string Tunit;
                double sP;                  /// pressure of experimental sample
                std::string Punit;
                double sV;                  /// (system) volume of experimental sample
                std::string Vunit;

                // defines bulk composition of chemical system for this experiment 2nd level in EJDB
                struct components
                {
                    std::string comp;         /// formula defining PCO stoichiometry (GEM formula syntax)
                    double Qnt;         /// quantity (to be added to system bulk composition)
                    double Qerror;       /// error (uncertainty) of quantity in the same units
                    std::string Qunit;        /// units of measurement of quantity { 'g' or 'mol' (default) or … }
                };
                std::vector<std::shared_ptr<components>> sbcomp;

                // defines properties for this experiment 2nd level in EJDB
                struct properties
                {
                    std::string prop;         /// proparty name
                    double Qnt;         ///  value
                    double Qerror;       /// error (uncertainty) of quantity in the same units
                    std::string Qunit;        /// units of measurement of quantity
                };
                std::vector<std::shared_ptr<properties>> props;

                struct phases                         /// data for phases characterised (measured) in this experiment 2nd level in EJDB
                {
                    int idphase;
                    std::string phase;                     /// phase ID (name)
                    // measured properties of a phase 3rd level in EJDB
                    struct prop
                    {
                        std::string property;                /// known bulk properties of the phase / property of phase (pH, amount, volume, sarea, etc.)
                        double Qnt;                    /// value
                        double Qerror;                  /// error
                        std::string Qunit;                   /// units
                    };
                    std::vector<std::shared_ptr<prop>> phprop;

                    // composition of the phases in elements
                    std::vector<std::shared_ptr<components>> phIC;

                    std::vector<std::shared_ptr<components>> phMR;

                    // dcomps (end member, phase component) 3rd level in EJDB
                    struct dcomps
                    {
                        std::string DC;               /// name of chemical dcomps (end member, phase component)
                        struct dcprop
                        {
                            std::string property;
                            double Qnt;                 /// measured quantity/concentration of phase dcomps
                            double Qerror;               /// error
                            std::string Qunit;                /// units
                        };
                        std::vector<std::shared_ptr<dcprop>> DCprop; /// std::vector of dependent components porperties
                    };
                    std::vector<std::shared_ptr<dcomps>> phDC;       /// std::vector of dependent components in a phase
                };
                std::vector<std::shared_ptr<phases>> expphases;      /// std::vector of phases measured in one experiment
            };
            std::vector<std::shared_ptr<samples>> experiments;         /// std::vector of samples from one experimental dataset


            struct DataSynonyms
            {
                std::string GemsName;
                std::vector<std::string> syn;
            };

            struct PhSyn
            {
                std::string GemsName;
                std::vector<std::string> syn;
                std::vector<DataSynonyms> SynDCs;
            };

            std::vector<PhSyn> SynPHs;
            std::vector<DataSynonyms> SynPHPs;
            std::vector<DataSynonyms> SynDCPs;
            std::vector<DataSynonyms> SynProps;

            void get_DataSyn ();

            void check_Syn ();
//        /**
//        * Writes database connection parameters in the template input file
//        * @author SD
//        * @date 14.03.2013
//        */
//        void out_db_specs_txt( bool with_comments, bool brief_mode );

protected:
            std::string default_unit_to_property(const std::string &property_name) const;
private:

        /// get measurement data from EJDB (default) (0) CSV file (1) or PostgreSQL database (2)
        int datasource;

        /// name of CSV file containing measurement data
        std::string CSVfile;

        // Database connection parameters
        /// PostgreSQL/EJDB database: database name (path)
        std::string DBname;

        /// PostgreSQL/EJDB database: table/colection name with solubility experiments
        std::string collection;

        /// PostgreSQL database: username
        std::string username;

        /// PostgreSQL database: password
        std::string passwd;

        /// PostgreSQL database: URL of psql server
        std::string psql_server;

        /// EJDB data selection query
        std::string DataSelect;

        int mode;

        /**
        * get database connection parameters
        * @author SD
        * @date 14.03.2013
        */
        void get_db_specs_txt( );

//        void define_db_specs( );

        /**
        * Read measurement data from EJDB database
        * @author DM
        * @date 19.04.2013
        */
        void get_EJDB( );

        /**
        * Puts the in BSON format (e.g. returned from an EJDB query) into Data Manager class
        * @author DM
        * @param *f
        * @param *data BSON object
        * @param pos position of the BSON object in the std::vector experiments
        * @date 19.04.2013
        */
        void bson_to_Data_Manager(const std::string& data, int pos);

        /**
        * Gest the distinct T-P pairs form the experimental data
        * @author DM
        * @date 19.04.2013
        */
        void get_distinct_TP( );
};




#endif // NEW_DATA_MANGER_H
