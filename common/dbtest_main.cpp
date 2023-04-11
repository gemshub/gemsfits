/// Test example the API for manipulating collections and documents into.

#include <iostream>
#include "f_database.h"

int test_CRUD(size_t db_ndx);
int test_query(size_t db_ndx);
int test_query_generator(size_t db_ndx);

int main(int, char* [])
{
#ifdef OLD_EJDB
    std::string dbPath = "EJDB/testdb"; // could be from argument
#else
    std::string dbPath = "EJDB2/test.db";
#endif

    try{
        // add database to test API
        auto test_ndx = rtEJ.size();
        std::vector<std::string> tsKeyFlds = { "fld1", "fld2"};
        rtEJ.push_back(TEJDataBase(test_ndx, "dbapi", tsKeyFlds));

        rtEJ.ChangePath(dbPath);
        rtEJ[test_ndx].OpenDB();

        // Test CRUD commands
        test_CRUD(test_ndx);

        // Test selection query
        test_query(test_ndx);

        // Example generation JQL query from json
        test_query_generator(test_ndx);

        rtEJ.Close();
    }
    catch(TError& e)
    {
        std::cout << e.title << " : " << e.mess <<  std::endl;
    }
    catch(std::exception& e)
    {
        std::cout <<  " std::exception " << e.what() <<  std::endl;
    }
    catch(...)
    {
        std::cout <<  " unknown exception" <<  std::endl;
    }

    return 0;
}

void printData( const std::string& title, const std::vector<std::string>& values )
{
    std::cout <<  title <<  "\n";
    for( const auto& item: values)
        std::cout <<  item <<  "\n";
    std::cout <<  std::endl;
}

void print_all_keys(const std::string& header, size_t db_ndx)
{
    std::vector<std::string> key_list;
    rtEJ[db_ndx].getKeyList("*", key_list);
    printData(header, key_list);
}

int test_CRUD(size_t db_ndx)
{
    std::string documentKey = "eCRUD:test1:";
    std::vector<std::string> key_list;

    print_all_keys("Start CRUD", db_ndx);

    // Create new document to database
    if( !rtEJ[db_ndx].findRecord(documentKey) ) {

        std::string documentData = "{ \"fld1\" : \"eCRUD\", \"fld2\" : \"test1\", "
                                   "  \"task\" : \"exampleCRUD\", "
                                   "  \"properties\" : { \"level\": \"insert record\" } "
                                   "}";

        std::cout << "Create document : " << documentKey << "\n" << documentData <<  std::endl;
        rtEJ[db_ndx].setJson(documentData);
        rtEJ[db_ndx].insertRecord();
    }

    // Read document from database
    rtEJ[db_ndx].getRecord(documentKey);
    std::cout << "Readed document : " << documentKey << "\n" << rtEJ[db_ndx].getJson() <<  std::endl;

    // update document
    auto object = fromJsonString(rtEJ[db_ndx].getJson());
    object["properties"]["level"] = "test update";
    object["properties"]["dvalue"] = 1.5;
    object["properties"]["1value"] = 7;
    std::cout << "Modified document : \n" << object.dump() <<  std::endl;
    rtEJ[db_ndx].setJson(object.dump());
    rtEJ[db_ndx].saveRecord(documentKey);

    // Read record after update
    rtEJ[db_ndx].getRecord(documentKey);
    std::cout << "Updated document : " << documentKey << "\n" << rtEJ[db_ndx].getJson() <<  std::endl;

    // Delete record
    rtEJ[db_ndx].deleteRecord(documentKey);

    print_all_keys("End CRUD", db_ndx);
    return 0;
}

/// Test different query types
int test_query(size_t db_ndx)
{
    // Select from number of documents
    int numOfDocuments =  10;

    // Record keys
    std::vector<std::string> recKeys;
    std::vector<std::string> recjsonValues;

    print_all_keys("Start Query", db_ndx);

    // Insert documents to database
    for( int ii=0; ii<numOfDocuments; ii++ ) {
        auto object = common::JsonFree::object();
        object["fld1"] = "eQuery";
        object["fld2"] = std::string("test_")+std::to_string(ii);
        object["name"] = ii%2 ? "a" : "b";
        object["index"] = ii;
        object["properties"]["task"] = "exampleQuery";
        object["properties"]["value"] = 10.01*ii;
        recKeys.push_back(rtEJ[db_ndx].getKeyFromJson(object));
        if( !rtEJ[db_ndx].findRecord(recKeys.back()) ) {
            rtEJ[db_ndx].setJson(object.dump());
            rtEJ[db_ndx].insertRecord();
        }
    }

    // Test keys query
    print_all_keys("Init Query", db_ndx);

    // Define call back function
    common::SetReaded_f setfnc = [&recjsonValues]( const std::string& jsondata )
    {
        recjsonValues.push_back(jsondata);
    };

    // Load all query
    recjsonValues.clear();
    rtEJ[db_ndx].selectQuery( "", setfnc );
    printData( "Load all query", recjsonValues );

    common::JsonFree query_object;
    query_object["index"]["$bt"] = std::vector<int>{3,6};
    query_object["name"] ="a";
    std::string templ_query = TEJDataBase::dbdriver->generate_query(query_object);

    recjsonValues.clear();
    std::cout << "Query : \n" << templ_query <<  std::endl;
    rtEJ[db_ndx].selectQuery( templ_query, setfnc );
    printData( "Load by query", recjsonValues );

    // delete all
    for( const auto& key: recKeys)
        rtEJ[db_ndx].deleteRecord(key);;

    print_all_keys("End Query", db_ndx);
    return 0;
}

