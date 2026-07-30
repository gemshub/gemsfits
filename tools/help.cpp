//-------------------------------------------------------------------
// $Id: help.h 333 2014-03-13 13:23:32Z gemsfits $
//
// Implementation of HelpConfigurator class
//
// Copyright (C) 2010-2025  S.V.Dmytriyeva
// Uses EJDB (https://ejdb.org),
//    yaml-cpp (https://code.google.com/p/yaml-cpp/)
//
// This file is part of the GEMSFITS GUI, which uses the
// Qt v.6 cross-platform App & UI framework (http://qt-project.org)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of LGPL v.3 license
//
// See http://gems.web.psi.ch/GEMSFIT for more information
// E-mail gems2.support@psi.ch
//-----------------------------------------------------------------

#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "help.h"
#include "GEMS3K/v_detail.h"
#include "GEMS3K/v_service.h"

const char *HTML_DOC = "Resources/doc/html";

// read and parse all *.html files from dir
int HelpConfigurator::readDir(const std::string& resources_dir)
{
    resource_path = resources_dir;
    html_path = resource_path+HTML_DOC;

    if (!fs::exists(html_path) || !fs::is_directory(html_path)) {
        Error( resources_dir, "Error: Directory does not exist or is not a directory." );
        return 0;
    }

    for (const auto& entry : fs::directory_iterator(html_path)) {
        if (fs::is_regular_file(entry.status())) {
            if (entry.path().extension() == ".html") {
                std::string file_name = entry.path().filename().string();
                get_hrefs(entry.path().string(), file_name);
                files.insert(file_name);
            }
        }
    }
    for (const auto& entry : fs::directory_iterator(html_path+"/img")) {
        if (fs::is_regular_file(entry.status())) {
            if (entry.path().extension() == ".html") {
                std::string file_name = entry.path().filename().string();
                get_hrefs(entry.path().string(), file_name);
                files.insert(file_name);
            }
        }
    }

    return 1;
}

// read string untill "end"
void HelpConfigurator::u_getline(std::istream& is, std::string& str, const std::string& end)
{
    char ch;
    is.get(ch);
    while(!is.eof() && is.good()) {
        str += ch;
        if(str.find(end)!= std::string::npos) {
            break;
        }
        is.get(ch);
    }
}

// read all reference from file
void HelpConfigurator::get_hrefs(const std::string& file, const std::string& file_name)
{
    char ch;
    std::fstream f_in(file, std::ios::in);
    std::string ref;

    if( !f_in.good() ) {
        Error("HelpConfigurator", file+" Fileopen error");
        return;
    }

    while( !f_in.eof() ) {
        f_in.get(ch);
        if(  ch == '<' ) {
            f_in.get(ch);
            if( ch == 'a' ) { // <a name="keywd"></a>
                ref = "<a";
                u_getline(f_in, ref, "</a>" );
                add_name(ref, file_name);
                continue;
            }
            if( ch == 'i') { // <img src="file" ... >
                ref = "<i";
                u_getline(f_in, ref, ">" );
                add_img(ref);
                continue;
            }
        }
    }
}

// read all reference from file
//<a name="tPom_14"></a>
void HelpConfigurator::add_name(const std::string& ref, const std::string& file_name)
{
    std::string key;
    std::string value;

    auto indx = ref.find("name=");
    if(indx != std::string::npos) {
        key = regexp_extract_string(".*name=\\s*\"([^\"]*)\".*", ref);
        value = file_name +"#"+key;
        links.insert({key, value});
        names.push_back(key);
    }
    else {
        hrefs.push_back(ref);
    }
}

// read all images from file
//<img src="gems1.png" align="left" height="48" width="48">
void HelpConfigurator::add_img(const std::string& ref)
{
    std::string value, rref, file_name;
    auto indx = ref.find("src=");
    if(indx != std::string::npos) {
       file_name = regexp_extract_string(".*src=\\s*\"([^\"]*)\".*", ref);
        if(!file_name.empty()) {
           images.insert(file_name);
        }
    }
    else {
        others.push_back(ref);
    }
}

// Build *.qhp file for help system (gfshelpconfig.qhp by default)
int HelpConfigurator::writeFile(const std::string& file)
{
    auto qhp_file=file;
    if(qhp_file.empty()) {
        qhp_file =  html_path+"/gfshelpconfig.qhp";
    }
    std::fstream f_out(qhp_file, std::ios::out);
    if( !f_out.good() ) {
        Error( "HelpConfigurator", qhp_file + " Fileopen error");
        return 0;
    }

    // Write head
    f_out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    f_out << "<QtHelpProject version=\"1.0\">" << std::endl;
    f_out << "    <namespace>gemsfits</namespace>" << std::endl;
    f_out << "    <virtualFolder>help</virtualFolder>" <<std::endl;
    f_out << "    <filterSection>" << std::endl;

    write_content(f_out);
    write_files(f_out);
    write_keywords(f_out);

    // Write end
    f_out << "    </filterSection>" << std::endl;
    f_out << "</QtHelpProject>" << std::endl;
    return 1;
}


void HelpConfigurator::write_files(std::fstream& f_out)
{
    f_out << "    <files>" << std::endl;
    for(const auto& file: files) {
      f_out << "      <file>" << file << "</file>" << std::endl;
    }
    for(const auto& image: images) {
      f_out << "      <file>" << image << "</file>" << std::endl;
    }
    f_out << "    </files>" << std::endl;
}


void HelpConfigurator::write_keywords(std::fstream& f_out)
{
    f_out << "    <keywords>" << std::endl;
    for(const auto& link: links)
    {
        f_out << "        <keyword name=\"" << link.first
              << "\" ref=\"" << link.second  << "\"/>" << std::endl;
    }
    f_out << "    </keywords>" << std::endl;
}

void HelpConfigurator::write_content(std::fstream& f_out)
{
    std::string ref;
    std::string contentfile = html_path + "/gfshelpconfig.toc";
    std::fstream f_in(contentfile, std::ios::in);
    if( !f_in.good() ) {
        Error( "HelpConfigurator", contentfile+" Fileopen error");
        return;
    }
    while( !f_in.eof() ) {
      ref = "";
      u_getline(f_in, ref, ">" );
      f_out << ref;
    }
}



// ----------------- End of help.cpp -----------------------

