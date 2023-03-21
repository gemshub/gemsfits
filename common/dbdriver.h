#pragma once

#include <functional>
#include <memory>
#include <map>
#include <vector>
#include <set>

namespace common {

class JsonFree;

#ifdef OLD_EJDB
typedef  std::string ejdb_id_type;
const ejdb_id_type empty_key="";
#else
/// Callback function fetching document from a collection that match the specified condition
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

    /// Return current record key in packed form
    std::string pack_key() const
    {
        std::string pack_key;
        for(const auto& fld: key_flds) {
            pack_key += fld+":";
        }
        return pack_key;
    }

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

/// Table key -> the selector, which must be an object containing the _id or _key attribute.
using keysmap_t = std::set<IndexEntry, std::less<IndexEntry>>;

/// Callback function fetching document from a collection that match the specified condition
using  SetReaded_f = std::function<void( const std::string& jsondata )>;
/// Callback function fetching document and _id from a collection that match the specified condition
using  SetReadedKey_f = std::function<void(const std::string& key, const ejdb_id_type& oidhex)>;

/// Interface for Abstract Database Driver
/// The driver works like an adaptor which connects a generic interface
/// to a specific database vendor implementation.
class DBDriverBase
{

public:

    ///  Constructor
    DBDriverBase()
    {}

    ///  Destructor
    virtual ~DBDriverBase()
    {}

    virtual void connect() = 0;
    virtual void disconnect() = 0;
    virtual void change_path(const std::string& path) = 0;

    // CRUD API

    /// Creates a new document in the collection from the given data.
    /// \param collname - collection name
    /// \param it -  pair: key -> selector
    /// \param jsonrec - json object with data
    /// \return the error message if error.
    virtual std::string create_record(const std::string& collname, keysmap_t::iterator& it, const std::string& jsonrec) = 0;

    /// Returns the document described by the selector.
    /// \param collname - collection name
    /// \param it -  pair: key -> selector
    /// \param jsonrec - object to receive data
    virtual bool read_record(const std::string& collname, keysmap_t::iterator& it, std::string& jsonrec) = 0;

    /// Update an existing document described by the selector.
    /// \param collname - collection name
    /// \param it -  pair: key -> selector
    /// \param jsonrec - json object with data
    /// \return the error message if error.
    virtual std::string update_record(const std::string& collname, keysmap_t::iterator& it, const std::string& jsonrec) = 0;

    /// Removes a document described by the selector.
    /// \param collname - collection name
    /// \param it -  pair: key -> selector
    virtual bool delete_record(const std::string& collname, keysmap_t::iterator& it) = 0;

    // Query API

    /// Fetches all documents from a collection that match the specified condition.
    ///  \param collname - collection name
    ///  \param query -    selection condition
    ///  \param setfnc -   callback function fetching document data
    virtual void select_query( const std::string& collname, const std::string& query, SetReaded_f setfnc ) = 0;

    /// Fetches all documents from a collection.
    ///  \param collname -    collection name
    ///  \param query -      default selection condition
    ///  \param key_fields - list of fields to selection
    ///  \param setfnc -     callback function fetching document data
    virtual void all_query( const std::string& collname, const std::string& query,
                            const std::vector<std::string>& key_fields, SetReadedKey_f setfnc ) = 0;

};

} // namespace common


