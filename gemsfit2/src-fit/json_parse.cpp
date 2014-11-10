#include "json_parse.h"
#include <sstream>
#include <jansson.h>
#include <string>
#include "keywords.h"
#include <iomanip>
#include <iostream>

void parse_JSON_object(string query, const char* key, vector<string> &result)
{
    json_t *root; json_t *data; json_t *object;
    json_error_t jerror;
    std::stringstream ss;
    string sss;

    const char * JSON = query.c_str();
    root = json_loads(JSON, 0, &jerror);
    std::setprecision(15);

    if(!root)
    {
        fprintf(stderr, "error: on line %d: %s\n", jerror.line, jerror.text);
    }
    else
    {
        object = json_object_get(root, key);
        if(json_is_array(object))
        {
            for(unsigned int i = 0; i < json_array_size(object); i++)
            {
                data = json_array_get(object, i);
                    if(json_is_string(data))
                    {
                        result.push_back(json_string_value(data));
                    }
                    else if (json_is_real(data))
                    {
                        ss.precision(15);
                        ss << json_real_value(data);
                        sss = ss.str();
                        ss.str("");
                        result.push_back(sss);
                    }
                    else if (json_is_integer(data))
                    {
                        ss << json_integer_value(data);
                        sss = ss.str();
                        ss.str("");
                        result.push_back(sss);
                    }
                    else if (json_is_object(data))
                    {
                        result.push_back(json_dumps(data, 1));
                    }
             }
         }
        else
        {
            if(json_is_string(object))
            {
                result.push_back(json_string_value(object));
            }
            else if (json_is_real(object))
            {
                ss.precision(15);
                ss << json_real_value(object);
                sss = ss.str();
                ss.str("");
                result.push_back(sss);
            }
            else if (json_is_integer(object))
            {
                ss << json_integer_value(object);
                sss = ss.str();
                ss.str("");
                result.push_back(sss);
            }
            else if (json_is_object(object))
            {
                result.push_back(json_dumps(object, 1));
            }
        }
    }

}

void parse_JSON_array_object( string data_, const char *arr , const char *key, vector<string> &result )
{
    json_t *root; json_t *data1; json_t *data; json_t *object; json_t *data2;
    json_error_t jerror;
    stringstream ss;
    string sss;

    const char * JSON = data_.c_str();
    root = json_loads(JSON, 0, &jerror);

    if(!root)
    {
        fprintf(stderr, "error: on line %d: %s\n", jerror.line, jerror.text);
    }
    else
    {
        object = json_object_get(root, arr);
        if(json_is_array(object))
        {
            for(unsigned int i = 0; i < json_array_size(object); i++)
            {
                data1 = json_array_get(object, i);
                if (json_is_object(data1))
                {
                    data = json_object_get(data1, key);
                    if(json_is_string(data))
                    {
                        result.push_back(json_string_value(data));
                    }
                    else if (json_is_real(data))
                    {
                        ss.precision(15);
                        ss << json_real_value(data);
                        sss = ss.str();
                        ss.str("");
                        result.push_back(sss);
                    }
                    else if (json_is_integer(data))
                    {
                        ss << json_integer_value(data);
                        sss = ss.str();
                        ss.str("");
                        result.push_back(sss);
                    }
                    else if (json_is_array(data))
                    {
                        for(unsigned int j = 0; j < json_array_size(data); j++)
                        {
                            data2 = json_array_get(data, j);
                            if(json_is_string(data2))
                            {
                                result.push_back(json_string_value(data2));
                            }
                            else if (json_is_real(data2))
                            {
                                ss.precision(15);
                                ss << json_real_value(data2);
                                sss = ss.str();
                                ss.str("");
                                result.push_back(sss);
                            }
                            else if (json_is_integer(data2))
                            {
                                ss << json_integer_value(data2);
                                sss = ss.str();
                                ss.str("");
                                result.push_back(sss);
                            }
                        }
                        if ((arr == keys::usepair) || (arr == keys::skippair))
                        {result.push_back("");}
                    }
                }
             }
         }
        else
        {
//            data = json_object_get(data1, key);  // maybe error
            if(json_is_string(object))
            {
                result.push_back(json_string_value(object));
            }
            else if (json_is_real(object))
            {
                ss.precision(15);
                ss << json_real_value(object);
                sss = ss.str();
                ss.str("");
                result.push_back(sss);
            }
            else if (json_is_integer(object))
            {
                ss << json_integer_value(object);
                sss = ss.str();
                ss.str("");
                result.push_back(sss);
            }
        }
    }
}
