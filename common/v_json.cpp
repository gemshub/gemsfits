#include <cmath>
#include <iomanip>
#include <iostream>
#include <fstream>
#include "v_yaml.h"
#include "v_detail.h"
#include "jsonparser.h"

std::string fix_cvs(const std::string& json_str)
{
    std::string tmp_str;
    bool is_str_object = false;
    for(size_t ii=0; ii<json_str.size(); ii++) {
        if(json_str[ii]=='\"')
            is_str_object = !is_str_object;
        if(is_str_object && json_str[ii]=='\n' && json_str[ii-1]!='\\')
            tmp_str += "\\n";
        else if(is_str_object && json_str[ii]=='\r' && json_str[ii-1]!='\\')
            tmp_str += "\\r";
        else
            tmp_str += json_str[ii];
    }
    return tmp_str;
}

common::JsonFree fromJsonString(const std::string& json_str)
{
    common::JsonFree object;
    try  {
        // temporally fix \n in csv
        auto tmpstr = fix_cvs(json_str);
        object = common::JsonFree::parse(tmpstr);
    }
    catch (TError& ex) {
        std::cout << ex.mess << std::endl;
        std::cout << json_str << std::endl;
        Error("E01JSon: ", std::string("Json parse error: ") + ex.mess);
    }
    return object;
}

common::JsonFree fromYamlString(const std::string& yaml_str)
{
    common::JsonFree object = common::yaml::parse(yaml_str);
    return object;
}

common::JsonFree fromString(bool is_json, const std::string& data_str)
{
    if(is_json)
        return fromJsonString(data_str);
    else
        return fromYamlString(data_str);
}


