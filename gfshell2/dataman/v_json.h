//-------------------------------------------------------------------
// $Id: v_json.h 333 2014-03-13 13:23:32Z gemsfits $
//
// Declaration of ParserJson class and other bson functions
//
// Copyright (C) 2014  S.V.Dmytriyeva
// Uses EJDB (https://ejdb.org),
//    yaml-cpp (https://code.google.com/p/yaml-cpp/)
//
// This file is part of the GEMSFITS GUI, which uses the
// Qt v.5 cross-platform App & UI framework (http://qt-project.org)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of LGPL v.3 license
//
// See http://gems.web.psi.ch/GEMSFIT for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#pragma once

#include <vector>
#include <string>
#include <ejdb.h>

extern const char* S_EMPTY;
extern const char* S_ANY;

extern int generateTxtfromBson(std::string gemsfitfile, bson *obj, bool with_comments);
extern void csvToBson(bson *exp, const  std::vector<std::string>& headline, const std::vector<std::string>& row);

extern void print_bson_to_json(FILE *f, const bson *b);
/// Print bson object to text file
extern void print_bson_object_to_json(FILE *f, const bson *b);


enum {
    jsBeginArray = '[',    //0x5b,
    jsBeginObject = '{',   //0x7b,
    jsEndArray = ']',      //0x5d,
    jsEndObject = '}',     //0x7d,
    jsNameSeparator = ':', //0x3a,
    jsValueSeparator = ',',//0x2c,
    jsQuote = '\"'         //0x22
};

/// Class for read/write bson structure from/to text file or std::string
class ParserJson
{
protected:

    std::string jsontext;
    const char *curjson;
    const char *end;

    bool xblanc();
    void getString(std::string& str);
    void getNumber(double& value, int& type);
    void parseValue(const char *name, bson *brec);
    void bson_print_raw_txt(std::iostream& os, const char *data, int depth, int datatype);

public:

    ParserJson():curjson(0), end(0)
    {}
    virtual ~ParserJson()
    {}

    /// Parse internal jsontext std::string to bson structure (without first {)
    void parseObject(bson *brec);
    void parseArray(bson *brec);

    /// Print bson structure to JSON std::string
    void printBsonObjectToJson(std::string& resStr, const char *b);

    /// Read one json object from text file
    std::string readObjectText(std::fstream& fin);
    /// Set up internal jsontext before parsing
    void  setJsonText(const std::string& json);

};

/// Get std::string from bson structure by name
bool bson_find_string(const char *obj, const char *name, std::string& str);
/// Get array from bson structure by name
const char* bson_find_array(const char *obj, const char *name);
/// Get object from bson structure by name
const char* bson_find_object(const char *obj, const char *name);
/// Get time from bson structure by name
time_t bson_find_time_t(const char *obj, const char *name);

/// Get value from bson structure by name
template <class T>
bool bson_find_value(const char *obj, const char *name, T& value)
{
    bson_iterator it;
    bson_type type;
    type =  bson_find_from_buffer(&it, obj, name);

    switch( type )
    {
    // impotant datatypes
    case BSON_BOOL:
        value = bson_iterator_bool(&it);
        break;
    case BSON_INT:
        value = bson_iterator_int(&it);
        break;
    case BSON_LONG:
        value =  bson_iterator_long(&it);
        break;
    case BSON_DOUBLE:
        value = bson_iterator_double(&it);
        break;
    case BSON_NULL:
        // return false;
    default: // error
        // bson_errprintf("can't print type : %d\n", type);
        return false;
    }
    return true;
}

/// Get value from bson structure by name
template <class T>
void bson_find_value_def(const char *obj, const char *name, T& value, const T& defval)
{
    bson_iterator it;
    bson_type type;
    type =  bson_find_from_buffer(&it, obj, name );

    switch( type )
    {
    // impotant datatypes
    case BSON_BOOL:
        value = bson_iterator_bool(&it);
        break;
    case BSON_INT:
        value = bson_iterator_int(&it);
        break;
    case BSON_LONG:
        value =  bson_iterator_long(&it);
        break;
    case BSON_DOUBLE:
        value = bson_iterator_double(&it);
        break;
    case BSON_NULL:
        // return false;
    default: // error
        // bson_errprintf("can't print type : %d\n", type);
        value = defval;
    }
}

