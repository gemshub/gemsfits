/// \file data_manager.cpp
/// Implementation of reading database parameters form input file
/// and experimental data form databsse
//
// Copyright (C) 2013 G.D.Miron, D.Kulik
// <GEMS Development Team, mailto:gems2.support@psi.ch>
//
// This file is part of the GEMSFIT2 code for parameterization of thermodynamic
// data and models <http://gems.web.psi.ch/GEMSFIT2/>
//
// GEMSIFT is free software: you can redistribute it and/or modify
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
 *	@file data_manager.cpp
 *
 *	@brief this source file contains the implementation of the data manager class,
 *	which retrieves and stores data from thre GEMSFIT2 input file as well as
 *	the measurement form an EJDB local database.
 *
 *	@author G. Dan Miron
 *
 * 	@date 26.03.2013
 *
 */

#include "data_manager.h"
#include "gemsfit_iofiles.h"
#include "keywords.h"
#include <jansson.h>
#include "ejdb.h"
#include <sstream>

// Constructor
Data_Manager::Data_Manager( )
{
    // GEMSFIT logfile
    //const char path[200] = "output_GEMSFIT/SS_GEMSFIT.log";
    ofstream fout;
    fout.open(gpf->FITLogFile().c_str(), ios::app);
    if( fout.fail() )
    { cout<<"Output fileopen error"<<endl; exit(1); }

    // Read parameters for database connection
    fout << "2. data_manager.cpp line 57. Reading database parameter get_db_specs(); " << endl;
    get_db_specs_txt();

    // Read measurement data from PosgreSQL server
    fout << "3. data_manager.cpp line 61. Reading in the data selection query; " << endl;
    // Readin in the data slection query
    DataSelect = readin_JSON("<DataSelect>");
    fout << "4. data_manager.cpp line 64. Reading in the Target function form the input file; " << endl;
    DataTarget = readin_JSON("<DataTarget>");

    // Getting the query result data into the Data_Manager class
    fout << "5. data_manager.cpp line 68. Getting data form the EJDB database; " << endl;
    get_EJDB();

    fout << "7. data_manager.cpp line 71. Getting distinct T and P pairs; " << endl;
    get_distinct_TP();

    fout.close();
}


// Destructor
Data_Manager::~Data_Manager( )
{
    // Delete measurement data vector of experiments
    for (unsigned int i=0; i<experiments.size(); ++i)
    {
        delete experiments[i];
    }
}


