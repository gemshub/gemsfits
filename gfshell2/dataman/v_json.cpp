//-------------------------------------------------------------------
// $Id: v_json.h 333 2014-03-13 13:23:32Z gemsfits $
//
// Implementation of ParserJson class and other bson functions
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

#include <sstream>
#include <cstring>
#include <iomanip>
#include "v_service.h"
#include "f_ejdb.h"

#define SHORT_EMPTY 	   -32768
#define SHORT_ANY   	    32767
#define USHORT_EMPTY         0
#define USHORT_ANY           65535
#define DOUBLE_EMPTY         2.2250738585072014e-308
#define DOUBLE_ANY           1.7976931348623157e+308

bool bson_find_string( const char *obj, const char *name, string& str )
{
    bson_iterator it;
    bson_type type;
    type =  bson_find_from_buffer(&it, obj, name );

    switch( type )
    {
    case BSON_NULL:
        str = S_EMPTY;
        break;
    case BSON_STRING:
        str = bson_iterator_string(&it);
         break;
    default: // error
         // bson_errprintf("can't print type : %d\n", type);
        str = S_EMPTY;
        return false;
   }
   return true;
}

const char* bson_find_array( const char *obj, const char *name )
{
    bson_iterator it;
    bson_type type;
    type =  bson_find_from_buffer(&it, obj, name );
    ErrorIf( type != BSON_ARRAY, "E005BSon: ", "Must be array.");
    return bson_iterator_value(&it);
}

const char* bson_find_object( const char *obj, const char *name )
{
    bson_iterator it;
    bson_type type;
    type =  bson_find_from_buffer(&it, obj, name );
    ErrorIf( type != BSON_OBJECT, "E006BSon: ", "Must be object.");
    return bson_iterator_value(&it);
}

time_t bson_find_time_t( const char *obj, const char *name )
{
    time_t ctm;
    bson_iterator it;
    bson_type type;
    type =  bson_find_from_buffer(&it, obj, name );
    switch( type )
    {
    case BSON_STRING:
         { string stime = bson_iterator_string(&it);
           ctm =  tcstrmktime(stime.c_str());
         }
         break;
    case BSON_DATE:
         ctm = bson_iterator_time_t(&it);
         break;
    case BSON_NULL:
    default:
            ctm = time(NULL);
            break;
   }
   return ctm;
}

void bson_print_raw_txt_(FILE *f, const char *data, int depth, int datatype );

