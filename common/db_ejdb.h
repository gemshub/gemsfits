#pragma once

#ifdef OLD_EJDB

#include "dbdriver.h"

class EJCOLL;

namespace common {

class TEJDB;

/// Implementation of Database Driver using ejdb.
class EjdbDBClient: public DBDriverBase
{

public:

    ///  Constructor
    EjdbDBClient():
       DBDriverBase(), ejdb_db_path(""), ejdb_db(nullptr)
    {}
    /// Constructor
    explicit EjdbDBClient(const std::string& db_path):
        DBDriverBase(), ejdb_db_path(db_path), ejdb_db(nullptr)
    {}

    ///  Destructor
    ~EjdbDBClient()
    {}

    /// Ejdb connection data
    const std::string& connect_data() const
    {
        return ejdb_db_path;
    }

    void connect() override;
    void disconnect() override;
    void change_path(const std::string& path) override;

    // CRUD API

    /// Creates a new document in the collection from the given data.
    /// \param collname - collection name
    /// \param it -  pair: key -> selector
    /// \param jsonrec - json object with data
    /// \return the error message if error.
    std::string create_record(const std::string& collname, keysmap_t::iterator& it, const std::string& jsonrec) override;

    /// Returns the document described by the selector.
    /// \param collname - collection name
    /// \param it -  pair: key -> selector
    /// \param jsonrec - object to receive data
    bool read_record(const std::string& collname, keysmap_t::iterator& it, std::string& jsonrec) override;

    /// Update an existing document described by the selector.
    /// \param collname - collection name
    /// \param it -  pair: key -> selector
    /// \param jsonrec - json object with data
    /// \return the error message if error.
    std::string update_record(const std::string& collname, keysmap_t::iterator& it, const std::string& jsonrec) override;

    /// Removes a document described by the selector.
    /// \param collname - collection name
    /// \param it -  pair: key -> selector
    bool delete_record(const std::string& collname, keysmap_t::iterator& it) override;

    // Query API

    /// Fetches all documents from a collection that match the specified condition.
    ///  \param collname - collection name
    ///  \param query -    selection condition
    ///  \param setfnc -   callback function fetching document data
    void select_query(const std::string& collname, const std::string& query, SetReaded_f setfnc) override;

    /// Fetches all documents from a collection.
    ///  \param collname -    collection name
    ///  \param query_fields - list of fields to selection
    ///  \param setfnc -     callback function fetching document data
    void all_query(const std::string& collname, const std::string& query,
                   const std::vector<std::string>& key_fields, SetReadedKey_f setfnc) override;

protected:

    /// Ejdb connection data
    std::string ejdb_db_path;
    /// connection for db
    std::unique_ptr<TEJDB> ejdb_db;

    EJCOLL *open_collection(const std::string& collname, bool create_if_empty = true);
    void close_collection();
    std::string get_key_from_bson(const std::vector<std::string>& key_fields, const char* bsdata);

    bool is_connected();

};


} // namespace common

#endif
