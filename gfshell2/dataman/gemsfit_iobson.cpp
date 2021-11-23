//-------------------------------------------------------------------
// $Id: gemsfit_iobson.cpp 333 2015-12-15 12:30:32Z gemsfits $
//
// Read/Write GEMSFIT input configuration file functions
//
// Copyright (C) 2015  S.V.Dmytriyeva, G.D.Miron
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
using namespace std;

#include "node.h"
#include "keywords.h"
#include "v_json.h"


//-----------------------------------------------------------------------------------------------------
// from file # include "csvtoejdb.cpp"
using namespace keys;

// cuts the csv header into strings from phase.aq_gen.IC.Al.Q to output = [pahse, aq_gen, IC, Al, Q]
int cut_csv_header (string input, string delimiter, vector<string> &output);
// input an integer output a const char
const char *int_to_c_str(int in);
// adds Qunit and Qerror to BSON object
bool add_unit_and_error(bson * exp, vector<string> headline, vector<string> row , unsigned &position);
// checks if the string is the name of a possilbe phase property
bool it_is_phase_property (string ph_prop);

bool add_unit_and_error (bson * exp, vector<string> headline, vector<string> row, unsigned &position )
{
    if ((headline[position+1]==Qerror))
    {
        ++position;
        if ((!row[position].empty()))
        {
        bson_append_double(exp, Qerror, atof(row[position].c_str()));
        }
    }
    if ((headline[position+1]==Qunit) && (!row[position+1].empty()))
    {
        ++position;
        bson_append_string(exp, Qunit, row[position].c_str());
    }
}