// Reading data from EJDB database
void Data_Manager::get_EJDB( )
{
    typedef vector<int>     int_v;
    typedef vector<double>  double_v;
    typedef vector<string>  string_v;

    //    {   "sample" : "",
    //        "expdataset" : ["CHO4", "CH04D"],
    //        "sT" : [100, 1000],
    //        "sP" : [1, 2500]
    //    }

    // GEMSFIT logfile
    ofstream fout;
    fout.open(gpf->FITLogFile().c_str(), ios::app);
    if( fout.fail() )
    { cout<<"Output fileopen error"<<endl; exit(1); }

    string_v out, qsample, usedataset, skipdataset;
    int_v qsT, qsP;

    stringstream ss;
    string sss;

    // processing DataSelect
    if (DataSelect != "all")
    {
        parse_JSON_object(DataSelect, keys::expsample, out);
        qsample = out; // query for selecting samples
        out.clear();

        parse_JSON_object(DataSelect, keys::usedataset, out);
        usedataset = out; // query for selecting expdatasets
        out.clear();

        parse_JSON_object(DataSelect, keys::skipdataset, out);
        skipdataset = out; // query for skiping expdatasets
        out.clear();

        parse_JSON_object(DataSelect, keys::sT, out);
        for (unsigned int i = 0 ; i < out.size() ; i++)
        {
            qsT.push_back( atoi(out.at(i).c_str()) ); // query for selecting T
        }
        out.clear();

        parse_JSON_object(DataSelect, keys::sP, out);
        for (unsigned int i = 0 ; i < out.size() ; i++)
        {
            qsP.push_back( atoi(out.at(i).c_str()) ); // query for selecting P
        }
        out.clear();

        // Build the query in EJDB format

        // create EJDB databse object
        static EJDB *jb;
        jb = ejdbnew();
        // open the database file as a writer JBOWRITER, create new is not existent JBOCREAT, and truncate db on open JBOTRUNC
        if (!ejdbopen(jb, DBname.c_str(), JBOREADER)) {
            cout << "Error opening the database" << endl; exit(1);

        }
        //Get or create collection 'experiments'
        EJCOLL *coll = ejdbcreatecoll(jb, collection.c_str(), NULL);

        // Creat bson query object
        bson bq2;
        bson_init_as_query(&bq2);

        // for selecting expdatasets
        if (!usedataset[0].empty())
        {
            bson_append_start_object(&bq2, keys::expdataset);
            bson_append_start_array(&bq2, "$in");
            for (unsigned int j=0; j<usedataset.size(); ++j)
            {
                ss << j;
                sss = ss.str();
                ss.str("");
                bson_append_string(&bq2, sss.c_str(), usedataset[j].c_str());
            }
            bson_append_finish_array(&bq2);
            bson_append_finish_object(&bq2);
        }

        // for skiping expdatasets
        if (!skipdataset[0].empty())
        {
            bson_append_start_object(&bq2, keys::expdataset);
            bson_append_start_array(&bq2, "$nin");
            for (unsigned int j=0; j<skipdataset.size(); ++j)
            {
                ss << j;
                sss = ss.str();
                ss.str("");
                bson_append_string(&bq2, sss.c_str(), skipdataset[j].c_str());
            }
            bson_append_finish_array(&bq2);
            bson_append_finish_object(&bq2);
        }

        // for selecting samples
//        cout << qsample[0].c_str() <<  endl;
        if (!qsample[0].empty())
        {
            bson_append_start_object(&bq2, keys::expsample);
            bson_append_start_array(&bq2, "$in");
            for (unsigned int j=0; j<usedataset.size(); ++j)
            {
                ss << j;
                sss = ss.str();
                ss.str("");
                bson_append_string(&bq2, sss.c_str(), qsample[j].c_str());
            }
            bson_append_finish_array(&bq2);
            bson_append_finish_object(&bq2);
        }

        // for selection temperatures
        if ((qsT.size() == 2))
        {
            // for selecting T interval
            bson_append_start_object(&bq2, keys::sT);
            bson_append_start_array(&bq2, "$bt");
            bson_append_int(&bq2, "0", qsT[0]);
            bson_append_int(&bq2, "1", qsT[1]);
            bson_append_finish_array(&bq2);
            bson_append_finish_object(&bq2);
        } else
            if (!(qsT[0] == NULL))
            {
                bson_append_start_object(&bq2, keys::sT);
                bson_append_start_array(&bq2, "$in");
                for (unsigned int j=0; j<qsT.size(); ++j)
                {
                    ss << j;
                    sss = ss.str();
                    ss.str("");
                    bson_append_int(&bq2, sss.c_str(), qsT[j]);
                }
                bson_append_finish_array(&bq2);
                bson_append_finish_object(&bq2);
            }

        // for selection presures
        if ((qsP.size() == 2))
        {
            // for selecting P interval
            bson_append_start_object(&bq2, keys::sP);
            bson_append_start_array(&bq2, "$bt");
            bson_append_int(&bq2, "0", qsP[0]);
            bson_append_int(&bq2, "1", qsP[1]);
            bson_append_finish_array(&bq2);
            bson_append_finish_object(&bq2);
        } else
            if (!(qsP[0] == NULL))
            {
                bson_append_start_object(&bq2, keys::sP);
                bson_append_start_array(&bq2, "$in");
                for (unsigned int j=0; j<qsP.size(); ++j)
                {
                    ss << j;
                    sss = ss.str();
                    ss.str("");
                    bson_append_int(&bq2, sss.c_str(), qsP[j]);
                }
                bson_append_finish_array(&bq2);
                bson_append_finish_object(&bq2);
            }

        // finish creading bson query object
        bson_finish(&bq2);
        EJQ *q2 = ejdbcreatequery(jb, &bq2, NULL, 0, NULL);

        uint32_t count;
        TCLIST *res = ejdbqryexecute(coll, q2, &count, 0, NULL);
        fprintf(stderr, "Records found: %d\n", count);

        //Print the result set records
//         for (int i = 0; i < TCLISTNUM(res); ++i) {
//             void *bsdata = TCLISTVALPTR(res, i);
//             char *bsdata_ = static_cast<char*>(bsdata);
//             bson_print_raw(stderr, bsdata_, 0);
//         }
//         fprintf(stderr, "\n");

         // adding data into Data_manager storage class
         for (unsigned int j=0; j<count; j++)
         {
             experiments.push_back( new samples );
             // set experiments variables empty
             experiments.at(j)->sP = NULL; experiments.at(j)->sT = NULL; experiments.at(j)->sV= NULL;
             experiments.at(j)->idsample= NULL;
             // set Hexperiments variables false
         }

         fout << "6. data_manager.cpp line 410. Adding the data returned by the selection query into the data structure; " << endl;

//#ifdef USE_MPI
         omp_set_num_threads(MPI);
         #pragma omp parallel for
//#endif
         for (unsigned int i = 0; i < TCLISTNUM(res); ++i) {
             void *bsdata = TCLISTVALPTR(res, i);
             char *bsdata_ = static_cast<char*>(bsdata);
             bson_to_Data_Manager(stderr, bsdata_, i); // adding the data returned by the selection query into the data storage class
         }

        //Dispose result set
        tclistdel(res);

        //Dispose query
        ejdbquerydel(q2);
        bson_destroy(&bq2);

        //Close database
        ejdbclose(jb);
        ejdbdel(jb);
    }
    else
    {
        // selec all
    }
}