std::string gemsfit_query1 = R"({
     "expdataset" :   {
          "$in" :   [
               "useds1",
               "useds2"
          ],
          "$nin" :   [
               "skipds1",
               "skipds2",
               "skipds3"
          ]
     },
     "sample" :   {
          "$in" :   [
               "usesmp"
          ],
          "$nin" :   [
               "skipsmp1",
               "skipsmp2"
          ]
     },
     "$or" :   [
          {
               "$and" :   [
                    {
                         "expdataset" :   "set1"
                    },
                    {
                         "sample" :   "smp1"
                    }
               ]
          },
          {
               "$and" :   [
                    {
                         "expdataset" :   "set1"
                    },
                    {
                         "sample" :   "smp2"
                    }
               ]
          }
     ]
})";

std::string gemsfit_query2 = R"({
     "expdataset" :   {
          "$in" :   [
               "CH04D",
               "Haas-thesis-CSHeq"
          ]
     },
     "sample" :   {
          "$nin" :   [
               "Cw_06",
               "Cw_11"
          ]
     }
})";

std::string gemsfit_query3 = R"({
     "expdataset" :   {
          "$in" :   [
               "CH04D",
               "Haas-thesis-CSHeq"
          ]
     },
     "sample" :   {
          "$nin" :   [
               "Cw_06",
               "Cw_11"
          ]
     },
     "sT" :   {
          "$in" :   [
               25,
               50,
               100
          ]
     },
     "sP" :   {
          "$bt" :   [
               1,
               1000
          ]
     }
})";

std::string gemsfit_query4 = R"({
     "$or" :   [
          {
               "$and" :   [
                    {
                         "expdataset" :   "set1"
                    },
                    {
                         "sample" :   "smp1"
                    }
               ]
          },
          {
               "$and" :   [
                    {
                         "expdataset" :   "set1"
                    },
                    {
                         "sample" :   "smp2"
                    }
               ]
          }
     ]
})";


int test_query_generator(size_t db_ndx)
{
    std::vector<std::string> recjsonValues;
    // Define call back function
    common::SetReaded_f setfnc = [&recjsonValues]( const std::string& jsondata )
    {
        recjsonValues.push_back(jsondata);
    };

    common::JsonFree query_object2;
    query_object2["index"]["$bt"] = std::vector<int>{3,6};
    query_object2["name"] ="a";
    auto templ_query = TEJDataBase::dbdriver->generate_query(query_object2);
    std::cout << "Json : \n" << query_object2.dump() <<  std::endl;
    std::cout << "Query : \n" << templ_query <<  std::endl;
    // check query
    rtEJ[db_ndx].selectQuery( templ_query, setfnc );

    // Load by query
    // generate query (need test all cases from data_manager $or, $and, $in, $nin, $bt )
    common::JsonFree query_object;
    common::JsonFree condition1;
    condition1["index"]["$bt"] = std::vector<int>{3,6};
    common::JsonFree condition2;
    condition2["name"] ="a";
    query_object["$and"].push_back(std::move(condition1));
    query_object["$and"].push_back(std::move(condition2));
    templ_query = TEJDataBase::dbdriver->generate_query(query_object);
    std::cout << "\nJson : \n" << query_object.dump() <<  std::endl;
    std::cout << "Query : \n" << templ_query <<  std::endl;
    // check query
    rtEJ[db_ndx].selectQuery( templ_query, setfnc );

    auto object1 = fromJsonString(gemsfit_query1);
    templ_query = TEJDataBase::dbdriver->generate_query(object1);
    std::cout << "\nJson : \n" << object1.dump() <<  std::endl;
    std::cout << "Query : \n" << templ_query <<  std::endl;
    // check query
    rtEJ[db_ndx].selectQuery( templ_query, setfnc );

    auto object2 = fromJsonString(gemsfit_query2);
    templ_query = TEJDataBase::dbdriver->generate_query(object2);
    std::cout << "\nJson : \n" << object2.dump() <<  std::endl;
    std::cout << "Query : \n" << templ_query <<  std::endl;
    // check query
    rtEJ[db_ndx].selectQuery( templ_query, setfnc );

    auto object3 = fromJsonString(gemsfit_query3);
    templ_query = TEJDataBase::dbdriver->generate_query(object3);
    std::cout << "\nJson : \n" << object3.dump() <<  std::endl;
    std::cout << "Query : \n" << templ_query <<  std::endl;
    // check query
    rtEJ[db_ndx].selectQuery( templ_query, setfnc );

    auto object4 = fromJsonString(gemsfit_query4);
    templ_query = TEJDataBase::dbdriver->generate_query(object4);
    templ_query = "((/[ expdataset = set1 ]  and /[ sample = smp1 ])  or (/[ expdataset = set1 ]  and /[ sample = smp2 ]))";
    std::cout << "\nJson : \n" << object4.dump() <<  std::endl;
    std::cout << "Query : \n" << templ_query <<  std::endl;
    // check query
    rtEJ[db_ndx].selectQuery( templ_query, setfnc );

    return 0;
}
