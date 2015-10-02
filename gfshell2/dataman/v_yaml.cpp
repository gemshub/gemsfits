//-------------------------------------------------------------------
// $Id: v_yaml.h  $
//
// Implementation of ParserYAML class
//
// Copyright (C) 2015  S.V.Dmytriyeva
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

#include <iomanip>
#include "verror.h"
#include "v_json.h"
#include "v_yaml.h"
using namespace YAML;

void addScalar(const char* key, const string& value, bson* brec )
{
    int ival = 0;
    double dval=0.;

    if( value == "~" )
       bson_append_null(brec, key  );
    else
     if( value == "null" )
         bson_append_null(brec, key );
     else
      if( value == "true" )
        bson_append_bool(brec, key, true );
      else
       if( value == "false" )
            bson_append_bool(brec, key, false );
       else
        if( is<int>( ival, value.c_str()) )
           bson_append_int( brec, key, ival );
        else
         if( is<double>( dval, value.c_str()))
              bson_append_double( brec, key, dval );
         else
            bson_append_string( brec, key, value.c_str() );
}

string Json2YAML( const string& jsonData )
{
  string YamlData;
  bson bobj;

  //json to bson
  bson_init( &bobj );
  ParserJson pars;
  pars.setJsonText( jsonData.substr( jsonData.find_first_of('{')+1 ) );
  pars.parseObject( &bobj );
  bson_finish( &bobj );

  // bson  to YAML string
  ParserYAML::printBsonObjectToYAML( YamlData, bobj.data );
  bson_destroy(&bobj);

  return YamlData;
}

/// Parse one YAML object from string to bson structure
string parseYAMLToJson( const string& currentYAML )
{
  stringstream jsonstriam;

 try{
     stringstream stream(currentYAML);
     YAML::Parser parser(stream);
     JsonHandler builder(jsonstriam);
     parser.HandleNextDocument(builder);
     //cout << builder.to_string() << endl;
   }
   catch(YAML::Exception& e) {
      cout << "parseYAMLToBson " << e.what() << endl;
      Error( "parseYAMLToBson",  e.what() );
   }
  return jsonstriam.str();
 }

namespace ParserYAML{

void printBsonObjectToYAML(fstream& fout, const char *b)
{
    Emitter out;
    out << BeginMap;
    bson_emitter( out, b, BSON_OBJECT);
    out << EndMap;
    fout << out.c_str();
}

void printBsonObjectToYAML(string& fout, const char *b)
{
    Emitter out;
    out << BeginMap;
    bson_emitter( out, b, BSON_OBJECT);
    out << EndMap;
    fout = string( out.c_str());
 //   cout << fout;
}

/// Read one YAML object from text file and parse to bson structure
void parseYAMLToBson( fstream& fin, bson *brec )
{
    Node doc;

    try{
        doc = Load( fin );
        // test output
        //Emitter out;
        //out << doc;
        //cout << out.c_str() << endl;
        parseObject( doc, brec );
    }
    catch(YAML::Exception& e) {
      cout << "parseYAMLToBson " << e.what() << endl;
        Error( "parseYAMLToBson",  e.what() );
    }
}

/// Parse one YAML object from string to bson structure
void parseYAMLToBson( const string& currentYAML, bson *obj )
{
 try{
     stringstream stream(currentYAML);
     YAML::Parser parser(stream);
     BsonHandler builder(obj);
     parser.HandleNextDocument(builder);
     //cout << builder.to_string() << endl;
   }
   catch(YAML::Exception& e) {
      cout << "parseYAMLToBson " << e.what() << endl;
      Error( "parseYAMLToBson",  e.what() );
   }
 }

void bson_emitter( Emitter& out, const char *data, int datatype )
{
    bson_iterator i;
    const char *key;
    bson_timestamp_t ts;

    bson_iterator_from_buffer(&i, data);
    while (bson_iterator_next(&i))
    {
        bson_type t = bson_iterator_type(&i);
        if (t == 0)
          break;
        if( t == BSON_OID )
          continue;
        key = bson_iterator_key(&i);

        // before print
       switch( datatype )
       {
        case BSON_OBJECT:
           out << Key << key;
           out << Value;
           break;
        default:
           break;
       }

        switch (t)
        {
          // impotant datatypes
          case BSON_NULL:
              out << _Null();
              break;
          case BSON_BOOL:
               out <<  (bson_iterator_bool(&i) ? "true" : "false");
               break;
          case BSON_INT:
               out << bson_iterator_int(&i);
               break;
          case BSON_LONG:
               out << bson_iterator_long(&i);
               break;
          case BSON_DOUBLE:
               {
                 char vbuf[50];
                 sprintf(vbuf, "%.*lg" , 15, bson_iterator_double(&i) );
                 out << vbuf;
                 break;
               }
          case BSON_STRING:
               out << bson_iterator_string(&i);
                break;

          // main constructions
          case BSON_OBJECT:
             out << BeginMap;
             bson_emitter( out, bson_iterator_value(&i), BSON_OBJECT);
             out << EndMap;
             break;
          case BSON_ARRAY:
              out << BeginSeq;
              bson_emitter(out, bson_iterator_value(&i), BSON_ARRAY );
              out << EndSeq;
             break;

           // not used in GEMS data types
              case BSON_SYMBOL:
                     out <<  bson_iterator_string(&i);
                     break;
              case BSON_OID:
              //       bson_oid_to_string(bson_iterator_oid(&i), oidhex);
              //       os << oidhex;
                     break;
              case BSON_DATE:
                     out << bson_iterator_time_t(&i);
                     break;
              case BSON_BINDATA:
              //       out << "BSON_BINDATA";
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
                     out  << "can't print type : " << t;
        }
    }
}

void parseObject( const Node& doc, bson* brec )
{
    string key;
    int type;

    for (const_iterator it = doc.begin(); it != doc.end(); ++it)
    {
      key = it->first.as<string>();
      type = it->second.Type();

      switch( type )
      {
        case NodeType::Null:
          bson_append_null(brec, key.c_str() );
          break;
        case NodeType::Scalar:
          parseScalar( key.c_str(), it->second, brec );
          break;
        case NodeType::Sequence:
          bson_append_start_array( brec, key.c_str() );
          parseArray( it->second, brec );
          bson_append_finish_array(brec);
          break;
        case NodeType::Map:
          bson_append_start_object( brec, key.c_str() );
          parseObject( it->second, brec );
          bson_append_finish_object(brec);
          break;
        case NodeType::Undefined:
          Error( "ParserYAML02: ", "Undefined value type.");
      }
   }
}

void parseArray( const Node& doc, bson* brec )
{
    int ii = 0;
    string key;
    int type;

    for (const_iterator it = doc.begin(); it != doc.end(); ++it, ++ii)
    {
        key = to_string(ii);
        type = it->Type();

        switch( type )
        {
          case NodeType::Null:
            bson_append_null(brec, key.c_str() );
            break;
          case NodeType::Scalar:
            parseScalar( key.c_str(), *it, brec );
            break;
          case NodeType::Sequence:
            bson_append_start_array( brec, key.c_str() );
            parseArray( *it, brec );
            bson_append_finish_array(brec);
            break;
          case NodeType::Map:
            bson_append_start_object( brec, key.c_str() );
            parseObject( *it, brec );
            bson_append_finish_object(brec);
            break;
          case NodeType::Undefined:
            Error( "ParserYAML02: ", "Undefined value type.");
        }
    }
}

void parseScalar(const char* key, const Node& doc, bson* brec )
{
    string value = doc.as<std::string>();
    addScalar( key, value, brec );
}

};

