//-------------------------------------------------------------------
// $Id: f_ejdb.cpp 333 2014-03-13 13:23:32Z gemsfits $
//
// Implementation of TEJDBKey, TEJDataBase and EJDataBaseList classes
//
// Copyright (C) 2014  S.V.Dmytriyeva
// Uses Qwt (http://qwt.sourceforge.net), EJDB (http://ejdb.org),
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

string replace( string str, const char* old_part, const char* new_part)
{
    size_t pos = str.find( old_part );
    if( pos == string::npos )
      return str;

    string res = str.substr(0, pos);
            res += new_part;
            res += str.substr( pos+strlen(old_part));
    return res;
}

string replace_all( string str, const char* old_part, const char* new_part)
{
    size_t pos = str.find( old_part );
    string res = "";

    while( pos != string::npos )
    {
        res += str.substr(0, pos);
        res += new_part;
        str = str.substr( pos+strlen(old_part));
        pos = str.find( old_part );
    }
    return res+str;
}

//-------------------------------------------------------------
// IndexEntry  - Element in sequence of record keys
//-------------------------------------------------------------

bool operator <( const IndexEntry& iEl,  const IndexEntry& iEr)
{
    for( int ii=0; ii<iEl.keyFlds.size(); ii++ )
    {
        if(iEl.keyFlds[ii] == iEr.keyFlds[ii])
          continue;
        return ( iEl.keyFlds[ii] < iEr.keyFlds[ii] ? true: false);
    }
    return false;
}

bool operator >( const IndexEntry& iEl,  const IndexEntry& iEr)
{
    for( int ii=0; ii<iEl.keyFlds.size(); ii++ )
    {
        if(iEl.keyFlds[ii] == iEr.keyFlds[ii])
          continue;
        return ( iEl.keyFlds[ii] > iEr.keyFlds[ii] ? true: false);
    }
    return false;
}

bool operator ==( const IndexEntry& iEl,  const IndexEntry& iEr)
{
    return equal( iEl.keyFlds.begin(), iEl.keyFlds.end(), iEr.keyFlds.begin());
}

bool operator !=( const IndexEntry& iEl,  const IndexEntry& iEr)
{
    return !equal( iEl.keyFlds.begin(), iEl.keyFlds.end(), iEr.keyFlds.begin());
}

//-------------------------------------------------------------
// TEJDBKey  - working with DB record key
//-------------------------------------------------------------

/// Default configuration
TEJDBKey::TEJDBKey( const vector<string>& nameKeyFlds )
{
    for( int ii=0; ii<nameKeyFlds.size(); ii++)
    {   rkFldName.push_back( nameKeyFlds[ii] );
        rkFld.push_back( "*" );
    }
}


/// Copy struct
TEJDBKey::TEJDBKey( const TEJDBKey& dbKey )
{
    for(int ii=0; ii<dbKey.KeyNumFlds(); ii++)
    {
        rkFldName.push_back( dbKey.rkFldName[ii] );
        rkFld.push_back( dbKey.rkFld[ii]  );
    }
    pKey = dbKey.pKey;
}

// Return record key in packed form
const char *TEJDBKey::pack( const vector<string>& akeyFlds )
{
    string sp;
    pKey = "";

    for(int ii=0; ii<KeyNumFlds(); ii++)
    {
        sp = akeyFlds[ii];
        //strip( sp );
        pKey += sp;
        pKey+= ":";
    }
    return pKey.c_str();
}

// Putting DB record key key to internal structure
void TEJDBKey::SetKey( const char *key )
{
    ErrorIf( key==0, "TEJDBKey", "Undefined key of record.");
    rkFld.clear();
    string sp = key;

    if( sp == ALLKEY )
    {
        for(int ii=0; ii<KeyNumFlds(); ii++)
           rkFld.push_back( "*" );
    }
    else
    {
        for(int ii=0; ii<KeyNumFlds(); ii++)
        {
          size_t pos = sp.find_first_of(':');
          if( pos == string::npos )
          {
              if( ii < KeyNumFlds()-1)
                  Error( key, "Invalid packed record key.");
          }
          rkFld.push_back( sp.substr(0, pos) );
          strip( rkFld[ii] );
          if( sp[pos] == ':' )
              pos++;
          sp = sp.substr(pos);
        }
    }
 }

