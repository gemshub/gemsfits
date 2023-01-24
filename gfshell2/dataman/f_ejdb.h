//-------------------------------------------------------------------
// $Id: f_ejdb.h 333 2014-03-13 13:23:32Z gemsfits $
//
// Declaration of TEJDBKey, TEJDataBase and EJDataBaseList classes
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

#pragma once

#include <set>
#include "f_ejdb_file.h"
#include "v_json.h"
#include "v_json_old.h"

extern const char* ALLKEY;

#ifdef OLD_EJDB
/// Callback function fetching document from a collection that match the specified condition
using  SetReaded_f = std::function<void( const char * bjsondata )>;
typedef  std::string ejdb_id_type;
const ejdb_id_type empty_key="";
#else

/// Callback function fetching document from a collection that match the specified condition
using  SetReaded_f = std::function<void( const char * bjsondata )>;
typedef  int64_t ejdb_id_type;
const ejdb_id_type empty_key=0;
#endif

/// Element in sequence of record keys
class IndexEntry
{
    /// Document id placeholder
    mutable ejdb_id_type ejdb_id;
    /// Record key fields
    std::vector<std::string> key_flds;

    friend class TEJDataBase;
    friend class TEJDBKey;

public:

    IndexEntry(ejdb_id_type id, const std::vector<std::string>& akey_flds):
        ejdb_id(id),  key_flds(akey_flds)
    {}
    IndexEntry(std::vector<std::string>& akey_flds):
        ejdb_id(empty_key),  key_flds(akey_flds)
    {}
    IndexEntry(const IndexEntry& ndx):
        ejdb_id(ndx.ejdb_id),  key_flds(ndx.key_flds)
    {}

    const std::string& get_key_field(int ii) const
    {  return key_flds[ii];  }

    ejdb_id_type get_id() const
    {  return ejdb_id;  }
    void set_id(ejdb_id_type oid) const
    {  ejdb_id = oid;  }

    friend bool operator <( const IndexEntry&,  const IndexEntry& );
    friend bool operator >( const IndexEntry&,  const IndexEntry& );
    friend bool operator==( const IndexEntry&,  const IndexEntry& );
    friend bool operator!=( const IndexEntry&,  const IndexEntry& );

};


/// This is struct contened the key of record
class TEJDBKey
{

public:
    TEJDBKey(const std::vector<std::string>& field_names);
    TEJDBKey(const TEJDBKey& other):
        rk_fld_name(other.rk_fld_name),
        rk_fld(other.rk_fld),
        pack_key(other.pack_key) {}
    virtual ~TEJDBKey() {}

    /// Return current record key in packed form
    const std::string& packKey()
    {  return pack(rk_fld);  }

    /// Return number of record key fields
    size_t keySize() const
    {  return rk_fld_name.size();  }

    /// Return record key field i
    const std::string& keyField(size_t i) const
    {  return rk_fld[i];  }
    /// Return record key field name i
    const std::string& keyFieldName(size_t i) const
    {  return rk_fld_name[i];  }

    //--- Manipulation keys
    /// Putting DB record key pkey to internal structure
    void setKey(const std::string& pkey);
    /// Change i-th field of TEJDBKey to fld
    void setKeyField(size_t ndx, const char *fld);

    IndexEntry retIndex()
    {  return IndexEntry(rk_fld);  }
    /// Return index key in form
    const std::string& indexKey(const IndexEntry& ndx)
    {  return pack(ndx.key_flds); }
    bool compareTemplate(const IndexEntry& elm);

    bool is_pattern();
    bool is_all();

protected:
    /// Key fields names
    std::vector<std::string> rk_fld_name;
    /// Current key fields
    std::vector<std::string> rk_fld;
    /// Current key in packed form ( external )
    std::string pack_key;

    /// Get record key in packed form
    const std::string& pack(const std::vector<std::string>& akey_flds);
};


/// This class contened the structure of EJDB Data Base
class TEJDataBase final
{
    /// Definition of collection
    std::string keywd;
    /// Module number
    size_t nRT;

    /// Definition of record key
    TEJDBKey key;

    // Definition of record key list
    /// Linked records list
    std::set<IndexEntry, std::less<IndexEntry>> records;
    /// Current index in recList
    std::set<IndexEntry, std::less<IndexEntry>>::iterator current_ndx;

