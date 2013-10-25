#ifndef CSVTOEJDB_H
#define CSVTOEJDB_H


#include <ejdb.h>
#include <vector>

using namespace std;

/**
* Reads formated experimental data from a saved CSV file to ejdb database
* See How to CSVtoEJDB.txt file fo how to format the data in the the csv file
* @author DM
* @param csv_path   path to the csv file e.g. ./FOLDER/file.csv
* @param jb         pointer to EJDB database object
* @param coll       pointer to EJDB collection object
* @date 19.04.2013
*/
void csvtoejdb(char csv_path[64], EJDB *jb, EJCOLL *coll);

#endif // CSVTOEJDB_H
