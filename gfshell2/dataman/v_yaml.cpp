//-------------------------------------------------------------------
// $Id: v_yaml.cpp  $
//
// Implementation of ParserYAML class
//
// Copyright (C) 2015-2023  S.V.Dmytriyeva
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

#include <fstream>
#include <cassert>
#include "v_json.h"
#include "v_yaml.h"
#include "v_detail.h"


void addScalar(const char* key, const std::string& value, bson* brec )
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

std::string Json2YAML( const std::string& jsonData )
{
    std::string YamlData;
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
std::string parseYAMLToJson( const std::string& currentYAML )
{
    std::stringstream jsonstriam;

    try{
        std::stringstream stream(currentYAML);
        YAML::Parser parser(stream);
        JsonHandler builder(jsonstriam);
        parser.HandleNextDocument(builder);
        //cout << builder.to_string() << endl;
    }
    catch(YAML::Exception& e) {
        std::cout << "parseYAMLToBson " << e.what() << std::endl;
        Error( "parseYAMLToBson",  e.what() );
    }
    return jsonstriam.str();
}

namespace ParserYAML{

void printBsonObjectToYAML(std::fstream& fout, const char *b)
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    bson_emitter( out, b, BSON_OBJECT);
    out << YAML::EndMap;
    fout << out.c_str();
}

void printBsonObjectToYAML(std::string& fout, const char *b)
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    bson_emitter( out, b, BSON_OBJECT);
    out << YAML::EndMap;
    fout = std::string( out.c_str());
    //   cout << fout;
}

/// Read one YAML object from text file and parse to bson structure
void parseYAMLToBson( std::fstream& fin, bson *brec )
{
    YAML::Node doc;

    try{
        doc = YAML::Load( fin );
        // test output
        //Emitter out;
        //out << doc;
        //cout << out.c_str() << endl;
        parseObject( doc, brec );
    }
    catch(YAML::Exception& e) {
        std::cout << "parseYAMLToBson " << e.what() << std::endl;
        Error( "parseYAMLToBson",  e.what() );
    }
}

/// Parse one YAML object from string to bson structure
void parseYAMLToBson( const std::string& currentYAML, bson *obj )
{
    try{
        std::stringstream stream(currentYAML);
        YAML::Parser parser(stream);
        BsonHandler builder(obj);
        parser.HandleNextDocument(builder);
        //cout << builder.to_string() << endl;
    }
    catch(YAML::Exception& e) {
        std::cout << "parseYAMLToBson " << e.what() << std::endl;
        Error( "parseYAMLToBson",  e.what() );
    }
}

void bson_emitter( YAML::Emitter& out, const char *data, int datatype )
{
    bson_iterator i;
    const char *key;
    //bson_timestamp_t ts;

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
            out << YAML::Key << key;
            out << YAML::Value;
            break;
        default:
            break;
        }

        switch (t)
        {
        // impotant datatypes
        case BSON_NULL:
            out << YAML::_Null();
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
            out << YAML::BeginMap;
            bson_emitter( out, bson_iterator_value(&i), BSON_OBJECT);
            out << YAML::EndMap;
            break;
        case BSON_ARRAY:
            out << YAML::BeginSeq;
            bson_emitter(out, bson_iterator_value(&i), BSON_ARRAY );
            out << YAML::EndSeq;
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

void parseObject( const YAML::Node& doc, bson* brec )
{
    std::string key;
    int type;

    for (YAML::const_iterator it = doc.begin(); it != doc.end(); ++it)
    {
        key = it->first.as<std::string>();
        type = it->second.Type();

        switch( type )
        {
        case YAML::NodeType::Null:
            bson_append_null(brec, key.c_str() );
            break;
        case YAML::NodeType::Scalar:
            parseScalar( key.c_str(), it->second, brec );
            break;
        case YAML::NodeType::Sequence:
            bson_append_start_array( brec, key.c_str() );
            parseArray( it->second, brec );
            bson_append_finish_array(brec);
            break;
        case YAML::NodeType::Map:
            bson_append_start_object( brec, key.c_str() );
            parseObject( it->second, brec );
            bson_append_finish_object(brec);
            break;
        case YAML::NodeType::Undefined:
            Error( "ParserYAML02: ", "Undefined value type.");
        }
    }
}

void parseArray( const YAML::Node& doc, bson* brec )
{
    int ii = 0;
    std::string key;
    int type;

    for (YAML::const_iterator it = doc.begin(); it != doc.end(); ++it, ++ii)
    {
        key = std::to_string(ii);
        type = it->Type();

        switch( type )
        {
        case YAML::NodeType::Null:
            bson_append_null(brec, key.c_str() );
            break;
        case YAML::NodeType::Scalar:
            parseScalar( key.c_str(), *it, brec );
            break;
        case YAML::NodeType::Sequence:
            bson_append_start_array( brec, key.c_str() );
            parseArray( *it, brec );
            bson_append_finish_array(brec);
            break;
        case YAML::NodeType::Map:
            bson_append_start_object( brec, key.c_str() );
            parseObject( *it, brec );
            bson_append_finish_object(brec);
            break;
        case YAML::NodeType::Undefined:
            Error( "ParserYAML02: ", "Undefined value type.");
        }
    }
}

void parseScalar(const char* key, const YAML::Node& doc, bson* brec )
{
    std::string value = doc.as<std::string>();
    addScalar( key, value, brec );
}

};