    // Work data
    /// last read/save record json
    std::string current_Json;
    /// last read/save record YAML
    std::string current_YAML;
    /// last read gems3k files name (<SystemFiles>) in record
    std::string current_Gems3k_name;
    /// last query for select record
    std::string current_JQL;

public:

    /// Default configuration of the Data Base
    TEJDataBase(size_t nrt, const char* name,
                const std::vector<std::string>& key_flds_names):
        keywd(name), nRT(nrt), key(key_flds_names)
    {}
    ~TEJDataBase()
    {}

    /// Get name of modules DB
    const std::string& getKeywd() const
    {  return keywd;  }
    /// Set name of modules DB
    void setKeywd(const std::string& newKeywd)
    {  keywd = newKeywd;  }
    /// Get records count in opened files
    size_t recordsCount() const
    {  return records.size();  }

    /// Return number or record key fields
    size_t keySize() const
    {  return key.keySize();  }
    /// Access to TEJDBKey
    const TEJDBKey& getDBKey() const
    {  return key;  }
    /// Return current record key in packed form
    const std::string& packKey()
    {  return key.packKey();  }
    /// Putting DB record key pkey to internal structure
    void setKey(const std::string& ckey)
    {  key.setKey(ckey);  }
    /// Return record key field name i
    const char* keyFieldName(int i) const
    {  return key.keyFieldName(i).c_str();  }

    /// Return curent record in json format std::string
    const std::string& getJson() const
    {  return current_Json; }
    /// Return curent record in YAML format std::string
    const std::string& getYAML() const
    {  return current_YAML; }
    /// Set json format std::string to curent record
    void setJson( const std::string& sjson, bool is_json = true);

    /// Return curent gems3k files name
    const std::string& getGems3kName()
    {  return current_Gems3k_name; }

    /// Return curent query std::string value
    const std::string& getLastQuery()
    {  return current_JQL;  }
    /// Set query string for collection
    void setQuery(const std::string& query_str);
    /// Fetches all documents from a collection that match the specified condition.
    ///  \param query -    selection condition
    ///  \param setfnc -   callback function fetching document data
    void selectQuery(const std::string &query, SetReaded_f setfnc);

    /// Get current record key from json structure
    std::string getKeyFromJson(const jsonio::JsonFree& bsdata);
    /// Put current record key to json structure
    void addKeyToJson(jsonio::JsonFree& object);

    /// Find record with key into internal record keys list
    bool findRecord(const std::string& pkey);
    /// Retrive one record from the collection
    void getRecord(const std::string& pkey);
    /// Removes record from the collection
    void deleteRecord(const std::string& pkey);
    /// Save/update record in the collection
    void saveRecord(const std::string& pkey);
    /// Save/update record in the collection
    /// Question for replase
    void saveRecordQuestion(const std::string& pkey, bool& yes_to_all);
    /// Save new record in the collection
    void insertRecord();

    /// Get key list for a wildcard search
    int getKeyList(const std::string& keypat, std::vector<std::string>& key_list);

    /// Open EJDB files and build linked record list
    void Open();
    /// Close files all EJDB
    void Close();

 // not used -------------------------------------
    /// Test state of record with key as template.
    /// in field field setted any(*) data
    bool findPart(const std::string& pkey, int field);

protected:

    /// Save current record to json structure
    jsonio::JsonFree current_to_json(const std::string& pkey);
    /// Load data from json structure (return readed record key)
    std::string record_from_json(const std::string& json_str);
    /// Download all or by query (current_JQL) record keys from collection
    void load_collection();

#ifdef OLD_EJDB
    EJCOLL *open_collection(bool create_if_empty = true);
    void close_collection();
    std::string getKeyFromBson(const char* bsdata);
    void keyFromBson(const char* bsdata);
#endif

 // not used -------------------------------------
    /// Test state of record with key pkey.
    /// If mode == 1 and one record, read this record.
    bool Rtest(const std::string& pkey, int mode);
};


class EJDataBaseList: public std::vector<TEJDataBase>
{
public:
    EJDataBaseList()
    { Init(); }
    ~EJDataBaseList()
    {}

    TEJDataBase& operator[](size_t) ;
    int Find(const char* keywd);
    void Init();
};

extern EJDataBaseList rtEJ;

