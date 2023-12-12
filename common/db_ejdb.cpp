#ifdef OLD_EJDB

#include <iostream>
#include <ejdb/ejdb.h>
#include <ejdb/bson.h>
#include "db_ejdb.h"
#include "f_file.h"
#include "f_database.h"
#include "v_bson_ejdb.h"
#include "v_service.h"

#ifdef useomp
#include <omp.h>
#endif

std::shared_ptr<common::DBDriverBase> TEJDataBase::dbdriver = std::make_shared<common::EjdbDBClient>();

namespace common {

/// Class for EJDB file manipulation
class TEJDB: public TDataBaseFile
{

public:
    EJDB *ejDB;
    int numEJDB=0;    ///< Number of usage EJDB database
    TEJDB(const std::string& path):
        TDataBaseFile(path), ejDB(nullptr)
    {}
    ~TEJDB()
    {
        if(is_opened) {
//            Close();
        }
    }

    void Open() override
    {

        if(is_opened) {
            numEJDB++;
        }
        else
        {
            // Test and open file  (name of ejdb must be take from nFile)
            ejDB = ejdbnew();
            if (!ejdbopen(ejDB, path.c_str(), JBOWRITER | JBOCREAT )) {
                ejdbdel(ejDB);
                ejDB = 0;
                is_opened = false;
                Error( path, " EJDB1 open error");
            }
            numEJDB++;
            is_opened = true;
        }
        //std::cout << "EJDB open " << numEJDB << std::endl;
    }