void bson_print_raw_txt_(FILE *f, const char *data, int depth, int datatype )
{
    bson_iterator i;
    const char *key;
    int temp;
    bson_timestamp_t ts;
    char oidhex[25];
    bson scope;
    bool first = true;

    bson_iterator_from_buffer(&i, data);
    while (bson_iterator_next(&i))
    {
        if(!first )
         fprintf(f, ",\n");
        else
         first = false;

        bson_type t = bson_iterator_type(&i);
        if (t == 0)
          break;

        key = bson_iterator_key(&i);

        // before print
        switch( datatype )
        {
         case BSON_OBJECT:
           for (temp = 0; temp <= depth; temp++)
             fprintf(f, "\t");
           fprintf(f, "\"%s\": ", key );
           break;
         case BSON_ARRAY:
            for (temp = 0; temp <= depth; temp++)
              fprintf(f, "\t");
            break;
         default:
            break;
        }

        switch (t)
        {
          // impotant datatypes
          case BSON_NULL:
              fprintf(f, "null");
              break;
          case BSON_BOOL:
               fprintf(f, "%s", bson_iterator_bool(&i) ? "true" : "false");
               break;
          case BSON_INT:
               fprintf(f, "%d", bson_iterator_int(&i));
               break;
          case BSON_LONG:
               fprintf(f, "%lld", (uint64_t) bson_iterator_long(&i));
               break;
          case BSON_DOUBLE:
               fprintf(f, "%.10lg", bson_iterator_double(&i));
               break;
          case BSON_STRING:
                fprintf(f, "\"%s\"", bson_iterator_string(&i));
                break;

          // main constructions
          case BSON_OBJECT:
             fprintf(f, "{\n");
             bson_print_raw_txt_( f, bson_iterator_value(&i), depth + 1, BSON_OBJECT);
             for (temp = 0; temp <= depth; temp++)
               fprintf(f, "\t");
             fprintf(f, "}");
             break;
          case BSON_ARRAY:
               fprintf(f, "[\n");
               bson_print_raw_txt_(f, bson_iterator_value(&i), depth + 1, BSON_ARRAY );
               for (temp = 0; temp <= depth; temp++)
                 fprintf(f, "\t");
               fprintf(f, "]");
               break;

           // not used in GEMS data types
              case BSON_SYMBOL:
                     fprintf(f, "SYMBOL: %s", bson_iterator_string(&i));
                     break;
              case BSON_OID:
                     bson_oid_to_string(bson_iterator_oid(&i), oidhex);
                     fprintf(f, "%s", oidhex);
                     break;
              case BSON_DATE:
                     char buf[100];
                     tcdatestrhttp(bson_iterator_time_t(&i), INT_MAX, buf);
                     fprintf(f, "\"%s\"", buf);
            //fprintf(f, "%ld", (long int) bson_iterator_date(&i));
                     break;
              case BSON_BINDATA:
                     fprintf(f, "BSON_BINDATA");
                     break;
              case BSON_UNDEFINED:
                     fprintf(f, "BSON_UNDEFINED");
                     break;
              case BSON_REGEX:
                     fprintf(f, "BSON_REGEX: %s", bson_iterator_regex(&i));
                     break;
              case BSON_CODE:
                     fprintf(f, "BSON_CODE: %s", bson_iterator_code(&i));
                     break;
              case BSON_CODEWSCOPE:
                     fprintf(f, "BSON_CODE_W_SCOPE: %s", bson_iterator_code(&i));
                     /* bson_init( &scope ); */ /* review - stepped on by bson_iterator_code_scope? */
                     bson_iterator_code_scope(&i, &scope);
                     fprintf(f, "\n\t SCOPE: ");
                     bson_print_raw_txt_(f, (const char*) &scope, 0, BSON_CODEWSCOPE);
                     //bson_print(f, &scope);
                     /* bson_destroy( &scope ); */ /* review - causes free error */
                     break;
               case BSON_TIMESTAMP:
                     ts = bson_iterator_timestamp(&i);
                     fprintf(f, "i: %d, t: %d", ts.i, ts.t);
                     break;
               default:
                     fprintf(f, "can't print type : %d\n", t);
        }
    }
    fprintf(f, "\n");

}

void print_bson_to_json(FILE *f, const bson *b)
{
    bson_print_raw_txt_(f, b->data, 0, BSON_OBJECT);
}

void print_bson_object_to_json(FILE *f, const bson *b)
{
    fprintf(f, "{\n");
    bson_print_raw_txt_(f, b->data, 0, BSON_OBJECT);
    fprintf(f, "}");
}


// Read one Json Object from text file to string
string  ParserJson::readObjectText( fstream& ff )
{
  jsontext = "";
  //jsontext += jsBeginObject;
  int cntBeginObject = 1;
  char input;

  do{
       ff.get( input );
       if( input ==  jsBeginObject )
          cntBeginObject++;
       else if( input ==  jsEndObject )
             cntBeginObject--;
       jsontext+= input;

     } while( cntBeginObject > 0 && !ff.eof());

  curjson = jsontext.c_str();
  end = curjson + jsontext.length();
  return jsontext;
}

// Load Json text
void  ParserJson::setJsonText( const string& json )
{
  jsontext = json;
  curjson = jsontext.c_str();
  end = curjson + jsontext.length();
}

bool ParserJson::xblanc()
{
    curjson += strspn( curjson, " \r\n\t" );
    return (curjson < end);
}

// Get "<string>" data
void ParserJson::getString( string& str )
{
    //curjson++;
    const char * posQuote = strchr( curjson, jsQuote );
    ErrorIf( !posQuote ,"E01JSon: ",
             "Missing \" - end of string constant.");
    str = string(curjson, 0, posQuote-curjson);
    curjson = ++posQuote;
    ErrorIf( curjson >= end ,"E02JSon: ",
             "Termination by String.");
}

