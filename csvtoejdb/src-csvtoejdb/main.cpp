# include <iostream>
# include <ejdb.h>
# include <locale.h>

//#include <stdio.h>
#include <string.h>

# include "keywords.h"
# include "ejdbtojson.h"
# include "csvtoejdb.h"
# include "jsontoejdb.h"

using namespace std;
using namespace keys;

static EJDB *jb;

int main(int argc, char *argv[])
{
    int iinit = 0, irun = 0, ihelp = 0, iback = 0, irest = 0;
    vector<int> TP[2], TP_pairs[2]; // holding the temperature and pressures and the uniques pairs for printing
    bool isfound = false, isfound2 = false;
    string coll_name;

    // create EJDB databse object
    jb = ejdbnew();
    EJCOLL *coll = ejdbcreatecoll(jb, experiments, NULL);
    coll_name = experiments;

    for (int ii = 1; ii < argc; ii++)                                           //A.1 GETTING THE RUNNING MODES
    {
        if (strcmp(argv[ii], "-init") == 0 )
            iinit = ii;
            else if (strcmp(argv[ii], "-run") == 0 )
                irun = ii;
                else if (strcmp(argv[ii], "-help") == 0)
                     ihelp = ii;
                     else if (strcmp(argv[ii], "-back") == 0)
                        iback = ii;
                        else if (strcmp(argv[ii], "-rest") == 0)
                            irest = ii;
    }

    if (ihelp != 0)                                                             //A.2 HELP MODE
    {
        cout << " USAGE: \n"
                "   csvtoejdb   -run -t <path to the database and CSV file> <collection name> <path to the CSV file> \n\n"
                " WHERE: \n"
                "   -run,   runs the program \n"
                "   -run -t,     truncate (overwrite the existing database) \n"
                "   -run -a,     append (add more data) \n"
                "                if database name is not present a new database is created \n\n"
                "   -back,  backup the database in JSON file with the same name as the database \n"
                "   -rest,  restore a database from a JSON file with the same name as the JSON file\n\n"
                " EXAMPLE; \n"
                "   -run -t ./CASH/cashtest experiments ./CASH/CASHtest.csv \n"
                "   -back ./CASH/cashtest experiments \n"
                "   -rest ./CASH/cashtest.json experiments"<< endl;
        return 0;
    }

    if (irun != 0)                                                              //A.4 Checking correct arguments for run mode
    {
        if ((argc <= irun + 4) || (argc > irun + 6))
        {
            cout << "Wrong options, Wrong argument for option -run \n"
                    "Example: -run -t CASH/ cashtest experiments CASHtest.csv \n";
            exit(1);
        }
    }

    if (iback != 0)                                                             //A.5 Checking correct arguments for backup mode
    {
        if ((argc <= iback + 2) || (argc > iback + 3) /*|| (strcmp(argv[iback + 1], "-a")) || (strcmp(argv[iback + 1], "-t"))*/)
        {
            cout << "Wrong options, Wrong argument for option -back \n"
                    "Example: -back CASH/ cashtest experiments \n";
            exit(1);
        }
    }

    if (irest != 0)                                                             //A.6 Checking correct arguments for restore mode
    {
        if ((argc <= irest + 2) || (argc > irest + 3) /*|| (strcmp(argv[irest + 1], "-a")) || (strcmp(argv[irest + 1], "-t"))*/)
        {
            cout << "Wrong options, Wrong argument for option -rest \n"
                    "Example: -rest CASH/ cashtest.json experiments \n";
            exit(1);
        }
    }

    // create directory for db if not existent
//    if (0 != access(argv[irun + 2], F_OK)) {
//      if (ENOENT == errno) {
//         // does not exist
//          mkdir(argv[irun + 2], S_IRWXU|S_IRGRP|S_IXGRP);
//      }
//      if (ENOTDIR == errno) {
//         // not a directory
//      }
//    }

    char ejdb_path[64] = {};
    cout << ejdb_path << endl;

    strcat(ejdb_path, argv[irun + 2]);                                          //B gets the path and name of the database (where it is/will be) / or path and name of the json file to be restored
//    strcat(ejdb_path, argv[irun + 3]);
    string ejdb_path2 = ejdb_path;
    cout << ejdb_path << endl;

    char csv_path[64] = {};

    if (irest != 0 )                                                            //1.0 RESTORING
    {
        int pos = 0;
        string point = "_";
                                                                                //1.1 checking if input file is json terminated
        pos = ejdb_path2.find(".json", pos);
        if (pos < 0)
        {
            cout << "File not .json terminated... exiting!";
            exit(1);
        }

        pos = 0;
        pos = ejdb_path2.find(point, pos);                                      //1.2 gets position of the character "_"
        ejdb_path2.erase(pos, ejdb_path2.size());                               //1.3 ereasing the collation name, what follows after the "_" e.g. _<name_of_colection>.json
        if (!ejdbopen(jb, ejdb_path2.c_str(), JBOWRITER | JBOCREAT /*| JBOTRUNC*/)) {
            return 1;                                                           //1.4 creates the database form the name of the json file which comes before "_"
        }

        //Get or create collection 'experiments'
        EJCOLL *coll = ejdbcreatecoll(jb, argv[irest + 2], NULL);               //1.5 gets the name of the colection from comand line and adds it in the database
        coll_name = argv[irest + 2];

        bson_oid_t oid;
        string line;
        ifstream in(ejdb_path);
        if (in.fail())  { cout << "File not found" <<endl; return 0; }

        while(getline(in, line)  && in.good() )
        {
            jsontoejdb(line, jb, coll, oid);                                    //1.6 Line by line the json file is read and data aded into the database
        }
        in.close();
    } else
    if ((irun != 0))                                                            //2.0 ADDING DATA (MAIN FUNCTIONALITY)
    {
//        strcat(csv_path, argv[irun + 2]);
        strcat(csv_path, argv[irun + 4]);                                       //2.1 gets the path and name of the csv file
        if (!strcmp(argv[irun + 1], "-t"))
        {
            if (!ejdbopen(jb, ejdb_path, JBOWRITER | JBOCREAT | JBOTRUNC)) {    //2.2 if -t parameter is in the command line the database if exists is truncated (made fresh, everything previously present is deleted!!)
                return 1;
            }
        } else
        if (!strcmp(argv[irun + 1], "-a"))
        {
            if (!ejdbopen(jb, ejdb_path, JBOWRITER | JBOCREAT )) {              //2.3 if -a parameter is in the command line the database if exists is appended (new data is added, old data is kept)
                return 1;
            }
        } else { cout << "Wrong options, Wrong argument for option -run";
            exit(1);}

        //Get or create collection
        EJCOLL *coll = ejdbcreatecoll(jb, argv[irun + 3], NULL);                //2.4 geting the collection name from comand line and adding it to the database
        coll_name = argv[irun + 3];

        ifstream in(csv_path);
        if (in.fail())  { cout << "File not found" <<endl; return 0; }

        csvtoejdb( csv_path, jb, coll );                                        //2.5 adding data form the csv file into the database FUNCTION csvtoejdb
    } else
    if (iback != 0 )                                                            //3.0 BACKUP
    {
        if (!ejdbopen(jb, ejdb_path, JBOREADER)) {                              //3.1 opening the database form the ejdb_path for reading
            return 1; }
    }

    if (irun !=0)
    {
        coll = ejdbcreatecoll(jb, argv[irun + 3], NULL);
    } else {coll = ejdbcreatecoll(jb, argv[irun + 3], NULL);}


    //Now select records                                                        //C.1 Query and screen printing of the data in the database
    bson bq2;
    bson_init_as_query(&bq2);

    bson_append_start_object(&bq2, sT);
    bson_append_start_array(&bq2, "$bt");
    bson_append_string(&bq2, "0", "0");
    bson_append_string(&bq2, "1", "2000");
    bson_append_finish_array(&bq2);
    bson_append_finish_object(&bq2);

//    bson_append_start_array(&bq2, "$and");
//    bson_append_start_object(&bq2, "0");
//    bson_append_string(&bq2, expdataset, "test");
//    bson_append_finish_object(&bq2);

//    bson_append_start_object(&bq2, "0");
//    bson_append_string(&bq2, expdataset, "test");
//    bson_append_finish_object(&bq2);
//    bson_append_finish_array(&bq2);


    bson_finish(&bq2);
    EJQ *q2 = ejdbcreatequery(jb, &bq2, NULL, 0, NULL);

    uint32_t count;
    TCLIST *res2 = ejdbqryexecute(coll, q2, &count, 0, NULL);
    fprintf(stderr, "\n\nRecords found: %d\n", count);

    // Clearing the backup JSON file
    if (irest == 0)                                                             //C.2 if not restoring the new backup file .json is written and emptied
    {
        // name of the file
        string path = ejdb_path2 + "_" + argv[irun + 3] + ".json";
        ofstream fout_json;
        fout_json.open(path.c_str(), ios::trunc);
        if( fout_json.fail() )
        { cout<<"Output fileopen error"<<endl; exit(1); }
        fout_json.close();
    }

    //Now print the result set records
     for (int i = 0; i < TCLISTNUM(res2); ++i) {                                //C.3 Writing data on screen
         void *bsdata = TCLISTVALPTR(res2, i);
         char *bsdata_ = static_cast<char*>(bsdata);
         bson_print_raw(/*stderr,*/ bsdata_, 0);

         if (irest == 0)                                                        //3.0.1 BACKUP (if not restoring), either in run (-run) or backup (-back) mode
         {   // writing data into the JSON text file
             ejdbtojson( bsdata_, ejdb_path2 + "_" + argv[irun + 3] + ".json");
         }


         // filing in the TP[]                                                  //D.1 getting the T and P of the experiments which will be later used to select the distinct P and T pairs
         bson_iterator it;
         const char *key;
         string key_;

         bson_iterator_from_buffer(&it, bsdata_);

         while (bson_iterator_next(&it))
         {
             bson_type t = bson_iterator_type(&it);
             if (t == 0)
                 break;
             key = bson_iterator_key(&it);
             key_ = key;

             if (key_ == sT)
             {
                 // adding temperature
                 TP[0].push_back(bson_iterator_int(&it));
             } else
             if (key_ == sP)
             {
                 // adding pressure
                 TP[1].push_back(bson_iterator_int(&it));
             }
         }
     }
     fprintf(stderr, "\n");

    tclistdel(res2);

    //Dispose query
    ejdbquerydel(q2);
    bson_destroy(&bq2);




    // get distinct TP                                                          //D.2 getting the distinct T and P pairs
    for (int i=0; i<TP[0].size(); i++)
    {
        // check if TP pair is presnt more than once in the TP vector
        for (int j=0; j<TP[0].size(); j++)
        {
            if ((TP[0][i] == TP[0][j]) && (TP[1][i] == TP[1][j]) && (i != j))
            {
                isfound = true;
            }
        }
        // check if TP pair was added to the unique TP pairs container
        for (int j=0; j<TP_pairs[0].size(); ++j)
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

    ofstream fout;
//    string path2 = argv[irun + 2];
    string path2= ejdb_path2 + "_" + coll_name +"_distinctTP.csv";          //D.3 getting the name of the file and writing the distinct P and T pairs in it
    fout.open(path2.c_str(), ios::trunc);
    if( fout.fail() )
    { cout<<"Output fileopen error"<<endl; exit(1); }

    for (int i=0; i<TP_pairs[1].size(); ++i)
    {
        fout <<TP_pairs[1][i]<<";"<<TP_pairs[0][i]<<endl;
    }
    fout << TP_pairs[1].size() <<endl;
    fout.close();

    cout << endl;

    //Close database
    ejdbclose(jb);
    ejdbdel(jb);

    return 0;
}

