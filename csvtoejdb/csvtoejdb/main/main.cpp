#include <iostream>
#include "csv_praser.h"
#include <tcejdb/ejdb.h>
#include <locale.h>

using namespace std;

static EJDB *jb;

int main(int argc, char *argv[])
{
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
    EJCOLL *coll = ejdbcreatecoll(jb, "experiments", NULL);

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
        csvline(row, line, ',');
        // going trough the headline markers to identify the data type, based on which it is assigned into the database
        bson exp;
        bson_init(&exp);

        //first level objects: sample, expdataset, sT, sP.
        for (int i=0; i<headline.size(); ++i)
        {
            if ((headline[i]=="sample") || (headline[i]=="expdataset") || (headline[i]=="Tunit") || (headline[i]=="Punit")|| (headline[i]=="Vunit") )
            {
                bson_append_string(&exp, headline[i].c_str(), row[i].c_str());
            }
            else if  ((headline[i]=="sT") || (headline[i]=="sP"))
            {
                bson_append_int(&exp, headline[i].c_str(), atoi(row[i].c_str()));
            }
            else if (headline[i]=="sV")
            {
                bson_append_double(&exp, headline[i].c_str(), atof(row[i].c_str()));
            }

            // 2nd level - bulk composition of chemical system for this experiment
            else if ((strncmp(headline[i].c_str(),"comp", 4) == 0) && (!row[i].empty()))
            {
                int pos_start, pos_end;
                string comp;
                string f1(".");

                // getting the name of the component e.g. SiO2 form comp.SiO2
                pos_start = headline[i].find(f1);
                pos_end   = headline[i].find(f1,pos_start+1);
                comp = headline[i].substr((pos_start+f1.length()),(pos_end-pos_start-f1.length()));

                // array of components
                //++ START array sbcomp ++//
                bson_append_start_array(&exp, "sbcomp");
                bson_append_string(&exp, "comp", comp.c_str());
                bson_append_double(&exp, "bQnt", atof(row[i].c_str()));

                // checking if there are errors and units included in the CSV and adding tem in the database
                if ((headline[i+1]=="error") && (!row[i+1].empty()))
                {
                    ++i;
                    bson_append_double(&exp, "Qerror", atof(row[i].c_str()));
                }
                if ((headline[i+1]=="unit") && (!row[i+1].empty()))
                {
                    ++i;
                    bson_append_string(&exp, "Qunit", row[i].c_str());
                }
                //++ END array sbcomp ++//
                bson_append_finish_array(&exp);
            }
            // 2nd level - data for phases charactrised/measured in this experiment
            else if ((strncmp(headline[i].c_str(),"phase", 5) == 0) && (!row[i].empty()))
            {
                int pos_start, pos_end;
                string phase, ph_prop;
                string f1(".");

                // getting the name of the phase e.g. aq_gen form phase.aq_gen
                pos_start = headline[i].find(f1);
                pos_end   = headline[i].find(f1,pos_start+1);
                phase = headline[i].substr((pos_start+f1.length()),(pos_end-pos_start-f1.length()));

                //++ START array expphases ++//
                bson_append_start_array(&exp, "expphases");
                bson_append_string(&exp, "phase", phase.c_str());

                // getting the name of the property phase e.g. Si from phase.aq_gen.Si, or pQunt from pahse.aq_gen.pQunt
                ph_prop = headline[i].substr((pos_end+f1.length()),(headline[i].size()));

                // qunatity of this pahse in the experiment
                if ((ph_prop == "pQnt") && (!row[i].empty()))
                {
                    //++ START array phprop ++//
                    bson_append_start_array(&exp, "phprop");
                    bson_append_double(&exp, "pQnt", atof(row[i].c_str()));
                    // checking if there are errors and units included in the CSV and adding tem in the database
                    if ((headline[i+1]=="error") && (!row[i+1].empty()))
                    {
                        ++i;
                        bson_append_double(&exp, "error", atof(row[i].c_str()));
                    }
                    if ((headline[i+1]=="unit") && (!row[i+1].empty()))
                    {
                        ++i;
                        bson_append_string(&exp, "unit", row[i].c_str());
                    }
                    //++ END array phprop ++//
                    bson_append_finish_array(&exp);
                }

                // pH of this pahse in the experiment
                else if ((ph_prop == "pH") && (!row[i].empty()))
                {
                    //++ START array phprop ++//
                    bson_append_start_array(&exp, "phprop");
                    bson_append_double(&exp, "pH", atof(row[i].c_str()));
                    // checking if there are errors and units included in the CSV and adding tem in the database
                    if ((headline[i+1]=="error") && (!row[i+1].empty()))
                    {
                        ++i;
                        bson_append_double(&exp, "error", atof(row[i].c_str()));
                    }
                    if ((headline[i+1]=="unit") && (!row[i+1].empty()))
                    {
                        ++i;
                        bson_append_string(&exp, "unit", row[i].c_str());
                    }
                    //++ END array phprop ++//
                    bson_append_finish_array(&exp);
                }
                // volume of this pahse in the experiment
                else if ((ph_prop == "pV") && (!row[i].empty()))
                {
                    //++ START array phprop ++//
                    bson_append_start_array(&exp, "phprop");
                    bson_append_double(&exp, "pH", atof(row[i].c_str()));
                    // checking if there are errors and units included in the CSV and adding tem in the database
                    if ((headline[i+1]=="error") && (!row[i+1].empty()))
                    {
                        ++i;
                        bson_append_double(&exp, "error", atof(row[i].c_str()));
                    }
                    if ((headline[i+1]=="unit") && (!row[i+1].empty()))
                    {
                        ++i;
                        bson_append_string(&exp, "unit", row[i].c_str());
                    }
                    //++ END array phprop ++//
                    bson_append_finish_array(&exp);
                }
                // Eh of this pahse in the experiment
                else if ((ph_prop == "Eh") && (!row[i].empty()))
                {
                    //++ START array phprop ++//
                    bson_append_start_array(&exp, "phprop");
                    bson_append_double(&exp, "Eh", atof(row[i].c_str()));
                    // checking if there are errors and units included in the CSV and adding tem in the database
                    if ((headline[i+1]=="error") && (!row[i+1].empty()))
                    {
                        ++i;
                        bson_append_double(&exp, "error", atof(row[i].c_str()));
                    }
                    if ((headline[i+1]=="unit") && (!row[i+1].empty()))
                    {
                        ++i;
                        bson_append_string(&exp, "unit", row[i].c_str());
                    }
                    //++ END array phprop ++//
                    bson_append_finish_array(&exp);
                }
                // IS (ionic strength) of this pahse in the experiment
                else if ((ph_prop == "IS") && (!row[i].empty()))
                {
                    //++ START array phprop ++//
                    bson_append_start_array(&exp, "phprop");
                    bson_append_double(&exp, "IS", atof(row[i].c_str()));
                    // checking if there are errors and units included in the CSV and adding tem in the database
                    if ((headline[i+1]=="error") && (!row[i+1].empty()))
                    {
                        ++i;
                        bson_append_double(&exp, "error", atof(row[i].c_str()));
                    }
                    if ((headline[i+1]=="unit") && (!row[i+1].empty()))
                    {
                        ++i;
                        bson_append_string(&exp, "unit", row[i].c_str());
                    }
                    //++ END array phprop ++//
                    bson_append_finish_array(&exp);
                }
                // Alkalinity of this pahse in the experiment
                else if ((ph_prop == "all") && (!row[i].empty()))
                {
                    //++ START array phprop ++//
                    bson_append_start_array(&exp, "phprop");
                    bson_append_double(&exp, "all", atof(row[i].c_str()));
                    // checking if there are errors and units included in the CSV and adding tem in the database
                    if ((headline[i+1]=="error") && (!row[i+1].empty()))
                    {
                        ++i;
                        bson_append_double(&exp, "error", atof(row[i].c_str()));
                    }
                    if ((headline[i+1]=="unit") && (!row[i+1].empty()))
                    {
                        ++i;
                        bson_append_string(&exp, "unit", row[i].c_str());
                    }
                    //++ END array phprop ++//
                    bson_append_finish_array(&exp);
                }
                // Surface area of this pahse in the experiment
                else if ((ph_prop == "sArea") && (!row[i].empty()))
                {
                    //++ START array phprop ++//
                    bson_append_start_array(&exp, "phprop");
                    bson_append_double(&exp, "sArea", atof(row[i].c_str()));
                    // checking if there are errors and units included in the CSV and adding tem in the database
                    if ((headline[i+1]=="error") && (!row[i+1].empty()))
                    {
                        ++i;
                        bson_append_double(&exp, "error", atof(row[i].c_str()));
                    }
                    if ((headline[i+1]=="unit") && (!row[i+1].empty()))
                    {
                        ++i;
                        bson_append_string(&exp, "unit", row[i].c_str());
                    }
                    //++ END array phprop ++//
                    bson_append_finish_array(&exp);
                }

                // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                // name of chemical species (end member, pahse component)
                else if ((ph_prop == "species") && (!row[i].empty())) {}
                // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

                // 3rd level - measured composition of the phase
                else
                {
                    if (!row[i].empty())
                    {
                    //++ START array phcomp ++//
                    bson_append_start_array(&exp, "phcomp");
                    bson_append_string(&exp, "element", ph_prop.c_str());
                    bson_append_double(&exp, "eQnt", atof(row[i].c_str()));

                    // if error and unit follow in the CSV they are added in the DB
                    if ((headline[i+1]=="error") && (!row[i+1].empty()))
                    {
                        ++i;
                        bson_append_double(&exp, "Qerror", atof(row[i].c_str()));
                    }
                    if ((headline[i+1]=="unit") && (!row[i+1].empty()))
                    {
                        ++i;
                        bson_append_string(&exp, "Qunit", row[i].c_str());
                    }
                    //++ END array phcomp ++//
                    bson_append_finish_array(&exp);
                    }
                }
                //++ END array expphases ++//
                bson_append_finish_array(&exp);
            }
        }

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
    bson_append_start_object(&bq1, "expdataset");
    bson_append_string(&bq1, "$begin", "qCH");
    bson_append_finish_object(&bq1);
    bson_finish(&bq1);
    EJQ *q1 = ejdbcreatequery(jb, &bq1, NULL, 0, NULL);

    uint32_t count;
    TCLIST *res = ejdbqryexecute(coll, q1, &count, 0, NULL);
    fprintf(stderr, "\n\nRecords found: %d\n", count);

    //Now print the result set records
     for (int i = 0; i < TCLISTNUM(res); ++i) {
         void *bsdata = TCLISTVALPTR(res, i);
         char *bsdata_ = static_cast<char*>(bsdata);
         bson_print_raw(stderr, bsdata_, 0);
     }
     fprintf(stderr, "\n");

    //Dispose result set
    tclistdel(res);

    //Dispose query
    ejdbquerydel(q1);
    bson_destroy(&bq1);

    //Close database
    ejdbclose(jb);
    ejdbdel(jb);

    return 0;
}

