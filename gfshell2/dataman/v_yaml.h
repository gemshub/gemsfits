#pragma once

#include "v_json.h"

namespace yaml {

/// @brief Dump object to YAML string.
std::string dump(const jsonio::JsonFree& object);

/// Deserialize a YAML document to a free format json object.
jsonio::JsonFree parse(const std::string& yaml_str);

/// Parse YAML string to json string
std::string Yaml2Json(const std::string& yaml_str);

/// Parse json string to YAML string
std::string Json2Yaml(const std::string& json_str);

} // namespace yaml


