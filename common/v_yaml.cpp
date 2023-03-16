
#include <iostream>
#include <yaml-cpp/yaml.h>
#include "v_yaml.h"

namespace yaml {

void yaml_emitter(YAML::Emitter& out, const jsonio::JsonFree& object);
jsonio::JsonFree json_emitter(const YAML::Node &yaml_object);

//---------------------------------------------------------------------

void yaml_emitter(YAML::Emitter& out, const jsonio::JsonFree& object)
{
    //for (const auto& childobj: object.items())
    for (const auto& childobj: object)
    {
        if(object.is_object()) {
            out << YAML::Key << childobj->get_key();
            out << YAML::Value;
        }

        switch(childobj->type())
        {
        case jsonio::JsonFree::Null:
            out << YAML::_Null();
            break;
        case jsonio::JsonFree::Bool:
        case jsonio::JsonFree::Double:
        case jsonio::JsonFree::Int:
            out << childobj->dump();
            break;
        case jsonio::JsonFree::String:
            out << childobj->to_string();
            break;
        case jsonio::JsonFree::Object:
            out << YAML::BeginMap;
            yaml_emitter(out, *childobj);
            out << YAML::EndMap;
            break;
        case jsonio::JsonFree::Array:
            out << YAML::BeginSeq;
            yaml_emitter(out, *childobj);
            out << YAML::EndSeq;
            break;
        }
    }
}

//void yaml_emitter(YAML::Emitter& out, const jsonio::JsonFree& object)
//{
//    //for (const auto& childobj: object.items())
//    for (const auto& [key, val] : object.items())
//    {
//        if(object.is_object()) {
//            out << YAML::Key << key;
//            out << YAML::Value;
//        }

//        switch(val.type())
//        {
//        case jsonio::JsonFree::value_t::null:
//            out << YAML::_Null();
//            break;
//        case jsonio::JsonFree::value_t::boolean:
//        case jsonio::JsonFree::value_t::number_float:
//        case jsonio::JsonFree::value_t::number_integer:
//        case jsonio::JsonFree::value_t::number_unsigned:
//            out << val.dump();
//            break;
//        case jsonio::JsonFree::value_t::string:
//            out << val.get<std::string>();
//            break;
//        case jsonio::JsonFree::value_t::object:
//            out << YAML::BeginMap;
//            yaml_emitter(out, val);
//            out << YAML::EndMap;
//            break;
//        case jsonio::JsonFree::value_t::array:
//            out << YAML::BeginSeq;
//            yaml_emitter(out, val);
//            out << YAML::EndSeq;
//            break;
//        case jsonio::JsonFree::value_t::binary:
//        case jsonio::JsonFree::value_t::discarded:
//            out  << key << "can't print type \n";
//        }
//    }
//}


jsonio::JsonFree json_emitter(const YAML::Node &yaml_object)
{
    jsonio::JsonFree json_object{};

    switch(yaml_object.Type())
    {
    case YAML::NodeType::Null:
        break;
    case YAML::NodeType::Scalar:
        return jsonio::JsonFree::scalar(yaml_object.as<std::string>());
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

std::string dump(const jsonio::JsonFree& object)
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

jsonio::JsonFree parse(const std::string &yaml_str)
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



