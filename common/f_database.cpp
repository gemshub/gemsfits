//-------------------------------------------------------------------
// $Id: f_ejdb.cpp 333 2014-03-13 13:23:32Z gemsfits $
//
// Implementation of TEJDBKey, TEJDB, TEJDataBase and EJDataBaseList classes
//
// Copyright (C) 2014-2023  S.V.Dmytriyeva
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

#include <iostream>
#include <cstdarg>
#include <cstring>

#include "f_database.h"
#include "f_file.h"
#include "FITMainWindow.h"
#include "v_yaml.h"
#include "v_service.h"
#include "keywords.h"
#include "yaml-cpp/emitfromevents.h"

const char* ALLKEY="*";

namespace common {

//-------------------------------------------------------------
// IndexEntry  - Element in sequence of record keys
//-------------------------------------------------------------

bool operator <(const IndexEntry& iEl, const IndexEntry& iEr)
{
    for( size_t ii=0; ii<iEl.key_flds.size(); ii++ ) {
        if(iEl.key_flds[ii] == iEr.key_flds[ii])
            continue;
        return (iEl.key_flds[ii] < iEr.key_flds[ii] ? true: false);
    }
    return false;
}

bool operator >(const IndexEntry& iEl, const IndexEntry& iEr)
{
    for( size_t ii=0; ii<iEl.key_flds.size(); ii++ ) {
        if(iEl.key_flds[ii] == iEr.key_flds[ii])
            continue;
        return (iEl.key_flds[ii] > iEr.key_flds[ii] ? true: false);
    }
    return false;
}

bool operator ==(const IndexEntry& iEl, const IndexEntry& iEr)
{
    return equal(iEl.key_flds.begin(), iEl.key_flds.end(), iEr.key_flds.begin());
}

bool operator !=(const IndexEntry& iEl, const IndexEntry& iEr)
{
    return !(iEl==iEr);
}

} // common

//-------------------------------------------------------------
// TEJDBKey  - working with DB record key
//-------------------------------------------------------------

/// Default configuration
TEJDBKey::TEJDBKey(const std::vector<std::string>& field_names)
{
    for(const auto& name: field_names) {
        rk_fld_name.push_back(name);
        rk_fld.push_back("*");
    }
}

// Return record key in packed form
const std::string& TEJDBKey::pack(const std::vector<std::string>& akey_flds)
{
    pack_key.clear();
    for(const auto& fld: akey_flds) {
        //strip( fld );
        pack_key += fld;
        pack_key+= ":";
    }
    return pack_key;
}

/// Check if pattern in record key
bool TEJDBKey::is_pattern()
{
    for(const auto& fld: rk_fld) {
        if(fld.find_first_of("*?") != std::string::npos) {
            return true;
        }
    }
    return false;
}

/// Setted ALLKEY
bool TEJDBKey::is_all()
{
    for(const auto& fld: rk_fld) {
        if(fld != "*") {
            return false;
        }
    }
    return true;
}

// Putting DB record key key to internal structure
void TEJDBKey::setKey(const std::string& pkey)
{
    size_t ii;
    ErrorIf( pkey.empty(), "TEJDBKey", "Undefined key of record.");
    std::vector<std::string> rk_fld_new;
    std::string sp = pkey;

    if( sp == ALLKEY ) {
        for(ii=0; ii<keySize(); ii++)
            rk_fld_new.push_back("*");
    }
    else {
        for(ii=0; ii<keySize(); ii++) {
            size_t pos = sp.find_first_of(':');
            if( pos == std::string::npos ) {
                if( ii < keySize()-1)
                    Error(pkey, "Invalid packed record key.");
            }
            rk_fld_new.push_back(sp.substr(0, pos));
            strip(rk_fld_new[ii]);
            if( sp[pos] == ':' )
                pos++;
            sp = sp.substr(pos);
        }
    }
    rk_fld = std::move(rk_fld_new);
}

// Change i-th field of TEJDBKey to key
void TEJDBKey::setKeyField(size_t ndx, const char *fld)
{
    ErrorIf(ndx>=keySize(), fld, "Invalid key field number");
    std::string sp = fld;
    strip(sp);
    rk_fld[ndx] = sp;
}

bool TEJDBKey::compareTemplate(const common::IndexEntry& elm)
{
    size_t i, j, rklen;
    std::string kpart, kelm;

    for( j=0; j<keySize(); j++) {
        kpart = keyField(j);
        kelm = elm.get_key_field(j);

        if( kpart == "*" )
            continue; //to next field

        if( kpart.length() > kelm.length() )
            return false;

        rklen = kpart.length();
        for( i=0; i<rklen; i++ ) {
            if( kpart[i] == '*' ) // next field
                break;
            switch( kpart[i] ) {
            case '?': // no ' '
                if( kelm[i] == ' ' )
                    return false;
                break;
            default:
                if( kpart[i] != kelm[i] )
                    return false;
            }
        }
        if( kpart[i] != '*' && i < kelm.length())
            return false;
    }
    return true;
}

//-------------------------------------------------------------
// TEJDataBase - This class contains the structure of EJDB Data Base
//-------------------------------------------------------------

