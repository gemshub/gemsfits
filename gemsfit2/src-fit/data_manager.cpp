/// \file data_manager.cpp
/// Implementation of reading database parameters form input file
/// and experimental data form databsse
//
// Copyright (C) 2013 G.D.Miron, D.Kulik
// <GEMS Development Team, mailto:gems2.support@psi.ch>
//
// This file is part of the GEMSFIT code for parameterization of thermodynamic
// data and models <http://gems.web.psi.ch/GEMSFIT/>
//
// GEMSIFT is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.

// GEMSFIT is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with GEMSFIT code. If not, see <http://www.gnu.org/licenses/>.
//-------------------------------------------------------------------
//

/**
 *	@file data_manager.cpp
 *
 *	@brief this source file contains the implementation of the data manager class,
 *	which retrieves and stores data from thre GEMSFIT input file as well as
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
#include <boost/lexical_cast.hpp>
#include "ejdb.h"

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
    fout << "3. data_manager.cpp line 48. Reading database parameter get_db_specs(); " << endl;
    get_db_specs_txt();

    // Read measurement data from PosgreSQL server
    fout << "4. data_manager.cpp line 52. allexp.push_back(new experiment) - empty; " << endl;

    // Readin in the data slection query
    DataSelect = readin_JSON("<DatSelect>");
    // Getting the query result data into the Data_Manager class
    get_EJDB();

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

string Data_Manager::readin_JSON(string key)
{
    // Variable declarations
    vector<string> vdata;
    string line, allparam;
    string json_s;
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

    return json_s;
}

void Data_Manager::parse_JSON_object(string query, const char* key, vector<string> &result)
{
    json_t *root; json_t *data; json_t *object;
    json_error_t jerror;

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
                        result.push_back(boost::lexical_cast<string>(json_real_value(data)));
                    }
                    else if (json_is_integer(data))
                    {
                        result.push_back(boost::lexical_cast<string>(json_integer_value(data)));
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
                    result.push_back(boost::lexical_cast<string>(json_real_value(object)));
                }
                else if (json_is_integer(object))
                {
                    result.push_back(boost::lexical_cast<string>(json_integer_value(object)));
                }
        }
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

    string_v out, qsample, qexpdataset;
    int_v qsT, qsP;

    // processing DataSelect
    if (DataSelect != "all")
    {
        parse_JSON_object(DataSelect, expsample, out);
        qsample = out; // query for selecting samples
        out.clear();

        parse_JSON_object(DataSelect, expdataset, out);
        qexpdataset = out; // query for selecting expdatasets
        out.clear();

        parse_JSON_object(DataSelect, sT, out);
        for (unsigned int i = 0 ; i < out.size() ; i++)
        {
            qsT.push_back( atoi(out.at(i).c_str()) ); // query for selecting T
        }
        out.clear();

        parse_JSON_object(DataSelect, sP, out);
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
        EJCOLL *coll = ejdbcreatecoll(jb, colection.c_str(), NULL);

        // Creat bson query object
        bson bq2;
        bson_init_as_query(&bq2);

        // for selecting expdatasets
        if (!qexpdataset[0].empty())
        {
            bson_append_start_object(&bq2, expdataset);
            bson_append_start_array(&bq2, "$in");
            for (unsigned int j=0; j<qexpdataset.size(); ++j)
            {
                bson_append_string(&bq2, boost::lexical_cast<string>(j).c_str(), qexpdataset[j].c_str());
            }
            bson_append_finish_array(&bq2);
            bson_append_finish_object(&bq2);
        }

        // for selecting samples
        cout << qsample[0].c_str() <<  endl;
        if (!qsample[0].empty())
        {
            bson_append_start_object(&bq2, expsample);
            bson_append_start_array(&bq2, "$in");
            for (unsigned int j=0; j<qexpdataset.size(); ++j)
            {
                bson_append_string(&bq2, boost::lexical_cast<string>(j).c_str(), qsample[j].c_str());
            }
            bson_append_finish_array(&bq2);
            bson_append_finish_object(&bq2);
        }

        // for selection temperatures
        if ((qsT.size() == 2))
        {
            // for selecting T interval
            bson_append_start_object(&bq2, sT);
            bson_append_start_array(&bq2, "$bt");
            bson_append_int(&bq2, "0", qsT[0]);
            bson_append_int(&bq2, "1", qsT[1]);
            bson_append_finish_array(&bq2);
            bson_append_finish_object(&bq2);
        } else
            if (!(qsT[0] == NULL))
            {
                bson_append_start_object(&bq2, sT);
                bson_append_start_array(&bq2, "$in");
                for (int j=0; j<qsT.size(); ++j)
                {
                    bson_append_int(&bq2, boost::lexical_cast<string>(j).c_str(), qsT[j]);
                }
                bson_append_finish_array(&bq2);
                bson_append_finish_object(&bq2);
            }

        // for selection presures
        if ((qsP.size() == 2))
        {
            // for selecting P interval
            bson_append_start_object(&bq2, sP);
            bson_append_start_array(&bq2, "$bt");
            bson_append_int(&bq2, "0", qsP[0]);
            bson_append_int(&bq2, "1", qsP[1]);
            bson_append_finish_array(&bq2);
            bson_append_finish_object(&bq2);
        } else
            if (!(qsP[0] == NULL))
            {
                bson_append_start_object(&bq2, sP);
                bson_append_start_array(&bq2, "$in");
                for (int j=0; j<qsP.size(); ++j)
                {
                    bson_append_int(&bq2, boost::lexical_cast<string>(j).c_str(), qsP[j]);
                }
                bson_append_finish_array(&bq2);
                bson_append_finish_object(&bq2);
            }

        // finish creading bson query object
        bson_finish(&bq2);
        EJQ *q2 = ejdbcreatequery(jb, &bq2, NULL, 0, NULL);

        uint32_t count;
        TCLIST *res = ejdbqryexecute(coll, q2, &count, 0, NULL);
        fprintf(stderr, "\n\nRecords found: %d\n", count);

        //Print the result set records
         for (int i = 0; i < TCLISTNUM(res); ++i) {
             void *bsdata = TCLISTVALPTR(res, i);
             char *bsdata_ = static_cast<char*>(bsdata);
             bson_print_raw(stderr, bsdata_, 0);
         }
         fprintf(stderr, "\n");

         // adding data into Data_manager storage class
         for (int j=0; j<count; j++)
         {
             experiments.push_back( new samples );
             // set experiments variables empty
             experiments.at(j)->sP = NULL; experiments.at(j)->sT = NULL; experiments.at(j)->sV= NULL;
             experiments.at(j)->idsample= NULL;
             Hexperiments.push_back( new Hsamples ); // handels for marking data to compare
             // set Hexperiments variables false

         }
         for (int i = 0; i < TCLISTNUM(res); ++i) {
             void *bsdata = TCLISTVALPTR(res, i);
             char *bsdata_ = static_cast<char*>(bsdata);
             bson_to_Data_Manager(stderr, bsdata_, i); // addin the data
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
    bson_iterator i, j, k, d, d2; // 1st, 2nd, 3rd, 2-1, 3-1 level
    const char *key;
    string key_;
    int ip = -1, ic = -1, ipc, ipp, ips;
    bson_iterator_from_buffer(&i, data);

    while (bson_iterator_next(&i))
    {
        bson_type t = bson_iterator_type(&i);
        if (t == 0)
            break;
        key = bson_iterator_key(&i);
        key_ = key;


        if (key_ == expdataset)
        {
            // adding expdataset
            experiments.at(pos)->expdataset = bson_iterator_string(&i);
            Hexperiments.at(pos)->expdataset = bson_iterator_string(&i);

        } else
        if (key_ == expsample)
        {
            // adding sample name
            experiments.at(pos)->sample = bson_iterator_string(&i);
            Hexperiments.at(pos)->sample = bson_iterator_string(&i);
        } else
        if (key_ == sT)
        {
            // adding temperature
            experiments.at(pos)->sT = bson_iterator_int(&i);
        } else
        if (key_ == sP)
        {
            // adding pressure
            experiments.at(pos)->sP = bson_iterator_int(&i);
        } else

        // adding experiment components/recipe
        if ((key_ == sbcomp) && (t == BSON_ARRAY))
        {
            bson_iterator_from_buffer(&j, bson_iterator_value(&i));
            while (bson_iterator_next(&j))
            {
                bson_iterator_from_buffer(&d, bson_iterator_value(&j));
                experiments.at(pos)->sbcomp.push_back( new samples::components );
                Hexperiments.at(pos)->Hsbcomp.push_back( new Hsamples::Hcomponents );
                ic++; // position of the component in sbcomp vector
                experiments.at(pos)->sbcomp.at(ic)->bQnt = NULL; experiments.at(pos)->sbcomp.at(ic)->Qerror = NULL;
                Hexperiments.at(pos)->Hsbcomp.at(ic)->comp = false;

                while (bson_iterator_next(&d))
                {
                    t = bson_iterator_type(&d);
                    if (t == 0)
                        break;
                    key = bson_iterator_key(&d);
                    key_ = key;

                    if ((key_ == comp))
                    {
                        experiments.at(pos)->sbcomp.at(ic)->comp =  bson_iterator_string(&d) ;
                        Hexperiments.at(pos)->Hsbcomp.at(ic)->comp = true;
                    } else
                    if ((key_ == bQnt))
                    {
                        experiments.at(pos)->sbcomp.at(ic)->bQnt = bson_iterator_double(&d) ;
                    } else
                    if ((key_ == Qerror))
                    {
                        experiments.at(pos)->sbcomp.at(ic)->Qerror = bson_iterator_double(&d) ;
                    } else
                    if ((key_ == Qunit))
                    {
                        experiments.at(pos)->sbcomp.at(ic)->Qunit = bson_iterator_string(&d) ;
                    }
                }
            }
        } else

        // adding experimental phases
        if ((key_ == expphases) && (t == BSON_ARRAY))
        {
            bson_iterator_from_buffer(&j, bson_iterator_value(&i));
            while (bson_iterator_next(&j))
            {
                bson_iterator_from_buffer(&d, bson_iterator_value(&j));
                experiments.at(pos)->expphases.push_back( new samples::phases );
                Hexperiments.at(pos)->Hexpphases.push_back( new Hsamples::Hphases );
                ip++; // position of the phase in expphases vector
                ipc = -1; // phases components - reset to -1 for every new pahse
                ipp = -1; // phases properties
                ips = -1; // phases species
                experiments.at(pos)->expphases.at(ip)->idphase = NULL;
                Hexperiments.at(pos)->Hexpphases.at(ip)->Hphase = false;

                while (bson_iterator_next(&d))
                {
                    t = bson_iterator_type(&d);
                    if (t == 0)
                        break;
                    key = bson_iterator_key(&d);
                    key_ = key;

                    if ((key_ == phase))
                    {
                        experiments.at(pos)->expphases.at(ip)->phase =  bson_iterator_string(&d) ;
                        Hexperiments.at(pos)->Hexpphases.at(ip)->phase = bson_iterator_string(&d) ;
                        Hexperiments.at(pos)->Hexpphases.at(ip)->Hphase = true;
                    } else

                    // adding phase components
                    if ((key_ == phcomp) && (t == BSON_ARRAY))
                    {
                        bson_iterator_from_buffer(&k, bson_iterator_value(&d));
                        while (bson_iterator_next(&k))
                        {
                            bson_iterator_from_buffer(&d2, bson_iterator_value(&k));
                            experiments.at(pos)->expphases.at(ip)->phcomp.push_back( new samples::components );
                            Hexperiments.at(pos)->Hexpphases.at(ip)->Hphcomp.push_back( new Hsamples::Hcomponents );
                            ipc++; // position of the component in phcomp vector
                            experiments.at(pos)->expphases.at(ip)->phcomp.at(ipc)->Qerror = NULL;
                            experiments.at(pos)->expphases.at(ip)->phcomp.at(ipc)->bQnt   = NULL;
                            Hexperiments.at(pos)->Hexpphases.at(ip)->Hphcomp.at(ipc)->comp= false;

                            while (bson_iterator_next(&d2))
                            {
                                t = bson_iterator_type(&d2);
                                if (t == 0)
                                    break;
                                key = bson_iterator_key(&d2);
                                key_ = key;

                                if ((key_ == element))
                                {
                                    experiments.at(pos)->expphases.at(ip)->phcomp.at(ipc)->comp = bson_iterator_string(&d2) ;
                                    Hexperiments.at(pos)->Hexpphases.at(ip)->Hphcomp.at(ipc)->comp= true;
                                } else
                                if ((key_ == eQnt))
                                {
                                    experiments.at(pos)->expphases.at(ip)->phcomp.at(ipc)->bQnt = bson_iterator_double(&d2) ;
                                } else
                                if ((key_ == Qerror))
                                {
                                    experiments.at(pos)->expphases.at(ip)->phcomp.at(ipc)->Qerror = bson_iterator_double(&d2) ;
                                } else
                                if ((key_ == Qunit))
                                {
                                    experiments.at(pos)->expphases.at(ip)->phcomp.at(ipc)->Qunit = bson_iterator_string(&d2) ;
                                }
                            }
                        }
                    } else

                    // adding phase properties
                    if ((key_ == phprop) && (t == BSON_ARRAY))
                    {
                        bson_iterator_from_buffer(&k, bson_iterator_value(&d));
                        while (bson_iterator_next(&k))
                        {
                            bson_iterator_from_buffer(&d2, bson_iterator_value(&k));
                            experiments.at(pos)->expphases.at(ip)->phprop.push_back( new samples::phases::prop );
                            Hexperiments.at(pos)->Hexpphases.at(ip)->Hphprop.push_back( new Hsamples::Hphases::Hprop );
                            ipp++; // position of the component in phcomp vector
                            experiments.at(pos)->expphases.at(ip)->phprop.at(ipp)->Qerror = NULL;
                            experiments.at(pos)->expphases.at(ip)->phprop.at(ipp)->pQnt   = NULL;
                            Hexperiments.at(pos)->Hexpphases.at(ip)->Hphprop.at(ipp)->property = false;

                            while (bson_iterator_next(&d2))
                            {
                                t = bson_iterator_type(&d2);
                                if (t == 0)
                                    break;
                                key = bson_iterator_key(&d2);
                                key_ = key;

                                if ((key_ == property))
                                {
                                    experiments.at(pos)->expphases.at(ip)->phprop.at(ipp)->property = bson_iterator_string(&d2) ;
                                    Hexperiments.at(pos)->Hexpphases.at(ip)->Hphprop.at(ipp)->property = true;
                                } else
                                if ((key_ == pQnt))
                                {
                                    experiments.at(pos)->expphases.at(ip)->phprop.at(ipp)->pQnt = bson_iterator_double(&d2) ;
                                } else
                                if ((key_ == Qerror))
                                {
                                    experiments.at(pos)->expphases.at(ip)->phprop.at(ipp)->Qerror = bson_iterator_double(&d2) ;
                                } else
                                if ((key_ == Qunit))
                                {
                                    experiments.at(pos)->expphases.at(ip)->phprop.at(ipp)->Qunit = bson_iterator_string(&d2) ;
                                }
                            }
                        }
                    }

                    // adding pahse species
                    if ((key_ == phspecies) && (t == BSON_ARRAY))
                    {
                        bson_iterator_from_buffer(&k, bson_iterator_value(&d));
                        while (bson_iterator_next(&k))
                        {
                            bson_iterator_from_buffer(&d2, bson_iterator_value(&k));
                            experiments.at(pos)->expphases.at(ip)->phspecies.push_back( new samples::phases::species );
                            Hexperiments.at(pos)->Hexpphases.at(ip)->Hphspecies.push_back( new Hsamples::Hphases::Hspecies );
                            ips++; // position of the specie in phspecies vector
                            experiments.at(pos)->expphases.at(ip)->phspecies.at(ips)->Qerror = NULL;
                            experiments.at(pos)->expphases.at(ip)->phspecies.at(ips)->sQnt   = NULL;
                            Hexperiments.at(pos)->Hexpphases.at(ip)->Hphspecies.at(ips)->formula = false;

                            while (bson_iterator_next(&d2))
                            {
                                t = bson_iterator_type(&d2);
                                if (t == 0)
                                    break;
                                key = bson_iterator_key(&d2);
                                key_ = key;

                                if ((key_ == species))
                                {
                                    experiments.at(pos)->expphases.at(ip)->phspecies.at(ips)->formula = bson_iterator_string(&d2) ;
                                    Hexperiments.at(pos)->Hexpphases.at(ip)->Hphspecies.at(ips)->formula = true;
                                } else
                                if ((key_ == sQnt))
                                {
                                    experiments.at(pos)->expphases.at(ip)->phspecies.at(ips)->sQnt = bson_iterator_double(&d2) ;
                                } else
                                if ((key_ == Qerror))
                                {
                                    experiments.at(pos)->expphases.at(ip)->phspecies.at(ips)->Qerror = bson_iterator_double(&d2) ;
                                } else
                                if ((key_ == Qunit))
                                {
                                    experiments.at(pos)->expphases.at(ip)->phspecies.at(ips)->Qunit = bson_iterator_string(&d2) ;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

