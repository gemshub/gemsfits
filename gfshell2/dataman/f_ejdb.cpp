//-------------------------------------------------------------------
// $Id: f_ejdb.cpp 333 2014-03-13 13:23:32Z gemsfits $
//
// Implementation of TEJDBKey, TEJDataBase and EJDataBaseList classes
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

#include "f_ejdb.h"
#include "FITMainWindow.h"
#include "v_yaml.h"
#include "v_service.h"
#include "keywords.h"
#include "yaml-cpp/emitfromevents.h"

const char* ALLKEY="*";

/*
std::string replace(std::string str, const char* old_part, const char* new_part)
{
    size_t pos = str.find(old_part);
    if(pos == std::string::npos)
        return str;

    std::string res = str.substr(0, pos);
    res += new_part;
    res += str.substr(pos+strlen(old_part));
    return res;
}

std::string replace_all( std::string str, const char* old_part, const char* new_part)
{
    size_t pos = str.find(old_part);
    std::string res = "";

    while(pos != std::string::npos) {
        res += str.substr(0, pos);
        res += new_part;
        str = str.substr( pos+strlen(old_part));
        pos = str.find( old_part );
    }
    return res+str;
}*/

//-------------------------------------------------------------
// IndexEntry  - Element in sequence of record keys
//-------------------------------------------------------------

bool operator <( const IndexEntry& iEl,  const IndexEntry& iEr)
{
    for( size_t ii=0; ii<iEl.key_flds.size(); ii++ ) {
        if(iEl.key_flds[ii] == iEr.key_flds[ii])
            continue;
        return (iEl.key_flds[ii] < iEr.key_flds[ii] ? true: false);
    }
    return false;
}

bool operator >( const IndexEntry& iEl,  const IndexEntry& iEr)
{
    for( size_t ii=0; ii<iEl.key_flds.size(); ii++ ) {
        if(iEl.key_flds[ii] == iEr.key_flds[ii])
            continue;
        return (iEl.key_flds[ii] > iEr.key_flds[ii] ? true: false);
    }
    return false;
}

bool operator ==( const IndexEntry& iEl,  const IndexEntry& iEr)
{
    return equal(iEl.key_flds.begin(), iEl.key_flds.end(), iEr.key_flds.begin());
}

bool operator !=( const IndexEntry& iEl,  const IndexEntry& iEr)
{
    return !(iEl==iEr);
}

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

bool TEJDBKey::compareTemplate( const IndexEntry& elm )
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
void TEJDataBase::setJson( const std::string& sjson, bool is_json )
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
#ifdef OLD_EJDB
    current_JQL = query_str;
    replace_all(current_JQL, "\'", '\"');
#else
#endif
}

std::string TEJDataBase::getKeyFromJson(const jsonio::JsonFree& bsdata)
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


void TEJDataBase::addKeyToJson(jsonio::JsonFree& object)
{
    for(size_t ii=0; ii<keySize(); ii++) {
        object[key.keyFieldName(ii)] = key.keyField(ii);
    }
}

// Save current record to json structure
jsonio::JsonFree TEJDataBase::current_to_json(const std::string& pkey)
{
    jsonio::JsonFree object;
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
    current_YAML = yaml::dump(object);

    current_Gems3k_name.clear();
    current_Gems3k_name = object.value(keys::G3Ksys[1], current_Gems3k_name);
    current_Gems3k_name = object.value(keys::G3Ksys[0], current_Gems3k_name);

    return getKeyFromJson(object);
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

    auto stroid = current_ndx->get_id();
    std::string readed_json;

#ifdef OLD_EJDB
    // Get oid of record
    bson_oid_t oid;
    bson_oid_from_string( &oid, stroid.c_str() );

    EJCOLL *coll = open_collection();
    bson *bsrec = ejdbloadbson(coll, &oid);
    close_collection();

    if(!bsrec){
        std::string errejdb = "Error Loading record ";
        errejdb+= pkey;
        errejdb+= " from EJDB";
        Error( "TEJDB0025",  errejdb );
    }

    // record to json std::string
    ParserJson pars;
    pars.printBsonObjectToJson(readed_json, bsrec->data);
    bson_destroy(bsrec);
#else

#endif
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
#ifdef OLD_EJDB
    bson_oid_t oid;
    bson_oid_from_string( &oid, current_ndx->get_id().c_str() );

    EJCOLL *coll = open_collection();
    // Remove BSON object from collection.
    bool iRet = ejdbrmbson( coll,  &oid );
    close_collection();

    if( !iRet ) {
        std::string errejdb = "Error deleting of record ";
        errejdb+= pkey;
        errejdb+= " from EJDB";
        Error( "TEJDB0024",  errejdb );
    }
#else

#endif
    // Set up internal data
    records.erase(current_ndx);
    current_ndx = records.end();
}

