//-------------------------------------------------------------------
// $Id: v_json_old.h 333 2014-03-13 13:23:32Z gemsfits $
//
// Declaration of ParserJson class and other json functions
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


#ifdef OLD_EJDB

#include <string>
#include <ejdb/bson.h>

namespace common {

/// Get std::string from bson structure by name
bool bson_find_string(const char *obj, const char *name, std::string& str);

/// Generate json string from bson structure
bool bson_to_json_string(const char *bsdata, std::string& result_json);

/// Generate bson structure from json string
bool json_string_to_bson(const std::string& input_json, bson *bsrec);

} // namespace common

#endif