    void Close() override
    {
        numEJDB--;

        if( numEJDB <= 0 ) {
            if(ejDB ) {
                ejdbclose(ejDB);
                ejdbdel(ejDB);
                ejDB = 0;
            }
            is_opened = false;
        }
        //std::cout << "EJDB close " << numEJDB << std::endl;
    }
};

//-------------------------------------------------------------
// EjdbDBClient
//-------------------------------------------------------------

bool EjdbDBClient::is_connected() {
    return ( ejdb_db!=nullptr );// && ejdb_db->ejDB);
}

void EjdbDBClient::connect()
{
  if(!ejdb_db) {
    ejdb_db = std::make_unique<TEJDB>(ejdb_db_path);
  }
  //ejdb_db->Open();
  // Test and open file  (name of ejdb must be take from nFile)
  //if(!ejdb_db->ejDB) {
  //      std::string err= "Cannot open EJDB "+ ejdb_db->Name();
  //      Error("TEJDB0011", err );
  //}
}

void EjdbDBClient::disconnect()
{
    if(is_connected()) {
      ejdb_db->Close();
    }
}

void EjdbDBClient::change_path(const std::string &path)
{
    disconnect();
    ejdb_db_path = path;
    if(!ejdb_db) {
      ejdb_db = std::make_unique<TEJDB>(ejdb_db_path);
    }
    ejdb_db->ChangePath(ejdb_db_path);
}

std::string EjdbDBClient::generate_query(const JsonFree &object)
{
    return object.dump();
}

/// Get or create collection 'module name' before saving/delete
EJCOLL *EjdbDBClient::open_collection(const std::string& collname, bool createifempty)
{
    ejdb_db->Open();
    EJCOLL *coll=nullptr;
    if( createifempty ) {  // Get or create collection 'module name'
        coll = ejdbcreatecoll(ejdb_db->ejDB, collname.c_str(), NULL );
        if( !coll ) {
            std::string err= "Cannot open EJDB1 collection "+ ejdb_db->Name()+collname;
            Error("TEJDB0012", err );
        }
    }
    else {
        coll = ejdbgetcoll(ejdb_db->ejDB, collname.c_str());
    }

    return coll;
}

void EjdbDBClient::close_collection()
{
 // do we need free   EJCOLL *
    ejdb_db->Close();
}

std::string EjdbDBClient::get_key_from_bson(const std::vector<std::string>& key_fields,  const char* bsdata)
{
    //get key from object
    std::string keyStr = "", kbuf;
    for(const auto& key_fld: key_fields) {
        if( !bson_find_string( bsdata, key_fld.c_str(), kbuf ) )
            kbuf = "*";
        strip(kbuf);
        keyStr += kbuf;
        keyStr += ":";

    }
    return keyStr;
}


std::string EjdbDBClient::create_record(const std::string &collname,
                                        keysmap_t::iterator& it, const std::string& jsonrec)
{
    if(!is_connected()) {
      return "Database not connected";
    }
    std::string errejdb;
    bson bsrec;

    // Get bson structure from internal std::string
    bson_init(&bsrec);
    json_string_to_bson(jsonrec, &bsrec);
    bson_finish(&bsrec);

    EJCOLL *coll = open_collection(collname);
    // Persist BSON object in the collection
    bson_oid_t oid;
    bool retSave = ejdbsavebson(coll, &bsrec, &oid);
    close_collection();
    if( !retSave ) {
        errejdb = bson_first_errormsg(&bsrec);
    }
    else  {
        char bytes[25];
        bson_oid_to_string( &oid, bytes );
        it->set_id(bytes);
    }
    //std::cout << "Add record " << retSave << " oid " << bytes << std::endl;

    bson_destroy(&bsrec);
    return errejdb;
}

// Retrive one record from the collection
bool EjdbDBClient::read_record(const std::string& collname, keysmap_t::iterator&it, std::string& jsonrec)
{
    if(!is_connected()) {
      return false;
    }
    // Get oid of record
    bson_oid_t oid;
    bson_oid_from_string( &oid, it->get_id().c_str() );

    EJCOLL *coll = open_collection(collname);
    bson *bsrec = ejdbloadbson(coll, &oid);
    close_collection();

    if(!bsrec){
        std::string errejdb = "Error Loading record ";
        errejdb+= it->pack_key()+" from EJDB1";
        Error( "TEJDB0025",  errejdb );
    }

    // record to json string
    bson_to_json_string(bsrec->data, jsonrec);
    bson_destroy(bsrec);
    return true;
}

// Removes record from the collection
bool EjdbDBClient::delete_record(const std::string& collname, keysmap_t::iterator& itr)
{
    if(!is_connected()) {
      return false;
    }
    bson_oid_t oid;
    bson_oid_from_string( &oid, itr->get_id().c_str() );

    EJCOLL *coll = open_collection(collname);
    // Remove BSON object from collection.
    auto ret = ejdbrmbson( coll,  &oid );
    close_collection();

    if( !ret ) {
        std::string errejdb = "Error deleting of record ";
        errejdb+= itr->pack_key() + " from EJDB1";
        Error( "TEJDB0024",  errejdb );
    }
    return true;
}

// Save/update record in the collection
std::string EjdbDBClient::update_record(const std::string& collname, keysmap_t::iterator& it, const std::string& jsonrec)
{
    if(!is_connected()) {
      return "Database not connected";
    }

    std::string errejdb;
    bson_oid_t oid;
    bson bsrec;

    bson_init(&bsrec);
    // Get bson structure from internal std::string
    json_string_to_bson(jsonrec, &bsrec);
    bson_oid_from_string(&oid, it->get_id().c_str());
    bson_append_oid( &bsrec, JDBIDKEYNAME, &oid);
    bson_finish( &bsrec );

    EJCOLL *coll = open_collection(collname);
    // Persist BSON object in the collection
    bool retSave = ejdbsavebson(coll, &bsrec, &oid);
    close_collection();

    if( !retSave ) {
        errejdb = bson_first_errormsg(&bsrec);
    }

    //std::cout << "Saving record " << retSave << " oid " << current_ndx->get_id() << std::endl;
    bson_destroy(&bsrec);
    return errejdb;

}


void EjdbDBClient::select_query(const std::string& collname, const std::string& query,  SetReaded_f setfnc)
{
    if(!is_connected()) {
      return;
    }

    EJCOLL *coll = open_collection(collname, false);
    if( !coll ) {
        close_collection();
        return;
    }

    bson bsq1;
    bson_init_as_query(&bsq1);
    if( !query.empty() ) {
        json_string_to_bson(query, &bsq1);
    }
    bson_finish(&bsq1);

    EJQ *q = ejdbcreatequery(ejdb_db->ejDB, &bsq1, NULL, 0, NULL);
    ErrorIf( !q, "TEJDB0015", "Error in query (test)" );
    uint32_t count = 0;
    TCXSTR *log = tcxstrnew();
    TCLIST *qres = ejdbqryexecute(coll, q, &count, 0, log);
    //fprintf(stderr, "%s", TCXSTRPTR(log));
    //std::cout << count << " records in collection " << collname << std::endl;

    for (int i = 0; i < TCLISTNUM(qres); ++i) {
        void *bsdata = TCLISTVALPTR(qres, i);
        std::string json_str;
        bson_to_json_string(static_cast<const char*>(bsdata), json_str);
        setfnc(json_str);
    }

    tclistdel(qres);
    tcxstrdel(log);
    ejdbquerydel(q);
    bson_destroy(&bsq1);

    close_collection();
}

void EjdbDBClient::select_query_omp(const std::string& collname, const std::string& query, SetReaded_f setfnc, int num_threads)
{
    if(!is_connected()) {
      return;
    }

    EJCOLL *coll = open_collection(collname, false);
    if( !coll ) {
        close_collection();
        return;
    }

    bson bsq1;
    bson_init_as_query(&bsq1);
    if( !query.empty() ) {
        json_string_to_bson(query, &bsq1);
    }
    bson_finish(&bsq1);

    // make order
    bson bshits1;
    bson_init_as_query(&bshits1);
    bson_append_start_object(&bshits1, "$orderby");
    bson_append_int(&bshits1, "sample", 1);
    bson_append_finish_object(&bshits1);
    bson_finish(&bshits1);

    EJQ *q = ejdbcreatequery(ejdb_db->ejDB, &bsq1, NULL, 0, &bshits1);
    ErrorIf( !q, "TEJDB0015", "Error in query (test)" );
    uint32_t count = 0;
    TCXSTR *log = tcxstrnew();
    TCLIST *qres = ejdbqryexecute(coll, q, &count, 0, log);
    //fprintf(stderr, "%s", TCXSTRPTR(log));
    //std::cout << count << " records in collection " << collname << std::endl;

#ifdef useomp1 // hide parallel valrind problems
    omp_set_num_threads(num_threads);
#ifdef _WIN32
    #pragma omp parallel for schedule(static)
#else
    #pragma omp parallel for schedule(dynamic)
#endif
#endif
    for (int i = 0; i < TCLISTNUM(qres); ++i) {
        void *bsdata = TCLISTVALPTR(qres, i);
        std::string json_str;
        bson_to_json_string(static_cast<const char*>(bsdata), json_str);
        setfnc(json_str);
    }

    tclistdel(qres);
    tcxstrdel(log);
    ejdbquerydel(q);
    bson_destroy(&bshits1);
    bson_destroy(&bsq1);

    close_collection();
}


void EjdbDBClient::all_query(const std::string& collname, const std::string& query,
                             const std::vector<std::string>& key_fields, SetReadedKey_f setfnc)
{
    if(!is_connected()) {
      return;
    }

    EJCOLL *coll = open_collection(collname, false);
    if( !coll ) {
        close_collection();
        return;
    }

    // select all records by query
    bson bsq1;
    bson_init_as_query(&bsq1);
    if( !query.empty() ) {
        json_string_to_bson(query,  &bsq1 );
    }
    bson_finish(&bsq1);

    // make list of fields
    bson bshits1;
    bson_init_as_query(&bshits1);
    bson_append_start_object(&bshits1, "$fields");
    bson_append_int(&bshits1, "_id", 1);
    for(const auto& key_fls: key_fields) {
        bson_append_int(&bshits1, key_fls.c_str(), 1);
    }
    bson_append_finish_object(&bshits1);
    bson_finish(&bshits1);

    EJQ *q1 = ejdbcreatequery(ejdb_db->ejDB, &bsq1, NULL, 0, &bshits1);
    ErrorIf( !q1, "TEJDB0013", "Error in query (test)" );

    uint32_t count = 0;
    TCXSTR *log = tcxstrnew();
    TCLIST *q1res = ejdbqryexecute(coll, q1, &count, 0, log);
    //fprintf(stderr, "%s", TCXSTRPTR(log));

    //std::cout << count << " records in collection " << collname << std::endl;
    for (int i = 0; i < TCLISTNUM(q1res); ++i)  {
        const char *bsdata = static_cast<const char *>(TCLISTVALPTR(q1res, i));
        char oidhex[25];
        bson_iterator it;
        bson_find_from_buffer(&it, bsdata, "_id" );
        bson_oid_to_string(bson_iterator_oid(&it), oidhex);
        auto current_key = get_key_from_bson(key_fields, bsdata);
        // added record key to list
        setfnc(current_key, oidhex);
    }

    bson_destroy(&bshits1);
    bson_destroy(&bsq1);
    tclistdel(q1res);
    tcxstrdel(log);
    ejdbquerydel(q1);

    close_collection();
}


} // namespace common

#endif
