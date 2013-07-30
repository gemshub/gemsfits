#include "ejdbtojson.h"
#include "ejdb.h"
#include <string>
#include <iostream>
#include <fstream>

using namespace std;


void ejdbtojson(const char *data, int pos, string path)
{
    string out;

    ofstream fout;
    fout.open(path.c_str(), ios::app);
    if( fout.fail() )
    { cout<<"Output fileopen error"<<endl; exit(1); }

    out = "{";
    fout.close();
    out += foutjson(data);

    fout.open(path.c_str(), ios::app);
    if( fout.fail() )
    { cout<<"Output fileopen error"<<endl; exit(1); }

    out += "}";

    int p = 1;

    while (p > 0)
    {
        p = out.find(", }", p);
        if (p>0)
        {
                out.erase(p, 1);
        }
    }
    p=1;

    while (p > 0)
    {
        p = out.find(", ]", p);
        if (p>0)
        {
                out.erase(p, 1);
        }
    }

    fout << out.c_str() << endl;

    fout.close();

}

string foutjson(const char *data)
{
    bson_iterator i, j;
    const char *key;
    string key_, out;

    bson_iterator_from_buffer(&i, data);
    while (bson_iterator_next(&i))
    {
        bson_type t = bson_iterator_type(&i);
        if (t == 0)
            break;

        cout << t << endl;

        key = bson_iterator_key(&i);
        key_ = key;
        cout << key_ << endl;

        if ((t == BSON_DOUBLE) || (t == BSON_OID) || (t == BSON_INT))
        {
            out = out + " \"" + key + "\"" + " : ";
            switch (t)
            {
                case BSON_DOUBLE:
                    out = out + boost::lexical_cast<string>(bson_iterator_double(&i)).c_str() + ", ";
                    break;
                case BSON_OID:
                    out = out + boost::lexical_cast<string>(bson_iterator_oid(&i)).c_str() + ", ";
                    break;
                case BSON_INT:
                    out = out + boost::lexical_cast<string>(bson_iterator_int(&i)).c_str() + ", ";
                    break;
            }
        } else
        if ( t == BSON_STRING)
        {
            out = out + " \"" + key + "\"" + " : " + "\"" + bson_iterator_string(&i) + "\", ";
        } else
        if ( t == BSON_ARRAY)
        {
            out = out + " \"" + key + "\"" + " : [" + foutjson(bson_iterator_value(&i));;
            out += "], ";
        } else
        if (t == BSON_OBJECT)
        {
            out = out + "{" + foutjson(bson_iterator_value(&i));
            out += "}, ";
        }
    }
    return out;
}
