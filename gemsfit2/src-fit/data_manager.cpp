/// \file data_manager.cpp
/// Implementation of reading database parameters form input file
/// and experimental data form databsse
//
// Copyright (C) 2013-2014 G.D.Miron, D.Kulik
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
    // Read parameters for database connection
    gpf->flog << "02. data_manager.cpp(51). Reading database parameter get_db_specs(); " << endl;
    get_db_specs_txt();

    // Getting the query result data into the Data_Manager class
    gpf->flog << "03. data_manager.cpp(55). Getting data form the EJDB database; " << endl;
    get_EJDB();

    gpf->flog << "04. data_manager.cpp(58). Getting distinct T and P pairs; " << endl;
    get_distinct_TP();

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
    json_t *root;
    json_error_t jerror;

    string_v out, out2, usesample, skipsample, usedataset, skipdataset, skippdatasets,skippsamples, usepdatasets, usepsamples, SA, DS ;
    double_v qsT, qsP, WT;
    int Nsamples;

    stringstream ss;
    string sss;

    const char * JSON = DataSelect.c_str();
    root = json_loads(JSON, 0, &jerror);

    if(root)
    {
        // processing DataSelect
        parse_JSON_object(DataSelect, keys::usesamples, out);
        usesample = out; // query for selecting samples
        out.clear();

        parse_JSON_object(DataSelect, keys::skipsamples, out);
        skipsample = out; // query for selecting samples
        out.clear();

        parse_JSON_object(DataSelect, keys::usedatasets, out);
        usedataset = out; // query for selecting expdatasets
        out.clear();

        parse_JSON_object(DataSelect, keys::skipdatasets, out);
        skipdataset = out; // query for skipping expdatasets
        out.clear();

        parse_JSON_array_object(DataSelect, keys::usepair, keys::dataset, out);
        if (out.size() == 0) parse_JSON_array_object(DataSelect, keys::usepair, keys::usedatasets, out);
        usepdatasets = out; // query for skipping expdatasets
        out.clear();

        parse_JSON_array_object(DataSelect, keys::usepair, keys::samples, out);
        usepsamples = out; // query for skipping expdatasets
        out.clear();

        parse_JSON_array_object(DataSelect, keys::skippair, keys::dataset, out);
        if (out.size() == 0) parse_JSON_array_object(DataSelect, keys::skippair, keys::skipdatasets, out);
        skippdatasets = out; // query for skipping expdatasets
        out.clear();

        parse_JSON_array_object(DataSelect, keys::skippair, keys::samples, out);
        skippsamples = out; // query for skipping expdatasets
        out.clear();

        parse_JSON_object(DataSelect, keys::sT, out);
        for (unsigned int i = 0 ; i < out.size() ; i++)
        {
            qsT.push_back( atof(out.at(i).c_str()) ); // query for selecting T
        }
        out.clear();

        parse_JSON_object(DataSelect, keys::sP, out);
        for (unsigned int i = 0 ; i < out.size() ; i++)
        {
            qsP.push_back( atof(out.at(i).c_str()) ); // query for selecting P
        }
        out.clear();


        parse_JSON_object(DataSelect, keys::samplelist, out);
        Nsamples = out.size();
        for (unsigned int i = 0; i <out.size(); i++)
        {
            parse_JSON_object(out[i], keys::SA, out2);
//            if (out2.size() == 0) { cout << "Phase name has to be specified in Data Target->OFUN->EPH!"<< endl; exit(1);} // ERROR
            SA.push_back(out2[0]);
            out2.clear();

            parse_JSON_object(out[i], keys::DS, out2);
//            if (out2.size() == 0) { cout << "Phase name has to be specified in Data Target->OFUN->EPH!"<< endl; exit(1);} // ERROR
            DS.push_back(out2[0]);
            out2.clear();

            parse_JSON_object(out[i], keys::WT, out2);
//            if (out2.size() == 0) { cout << "Phase name has to be specified in Data Target->OFUN->EPH!"<< endl; exit(1);} // ERROR
            WT.push_back(atof(out2[0].c_str()));
            out2.clear();
        }

    }

    // Build the query in EJDB format

        // create EJDB database object
        static EJDB *jb;
        jb = ejdbnew();
        // open the database file as a writer JBOWRITER, create new is not existent JBOCREAT, and truncate db on open JBOTRUNC
