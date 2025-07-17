//-------------------------------------------------------------------
// $Id: gemsfit_iobson.cpp 333 2014-03-13 13:23:32Z gemsfits $
//
// Read/Write GEMSFIT input configuration file functions
//
// Copyright (C) 2014  S.V.Dmytriyeva, G.D.Miron
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
#include "io_arrays.h"
#include "keywords.h"


//-----------------------------------------------------------------------------------------------------
// from file # include "csvtoejdb.cpp"
using namespace keys;


// keeps one row of the CSV file to bson structure
// keeps headline of the CSV file

void csvToBson( bson *exp, const  vector<string>& headline, const vector<string>& row )
{
    int is=0,       // counts the number of processed samples (.csv data rows)
        ic=0,       // counts the number of system comp (recipe entries), independent components per pahse, phase properties per pahse, and dependent components properties per dependent component
        phc = 0,    // counts the number of phases per system/experiment
        dcc = 0,    // counts the number of dependent components per phase
        sk = 0,     // counts the number of metastability constraints per constraint type
        mf = 0;     // counts the number of molar fraction entries per system/exmperiment
    string ph_new, ph_old, sss;
    vector<string> phases, dcomps; // keeps the already read phases and dcomps
    stringstream ss;
    bool h_phprop = false, h_phases = false, h_phIC = false, h_dcomp = false, h_UMC= false,
         h_LMC = false, h_phMR = false, h_phDC = false; // handle that is true if we have the entry in the CSV file

    //string line;

   // getting the data from CSV line by line and processing it into BSON
    {
        is++;
        ic = 0;
        phc = 0;
        dcc = 0;
        // going trough the headline markers to identify the data type, based on which it is assigned into the database
        bson_init(exp);

        //first level objects: sample, expdataset, sT, sP.
        for (unsigned int i=0; i<headline.size(); ++i)
        {
            // check for U and L metastability constraint columns
            if ((strncmp(headline[i].c_str(),LMC, strlen(LMC)) == 0))
            {
                h_LMC = true;
            }
            if ((strncmp(headline[i].c_str(),UMC, strlen(UMC)) == 0))
            {
                h_UMC = true;
            }
            if ((headline[i]==expsample) || (headline[i]==expdataset) || (headline[i]==Tunit) || (headline[i]==Punit)|| (headline[i]==Vunit) || (headline[i]==type) || (headline[i]==comment))
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

         // 2nd level - upper and lower metastability restrictions
        if (h_UMC)
        {
            bson_append_start_array(exp, UMC);
            for (unsigned int i=0; i<headline.size(); ++i)
            {
                if ((strncmp(headline[i].c_str(),UMC, strlen(UMC)) == 0) && (!row[i].empty()))
                {
                    int pos_start, pos_end;
                    string component, type;
                    string f1(".");

                    // getting the name and the type: dependent component (DC) or phase
                    pos_start = headline[i].find(f1);
                    pos_end   = headline[i].find(f1,pos_start+1);
                    type      = headline[i].substr((pos_start+f1.length()),(pos_end-pos_start-f1.length()));
                    component = headline[i].substr((pos_end+f1.length()),( headline[i].size() -1));

                    ss << sk;
                    sss = ss.str();
                    ss.str("");
                    bson_append_start_object(exp, sss.c_str());
                    sk++;
                    if (type == DC) // dependent component
                    bson_append_string(exp, DC, component.c_str());
                    else
                        if (type == phase) // phase
                            bson_append_string(exp, phase, component.c_str());

                    bson_append_double(exp, Qnt, atof(row[i].c_str()));

                    // checking if there are errors and units included in the CSV and adding tem in the database
                    if (i+1 < headline.size())
                    {
                        if ((headline[i+1]==Qerror))
                        {
                            ++i;
                            if ((!row[i].empty()))
                            {
                            bson_append_double(exp, Qerror, atof(row[i].c_str()));
                            }
                        }
                        if ((headline[i+1]==_unit) && (!row[i+1].empty()))
                        {
                            ++i;
                            bson_append_string(exp, Qunit, row[i].c_str());
                        }
                    }
                    bson_append_finish_object(exp);
                }
            }
            //++ END array UMC ++//
            bson_append_finish_array(exp);
            sk=0;

        }

        if (h_LMC)
        {
            bson_append_start_array(exp, LMC);
            for (unsigned int i=0; i<headline.size(); ++i)
            {
                if ((strncmp(headline[i].c_str(),LMC, strlen(LMC)) == 0) && (!row[i].empty()))
                {
                    int pos_start, pos_end;
                    string component, type;
                    string f1(".");

                    // getting the name and the type: dependent component (DC) or phase
                    pos_start = headline[i].find(f1);
                    pos_end   = headline[i].find(f1,pos_start+1);
                    type      = headline[i].substr((pos_start+f1.length()),(pos_end-pos_start-f1.length()));
                    component = headline[i].substr((pos_end+f1.length()),( headline[i].size() -1));

                    ss << sk;
                    sss = ss.str();
                    ss.str("");
                    bson_append_start_object(exp, sss.c_str());
                    sk++;
                    if (type == DC) // dependent component
                    bson_append_string(exp, DC, component.c_str());
                    else
                        if (type == phase) // phase
                            bson_append_string(exp, phase, component.c_str());
                    bson_append_double(exp, Qnt, atof(row[i].c_str()));

                    // checking if there are errors and units included in the CSV and adding tem in the database
                    if (i+1 < headline.size())
                    {
                        if ((headline[i+1]==Qerror))
                        {
                            ++i;
                            if ((!row[i].empty()))
                            {
                            bson_append_double(exp, Qerror, atof(row[i].c_str()));
                            }
                        }
                        if ((headline[i+1]==_unit) && (!row[i+1].empty()))
                        {
                            ++i;
                            bson_append_string(exp, Qunit, row[i].c_str());
                        }
                    }
                    bson_append_finish_object(exp);
                }
            }
            //++ END array LMC ++//
            bson_append_finish_array(exp);
            sk=0;
        }

        // 2nd level - bulk composition of chemical system for the current experiment
        // array of components
        //++ START array sbcomp ++//
        bson_append_start_array(exp, sbcomp);
        for (unsigned int i=0; i<headline.size(); ++i)
        {
            if ((strncmp(headline[i].c_str(),comp, strlen(comp)) == 0) && (!row[i].empty()))
            {
                int pos_start, pos_end;
                string component;
                string f1(".");

                // getting the name of the component e.g. SiO2 form comp.SiO2
                pos_start = headline[i].find(f1);
                pos_end   = headline[i].find(f1,pos_start+1);
                component = headline[i].substr((pos_start+f1.length()),(pos_end-pos_start-f1.length()));

                ss << ic;
                sss = ss.str();
                ss.str("");
                bson_append_start_object(exp, sss.c_str());
                ic++;
                bson_append_string(exp, comp, component.c_str());
                bson_append_double(exp, Qnt, atof(row[i].c_str()));

                // checking if there are errors and units included in the CSV and adding tem in the database
                if (i+1 < headline.size())
                {
                    if ((headline[i+1]==Qerror))
                    {
                        ++i;
                        if ((!row[i].empty()))
                        {
                        bson_append_double(exp, Qerror, atof(row[i].c_str()));
                        }
                    }
                    if ((headline[i+1]==_unit) && (!row[i+1].empty()))
                    {
                        ++i;
                        bson_append_string(exp, Qunit, row[i].c_str());
                    }
                }
                bson_append_finish_object(exp);
            }
        }
        //++ END array sbcomp ++//
        bson_append_finish_array(exp);
        ic=0;

        // 2nd level - data for phases charactrised/measured in this experiment
        //++ START array expphases ++//
        bson_append_start_array(exp, expphases);
        // going trough the headline and searching for "phase" keword
        for (unsigned int i=0; i<headline.size(); ++i)
        {
            if ((strncmp(headline[i].c_str(),phase, strlen(phase)) == 0) && (!row[i].empty()))

            {
                int pos_start, pos_end;
                string phase_name, ph_prop, ph_prop_1, ph_prop_2, ph_prop_3;
                string f1(".");

                // getting the name of the phase e.g. aq_gen form phase.aq_gen
                pos_start  = headline[i].find(f1);
                pos_end    = headline[i].find(f1,pos_start+1);
                phase_name = headline[i].substr((pos_start+f1.length()),(pos_end-pos_start-f1.length()));
                ph_new = phase_name;
                h_phases = false;

                // only if we have a new pahse name we add new pahse name string, as pahse names can appear multiple times as keys in the headline
                if ((ph_new != ph_old))
                {
                    // check if pahse name was not present before
                    for (unsigned int j=0; j<phases.size(); ++j)
                    {
                        if (phase_name == phases[j])
                        {
                            h_phases = true;
                        }
                    }

                    if (!h_phases) // START if h_phases. Every phase is scaned only once troughout out the headline.
                        // The program goes trough this if only at the first enocunter of the phase name and scans the document for recuring of the phase.
                    {
                        ss << phc;
                        sss = ss.str();
                        ss.str("");
                        bson_append_start_object(exp, sss.c_str()); // START phase object
                        phc++;
                        bson_append_string(exp, phase, phase_name.c_str());
                        phases.push_back(phase_name); // vector that keeps already present phases
                        ic = 0;

                        // START check if there is phprop & phcomp data in the CSV
                        for (unsigned int j=0; j<headline.size(); ++j)
                        {
                            if ((strncmp(headline[j].c_str(),phase, strlen(phase)) == 0) && (!row[j].empty())) // checks where in the headline the same pahse name is present
                            {
                                pos_start = headline[j].find(f1);
                                pos_end   = headline[j].find(f1,pos_start+1);
                                // getting the phase properties and composition, what follows fter phase name e.g. aq_gen.IC.Si.Q
                                if (phase_name == headline[j].substr((pos_start+f1.length()),(pos_end-pos_start-f1.length())))
                                {
                                    // getting the name of the property phase e.g. IC.Si.Q from phase.aq_gen.IC.Si.Q, or Q from pahse.aq_gen.Q
                                    ph_prop_1 = headline[j].substr((pos_end+f1.length()),(headline[j].size()));

                                    pos_start = ph_prop_1.find(f1);
                                    pos_end   = ph_prop_1.find(f1,pos_start+1);
                                    ph_prop   = ph_prop_1.substr((0),(pos_start));

                                    // if property present
                                    if (((ph_prop == Qnt) || (ph_prop == pH)  || (ph_prop == pHm)   || (ph_prop == pV)        || (ph_prop == Eh)     ||
                                         (ph_prop == IS)  || (ph_prop == all) || (ph_prop == sArea) || (ph_prop == RHO)       ||
                                         (ph_prop == Gex) || (ph_prop == pe)  || (ph_prop == oscw)  || (ph_prop == mChainL)   || (ph_prop == Rd)     ||
                                         (ph_prop == frAlIV)   ||  (ph_prop == frAlV)   ||  (ph_prop == frAlVI)   ||
                                         (ph_prop == UMC) || (ph_prop == LMC)) && (!row[j].empty()))
                                    {
                                        h_phprop = true;
                                    }
                                    // if composition present
                                    if (ph_prop == IC/*((ph_prop != pQnt) && (ph_prop != pH) && (ph_prop != pV) &&  (ph_prop != Eh) && (ph_prop != IS) && (ph_prop != all) &&  (ph_prop != sArea) && (ph_prop != RHO)) && (strncmp(ph_prop.c_str(),"Dc", 2) != 0) && (!row[j].empty())*/)
                                    {
                                        h_phIC = true;
                                    }
                                    if (ph_prop == MR)
                                    {
                                        h_phMR = true;
                                    }
                                    if (ph_prop == DC)
                                    {
                                        h_phDC = true;
                                    }
                                }
                            }
                        } // END check if there is phprop & phcomp data in the CSV

                        //++ START array phprop ++//
                        if (h_phprop)
                        {
                            bson_append_start_array(exp, phprop);
                            // get phase poperties
                            for (unsigned int j=0; j<headline.size(); ++j)
                            {
                                if ((strncmp(headline[j].c_str(),phase, strlen(phase)) == 0) && (!row[j].empty()))
                                {
                                    pos_start = headline[j].find(f1);
                                    pos_end   = headline[j].find(f1,pos_start+1);
                                    // getting the phase properties
                                    if (phase_name == headline[j].substr((pos_start+f1.length()),(pos_end-pos_start-f1.length())))
                                    {
                                        // getting the name of the property
                                        ph_prop = headline[j].substr((pos_end+f1.length()),(headline[j].size()));

                                        // amount of the property of the phase in the experiment
                                        if (((ph_prop == Qnt) || (ph_prop == pH)  || (ph_prop == pHm)    || (ph_prop == pV)      || (ph_prop == Eh)     ||
                                             (ph_prop == IS)  || (ph_prop == all) || (ph_prop == sArea)  || (ph_prop == RHO)     ||
                                             (ph_prop == Gex) || (ph_prop == pe)  || (ph_prop == oscw)   || (ph_prop == mChainL) || (ph_prop == Rd) ||
                                             (ph_prop == frAlIV)   ||  (ph_prop == frAlV)   ||  (ph_prop == frAlVI)   ||
                                             (ph_prop == UMC) || (ph_prop == LMC)) && (!row[j].empty()))
                                        {
//                                            if (((ph_prop == pH) || (ph_prop == Eh) || (ph_prop == IS) || (ph_prop == all) || (ph_prop == pe) || (ph_prop == oscw)) /*&& (phase_name != aq)*/)
//                                            {
//                                                cout << ph_prop << " only works with aqueous phase (aq_gen)!"<< endl;
//                                                exit(1);
//                                            }

                                            ss << ic;
                                            sss = ss.str();
                                            ss.str("");
                                            bson_append_start_object(exp, sss.c_str()); // START property object
                                            ic++;
                                            bson_append_string(exp, property, ph_prop.c_str());
                                            bson_append_double(exp, Qnt, atof(row[j].c_str()));

                                            // checking if there are errors and units included in the CSV and adding tem in the database
                                            if (j+1 < headline.size())
                                            {
                                                if ((headline[j+1]==Qerror))
                                                {
                                                    ++j;
                                                    if ((!row[j].empty()))
                                                    {
                                                        bson_append_double(exp, Qerror, atof(row[j].c_str()));
                                                    }
                                                }
                                                if (j+1 < headline.size())
                                                if ((headline[j+1]==_unit) && (!row[j+1].empty()))
                                                {
                                                    ++j;
                                                    bson_append_string(exp, Qunit, row[j].c_str());
                                                }
                                            }
                                            bson_append_finish_object(exp); // END property object
                                            ph_old = phase_name;
                                        }
                                    }
                                }
                            }
                            //++ END array phprop ++//
                            bson_append_finish_array(exp);
                            ic =0;
                        } h_phprop = false;

                        //++ START array phcomp IC ++//
                        if (h_phIC)
                        {
                            bson_append_start_array(exp, phIC);
                            // get phase comp
                            for (unsigned int j=0; j<headline.size(); ++j)
                            {
                                if ((strncmp(headline[j].c_str(),phase, strlen(phase)) == 0) && (!row[j].empty()))
                                {
                                    pos_start = headline[j].find(f1);
                                    pos_end   = headline[j].find(f1,pos_start+1);
                                    // getting the phase_name properties and composition from CSV
                                    if (phase_name == headline[j].substr((pos_start+f1.length()),(pos_end-pos_start-f1.length())))
                                    {
                                        // getting the name of the property phase e.g. Si from phase.aq_gen.IC.Si.Q,
                                        ph_prop_1 = headline[j].substr((pos_end+f1.length()),(headline[j].size()));
                                        pos_start = ph_prop_1.find(f1);
                                        pos_end   = ph_prop_1.find(f1,pos_start+1);
                                        ph_prop_2 = ph_prop_1.substr((0),(pos_start));
                                        ph_prop   = ph_prop_1.substr((pos_start+1),(ph_prop_1.size()));
                                        pos_start = ph_prop_1.find(f1,pos_start +1);
                                        pos_end   = ph_prop_1.find(f1,pos_end+1);
                                        ph_prop_1 = ph_prop_1.substr((pos_start+1),(pos_start));

                                        ph_prop_3 = ph_prop;
                                        pos_start = ph_prop_3.find(f1,0);
                                        pos_end   = ph_prop_3.find(f1,pos_end+1);
                                        ph_prop   = ph_prop_3.substr((0),(pos_start));

                                        // quantity of this IC in the phase
                                        if (((ph_prop != Qnt)   && (ph_prop_1 == Qnt)    && (ph_prop_2 == IC) && (ph_prop != pH)    && (ph_prop != pHm)    &&
                                             (ph_prop != pV)    && (ph_prop != Eh)       && (ph_prop != IS)   && (ph_prop != all)   &&
                                             (ph_prop != sArea) && (ph_prop != RHO)      && (ph_prop != Gex)  && (ph_prop != pe)    && (ph_prop != Rd) &&
                                             (ph_prop != frAlIV)   &&  (ph_prop != frAlV)   &&  (ph_prop != frAlVI)   &&
                                             (ph_prop != oscw)  && (ph_prop != mChainL)) && (strncmp(ph_prop.c_str(),"DC", 2) != 0) &&
                                             (ph_prop != UMC)   && (ph_prop != LMC)      && (!row[j].empty()))
                                        {
                                            ss << ic;
                                            sss = ss.str();
                                            ss.str("");
                                            bson_append_start_object(exp, sss.c_str()); // START phase element object
                                            ic++;
                                            bson_append_string(exp, IC, ph_prop.c_str());
                                            bson_append_double(exp, Qnt, strtod(row[j].c_str(), NULL));
//                                            cout << strtod(row[j].c_str(), NULL) << endl;
//                                            cout << atof(row[j].c_str()) << endl;

                                            // checking if there are errors and units included in the CSV and adding them in the database
                                            if (j+1 < headline.size())
                                            {
                                                if ((headline[j+1]==Qerror))
                                                {
                                                    ++j;
                                                    if ((!row[j].empty()))
                                                    {
                                                        bson_append_double(exp, Qerror, atof(row[j].c_str()));
                                                    }
                                                }
                                                if (j+1 < headline.size())
                                                if ((headline[j+1]==_unit) && (!row[j+1].empty()))
                                                {
                                                    ++j;
                                                    bson_append_string(exp, Qunit, row[j].c_str());
                                                }
                                            }
                                            bson_append_finish_object(exp); // END phase element object
                                            ph_old = phase_name;
                                        }
                                    }
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
                            // get phase comp
                            for (unsigned int j=0; j<headline.size(); ++j)
                            {
                                if ((strncmp(headline[j].c_str(),phase, strlen(phase)) == 0) && (!row[j].empty()))
                                {
                                    pos_start = headline[j].find(f1);
                                    pos_end   = headline[j].find(f1,pos_start+1);
                                    // getting the phase_name properties and composition from CSV
                                    if (phase_name == headline[j].substr((pos_start+f1.length()),(pos_end-pos_start-f1.length())))
                                    {
                                        // getting the name of the property phase e.g. Si from phase.aq_gen.IC.Si.Q,
                                        ph_prop_1 = headline[j].substr((pos_end+f1.length()),(headline[j].size()));
                                        pos_start = ph_prop_1.find(f1);
                                        pos_end   = ph_prop_1.find(f1,pos_start+1);
                                        ph_prop_2 = ph_prop_1.substr((0),(pos_start));
                                        ph_prop   = ph_prop_1.substr((pos_start+1),(ph_prop_1.size()));
                                        pos_start = ph_prop_1.find(f1,pos_start +1);
                                        pos_end   = ph_prop_1.find(f1,pos_end+1);
                                        ph_prop_1 = ph_prop_1.substr((pos_start+1),(pos_start));

                                        ph_prop_3 = ph_prop;
                                        pos_start = ph_prop_3.find(f1,0);
                                        pos_end   = ph_prop_3.find(f1,pos_end+1);
                                        ph_prop   = ph_prop_3.substr((0),(pos_start));

                                        if ((ph_prop_2 == MR) && (!row[j].empty()))
                                        {
                                            ss << mf;
                                            sss = ss.str();
                                            ss.str("");
                                            bson_append_start_object(exp, sss.c_str()); // START phase element object
                                            mf++;
                                            bson_append_string(exp, MR, ph_prop.c_str());
                                            bson_append_double(exp, Qnt, atof(row[j].c_str()));

                                            // checking if there are errors and units included in the CSV and adding tem in the database
                                            if ((j+1) < headline.size())
                                            {
                                                if ((headline[j+1]==Qerror))
                                                {
                                                    ++j;
                                                    if ((!row[j].empty()))
                                                    {
                                                        bson_append_double(exp, Qerror, atof(row[j].c_str()));
                                                    }
                                                }
                                                if (j+1 < headline.size())
                                                if ((headline[j+1]==_unit) && (!row[j+1].empty()))
                                                {
                                                    ++j;
                                                    bson_append_string(exp, Qunit, row[j].c_str());
                                                }
                                            }
                                            bson_append_finish_object(exp); // END phase element object
                                            ph_old = phase_name;
                                        }
                                    }
                                }
                            }
                            //++ END array phMR ++//
                            bson_append_finish_array(exp);
                            mf = 0;
                        } h_phMR = false;


                        //++ START array phspecies ++//
                        if (h_phDC)/*((ph_prop_2 == DC) && (!row[i].empty()))*/ // check if there is species data in the CSV header
                        {
                            string dcomp_new, dcomp_old;
                            bson_append_start_array(exp, phDC);
                            for (unsigned int j=0; j<headline.size(); ++j)
                            {
                                if ((strncmp(headline[j].c_str(),phase, strlen(phase)) == 0) && (!row[j].empty())) // check for pahse. key
                                {
                                    pos_start = headline[j].find(f1);
                                    pos_end   = headline[j].find(f1,pos_start+1);
                                    // getting the phase species
                                    if (phase_name == headline[j].substr((pos_start+f1.length()),(pos_end-pos_start-f1.length()))) // check for phase name
                                    {
                                        ph_prop = headline[j].substr((pos_end+f1.length()),(headline[j].size()));
                                        // species
                                        if ((strncmp(ph_prop.c_str(),"DC.", 1) == 0) && (!row[j].empty()))
                                        {
                                            string dcomp_name, ph_dcomp;

                                            ph_dcomp = "phase." + phase_name + ".";

                                            ph_prop = ph_prop.substr(3,ph_prop.length()); // deleting the "Dc" - species name

                                            // getting the name of the phase e.g. aq_gen form phase.aq_gen
                                            pos_start  = -1;
                                            pos_end    = ph_prop.find(f1);
                                            dcomp_name = ph_prop.substr((pos_start+f1.length()),(pos_end-pos_start-f1.length()));
                                            dcomp_new  = dcomp_name;
                                            h_dcomp    = false;

                                            if ((dcomp_new != dcomp_old))
                                            {
                                                // check if dcomp name was not present before
                                                for (unsigned int j=0; j<dcomps.size(); ++j)
                                                {
                                                    if (dcomp_name == dcomps[j])
                                                    {
                                                        h_dcomp = true;
                                                    }
                                                }

                                                //++ START if h_dcomp
                                                if (!h_dcomp)
                                                {
                                                    string dcomp_prop;
                                                    ss << dcc;
                                                    sss = ss.str();
                                                    ss.str("");
                                                    //++ START species object
                                                    bson_append_start_object(exp, sss.c_str());
                                                    dcc++;
                                                    bson_append_string(exp, DC, dcomp_name.c_str());
                                                    dcomps.push_back(dcomp_name);
                                                    ph_dcomp += "DC." + dcomp_name + ".";
                                                    ic = 0;

                                                    // loop to get all the properties of current dcomp
                                                    bson_append_start_array(exp, DCprop);
                                                    for (unsigned int j=0; j<headline.size(); ++j)
                                                    {
                                                        if ((strncmp(headline[j].c_str(),ph_dcomp.c_str(), ph_dcomp.size()) == 0) && (!row[j].empty()))
                                                        {
//                                                            cout << j << endl;

                                                            pos_start  = ph_dcomp.size();
                                                            pos_end    = headline[j].size();
                                                            dcomp_prop = headline[j].substr(pos_start, pos_end);

                                                            ss << ic;
                                                            sss = ss.str();
                                                            ss.str("");
                                                            bson_append_start_object(exp, sss.c_str()); // START property object
                                                            ic++;
                                                            bson_append_string(exp, property, dcomp_prop.c_str());
                                                            bson_append_double(exp, Qnt, atof(row[j].c_str())); // quantity of the property

                                                            // checking if there are errors and units included in the CSV and adding tem in the database
                                                            if (j+1 < headline.size())
                                                            {
                                                                if ((headline[j+1]==Qerror))
                                                                {
                                                                    ++j;
                                                                    if ((!row[j].empty()))
                                                                    {
                                                                        bson_append_double(exp, Qerror, atof(row[j].c_str()));
                                                                    }
                                                                }
                                                                if (j+1 < headline.size())
                                                                if ((headline[j+1]==_unit) && (!row[j+1].empty()))
                                                                {
                                                                    ++j;
                                                                    bson_append_string(exp, Qunit, row[j].c_str());
                                                                }
                                                            }
                                                            //++ END dcomp property object
                                                            bson_append_finish_object(exp);
                                                            dcomp_old = dcomp_name;
                                                        } // end search for dcomp name in the headline of the csv file
                                                    }
                                                    //++ END dcompprop array
                                                    bson_append_finish_array(exp);
                                                } //++ END if h_dcomp
                                                // end dcomp onject
                                                bson_append_finish_array(exp);
                                            } // END comparison to check fo unique dcomps
                                            ph_old = phase_name;
                                        } // search for "DC" indicator of dependent component entry in the csv headline
                                    } //
                                    dcomps.clear();
                                }
                            }
                            //++ END array phspecies ++//
                            bson_append_finish_array(exp);
                        } h_phDC = false;
                        // end object in the array phases
                        dcc = 0;
                        bson_append_finish_object(exp); //++ END phase object ++
                    } // END if h_phases
                } // END ph_new != ph_old
            } // END check for key phase in the headline
        }
        //++ END array expphases ++//
        bson_append_finish_array(exp);
        bson_finish(exp);
     }

}



// ----------- End of  gemsfit_iobson.cpp ----------------------------
