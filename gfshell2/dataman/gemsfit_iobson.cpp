//-------------------------------------------------------------------
// $Id: gemsfit_iobson.cpp 333 2015-12-15 12:30:32Z gemsfits $
//
// Read/Write GEMSFIT input configuration file functions
//
// Copyright (C) 2015-2023  S.V.Dmytriyeva, G.D.Miron
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
#include <cstdlib>
#include <sstream>
#include <algorithm>

#include "node.h"
#include "keywords.h"
#include "v_json.h"

//-----------------------------------------------------------------------------------------------------
// from file # include "csvtoejdb.cpp"
using namespace keys;

// cuts the csv header into strings from phase.aq_gen.IC.Al.Q to output = [pahse, aq_gen, IC, Al, Q]
int cut_csv_header (std::string input, std::string delimiter, std::vector<std::string> &output);
// adds Qunit and Qerror to BSON object
bool add_unit_and_error(jsonio::JsonFree& exp, std::vector<std::string> headline, std::vector<std::string> row , unsigned int &position);
// checks if the std::string is the name of a possilbe phase property
bool it_is_phase_property (std::string ph_prop);

bool add_unit_and_error (jsonio::JsonFree& exp, std::vector<std::string> headline, std::vector<std::string> row, unsigned int &position )
{
    double error = 0.0;
    if (position+1< headline.size() && (headline[position+1]==Qerror))
    {
        ++position;
        error = atof(row[position].c_str());
        if (error > 0)
            exp[Qerror] = atof(row[position].c_str());
        if (position+1< headline.size())
            if ((headline[position+1]==Qunit) && (!row[position+1].empty()))
            {
                ++position;
                exp[Qunit] = row[position];
            }
    }
    else
        if (position+1 < headline.size() && (headline[position+1]==Qunit) && (!row[position+1].empty()))
        {
            ++position;
            exp[Qunit] = row[position];
            if (position+1< headline.size())
                if ((headline[position+1]==Qerror)&& (!row[position+1].empty()))
                {
                    ++position;
                    error = atof(row[position].c_str());
                    if (error > 0)
                        exp[Qerror] = atof(row[position].c_str());
                }
        }
    return true;
}

int cut_csv_header (std::string input, std::string delimiter, std::vector<std::string> &output)
{
    int pos_delimiter = 0;
    std::string cut_string;

    output.clear();
    while (pos_delimiter >= 0)
    {
        pos_delimiter = input.find(delimiter);
        if (pos_delimiter >= 0)
        {
        cut_string = input.substr((0),(pos_delimiter));
        input = input.substr((pos_delimiter + 1), (input.size() - pos_delimiter) );
        }
        else cut_string = input;
        output.push_back(cut_string);
    }
    return output.size();
}

bool it_is_phase_property (std::string ph_prop)
{
    if (((ph_prop == Qnt)   || (ph_prop == pH)     || (ph_prop == pHm)     || (ph_prop == pV)  || (ph_prop == Eh)     ||
         (ph_prop == IS)    || (ph_prop == all)    || (ph_prop == sArea)   || (ph_prop == RHO) || (ph_prop == Gex)    || (ph_prop == SI)    ||
         (ph_prop == pe)    || (ph_prop == oscw)   || (ph_prop == mChainL) || (ph_prop == Rd)  || (ph_prop == frAlIV) || (ph_prop == expr)  ||
         (ph_prop == frAlV) || (ph_prop == frAlVI) || (ph_prop == UMC)     || (ph_prop == LMC) || (ph_prop == netH_OH))) return true;
    else return false;
}

bool iequals(const std::string& a, const std::string& b)
{
    unsigned int sz = a.size();
    if (b.size() != sz)
        return false;
    for (unsigned int i = 0; i < sz; ++i)
        if (tolower(a[i]) != tolower(b[i]))
            return false;
    return true;
}

