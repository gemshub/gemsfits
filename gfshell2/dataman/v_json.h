//-------------------------------------------------------------------
// $Id: v_json.h 333 2014-03-13 13:23:32Z gemsfits $
//
// Declaration of ParserJson class and other bson functions
//
// Copyright (C) 2023  S.V.Dmytriyeva
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

#pragma once

#include <vector>
#include <string>
#include <nlohmann/json.hpp>

nlohmann::json fromJsonString(const std::string& json_str);
nlohmann::json fromYamlString(const std::string& yaml_str);
nlohmann::json fromString(bool is_json, const std::string& data_str);

extern void csvToBson(nlohmann::json& object,
                      const std::vector<std::string>& headline,
                      const std::vector<std::string>& row);

#ifdef OLD_EJDB

#include <ejdb.h>

/// Class for read/write bson structure from/to text file or std::string
class ParserJson
{
protected:

    std::string jsontext;
    const char *curjson;
    const char *end;

    bool xblanc();
    void getString(std::string& str);
    void getNumber(double& value, int& type);
    void parseValue(const char *name, bson *brec);
    void bson_print_raw_txt(std::iostream& os, const char *data, int depth, int datatype);

public:

    ParserJson():curjson(0), end(0)
    {}
    virtual ~ParserJson()
    {}

    /// Parse internal jsontext std::string to bson structure (without first {)
    void parseObject(bson *brec);
    void parseArray(bson *brec);

    /// Print bson structure to JSON std::string
    void printBsonObjectToJson(std::string& resStr, const char *b);

    /// Read one json object from text file
    std::string readObjectText(std::fstream& fin);
    /// Set up internal jsontext before parsing
    void  setJsonText(const std::string& json);

};

/// Get std::string from bson structure by name
bool bson_find_string(const char *obj, const char *name, std::string& str);


#endif
