#include "ejdbtojson.h"
#include "ejdb.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;


void ejdbtojson(const char *data, string path)
{
    string out;
    ofstream fout;
    fout.open(path.c_str(), ios::app);
    if( fout.fail() )
    { cout<<"Output fileopen error"<<endl; exit(1); }

    out = "{"; // writes first { for the JSON object
    fout.close();
    out += foutjson(data); // writes the rest of the JSON object

    fout.open(path.c_str(), ios::app);
    if( fout.fail() )
    { cout<<"Output fileopen error"<<endl; exit(1); }

    out += "}"; // writes last } for the JSON object

    int p = 1;
    // Erases } which are wrongly added by the foutjson function
    while (p > 0)
    {
        p = out.find(", }", p);
        if (p>0)
        {
                out.erase(p, 1);
        }
    }
    p=1;

     // Erases ] which are wrongly added by the foutjson function
    while (p > 0)
    {
        p = out.find(", ]", p);
        if (p>0)
        {
                out.erase(p, 1);
        }
    }
    fout << out.c_str() << endl; // writes the JSON object to the file
    fout.close();

}

string foutjson(const char *data)
{
    bson_iterator i, j;
    const char *key;
    stringstream ss;
    string key_, out, sss;

    bson_iterator_from_buffer(&i, data);
    while (bson_iterator_next(&i))
    {
        bson_type t = bson_iterator_type(&i);
        if (t == 0)
            break;

        key = bson_iterator_key(&i);
        key_ = key;

        if ((t == BSON_DOUBLE) || (t == BSON_OID) || (t == BSON_INT))
        {
            out = out + " \"" + key + "\"" + " : ";
            switch (t)
            {
                case BSON_DOUBLE:
                    ss << bson_iterator_double(&i);
                    sss = ss.str();
                    ss.str("");
                    out = out + sss.c_str() + ", ";
                    break;
                case BSON_OID:
                    ss << bson_iterator_oid(&i);
                    sss = ss.str();
                    ss.str("");
                    out = out + "\"" + sss.c_str() + "\"" + ", ";
                    break;
                case BSON_INT:
                    ss << bson_iterator_int(&i);
                    sss = ss.str();
                    ss.str("");
                    out = out + sss.c_str() + ", ";
                    break;
            }
        } else
        if ( t == BSON_STRING)
        {
            out = out + " \"" + key + "\"" + " : " + "\"" + bson_iterator_string(&i) + "\", ";
        } else
        if ( t == BSON_ARRAY)
        {
            // recursive call of the function until all elements of the array are read
            out = out + " \"" + key + "\"" + " : [" + foutjson(bson_iterator_value(&i));;
            out += "], ";
        } else
        if (t == BSON_OBJECT)
        {
            // recursive call of the function until all objects are read
            out = out + "{" + foutjson(bson_iterator_value(&i));
            out += "}, ";
        }
    }
    return out;
}