// BsonHandler------------------------------------------------

string BsonHandler::to_string()
{
  string yamlstr;
  ParserYAML::printBsonObjectToYAML( yamlstr, m_bson->data);
  return yamlstr;
}

//struct Mark;

BsonHandler::BsonHandler(bson* bobj) : m_bson(bobj) {}

void BsonHandler::OnDocumentStart(const Mark&) {}

void BsonHandler::OnDocumentEnd() {}

void BsonHandler::OnNull(const Mark&, anchor_t anchor)
{
  assert(m_stateStack.top().state == WaitingForValue );
  bson_append_null( m_bson, m_stateStack.top().key.c_str() );
  BeginNode();
}

void BsonHandler::OnAlias(const Mark&, anchor_t anchor) {}

void BsonHandler::OnScalar(const Mark&, const std::string& tag,
                              anchor_t anchor, const std::string& value)
{
   switch (m_stateStack.top().state)
   {
     case WaitingForSequenceEntry:
        { string key = std::to_string(m_stateStack.top().ndx++);
          addScalar( key.c_str(), value, m_bson );
          break;
        }
     case WaitingForKey:
        m_stateStack.top().key = value;
        m_stateStack.top().state = WaitingForValue;
        break;
     case WaitingForValue:
        string key =  m_stateStack.top().key;
        addScalar( key.c_str(), value, m_bson );
        m_stateStack.top().key = "";
        m_stateStack.top().state = WaitingForKey;
        break;
    }
}

void BsonHandler::OnSequenceStart(const Mark&, const std::string& tag,
            anchor_t anchor, EmitterStyle::value style)
{
  if (!m_stateStack.empty())
  {
     string key;
     if( m_stateStack.top().state == WaitingForSequenceEntry )
        key = std::to_string(m_stateStack.top().ndx++);
     else
        key =  m_stateStack.top().key;
     bson_append_start_array( m_bson, key.c_str() );
  }
  BeginNode();
  m_stateStack.push(State(WaitingForSequenceEntry));
}

void BsonHandler::OnSequenceEnd()
{
  assert(m_stateStack.top().state == WaitingForSequenceEntry );
  m_stateStack.pop();
  if (!m_stateStack.empty())
   bson_append_finish_array(m_bson);
}