void csvToBson( jsonio::JsonFree& exp, const  std::vector<std::string>& headline, const std::vector<std::string>& row )
{
    //int //ic = 0,      // counts the number of system comp (recipe entries), independent components per pahse, phase properties per pahse, and dependent components properties per dependent component
        //phc = 0,     // counts the number of phases per system/experiment
        //dcc = 0;     // counts the number of dependent components per phase
        //mf = 0;      // counts the number of molar fraction entries per system/exmperiment
    // the following consts define where are the keys / names expected to be in the std::string std::vector resulted form cutting each column header
    const unsigned ndx_comp = 0, ndx_props = 0, ndx_phase = 0, ndx_component_name = 1, ndx_prop_name =1, ndx_phase_name = 1, ndx_phase_property = 2, ndx_IC = 2,
                   ndx_MR = 2, ndx_DC = 2, ndx_IC_name = 3, ndx_MR_formula = 3, ndx_DC_name = 3, ndx_DC_prop_name = 4;
    std::string phase_name, header_delimiter = ".";
    std::vector<std::string> phases, dcomps, header_vector; // keeps the already read phases and dcomps
    bool h_phprop = false, phase_already_added = false, h_phIC = false, dcomp_already_added = false, h_phMR = false, h_phDC = false; // handle that is true if we have the entry in the CSV file
    bool h_phase = false, h_prop=false;
    // getting the data from CSV line by line and processing it into JSON
    // the csv header of each column is cut into strings which are separated by the delimiter ".".
    // Based on this string and their value the JSON oject is constructed.

    //first level objects: sample, expdataset, sT, sP.
    for (unsigned int i=0; i<headline.size(); ++i)
    {
        cut_csv_header(headline[i], header_delimiter, header_vector);
        std::size_t found = headline[i].find(phase);
        if (found!=std::string::npos)
            h_phase = true;
        found = headline[i].find(property);
        if (found == 0 )
            h_prop = true;

        if ((headline[i]==expsample) || (headline[i]==expdataset) || (headline[i]==Tunit) ||
            (headline[i]==Punit)|| (headline[i]==Vunit) || (headline[i]==type) || (headline[i]==comment))
        {
            exp[headline[i]] = row[i];
            // for query
            if (headline[i]==expdataset)
            {
                if (row[i].empty()) { Error( "Loading csv file", "expdataset column cell cannot be empty. Exiting..." );}
            }
            if (headline[i]==expsample)
            {
                if (row[i].empty()) { Error( "Loading csv file", "sample (sample name) column cell cannot be empty. Exiting...");}
            } // end for query
        }
        else if  ((headline[i]==sT) || (headline[i]==sP))
        {
            if (row[i].empty()) { Error( "Loading csv file", "Cells of P and/or T columns cannot be empty. Exiting...");}
            exp[headline[i]] = atof(row[i].c_str());
        }
        else if (headline[i]==sV)
        {
            exp[headline[i]] = atof(row[i].c_str());
        }
        else if (headline[i]==type)
        {
            exp[headline[i]] = row[i];
        }
        else if (headline[i]==comment)
        {
            exp[headline[i]] = row[i];
        }
        else if (headline[i] == Weight)
        {
            if (!row[i].empty())
            {
                exp[headline[i]] = atof(row[i].c_str());
            }
            else
                exp[headline[i]] = 1.0;
        }
     }

    // 2nd level - bulk composition of chemical system for the current experiment
    // array of components
    //++ START array sbcomp ++//
    exp[sbcomp] = jsonio::JsonFree::array();
    for (unsigned int i=0; i<headline.size(); ++i)
    {
        cut_csv_header(headline[i], header_delimiter, header_vector);
        if ((header_vector[ndx_comp] == comp) && (!row[i].empty()))
        {
            auto obj = jsonio::JsonFree::object();
            obj[comp] = header_vector[ndx_component_name];
            obj[Qnt] = atof(row[i].c_str());
            // checking if there are errors and units included in the CSV and adding them in the database
            if (i+1 < headline.size())
            {
                add_unit_and_error(obj, headline, row, i);
            }
            exp[sbcomp].push_back(std::move(obj));
        }
    }
    //++ END array sbcomp ++//

    // 2nd level - properties logQ, logK,... of chemical system for the current experiment
    // array properties
    //++ START array prop ++//
    if (h_prop)
    {
    exp[props] = jsonio::JsonFree::array();
    for (unsigned int i=0; i<headline.size(); ++i)
    {
        cut_csv_header(headline[i], header_delimiter, header_vector);
        if ((header_vector[ndx_props] == property) && (!row[i].empty()))
        {
            auto obj = jsonio::JsonFree::object();
            obj[property] = header_vector[ndx_prop_name];
            obj[Qnt] = atof(row[i].c_str());
            // checking if there are errors and units included in the CSV and adding them in the database
            if (i+1 < headline.size())
            {
                add_unit_and_error(obj, headline, row, i);
            }
            exp[props].push_back(std::move(obj));
        }
    }
    //++ END array props  ++//
    }

    // 2nd level - data for phases charactrised/measured in this experiment
    //++ START array expphases ++//
    if (h_phase)
    {
    exp[expphases] = jsonio::JsonFree::array(); // going trough the headline and searching for "phase" keyword
    for (unsigned int i=0; i<headline.size(); ++i)
    {
        cut_csv_header(headline[i], header_delimiter, header_vector);
        if ((header_vector[ndx_phase] == phase) && (!row[i].empty()))
        {
            phase_name = header_vector[ndx_phase_name];
            phase_already_added = false;

            // check if phase name was not found before
            for (unsigned int j=0; j<phases.size(); ++j)
            {
                if (phase_name == phases[j])
                {
                    phase_already_added = true;
                }
            }

            if (!phase_already_added) // START if not phase_already_added. Every phase is scaned only once troughout out the headline.
            // The program goes trough this if only at the first enocunter of the phase name and scans the document for recuring of the phase.
            {
                auto obj_phase = jsonio::JsonFree::object();                                                            // phases counter increased by 1
                obj_phase[phase] = phase_name;
                phases.push_back(phase_name);                    // std::vector that keeps already present phases

                // going through the header to read the entries related to the phase with phase_name
                for (unsigned int j=0; j<headline.size(); ++j)
                {
                    cut_csv_header(headline[j], header_delimiter, header_vector);
                    // if keyword pahse and phase name are found, and row not empty proceed
                    if ((header_vector[ndx_phase] == phase) && (header_vector[ndx_phase_name] == phase_name) && (!row[j].empty())) // checks where in the headline the same phase name is present
                    {
                        // if property present
                        if ((it_is_phase_property(header_vector[ndx_phase_property])) && (!row[j].empty()))
                        {
                            h_phprop = true;
                        }
                        // if composition present
                        if (header_vector[ndx_IC] == IC)
                        {
                            h_phIC = true;
                        }
                        if (header_vector[ndx_MR] == MR)
                        {
                            h_phMR = true;
                        }
                        if (header_vector[ndx_DC] == DC)
                        {
                            h_phDC = true;
                        }
                    }
                } // END check if there is phprop & phcomp data in the CSV

                //++ START array phprop ++//
                if (h_phprop)
                {
                    obj_phase[phprop] = jsonio::JsonFree::array();
                    // get phase properties
                    for (unsigned int j=0; j<headline.size(); ++j)
                    {
                        cut_csv_header(headline[j], header_delimiter, header_vector);
                        // if keyword phase and phase name match, and it is pahse property and row is not empty proceed.
                        if ((header_vector[ndx_phase] == phase) && (header_vector[ndx_phase_name] == phase_name) &&
                            (it_is_phase_property(header_vector[ndx_phase_property])) && (!row[j].empty()))
                        {
                            auto obj = jsonio::JsonFree::object();                                                            // phases counter increased by 1                                      // increase the number of phases properties with one
                            obj[property] = header_vector[ndx_phase_property];
                            obj[Qnt] = atof(row[j].c_str());
                            // checking if there are errors and units included in the CSV and adding tem in the database
                            if (j+1 < headline.size())
                            {
                                add_unit_and_error(obj, headline, row, j);
                            }

                            obj_phase[phprop].push_back(std::move(obj)); // END property object
                        }
                    }
                    //++ END array phprop ++//
                } h_phprop = false;

                //++ START array phcomp IC ++//
                if (h_phIC)
                {
                    obj_phase[phIC] = jsonio::JsonFree::array();
                    // get phase comp
                    for (unsigned int j=0; j<headline.size(); ++j)
                    {
                        cut_csv_header(headline[j], header_delimiter, header_vector);
                        // if keyword phase, phase name, and IC found in header proceed
                        if ((header_vector[ndx_phase] == phase) && (!row[j].empty()) &&
                            (header_vector[ndx_phase_name] == phase_name) && (header_vector[ndx_IC] == IC ))
                        {
                            auto obj = jsonio::JsonFree::object();
                            // quantity of this IC in the phase
                            obj[IC] = header_vector[ndx_IC_name];
                            obj[Qnt] = strtod(row[j].c_str(), NULL);

                            // checking if there are errors and units included in the CSV and adding them in the database
                            if (j+1 < headline.size())
                            {
                                add_unit_and_error(obj, headline, row, j);
                            }
                            obj_phase[phIC].push_back(std::move(obj)); // END phase element object
                        }
                    }
                    //++ END array phcomp IC ++//
                } h_phIC = false;

                //++ START getting data reported as molar fraction
                if (h_phMR)
                {
                    obj_phase[phMR] = jsonio::JsonFree::array();
                    // get phase MR
                    for (unsigned int j=0; j<headline.size(); ++j)
                    {
                        cut_csv_header(headline[j], header_delimiter, header_vector);
                        // if keyword phase, phase name, and IC found in header proceed
                        if ((header_vector[ndx_phase] == phase) && (!row[j].empty()) &&
                            (header_vector[ndx_phase_name] == phase_name) && (header_vector[ndx_MR] == MR ))
                        {
                            auto obj = jsonio::JsonFree::object();
                            obj[MR] = header_vector[ndx_MR_formula];
                            obj[Qnt] = atof(row[j].c_str());

                            // checking if there are errors and units included in the CSV and adding tem in the database
                            if ((j+1) < headline.size())
                            {
                                add_unit_and_error(obj, headline, row, j);
                            }
                            obj_phase[phMR].push_back(std::move(obj)); // END phase element object
                        }
                    }
                    //++ END array phMR ++//
                } h_phMR = false;

                //++ START array phspecies ++//
                if (h_phDC) // check if there is species data in the CSV header
                {
                    std::string dcomp_name;
                    obj_phase[phDC] = jsonio::JsonFree::array();
                    for (unsigned int j=0; j<headline.size(); ++j)
                    {
                        cut_csv_header(headline[j], header_delimiter, header_vector);
                        // if keyword phase, phase name, and DC found in header proceed
                        if ((header_vector[ndx_phase] == phase) && (!row[j].empty()) &&
                            (header_vector[ndx_phase_name] == phase_name) && (header_vector[ndx_DC] == DC ))
                        {
                            dcomp_name = header_vector[ndx_DC_name];
                            dcomp_already_added    = false;

                            // check if dcomp name was not present before
                            for (unsigned int j=0; j<dcomps.size(); ++j)
                            {
                                if (dcomp_name == dcomps[j])
                                {
                                    dcomp_already_added = true;
                                }
                            }

                            //++ START if h_dcomp
                            if (!dcomp_already_added)
                            {
                                //++ START species object
                                auto spec_obj = jsonio::JsonFree::object();
                                spec_obj[DC] = dcomp_name;
                                dcomps.push_back(dcomp_name);

                                // loop to get all the properties of current dcomp
                                spec_obj[DCprop] = jsonio::JsonFree::array();
                                for (unsigned int k=0; k<headline.size(); ++k)
                                {
                                    cut_csv_header(headline[k], header_delimiter, header_vector);
                                    if ((header_vector[ndx_DC_name] == dcomp_name) && (header_vector[ndx_phase] == phase) &&
                                        (header_vector[ndx_phase_name] == phase_name) && (header_vector[ndx_DC] == DC ) && (!row[k].empty()))
                                    {
                                        auto obj = jsonio::JsonFree::object();
                                        obj[property] = header_vector[ndx_DC_prop_name];
                                        obj[Qnt] = atof(row[k].c_str()); // quantity of the property

                                        // checking if there are errors and units included in the CSV and adding tem in the database
                                        if (k+1 < headline.size())
                                        {
                                            add_unit_and_error(obj, headline, row, k);
                                        }
                                        //++ END dcomp property object
                                        spec_obj[DCprop].push_back(std::move(obj));
                                    } // end search for dcomp name in the headline of the csv file
                                }
                                //++ END dcompprop array
                                obj_phase[phDC].push_back(std::move(spec_obj));
                            } //++ END if h_dcomp // end dcomp onject
                        }
                    } dcomps.clear();
                    //++ END array phspecies ++//
                } h_phDC = false;
                // end object in the array phases
                exp[expphases].push_back(std::move(obj_phase)); //++ END phase object ++
            } // END if phase_already_added
        } // END check for key phase in the headline
    } phases.clear();
    }
    //++ END array expphases ++//
}
// ----------- End of  gemsfit_iobson.cpp ----------------------------