// BsonHandler------------------------------------------------

std::string BsonHandler::to_string()
{
    std::string yamlstr;
    ParserYAML::printBsonObjectToYAML( yamlstr, m_bson->data);
    return yamlstr;
}

//struct Mark;

BsonHandler::BsonHandler(bson* bobj) : m_bson(bobj) {}

void BsonHandler::OnDocumentStart(const YAML::Mark&) {}

void BsonHandler::OnDocumentEnd() {}

void BsonHandler::OnNull(const YAML::Mark&, YAML::anchor_t /*anchor*/)
{
    assert(m_stateStack.top().state == WaitingForValue );
    bson_append_null( m_bson, m_stateStack.top().key.c_str() );
    BeginNode();
}

void BsonHandler::OnAlias(const YAML::Mark&, YAML::anchor_t /*anchor*/) {}

void BsonHandler::OnScalar(const YAML::Mark&, const std::string& /*tag*/,
                           YAML::anchor_t /*anchor*/, const std::string& value)
{
    switch (m_stateStack.top().state)
    {
    case WaitingForSequenceEntry:
    { std::string key = std::to_string(m_stateStack.top().ndx++);
        addScalar( key.c_str(), value, m_bson );
        break;
    }
    case WaitingForKey:
        m_stateStack.top().key = value;
        m_stateStack.top().state = WaitingForValue;
        break;
    case WaitingForValue:
        std::string key =  m_stateStack.top().key;
        addScalar( key.c_str(), value, m_bson );
        m_stateStack.top().key = "";
        m_stateStack.top().state = WaitingForKey;
        break;
    }
}

void BsonHandler::OnSequenceStart(const YAML::Mark&, const std::string& /*tag*/,
                                  YAML::anchor_t /*anchor*/, YAML::EmitterStyle::value /*style*/)
{
    if (!m_stateStack.empty())
    {
        std::string key;
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

void BsonHandler::OnMapStart(const YAML::Mark&, const std::string& /*tag*/,
                             YAML::anchor_t /*anchor*/, YAML::EmitterStyle::value /*style*/)
{
    if (!m_stateStack.empty() )
    {
        std::string key;
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

std::string JsonHandler::to_string()
{
    return m_os.str();
}

JsonHandler::JsonHandler(std::stringstream& os_) : m_os(os_) {}

void JsonHandler::OnDocumentStart(const YAML::Mark&) {}

void JsonHandler::OnDocumentEnd() {}

void JsonHandler::OnNull(const YAML::Mark&, YAML::anchor_t /*anchor*/)
{
    assert(m_stateStack.top().state == WaitingForValue );
    addScalar(m_stateStack.top().key.c_str(), "null" );
    BeginNode();
}

void JsonHandler::OnAlias(const YAML::Mark&, YAML::anchor_t /*anchor*/) {}

void JsonHandler::OnScalar(const YAML::Mark&, const std::string& /*tag*/,
                           YAML::anchor_t /*anchor*/, const std::string& value)
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
        std::string key =  m_stateStack.top().key;
        addScalar( key.c_str(), value );
        m_stateStack.top().key = "";
        m_stateStack.top().state = WaitingForKey;
        break;
    }
}

void JsonHandler::OnSequenceStart(const YAML::Mark&, const std::string& /*tag*/,
                                  YAML::anchor_t /*anchor*/, YAML::EmitterStyle::value /*style*/)
{
    std::string key="";
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

void JsonHandler::OnMapStart(const YAML::Mark&, const std::string& /*tag*/,
                             YAML::anchor_t /*anchor*/, YAML::EmitterStyle::value /*style*/)
{
    std::string key="";
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

void JsonHandler::addHead(const std::string& key )
{
    if(!m_first )
        m_os <<  ",\n";
    else
        m_first = false;

    shift();

    if( !key.empty())
        m_os << "\"" << key << "\" :   ";
}

void JsonHandler::addScalar(const std::string&  key, const std::string& value )
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