// Save new record in the collection
void TEJDataBase::insertRecord()
{
    // Get json structure from internal std::string
    auto object = current_to_json("");

    std::pair<std::set<IndexEntry>::iterator,bool> ret;
    ret = records.insert(key.retIndex());
    current_ndx = ret.first;
    // Test unique keys name before add the record(s)
    if( ret.second == false)  {
        std::string erstr = "Cannot insert record:\n";
        erstr += key.packKey();
        erstr += ".\nTwo records with the same key!";
        Error("TEJDB0004", erstr );
    }

#ifdef OLD_EJDB
    bson bsrec;

    bson_init( &bsrec );
    // Get bson structure from internal std::string
    ParserJson pars;
    pars.setJsonText( current_Json.substr( current_Json.find_first_of('{')+1 ) );
    pars.parseObject(&bsrec);
    bson_finish( &bsrec );

    EJCOLL *coll = open_collection();
    // Persist BSON object in the collection
    char bytes[25];
    bson_oid_t oid;
    bool retSave = ejdbsavebson(coll, &bsrec, &oid);
    close_collection();
    if( !retSave ) {
        std::string errejdb = bson_first_errormsg(&bsrec);
        bson_destroy(&bsrec);
        records.erase(current_ndx);
        Error( "TEJDB0021",  errejdb );
    }
    else  {
        bson_oid_to_string( &oid, bytes );
        current_ndx->set_id(bytes);
        /// putndx(nF); work with indexes
        bson_destroy(&bsrec);
    }
#else

#endif
    std::cout << "Add record " << retSave << " oid " << bytes << std::endl;
}

// Save/update record in the collection
void TEJDataBase::saveRecord(const std::string& pkey)
{
    auto object = current_to_json(pkey);
    std::pair<std::set<IndexEntry>::iterator,bool> ret;
    ret = records.insert(key.retIndex());
    current_ndx = ret.first;

#ifdef OLD_EJDB
    bson_oid_t oid;
    bson bsrec;

    bson_init( &bsrec );
    // Get bson structure from internal std::string
    ParserJson pars;
    pars.setJsonText( current_Json.substr( current_Json.find_first_of('{')+1 ) );
    pars.parseObject(&bsrec);
    if( ret.second != true ) {
        bson_oid_from_string(&oid, current_ndx->get_id().c_str());
        bson_append_oid( &bsrec, JDBIDKEYNAME, &oid);
    }
    bson_finish( &bsrec );

    EJCOLL *coll = open_collection();
    // Persist BSON object in the collection
    bool retSave = ejdbsavebson(coll, &bsrec, &oid);
    close_collection();

    if( !retSave ) {
        std::string errejdb = bson_first_errormsg(&bsrec);
        bson_destroy(&bsrec);
        if( ret.second == true )
            records.erase(current_ndx);
        Error( "TEJDB0022",  errejdb );
    }

    if( ret.second == true )  { // new record
        char bytes[25];
        bson_oid_to_string( &oid, bytes );
        current_ndx->set_id(bytes);
    }
    bson_destroy(&bsrec);
#else

#endif
    std::cout << "Saving record " << retSave << " oid " << current_ndx->get_id() << std::endl;
}

// Save/update record in the collection
// Question for replase
void TEJDataBase::saveRecordQuestion(const std::string& pkey, bool& yes_to_all)
{
    if(!yes_to_all && findRecord(pkey))  {
        switch( vfQuestion3( pFitImp, pkey,
                             "Data record with this key already exists! Replace?",
                             "&Yes", "&No", "&Yes to All" ))
        {
        case VF3_3: yes_to_all=true;
        case VF3_1: break;
        case VF3_2: return;
        }
    }
    saveRecord(pkey);
}

int TEJDataBase::getKeyList(const std::string& keypat, std::vector<std::string>& key_list)
{
    // Set key template
    TEJDBKey wkey(key);
    wkey.setKey( keypat );
    key_list.clear();

    bool OneRec = !wkey.is_pattern(),
            AllRecs = wkey.is_all();

    std::set<IndexEntry, std::less<IndexEntry> >::iterator it;
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
#ifdef OLD_EJDB
    EJCOLL *coll = open_collection(false);
    if( !coll ) {
        close_collection();
        return;
    }

    // select all records
    bson bsq1;
    bson_init_as_query(&bsq1);
    if( !current_JQL.empty() )
    {
        ParserJson pars;
        pars.setJsonText( current_JQL.substr(current_JQL.find_first_of('{')+1 ));
        pars.parseObject( &bsq1 );
    }
    bson_finish(&bsq1);

    bson bshits1;
    bson_init_as_query(&bshits1);
    bson_append_start_object(&bshits1, "$fields");
    bson_append_int(&bshits1, "_id", 1);
    for(size_t ii=0; ii<keySize(); ii++ )
        bson_append_int(&bshits1, key.keyFieldName(ii).c_str(), 1);
    bson_append_finish_object(&bshits1);
    bson_finish(&bshits1);

    EJQ *q1 = ejdbcreatequery(EJDBFile.ejDB, &bsq1, NULL, 0, &bshits1 );
    ErrorIf( !q1, "TEJDB0013", "Error in query (test)" );

    uint32_t count = 0;
    TCXSTR *log = tcxstrnew();
    TCLIST *q1res = ejdbqryexecute(coll, q1, &count, 0, log);
    //fprintf(stderr, "%s", TCXSTRPTR(log));

    std::cout << count << " records in collection " << getKeywd() << std::endl;
    for (int i = 0; i < TCLISTNUM(q1res); ++i)
    {
        void *bsdata = TCLISTVALPTR(q1res, i);
        //bson_iterator_from_buffer(&it, (char *)bsdata);
        // added record key to list
        keyFromBson((const char *)bsdata);
    }

    bson_destroy(&bsq1);
    tclistdel(q1res);
    tcxstrdel(log);
    ejdbquerydel(q1);

    close_collection();
#else

#endif
}


