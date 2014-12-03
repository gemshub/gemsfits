//-------------------------------------------------------------------
// $Id: v_json.h 333 2014-03-13 13:23:32Z gemsfits $
//
// Declaration of ParserJson class and other bson functions
//
// Copyright (C) 2014  S.V.Dmytriyeva
// Uses Qwt (http://qwt.sourceforge.net), EJDB (http://ejdb.org),
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

#ifndef __v_json_h_
#define __v_json_h_

#include <fstream>
#include <string>
using namespace std;
#ifdef buildWIN32
#include <tcejdb/ejdb.h>
#else
#include "ejdb.h"
#endif

extern int generateTxtfromBson( string gemsfitfile, bson *obj, bool with_comments );
extern void csvToBson( bson *exp, const  vector<string>& headline, const vector<string>& row );

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
    jsQuote = '\"'      //0x22
};

/// Class for read/write bson structure from/to text file or string
class ParserJson
{
protected:

  string jsontext;
  const char *curjson;
  const char *end;

  bool xblanc();
  void getString( string& str );
  void getNumber( double& value, int& type );
  void parseValue( const char *name, bson *brec );
  void bson_print_raw_txt( iostream& os, const char *data, int depth, int datatype );


 public:

  ParserJson():curjson(0), end(0)
    {}
  virtual ~ParserJson()
    {}

  /// Parse internal jsontext string to bson structure (without first {)
  void parseObject( bson *brec );
  void parseArray( bson *brec );

  /// Print bson structure to JSON string
  void printBsonObjectToJson( string& resStr, const char *b);

  /// Read one json object from text file
  string readObjectText( fstream& fin );
  /// Set up internal jsontext before parsing
  void  setJsonText( const string& json );

};

/// Get string from bson structure by name
bool bson_find_string( const char *obj, const char *name, string& str );
/// Get array from bson structure by name
const char* bson_find_array( const char *obj, const char *name );
/// Get object from bson structure by name
const char* bson_find_object( const char *obj, const char *name );
/// Get time from bson structure by name
time_t bson_find_time_t( const char *obj, const char *name );

/// Get value from bson structure by name
template <class T>
bool bson_find_value( const char *obj, const char *name, T& value )
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
         return false;
   }
   return true;
}

#endif	// __v_json_h_