// Set json or yaml format string to curent record
void TEJDataBase::setJson(const std::string& sjson, bool is_json)
{
    if( is_json ) {
        current_Json = sjson;
        current_YAML = "";
    }
    else {
        current_YAML = sjson;
        current_Json = "";
    }
}

void TEJDataBase::setQuery(const std::string& query_str)
{
    current_JQL = query_str;
    replace_all(current_JQL, "\'", '\"');
}

std::string TEJDataBase::getKeyFromJson(const common::JsonFree& bsdata)
{
    std::string key_str = "", kbuf;
    for(size_t ii=0; ii<keySize(); ii++) {
        kbuf = bsdata.value(key.keyFieldName(ii), "*");
        strip( kbuf );
        key_str += kbuf;
        key_str += ":";
    }
    return key_str;
}


void TEJDataBase::addKeyToJson(common::JsonFree& object)
{
    for(size_t ii=0; ii<keySize(); ii++) {
        object[key.keyFieldName(ii)] = key.keyField(ii);
    }
}

// Save current record to json structure
common::JsonFree TEJDataBase::current_to_json(const std::string& pkey)
{
    common::JsonFree object;
    if(!current_Json.empty())
        object = fromJsonString(current_Json);
    else
        object = fromYamlString(current_YAML);
    std::string key_str = getKeyFromJson(object);

    if(!pkey.empty()) {
        // Try to insert new record to list
        key.setKey(pkey);
        if(key.packKey() != key_str )  {
            std::string mess = " Try to update record with changed key fields\n";
            mess += " You must use command Insert";
            Error( pkey, mess );
        }
    }
    else {
        key.setKey(key_str);
    }

    if( key.is_pattern() )
        Error("TEJDB0010", "Cannot save under record key template" );
    return object;
}

// Load data from json string
std::string TEJDataBase::record_from_json(const std::string& json_str)
{
    current_Json = json_str;
    auto object = fromJsonString(current_Json);
    current_YAML = common::yaml::dump(object);

    current_Gems3k_name.clear();
    current_Gems3k_name = object.value(keys::G3Ksys[1], current_Gems3k_name);
    current_Gems3k_name = object.value(keys::G3Ksys[0], current_Gems3k_name);

    return getKeyFromJson(object);
}


// Load data from bson structure
void TEJDataBase::add_key_to_set(const std::string& key_str, const common::ejdb_id_type& oidhex)
{
    // Try to insert new record to list
    key.setKey(key_str);
    if( key.is_pattern() )
        Error("TEJDB0110", "Cannot save under record key template" );

    auto ret = records.insert(key.retIndex());
    current_ndx = ret.first;
    // Test unique keys name before add the record(s)
    if( ret.second == false) {
        std::string erstr = "Cannot add new record:\n";
        erstr += key_str+".\nTwo records with the same key!";
        Error("TEJDB0014", erstr );
    }
    current_ndx->set_id(oidhex);
}

//---------------------------------------------------------------------

// Seach record index with key pkey.
bool TEJDataBase::findRecord(const std::string& pkey)
{
    if(records.empty())
        return false;
    TEJDBKey wkey(key);
    wkey.setKey(pkey);
    current_ndx = records.find(wkey.retIndex());
    return( current_ndx != records.end() );
}

// Retrive one record from the collection
void TEJDataBase::getRecord(const std::string& pkey)
{
    key.setKey(pkey);
    if(key.is_pattern())
        Error("TEJDB0010", "Cannot get under record key template" );

    current_ndx = records.find( key.retIndex());
    if(current_ndx == records.end()) {
        std::string erstr = pkey;
        erstr += "\nrecord to retrive does not exist!";
        Error("TEJDB0001", erstr );
    }

    std::string readed_json;

    dbdriver->read_record(getKeywd(), current_ndx, readed_json);
    // Save readed json string to internal arrays
    record_from_json(readed_json);
}

/// Removes record from the collection
void TEJDataBase::deleteRecord(const std::string& pkey)
{
    key.setKey( pkey );
    if( key.is_pattern() )
        Error("TEJDB0010", "Cannot delete under record key template" );

    current_ndx = records.find(key.retIndex());
    if(current_ndx == records.end()) {
        std::string erstr = pkey;
        erstr+= "\nrecord to delete does not exist!";
        Error("TEJDB0002", erstr );
    }

    dbdriver->delete_record(getKeywd(), current_ndx);
    // Set up internal data
    records.erase(current_ndx);
    current_ndx = records.end();
}

// Save new record in the collection
void TEJDataBase::insertRecord()
{
    // Get json structure from internal std::string
    auto object = current_to_json("");
    auto ret = records.insert(key.retIndex());
    current_ndx = ret.first;
    // Test unique keys name before add the record(s)
    if( ret.second == false)  {
        std::string erstr = "Cannot insert record:\n";
        erstr += key.packKey();
        erstr += ".\nTwo records with the same key!";
        Error("TEJDB0004", erstr );
    }

    auto err_message = dbdriver->create_record(getKeywd(), current_ndx, current_Json);
    if( !err_message.empty() ) {
        records.erase(current_ndx);
        Error("TEJDB0021",  err_message);
    }
}

