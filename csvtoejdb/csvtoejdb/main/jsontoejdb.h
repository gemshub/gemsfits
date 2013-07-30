#ifndef JSONTOEJDB_H
#define JSONTOEJDB_H

#include <jansson.h>
#include <string>
#include "ejdb.h"


using namespace std;

void jsontoejdb (string data_, EJDB *jb, EJCOLL *coll, bson_oid_t oid);


#endif // JSONTOEJDB_H
