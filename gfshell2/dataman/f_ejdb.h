#ifndef __v_ejdb_h_
#define __v_ejdb_h_

#include <vector>
#include <set>

#include "f_ejdb_file.h"
#include "v_json.h"

class EJCOLL;

extern const char* ALLKEY;

enum keyctrl {   // codes key bild
    K_END = -5, K_EMP, K_ANY, K_IMM, K_ACT
};

enum RecStatus {   // states of keys record Data Base
    UNDF_=-7 /* undefined state */, FAIL_=-1 /* access error */,
    NONE_=0 /* no records */, ONEF_ ,
    MANY_,  EMPC_ /* empty chain */
};

//enum modeList { openf = 0x01, closef = 0x02, oldself = 0x04 };


/// Element in sequence of record keys
class IndexEntry
{
   vector<string> keyFlds;      /// Record key fields
   mutable string bson_oid;             /// oid of record (bson indexes)
   //mutable int nFile;                   /// Index in db files list

 public:

   IndexEntry( const char* abson_oid, vector<string> akeyFlds):
       bson_oid(abson_oid),  keyFlds(akeyFlds)
   { }

   IndexEntry( vector<string>& akeyFlds ):
       bson_oid("-1"),  keyFlds(akeyFlds)
   { }

   IndexEntry( const IndexEntry& ndxE ):
       bson_oid(ndxE.bson_oid),  keyFlds(ndxE.keyFlds)
   { }

   string getKey( vector<size_t>& rkLen );
   const string& getKeyField( int ii ) const
   { return keyFlds[ii];}

   const string& getBsonOid() const
   { return bson_oid; }
   void setBsonOid( const char* oid ) const
   { bson_oid = oid; }

   friend bool operator <( const IndexEntry&,  const IndexEntry& );
   friend bool operator >( const IndexEntry&,  const IndexEntry& );
   friend bool operator==( const IndexEntry&,  const IndexEntry& );
   friend bool operator!=( const IndexEntry&,  const IndexEntry& );
   friend class TEJDBKey;
};


/// This is struct contened the key of record
class TEJDBKey
{
protected:
    vector<string> rkFldName;    ///< Key fields names
    vector<string> rkFld;        ///< Current key fields
    string pKey;             /// Current key in packed form ( external )

    /// Return record key in packed form
    const char *pack( const vector<string>& akeyFlds );

public:
    TEJDBKey( const vector<string>& nameKeyFlds );
    //TEJDBKey( fstream& f);
    TEJDBKey( const TEJDBKey& dbKey );
    virtual ~TEJDBKey(){}

    /// Return current record key in packed form
    const char *PackKey()
    { return pack( rkFld ); }


    /// Return number of record key fields
    int KeyNumFlds() const
    {  return rkFldName.size();  }

    /// Return record key field i
    const char* FldKey(int i) const
    {   return rkFld[i].c_str(); }
    /// Return record key field name i
    const char* FldKeyName(int i) const
    {   return rkFldName[i].c_str(); }


    //--- Manipulation keys
    /// Putting DB record key pkey to internal structure
    void SetKey( const char *pkey );
    /// Change i-th field of TEJDBKey to fld
    void SetFldKey( int i, const char *fld );

    IndexEntry retIndex()
    { return IndexEntry(rkFld); }
    /// Return index key in form
    const char *indexKey( const IndexEntry& ndx )
    {
          return pack( ndx.keyFlds );
    }
    bool compareTemplate( const IndexEntry& elm );

    // Test work DB record key
    bool isPattern();
    bool isAll();

 //   friend class compareTemplate;
 };


/// This class contened the structure of EJDB Data Base
class TEJDataBase
{
    // Definition of chain
    string Keywd;  /// Name of modules DB
    int nRT;       /// Module number

    /// Definition of record key
    TEJDBKey key;

    // Definition of record key list
    /// Linked records list
    set<IndexEntry, less<IndexEntry> > recList;
    /// Current index in recList
    set<IndexEntry, less<IndexEntry> >::iterator itrL;

    // Work data
    RecStatus status;       /// ? Current states of keys record DB
    time_t crt;
    //bson currentRecord;    ///< last read/save record
    string currentJson;     ///< last read/save record json

protected:

    //void fromCFG(fstream& f);

    //
    EJCOLL *openCollection( bool createifempty = true );
    void closeCollection();
    void loadCollection();
    /// Load records key from bson structure
    void KeyFromBson( const char* bsdata );

 public:

    //  Constructor
    TEJDataBase( int nrt, const char* name, const vector<string>& nameKeyFlds  );
    //TEJDataBase( fstream& f );
    ~TEJDataBase();

    // Save definition of chain to configuration file
    //void toCFG( fstream& f );

    //--- Selectors
    /// Get name of modules DB
    const char* GetKeywd() const
       {  return Keywd.c_str();   }
    /// Set name of modules DB
    void SetKeywd( const string& newKeywd )
       {  Keywd = newKeywd;   }
    /// Get records count in opened files
    int RecCount() const
       {  return recList.size(); }

    /// Get current record status
    RecStatus GetStatus() const
       {  return status;  }
    /// Set current record status
    void SetStatus( RecStatus stt )
       {   status = stt;   }
    /// Time of current record
    time_t Rtime() const
        {  return crt;  }
    time_t GetTime( const char *pkey );

    //--- Selectors for key
    /* Return record key length
    int KeyLen() const
        { return key.KeyLen(); }*/
    /// Return current record key in packed form
    const char *PackKey()
        { return key.PackKey(); }
    /// Return record key field i
    const char *FldKey( int i ) const
        { return key.FldKey(i); }
    /// Return number or record key fields
    int KeyNumFlds() const
        { return key.KeyNumFlds();  }
    /// Access to TEJDBKey
    const TEJDBKey& GetDBKey() const
        { return key; }
    /// Putting DB record key pkey to internal structure
    void SetKey( const char *ckey )
        {  key.SetKey(ckey);
           // status = UNDF_;
        }
    /// Change i-th field of TEJDBKey to key
    void SetFldKey( int i, const char *fld )
        {  key.SetFldKey( i, fld ); }
    /// Return record key field name i
    const char* FldKeyName(int i) const
    {   return key.FldKeyName(i); }
    // Make packed key to seach.
    void MakeKey( unsigned char nRtwrk, string& pkey, ...);

    //--- Manipulation records
    /// Save current record to bson structure
    void RecToBson( bson *obj, time_t crtt, bson_oid_t *oid  = 0 );
    /// Load data from bson structure (return readed record key)
    string RecFromBson( bson *obj );
    /// Test text is good bson structure
    void TestBson( const string& recjson );


    /// Return curent record in json format string
    const string& GetJson();
    /// Set json format string to curent record
    void SetJson( const string& sjson);

    /// Find record with key into internal record keys list
    bool Find( const char *key );
    /// Test state of record with key key_ as template.
    /// in field field setted any(*) data
    bool FindPart( const char *key_, int field );
    /// Retrive one record from the collection
    void Get( const char *key );
    /// Removes record from the collection
    void Del( const char *key );
    /// Save/update record in the collection
    // fnum - index into internal file list
    void SaveRecord( const char* key  );
    /// Save/update record in the collection
    /// Question for replase
    void SaveRecordQuestion(const char* pkey, bool& yesToAll );
    /// Save new record in the collection
    /// Test unique keys name before add the record(s) ???? must be
    // fnum - index into internal file list
    void AddRecord(const char* key );
    /// Test state of record with key pkey.
    /// If mode == 1 and one record, read this record.
    RecStatus Rtest( const char *key, int mode );

    //--- Manipulation list of records
    /// Get key list for a wildcard search
    int GetKeyList( const char *keypat,
        vector<string>& aKeyList, bool retUnpackform = true );

    //--- From module part

    //--- Manipulation Data Base

    void Open( vector<int>& indx);
    void Open( );
    void Close();

};


class EJDataBaseList:
            public vector<TEJDataBase>
{
  public:
    EJDataBaseList()
    {  Init(); }

    ~EJDataBaseList()
    {}

    void Init();
    //void fromCFG( fstream& f );
    //void toCFG(fstream& f);

    //--- Selectors
    TEJDataBase& operator[](int) ;
    int Find(const char* keywd);
};

extern EJDataBaseList rtEJ;

#endif	// __f_ejdb_h_