cout << DBname.c_str() << endl;
        if (!ejdbopen(jb, DBname.c_str(), JBOREADER)) {
            cout << "Error opening the database" << endl; exit(1);
        }
// cout << gpf->GEMS3LstFilePath().c_str() << endl;
        //Get or create collection 'experiments'
        EJCOLL *coll = ejdbcreatecoll(jb, collection.c_str(), NULL);

        // Creat bson query object
        bson bq2;
        bson_init_as_query(&bq2);

        if (Nsamples == 0)
        {
            // for selecting expdatasets
            if  (usedataset.size() > 0)
            {
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
            }

            // for skipping expdatasets
            if (skipdataset.size() > 0)
            {
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
            }

            // for selecting usesamples
            if (usesample.size() > 0)
            {
                if (!usesample[0].empty())
                {
                    bson_append_start_object(&bq2, keys::expsample);
                    bson_append_start_array(&bq2, "$in");
                    for (unsigned int j=0; j<usesample.size(); ++j)
                    {
                        ss << j;
                        sss = ss.str();
                        ss.str("");
                        bson_append_string(&bq2, sss.c_str(), usesample[j].c_str());
         // cout << usesample[j].c_str() <<  endl;
                    }
                    bson_append_finish_array(&bq2);
                    bson_append_finish_object(&bq2);
                }
            }

            // for skipping skipsamples
            if (skipsample.size() > 0)
            {
                if (!skipsample[0].empty())
                {
                    bson_append_start_object(&bq2, keys::expsample);
                    bson_append_start_array(&bq2, "$nin");
                    for (unsigned int j=0; j<skipsample.size(); ++j)
                    {
                        ss << j;
                        sss = ss.str();
                        ss.str("");
                        bson_append_string(&bq2, sss.c_str(), skipsample[j].c_str());
         // cout << skipsample[j].c_str() <<  endl;
                    }
                    bson_append_finish_array(&bq2);
                    bson_append_finish_object(&bq2);
                }
            }

            // for selection of temperatures
            if (qsT.size() > 0)
            {
                if ((qsT.size() == 2))
                {
                    // for selecting T interval
                    bson_append_start_object(&bq2, keys::sT);
                    bson_append_start_array(&bq2, "$bt");
                    bson_append_double(&bq2, "0", qsT[0]);
                    bson_append_double(&bq2, "1", qsT[1]);
                    bson_append_finish_array(&bq2);
                    bson_append_finish_object(&bq2);
                } else
                    if (!(qsT[0] == 0))
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
            }

            // for selection of pressures
            if (qsP.size() > 0)
            {
                if ((qsP.size() == 2))
                {
                    // for selecting P interval
                    bson_append_start_object(&bq2, keys::sP);
                    bson_append_start_array(&bq2, "$bt");
                    bson_append_double(&bq2, "0", qsP[0]);
                    bson_append_double(&bq2, "1", qsP[1]);
                    bson_append_finish_array(&bq2);
                    bson_append_finish_object(&bq2);
                } else
                    if (!(qsP[0] == 0))
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
            }

            // for selecting pairs of datasets-samples
            if (usepdatasets.size() > 0)
            {
                if (!usepdatasets[0].empty())
                {
                    bson_append_start_array(&bq2, "$or");
                    unsigned int k=0; // counts trough the usesamples vector
                    int count = 0;
                    for (unsigned int j=0; j<usepdatasets.size(); ++j)
                    {
    //                    for (k; k<usepsamples.size(); k++)
                            while (k<usepsamples.size())
                        {
                            if (!(usepsamples[k] == ""))
                            {
                                ss << count;
                                sss = ss.str();
                                ss.str("");
                                bson_append_start_object(&bq2, sss.c_str());

                                bson_append_start_array(&bq2, "$and");

                                bson_append_start_object(&bq2, "0");
                                bson_append_string(&bq2, keys::expdataset, usepdatasets[j].c_str());
                                bson_append_finish_object(&bq2);

                                bson_append_start_object(&bq2, "1");
                                bson_append_string(&bq2, keys::expsample, usepsamples[k].c_str());
                                bson_append_finish_object(&bq2);

                                bson_append_finish_array(&bq2);
                                bson_append_finish_object(&bq2);

                                count++;

                            } else
                            {
                                k++;
                                break;
                            }
                            k++;
                        }
                    }
                    bson_append_finish_array(&bq2);
                }
            }
        } else // if we have samples list the search query is bulil as follows
        {
            // for selecting pairs of datasets-samples
            if (DS.size() > 0)
            {
                if (!DS[0].empty())
                {
                    bson_append_start_array(&bq2, "$or");
                    for  (unsigned k = 0; k<SA.size(); k++)
                    {
                        ss << k;
                        sss = ss.str();
                        ss.str("");
                        bson_append_start_object(&bq2, sss.c_str());
                        bson_append_start_array(&bq2, "$and");

                        bson_append_start_object(&bq2, "0");
                        bson_append_string(&bq2, keys::expdataset, DS[k].c_str());
                        bson_append_finish_object(&bq2);

                        bson_append_start_object(&bq2, "1");
                        bson_append_string(&bq2, keys::expsample, SA[k].c_str());
                        bson_append_finish_object(&bq2);

                        bson_append_finish_array(&bq2);
                        bson_append_finish_object(&bq2);
                    }
                    bson_append_finish_array(&bq2);
                }
            }
        }

        // finish creading bson query object
        bson_finish(&bq2);
        EJQ *q2 = ejdbcreatequery(jb, &bq2, NULL, 0, NULL);

        uint32_t count;
        TCLIST *res = ejdbqryexecute(coll, q2, &count, 0, NULL);
        fprintf(stderr, "Records found: %d\n", count);

         // adding data into Data_manager storage class
         for (unsigned int j=0; j<count; j++)
         {
             experiments.push_back( new samples );
             // set experiments variables empty
             experiments[j]->sP = 0.; experiments[j]->sT = 0.; experiments[j]->sV= 0.;
             experiments[j]->idsample= 0;
             experiments[j]->weight = 1;
             // set experiments variables false
         }

         gpf->flog << "05. data_manager.cpp(365). Adding the data returned by the selection query into the data structure; " << endl;