// Save/update record in the collection
void TEJDataBase::saveRecord(const std::string& pkey)
{
    auto object = current_to_json(pkey);
    auto ret = records.insert(key.retIndex());
    current_ndx = ret.first;

    if( ret.second == true ) { // new record
        auto err_message = dbdriver->create_record(getKeywd(), current_ndx, current_Json);
        if( !err_message.empty() ) {
            records.erase(current_ndx);
            Error("TEJDB0021",  err_message);
        }
    }
    else { // update
        auto err_message = dbdriver->update_record(getKeywd(), current_ndx, current_Json);
        if( !err_message.empty() ) {
            Error("TEJDB0021",  err_message);
        }
    }
}

int TEJDataBase::getKeyList(const std::string& keypat, std::vector<std::string>& key_list)
{
    // Set key template
    TEJDBKey wkey(key);
    wkey.setKey( keypat );
    key_list.clear();

    bool OneRec = !wkey.is_pattern(),
            AllRecs = wkey.is_all();

    std::set<common::IndexEntry, std::less<common::IndexEntry> >::iterator it;
    it = records.begin();
    while( it != records.end() ) {
        if( !AllRecs )
            if( !wkey.compareTemplate( *it ) ) {
                it++;
                continue;
            }
        key_list.push_back( wkey.indexKey( *it ) );
        if( OneRec )
            break;

        it++;
    }
    return key_list.size();
}

void TEJDataBase::load_collection()
{
    common::SetReadedKey_f setfnc = [&](const std::string& key, const common::ejdb_id_type& oidhex)
    {
        add_key_to_set( key, oidhex );
    };
    dbdriver->all_query(getKeywd(), current_JQL, key.rk_fld_name, setfnc);
}


// write TP paris file
void TEJDataBase::selectQuery(const std::string& query, common::SetReaded_f setfnc)
{
    dbdriver->select_query(getKeywd(), query, setfnc);
}

// Open EJDB files and build linked record list
void TEJDataBase::OpenDB()
{
    records.clear();
    current_ndx= records.end();
    load_collection();
}

/// Close files all EJDB files
void TEJDataBase::CloseDB()
{
    records.clear();
    current_ndx = records.end();
}

// not used -------------------------------------

//Test state of record with key key_ as template.
// in field field setted any(*) data
bool TEJDataBase::findPart(const std::string& pkey, int field)
{
    TEJDBKey wkey(key);
    wkey.setKey(pkey);
    wkey.setKeyField(field,"*");
    return Rtest(wkey.packKey(), 0);
}

// Test state of record with key pkey.
// If mode == 1 and one record, read this record.
// returns state of record
bool TEJDataBase::Rtest(const std::string& akey, int mode)
{
    int iRec;
    std::string pkey;
    bool OneRec = true;

    if(!akey.empty())
        pkey = akey;
    else
        pkey = packKey();

    if(pkey.find_first_of("*?") != std::string::npos)
        OneRec = false; // pattern
    if(recordsCount() <= 0)
        return false;

    if( OneRec ) {
        if(!findRecord(pkey))
            return false;
        if(mode == 1)
            getRecord(pkey);
        return true;
    }
    else { // template
        std::vector<std::string> aKeyList;
        iRec = getKeyList(pkey, aKeyList);
        if(iRec == 0)
            return false;
        if(iRec == 1) {
            if(mode == 1)
                getRecord(aKeyList[0]);
            return true;
        }
        return true;
    }
}

//-------------------------------------------------------------
// DataBaseList
//-------------------------------------------------------------

int EJDataBaseList::Find(const char* s)
{
    for( size_t ii=0; ii<size(); ii++ )
        if( at(ii).getKeywd() == s  )
            return ii;
    return -1;
}

// default configuration
void EJDataBaseList::Init()
{
    // MDF_DATABASE default
    vector<std::string> dbKeyFlds;
    dbKeyFlds.push_back("sample");
    dbKeyFlds.push_back("expdataset");
    push_back( TEJDataBase( MDF_DATABASE, "experiments", dbKeyFlds  ));

    // MDF_TASK default
    vector<std::string> tsKeyFlds;
    tsKeyFlds.push_back("taskid");
    tsKeyFlds.push_back("projectid");
    push_back( TEJDataBase( MDF_TASK, "tests", tsKeyFlds  ));

    // MDF_FITS default
    push_back( TEJDataBase( MDF_FITS, "fits", tsKeyFlds  ));
}

void EJDataBaseList::Close()
{
    for(size_t ii=0; ii<size(); ii++ )
        at(ii).CloseDB();
    TEJDataBase::dbdriver->disconnect();
}

void EJDataBaseList::ChangePath(const std::string &ejdbPath)
{
    TEJDataBase::dbdriver->change_path(ejdbPath);
    TEJDataBase::dbdriver->connect();
}

TEJDataBase& EJDataBaseList::operator[](size_t ii)
{
    ErrorIf( ii > size(), "DataBaseList","Invalid chain index.");
    return at(ii);
}

EJDataBaseList rtEJ;

// ------------------------ end of f_ejdb.cpp --------------------------------------
