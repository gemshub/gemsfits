//-------------------------------------------------------------------
// $Id: help.h 333 2014-03-13 13:23:32Z gemsfits $
//
// Declaration of HelpConfigurator class
//
// Copyright (C) 2010-2023  S.V.Dmytriyeva
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
//-----------------------------------------------------------------

#ifndef __help_h
#define __help_h

#include <string>
#include <vector>
#include <set>
#include <map>

class HelpConfigurator
{
public:
    HelpConfigurator(){}

    int readDir(const std::string& resources_dir);
    int writeFile(const std::string& file="");

protected:

    // only for test
    std::vector<std::string> hrefs;
    std::vector<std::string> names;
    std::vector<std::string> others;

    std::string resource_path;
    std::string html_path;
    std::multimap<std::string, std::string> links;
    std::set<std::string> images;
    std::set<std::string> files;

    void get_hrefs(const std::string& file, const std::string& file_name);
    void u_getline(std::istream& is, std::string& str, const std::string& end);
    void add_name(const std::string& ref, const std::string& file_name );
    void add_img(const std::string& ref);

    void write_files(std::fstream& f_out);
    void write_keywords(std::fstream& f_out);
    void write_content(std::fstream& f_out);

};



#endif  //__help_h