int cut_csv_header (string input, string delimiter, vector<string> &output)
{
    int pos_delimiter = 0;
    string cut_string;

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

const char * int_to_c_str (int in)
{
    string sss; stringstream ss;
    ss << in;
    sss = ss.str();
    return sss.c_str();
}

bool it_is_phase_property (string ph_prop)
{
    if (((ph_prop == Qnt)   || (ph_prop == pH)     || (ph_prop == pHm)     || (ph_prop == pV)  || (ph_prop == Eh)     ||
         (ph_prop == IS)    || (ph_prop == all)    || (ph_prop == sArea)   || (ph_prop == RHO) || (ph_prop == Gex)    || (ph_prop == SI)    ||
         (ph_prop == pe)    || (ph_prop == oscw)   || (ph_prop == mChainL) || (ph_prop == Rd)  || (ph_prop == frAlIV) || (ph_prop == expr)  ||
         (ph_prop == frAlV) || (ph_prop == frAlVI) || (ph_prop == UMC)     || (ph_prop == LMC))) return true;
    else return false;
}

void csvToBson( bson *exp, const  vector<string>& headline, const vector<string>& row )
{
    int ic = 0,      // counts the number of system comp (recipe entries), independent components per pahse, phase properties per pahse, and dependent components properties per dependent component
        phc = 0,     // counts the number of phases per system/experiment
        dcc = 0,     // counts the number of dependent components per phase
        mf = 0;      // counts the number of molar fraction entries per system/exmperiment
    // the following consts define where are the keys / names expected to be in the string vector resulted form cutting each column header
    const unsigned ndx_comp = 0, ndx_phase = 0, ndx_component_name = 1, ndx_phase_name = 1, ndx_phase_property = 2, ndx_IC = 2,
                   ndx_MR = 2, ndx_DC = 2, ndx_IC_name = 3, ndx_MR_formula = 3, ndx_DC_name = 3, ndx_DC_prop_name = 4;
    string phase_name, header_delimiter = ".";
    vector<string> phases, dcomps, header_vector; // keeps the already read phases and dcomps
    bool h_phprop = false, phase_already_added = false, h_phIC = false, dcomp_already_added = false, h_phMR = false, h_phDC = false; // handle that is true if we have the entry in the CSV file

    // getting the data from CSV line by line and processing it into BSON
    // the csv header of each column is cut into strings which are separated by the delimiter ".". Based on this string and their value the BSON oject is constructed.
    bson_init(exp);

    //first level objects: sample, expdataset, sT, sP.
    for (unsigned int i=0; i<headline.size(); ++i)
    {
        cut_csv_header(headline[i], header_delimiter, header_vector);

        if ((headline[i]==expsample) || (headline[i]==expdataset) || (headline[i]==Tunit) ||
            (headline[i]==Punit)|| (headline[i]==Vunit) || (headline[i]==type) || (headline[i]==comment))
        {
            bson_append_string(exp, headline[i].c_str(), row[i].c_str());
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
            bson_append_double(exp, headline[i].c_str(), atof(row[i].c_str()));
        }
        else if (headline[i]==sV)
        {
            bson_append_double(exp, headline[i].c_str(), atof(row[i].c_str()));
        }
        else if (headline[i]==type)
        {
            bson_append_string(exp, headline[i].c_str(), row[i].c_str());
        }
        else if (headline[i]==comment)
        {
            bson_append_string(exp, headline[i].c_str(), row[i].c_str());
        }
        else if (headline[i] == Weight)
        {
            if (!row[i].empty())
            {
                bson_append_double(exp, headline[i].c_str(), atof(row[i].c_str()));
            } else bson_append_double(exp, headline[i].c_str(), 1.0);
        }
     }

    // 2nd level - bulk composition of chemical system for the current experiment
    // array of components
    //++ START array sbcomp ++//
    bson_append_start_array(exp, sbcomp);
    for (unsigned int i=0; i<headline.size(); ++i)
    {
        cut_csv_header(headline[i], header_delimiter, header_vector);
        if ((header_vector[ndx_comp] == comp) && (!row[i].empty()))
        {
            bson_append_start_object(exp, int_to_c_str(ic));
            ic++;
            bson_append_string(exp, comp, header_vector[ndx_component_name].c_str());
            bson_append_double(exp, Qnt, atof(row[i].c_str()));

            // checking if there are errors and units included in the CSV and adding them in the database
            if (i+1 < headline.size())
            {
                add_unit_and_error(exp, headline, row, i);
            }
            bson_append_finish_object(exp);
        }
    }
    //++ END array sbcomp ++//
    bson_append_finish_array(exp);
    ic=0;

    // 2nd level - data for phases charactrised/measured in this experiment
    //++ START array expphases ++//
    bson_append_start_array(exp, expphases); // going trough the headline and searching for "phase" keyword
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
                bson_append_start_object(exp, int_to_c_str(phc));                   // START phase object
                phc++;                                                              // phases counter increased by 1
                bson_append_string(exp, phase, phase_name.c_str());
                phases.push_back(phase_name);                    // vector that keeps already present phases
                ic = 0;

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
                    bson_append_start_array(exp, phprop);
                    // get phase properties
                    for (unsigned int j=0; j<headline.size(); ++j)
                    {
                        cut_csv_header(headline[j], header_delimiter, header_vector);
                        // if keyword phase and phase name match, and it is pahse property and row is not empty proceed.
                        if ((header_vector[ndx_phase] == phase) && (header_vector[ndx_phase_name] == phase_name) &&
                            (it_is_phase_property(header_vector[ndx_phase_property])) && (!row[j].empty()))
                        {
                            bson_append_start_object(exp, int_to_c_str(ic)); // START property object
                            ic++;                                            // increase the number of phases properties with one
                            bson_append_string(exp, property, header_vector[ndx_phase_property].c_str());
                            bson_append_double(exp, Qnt, atof(row[j].c_str()));

                            // checking if there are errors and units included in the CSV and adding tem in the database
                            if (j+1 < headline.size())
                            {
                                add_unit_and_error(exp, headline, row, j);
                            }
                            bson_append_finish_object(exp); // END property object
                        }
                    }
                    //++ END array phprop ++//
                    bson_append_finish_array(exp);
                    ic =0; // reset counter
                } h_phprop = false;

                //++ START array phcomp IC ++//
                if (h_phIC)
                {
                    bson_append_start_array(exp, phIC);
                    // get phase comp
                    for (unsigned int j=0; j<headline.size(); ++j)
                    {
                        cut_csv_header(headline[j], header_delimiter, header_vector);
                        // if keyword phase, phase name, and IC found in header proceed
                        if ((header_vector[ndx_phase] == phase) && (!row[j].empty()) &&
                            (header_vector[ndx_phase_name] == phase_name) && (header_vector[ndx_IC] == IC ))
                        {
                            // quantity of this IC in the phase
                            bson_append_start_object(exp, int_to_c_str(ic)); // START phase element object
                            ic++;
                            bson_append_string(exp, IC, header_vector[ndx_IC_name].c_str());
                            bson_append_double(exp, Qnt, strtod(row[j].c_str(), NULL));

                            // checking if there are errors and units included in the CSV and adding them in the database
                            if (j+1 < headline.size())
                            {
                                add_unit_and_error(exp, headline, row, j);
                            }
                            bson_append_finish_object(exp); // END phase element object
                        }
                    }
                    //++ END array phcomp IC ++//
                    bson_append_finish_array(exp);
                    ic = 0;
                } h_phIC = false;

                //++ START getting data reported as molar fraction
                if (h_phMR)
                {
                    bson_append_start_array(exp, phMR);
                    // get phase MR
                    for (unsigned int j=0; j<headline.size(); ++j)
                    {
                        cut_csv_header(headline[j], header_delimiter, header_vector);
                        // if keyword phase, phase name, and IC found in header proceed
                        if ((header_vector[ndx_phase] == phase) && (!row[j].empty()) &&
                            (header_vector[ndx_phase_name] == phase_name) && (header_vector[ndx_MR] == MR ))
                        {
                            bson_append_start_object(exp, int_to_c_str(mf)); // START phase element object
                            mf++;
                            bson_append_string(exp, MR, header_vector[ndx_MR_formula].c_str());
                            bson_append_double(exp, Qnt, atof(row[j].c_str()));

                            // checking if there are errors and units included in the CSV and adding tem in the database
                            if ((j+1) < headline.size())
                            {
                                add_unit_and_error(exp, headline, row, j);
                            }
                            bson_append_finish_object(exp); // END phase element object
                        }
                    }
                    //++ END array phMR ++//
                    bson_append_finish_array(exp);
                    mf = 0;
                } h_phMR = false;

                //++ START array phspecies ++//
                if (h_phDC) // check if there is species data in the CSV header
                {
                    string dcomp_name;
                    bson_append_start_array(exp, phDC);
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
                                bson_append_start_object(exp, int_to_c_str(dcc));
                                dcc++;
                                bson_append_string(exp, DC, dcomp_name.c_str());
                                dcomps.push_back(dcomp_name);
                                ic = 0;

                                // loop to get all the properties of current dcomp
                                bson_append_start_array(exp, DCprop);
                                for (unsigned int k=0; k<headline.size(); ++k)
                                {
                                    cut_csv_header(headline[k], header_delimiter, header_vector);
                                    if ((header_vector[ndx_DC_name] == dcomp_name) && (header_vector[ndx_phase] == phase) &&
                                        (header_vector[ndx_phase_name] == phase_name) && (header_vector[ndx_DC] == DC ) && (!row[k].empty()))
                                    {
                                        bson_append_start_object(exp, int_to_c_str(ic)); // START property object
                                        ic++;
                                        bson_append_string(exp, property, header_vector[ndx_DC_prop_name].c_str());
                                        bson_append_double(exp, Qnt, atof(row[k].c_str())); // quantity of the property

                                        // checking if there are errors and units included in the CSV and adding tem in the database
                                        if (k+1 < headline.size())
                                        {
                                            add_unit_and_error(exp, headline, row, k);
                                        }
                                        //++ END dcomp property object
                                        bson_append_finish_object(exp);
                                    } // end search for dcomp name in the headline of the csv file
                                }
                                //++ END dcompprop array
                                bson_append_finish_array(exp);
                                bson_append_finish_object(exp);
                            } //++ END if h_dcomp // end dcomp onject
//                            bson_append_finish_array(exp);
                        }
                    } dcomps.clear();
                    //++ END array phspecies ++//
                    bson_append_finish_array(exp);
                } h_phDC = false;
                // end object in the array phases
                dcc = 0;
                bson_append_finish_object(exp); //++ END phase object ++
            } // END if phase_already_added
        } // END check for key phase in the headline
    } phases.clear();
    //++ END array expphases ++//
    bson_append_finish_array(exp);
    bson_finish(exp);
}
// ----------- End of  gemsfit_iobson.cpp ----------------------------
