# include <iostream>
# include "csv_parser.h"
# include <ejdb.h>
# include <locale.h>
# include "read_ejdb.h"
# include "keywords.h"
#include <boost/lexical_cast.hpp>

using namespace std;
//using namespace keys;

static EJDB *jb;

int main(int argc, char *argv[])
{
    int ic=0;
    string ph_new, ph_old;

    // create EJDB databse object
    jb = ejdbnew();

    // create directory for db if not existent
    if (0 != access(argv[1], F_OK)) {
      if (ENOENT == errno) {
         // does not exist
          mkdir(argv[1], S_IRWXU|S_IRGRP|S_IXGRP);
      }
      if (ENOTDIR == errno) {
         // not a directory
      }
    }

    char ejdb_path[64] = {};
    cout << ejdb_path << endl;

    strcat(ejdb_path, argv[1]);
    strcat(ejdb_path, argv[2]);
    cout << ejdb_path << endl;
    // open the database file as a writer JBOWRITER, create new is not existent JBOCREAT, and truncate db on open JBOTRUNC
    if (!ejdbopen(jb, ejdb_path, JBOWRITER | JBOCREAT | JBOTRUNC)) {
        return 1;
    }

    //Get or create collection 'contacts'
    EJCOLL *coll = ejdbcreatecoll(jb, experiments, NULL);

    bson_oid_t oid;
    // keeps each row of the CSV file
    vector<string> row;
    // keeps headline of the CSV file
    vector<string> headline;
    string line;

    char csv_path[64] = {};
    strcat(csv_path, argv[1]);
    strcat(csv_path, argv[3]);
    ifstream in(csv_path);
    if (in.fail())  { cout << "File not found" <<endl; return 0; }

    getline(in, line);
    // getting headline
    csvline(headline, line, ',');

    // getting the data form CSV line by line and porcessing it into BSON
    while(getline(in, line)  && in.good() )
    {
        ic = 0;
        csvline(row, line, ',');
        // going trough the headline markers to identify the data type, based on which it is assigned into the database
        bson exp;
        bson_init(&exp);

        //first level objects: sample, expdataset, sT, sP.
        for (int i=0; i<headline.size(); ++i)
        {
            if ((headline[i]==expsample) || (headline[i]==expdataset) || (headline[i]==Tunit) || (headline[i]==Punit)|| (headline[i]==Vunit) )
            {
                bson_append_string(&exp, headline[i].c_str(), row[i].c_str());
            }
            else if  ((headline[i]==sT) || (headline[i]==sP))
            {
                bson_append_int(&exp, headline[i].c_str(), atoi(row[i].c_str()));
            }
            else if (headline[i]==sV)
            {
                bson_append_double(&exp, headline[i].c_str(), atof(row[i].c_str()));
            }

         }

            // 2nd level - bulk composition of chemical system for this experiment
        // array of components
        //++ START array sbcomp ++//
        bson_append_start_array(&exp, sbcomp);
        for (int i=0; i<headline.size(); ++i)
        {
            if ((strncmp(headline[i].c_str(),comp, 4) == 0) && (!row[i].empty()))
            {
                int pos_start, pos_end;
                string component;
                string f1(".");

                // getting the name of the component e.g. SiO2 form comp.SiO2
                pos_start = headline[i].find(f1);
                pos_end   = headline[i].find(f1,pos_start+1);
                component = headline[i].substr((pos_start+f1.length()),(pos_end-pos_start-f1.length()));

                bson_append_start_object(&exp, boost::lexical_cast<string>(ic).c_str());
                ic++;
                bson_append_string(&exp, comp, component.c_str());
                bson_append_double(&exp, bQnt, atof(row[i].c_str()));

                // checking if there are errors and units included in the CSV and adding tem in the database
                if ((headline[i+1]==_error))
                {
                    ++i;
                    if ((!row[i].empty()))
                    {
                    bson_append_double(&exp, Qerror, atof(row[i].c_str()));
                    }
                }
                if ((headline[i+1]==_unit) && (!row[i+1].empty()))
                {
                    ++i;
                    bson_append_string(&exp, Qunit, row[i].c_str());
                }
                bson_append_finish_object(&exp);

            }
        }
        //++ END array sbcomp ++//
        bson_append_finish_array(&exp);
        ic=0;



            // 2nd level - data for phases charactrised/measured in this experiment
        //++ START array expphases ++//
        bson_append_start_array(&exp, expphases);

        for (int i=0; i<headline.size(); ++i)
        {
            if ((strncmp(headline[i].c_str(),phase, 5) == 0) && (!row[i].empty()))
            {
                int pos_start, pos_end;
                string phase_name, ph_prop;
                string f1(".");

                // getting the name of the phase e.g. aq_gen form phase.aq_gen
                pos_start = headline[i].find(f1);
                pos_end   = headline[i].find(f1,pos_start+1);
                phase_name = headline[i].substr((pos_start+f1.length()),(pos_end-pos_start-f1.length()));
                ph_new = phase_name;

                if (ph_new != ph_old)
                {
                    bson_append_string(&exp, phase, phase_name.c_str());
                    ic = 0;

                    //++ START array phprop ++//
                    bson_append_start_array(&exp, phprop);

                    // get phase poperties
                    for (int j=0; j<headline.size(); ++j)
                    {
                        if ((strncmp(headline[j].c_str(),phase, 5) == 0) && (!row[j].empty()))
                        {
                        pos_start = headline[j].find(f1);
                        pos_end   = headline[j].find(f1,pos_start+1);
                        // getting the phase properties and composition
                        if (phase_name == headline[j].substr((pos_start+f1.length()),(pos_end-pos_start-f1.length())))
                        {
                            // getting the name of the property phase e.g. Si from phase.aq_gen.Si, or pQunt from pahse.aq_gen.pQunt
                            ph_prop = headline[j].substr((pos_end+f1.length()),(headline[j].size()));

                            // qunatity of this pahse in the experiment
                            if (((ph_prop == pQnt) || (ph_prop == pH) || (ph_prop == pV) ||  (ph_prop == Eh) || (ph_prop == IS) || (ph_prop == all) ||  (ph_prop == sArea)) && (!row[j].empty()))
                            {
                                bson_append_start_object(&exp, boost::lexical_cast<string>(ic).c_str());
                                ic++;

                                bson_append_string(&exp, property, ph_prop.c_str());
                                bson_append_double(&exp, pQnt, atof(row[j].c_str()));
                                // checking if there are errors and units included in the CSV and adding tem in the database
                                if ((headline[j+1]==_error))
                                {
                                    ++j;
                                    if ((!row[j].empty()))
                                    {
                                    bson_append_double(&exp, Qerror, atof(row[j].c_str()));
                                    }
                                }
                                if ((headline[j+1]==_unit) && (!row[j+1].empty()))
                                {
                                    ++j;
                                    bson_append_string(&exp, _unit, row[j].c_str());
                                }

                                bson_append_finish_object(&exp);
                                ph_old = phase_name;
                            }
                        }
                        }
                    }

                    //++ END array phprop ++//
                    bson_append_finish_array(&exp);
                    ic =0;

                    //++ START array phcomp ++//
                    bson_append_start_array(&exp, phcomp);
                    // get phase comp
                    for (int j=0; j<headline.size(); ++j)
                    {
                        if ((strncmp(headline[j].c_str(),phase, 5) == 0) && (!row[j].empty()))
                        {
                        pos_start = headline[j].find(f1);
                        pos_end   = headline[j].find(f1,pos_start+1);
                        // getting the phase properties and composition
                        if (phase_name == headline[j].substr((pos_start+f1.length()),(pos_end-pos_start-f1.length())))
                        {
                            // getting the name of the property phase e.g. Si from phase.aq_gen.Si, or pQunt from pahse.aq_gen.pQunt
                            ph_prop = headline[j].substr((pos_end+f1.length()),(headline[j].size()));

                            // qunatity of this pahse in the experiment
                            if (((ph_prop != pQnt) && (ph_prop != pH) && (ph_prop != pV) &&  (ph_prop != Eh) && (ph_prop != IS) && (ph_prop != all) &&  (ph_prop != sArea)) && (strncmp(ph_prop.c_str(),"s", 1) != 0) && (!row[j].empty()))
                            {
                                bson_append_start_object(&exp, boost::lexical_cast<string>(ic).c_str());
                                ic++;

                                bson_append_string(&exp, property, ph_prop.c_str());
                                bson_append_double(&exp, pQnt, atof(row[j].c_str()));
                                // checking if there are errors and units included in the CSV and adding tem in the database
                                if ((headline[j+1]==_error))
                                {
                                    ++j;
                                    if ((!row[j].empty()))
                                    {
                                    bson_append_double(&exp, Qerror, atof(row[j].c_str()));
                                    }
                                }
                                if ((headline[j+1]==_unit) && (!row[j+1].empty()))
                                {
                                    ++j;
                                    bson_append_string(&exp, _unit, row[j].c_str());
                                }

                                bson_append_finish_object(&exp);
                                ph_old = phase_name;

                            }
                        }
                        }
                    }
                    //++ END array phcomp ++//
                    bson_append_finish_array(&exp);
                    ic = 0;

                    //++ START array phspecies ++//
                    if ((strncmp(ph_prop.c_str(),"s", 1) == 0) && (!row[i].empty())) // check is there is species data in the CSV header
                    {
                    bson_append_start_array(&exp, phspecies);
                    for (int j=0; j<headline.size(); ++j)
                    {
                        if ((strncmp(headline[j].c_str(),phase, 5) == 0) && (!row[j].empty()))
                        {
                        pos_start = headline[j].find(f1);
                        pos_end   = headline[j].find(f1,pos_start+1);
                        // getting the phase species
                        if (phase_name == headline[j].substr((pos_start+f1.length()),(pos_end-pos_start-f1.length())))
                        {
                            ph_prop = headline[j].substr((pos_end+f1.length()),(headline[j].size()));

                            // specie
                            if ((strncmp(ph_prop.c_str(),"s", 1) == 0) && (!row[j].empty()))
                            {
                                bson_append_start_object(&exp, boost::lexical_cast<string>(ic).c_str());
                                ic++;

                                bson_append_string(&exp, species, ph_prop.c_str());
                                bson_append_double(&exp, sQnt, atof(row[j].c_str()));

                                // if error and unit follow in the CSV they are added in the DB
                                if ((headline[j+1]==_error))
                                {
                                    ++j;
                                    if ((!row[j].empty()))
                                    {
                                    bson_append_double(&exp, Qerror, atof(row[j].c_str()));
                                    }
                                }
                                if ((headline[j+1]==_unit) && (!row[j+1].empty()))
                                {
                                    ++j;
                                    bson_append_string(&exp, Qunit, row[j].c_str());
                                }

                                bson_append_finish_object(&exp);
                                ph_old = phase_name;

                            }
                        }
                        }
                    }
                    //++ END array phspecies ++//
                    bson_append_finish_array(&exp);
                    }
                }
            }
        }
        //++ END array expphases ++//
        bson_append_finish_array(&exp);

        bson_finish(&exp);
        ejdbsavebson(coll, &exp, &oid);
        bson_destroy(&exp);

//        for(int i=0, leng=row.size(); i<leng; i++)
//           cout << row[i] << "\t";
//           cout << endl;
    }

    in.close();

    //Now select records.
    //Query: {'expdataset' : {'$begin' : 'CH'}} //Expdataset name starts with 'CH' string
    bson bq1;
    bson_init_as_query(&bq1);
    bson_append_start_object(&bq1, expdataset);
    bson_append_string(&bq1, "$begin", "CH");
    bson_append_finish_object(&bq1);
    bson_finish(&bq1);
    EJQ *q1 = ejdbcreatequery(jb, &bq1, NULL, 0, NULL);

//    {   "sample" : "",
//        "expdataset" : ["CHO4", "CH04D"],
//        "sT" : [100, 1000],
//        "sP" : [1, 2500]
//    }

    bson bq2;
    bson_init_as_query(&bq2);
    bson_append_start_object(&bq2, expdataset);
    bson_append_start_array(&bq2, "$in");
    bson_append_string(&bq2, "0", "CH04D");
    bson_append_string(&bq2, "1", "CH04");
    bson_append_finish_array(&bq2);
    bson_append_finish_object(&bq2);

    bson_append_start_object(&bq2, sT);
    bson_append_start_array(&bq2, "$bt");
    bson_append_string(&bq2, "0", "100");
    bson_append_string(&bq2, "1", "1500");
    bson_append_finish_array(&bq2);
    bson_append_finish_object(&bq2);

//    bson_append_start_array(&bq2, sT);
//    bson_append_string(&bq2, "$bt", "10");
//    bson_append_string(&bq2, "$bt", "1000");
//    bson_append_finish_array(&bq2);
//    bson_append_finish_object(&bq2);

    bson_finish(&bq2);
    EJQ *q2 = ejdbcreatequery(jb, &bq2, NULL, 0, NULL);


    uint32_t count;
    TCLIST *res = ejdbqryexecute(coll, q1, &count, 0, NULL);
    TCLIST *res2 = ejdbqryexecute(coll, q2, &count, 0, NULL);
    fprintf(stderr, "\n\nRecords found: %d\n", count);

    //Now print the result set records
     for (int i = 0; i < TCLISTNUM(res); ++i) {
         void *bsdata = TCLISTVALPTR(res, i);
         char *bsdata_ = static_cast<char*>(bsdata);
         bson_print_raw(stderr, bsdata_, 0);
     }
     fprintf(stderr, "\n");

//     for (int i = 0; i < TCLISTNUM(res2); ++i) {
//         void *bsdata = TCLISTVALPTR(res2, i);
//         char *bsdata_ = static_cast<char*>(bsdata);


// testing data output!! still working on it
//         bson_read_raw(stderr, bsdata_);
//     }
//     fprintf(stderr, "\n");

    //Dispose result set
    tclistdel(res);

    tclistdel(res2);

    //Dispose query
    ejdbquerydel(q1);
    bson_destroy(&bq1);

    ejdbquerydel(q2);
    bson_destroy(&bq2);

    //Close database
    ejdbclose(jb);
    ejdbdel(jb);

    return 0;
}

