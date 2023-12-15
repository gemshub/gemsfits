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
#include "v_json.h"
#include "dbdriver.h"

extern const char* ALLKEY;
// Mode enums
enum { MDF_DATABASE=0, MDF_TASK=1, MDF_FITS=2 };

/// This is struct contened the key of record
class TEJDBKey
{
    friend class TEJDataBase;

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

    common::IndexEntry retIndex()
    {  return common::IndexEntry(rk_fld);  }
    /// Return index key in form
    std::string indexKey(const common::IndexEntry& ndx) const
    {  return ndx.pack_key(); }
    bool compareTemplate(const common::IndexEntry& elm);

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


/// This class contened the structure of Data Base
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
    common::keysmap_t records;
    /// Current index in recList
    common::keysmap_t::iterator current_ndx;

    // Work data
    /// last read/save record json
    std::string current_Json;
    /// last read gems3k files name (<SystemFiles>) in record
    std::string current_Gems3k_name;
    /// last query for select record
    std::string current_JQL;

public:

    static std::shared_ptr<common::DBDriverBase> dbdriver;

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
    /// Set json format std::string to curent record
    void setJson( const std::string& sjson);

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
    void selectQuery(const std::string &query, common::SetReaded_f setfnc);

    /// Get current record key from json structure
    std::string getKeyFromJson(const common::JsonFree& bsdata);
    /// Put current record key to json structure
    void addKeyToJson(common::JsonFree& object);

    /// Find record with key into internal record keys list
    bool findRecord(const std::string& pkey);
    /// Retrive one record from the collection
    void getRecord(const std::string& pkey);
    /// Removes record from the collection
    void deleteRecord(const std::string& pkey);
    /// Save/update record in the collection
    void saveRecord(const std::string& pkey);
    /// Save new record in the collection
    void insertRecord();

    /// Get key list for a wildcard search
    size_t getKeyList(const std::string& keypat, std::vector<std::string>& key_list);

    /// Open DB files and build linked record list
    void OpenDB();
    /// Close all DB files
    void CloseDB();

 // not used -------------------------------------
    /// Test state of record with key as template.
    /// in field field setted any(*) data
    bool findPart(const std::string& pkey, int field);

protected:

    /// Save current record to json structure
    common::JsonFree current_to_json(const std::string& pkey);
    /// Load data from json structure (return readed record key)
    std::string record_from_json(const std::string& json_str);
    /// Download all or by query (current_JQL) record keys from collection
    void load_collection();

    void add_key_to_set(const std::string& key, const common::ejdb_id_type& oidhex);

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
    void Init();
    void Close();
    void ChangePath(const std::string& ejdbPath);
};

extern EJDataBaseList rtEJ;

