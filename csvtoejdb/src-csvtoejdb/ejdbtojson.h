#ifndef EJDBTOJSON_H
#define EJDBTOJSON_H

# include <fstream>
# include <string>
# include <ejdb.h>


/**
* Reads data form EJDB database and writes it in a file in JSON format
* @author DM
* @param data       BSON data coming form EJDB read query
* @param path       path to the exported JSON format file
* @date 19.04.2013
*/
void ejdbtojson(const char *data, std::string path);

/**
* Recursive function that parses a BSON object into string (in JSON style)
* @author DM
* @param data       BSON data coming form EJDB
* @return string    string in JSON format
* @date 19.04.2013
*/
std::string foutjson(const char *data);



#endif // EJDBTOJSON_H