// Change i-th field of TEJDBKey to key
void TEJDBKey::SetFldKey( int i, const char *key )
{
    ErrorIf( i>= KeyNumFlds() || i<0, key, "Invalid key field number");
    string sp = key;
    strip( sp );
    rkFld[i] = sp;
}

/// Check if pattern in record key
bool TEJDBKey::isPattern()
{
  bool OneRec =  true;

  for(int ii=0; ii<KeyNumFlds(); ii++)
      if( rkFld[ii].find_first_of("*?") != std::string::npos )
      {    OneRec = false;
           break;
      }
  return !OneRec;
}

/// Setted ALLKEY
bool TEJDBKey::isAll()
{
  bool AllRecs = true;

  for(int ii=0; ii<KeyNumFlds(); ii++)
      if( rkFld[ii] != "*" )
      {    AllRecs = false;
           break;
      }
  return AllRecs;
}

bool TEJDBKey::compareTemplate( const IndexEntry& elm )
{
  int i, j, rklen;
  string kpart, kelm;

  for( j=0; j<KeyNumFlds(); j++)
    {
        kpart = FldKey(j);
        kelm = elm.getKeyField(j);

        if( kpart == "*" )
            continue; //to next field

        if( kpart.length() > kelm.length() )
            return false;

        rklen = kpart.length();

        for( i=0; i<rklen; i++ )
        {
            if( kpart[i] == '*' ) // next field
              break;
            switch( kpart[i] )
            {
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


/// Default configuration of the Data Base
TEJDataBase::TEJDataBase( int nrt, const char* name, const vector<string>& nameKeyFlds  ):
    Keywd( name ), nRT( nrt ), key( nameKeyFlds ), status( UNDF_)
{
    crt = time(NULL);
    currentSearchJson = "";
}


TEJDataBase::~TEJDataBase()
{
}

/// Get or create collection 'module name' before saving/delete
EJCOLL *TEJDataBase::openCollection( bool createifempty )
{
  // Test and open file  (name of ejdb must be take from nFile)
  EJDBFile.Open();
  if( !EJDBFile.ejDB )
  {
      string err= "Cannot open EJDB "+ EJDBFile.Name();
      Error("TEJDB0011", err ); //return NULL;
  }

  EJCOLL *coll;
  if( createifempty )
  {  // Get or create collection 'module name'
     coll = ejdbcreatecoll(EJDBFile.ejDB, GetKeywd(), NULL );
     if( !coll )
     {
       closeCollection();
       string err= "Cannot open EJDB collection "+ EJDBFile.Name()+GetKeywd();
        Error("TEJDB0012", err );
     }
   }
   else
      coll = ejdbgetcoll(EJDBFile.ejDB, GetKeywd());

   return coll;
}
EJCOLL *TEJDataBase::openCollection2( bool createifempty )
{
    return openCollection(createifempty);
}


/// Close
void TEJDataBase::closeCollection( )
{
    EJDBFile.Close();
}

// end of extern

//--- Manipulation records

// Load data from bson structure
void TEJDataBase::KeyFromBson( const char* bsdata )
{
    // Get key of record
    string keyStr = getKeyFromBson( bsdata );

    char oidhex[25];
    bson_iterator it;
    bson_find_from_buffer(&it, bsdata, "_id" );
    bson_oid_to_string(bson_iterator_oid(&it), oidhex);

    // Try to insert new record to list
    key.SetKey( keyStr.c_str() );
    if( key.isPattern() )
      Error("TEJDB0110", "Cannot save under record key template" );

    pair<set<IndexEntry>::iterator,bool> ret;
    ret = recList.insert( key.retIndex() );
    itrL = ret.first;
    // Test unique keys name before add the record(s)
    if( ret.second == false)
    {
        string erstr = "Cannot add new record:\n";
               erstr += keyStr;
               erstr += ".\nTwo records with the same key!";
        Error("TEJDB0014", erstr );
    }
    itrL->setBsonOid(oidhex);
}

/// put current record key to bson structure
void TEJDataBase::putKeyToBson( bson *obj )
{
    for( int ii=0; ii < KeyNumFlds(); ii++)
       bson_append_string( obj, FldKeyName(ii), FldKey(ii) );
}

// Save current record to bson structure
void TEJDataBase::RecToBson( bson *obj, time_t crtt, const char *pkey )
{
    if( !currentJson.empty() )
    { ParserJson pars;
      pars.setJsonText( currentJson.substr( currentJson.find_first_of('{')+1 ) );
      pars.parseObject(  obj );
    }
      else // yaml
      {
        ParserYAML::parseYAMLToBson( currentYAML, obj );
      }

    // added Modify time
    // bson_append_time_t( obj , "mtime", crtt );

    //get key from object
    string keyStr = getKeyFromBson( obj->data );

    if( pkey )
    {
        // Try to insert new record to list
        key.SetKey( pkey );
        if( key.PackKey() !=  keyStr )
        {
            string mess = " Try to update record with changed key fields\n";
                   mess += " You must use command Insert";
            Error( pkey, mess );
        }
    } else
        key.SetKey( keyStr.c_str() );

    if( key.isPattern() )
      Error("TEJDB0010", "Cannot save under record key template" );

}

/// Get current record key from bson structure
string TEJDataBase::getKeyFromBson( const char* bsdata )
{
    //get key from object
    string keyStr = "", kbuf;
    for(int ii=0; ii<KeyNumFlds(); ii++ )
    {
        if( !bson_find_string( bsdata, key.FldKeyName(ii), kbuf ) )
            kbuf = "*";
        strip( kbuf );
        keyStr += kbuf;
        keyStr += ":";
    }
    return keyStr;
}

// Load data from bson structure
string TEJDataBase::RecFromBson( bson *obj )
{
    // record to json string
    ParserJson pars;
    pars.printBsonObjectToJson( currentJson, obj->data );

    // record to YAML string
    ParserYAML::printBsonObjectToYAML( currentYAML, obj->data );

    // get gems3k name
    if( !bson_find_string( obj->data, keys::G3Ksys[0], currentGems3kName ) )
        if( !bson_find_string( obj->data, keys::G3Ksys[1], currentGems3kName ) )
        currentGems3kName = "";

    // Get key of record
    string keyStr = getKeyFromBson( obj->data );
    return keyStr;
}

// Test text is good bson structure
void TEJDataBase::TestBsonJson( const string& recjson )
{
    ParserJson pars;
    pars.setJsonText( recjson.substr( recjson.find_first_of('{')+1 ) );

    bson obj;
    bson_init( &obj );
    pars.parseObject(  &obj );
    bson_finish( &obj );
    bson_destroy( &obj );
}

// Test text is good yaml(bson) structure
void TEJDataBase::TestBsonYAML( const string& recjson )
{
    bson obj;
    bson_init( &obj );
    ParserYAML::parseYAMLToBson( recjson, &obj );
    bson_finish( &obj );
    bson_destroy( &obj );
}

//Seach record index with key pkey.
bool TEJDataBase::Find( const char *pkey )
{
    if(recList.empty() )
      return false;
    TEJDBKey wkey(key);
    wkey.SetKey( pkey );
    itrL = recList.find( wkey.retIndex() );

    if( itrL == recList.end() )
        return  false;
    else
        return true;
}


// Return curent record in json format string
const string& TEJDataBase::GetJson()
{
     return currentJson;
}


// Set json format string to curent record
void TEJDataBase::SetJson( const string& sjson, bool is_json )
{
    if( is_json )
    { currentJson = sjson;
      currentYAML = "";
    }
    else
    { currentYAML = sjson;
      currentJson = "";
    }

}


// Return curent record in yaml format string
const string& TEJDataBase::GetYAML()
{
     return currentYAML;
}




// Set json format string to curent record
void TEJDataBase::SetQueryJson( const string& qrjson)
{
    currentSearchJson = replace_all( qrjson, "\'", "\"");
}

//Test state of record with key key_ as template.
// in field field setted any(*) data
bool TEJDataBase::FindPart( const char *pkey, int field )
{
    TEJDBKey wkey(key);
    wkey.SetKey( pkey );
    wkey.SetFldKey(field,"*");
    RecStatus iRet = Rtest( wkey.PackKey(), 0 );
    return ( iRet==MANY_ || iRet==ONEF_ );
}

// Retrive one record from the collection
void TEJDataBase::Get( const char *pkey )
{
    key.SetKey( pkey );
    if( key.isPattern() )
      Error("TEJDB0010", "Cannot get under record key template" );

    itrL = recList.find( key.retIndex() );

    if( itrL == recList.end() )
    {
       string erstr = pkey;
              erstr += "\nrecord to retrive does not exist!";
       Error("TEJDB0001", erstr );
    }

    // Get oid of record
    bson_oid_t oid;
    string stroid = itrL->getBsonOid();
    bson_oid_from_string( &oid, stroid.c_str() );

    // Get current collection file ( must be done )
    EJCOLL *coll = openCollection();

    bson *bsrec = ejdbloadbson(coll, &oid);
     // Close database (must be done for exeption )
    closeCollection();

     if( !bsrec )
     {  string errejdb = "Error Loading record ";
               errejdb+= pkey;
               errejdb+= " from EJDB";
        Error( "TEJDB0025",  errejdb );
     }

     // Save bson structure to internal arrays
     RecFromBson( bsrec );
     bson_destroy(bsrec);

    // Set up internal data
    status = ONEF_;
}

/// Removes record from the collection
void TEJDataBase::Del( const char *pkey )
{
    key.SetKey( pkey );
    if( key.isPattern() )
      Error("TEJDB0010", "Cannot delete under record key template" );

    itrL = recList.find( key.retIndex() );

    if( itrL == recList.end() )
    {
       string erstr = pkey;
              erstr+= "\nrecord to delete does not exist!";
       Error("TEJDB0002", erstr );
    }

    // Get oid of record
    bson_oid_t oid;
    bson_oid_from_string( &oid, itrL->getBsonOid().c_str() );

    // Get current collection file
    EJCOLL *coll = openCollection();

    // Remove BSON object from collection.
    bool iRet = ejdbrmbson( coll,  &oid );

    // Close database (must be done for exeption )
    closeCollection( );

    if( !iRet )
    {  string errejdb = "Error deleting of record ";
              errejdb+= pkey;
              errejdb+= " from EJDB";
       Error( "TEJDB0024",  errejdb );
    }

    // Set up internal data
    recList.erase(itrL);
    itrL = recList.end();
    status = UNDF_;
}

/// Save new record in the collection
void TEJDataBase::InsertRecord()
{
    bson bsrec;

    bson_init( &bsrec );
    // Get bson structure from internal string
    RecToBson( &bsrec, time(NULL) );
    bson_finish( &bsrec );

    pair<set<IndexEntry>::iterator,bool> ret;
    ret = recList.insert( key.retIndex() );
    itrL = ret.first;
    // Test unique keys name before add the record(s)
    if( ret.second == false)
    {
        string erstr = "Cannot insert record:\n";
               erstr += key.PackKey();
               erstr += ".\nTwo records with the same key!";
        Error("TEJDB0004", erstr );
    }

    // Get current collection file ( must be done )
    EJCOLL *coll = openCollection();

    // Persist BSON object in the collection
     char bytes[25];
     bson_oid_t oid;
     bool retSave = ejdbsavebson(coll, &bsrec, &oid);
     // Close database (must be done for exeption )
     closeCollection();
     if( !retSave )
     {  string errejdb = bson_first_errormsg(&bsrec);
        bson_destroy(&bsrec);
        recList.erase(itrL);
        Error( "TEJDB0021",  errejdb );
      }
      else
        {
           bson_oid_to_string( &oid, bytes );
          itrL->setBsonOid(bytes);
          /// putndx(nF); work with indexes
          bson_destroy(&bsrec);
        }

    cout << "Add record " << retSave << " oid " << bytes[25] << endl;

    // Set up internal data
    status = ONEF_;
}

/// Save/update record in the collection
void TEJDataBase::SaveRecord(const char* pkey )
{
   bson_oid_t oid;
   bson bsrec;

   bson_init( &bsrec );
   // Get bson structure from internal string
   RecToBson( &bsrec, time(NULL), pkey );

   pair<set<IndexEntry>::iterator,bool> ret;
   ret = recList.insert( key.retIndex() );
   itrL = ret.first;

   if( ret.second == true ) // new record
   {
      // Get bson structure from internal arrays
    }
    else  // update record in the collection
       {
           bson_oid_from_string( &oid, itrL->getBsonOid().c_str() );
           bson_append_oid( &bsrec, JDBIDKEYNAME, &oid);
       }
   bson_finish( &bsrec );

   EJCOLL *coll = openCollection();

     // Persist BSON object in the collection
   bool retSave = ejdbsavebson(coll, &bsrec, &oid);
      // Close database (must be done for exeption )
    closeCollection();

   if( !retSave )
     {  string errejdb = bson_first_errormsg(&bsrec);
        bson_destroy(&bsrec);
        if( ret.second == true )
              recList.erase(itrL);
        Error( "TEJDB0022",  errejdb );
      }

      if( ret.second == true ) // new record
        {   char bytes[25];
            bson_oid_to_string( &oid, bytes );
            itrL->setBsonOid(bytes);
        }
     bson_destroy(&bsrec);
     cout << "Saving record " << retSave <<
                 " oid " << itrL->getBsonOid().c_str() << endl;

     // Set up internal data
     status = ONEF_;
}

/// Save/update record in the collection
/// Question for replase
void TEJDataBase::SaveRecordQuestion(const char* pkey, bool& yesToAll )
{
    if( !yesToAll && Find( pkey ) )
    {
      switch( vfQuestion3( pFitImp, pkey,
                         "Data record with this key already exists! Replace?",
                          "&Yes", "&No", "&Yes to All" ))
      {
         case VF3_3: yesToAll=true;
         case VF3_1: break;
         case VF3_2: return;
      }
    }
    SaveRecord( pkey );
}

// Test state of record with key pkey.
// If mode == 1 and one record, read this record.
// returns state of record
RecStatus TEJDataBase::Rtest( const char *key, int mode )
{
    int iRec;
    const char *pkey;
    bool OneRec = true;

    if( key != 0 && *key )
        pkey = key;
    else
        pkey = PackKey();

    if( strpbrk( pkey, "*?" ))
        OneRec = false; // pattern

    status = UNDF_;

    if( RecCount() <= 0 )
        return EMPC_;

    if( OneRec )
    {
       if( !Find( pkey )  )
            return NONE_;
       if( mode == 1 )
          Get( pkey );
       return ONEF_;
    }
    else // template
    {   vector<string> aKeyList;
        iRec = GetKeyList( pkey, aKeyList, false );
        if( iRec == 0 )
            return NONE_;
        if( iRec == 1 )
        {
            if( mode == 1 )
                Get( aKeyList[0].c_str() );
            return ONEF_;
        }
        return MANY_;
    }
}

int TEJDataBase::GetKeyList( const char *keypat,
        vector<string>& aKeyList, bool retUnpackform )
{
    // Set key template
    TEJDBKey wkey(key);
    wkey.SetKey( keypat );
    aKeyList.clear();

    /* Define functional
    compareTemplate tmpl;
    tmpl.setKey( &wkey );
    tmpl.setForm( retUnpackform );
    // Applies function to each of the elements in the range
    for_each( recList.begin(), recList.end(), tmpl );
    // Copy result
    copy( tmpl.getKeyList().begin(), tmpl.getKeyList().end(), aKeyList.begin() );
    */

    bool OneRec = !wkey.isPattern(),
         AllRecs = wkey.isAll();

    set<IndexEntry, less<IndexEntry> >::iterator it;
    it = recList.begin();
    while( it != recList.end() )
    {
        if( !AllRecs )
            if( !wkey.compareTemplate( *it ) )
              { it++;
                continue;
              }
        aKeyList.push_back( wkey.indexKey( *it ) );
        if( OneRec )
         break;

        it++;
    }
    return aKeyList.size();
}



// make packed key to seach.
void TEJDataBase::MakeKey( unsigned char nRTwrk, string& pkey, ... )
{
    TEJDBKey wkey(key);
    va_list Marker;
    int ii, rts, nkf;
    char *imf;

    va_start( Marker, pkey );
    for( ii=0; ii< wkey.KeyNumFlds(); ii++ )
    {
        rts = va_arg( Marker, int );
        switch( rts )
        {
        case K_END:
            for( ; ii < wkey.KeyNumFlds(); ii++ )
              wkey.SetFldKey( ii, S_ANY );
            break;
        case K_EMP:  // field "`"
            wkey.SetFldKey( ii, S_EMPTY );
            break;
        case K_ANY:  // field  "*"
            wkey.SetFldKey( ii, S_ANY );
            break;
        case K_IMM:  // field in string
            imf = va_arg( Marker, char * );
            wkey.SetFldKey( ii, imf );
            break;
        case K_ACT:  // get field from  PRIE request
            rts = nRTwrk;
        default:     // get field from enathe chain key
            nkf = va_arg( Marker, int );
            if( !*rtEJ[rts].FldKey( nkf ))
                wkey.SetFldKey( ii, S_EMPTY );
            else
                wkey.SetFldKey( ii, rtEJ[rts].FldKey( nkf ) );

            break;
        }
    }
    pkey = wkey.PackKey();
    va_end( Marker );
}



// Working with collections


void TEJDataBase::loadCollection( )
{
    EJCOLL *coll = openCollection( false );

    if( !coll )
    {
        closeCollection();
        return;
    }

    // select all records
    bson bsq1;
    bson_init_as_query(&bsq1);
    if( !currentSearchJson.empty() )
    { ParserJson pars;
      pars.setJsonText( currentSearchJson.substr( currentSearchJson.find_first_of('{')+1 ) );
      pars.parseObject( &bsq1 );
    }
    bson_finish(&bsq1);

    bson bshits1;
    bson_init_as_query(&bshits1);
    bson_append_start_object(&bshits1, "$fields");
    bson_append_int(&bshits1, "_id", 1);
    for(int ii=0; ii<KeyNumFlds(); ii++ )
        bson_append_int(&bshits1, key.FldKeyName(ii), 1);
    bson_append_finish_object(&bshits1);
    bson_finish(&bshits1);

    EJQ *q1 = ejdbcreatequery(EJDBFile.ejDB, &bsq1, NULL, 0, &bshits1 );
    ErrorIf( !q1, "TEJDB0013", "Error in query (test)" );

    uint32_t count = 0;
    TCXSTR *log = tcxstrnew();
    TCLIST *q1res = ejdbqryexecute(coll, q1, &count, 0, log);
    //fprintf(stderr, "%s", TCXSTRPTR(log));

cout << count << " records in collection " << GetKeywd() << endl;
    for (int i = 0; i < TCLISTNUM(q1res); ++i)
    {
        void *bsdata = TCLISTVALPTR(q1res, i);
        //bson_iterator_from_buffer(&it, (char *)bsdata);
        // added record key to list
        KeyFromBson( (const char *)bsdata );
    }

    bson_destroy(&bsq1);
    tclistdel(q1res);
    tcxstrdel(log);
    ejdbquerydel(q1);

    closeCollection( );
}


/// Open EJDB files and build linked record list
void TEJDataBase::Open()
{
    if( EJDBFile.GetPath() == "" )
        return;

    recList.clear();
    itrL= recList.end();
    loadCollection();
}


/// Close files all EJDB files
void TEJDataBase::Close()
{
    EJDBFile.Close();
    recList.clear();
    itrL= recList.end();
}


//-------------------------------------------------------------
// DataBaseList
//-------------------------------------------------------------

int EJDataBaseList::Find(const char* s)
{
    for( int ii=0; ii<size(); ii++ )
        if( at(ii).GetKeywd() == s  )
            return ii;
    return -1;
}

// default configuration
void EJDataBaseList::Init()
{
    // MDF_DATABASE default
    vector<string> dbKeyFlds;
    dbKeyFlds.push_back("sample");
    dbKeyFlds.push_back("expdataset");
    push_back( TEJDataBase( MDF_DATABASE, "experiments", dbKeyFlds  ));

    // MDF_TASK default
    vector<string> tsKeyFlds;
    tsKeyFlds.push_back("taskid");
    tsKeyFlds.push_back("projectid");
    push_back( TEJDataBase( MDF_TASK, "tests", tsKeyFlds  ));

    // MDF_FITS default
    push_back( TEJDataBase( MDF_FITS, "fits", tsKeyFlds  ));
}


TEJDataBase&
EJDataBaseList::operator[](int ii)
{
    ErrorIf( ii > size(),
             "DataBaseList","Invalid chain index.");
    return at(ii);
}


EJDataBaseList rtEJ;

// ------------------------ end of f_ejdb.cpp --------------------------------------
