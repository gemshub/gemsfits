
#include <iostream>
#include <yaml-cpp/yaml.h>
#include "v_detail.h"
#include "v_yaml.h"

namespace yaml {

void yaml_emitter(YAML::Emitter& out, const nlohmann::json& object);
nlohmann::json json_emitter(const YAML::Node &yaml_object);

//---------------------------------------------------------------------

void yaml_emitter(YAML::Emitter& out, const nlohmann::json& object)
{
    //for (const auto& childobj: object.items())
    for (const auto& [key, val] : object.items())
    {
        if(object.is_object()) {
            out << YAML::Key << key;
            out << YAML::Value;
        }

        switch(val.type())
        {
        case nlohmann::json::value_t::null:
            out << YAML::_Null();
            break;
        case nlohmann::json::value_t::boolean:
        case nlohmann::json::value_t::number_float:
        case nlohmann::json::value_t::number_integer:
        case nlohmann::json::value_t::number_unsigned:
            out << val.dump();
            break;
        case nlohmann::json::value_t::string:
            out << val.get<std::string>();
            break;
        case nlohmann::json::value_t::object:
            out << YAML::BeginMap;
            yaml_emitter(out, val);
            out << YAML::EndMap;
            break;
        case nlohmann::json::value_t::array:
            out << YAML::BeginSeq;
            yaml_emitter(out, val);
            out << YAML::EndSeq;
            break;
        case nlohmann::json::value_t::binary:
        case nlohmann::json::value_t::discarded:
            out  << key << "can't print type \n";
        }
    }
}

nlohmann::json add_scalar(const std::string& value)
{
    nlohmann::json object;
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

nlohmann::json json_emitter(const YAML::Node &yaml_object)
{
    nlohmann::json json_object{};

    switch(yaml_object.Type())
    {
    case YAML::NodeType::Null:
        break;
    case YAML::NodeType::Scalar:
        return add_scalar(yaml_object.as<std::string>());
    case YAML::NodeType::Sequence:
        for(const auto &node : yaml_object)
            json_object.emplace_back(json_emitter(node));
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

std::string dump(const nlohmann::json& object)
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

nlohmann::json loads(const std::string &yaml_str)
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
    auto object = yaml::loads(yaml_str);
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



