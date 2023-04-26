
#include <iostream>
#include <yaml-cpp/yaml.h>
#include "v_yaml.h"

namespace common {

namespace yaml {

void yaml_emitter(YAML::Emitter& out, const JsonFree& object);
JsonFree json_emitter(const YAML::Node &yaml_object);

//---------------------------------------------------------------------

void yaml_emitter(YAML::Emitter& out, const JsonFree& object)
{
    //for (const auto& childobj: object.items())
    for (const auto& childobj: object ) {
        if(object.is_object()) {
            out << YAML::Key << childobj->get_key();
            out << YAML::Value;
        }

        switch(childobj->type()) {
        case JsonFree::Null:
            out << YAML::_Null();
            break;
        case JsonFree::Bool:
        case JsonFree::Double:
            out << childobj->dump();
            break;
        case JsonFree::String:
            out << childobj->to_string();
            break;
        case JsonFree::Object:
            out << YAML::BeginMap;
            yaml_emitter(out, *childobj);
            out << YAML::EndMap;
            break;
        case JsonFree::Array:
            out << YAML::BeginSeq;
            yaml_emitter(out, *childobj);
            out << YAML::EndSeq;
            break;
        }
    }
}

//void yaml_emitter(YAML::Emitter& out, const JsonFree& object)
//{
//    //for (const auto& childobj: object.items())
//    for (const auto& [key, val] : object.items()) {
//        if(object.is_object()) {
//            out << YAML::Key << key;
//            out << YAML::Value;
//        }

//        switch(val.type())
//        {
//        case JsonFree::value_t::null:
//            out << YAML::_Null();
//            break;
//        case JsonFree::value_t::boolean:
//        case JsonFree::value_t::number_float:
//        case JsonFree::value_t::number_integer:
//        case JsonFree::value_t::number_unsigned:
//            out << val.dump();
//            break;
//        case JsonFree::value_t::string:
//            out << val.get<std::string>();
//            break;
//        case JsonFree::value_t::object:
//            out << YAML::BeginMap;
//            yaml_emitter(out, val);
//            out << YAML::EndMap;
//            break;
//        case JsonFree::value_t::array:
//            out << YAML::BeginSeq;
//            yaml_emitter(out, val);
//            out << YAML::EndSeq;
//            break;
//        case JsonFree::value_t::binary:
//        case JsonFree::value_t::discarded:
//            out  << key << "can't print type \n";
//        }
//    }
//}


JsonFree json_emitter(const YAML::Node &yaml_object)
{
    JsonFree json_object{};

    switch(yaml_object.Type()) {
    case YAML::NodeType::Null:
        break;
    case YAML::NodeType::Scalar:
        return JsonFree::scalar(yaml_object.as<std::string>());
    case YAML::NodeType::Sequence:
        for(const auto &node : yaml_object)
            json_object.push_back(json_emitter(node));
        break;
    case YAML::NodeType::Map:
        for(const auto &it : yaml_object)
            json_object[it.first.as<std::string>()] = json_emitter(it.second);
        break;
    default: break;
    }
    return json_object;
}

//-------------------------------------------------------------------------------------------------

std::string dump(const JsonFree& object)
{
    YAML::Emitter out;
    if( object.is_object() )
        out << YAML::BeginMap;
    else if( object.is_array() )
        out << YAML::BeginSeq;
    else
        return "";

    yaml_emitter( out, object );

    if( object.is_object() )
        out << YAML::EndMap;
    else
        out << YAML::EndSeq;
    return out.c_str();
}

JsonFree parse(const std::string &yaml_str)
{
    try{
        YAML::Node yaml_node = YAML::Load(yaml_str);
        return json_emitter(yaml_node);
    }
    catch(YAML::Exception& ex)  {
        Error("E01Yaml: ", std::string("Yaml parse error: ") + ex.what());
    }
}

std::string Yaml2Json(const std::string &yaml_str)
{
    auto object = yaml::parse(yaml_str);
    return object.dump();
}


std::string Json2Yaml(const std::string& json_str)
{
    try  {
        auto json_object  = fromJsonString(json_str);
        //std::cout << json_object.dump() << std::endl;
        return yaml::dump(json_object);
    }
    catch(YAML::Exception& ex) {
        std::cout << ex.what()<< std::endl;
        Error("E01Yaml: ", std::string("Yaml parse error: ") + ex.what());
    }
    catch(...) {
        std::cout << "Error" << std::endl;
        Error("E01Yaml: ", std::string("Yaml parse error: "));
    }
}

} // yaml namespace

} // common namespace