void BsonHandler::OnMapStart(const Mark&, const std::string& tag,
                                anchor_t anchor, EmitterStyle::value style)
{
  if (!m_stateStack.empty() )
  {
    string key;
    if( m_stateStack.top().state == WaitingForSequenceEntry )
      key = std::to_string(m_stateStack.top().ndx++);
    else
      key =  m_stateStack.top().key;
    bson_append_start_object( m_bson, key.c_str() );
  }
  BeginNode();
  m_stateStack.push(State(WaitingForKey));
}

void BsonHandler::OnMapEnd() {
 m_stateStack.pop();
 if (!m_stateStack.empty() )
  bson_append_finish_object(m_bson);
  //assert(m_stateStack.top().state == WaitingForKey);
}

void BsonHandler::BeginNode()
{
  if (m_stateStack.empty())
    return;

  switch (m_stateStack.top().state) {
    case WaitingForKey:
      m_stateStack.top().state = WaitingForValue;
      break;
    case WaitingForValue:
      m_stateStack.top().state = WaitingForKey;
      break;
    default:
      break;
  }
}

// JsonHandler------------------------------------------------

string JsonHandler::to_string()
{
  return m_os.str();
}

JsonHandler::JsonHandler(stringstream& os_) : m_os(os_) {}

void JsonHandler::OnDocumentStart(const Mark&) {}

void JsonHandler::OnDocumentEnd() {}

void JsonHandler::OnNull(const Mark&, anchor_t anchor)
{
  assert(m_stateStack.top().state == WaitingForValue );
  addScalar(m_stateStack.top().key.c_str(), "null" );
  BeginNode();
}

void JsonHandler::OnAlias(const Mark&, anchor_t anchor) {}

void JsonHandler::OnScalar(const Mark&, const std::string& tag,
                              anchor_t anchor, const std::string& value)
{
   switch (m_stateStack.top().state)
   {
     case WaitingForSequenceEntry:
        { m_stateStack.top().ndx++;
          addScalar( "", value );
          break;
        }
     case WaitingForKey:
        m_stateStack.top().key = value;
        m_stateStack.top().state = WaitingForValue;
        break;
     case WaitingForValue:
        string key =  m_stateStack.top().key;
        addScalar( key.c_str(), value );
        m_stateStack.top().key = "";
        m_stateStack.top().state = WaitingForKey;
        break;
    }
}

void JsonHandler::OnSequenceStart(const Mark&, const std::string& tag,
            anchor_t anchor, EmitterStyle::value style)
{
  string key="";
  if (!m_stateStack.empty())
  {
     if( m_stateStack.top().state == WaitingForSequenceEntry )
        m_stateStack.top().ndx++;
     else
        key =  m_stateStack.top().key;
  }
  addHead( key );
  m_os << "[\n";
  m_depth++;
  m_first = true;
  BeginNode();
  m_stateStack.push(State(WaitingForSequenceEntry));
}

void JsonHandler::OnSequenceEnd()
{
  assert(m_stateStack.top().state == WaitingForSequenceEntry );
  m_stateStack.pop();
  m_depth--;
  m_os << "\n";
  shift();
  m_os << "]";
}

void JsonHandler::OnMapStart(const Mark&, const std::string& tag,
                                anchor_t anchor, EmitterStyle::value style)
{
  string key="";
  if (!m_stateStack.empty() )
  {
    if( m_stateStack.top().state == WaitingForSequenceEntry )
      m_stateStack.top().ndx++;
    else
      key =  m_stateStack.top().key;
  }
  addHead( key );
  m_os << "{\n";
  m_depth++;
  m_first = true;
  BeginNode();
  m_stateStack.push(State(WaitingForKey));
}

void JsonHandler::OnMapEnd() {
 m_stateStack.pop();
 m_depth--;
 m_os << "\n";
 shift();
 m_os << "}";
}

void JsonHandler::BeginNode()
{
  if (m_stateStack.empty())
    return;

  switch (m_stateStack.top().state) {
    case WaitingForKey:
      m_stateStack.top().state = WaitingForValue;
      break;
    case WaitingForValue:
      m_stateStack.top().state = WaitingForKey;
      break;
    default:
      break;
  }
}

void JsonHandler::addHead(const string& key )
{
    if(!m_first )
     m_os <<  ",\n";
    else
     m_first = false;

    shift();

    if( !key.empty())
      m_os << "\"" << key << "\" :   ";
}

void JsonHandler::addScalar(const string&  key, const string& value )
{
    int ival = 0;
    double dval=0.;

    addHead( key );

    if( value == "null" || value == "true" ||  value == "false" )
           m_os << value;
       else
        if( is<int>( ival, value.c_str()) || is<double>( dval, value.c_str()))
              m_os << value.c_str();
         else
            m_os << "\"" << value.c_str() << "\"";
}