void Data_Manager::bson_to_Data_Manager(FILE *f, const char *data, int pos) {
    bson_iterator i, j, k, k2, d, d2, d3; // 1st, 2nd, 3rd, 2-1, 3-1 level
    const char *key;
    string key_;
    int ip = -1, ic = -1, sk = -1, ipc, ipp, ips, ipdcp, ipm;
    bson_iterator_from_buffer(&i, data);

    while (bson_iterator_next(&i))
    {
        bson_type t = bson_iterator_type(&i);
        if (t == 0)
            break;
        key = bson_iterator_key(&i);
        key_ = key;


        if (key_ == keys::expdataset)
        {
            // adding expdataset
            experiments.at(pos)->expdataset = bson_iterator_string(&i);
        } else
        if (key_ == keys::expsample)
        {
            // adding sample name
            experiments.at(pos)->sample = bson_iterator_string(&i);
        } else
        if (key_ == keys::sT)
        {
            // adding temperature
            experiments.at(pos)->sT = bson_iterator_int(&i);
        } else
        if (key_ == keys::sP)
        {
            // adding pressure
            experiments.at(pos)->sP = bson_iterator_int(&i);
        } else
        if (key_ == keys::sV)
        {
            // adding volume
            experiments.at(pos)->sV = bson_iterator_int(&i);
        } else

        // adding experiment components/recipe
        if ((key_ == keys::sbcomp) && (t == BSON_ARRAY))
        {
            bson_iterator_from_buffer(&j, bson_iterator_value(&i));
            while (bson_iterator_next(&j))
            {
                bson_iterator_from_buffer(&d, bson_iterator_value(&j));
                experiments.at(pos)->sbcomp.push_back( new samples::components );
                ic++; // position of the component in sbcomp vector
                experiments.at(pos)->sbcomp.at(ic)->Qnt = NULL; experiments.at(pos)->sbcomp.at(ic)->Qerror = NULL;
                experiments.at(pos)->sbcomp.at(ic)->Qunit = keys::gram; // default

                while (bson_iterator_next(&d))
                {
                    t = bson_iterator_type(&d);
                    if (t == 0)
                        break;
                    key = bson_iterator_key(&d);
                    key_ = key;

                    if ((key_ == keys::comp))
                    {
                        experiments.at(pos)->sbcomp.at(ic)->comp =  bson_iterator_string(&d) ;
                    } else
                    if ((key_ == keys::Qnt))
                    {
                        experiments.at(pos)->sbcomp.at(ic)->Qnt = bson_iterator_double(&d) ;
                    } else
                    if ((key_ == keys::Qerror))
                    {
                        experiments.at(pos)->sbcomp.at(ic)->Qerror = bson_iterator_double(&d) ;
                    } else
                    if ((key_ == keys::Qunit))
                    {
                        experiments.at(pos)->sbcomp.at(ic)->Qunit = bson_iterator_string(&d) ;
                    }
                }
            }
        } else

        // adding Upper metastability constraints
        if ((key_ == keys::UMC) && (t == BSON_ARRAY))
        {
            bson_iterator_from_buffer(&j, bson_iterator_value(&i));
            while (bson_iterator_next(&j))
            {
                bson_iterator_from_buffer(&d, bson_iterator_value(&j));
                experiments.at(pos)->U_KC.push_back( new samples::Uconstraints );
                sk++; // position of the component in U_SK vector
//                experiments.at(pos)->U_KC.at(sk)->dcomp = NULL;
                experiments.at(pos)->U_KC.at(sk)->Qnt = 1000000;

                while (bson_iterator_next(&d))
                {
                    t = bson_iterator_type(&d);
                    if (t == 0)
                        break;
                    key = bson_iterator_key(&d);
                    key_ = key;

                    if ((key_ == keys::DC))
                    {
                        experiments.at(pos)->U_KC.at(sk)->type = keys::DC;
                        experiments.at(pos)->U_KC.at(sk)->name =  bson_iterator_string(&d) ;

                    } else
                    if ((key_ == keys::phase))
                    {
                        experiments.at(pos)->U_KC.at(sk)->type = keys::phase;
                        experiments.at(pos)->U_KC.at(sk)->name = bson_iterator_double(&d) ;
                    }
                    if ((key_ == keys::Qnt))
                    {
                        experiments.at(pos)->U_KC.at(sk)->Qnt = bson_iterator_double(&d) ;
                    }
                    /*else
                    if ((key_ == keys::Qerror))
                    {
                        experiments.at(pos)->sbcomp.at(ic)->Qerror = bson_iterator_double(&d) ;
                    } else
                    if ((key_ == keys::Qunit))
                    {
                        experiments.at(pos)->sbcomp.at(ic)->Qunit = bson_iterator_string(&d) ;
                    }*/
                }
            }
        } else

        // adding Lower metastability constraints
        if ((key_ == keys::LMC) && (t == BSON_ARRAY))
        {
            bson_iterator_from_buffer(&j, bson_iterator_value(&i));
            while (bson_iterator_next(&j))
            {
                bson_iterator_from_buffer(&d, bson_iterator_value(&j));
                experiments.at(pos)->L_KC.push_back( new samples::Lconstraints );
                sk++; // position of the component in U_SK vector
    //            experiments.at(pos)->U_KC.at(sk)->dcomp = NULL;
                experiments.at(pos)->L_KC.at(sk)->Qnt = 0;

                while (bson_iterator_next(&d))
                {
                    t = bson_iterator_type(&d);
                    if (t == 0)
                        break;
                    key = bson_iterator_key(&d);
                    key_ = key;

                    if ((key_ == keys::DC))
                    {
                        experiments.at(pos)->L_KC.at(sk)->name =  bson_iterator_string(&d) ;

                    } else
                    if ((key_ == keys::Qnt))
                    {
                        experiments.at(pos)->L_KC.at(sk)->Qnt = bson_iterator_double(&d) ;
                    } /*else
                        if ((key_ == keys::Qerror))
                        {
                            experiments.at(pos)->sbcomp.at(ic)->Qerror = bson_iterator_double(&d) ;
                        } else
                        if ((key_ == keys::Qunit))
                        {
                            experiments.at(pos)->sbcomp.at(ic)->Qunit = bson_iterator_string(&d) ;
                        }*/
                }
            }
        } else

        // adding experimental phases
        if ((key_ == keys::expphases) && (t == BSON_ARRAY))
        {
            bson_iterator_from_buffer(&j, bson_iterator_value(&i));
            while (bson_iterator_next(&j))
            {
                bson_iterator_from_buffer(&d, bson_iterator_value(&j));
                experiments.at(pos)->expphases.push_back( new samples::phases );
                ip++; // position of the phase in expphases vector
                ipc = -1; // phases components - reset to -1 for every new pahse
                ipm = -1; // molar fractions
                ipp = -1; // phases properties
                ips = -1; // phases dcomps
                experiments.at(pos)->expphases.at(ip)->idphase = NULL;

                while (bson_iterator_next(&d))
                {
                    t = bson_iterator_type(&d);
                    if (t == 0)
                        break;
                    key = bson_iterator_key(&d);
                    key_ = key;

                    if ((key_ == keys::phase))
                    {
                        experiments.at(pos)->expphases.at(ip)->phase =  bson_iterator_string(&d) ;
                    } else

                    // adding phase components
                    if ((key_ == keys::phIC) && (t == BSON_ARRAY))
                    {
                        bson_iterator_from_buffer(&k, bson_iterator_value(&d));
                        while (bson_iterator_next(&k))
                        {
                            bson_iterator_from_buffer(&d2, bson_iterator_value(&k));
                            experiments.at(pos)->expphases.at(ip)->phIC.push_back( new samples::components );
                            ipc++; // position of the component in phcomp vector
                            experiments.at(pos)->expphases.at(ip)->phIC.at(ipc)->Qerror = NULL;
                            experiments.at(pos)->expphases.at(ip)->phIC.at(ipc)->Qnt   = NULL;
                            string p_name = experiments.at(pos)->expphases.at(ip)->phase;
                            if (p_name == "aq_gen")
                            {
                                experiments.at(pos)->expphases.at(ip)->phIC.at(ipc)->Qunit = keys::molal;
                            } else experiments.at(pos)->expphases.at(ip)->phIC.at(ipc)->Qunit = keys::molfrac;

                            while (bson_iterator_next(&d2))
                            {
                                t = bson_iterator_type(&d2);
                                if (t == 0)
                                    break;
                                key = bson_iterator_key(&d2);
                                key_ = key;

                                if ((key_ == keys::IC))
                                {
                                    experiments.at(pos)->expphases.at(ip)->phIC.at(ipc)->comp = bson_iterator_string(&d2) ;
                                } else
                                if ((key_ == keys::Qnt))
                                {
                                    experiments.at(pos)->expphases.at(ip)->phIC.at(ipc)->Qnt = bson_iterator_double(&d2) ;
                                } else
                                if ((key_ == keys::Qerror))
                                {
                                    experiments.at(pos)->expphases.at(ip)->phIC.at(ipc)->Qerror = bson_iterator_double(&d2) ;
                                } else
                                if ((key_ == keys::Qunit))
                                {
                                    experiments.at(pos)->expphases.at(ip)->phIC.at(ipc)->Qunit = bson_iterator_string(&d2) ;
                                }
                            }
                        }
                    } else

                    // adding phase IC as molar fractions MF
                    if ((key_ == keys::phMF) && (t == BSON_ARRAY))
                    {
                        bson_iterator_from_buffer(&k, bson_iterator_value(&d));
                        while (bson_iterator_next(&k))
                        {
                            bson_iterator_from_buffer(&d2, bson_iterator_value(&k));
                            experiments.at(pos)->expphases.at(ip)->phMF.push_back( new samples::components );
                            ipm++; // position of the component in phcomp vector
                            experiments.at(pos)->expphases.at(ip)->phMF.at(ipm)->Qerror = NULL;
                            experiments.at(pos)->expphases.at(ip)->phMF.at(ipm)->Qnt   = NULL;
                            string p_name = experiments.at(pos)->expphases.at(ip)->phase;
                            if (p_name == "aq_gen")
                            {
                                experiments.at(pos)->expphases.at(ip)->phMF.at(ipm)->Qunit = keys::molfrac;
                            } else experiments.at(pos)->expphases.at(ip)->phMF.at(ipm)->Qunit = keys::molfrac;

                                while (bson_iterator_next(&d2))
                                {
                                    t = bson_iterator_type(&d2);
                                    if (t == 0)
                                        break;
                                    key = bson_iterator_key(&d2);
                                    key_ = key;

                                    if ((key_ == keys::MF))
                                    {
                                        experiments.at(pos)->expphases.at(ip)->phMF.at(ipm)->comp = bson_iterator_string(&d2) ;
                                    } else
                                    if ((key_ == keys::Qnt))
                                    {
                                        experiments.at(pos)->expphases.at(ip)->phMF.at(ipm)->Qnt = bson_iterator_double(&d2) ;
                                    } else
                                    if ((key_ == keys::Qerror))
                                    {
                                        experiments.at(pos)->expphases.at(ip)->phMF.at(ipm)->Qerror = bson_iterator_double(&d2) ;
                                    } else
                                    if ((key_ == keys::Qunit))
                                    {
                                        experiments.at(pos)->expphases.at(ip)->phMF.at(ipm)->Qunit = bson_iterator_string(&d2) ;
                                    }
                                }
                            }
                        } else

                    // adding phase properties
                    if ((key_ == keys::phprop) && (t == BSON_ARRAY))
                    {
                        bson_iterator_from_buffer(&k, bson_iterator_value(&d));
                        while (bson_iterator_next(&k))
                        {
                            bson_iterator_from_buffer(&d2, bson_iterator_value(&k));
                            experiments.at(pos)->expphases.at(ip)->phprop.push_back( new samples::phases::prop );
                            ipp++; // position of the component in phcomp vector
                            experiments.at(pos)->expphases.at(ip)->phprop.at(ipp)->Qerror = NULL;
                            experiments.at(pos)->expphases.at(ip)->phprop.at(ipp)->Qnt   = NULL;

                            while (bson_iterator_next(&d2))
                            {
                                t = bson_iterator_type(&d2);
                                if (t == 0)
                                    break;
                                key = bson_iterator_key(&d2);
                                key_ = key;

                                if ((key_ == keys::property))
                                {
                                    string p_name = bson_iterator_string(&d2);
                                    experiments.at(pos)->expphases.at(ip)->phprop.at(ipp)->property = bson_iterator_string(&d2) ;
                                    // assigining default values for units
                                    if (p_name==keys::Qnt)
                                    {
                                        experiments.at(pos)->expphases.at(ip)->phprop.at(ipp)->Qunit = keys::gram;
                                    } else
                                    if (p_name==keys::RHO)
                                    {
                                        experiments.at(pos)->expphases.at(ip)->phprop.at(ipp)->Qunit = keys::g_cm3;
                                    } else
                                    if (p_name==keys::pV)
                                    {
                                        experiments.at(pos)->expphases.at(ip)->phprop.at(ipp)->Qunit = keys::cm3;
                                    } else
                                    if (p_name==keys::pH)
                                    {
                                        experiments.at(pos)->expphases.at(ip)->phprop.at(ipp)->Qunit = keys::_loga;
                                    }
                                } else
                                if ((key_ == keys::Qnt))
                                {
                                    experiments.at(pos)->expphases.at(ip)->phprop.at(ipp)->Qnt = bson_iterator_double(&d2) ;
                                } else
                                if ((key_ == keys::Qerror))
                                {
                                    experiments.at(pos)->expphases.at(ip)->phprop.at(ipp)->Qerror = bson_iterator_double(&d2) ;
                                } else
                                if ((key_ == keys::Qunit))
                                {
                                    experiments.at(pos)->expphases.at(ip)->phprop.at(ipp)->Qunit = bson_iterator_string(&d2) ;
                                }
                            }
                        }
                    }

                    // adding phase dcomps
                    if ((key_ == keys::phDC) && (t == BSON_ARRAY))
                    {
                        bson_iterator_from_buffer(&k, bson_iterator_value(&d));
                        while (bson_iterator_next(&k))
                        {
                            bson_iterator_from_buffer(&d2, bson_iterator_value(&k));
                            experiments.at(pos)->expphases.at(ip)->phDC.push_back( new samples::phases::dcomps );
                            ips++; // position of the specie in phdcomps vector
                            ipdcp = -1;

                            while (bson_iterator_next(&d2))
                            {
                                t = bson_iterator_type(&d2);
                                if (t == 0)
                                    break;
                                key = bson_iterator_key(&d2);
                                key_ = key;

                                if ((key_ == keys::DC)) // adding the name of the specie
                                {
                                    experiments.at(pos)->expphases.at(ip)->phDC.at(ips)->DC =  bson_iterator_string(&d2) ;
                                } else

                                // adding dependent compoponents properties
                                if ((key_ == keys::DCprop) && (t == BSON_ARRAY))
                                {
                                    bson_iterator_from_buffer(&k2, bson_iterator_value(&d2));
                                    while (bson_iterator_next(&k2))
                                    {
                                        bson_iterator_from_buffer(&d3, bson_iterator_value(&k2));
                                        experiments.at(pos)->expphases.at(ip)->phDC.at(ips)->DCprop.push_back( new samples::phases::dcomps::dcprop );

                                        ipdcp++; // position of the component in phcomp vector
                                        experiments.at(pos)->expphases.at(ip)->phDC.at(ips)->DCprop.at(ipdcp)->Qerror = NULL;
                                        experiments.at(pos)->expphases.at(ip)->phDC.at(ips)->DCprop.at(ipdcp)->Qnt   = NULL;

                                        while (bson_iterator_next(&d3))
                                        {
                                            t = bson_iterator_type(&d3);
                                            if (t == 0)
                                                break;
                                            key = bson_iterator_key(&d3);
                                            key_ = key;

                                            if ((key_ == keys::property))
                                            {
                                                experiments.at(pos)->expphases.at(ip)->phDC.at(ips)->DCprop.at(ipdcp)->property = bson_iterator_string(&d3);
                                            } else
                                            if ((key_ == keys::Qnt))
                                            {
                                                experiments.at(pos)->expphases.at(ip)->phDC.at(ips)->DCprop.at(ipdcp)->Qnt = bson_iterator_double(&d3) ;
                                            } else
                                            if ((key_ == keys::Qerror))
                                            {
                                                experiments.at(pos)->expphases.at(ip)->phDC.at(ips)->DCprop.at(ipdcp)->Qerror = bson_iterator_double(&d3) ;
                                            } else
                                            if ((key_ == keys::Qunit))
                                            {
                                                experiments.at(pos)->expphases.at(ip)->phDC.at(ips)->DCprop.at(ipdcp)->Qunit = bson_iterator_string(&d3) ;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void Data_Manager::get_distinct_TP( )
{
    vector<int> TP[2];
    unsigned int i, j;
    bool isfound = false, isfound2 = false;

    for (i=0; i<experiments.size(); ++i)
    {
        TP[0].push_back(experiments[i]->sT);
        TP[1].push_back(experiments[i]->sP);
    }
    for (i=0; i<TP[0].size(); i++)
    {
        // check if TP pair is presnt more than once in the TP vector
        for (j=0; j<TP[0].size(); j++)
        {
            if ((TP[0][i] == TP[0][j]) && (TP[1][i] == TP[1][j]) && (i != j))
            {
                isfound = true;
            }
        }
        // check if TP pair was added to the unique TP pairs container
        for (j=0; j<TP_pairs[0].size(); ++j)
        {
            if ((TP[0][i] == TP_pairs[0][j]) && (TP[1][i] == TP_pairs[1][j]))
            {
                isfound2 = true;
            }
        }
        // add TP pair if it does not repeat itself or was not added already in the container
        if ((!isfound) || (!isfound2))
        {
            TP_pairs[0].push_back(TP[0][i]);
            TP_pairs[1].push_back(TP[1][i]);
        }
        isfound = false;
        isfound2 = false;
    }
}

string Data_Manager::readin_JSON(string key)
{
    // Variable declarations
    vector<string> vdata;
    string line, allparam;
    string json_s, result;
    string fname = gpf->OptParamFile();
    int pos_start, pos_end;
    unsigned int i;
    ifstream param_stream;

    // Keywords
    string f7(key);
    string f4("#");

    // Read parameter file into string
    param_stream.open(fname.c_str());
    if( param_stream.fail() )
    {
        cout << "Opening of file "<<fname<<" failed !!"<<endl;
        exit(1);
    }
    while( getline(param_stream, line) )
    {
        vdata.push_back(line);
    }
    param_stream.close();
    for( i=0; i < vdata.size(); i++ )
    allparam += vdata[i];

    // GEMSFIT logfile
    ofstream fout;
    fout.open(gpf->FITLogFile().c_str(), ios::app);
    if( fout.fail() )
    { cout<<"Output fileopen error"<<endl; exit(1); }

    pos_start = allparam.find(f7);
    pos_end   = allparam.find(f4,pos_start);
    json_s = allparam.substr((pos_start+f7.length()),(pos_end-pos_start-f7.length()));

    remove_copy(json_s.begin(), json_s.end(), std::back_inserter(result), '\'');
    json_s = result;

    return json_s;
}

void Data_Manager::parse_JSON_object(string query, const char* key, vector<string> &result)
{
    json_t *root; json_t *data; json_t *object;
    json_error_t jerror;
    stringstream ss;
    string sss;

    const char * JSON = query.c_str();
    root = json_loads(JSON, 0, &jerror);

    if(!root)
    {
        fprintf(stderr, "error: on line %d: %s\n", jerror.line, jerror.text);
    }
    else
    {
        object = json_object_get(root, key);
        if(json_is_array(object))
        {
            for(unsigned int i = 0; i < json_array_size(object); i++)
            {
                data = json_array_get(object, i);
                    if(json_is_string(data))
                    {
                        result.push_back(json_string_value(data));
                    }
                    else if (json_is_real(data))
                    {
                        ss << json_real_value(data);
                        sss = ss.str();
                        ss.str("");
                        result.push_back(sss);
                    }
                    else if (json_is_integer(data))
                    {
                        ss << json_integer_value(data);
                        sss = ss.str();
                        ss.str("");
                        result.push_back(sss);
                    }
                    else if (json_is_object(data))
                    {
                        result.push_back("is object");
                    }
             }
         }
        else
        {
                if(json_is_string(object))
                {
                    result.push_back(json_string_value(object));
                }
                else if (json_is_real(object))
                {
                    ss << json_real_value(object);
                    sss = ss.str();
                    ss.str("");
                    result.push_back(sss);
                }
                else if (json_is_integer(object))
                {
                    ss << json_integer_value(object);
                    sss = ss.str();
                    ss.str("");
                    result.push_back(sss);
                }
        }
    }
}

void Data_Manager::parse_JSON_array_object( string data_, const char *arr , const char *key, vector<string> &result )
{
    json_t *root; json_t *data1; json_t *data; json_t *object;
    json_error_t jerror;
    stringstream ss;
    string sss;

    const char * JSON = data_.c_str();
    root = json_loads(JSON, 0, &jerror);

    if(!root)
    {
        fprintf(stderr, "error: on line %d: %s\n", jerror.line, jerror.text);
    }
    else
    {
        object = json_object_get(root, arr);
        if(json_is_array(object))
        {
            for(unsigned int i = 0; i < json_array_size(object); i++)
            {
                data1 = json_array_get(object, i);
                if (json_is_object(data1))
                {
                    data = json_object_get(data1, key);
                    if(json_is_string(data))
                    {
                        result.push_back(json_string_value(data));
                    }
                    else if (json_is_real(data))
                    {
                        ss << json_real_value(data);
                        sss = ss.str();
                        ss.str("");
                        result.push_back(sss);
                    }
                    else if (json_is_integer(data))
                    {
                        ss << json_integer_value(data);
                        sss = ss.str();
                        ss.str("");
                        result.push_back(sss);
                    }
                }
             }
         }
        else
        {
            data = json_object_get(data1, key);
                if(json_is_string(object))
                {
                    result.push_back(json_string_value(object));
                }
                else if (json_is_real(object))
                {
                    ss << json_real_value(object);
                    sss = ss.str();
                    ss.str("");
                    result.push_back(sss);
                }
                else if (json_is_integer(object))
                {
                    ss << json_integer_value(object);
                    sss = ss.str();
                    ss.str("");
                    result.push_back(sss);
                }
        }
    }
}


