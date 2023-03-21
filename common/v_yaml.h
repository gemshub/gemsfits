#pragma once

#include "v_json.h"

namespace common {

namespace yaml {

/// @brief Dump object to YAML string.
std::string dump(const JsonFree& object);

/// Deserialize a YAML document to a free format json object.
JsonFree parse(const std::string& yaml_str);

/// Parse YAML string to json string
std::string Yaml2Json(const std::string& yaml_str);

/// Parse json string to YAML string
std::string Json2Yaml(const std::string& json_str);

} // namespace yaml

} // namespace common

