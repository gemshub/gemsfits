# include "csvtoejdb.h"
# include "csv_parser.h"
# include "keywords.h"
# include <boost/lexical_cast.hpp>

using namespace std;
using namespace keys;

void csvtoejdb(char csv_path[64], EJDB *jb, EJCOLL *coll)
{
    int ic=0, phc = 0, dcc = 0, sk = 0;
    string ph_new, ph_old, dcomp_new, dcomp_old;
    vector<string> phases, dcomps;
    bool h_phprop = false, h_phases = false, h_phIC = false, h_dcomp = false, h_Upper_CK= false, h_Lower_CK = false; // handle that is true if we have ph_prop in the CSV file

    bson_oid_t oid;
    // keeps each row of the CSV file
    vector<string> row;
    // keeps headline of the CSV file
    vector<string> headline;
    string line;

    ifstream in(csv_path);
    if (in.fail())  { cout << "File not found" <<endl; /*return 0;*/ }


    getline(in, line);
    // getting headline
    csvline(headline, line, ',');

    // getting the data form CSV line by line and porcessing it into BSON
    while(getline(in, line)  && in.good() )
    {
        ic = 0;
        phc = 0; // keeps number of phases per line
        dcc = 0; // keeps number of dependent components
        csvline(row, line, ',');
        // going trough the headline markers to identify the data type, based on which it is assigned into the database
        bson exp;
        bson_init(&exp);

        // cheking if the combination of sample name and dataset name is unique in the datasbase
        bson bq1;
        bson_init_as_query(&bq1);

        //first level objects: sample, expdataset, sT, sP.
        for (unsigned int i=0; i<headline.size(); ++i)
        {
            // check for U and L metastability constraint columns
            if ((strncmp(headline[i].c_str(),Lower_CK, 8) == 0))
            {
                h_Lower_CK = true;
            }
            if ((strncmp(headline[i].c_str(),Upper_CK, 8) == 0))
            {
                h_Upper_CK = true;
            }
            if ((headline[i]==expsample) || (headline[i]==expdataset) || (headline[i]==Tunit) || (headline[i]==Punit)|| (headline[i]==Vunit) )
            {
                bson_append_string(&exp, headline[i].c_str(), row[i].c_str());
                // for query
                if (headline[i]==expdataset)
                {
                    bson_append_start_object(&bq1, expdataset);
                    bson_append_string(&bq1, "$begin", row[i].c_str());
                    bson_append_finish_object(&bq1);
                }
                if (headline[i]==expsample)
                {
                    bson_append_start_object(&bq1, expsample);
                    bson_append_string(&bq1, "$begin", row[i].c_str());
                    bson_append_finish_object(&bq1);
                } // end for query
            }
            else if  ((headline[i]==sT) || (headline[i]==sP))
            {
                bson_append_int(&exp, headline[i].c_str(), atoi(row[i].c_str()));
//                if (headline[i]==sT)
//                {
//                    TP[0].push_back(atoi(row[i].c_str()));
//                }
//                if (headline[i]==sP)
//                {
//                    TP[1].push_back(atoi(row[i].c_str()));
//                }
            }
            else if (headline[i]==sV)
            {
                bson_append_double(&exp, headline[i].c_str(), atof(row[i].c_str()));
            } else if (headline[i] == Weight)
            {
                if (!row[i].empty())
                {
                    bson_append_double(&exp, headline[i].c_str(), atof(row[i].c_str()));
                } else bson_append_double(&exp, headline[i].c_str(), 1.0);
            }
         }

        bson_finish(&bq1);
        EJQ *q1 = ejdbcreatequery(jb, &bq1, NULL, 0, NULL);
        uint32_t count;
        TCLIST *res = ejdbqryexecute(coll, q1, &count, 0, NULL);

        if (count > 0)
        {
            bson_destroy(&exp);
            cout << "The combination of sample name and expdataset in the input csv file is already present in the database.\n"
                 << "Are you trying to import the same experimental data again? " << endl;
                 cout << "Number of experiments: " << count << endl;
            //Now print the result set records
             for (int i = 0; i < TCLISTNUM(res); ++i) {
                 void *bsdata = TCLISTVALPTR(res, i);
                 char *bsdata_ = static_cast<char*>(bsdata);
                 bson_print_raw(stderr, bsdata_, 0);
             }
             fprintf(stderr, "\n");
             //Dispose result set
             tclistdel(res);

             //Dispose query
             ejdbquerydel(q1);
             exit(1);
        }

        // 2nd level - upper and lower metastability restrictions
        if (h_Upper_CK)
        {
            bson_append_start_array(&exp, Upper_CK);
            for (unsigned int i=0; i<headline.size(); ++i)
            {
                if ((strncmp(headline[i].c_str(),Upper_CK, 8) == 0) && (!row[i].empty()))
                {
                    int pos_start, pos_end;
                    string component;
                    string f1(".");

                    // getting the name of the component e.g. SiO2 form comp.SiO2
                    pos_start = headline[i].find(f1);
                    pos_end   = headline[i].find(f1,pos_start+1);
                    component = headline[i].substr((pos_start+f1.length()),(pos_end-pos_start-f1.length()));

                    bson_append_start_object(&exp, boost::lexical_cast<string>(sk).c_str());
                    sk++;
                    bson_append_string(&exp, dcomp, component.c_str());
                    bson_append_double(&exp, pQnt, atof(row[i].c_str()));

                    // checking if there are errors and units included in the CSV and adding tem in the database
                    if (i+1 < headline.size())
                    {
                        if ((headline[i+1]==_error))
                        {
                            ++i;
                            if ((!row[i].empty()))
                            {
                            bson_append_double(&exp, Qerror, atof(row[i].c_str()));
                            }
                        }
                        if ((headline[i+1]==_unit) && (!row[i+1].empty()))
                        {
                            ++i;
                            bson_append_string(&exp, Qunit, row[i].c_str());
                        }
                    }
                    bson_append_finish_object(&exp);
                }
            }
            //++ END array Upper_CK ++//
            bson_append_finish_array(&exp);
            sk=0;

        }

        if (h_Lower_CK)
        {
            bson_append_start_array(&exp, Lower_CK);
            for (unsigned int i=0; i<headline.size(); ++i)
            {
                if ((strncmp(headline[i].c_str(),Lower_CK, 8) == 0) && (!row[i].empty()))
                {
                    int pos_start, pos_end;
                    string component;
                    string f1(".");

                    // getting the name of the component e.g. SiO2 form comp.SiO2
                    pos_start = headline[i].find(f1);
                    pos_end   = headline[i].find(f1,pos_start+1);
                    component = headline[i].substr((pos_start+f1.length()),(pos_end-pos_start-f1.length()));

                    bson_append_start_object(&exp, boost::lexical_cast<string>(sk).c_str());
                    sk++;
                    bson_append_string(&exp, dcomp, component.c_str());
                    bson_append_double(&exp, pQnt, atof(row[i].c_str()));

                    // checking if there are errors and units included in the CSV and adding tem in the database
                    if (i+1 < headline.size())
                    {
                        if ((headline[i+1]==_error))
                        {
                            ++i;
                            if ((!row[i].empty()))
                            {
                            bson_append_double(&exp, Qerror, atof(row[i].c_str()));
                            }
                        }
                        if ((headline[i+1]==_unit) && (!row[i+1].empty()))
                        {
                            ++i;
                            bson_append_string(&exp, Qunit, row[i].c_str());
                        }
                    }
                    bson_append_finish_object(&exp);
                }
            }
            //++ END array Lower_CK ++//
            bson_append_finish_array(&exp);
            sk=0;

        }

        // 2nd level - bulk composition of chemical system for this experiment
        // array of components
        //++ START array sbcomp ++//
        bson_append_start_array(&exp, sbcomp);
        for (unsigned int i=0; i<headline.size(); ++i)
        {
            if ((strncmp(headline[i].c_str(),comp, 4) == 0) && (!row[i].empty()))
            {
                int pos_start, pos_end;
                string component;
                string f1(".");

                // getting the name of the component e.g. SiO2 form comp.SiO2
                pos_start = headline[i].find(f1);
                pos_end   = headline[i].find(f1,pos_start+1);
                component = headline[i].substr((pos_start+f1.length()),(pos_end-pos_start-f1.length()));

                bson_append_start_object(&exp, boost::lexical_cast<string>(ic).c_str());
                ic++;
                bson_append_string(&exp, comp, component.c_str());
                bson_append_double(&exp, bQnt, atof(row[i].c_str()));

                // checking if there are errors and units included in the CSV and adding tem in the database
                if (i+1 < headline.size())
                {
                    if ((headline[i+1]==_error))
                    {
                        ++i;
                        if ((!row[i].empty()))
                        {
                        bson_append_double(&exp, Qerror, atof(row[i].c_str()));
                        }
                    }
                    if ((headline[i+1]==_unit) && (!row[i+1].empty()))
                    {
                        ++i;
                        bson_append_string(&exp, Qunit, row[i].c_str());
                    }
                }
                bson_append_finish_object(&exp);
            }
        }
        //++ END array sbcomp ++//
        bson_append_finish_array(&exp);
        ic=0;

        // 2nd level - data for phases charactrised/measured in this experiment
        //++ START array expphases ++//
        bson_append_start_array(&exp, expphases);
        // going trough the headline and searching for "phase" keword
        for (unsigned int i=0; i<headline.size(); ++i)
        {
            if ((strncmp(headline[i].c_str(),phase, 5) == 0) && (!row[i].empty()))
            {
                int pos_start, pos_end;
                string phase_name, ph_prop, ph_prop_1, ph_prop_2, ph_prop_3;
                string f1(".");

                // getting the name of the phase e.g. aq_gen form phase.aq_gen
                pos_start = headline[i].find(f1);
                pos_end   = headline[i].find(f1,pos_start+1);
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

                    if (!h_phases) // START if h_phases
                    {
                        bson_append_start_object(&exp, boost::lexical_cast<string>(phc).c_str()); // START phase object
                        phc++;
                        bson_append_string(&exp, phase, phase_name.c_str());
                        phases.push_back(phase_name);
                        ic = 0;

                        // START check if there is phprop & phcomp data exists in the CSV
                        for (unsigned int j=0; j<headline.size(); ++j)
                        {
                            if ((strncmp(headline[j].c_str(),phase, 5) == 0) && (!row[j].empty()))
                            {
                                pos_start = headline[j].find(f1);
                                pos_end   = headline[j].find(f1,pos_start+1);
                                // getting the phase properties and composition
                                if (phase_name == headline[j].substr((pos_start+f1.length()),(pos_end-pos_start-f1.length())))
                                {
                                    // getting the name of the property phase e.g. Si from phase.aq_gen.Si, or pQunt from pahse.aq_gen.pQunt
                                    ph_prop_1 = headline[j].substr((pos_end+f1.length()),(headline[j].size()));

                                    pos_start = ph_prop_1.find(f1);
                                    pos_end   = ph_prop_1.find(f1,pos_start+1);
                                    ph_prop = ph_prop_1.substr((0),(pos_start));

                                    // if property present
                                    if (((ph_prop == pQnt) || (ph_prop == pH) || (ph_prop == pV) ||  (ph_prop == Eh) || (ph_prop == IS) || (ph_prop == all) ||  (ph_prop == sArea) || (ph_prop == RHO)) && (!row[j].empty()))
                                    {
                                        h_phprop = true;
                                    }
                                    // if composition present
                                    if (ph_prop == IC/*((ph_prop != pQnt) && (ph_prop != pH) && (ph_prop != pV) &&  (ph_prop != Eh) && (ph_prop != IS) && (ph_prop != all) &&  (ph_prop != sArea) && (ph_prop != RHO)) && (strncmp(ph_prop.c_str(),"Dc", 2) != 0) && (!row[j].empty())*/)
                                    {
                                        h_phIC = true;
                                    }
//                                    if (ph_prop == DC)
//                                    {
//                                        h_phdcomp = true;
//                                    }
                                }
                            }
                        } // END check

                        //++ START array phprop ++//
                        if (h_phprop)
                        {
                            bson_append_start_array(&exp, phprop);
                            // get phase poperties
                            for (unsigned int j=0; j<headline.size(); ++j)
                            {
                                if ((strncmp(headline[j].c_str(),phase, 5) == 0) && (!row[j].empty()))
                                {
                                    pos_start = headline[j].find(f1);
                                    pos_end   = headline[j].find(f1,pos_start+1);
                                    // getting the phase properties
                                    if (phase_name == headline[j].substr((pos_start+f1.length()),(pos_end-pos_start-f1.length())))
                                    {
                                        // getting the name of the property phase e.g. Si from phase.aq_gen.Si
                                        ph_prop = headline[j].substr((pos_end+f1.length()),(headline[j].size()));

                                        // amount of the property of the phase pahse in the experiment
                                        if (((ph_prop == pQnt) || (ph_prop == pH) || (ph_prop == pV) ||  (ph_prop == Eh) || (ph_prop == IS) || (ph_prop == all) ||  (ph_prop == sArea) || (ph_prop == RHO)) && (!row[j].empty()))
                                        {
                                            bson_append_start_object(&exp, boost::lexical_cast<string>(ic).c_str()); // START property object
                                            ic++;
                                            bson_append_string(&exp, property, ph_prop.c_str());
                                            bson_append_double(&exp, pQnt, atof(row[j].c_str()));

                                            // checking if there are errors and units included in the CSV and adding tem in the database
                                            if (j+1 < headline.size())
                                            {
                                                if ((headline[j+1]==_error))
                                                {
                                                    ++j;
                                                    if ((!row[j].empty()))
                                                    {
                                                        bson_append_double(&exp, Qerror, atof(row[j].c_str()));
                                                    }
                                                }
                                                if ((headline[j+1]==_unit) && (!row[j+1].empty()))
                                                {
                                                    ++j;
                                                    bson_append_string(&exp, Qunit, row[j].c_str());
                                                }
                                            }
                                            bson_append_finish_object(&exp); // END property object
                                            ph_old = phase_name;
                                        }
                                    }
                                }
                            }
                            //++ END array phprop ++//
                            bson_append_finish_array(&exp);
                            ic =0;
                        } h_phprop = false;

                        //++ START array phcomp ++//
                        if (h_phIC)
                        {
                            bson_append_start_array(&exp, phcomp);
                            // get phase comp
                            for (unsigned int j=0; j<headline.size(); ++j)
                            {
                                if ((strncmp(headline[j].c_str(),phase, 5) == 0) && (!row[j].empty()))
                                {
                                    pos_start = headline[j].find(f1);
                                    pos_end   = headline[j].find(f1,pos_start+1);
                                    // getting the phase_name properties and composition from CSV
                                    if (phase_name == headline[j].substr((pos_start+f1.length()),(pos_end-pos_start-f1.length())))
                                    {
                                        // getting the name of the property phase e.g. Si from phase.aq_gen.Si, or pQunt from pahse.aq_gen.pQunt
                                        ph_prop_1 = headline[j].substr((pos_end+f1.length()),(headline[j].size()));
                                        pos_start = ph_prop_1.find(f1);
                                        pos_end   = ph_prop_1.find(f1,pos_start+1);
                                        ph_prop_2 = ph_prop_1.substr((0),(pos_start));
                                        ph_prop = ph_prop_1.substr((pos_start+1),(ph_prop_1.size()));
                                        pos_start = ph_prop_1.find(f1,pos_start +1);
                                        pos_end   = ph_prop_1.find(f1,pos_end+1);
                                        ph_prop_1 = ph_prop_1.substr((pos_start+1),(pos_start));

                                        ph_prop_3 = ph_prop;
                                        pos_start = ph_prop_3.find(f1,0);
                                        pos_end   = ph_prop_3.find(f1,pos_end+1);
                                        ph_prop = ph_prop_3.substr((0),(pos_start));


                                        // qunatity of this comp in the phase
                                        if (((ph_prop != pQnt) && (ph_prop_1 == pQnt) && (ph_prop_2 == IC) && (ph_prop != pH) && (ph_prop != pV) &&  (ph_prop != Eh) && (ph_prop != IS) && (ph_prop != all) &&  (ph_prop != sArea) && (ph_prop != RHO)) && (strncmp(ph_prop.c_str(),"Dc", 2) != 0) && (!row[j].empty()))
                                        {
                                            bson_append_start_object(&exp, boost::lexical_cast<string>(ic).c_str()); // START phase element object
                                            ic++;
                                            bson_append_string(&exp, element, ph_prop.c_str());
                                            bson_append_double(&exp, eQnt, atof(row[j].c_str()));

                                            // checking if there are errors and units included in the CSV and adding tem in the database
                                            if (j+1 < headline.size())
                                            {
                                                if ((headline[j+1]==_error))
                                                {
                                                    ++j;
                                                    if ((!row[j].empty()))
                                                    {
                                                        bson_append_double(&exp, Qerror, atof(row[j].c_str()));
                                                    }
                                                }
                                                if ((headline[j+1]==_unit) && (!row[j+1].empty()))
                                                {
                                                    ++j;
                                                    bson_append_string(&exp, Qunit, row[j].c_str());
                                                }
                                            }
                                            bson_append_finish_object(&exp); // END phase element object
                                            ph_old = phase_name;
                                        }
                                    }
                                }
                            }
                            //++ END array phcomp ++//
                            bson_append_finish_array(&exp);
                            ic = 0;
                        } h_phIC = false;

                        //++ START array phspecies ++//
                        if ((ph_prop_2 == "DC") && (!row[i].empty())) // check if there is species data in the CSV header
                        {
                            bson_append_start_array(&exp, phspecies);
                            for (unsigned int j=0; j<headline.size(); ++j)
                            {
                                if ((strncmp(headline[j].c_str(),phase, 5) == 0) && (!row[j].empty())) // check for pahse. key
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
                                            pos_start = -1;
                                            pos_end   = ph_prop.find(f1);
                                            dcomp_name = ph_prop.substr((pos_start+f1.length()),(pos_end-pos_start-f1.length()));
                                            dcomp_new = dcomp_name;
                                            h_dcomp = false;

                                            if ((dcomp_new != dcomp_old))
                                            {
                                                // check if pahse name was not present before
                                                for (unsigned int j=0; j<dcomps.size(); ++j)
                                                {
                                                    if (dcomp_name == dcomps[j])
                                                    {
                                                        h_dcomp = true;
                                                    }
                                                }

                                                if (!h_dcomp) // START if h_dcomp
                                                {
                                                    string dcomp_prop;
                                                    bson_append_start_object(&exp, boost::lexical_cast<string>(dcc).c_str()); // START species object
                                                    dcc++;
                                                    bson_append_string(&exp, species, dcomp_name.c_str());
                                                    dcomps.push_back(dcomp_name);
                                                    ph_dcomp += "DC." + dcomp_name + ".";
                                                    ic = 0;

                                                    // loop to get all the properties of current dcomp
                                                    bson_append_start_array(&exp, dcompprop);
                                                    for (unsigned int j=0; j<headline.size(); ++j)
                                                    {
//                                                        if (j== 30 || j ==33)
//                                                        {
//                                                            cout << headline[j].c_str() <<" "<< ph_dcomp.c_str() << " " << ph_dcomp.size() << endl;
//                                                        }
                                                        if ((strncmp(headline[j].c_str(),ph_dcomp.c_str(), ph_dcomp.size()) == 0) && (!row[j].empty()))
                                                        {
                                                            cout << j << endl;

                                                            pos_start = ph_dcomp.size();
                                                            pos_end = headline[j].size();
                                                            dcomp_prop = headline[j].substr(pos_start, pos_end);


                                                            bson_append_start_object(&exp, boost::lexical_cast<string>(ic).c_str()); // START property object
                                                            ic++;
                                                            bson_append_string(&exp, property, dcomp_prop.c_str());
                                                            bson_append_double(&exp, pQnt, atof(row[j].c_str())); // quantity of the property

                                                            // checking if there are errors and units included in the CSV and adding tem in the database
                                                            if (j+1 < headline.size())
                                                            {
                                                                if ((headline[j+1]==_error))
                                                                {
                                                                    ++j;
                                                                    if ((!row[j].empty()))
                                                                    {
                                                                        bson_append_double(&exp, Qerror, atof(row[j].c_str()));
                                                                    }
                                                                }
                                                                if ((headline[j+1]==_unit) && (!row[j+1].empty()))
                                                                {
                                                                    ++j;
                                                                    bson_append_string(&exp, Qunit, row[j].c_str());
                                                                }
                                                            }
                                                            bson_append_finish_object(&exp); // END property object
                                                            dcomp_old = dcomp_name;
                                                        }

                                                    }
                                                    bson_append_finish_array(&exp); // END dcompprop array
                                                }
                                            }
                                            ph_old = phase_name;
                                        }
                                    }
                                    dcomps.clear();
                                }
                            }
                            //++ END array phspecies ++//
                            bson_append_finish_array(&exp);
                        }
                        // end object in the array phases
                        bson_append_finish_object(&exp); // END phase object
                    } // END if h_phases
                } // END ph_new != ph_old
            } // END check for key pahse in the headline
        }
        //++ END array expphases ++//
        bson_append_finish_array(&exp);
        ph_old = ""; // reseting the old phase name after adding one experiment
        phases.clear(); // clearing the vector that holds the pahses in one experiment
        h_phases = false; // reseting the phases handle that cheks if one phase key was not present before

        bson_finish(&exp);
        ejdbsavebson(coll, &exp, &oid); // saving the document in the database
        bson_destroy(&exp);
    } // END getting data from CSV

    in.close();
}