namespace common {

namespace detail {

static const std::string field_path_delimiters =  "./[]\"";

int doublePrecision = 15;
int floatPrecision = 7;
const char* infiniteValue = "---"; //"null";


std::string value2string( const char* value )
{
    return std::string(value);
}

template <> std::string value2string( const std::string& value )
{
    return value;
}

template<> std::string value2string( const bool& value )
{
    return value ? "true" : "false";
}

template <> std::string value2string( const double& value )
{
    if(std::isfinite(value)) {
        std::ostringstream os;
        os << std::setprecision(doublePrecision) << value;
        return os.str();
    }
    else {
        return infiniteValue;
    }
}

template <> std::string value2string( const float& value )
{
    if(std::isfinite(value)) {
        std::ostringstream os;
        os << std::setprecision(floatPrecision) << value;
        return os.str();
    }
    else {
        return infiniteValue;
    }
}

template <>  bool string2value( const std::string& data, std::string& value )
{
    value = data;
    return true;
}

template <>  bool string2value( const std::string& data, bool& value )
{
    value = ( data.find("true") != std::string::npos ? true: false);
    return true;
}

static std::string decode_string(const std::string &value )
{
    std::string out;
    out += '"';
    for (size_t i = 0; i < value.length(); i++) {
        const char ch = value[i];
        if (ch == '\\') {
            out += "\\\\";
        } else if (ch == '"') {
            out += "\\\"";
        } else if (ch == '\b') {
            out += "\\b";
        } else if (ch == '\f') {
            out += "\\f";
        } else if (ch == '\n') {
            out += "\\n";
        } else if (ch == '\r') {
            out += "\\r";
        } else if (ch == '\t') {
            out += "\\t";
        } else if (static_cast<uint8_t>(ch) <= 0x1f) {
            char buf[8];
            snprintf(buf, sizeof buf, "\\u%04x", ch);
            out += buf;
        } else if (static_cast<uint8_t>(ch) == 0xe2 && static_cast<uint8_t>(value[i+1]) == 0x80
                   && static_cast<uint8_t>(value[i+2]) == 0xa8) {
            out += "\\u2028";
            i += 2;
        } else if (static_cast<uint8_t>(ch) == 0xe2 && static_cast<uint8_t>(value[i+1]) == 0x80
                   && static_cast<uint8_t>(value[i+2]) == 0xa9) {
            out += "\\u2029";
            i += 2;
        } else {
            out += ch;
        }
    }
    out += '"';

    return out;
}

}

JsonFree::JsonFree( JsonFree::Type atype, const std::string &akey, const std::string &avalue, JsonFree *aparent ):
    field_type(atype), field_key(akey), field_value(avalue), ndx_in_parent(0), parent_object(aparent), children()
{
    if(parent_object) {
        ndx_in_parent = parent_object->children.size();
    }
}

JsonFree::JsonFree():
    field_type(JsonFree::Null), field_key("top"), field_value(""),
    ndx_in_parent(0), parent_object(nullptr), children()
{ }


JsonFree::JsonFree( const JsonFree &obj ):
    field_type(obj.field_type), field_key(obj.field_key), field_value(obj.field_value),
    ndx_in_parent(0), parent_object(nullptr), children()
{
    copy(obj);
}

JsonFree::JsonFree( JsonFree &&obj ) noexcept:
    field_type(obj.field_type), field_key( obj.field_key ), field_value( "" ),
    ndx_in_parent(obj.ndx_in_parent), parent_object(obj.parent_object), children()
{
    move(std::move(obj));
}


// Deserialize a JSON document to a json object
JsonFree JsonFree::scalar(const std::string &value)
{
    JsonFree object;
    int ival = 0;
    double dval=0.;

    if( value == "~" || value == "null" )
        ;
    else if( value == "true" )
        object = true;
    else if( value == "false" )
        object = false;
    else if( is<int>(ival, value) )
        object = ival;
    else if( is<double>(dval, value))
        object = dval;
    else
        object = value;

    return object;
}

std::string JsonFree::dump(bool dense) const
{
    std::stringstream os;
    dump(os,  dense);
    return os.str();
}

void JsonFree::dump( std::ostream &os, bool dense ) const
{
    auto objtype = type();
    if( objtype == Object ) {
        os << ( dense ? "{" : "{\n" );
    }
    else if( objtype == Array ) {
        os << ( dense ? "[" : "[\n" );
    }
    else if( objtype == String ) {
        os << detail::decode_string(field_value);
        return;
    }
    else {
        os << field_value;
        return;
    }

    dump2stream( os, 0, dense );
    if( objtype == Object ) {
        os << "}";
    }
    else {
        os << "]";
    }
}

JsonFree JsonFree::parse(const std::string &json_str)
{
    JsonParser parser("");
    return parser.parse(json_str);
}

JsonFree JsonFree::parse(std::fstream& ff)
{
    std::stringstream buffer;
    buffer << ff.rdbuf();
    auto retstr = buffer.str();
    return parse(retstr);
}

bool JsonFree::clear()
{
    children.clear();
    if( is_bool() )
        field_value = "false";
    else if( is_number() )
        field_value = "0";
    else
        field_value = "";

    return true;
}

std::string JsonFree::to_string( bool dense ) const
{
    if( is_structured() ) {
        return dump( dense );
    }
    return field_value;
}

double JsonFree::to_double() const
{
    double val = 0.0;
    if( is_number() )  {
        detail::string2value( field_value, val );
    }
    return val;
}

long JsonFree::to_int() const
{
    long val = 0;
    if( type() == Type::Int )  {
        detail::string2value( field_value, val );
    }
    return val;
}

bool JsonFree::to_bool() const
{
    bool val = false;
    if( is_bool() ) {
        detail::string2value( field_value, val );
    }
    return val;
}


std::string JsonFree::get_path() const
{
    if( is_top() || get_parent()->is_top()) {
        return get_key();
    }
    else {
        return  get_parent()->get_path()+"."+ get_key();
    }
}

//----------------------------------------------------------------

// key and parent not changed
void JsonFree::copy(const JsonFree &obj)
{
    field_type =  obj.field_type;
    // field_key =  obj.field_key; // copy only data
    field_value = obj.field_value;

    children.clear();
    for( const auto& child: obj.children ) {
        children.push_back( std::make_shared<JsonFree>(*child) );
        children.back()->parent_object = this;
        children.back()->ndx_in_parent = children.size()-1;
    }
}

// key and parent not changed
void JsonFree::move(JsonFree &&obj)
{
    field_type =  obj.field_type;
    // field_key =  std::move(obj.field_key);  // stl using
    field_value = std::move(obj.field_value);
    children = std::move(obj.children);

    obj.children.clear();
    for( const auto& child: children ) {
        child->parent_object = this;
    }
}

void JsonFree::update_node(JsonFree::Type atype, const std::string &avalue)
{
    children.clear();
    field_type = atype;
    field_value = avalue;
}

JsonFree& JsonFree::append_node(const std::string &akey, JsonFree::Type atype, const std::string &avalue )
{
    auto shptr = new JsonFree(atype, akey, avalue, this);
    children.push_back( std::shared_ptr<JsonFree>(shptr) );
    return *children.back();
}

const JsonFree& JsonFree::get_child(std::size_t idx) const
{
    ErrorIf( idx>=size(), "JsonFree", "array index " + std::to_string(idx) + " is out of range" );
    return *children[idx];
}

JsonFree& JsonFree::get_child(std::size_t idx)
{
    ErrorIf( idx>size(), "JsonFree", "array index " + std::to_string(idx) + " is out of range" );
    if( idx==size() ) { // next element
        append_node( std::to_string(idx), JsonFree::Null, "" );
        return *children.back();
    }
    return *children[idx];
}

JsonFree& JsonFree::get_child(const std::string &key)
{
    auto element = find_key(key);
    if( element == children.end() ) {
        return append_node( key, JsonFree::Null, "" );
    }
    return *element->get();
}

const JsonFree& JsonFree::get_child(const std::string &key) const
{
    auto element = find_key(key);
    if( element == children.end() ) {
        Error( "JsonFree", "key '" + key + "' not found" );
    }
    return *element->get();
}

bool JsonFree::remove_child( JsonFree* child )
{
    int thisndx = -1;
    for(std::size_t ii=0; ii< children.size(); ii++ )
    {
        if( children[ii].get() == child )
            thisndx = static_cast<int>(ii);
        if( thisndx >= 0 )
        {
          children[ii]->ndx_in_parent--;
          if( is_array() )
              children[ii]->field_key = std::to_string(children[ii]->ndx_in_parent);
        }
    }
    if( thisndx >= 0 )
    {   children.erase(children.begin() + thisndx);
        return true;
    }
    return false;
}

void JsonFree::array_resize( std::size_t  newsize, const std::string& defval  )
{
    ErrorIf( !is_array(), "JsonFree", "cannot resize not array data " + std::string( type_name() ) );

    if( newsize == children.size() )     // the same size
        ;
    else if( newsize < children.size() ) // delete if smaler
        children.erase( children.begin()+newsize, children.end());
    else {
        JsonFree chdefval;
        if(!defval.empty()) {
            chdefval =  JsonFree::parse(defval);
        }
        else if( children.size()>0 )  {
            chdefval = *children[0];
        }

        for( auto ii=children.size(); ii<newsize; ii++) {
            push_back(JsonFree(chdefval));
        }
    }
}

JsonFree *JsonFree::get_parent() const
{
    ErrorIf( !parent_object, "JsonFree", "parent Object is undefined" );
    return parent_object;
}

const JsonFree *JsonFree::child_from_ndx(std::size_t idx) const
{
    ErrorIf( idx>=size(), "JsonFree", "array index " + std::to_string(idx) + " is out of range" );
    return children[idx].get();

}

void JsonFree::dump2stream( std::ostream& os, int depth, bool dense ) const
{
    int temp;
    bool first = true;
    auto objtype = type();

    for( const auto& childobj: children ) {
        if( !first )
            os << ( dense ? "," : ",\n" );
        else
            first = false;

        // before print
        switch( objtype ) {
        case Object:
            if(!dense) {
                for (temp = 0; temp <= depth; temp++)
                    os <<  "     ";
            }
            os << "\"" << childobj->get_key() << ( dense ? "\":" : "\" :   " );
            break;
        case Array:
            if(!dense) {
                for (temp = 0; temp <= depth; temp++)
                    os << "     ";
            }
            break;
        default:
            break;
        }

        switch (childobj->type()) {
        // impotant datatypes
        case Null:
            os << "null";
            break;
        case Bool:
        case Int:
            os << childobj->field_value;
            break;
        case Double:
            os << std::setprecision(detail::doublePrecision) << childobj->to_double();
            break;
        case String:
            os << detail::decode_string( childobj->field_value );
            break;

            // main constructions
        case Object:
            os << ( dense ? "{" : "{\n" );
            childobj->dump2stream( os, depth + 1, dense );
            if(!dense) {
                for (temp = 0; temp <= depth; temp++)
                    os << "     ";
            }
            os << "}";
            break;
        case Array:
            os << ( dense ? "[" : "[\n" );
            childobj->dump2stream(os, depth + 1, dense );
            if(!dense) {
                for (temp = 0; temp <= depth; temp++)
                    os << "     ";
            }
            os << "]";
            break;
        default:
            os  << "can't print type : " << childobj->type();
        }
    }
    if( !dense )
        os << "\n";
}



const char *JsonFree::type_name(JsonFree::Type type)
{
    switch( type ) {
    case Int:
        return "int";
    case Double:
        return "double";
    case Null:
        return "null";
    case Object:
        return "object";
    case Array:
        return "array";
    case String:
        return "string";
    case Bool:
        return "bool";
    }
    return "";
}

} // namespace jsonio