//#ifdef USE_MPI
         omp_set_num_threads(MPI);
         #pragma omp parallel for
//#endif
         for (int i = 0; i < TCLISTNUM(res); ++i) {
             void *bsdata = TCLISTVALPTR(res, i);
             char *bsdata_ = static_cast<char*>(bsdata);
             bson_to_Data_Manager(/*stderr,*/ bsdata_, i); // adding the data returned by the selection query into the data storage class
         }

        //Dispose result set
        tclistdel(res);

        //Dispose query
        ejdbquerydel(q2);
        bson_destroy(&bq2);

        //Close database
        ejdbclose(jb);
        ejdbdel(jb);

// Set weights provided in the sample list
        if (Nsamples > 0)
        {
            for (unsigned i = 0; i<experiments.size(); i++)
            {
                for (unsigned j = 0; j<Nsamples; j++)
                {
                    if ((experiments[i]->sample == SA[j]) && (experiments[i]->expdataset == DS[j]))
                    {
                        experiments[j]->weight = experiments[j]->weight * WT[j];
                    }
                }
            }
        }


        // for skipping expdataset-expsamples pairs
        if (skippdatasets.size() > 0)
        {
            if (!skippdatasets[0].empty())
            {
                unsigned int k=0; // counts torugh the skippsamples vector
                for (unsigned int j=0; j<skippdatasets.size(); ++j)
                {
//                    for (k; k<skippsamples.size(); k++)
                    while (k<skippsamples.size())
                    {
                        if (!(skippsamples[k] == ""))
                        {
                            for (unsigned int i = 0; i<this->experiments.size(); i++)
                            {
                                if ((this->experiments[i]->expdataset == skippdatasets[j]) && (this->experiments[i]->sample == skippsamples[k]))
                                {
                                    this->experiments.erase(this->experiments.begin() + i);
                                    count--;
                                }
                            }

                        } else
                        {
                            k++;
                            break;
                        }
                        k++;
                    }
                }
            }
            fprintf(stderr, "Records after removing skipped pairs (dataset-samples): %d\n", count);
        }
}

