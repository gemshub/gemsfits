#ifndef CSVTOEJDB_H
#define CSVTOEJDB_H


# include <ejdb.h>
# include <iostream>
#include <string>
#include <vector>

using namespace std;

void csvtoejdb(char csv_path[64], EJDB *jb, EJCOLL *coll);

#endif // CSVTOEJDB_H
