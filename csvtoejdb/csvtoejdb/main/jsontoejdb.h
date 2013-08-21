#ifndef JSONTOEJDB_H
#define JSONTOEJDB_H

#include <jansson.h>
#include <string>
#include "ejdb.h"


using namespace std;

/**
* Reads in row by row a file containing JSON format entries of experimental data
* exported with ejdbtojson and adds them into the EJDB database
* @author DM
* @param data_      one JSON object as string read form the JSON file
* @param jb         pointer to EJDB database object
* @param coll       pointer to EJDB collection object
* @param oid
* @date 19.04.2013
*/
void jsontoejdb (string data_, EJDB *jb, EJCOLL *coll, bson_oid_t oid);


#endif // JSONTOEJDB_H