// Get <double/integer> data
void ParserJson::getNumber( double& value, int& type )
{
    const char *start = curjson;
    bool isInt = true;

    // minus
    if (curjson < end &&  ( *curjson == '-' || *curjson == '+' ) )
        ++curjson;

    // int = zero / ( digit1-9 *DIGIT )
    while (curjson < end && isdigit(*curjson) )
            ++curjson;

    // frac = decimal-point 1*DIGIT
    if (curjson < end && *curjson == '.')
    {
        isInt = false;
        ++curjson;
        while (curjson < end && isdigit(*curjson) )
            ++curjson;
    }

    // exp = e [ minus / plus ] 1*DIGIT
    if (curjson < end && (*curjson == 'e' || *curjson == 'E'))
    {
        isInt = false;
        ++curjson;
        if (curjson < end && (*curjson == '-' || *curjson == '+'))
            ++curjson;
        while (curjson < end && isdigit(*curjson) )
            ++curjson;
    }

    ErrorIf( curjson >= end ,"E03JSon: ",
             "Termination by Number.");
    sscanf( start, "%lg", &value );
    if( isInt && ( value < SHORT_ANY && value > SHORT_EMPTY ) )
      type = BSON_INT;
    else
      type =  BSON_DOUBLE;       \
}


// value = false / null / true / object / array / number / string
void ParserJson::parseValue( const char *name, bson *brec )
{
    int type = BSON_UNDEFINED;
    ErrorIf( !xblanc() ,"E04JSon: ", "Must be value.");

    switch (*curjson++)
    {
    case 'n':
        if( (end - curjson) > 4 && (*curjson++ == 'u' &&
            *curjson++ == 'l' && *curjson++ == 'l') )
        {
            type = BSON_NULL;
            bson_append_null(brec, name );
            break;
        }
        Error( "E05JSon: ", "Illegal Value.");
    case 't':
        if( (end - curjson) > 4 && (*curjson++ == 'r' &&
            *curjson++ == 'u' && *curjson++ == 'e') )
        {
            type = BSON_BOOL;
            bson_append_bool(brec, name, true );
            break;
        }
        Error( "E05JSon: ", "Illegal Value.");
    case 'f':
        if( (end - curjson) > 5 && (*curjson++ == 'a' &&
            *curjson++ == 'l'  && *curjson++ == 's' && *curjson++ == 'e' ) )
        {
            type = BSON_BOOL;
            bson_append_bool(brec, name, false );
            break;
        }
        Error( "E05JSon: ", "Illegal Value.");
    case jsQuote:
       {
        string str = "";
        type = BSON_STRING;
        getString( str );
        bson_append_string( brec, name, str.c_str() );
       }
       break;

    case jsBeginArray:
        type = BSON_ARRAY;
        bson_append_start_array( brec, name);
        parseArray( brec );
        bson_append_finish_array(brec);
        break;

    case jsBeginObject:
        type = BSON_OBJECT;
        bson_append_start_object( brec, name);
        parseObject( brec );
        bson_append_finish_object(brec);
        break;

    case jsEndArray:
         --curjson;
        break;  // empty array
    case jsEndObject:
         --curjson;
        break;  // empty object

    default:  // number
      { --curjson;
        if( isdigit(*curjson) || *curjson == '+' ||
            *curjson == '-' || *curjson == '.' ||
            *curjson == 'e' || *curjson == 'E'    )
        {
            double value = DOUBLE_EMPTY;
            getNumber( value, type );
            if( type == BSON_INT )
              bson_append_int( brec, name, (int)value );
            else
              bson_append_double( brec, name, value );
            break;
        }
        else
            Error( "E05JSon: ", "Illegal Value.");
      }
      break;
    }
}

//    array = [ <value1>, ... <valueN> ]
void ParserJson::parseArray( bson *brec )
{
    char name[40];
    int ndx = 0;

    while( *curjson != jsEndArray )
    {
      sprintf( name, "%d", ndx);
      parseValue( name, brec );
      ErrorIf( !xblanc() ,"E06JSon: ", "Unterminated Array.");
      if( *curjson == jsValueSeparator  )
        curjson++;
      else
        ErrorIf( *curjson != jsEndArray ,"E07JSon: ",
                 "Missing Value Separator.");
      ndx++;
    }
    curjson++;
}

//  object = { "<key1>" : <value1>, ... , "<keyN>" : <valueN> }
void ParserJson::parseObject( bson *brec )
{
    string name;

    while( *curjson != jsEndObject )
    {
      // read key
      ErrorIf( !xblanc() ,"E08JSon: ", "Unterminated Object.");

      if( *curjson++== jsQuote  )
      {
          getString( name );
       }
      else
        Error( "E10JSon: ", "Missing Key of Object.");

      ErrorIf( !xblanc() ,"E08JSon: ", "Unterminated Object.");
      if( *curjson == jsNameSeparator  )
          curjson++;
      else
        Error( "E09JSon: ", "Missing Name Separator.");
      // read value
      parseValue( name.c_str(), brec );
      ErrorIf( !xblanc() ,"E08JSon: ", "Unterminated Object.");
      if( *curjson == jsValueSeparator  )
        curjson++;
      else
        ErrorIf( *curjson != jsEndObject ,"E07JSon: ",
                 "Missing Value Separator.");
    }
    curjson++;
}

