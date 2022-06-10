//-------------------------------------------------------------------
// $Id: v_yaml.h  $
//
// Declaration of ParserYAML class
//
// Copyright (C) 2015  S.V.Dmytriyeva
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

#ifndef V_YAML_H
#define V_YAML_H

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <iostream>
using namespace std;

#include "ejdb.h"
#include "yaml-cpp/yaml.h"
#include "yaml-cpp/eventhandler.h"

namespace ParserYAML
{
    void parseArray( const YAML::Node& doc, bson* brec );
    void parseScalar(const char* key, const YAML::Node& doc, bson* brec );
    void parseObject( const YAML::Node& doc, bson* brec );

    void bson_emitter( YAML::Emitter& out, const char *data, int datatype );

    /// Print bson structure to YAML string
    void printBsonObjectToYAML( string& resStr, const char *b);

    /// Print bson structure to YAML format file
    void printBsonObjectToYAML(fstream& fout, const char *b);

    /// Read one YAML object from text file and parse to bson structure
    void parseYAMLToBson( fstream& fin, bson *brec );

    /// Parse one YAML object from string to bson structure
    void parseYAMLToBson( const string& currentYAML, bson *obj );

};


class BsonHandler: public YAML::EventHandler
{

 public:

  string to_string();

  BsonHandler(bson* bobj);

  virtual void OnDocumentStart(const YAML::Mark& mark);
  virtual void OnDocumentEnd();

  virtual void OnNull(const YAML::Mark& mark, YAML::anchor_t anchor);
  virtual void OnAlias(const YAML::Mark& mark, YAML::anchor_t anchor);
  virtual void OnScalar(const YAML::Mark& mark, const std::string& tag,
                        YAML::anchor_t anchor, const std::string& value);

  virtual void OnSequenceStart(const YAML::Mark& mark, const std::string& tag,
                               YAML::anchor_t anchor, YAML::EmitterStyle::value style);
  virtual void OnSequenceEnd();

  virtual void OnMapStart(const YAML::Mark& mark, const std::string& tag,
                          YAML::anchor_t anchor, YAML::EmitterStyle::value style);
  virtual void OnMapEnd();

 private:

  void BeginNode();
  //void EmitProps(const std::string& tag, YAML::anchor_t anchor);

 private:
  bson* m_bson;

  enum   STATE_ { WaitingForSequenceEntry, WaitingForKey, WaitingForValue, };

  struct State {
    string key;
    int    ndx;
    STATE_ state;

    State( STATE_ st):
        key(""),ndx(0),state(st)
    {}
  };

  std::stack<State> m_stateStack;
};

class JsonHandler: public YAML::EventHandler
{

 void addHead(const string& key );
 void addScalar(const string&  key, const string& value );
 void shift()
 {
   for(int temp = 0; temp < m_depth; temp++)
        m_os << "     ";
 }

 public:

  string to_string();

  JsonHandler(stringstream& os_);

  virtual void OnDocumentStart(const YAML::Mark& mark);
  virtual void OnDocumentEnd();

  virtual void OnNull(const YAML::Mark& mark, YAML::anchor_t anchor);
  virtual void OnAlias(const YAML::Mark& mark, YAML::anchor_t anchor);
  virtual void OnScalar(const YAML::Mark& mark, const std::string& tag,
                        YAML::anchor_t anchor, const std::string& value);

  virtual void OnSequenceStart(const YAML::Mark& mark, const std::string& tag,
                               YAML::anchor_t anchor, YAML::EmitterStyle::value style);
  virtual void OnSequenceEnd();

  virtual void OnMapStart(const YAML::Mark& mark, const std::string& tag,
                          YAML::anchor_t anchor, YAML::EmitterStyle::value style);
  virtual void OnMapEnd();

 private:

  void BeginNode();
  //void EmitProps(const std::string& tag, YAML::anchor_t anchor);

 private:
  stringstream& m_os;
  int m_depth =0;
  bool m_first= true;

  enum   STATE_ { WaitingForSequenceEntry, WaitingForKey, WaitingForValue, };

  struct State {
    string key;
    int    ndx;
    STATE_ state;

    State( STATE_ st):
        key(""),ndx(0),state(st)
    {}
  };

  std::stack<State> m_stateStack;
};

string Json2YAML( const string& jsonData );
string parseYAMLToJson( const string& currentYAML );

//#include <iostream>
//template <typename T>
//bool is( T& x, const std::string& s)
//{
//    std::istringstream iss(s);
//    char c;
//    return iss >> x && !iss.ignore();
//}

#endif // V_YAML_H
