#ifndef OLD_EJDB

#include <iostream>
//https://stackoverflow.com/questions/18362315/static-size-of-array-in-c99
#include <ejdb2/ejdb2.h>

#include "db_ejdb2.h"
#include "f_file.h"
#include "f_database.h"
#include "v_service.h"

#ifdef useomp
#include <omp.h>
#endif

std::shared_ptr<common::DBDriverBase> TEJDataBase::dbdriver = std::make_shared<common::Ejdb2DBClient>();

namespace common {

static void ejdb_check_result(iwrc ecode, const std::string& mess)
{
    if (ecode) {
        //iwlog_ecode_error3(ecode);
        Error("EJDB2 storage handle", mess+iwlog_ecode_explained(ecode));
    }
}

/// Class for EJDB2 file manipulation
class TEJDB2: public TDataBaseFile
{

public:
    EJDB ejDB;
    TEJDB2(const std::string& path):
        TDataBaseFile(path), ejDB(nullptr)
    {
        iwrc ecode = ejdb_init();
        ejdb_check_result(ecode, "Error when init ejdb : ");
    }

    ~TEJDB2()
    {
        if(is_opened) {
            //            Close();
        }
    }

    void Open() override
    {
        if(!is_opened) {
            // Test and open file  (name of ejdb must be take from nFile)
            EJDB_OPTS opts = {
                .kv = {
                    .path = path.c_str()
                    //.oflags = IWKV_TRUNC
                },
                .no_wal = true
            };
            iwrc ecode = ejdb_open(&opts, &ejDB);
            ejdb_check_result(ecode, path +" open error :");
        }
        is_opened = true;
    }

