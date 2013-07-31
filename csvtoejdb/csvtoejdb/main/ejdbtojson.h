#ifndef EJDBTOJSON_H
#define EJDBTOJSON_H

# include <fstream>
# include <string>
# include <ejdb.h>
# include <boost/lexical_cast.hpp>

void ejdbtojson(const char *data, std::string path);


std::string foutjson(const char *data);



#endif // EJDBTOJSON_H