void Data_Manager::bson_to_Data_Manager(/* FILE *f, */ const char *data, int pos)
{
    bson_iterator i[1], j[1], k[1], k2[1], d[1], d2[1], d3[1]; // 1st, 2nd, 3rd, 2-1, 3-1 level
    int ip = -1, ic = -1, sk = -1, ipc, ipp, ips, ipdcp, ipm;

    bson_iterator_from_buffer(i, data); // getting primary iterator
    while ( bson_iterator_next(i) != BSON_EOO )
    {
        string key_ = bson_iterator_key(i);
//        key_ = key;

        if (key_ == keys::expdataset)
        {
            // adding expdataset
            experiments[pos]->expdataset = bson_iterator_string(i);
        } else
        if (key_ == keys::expsample)
        {
            // adding sample name
            experiments[pos]->sample = bson_iterator_string(i);
        } else
        if (key_ == keys::Type)
        {
            // adding sample type
            experiments[pos]->Type = bson_iterator_string(i);
        } else
        if (key_ == keys::sT)
        {
            // adding temperature
            experiments[pos]->sT = bson_iterator_double(i);
        } else
        if (key_ == keys::sP)
        {
            // adding pressure
            experiments[pos]->sP = bson_iterator_double(i);
        } else
        if (key_ == keys::sV)
        {
            // adding volume
            experiments[pos]->sV = bson_iterator_double(i);
        } else
        if (key_ == keys::Weight)
        {
            // adding volume
            experiments[pos]->weight = bson_iterator_double(i);
        } else

        // adding experiment components/recipe
        if ((key_ == keys::sbcomp) ) // && (t == BSON_ARRAY))
        {
            bson_iterator_subiterator( i, j );
            while (bson_iterator_next(j) != BSON_EOO )
            {
                experiments[pos]->sbcomp.push_back( new samples::components );
                ic++; // position of the component in sbcomp vector
                experiments[pos]->sbcomp[ic]->Qnt = 0.;
                experiments[pos]->sbcomp[ic]->Qerror = 1.;
                experiments[pos]->sbcomp[ic]->Qunit = keys::gram; // default

                bson_iterator_subiterator( j, d );

                while (bson_iterator_next(d) != BSON_EOO )
                {
                    string key_ = bson_iterator_key(d);

                    if ((key_ == keys::comp))
                    {
                        experiments[pos]->sbcomp[ic]->comp =  bson_iterator_string(d) ;
                    } else
                    if ((key_ == keys::Qnt))
                    {
                        experiments[pos]->sbcomp[ic]->Qnt = bson_iterator_double(d) ;
                    } else
                    if ((key_ == keys::Qerror))
                    {
                        experiments[pos]->sbcomp[ic]->Qerror = bson_iterator_double(d) ;
                    } else
                    if ((key_ == keys::Qunit))
                    {
                        experiments[pos]->sbcomp[ic]->Qunit = bson_iterator_string(d) ;
                    }
                }
            }
        } else

        // adding Upper metastability constraints
        if ((key_ == keys::UMC) ) // && (t == BSON_ARRAY))
        {
            bson_iterator_subiterator( i, j );
            while (bson_iterator_next(j) != BSON_EOO )
            {

                experiments[pos]->U_KC.push_back( new samples::Uconstraints );
                sk++; // position of the component in U_SK vector
//                experiments.at(pos)->U_KC.at(sk)->dcomp = NULL;
                experiments[pos]->U_KC[sk]->Qnt = 1000000.;

                bson_iterator_subiterator( j, d );
                while (bson_iterator_next(d) != BSON_EOO )
                {
                    string key_ = bson_iterator_key(d);

                    if ((key_ == keys::DC))
                    {
                        experiments[pos]->U_KC[sk]->type = keys::DC;
                        experiments[pos]->U_KC[sk]->name =  bson_iterator_string(d) ;

                    } else
                    if ((key_ == keys::phase))
                    {
                        experiments[pos]->U_KC[sk]->type = keys::phase;
                        experiments[pos]->U_KC[sk]->name = bson_iterator_string(d) ;
                    } else
                    if ((key_ == keys::Qnt))
                    {
                        experiments[pos]->U_KC[sk]->Qnt = bson_iterator_double(d) ;
                    }
                    /*else
                    if ((key_ == keys::Qerror))
                    {
                        experiments.at(pos)->sbcomp.at(ic)->Qerror = bson_iterator_double(d) ;
                    } else
                    if ((key_ == keys::Qunit))
                    {
                        experiments.at(pos)->sbcomp.at(ic)->Qunit = bson_iterator_string(d) ;
                    }*/
                }
            }
        } else

        // adding Lower metastability constraints
        if ((key_ == keys::LMC) ) // && (t == BSON_ARRAY))
        {
            bson_iterator_subiterator( i, j );
            while (bson_iterator_next(j) != BSON_EOO )
            {

                experiments[pos]->L_KC.push_back( new samples::Lconstraints );
                sk++; // position of the component in U_SK vector
    //            experiments.at(pos)->U_KC.at(sk)->dcomp = NULL;
                experiments[pos]->L_KC[sk]->Qnt = 0.;

                bson_iterator_subiterator( j, d );
                while (bson_iterator_next(d) != BSON_EOO )
                {
                    string key_ = bson_iterator_key(d);

                    if ((key_ == keys::DC))
                    {
                        experiments[pos]->L_KC[sk]->type = keys::DC;
                        experiments[pos]->L_KC[sk]->name =  bson_iterator_string(d) ;

                    } else
                    if ((key_ == keys::phase))
                    {
                        experiments[pos]->L_KC[sk]->type = keys::phase;
                        experiments[pos]->L_KC[sk]->name = bson_iterator_string(d) ;
                    } else
                    if ((key_ == keys::Qnt))
                    {
                        experiments[pos]->U_KC[sk]->Qnt = bson_iterator_double(d) ;
                    }
                     /*else
                        if ((key_ == keys::Qerror))
                        {
                            experiments.at(pos)->sbcomp.at(ic)->Qerror = bson_iterator_double(d) ;
                        } else
                        if ((key_ == keys::Qunit))
                        {
                            experiments.at(pos)->sbcomp.at(ic)->Qunit = bson_iterator_string(d) ;
                        }*/
                }
           }
        } else

        // adding experimental phases
        if ((key_ == keys::expphases) ) // && (t == BSON_ARRAY))
        {
            bson_iterator_subiterator( i, j );
            while ( bson_iterator_next(j) != BSON_EOO )
            {

                experiments[pos]->expphases.push_back( new samples::phases );
                ip++; // position of the phase in expphases vector
                ipc = -1; // phases components - reset to -1 for every new phase
                ipm = -1; // molar fractions
                ipp = -1; // phases properties
                ips = -1; // phases dcomps
                experiments[pos]->expphases[ip]->idphase = 0;

                bson_iterator_subiterator( j, d );
                while (bson_iterator_next(d) != BSON_EOO )
                {
                    string key_ = bson_iterator_key(d);

                    if ((key_ == keys::phase))
                    {
                        experiments[pos]->expphases[ip]->phase =  bson_iterator_string(d) ;
                    } else

                    // adding phase components
                    if ((key_ == keys::phIC) ) //  && (t == BSON_ARRAY))
                    {
                        bson_iterator_subiterator( d, k );
                        while ( bson_iterator_next(k) != BSON_EOO )
                        {
                            experiments[pos]->expphases[ip]->phIC.push_back( new samples::components );
                            ipc++; // position of the component in phcomp vector
                            experiments[pos]->expphases[ip]->phIC[ipc]->Qerror = 1.;
                            experiments[pos]->expphases[ip]->phIC[ipc]->Qnt   = 0.;
                            string p_name = experiments[pos]->expphases[ip]->phase;
                            if (p_name == keys::aqueous)
                            {
                                experiments[pos]->expphases[ip]->phIC[ipc]->Qunit = keys::molal;
                            } else
                                experiments[pos]->expphases[ip]->phIC[ipc]->Qunit = keys::molratio;

                            bson_iterator_subiterator( k, d2 );
                            while ( bson_iterator_next(d2) != BSON_EOO )
                            {
                                string key_ = bson_iterator_key(d2);

                                if ((key_ == keys::IC))
                                {
                                    experiments[pos]->expphases[ip]->phIC[ipc]->comp = bson_iterator_string(d2) ;
                                } else
                                if ((key_ == keys::Qnt))
                                {
                                    experiments[pos]->expphases[ip]->phIC[ipc]->Qnt = bson_iterator_double(d2) ;
                                } else
                                if ((key_ == keys::Qerror))
                                {
                                    experiments[pos]->expphases[ip]->phIC[ipc]->Qerror = bson_iterator_double(d2) ;
                                } else
                                if ((key_ == keys::Qunit))
                                {
                                    experiments[pos]->expphases[ip]->phIC[ipc]->Qunit = bson_iterator_string(d2) ;
                                }
                            }
                        }
                    } else

                    // adding phase IC as mole fractions MR
                    if ((key_ == keys::phMR) ) // && (t == BSON_ARRAY))
                    {
                        bson_iterator_subiterator( d, k );
                        while (bson_iterator_next(k) != BSON_EOO )
                        {
                            experiments[pos]->expphases[ip]->phMR.push_back( new samples::components );
                            ipm++; // position of the component in phcomp vector
                            experiments[pos]->expphases[ip]->phMR[ipm]->Qerror = 1.;
                            experiments[pos]->expphases[ip]->phMR[ipm]->Qnt   = 0.;
                            string p_name = experiments[pos]->expphases[ip]->phase;
                            if (p_name == keys::aqueous)
                            {
                                experiments[pos]->expphases[ip]->phMR[ipm]->Qunit = keys::molratio;
                            } else
                                experiments[pos]->expphases[ip]->phMR[ipm]->Qunit = keys::molratio;

                            bson_iterator_subiterator( k, d2 );
                            while (bson_iterator_next(d2) != BSON_EOO )
                            {
                                string key_ = bson_iterator_key(d2);

                                if ((key_ == keys::MR))
                                {
                                    experiments[pos]->expphases[ip]->phMR[ipm]->comp = bson_iterator_string(d2) ;
                                } else
                                if ((key_ == keys::Qnt))
                                {
                                    experiments[pos]->expphases[ip]->phMR[ipm]->Qnt = bson_iterator_double(d2) ;
                                 } else
                                 if ((key_ == keys::Qerror))
                                 {
                                    experiments[pos]->expphases[ip]->phMR[ipm]->Qerror = bson_iterator_double(d2) ;
                                 } else
                                 if ((key_ == keys::Qunit))
                                 {
                                    experiments[pos]->expphases[ip]->phMR[ipm]->Qunit = bson_iterator_string(d2) ;
                                 }
                             }
                        }
                    } else

                    // adding phase properties
                    if ((key_ == keys::phprop) ) // && (t == BSON_ARRAY))
                    {
                        bson_iterator_subiterator( d, k );
                        while ( bson_iterator_next(k) != BSON_EOO )
                        {
                            experiments[pos]->expphases[ip]->phprop.push_back( new samples::phases::prop );
                            ipp++; // position of the component in phcomp vector
                            experiments[pos]->expphases[ip]->phprop[ipp]->Qerror = 1.;
                            experiments[pos]->expphases[ip]->phprop[ipp]->Qnt   = 0.;
//cout << "pos: " << pos << " ip: " << ip << " ipp: " << ipp << " :: ";
                            bson_iterator_subiterator( k, d2 );
                            while (bson_iterator_next(d2) != BSON_EOO )
                            {
                                string key_ = bson_iterator_key(d2);
//cout << " " << key_.c_str();
                                if ((key_ == keys::property))
                                {
                                    string p_name = bson_iterator_string(d2);
//cout << ": " << p_name;
                                    experiments[pos]->expphases[ip]->phprop[ipp]->property = p_name; // bson_iterator_string(d2) ;
                                    // assigining default values for units
                                    if (p_name==keys::Qnt)
                                    {
                                        experiments[pos]->expphases[ip]->phprop[ipp]->Qunit = keys::gram;
                                    } else
                                    if (p_name==keys::RHO)
                                    {
                                        experiments[pos]->expphases[ip]->phprop[ipp]->Qunit = keys::g_cm3;
                                    } else
                                    if (p_name==keys::Gex)
                                    {
                                        experiments[pos]->expphases[ip]->phprop[ipp]->Qunit = keys::J_mol;
                                    } else
                                    if (p_name==keys::pV)
                                    {
                                        experiments[pos]->expphases[ip]->phprop[ipp]->Qunit = keys::cm3;
                                    } else
                                    if (p_name==keys::pH)
                                    {
                                        experiments[pos]->expphases[ip]->phprop[ipp]->Qunit = keys::_loga;
                                    }
                                    if (p_name==keys::Eh)
                                    {
                                        experiments[pos]->expphases[ip]->phprop[ipp]->Qunit = keys::Volts;
                                    } else
                                    if (p_name==keys::pe)
                                    {
                                        experiments[pos]->expphases[ip]->phprop[ipp]->Qunit = keys::_loga;
                                    } else
                                    if (p_name==keys::oscw)
                                    {
//                                        experiments[pos]->expphases[ip]->phprop[ipp]->Qunit = keys::_loga;
                                    }
                                } else
                                if ((key_ == keys::Qnt))
                                {
                                    double qw = bson_iterator_double(d2) ;
                                    experiments[pos]->expphases[ip]->phprop[ipp]->Qnt = qw;
//cout << ": " << qw;
                                } else
                                if ((key_ == keys::Qerror))
                                {
                                    double qw = bson_iterator_double(d2) ;
                                    experiments[pos]->expphases[ip]->phprop[ipp]->Qerror = qw ;
//cout << ": " << qw;
                                } else
                                if ((key_ == keys::Qunit))
                                {
                                    string unit = bson_iterator_string(d2) ;
                                    experiments[pos]->expphases[ip]->phprop[ipp]->Qunit = unit ;
//cout << ": " << unit;
                                }
                            }
                        }
//cout << endl;
                    } else

                    // adding phase dcomps
                    if ((key_ == keys::phDC) ) // && (t == BSON_ARRAY))
                    {
                        bson_iterator_subiterator( d, k );
                        while (bson_iterator_next(k) != BSON_EOO )
                        {
                            experiments[pos]->expphases[ip]->phDC.push_back( new samples::phases::dcomps );
                            ips++; // position of the specie in phdcomps vector
                            ipdcp = -1;

                            bson_iterator_subiterator( k, d2 );
                            while (bson_iterator_next(d2) != BSON_EOO )
                            {
                                string key_ = bson_iterator_key(d2);

                                if ((key_ == keys::DC)) // adding the name of the specie
                                {
                                    experiments[pos]->expphases[ip]->phDC[ips]->DC =  bson_iterator_string(d2) ;
                                } else

                                // adding dependent compoponents properties
                                if ((key_ == keys::DCprop) ) // && (t == BSON_ARRAY))
                                {
                                    bson_iterator_subiterator( d2, k2 );
                                    while (bson_iterator_next(k2) != BSON_EOO )
                                    {
                                        experiments[pos]->expphases[ip]->phDC[ips]->DCprop.push_back( new samples::phases::dcomps::dcprop );
                                        ipdcp++; // position of the component in phcomp vector
                                        experiments[pos]->expphases[ip]->phDC[ips]->DCprop[ipdcp]->Qerror = 1.;
                                        experiments[pos]->expphases[ip]->phDC[ips]->DCprop[ipdcp]->Qnt   = 0.;

                                        bson_iterator_subiterator( k2, d3 );
                                        while (bson_iterator_next(d3) != BSON_EOO )
                                        {
                                            string key_ = bson_iterator_key(d3);

                                            if ((key_ == keys::property))
                                            {
                                                experiments[pos]->expphases[ip]->phDC[ips]->DCprop[ipdcp]->property = bson_iterator_string(d3);
                                            } else
                                            if ((key_ == keys::Qnt))
                                            {
                                                experiments[pos]->expphases[ip]->phDC[ips]->DCprop[ipdcp]->Qnt = bson_iterator_double(d3) ;
                                            } else
                                            if ((key_ == keys::Qerror))
                                            {
                                                experiments[pos]->expphases[ip]->phDC[ips]->DCprop[ipdcp]->Qerror = bson_iterator_double(d3) ;
                                            } else
                                            if ((key_ == keys::Qunit))
                                            {
                                                experiments[pos]->expphases[ip]->phDC[ips]->DCprop[ipdcp]->Qunit = bson_iterator_string(d3) ;
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
//cout << endl;
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
                        result.push_back(json_dumps(data, 1));
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
    json_t *root; json_t *data1; json_t *data; json_t *object; json_t *data2;
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
                    else if (json_is_array(data))
                    {
                        for(unsigned int j = 0; j < json_array_size(data); j++)
                        {
                            data2 = json_array_get(data, j);
                            if(json_is_string(data2))
                            {
                                result.push_back(json_string_value(data2));
                            }
                            else if (json_is_real(data2))
                            {
                                ss << json_real_value(data2);
                                sss = ss.str();
                                ss.str("");
                                result.push_back(sss);
                            }
                            else if (json_is_integer(data2))
                            {
                                ss << json_integer_value(data2);
                                sss = ss.str();
                                ss.str("");
                                result.push_back(sss);
                            }
                        }
                        if ((arr == keys::usepair) || (arr == keys::skippair))
                        {result.push_back("");}
                    }
                }
             }
         }
        else
        {
//            data = json_object_get(data1, key);  // maybe error
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