// write TP paris file
void TEJDataBase::selectQuery(const std::string& query, SetReaded_f setfnc)
{
#ifdef OLD_EJDB
    EJCOLL *coll = open_collection(false);
    if( !coll ) {
        close_collection();
        return;
    }

    bson bsq1;
    bson_init_as_query(&bsq1);
    if( !query.empty() )
    {
        ParserJson pars;
        pars.setJsonText( query.substr(query.find_first_of('{')+1 ));
        pars.parseObject( &bsq1 );
    }
    bson_finish(&bsq1);

    EJQ *q = ejdbcreatequery(EJDBFile.ejDB, &bsq1, NULL, 0, NULL);
    ErrorIf( !q, "TEJDB0015", "Error in query (test)" );
    uint32_t count = 0;
    TCXSTR *log = tcxstrnew();
    TCLIST *qres = ejdbqryexecute(coll, q, &count, 0, log);
    //fprintf(stderr, "%s", TCXSTRPTR(log));
    std::cout << count << " records in collection " << getKeywd() << std::endl;

    for (int i = 0; i < TCLISTNUM(qres); ++i)
    {
        void *bsdata = TCLISTVALPTR(qres, i);
        const char *bsdata_ = static_cast<const char*>(bsdata);
        setfnc(bsdata_);
    }

    tclistdel(qres);
    tcxstrdel(log);
    ejdbquerydel(q);
    bson_destroy(&bsq1);

    close_collection();
#else

#endif
}



// Open EJDB files and build linked record list
void TEJDataBase::Open()
{
    if( EJDBFile.getPath() == "" )
        return;

    records.clear();
    current_ndx= records.end();
    load_collection();
}

/// Close files all EJDB files
void TEJDataBase::Close()
{
    EJDBFile.Close();
    records.clear();
    current_ndx = records.end();
}



#ifdef OLD_EJDB

/// Get or create collection 'module name' before saving/delete
EJCOLL *TEJDataBase::open_collection(bool createifempty)
{
    // Test and open file  (name of ejdb must be take from nFile)
    EJDBFile.Open();
    if(!EJDBFile.ejDB) {
        std::string err= "Cannot open EJDB "+ EJDBFile.Name();
        Error("TEJDB0011", err ); //return NULL;
    }

    EJCOLL *coll;
    if( createifempty ) {  // Get or create collection 'module name'
        coll = ejdbcreatecoll(EJDBFile.ejDB, getKeywd().c_str(), NULL );
        if( !coll )
        {
            close_collection();
            std::string err= "Cannot open EJDB collection "+ EJDBFile.Name()+getKeywd();
            Error("TEJDB0012", err );
        }
    }
    else
        coll = ejdbgetcoll(EJDBFile.ejDB, getKeywd().c_str());

    return coll;
}

void TEJDataBase::close_collection( )
{
    EJDBFile.Close();
}

std::string TEJDataBase::getKeyFromBson(const char* bsdata)
{
    //get key from object
    std::string keyStr = "", kbuf;
    for(size_t ii=0; ii<keySize(); ii++ ) {
        if( !bson_find_string( bsdata, key.keyFieldName(ii).c_str(), kbuf ) )
            kbuf = "*";
        strip(kbuf);
        keyStr += kbuf;
        keyStr += ":";
    }
    return keyStr;
}

// Load data from bson structure
void TEJDataBase::keyFromBson(const char* bsdata)
{
    // Get key of record
    std::string keyStr = getKeyFromBson( bsdata );

    char oidhex[25];
    bson_iterator it;
    bson_find_from_buffer(&it, bsdata, "_id" );
    bson_oid_to_string(bson_iterator_oid(&it), oidhex);

    // Try to insert new record to list
    key.setKey(keyStr);
    if( key.is_pattern() )
        Error("TEJDB0110", "Cannot save under record key template" );

    auto ret = records.insert(key.retIndex());
    current_ndx = ret.first;
    // Test unique keys name before add the record(s)
    if( ret.second == false)
    {
        std::string erstr = "Cannot add new record:\n";
        erstr += keyStr;
        erstr += ".\nTwo records with the same key!";
        Error("TEJDB0014", erstr );
    }
    current_ndx->set_id(oidhex);
}

#endif

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


TEJDataBase& EJDataBaseList::operator[](size_t ii)
{
    ErrorIf( ii > size(), "DataBaseList","Invalid chain index.");
    return at(ii);
}


EJDataBaseList rtEJ;

// ------------------------ end of f_ejdb.cpp --------------------------------------