    void Close() override
    {
        if(ejDB) {
            iwrc ecode=ejdb_close(&ejDB);
            ejdb_check_result(ecode, path +" close error :");
        }
        is_opened = false;
    }
};

//-------------------------------------------------------------
// EjdbDBClient
//-------------------------------------------------------------

bool Ejdb2DBClient::is_connected() {
    return ( ejdb_db!=nullptr && ejdb_db->ejDB);
}

void Ejdb2DBClient::connect()
{
    if(!ejdb_db) {
        ejdb_db = std::make_unique<TEJDB2>(ejdb_db_path);
    }
    ejdb_db->Open();
    // Test and open file  (name of ejdb must be take from nFile)
    if(!ejdb_db->ejDB) {
        std::string err= "Cannot open EJDB2 "+ ejdb_db->Name();
        Error("TEJDB2021", err );
    }
}

void Ejdb2DBClient::disconnect()
{
    if(is_connected()) {
        ejdb_db->Close();
    }
}

void Ejdb2DBClient::change_path(const std::string &path)
{
    disconnect();
    ejdb_db_path = path;
    if(!ejdb_db) {
        ejdb_db = std::make_unique<TEJDB2>(ejdb_db_path);
    }
    ejdb_db->ChangePath(ejdb_db_path);
}

// EJDB query language (JQL) generation
// https://github.com/Softmotions/ejdb#jql-grammar
// Not full only adapted part for gemsfit Data_Manager::get_EJDB() "DataSelect"
static std::string generate_filter(const common::JsonFree& object, const std::string& path)
{
    std::string gen_data;

    if( object.is_object()) {
        for( const auto& child: object ) {
            if(!gen_data.empty()) {
                gen_data += " and ";
            }
            if(child->get_key() == "$in") {
                gen_data += path + " in ";
                gen_data += child->dump(true);
            }
            else if(child->get_key() == "$nin") {
                gen_data += path + " not in ";
                gen_data += child->dump(true);
            }
            else if(child->get_key() == "$bt" && child->is_array() && child->size() > 1 ) {
                gen_data = path + " >= " + (*child)[0].to_string() + " and ";
                gen_data += path + " <= " + (*child)[1].to_string();
            }
            else {
                gen_data = path + " = " + child->dump(true);
            }
        }
    }
    else {
        gen_data = path + " = " + object.dump(true);
    }
    gen_data = "/[ " + gen_data + " ]";
    return gen_data;
}

static std::string generate_filters(const common::JsonFree& object, const std::string& and_or)
{
    std::string gen_data;

    for( const auto& child: object ) {

        if(!gen_data.empty()) {
            gen_data += and_or;
        }

        if(object.is_array()) {
            gen_data += generate_filters(*child, and_or);
        }
        else if(child->get_key() == "$and") {
            gen_data += "(";
            gen_data += generate_filters(*child, " and ");
            gen_data += ")";
        }
        else if(child->get_key() == "$or") {
            gen_data += "(";
            gen_data += generate_filters(*child, " or ");
            gen_data += ")";
        }
        else  {
            gen_data += "(";
            gen_data += generate_filter(*child, child->get_key());
            gen_data += ")";
        }
    }
    return gen_data;
}

std::string Ejdb2DBClient::generate_query(const JsonFree &object)
{
    return generate_filters(object, " and ");
}


std::string Ejdb2DBClient::create_record(const std::string &collname,
                                         keysmap_t::iterator& it, const std::string& jsonrec)
{
    if(!is_connected()) {
        return "Database not connected";
    }
    std::string errejdb;
    JBL jbl = 0; // Json document
    int64_t id;  // Document id placeholder

    iwrc rc = jbl_from_json(&jbl, jsonrec.c_str());
    RCGO(rc, error);
    rc = ejdb_put_new(ejdb_db->ejDB, collname.c_str(), jbl, &id);
    RCGO(rc, error);
    jbl_destroy(&jbl);
    it->set_id(id);
    return errejdb;
error:
    errejdb = "Create record error: ";
    errejdb += iwlog_ecode_explained(rc);
    if (jbl) {
        jbl_destroy(&jbl);
    }
    return errejdb;
}

// Retrive one record from the collection
bool Ejdb2DBClient::read_record(const std::string& collname, keysmap_t::iterator&it, std::string& jsonrec)
{
    if(!is_connected()) {
        return false;
    }
    JBL jbl = 0; // Json document
    int64_t id = it->get_id();  // Document id
    IWXSTR *xstr = iwxstr_new();
    iwrc rc = ejdb_get(ejdb_db->ejDB, collname.c_str(), id, &jbl);
    RCGO(rc, error);
    // record to json string
    rc = jbl_as_json(jbl, jbl_xstr_json_printer, xstr, JBL_PRINT_PRETTY/*0*/);
    RCGO(rc, error);
    jsonrec = iwxstr_ptr(xstr);
    //std::cout << "Record \n" << jsonrec << std::endl;
    iwxstr_clear(xstr);
    //size_t json_size;
    //rc = jbl_as_json(jbl, jbl_count_json_printer, &json_size, JBL_PRINT_PRETTY);
    //RCGO(rc, error);
    //jsonrec.resize(json_size+1);
    //rc = jbl_as_json(jbl, jbl_count_json_printer, jsonrec.c_str(), JBL_PRINT_PRETTY);
    jbl_destroy(&jbl);
    iwxstr_destroy(xstr);
    return true;
error:
    std::string errejdb = "Read record "+it->pack_key()+" error: ";
    errejdb += iwlog_ecode_explained(rc);
    if (jbl) {
        jbl_destroy(&jbl);
    }
    iwxstr_destroy(xstr);
    Error( "TEJDB2025", errejdb);
    return false;
}

// Removes record from the collection
bool Ejdb2DBClient::delete_record(const std::string& collname, keysmap_t::iterator& itr)
{
    if(!is_connected()) {
        return false;
    }
    int64_t id = itr->get_id();  // Document id
    iwrc rc = ejdb_del(ejdb_db->ejDB, collname.c_str(), id);
    RCGO(rc, error);
    return true;
error:
    std::string errejdb = "Delete record "+itr->pack_key()+" error: ";
    errejdb += iwlog_ecode_explained(rc);
    Error( "TEJDB2024",  errejdb );
    return false;
}

// Save/update record in the collection
std::string Ejdb2DBClient::update_record(const std::string& collname, keysmap_t::iterator& it, const std::string& jsonrec)
{
    if(!is_connected()) {
        return "Database not connected";
    }
    std::string errejdb;
    JBL jbl = 0; // Json document
    int64_t id = it->get_id();  // Document id
    iwrc rc = jbl_from_json(&jbl, jsonrec.c_str());
    RCGO(rc, error);
    rc = ejdb_put(ejdb_db->ejDB, collname.c_str(), jbl, id);
    RCGO(rc, error);
    jbl_destroy(&jbl);
    return errejdb;
error:
    errejdb = "Update record "+it->pack_key()+" error: ";
    errejdb += iwlog_ecode_explained(rc);
    if (jbl) {
        jbl_destroy(&jbl);
    }
    return errejdb;
}


void Ejdb2DBClient::select_query(const std::string& collname, const std::string& query, SetReaded_f setfnc)
{
    if(!is_connected()) {
        return;
    }
    auto internall_query = query;
    if(internall_query.empty()) {
        internall_query = "/*"; // all
    }
    else {
        common::JsonFree query_object = fromJsonString(query);
        internall_query = TEJDataBase::dbdriver->generate_query(query_object);
    }
    std::cout << "Select query " << internall_query <<  std::endl;


    EJDB_LIST list = 0;
    IWXSTR *xstr = iwxstr_new();
    auto rc = ejdb_list2(ejdb_db->ejDB, collname.c_str(), internall_query.c_str(), 0, &list);
    RCGO(rc, error);

    for (EJDB_DOC doc = list->first; doc; doc = doc->next) {
        iwxstr_clear(xstr);
        rc = jbl_as_json(doc->raw, jbl_xstr_json_printer, xstr, JBL_PRINT_PRETTY/*0*/);
        if(!rc) {
            std::string rec_json = iwxstr_ptr(xstr);
            //std::cout << doc->id << " " << rec_json <<  std::endl;
            setfnc(rec_json);
        }
        else {
            std::cout << doc->id << " error " <<  std::endl;
        }
    }
    ejdb_list_destroy(&list);
    iwxstr_destroy(xstr);
    return;

error:
    std::string errejdb = "Select query error: ";
    errejdb += iwlog_ecode_explained(rc);
    iwxstr_destroy(xstr);
    if (list) {
        ejdb_list_destroy(&list);
    }
    Error( "TEJDB2026", errejdb);
}

void Ejdb2DBClient::select_query_omp(const std::string& collname, const std::string& query, SetReaded_f setfnc, int num_threads)
{
    if(!is_connected()) {
        return;
    }
    auto internall_query = query;
    if(internall_query.empty()) {
        internall_query = "/*"; // all
    }
    else {
        common::JsonFree query_object = fromJsonString(query);
        internall_query = TEJDataBase::dbdriver->generate_query(query_object);
    }
    internall_query += " | asc /sample";
    std::cout << "Select query omp " << internall_query <<  std::endl;

    EJDB_LIST list = 0;
    IWXSTR *xstr = iwxstr_new();
    auto rc = ejdb_list2(ejdb_db->ejDB, collname.c_str(), internall_query.c_str(), 0, &list);
    RCGO(rc, error);

#ifdef useomp1
    omp_set_num_threads(num_threads);
#ifdef buildWIN32
    #pragma omp parallel for schedule(static)
#else
    #pragma omp parallel for schedule(dynamic)
#endif
#endif
    for (EJDB_DOC doc = list->first; doc; doc = doc->next) {
        iwxstr_clear(xstr);
        rc = jbl_as_json(doc->raw, jbl_xstr_json_printer, xstr, JBL_PRINT_PRETTY/*0*/);
        if(!rc) {
            std::string rec_json = iwxstr_ptr(xstr);
            //std::cout << doc->id << " " << rec_json <<  std::endl;
            setfnc(rec_json);
        }
        else {
            std::cout << doc->id << " error " <<  std::endl;
        }
    }
    ejdb_list_destroy(&list);
    iwxstr_destroy(xstr);
    return;

error:
    std::string errejdb = "Select query error: ";
    errejdb += iwlog_ecode_explained(rc);
    iwxstr_destroy(xstr);
    if (list) {
        ejdb_list_destroy(&list);
    }
    Error( "TEJDB2027", errejdb);
}


void Ejdb2DBClient::all_query(const std::string& collname, const std::string& query,
                              const std::vector<std::string>& key_fields, SetReadedKey_f setfnc)
{
    if(!is_connected()) {
        return;
    }

    auto internall_query = query;
    if(internall_query.empty()) {
        internall_query = "/*"; // all
    }
    // projection (now not work, but must look like)
    if(key_fields.size()>0) {
        internall_query += " | /{";
        for(const auto& key_fls: key_fields) {
            if(internall_query.back()!='{'){
                internall_query += ",";
            }
            internall_query += key_fls;
        }
        internall_query += "}";
    }
    std::cout << "All query " << internall_query <<  std::endl;

    EJDB_LIST list = 0;
 //   IWXSTR *xstr = iwxstr_new();
    auto rc = ejdb_list2(ejdb_db->ejDB, collname.c_str(), internall_query.c_str(), 0, &list);
    RCGO(rc, error);

    for (EJDB_DOC doc = list->first; doc; doc = doc->next) {
        std::string current_key;
        const char *ptr;
        for(const auto& key_fld: key_fields) {
            iwrc rc = jbl_object_get_str(doc->raw, key_fld.c_str(), &ptr);
            if(!rc && ptr) {
                current_key += ptr;
                strip(current_key);
                current_key += ":";
            }
            else {
                current_key += "*:";
            }
        }
        // added record key to list
        setfnc(current_key, doc->id);

//        // temporally to test
//        iwxstr_clear(xstr);
//        rc = jbl_as_json(doc->raw, jbl_xstr_json_printer, xstr, JBL_PRINT_PRETTY/*0*/);
//        if(!rc) {
//            std::string rec_json = iwxstr_ptr(xstr);
//            std::cout << doc->id << " " << rec_json <<  std::endl;
//        }
//        else {
//            std::cout << doc->id << " error " <<  std::endl;
//        }
    }
    ejdb_list_destroy(&list);
//    iwxstr_destroy(xstr);
    return;

error:
    std::string errejdb = "Select query error: ";
    errejdb += iwlog_ecode_explained(rc);
//    iwxstr_destroy(xstr);
    if (list) {
        ejdb_list_destroy(&list);
    }
    Error( "TEJDB2028", errejdb);
}


} // namespace common

#endif