void ParserJson::bson_print_raw_txt( iostream& os, const char *data, int depth, int datatype )
{
    bson_iterator i;
    const char *key;
    int temp;
    bson_timestamp_t ts;
    char oidhex[25];
    bson scope;
    bool first = true;

    bson_iterator_from_buffer(&i, data);
    while (bson_iterator_next(&i))
    {
        bson_type t = bson_iterator_type(&i);
        if (t == 0)
          break;
        if( t == BSON_OID )
          continue;

        if(!first )
         os <<  ",\n";
        else
         first = false;

        key = bson_iterator_key(&i);

         // before print
        switch( datatype )
        {
         case BSON_OBJECT:
           for (temp = 0; temp <= depth; temp++)
             os <<  "     ";
           os << "\"" << key << "\" :   ";
           break;
         case BSON_ARRAY:
            for (temp = 0; temp <= depth; temp++)
              os << "     ";
            break;
         default:
            break;
        }

        switch (t)
        {
          // impotant datatypes
          case BSON_NULL:
               os << "null";
              break;
          case BSON_BOOL:
               os << ( bson_iterator_bool(&i) ?  "true": "false");
               break;
          case BSON_INT:
               os << bson_iterator_int(&i);
               break;
          case BSON_LONG:
               os << bson_iterator_long(&i);
               break;
          case BSON_DOUBLE:
               os << setprecision(15) << bson_iterator_double(&i);
               break;
          case BSON_STRING:
               os << "\"" << bson_iterator_string(&i) << "\"";
                break;

          // main constructions
          case BSON_OBJECT:
             os << "{\n";
             bson_print_raw_txt( os, bson_iterator_value(&i), depth + 1, BSON_OBJECT);
             for (temp = 0; temp <= depth; temp++)
               os << "     ";
             os << "}";
             break;
          case BSON_ARRAY:
              os << "[\n";
              bson_print_raw_txt(os, bson_iterator_value(&i), depth + 1, BSON_ARRAY );
               for (temp = 0; temp <= depth; temp++)
                 os << "     ";
               os << "]";
               break;

           // not used in GEMS data types
              case BSON_SYMBOL:
              //       os<<  "SYMBOL: " << bson_iterator_string(&i);
                     break;
              case BSON_OID:
              //       bson_oid_to_string(bson_iterator_oid(&i), oidhex);
              //       os << oidhex;
                     break;
              case BSON_DATE:
              //       char buf[100];
              //       tcdatestrhttp(bson_iterator_time_t(&i), INT_MAX, buf);
              //       os << "\"" << buf <<"\"";
                     break;
              case BSON_BINDATA:
              //       os << "BSON_BINDATA";
                     break;
              case BSON_UNDEFINED:
              //      os << "BSON_UNDEFINED";
                     break;
              case BSON_REGEX:
              //       os << "BSON_REGEX: " << bson_iterator_regex(&i);
                     break;
              case BSON_CODE:
              //       os << "BSON_CODE: " << bson_iterator_code(&i);
                     break;
              case BSON_CODEWSCOPE:
              //       os << "BSON_CODE_W_SCOPE: " << bson_iterator_code(&i);
              //       bson_iterator_code_scope(&i, &scope);
              //       os << "\n      SCOPE: ";
              //       bson_print_raw_txt( os, (const char*) &scope, 0, BSON_CODEWSCOPE);
                     break;
               case BSON_TIMESTAMP:
              //       ts = bson_iterator_timestamp(&i);
              //       os <<  "i: " << ts.i << ", t: " << ts.t;
                     break;
               default:
                     os  << "can't print type : " << t;
        }
    }
    os << "\n";
}

void ParserJson::printBsonObjectToJson( string& resStr, const char *b)
{
    stringstream os;
    os << "{\n";
    bson_print_raw_txt( os, b, 0, BSON_OBJECT);
    os << "}";
    resStr = os.str();
}

// ------------------------ end of v_json.cpp -------------------------------------------------------
