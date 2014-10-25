// Copyright (C) 2013 G.D.Miron, D.Kulik
// <GEMS Development Team, mailto:gems2.support@psi.ch>
//
// This file is part of the GEMSFIT2 code for parameterization of thermodynamic
// data and models <http://gems.web.psi.ch/GEMSFIT/>
//
// GEMSIFT2 is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.

// GEMSFIT2 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with GEMSFIT2 code. If not, see <http://www.gnu.org/licenses/>.
//-------------------------------------------------------------------
//


#ifndef JSON_PARSE_H
#define JSON_PARSE_H

#include <string>
#include <vector>

using namespace std;


/**
* Interprets 1 level JSON object pointed by the variable key
* @author DM
* @param query JSON object
* @param key JSON object member that we want to have the value/s returned
* @param result vector of strings representing the values of the key in the JSON object
* @date 19.04.2013
*/
void parse_JSON_object( string query, const char *key, vector<string> &result );

/**
* Interprets 2 level JSON object in the array arr pointed by the variable key
* @author DM
* @param data_ JSON object
* @param arr JSON array name that we want to parse
* @param key JSON array-object member that we want to have the value/s returned
* @param result vector of strings representing the values of the key in the JSON object
* @date 08.05.2013
*/
void parse_JSON_array_object( string data_, const char *arr , const char *key, vector<string> &result );

#endif // JSON_PARSE_H
