/// \file data_manager.cpp
/// Implementation of reading database parameters form input file
/// and experimental data form databsse
//
// Copyright (C) 2013-2014 G.D.Miron, D.Kulik
// <GEMS Development Team, mailto:gems2.support@psi.ch>
//
// This file is part of the GEMSFIT2 code for parameterization of thermodynamic
// data and models <http://gems.web.psi.ch/GEMSFIT2/>
//
// GEMSIFT is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.

// GEMSFIT2 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with GEMSFIT2 code. If not, see <http://www.gnu.org/licenses/>.
//-------------------------------------------------------------------
//
//

/**
 *	@file data_manager.cpp
 *
 *	@brief this source file contains the implementation of the data manager class,
 *	which retrieves and stores data from thre GEMSFIT2 input file as well as
 *	the measurement form an EJDB local database.
 *
 *	@author G. Dan Miron
 *
 * 	@date 26.03.2013
 *
 */

#include <fstream>
#include "data_manager.h"
#include "keywords.h"
#include "f_database.h"

std::ostream& operator<<(std::ostream& stream, const Data_Manager::DataSynonyms& data);
std::ostream& operator<<(std::ostream& stream, const Data_Manager::PhSyn& data);
std::ostream& operator<<(std::ostream& stream, const Data_Manager::samples& data);

// Constructor
Data_Manager::Data_Manager( )
{
    mode = gpf->KeysNdx;
    h_datasetlist = false;
    // Read parameters for database connection
    gpf->flog << "02. data_manager.cpp(51). Reading database parameter get_db_specs(); " << std::endl;
    get_db_specs_txt();

    // Getting the query result data into the Data_Manager class
    gpf->flog << "03. data_manager.cpp(55). Getting data form the EJDB database; " << std::endl;
    get_EJDB();

    gpf->flog << "04. data_manager.cpp(58). Getting distinct T and P pairs; " << std::endl;
    get_distinct_TP();

    get_DataSyn();
    check_Syn();
    mode = gpf->KeysNdx;
}


// Destructor
Data_Manager::~Data_Manager( )
{
    // Delete measurement data std::vector of experiments
    for (unsigned int i=0; i<experiments.size(); ++i)
    {
        delete experiments[i];
        delete bICv[i];
    }  
}

void Data_Manager::get_DataSyn()
{
    Data_Manager::DataSynonyms ss;
    Data_Manager::PhSyn sss;
    common::JsonFree data_syn_object = fromJsonString(DataSyn);

    if( data_syn_object.contains(keys::PhNames[mode]) ) {
        for (const auto& element : data_syn_object[keys::PhNames[mode]]) {
            SynPHs.push_back(sss);
            SynPHs.back().GemsName  = element->value(keys::NameSys[mode], std::string());
            SynPHs.back().syn = element->value(keys::Syn[mode], std::vector<std::string>{});

            if( element->contains(keys::DcNames[mode]) ) {
                for (const auto& sub_element : (*element)[keys::DcNames[mode]]) {
                    SynPHs.back().SynDCs.push_back(ss);
                    SynPHs.back().SynDCs.back().GemsName  = sub_element->value(keys::NameSys[mode], std::string());
                    SynPHs.back().SynDCs.back().syn = sub_element->value(keys::Syn[mode], std::vector<std::string>{});
                }
            }
         }
    }

    if( data_syn_object.contains(keys::PhPropNames[mode]) ) {
        for (const auto& element : data_syn_object[keys::PhPropNames[mode]]) {
            SynPHPs.push_back(ss);
            SynPHPs.back().GemsName  = element->value(keys::NameSys[mode], std::string());
            SynPHPs.back().syn = element->value(keys::Syn[mode], std::vector<std::string>{});
        }
    }

    if( data_syn_object.contains(keys::PropertyNames[mode]) ) {
        for (const auto& element : data_syn_object[keys::PropertyNames[mode]]) {
            SynProps.push_back(ss);
            SynProps.back().GemsName  = element->value(keys::NameSys[mode], std::string());
            SynProps.back().syn = element->value(keys::Syn[mode], std::vector<std::string>{});
        }
    }

    if( data_syn_object.contains(keys::DcPropNames[mode]) ) {
        for (const auto& element : data_syn_object[keys::DcPropNames[mode]])  {
            SynDCPs.push_back(ss);
            SynDCPs.back().GemsName  = element->value(keys::NameSys[mode], std::string());
            SynDCPs.back().syn = element->value(keys::Syn[mode], std::vector<std::string>{});
        }
    }
#ifdef CHECK_LOAD
    std::fstream test_out("datasyn.log", std::ios::out);
    test_out << "SynPHs\n";
    for (auto const& item : SynPHs) {
           test_out << item << "\n";
    }
    test_out << "SynPHPs\n";
    for (auto const& item : SynPHPs) {
           test_out << item << "\n";
    }
    test_out << "SynDCPs\n";
    for (auto const& item : SynDCPs) {
           test_out << item << "\n";
    }
    test_out << "SynProps\n";
    for (auto const& item : SynProps) {
           test_out << item << "\n";
    }
#endif
}

void Data_Manager::check_Syn()
{
    // phases
    for (unsigned p = 0; p < SynPHs.size(); p++)
    {
        for (unsigned i = 0; i < experiments.size(); i++)
        {
            // phase name
            for (unsigned ep = 0; ep < experiments[i]->expphases.size(); ep++)
            {
                bool check = false;
                for (unsigned s = 0; s < SynPHs[p].syn.size(); s++)
                {
                    if ( (experiments[i]->expphases[ep]->phase == SynPHs[p].syn[s]))
                        check = true;
                    if (check)
                    {
                        experiments[i]->expphases[ep]->phase = SynPHs[p].GemsName;
                        check = false;
                        break;
                    }
                }
            }

            // DCs
            for (unsigned d = 0; d < SynPHs[p].SynDCs.size(); d++)
            {
                // DC name
                for (unsigned ep = 0; ep < experiments[i]->expphases.size(); ep++)
                {
                    for (unsigned dc = 0; dc < experiments[i]->expphases[ep]->phDC.size(); dc++)
                    {
                        bool check = false;
                        for (unsigned s = 0; s < SynPHs[p].SynDCs[d].syn.size(); s++)
                        {
                            if ((experiments[i]->expphases[ep]->phDC[dc]->DC == SynPHs[p].SynDCs[d].syn[s]) && (experiments[i]->expphases[ep]->phase == SynPHs[p].GemsName))
                                check = true;
                            if (check)
                            {
                                experiments[i]->expphases[ep]->phDC[dc]->DC = SynPHs[p].SynDCs[d].GemsName;
                                check = false;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    // DC properties
    for (unsigned d = 0; d < SynDCPs.size(); d++)
    {
        for (unsigned i = 0; i < experiments.size(); i++)
        {
            // DC prop
            for (unsigned ep = 0; ep < experiments[i]->expphases.size(); ep++)
            {
                for (unsigned dc = 0; dc < experiments[i]->expphases[ep]->phDC.size(); dc++)
                {
                    for (unsigned dcp = 0; dcp < experiments[i]->expphases[ep]->phDC[dc]->DCprop.size(); dcp++ )
                    {
                        bool check = false;
                        for (unsigned s = 0; s < SynDCPs[d].syn.size(); s++)
                        {
                            if ((experiments[i]->expphases[ep]->phDC[dc]->DCprop[dcp]->property == SynDCPs[d].syn[s]))
                                check = true;
                            if (check)
                            {
                                experiments[i]->expphases[ep]->phDC[dc]->DCprop[dcp]->property = SynDCPs[d].GemsName;
                                check = false;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    // Phase properties
    for (unsigned p = 0; p < SynPHPs.size(); p++)
    {
        for (unsigned i = 0; i < experiments.size(); i++)
        {
            // phase prop
            for (unsigned ep = 0; ep < experiments[i]->expphases.size(); ep++)
            {
                for (unsigned php = 0; php < experiments[i]->expphases[ep]->phprop.size(); php++ )
                {
                    bool check = false;
                    for (unsigned s = 0; s < SynPHPs[p].syn.size(); s++)
                    {
                        if ( (experiments[i]->expphases[ep]->phprop[php]->property == SynPHPs[p].syn[s]))
                            check = true;
                        if (check)
                        {
                            experiments[i]->expphases[ep]->phprop[php]->property = SynPHPs[p].GemsName;
                            check = false;
                            break;
                        }
                    }
                }
            }
        }
    }

}

void Data_Manager::get_db_specs_txt()
{
    int mode = gpf->KeysNdx;
    std::string fname = gpf->OptParamFile();
    std::ifstream file(fname);
    std::ostringstream tmp;
    tmp<<file.rdbuf();
    std::string s = tmp.str();
    //    std::std::cout<<s<<std::std::endl;

    common::JsonFree db_specs_object = fromJsonString(s);
    DBname = db_specs_object.value(keys::DBPath[0], std::string());
    if (DBname.empty()) {
        DBname = db_specs_object.value(keys::DBPath[1], DBname);
        mode = 1;
        gpf->KeysNdx = 1;
    }
    if (DBname.empty())  {
        std::cout << "Error: No keyword for \""
                  << keys::DBPath[0]<<"\" or \""
                  << keys::DBPath[1]<<"\" found in the task definition" << std::endl;
        exit(1);
    }
    collection = db_specs_object.value(keys::DBColl[mode], std::string());
    if (collection.empty()) {
        std::cout << "Error: No keyword for \""
                  << keys::DBColl[mode]<<"\" found in the task definition" << std::endl;
        exit(1);
    }
    DataSelect = db_specs_object[keys::DSelect[mode]].dump();
    if (DataSelect.empty()) {
        std::cout << "Error: No keyword for \""
                  <<keys::DSelect[mode]<<"\" found in the task definition"
                 << std::endl;
        exit(1);
    }
    DataTarget = db_specs_object[keys::DTarget[mode]].dump();
    if (DataSelect.empty())  {
        std::cout << "Error: No keyword for \""
                  << keys::DTarget[mode] <<"\" found in the task definition" << std::endl;
        exit(1);
    }
    LogK = db_specs_object.value(keys::LogK[mode], std::vector<std::string>{});

    std::string gems_path = db_specs_object.value(keys::G3Ksys[mode], std::string());
    if (gems_path.empty()) {
        std::cout << "Error: No keyword for \""
                  << keys::G3Ksys[mode]<<"\" found in the task definition" << std::endl;
        exit(1);
    }
#ifdef buildWIN32
    std::replace( out[0].begin(), out[0].end(), '/', '\\');
#endif
    gpf->setGEMS3LstFilePath(gems_path);

    DataLogK = db_specs_object.value(keys::DatLogK[mode], std::string());
    DataSyn = db_specs_object[keys::DataSyn[mode]].dump();
    //OptSet = db_specs_object[keys::OptSet[mode]].dump();
    OptSet = db_specs_object.value(keys::OptSet[mode], std::string());
    if (OptSet.empty()) {
        std::cout << "Error: No keyword for \""
                  <<keys::OptSet[mode]<<"\" found in the task definition" << std::endl;
        exit(1);
    }
    MPI = db_specs_object.value(keys::MPI[mode], MPI);
}

// Reading data from EJDB database
//"DataSelect" : { "usedatasets" : ["CH04", "CH04D"],
//                 "skipdatasets" : ["CH03", "CH03D"],
//                 "usesamples" : [],
//                 "skipsamples" : [],
//                 "usepairs" : [],
//                 "skippairs" : [
//                       { "dataset": "CHO4", "samples": ["01", "02"]},
//                       { "dataset": "CHO4D", "samples": ["01", "02"]},
//                     ],
//                  "sT" : [0, 1000],
//                  "sP" : [0, 500]
//},
void Data_Manager::get_EJDB()
{
    typedef std::vector<double>  double_v;
    typedef std::vector<std::string>  string_v;
    string_v usesample, skipsample, usedataset, skipdataset, skippdatasets,skippsamples, usepdatasets, usepsamples, SA, DS;
    double_v qsT, qsP, WT;
    unsigned int Nsamples = 0 , Ndatasets = 0;

    common::JsonFree data_sel_object = fromJsonString(DataSelect);
    {
        // processing DataSelect
        usesample = data_sel_object.value(keys::usesamples, usesample);
        skipsample = data_sel_object.value(keys::skipsamples, skipsample);
        usedataset = data_sel_object.value(keys::usedatasets, usedataset);
        skipdataset = data_sel_object.value(keys::skipdatasets, skipdataset);

        if( data_sel_object.contains(keys::usepair) ) {
            for (const auto& element : data_sel_object[keys::usepair])
            {
                string_v templ;
                templ = element->value(keys::dataset, templ);
                if(templ.empty())
                    templ = element->value(keys::usedatasets, templ);
                usepdatasets.insert(usepdatasets.end(), templ.begin(), templ.end());
                templ.clear();
                templ = element->value(keys::samples, templ);
                usepsamples.insert(usepsamples.end(), templ.begin(), templ.end());
            }
        }

        if( data_sel_object.contains(keys::skippair) ) {
            for (const auto& element : data_sel_object[keys::skippair])
            {
                string_v templ;
                templ = element->value(keys::dataset, templ);
                if(templ.empty())
                    templ = element->value(keys::usedatasets, templ);
                skippdatasets.insert(skippdatasets.end(), templ.begin(), templ.end());
                templ.clear();
                templ = element->value(keys::samples, templ);
                skippsamples.insert(skippsamples.end(), templ.begin(), templ.end());
            }
        }

        qsT = data_sel_object.value(keys::sT, qsT);
        qsP = data_sel_object.value(keys::sP, qsP);

        if( data_sel_object.contains(keys::samplelist) ) {
            Nsamples = data_sel_object[keys::samplelist].size();
            for (const auto& element : data_sel_object[keys::samplelist])
            {
                // need test only one element
                SA.push_back(element->value(keys::SA, std::string()));
                DS.push_back(element->value(keys::DS, std::string()));
                WT.push_back(element->value(keys::WT, 0.));
            }
        }

        if( data_sel_object.contains(keys::datasetlist) ) {
            Ndatasets = data_sel_object[keys::datasetlist].size();
            for (const auto& element : data_sel_object[keys::datasetlist])
            {
                // need test only one element
                DS.push_back(element->value(keys::DS, std::string()));
                WT.push_back(element->value(keys::WT, 0.));
            }
        }
        if( Ndatasets>0 && Nsamples>0 )
        {
            std::cout << "You can't have samplelist and datasetlist options at the same time. Use only one at a time. " << std::endl;
            exit(1);
        }
    }

    // generate query
    common::JsonFree query_object;
    if (Nsamples == 0) {
        // for selecting expdatasets
        if(usedataset.size() > 0 || Ndatasets > 0) {
            auto temp = usedataset;
            temp.insert(temp.end(), DS.begin(), DS.end());
            std::cout << temp[0] << "  " << temp[1] << std::endl;
            query_object[keys::expdataset]["$in"] = temp;
            this->h_datasetlist = (Ndatasets > 0);
        }

        // for skipping expdatasets
        if (skipdataset.size() > 0 && !skipdataset[0].empty()) {
            query_object[keys::expdataset]["$nin"] = skipdataset;
        }

        // for selecting usesamples
        if (usesample.size() > 0 && !usesample[0].empty()) {
            query_object[keys::expsample]["$in"] = usesample;
        }

        // for skipping skipsamples
        if (skipsample.size() > 0 && !skipsample[0].empty()) {
            query_object[keys::expsample]["$nin"] = skipsample;
        }

        // for selection of temperatures
        if (qsT.size() > 0) {
            if ((qsT.size() == 2)) {
                // for selecting T interval
                query_object[keys::sT]["$bt"] = qsT;
            }
            else if (!(qsT[0] == 0)) {
                query_object[keys::sT]["$in"] = qsT;
            }
        }

        // for selection of pressures
        if (qsP.size() > 0) {
            if ((qsP.size() == 2)) {
                // for selecting P interval
                query_object[keys::sP]["$bt"] = qsP;
            }
            else if (!(qsP[0] == 0)) {
                query_object[keys::sP]["$in"] = qsP;
            }
        }

        // for selecting pairs of datasets-samples
        if (usepdatasets.size() > 0 && !usepdatasets[0].empty()) {
            unsigned int k=0; // counts trough the usesamples std::vector
            auto arr = common::JsonFree::array();
            for (unsigned int j=0; j<usepdatasets.size(); ++j)  {
                while (k<usepsamples.size())  {
                    if (!(usepsamples[k] == "")) {
                        auto obj = common::JsonFree::object();
                        obj["$and"].push_back(common::JsonFree::parse(std::string("{\"")+keys::expdataset+"\":\""+usepdatasets[j]+"\"}"));
                        obj["$and"].push_back(common::JsonFree::parse(std::string("{\"")+keys::expsample+"\":\""+usepsamples[k]+"\"}"));
                        arr.push_back(std::move(obj));
                    }
                    else   {
                        k++;
                        break;
                    }
                    k++;
                }
                query_object["$or"] = arr;
            }
        }
    }
    else // if we have samples list the search query is bulil as follows
    {
        // for selecting pairs of datasets-samples
        if (Nsamples > 0)  {
            this->h_datasetlist = true;
            if(DS.size() > 0 && !DS[0].empty())     {
                auto arr = common::JsonFree::array();
                for  (unsigned k = 0; k<SA.size(); k++) {
                    auto obj = common::JsonFree::object();
                    obj["$and"].push_back(common::JsonFree::parse(std::string("{\"")+keys::expdataset+"\":\""+DS[k]+"\"}"));
                    obj["$and"].push_back(common::JsonFree::parse(std::string("{\"")+keys::expsample+"\":\""+SA[k]+"\"}"));
                    arr.push_back(std::move(obj));
                }
                query_object["$or"] = arr;
            }
        }
    }

#ifdef CHECK_LOAD
    std::fstream ff3("DBquery.json", std::ios::out );
    ff3 << query_object.dump();
    ff3.close();
#endif

    // create database connect
    std::cout << DBname.c_str() << std::endl;
    TEJDataBase::dbdriver->change_path(DBname);
    TEJDataBase::dbdriver->connect();

    common::SetReaded_f setfnc = [&](const std::string& json_rec)
    {
        experiments.push_back( new samples );
        experiments.back()->idsample= 0;
        // adding the data returned by the selection query into the data storage class
        bson_to_Data_Manager(json_rec, experiments.size()-1);
    };

    gpf->flog << "05. data_manager.cpp(365). Adding the data returned by the selection query into the data structure; " << std::endl;
    TEJDataBase::dbdriver->select_query_omp(collection, query_object.dump(), setfnc, this->MPI);
    TEJDataBase::dbdriver->disconnect();

    // Set weights provided in the sample list
    if (Nsamples > 0)
    {
        for (unsigned i = 0; i<experiments.size(); i++)
        {
            for (unsigned j = 0; j<Nsamples; j++)
            {
                if ((experiments[i]->sample == SA[j]) && (experiments[i]->expdataset == DS[j]))
                {
                    experiments[i]->weight = experiments[i]->weight * WT[j];
                }
            }
        }
    }
    // Set weights provided in the dataset list
    if (Ndatasets > 0)
    {
        for (unsigned i = 0; i<experiments.size(); i++)
        {
            for (unsigned j = 0; j<Ndatasets; j++)
            {
                if (experiments[i]->expdataset == DS[j])
                {
                    experiments[i]->weight = experiments[i]->weight * WT[j];
                }
            }
        }
    }

    // for skipping expdataset-expsamples pairs
    if (skippdatasets.size() > 0)
    {
        if (!skippdatasets[0].empty())
        {
            unsigned int k=0; // counts torugh the skippsamples std::vector
            for (unsigned int j=0; j<skippdatasets.size(); ++j)
            {
                //                    for (k; k<skippsamples.size(); k++)
                while (k<skippsamples.size())
                {
                    if (!(skippsamples[k] == ""))
                    {
                        for (unsigned int i = 0; i<this->experiments.size(); i++)
                        {
                            if ((this->experiments[i]->expdataset == skippdatasets[j]) && (this->experiments[i]->sample == skippsamples[k]))
                            {
                                this->experiments.erase(this->experiments.begin() + i);
                            }
                        }

                    } else
                    {
                        k++;
                        break;
                    }
                    k++;
                }
            }
        }
        fprintf(stderr, "Records after removing skipped pairs (dataset-samples): %ld\n", experiments.size());
    }
#ifdef CHECK_LOAD
    std::fstream test_out("experiments.log", std::ios::out);
    for (auto const& item : experiments) {
           test_out << *item << "\n";
    }
#endif
}

std::string Data_Manager::default_unit_to_property(const std::string& property_name) const
{
    // assigining default values for units
    std::string def_unit;// = keys::molratio;
    if (property_name==keys::Qnt) {
        def_unit = keys::gram;
    }
    else if (property_name==keys::RHO) {
        def_unit = keys::g_cm3;
    }
    else if (property_name==keys::Gex) {
        def_unit = keys::J_mol;
    }
    else if (property_name==keys::pV) {
        def_unit = keys::cm3;
    }
    else if (property_name==keys::pH) {
        def_unit = keys::_loga;
    }
    else if (property_name==keys::Eh) {
        def_unit = keys::Volts;
    }
    else if (property_name==keys::pe) {
        def_unit = keys::_loga;
    }
    else if (property_name==keys::oscw) {
        //def_unit = keys::_loga;
    }
    return def_unit;
}

void Data_Manager::bson_to_Data_Manager(const std::string& data, int pos)
{
    common::JsonFree data_object = fromJsonString(data);

    experiments[pos]->expdataset = data_object.value(keys::expdataset, std::string());
    experiments[pos]->sample = data_object.value(keys::expsample, std::string());
    experiments[pos]->Type = data_object.value(keys::type, std::string());
    experiments[pos]->sT = data_object.value(keys::sT, 0.);
    experiments[pos]->sP = data_object.value(keys::sP, 0.);
    experiments[pos]->sV = data_object.value(keys::sV, 0.);
    experiments[pos]->weight = data_object.value(keys::Weight, 1.);

    // adding experiment components/recipe
    if( data_object.contains(keys::sbcomp) ) {
        for (const auto& element : data_object[keys::sbcomp]) {
            experiments[pos]->sbcomp.push_back( new samples::components );
            experiments[pos]->sbcomp.back()->comp = element->value(keys::comp, std::string());
            experiments[pos]->sbcomp.back()->Qnt = element->value(keys::Qnt, 0.);
            experiments[pos]->sbcomp.back()->Qerror = element->value(keys::Qerror, 1.);
            experiments[pos]->sbcomp.back()->Qunit = element->value(keys::Qunit, keys::gram);
        }
    }

    // adding experiment components/recipe
    if( data_object.contains(keys::props) ) {
        for (const auto& element : data_object[keys::props]) {
            experiments[pos]->props.push_back( new samples::properties );
            experiments[pos]->props.back()->prop = element->value(keys::property, std::string());
            experiments[pos]->props.back()->Qnt = element->value(keys::Qnt, 0.);
            experiments[pos]->props.back()->Qerror = element->value(keys::Qerror, 0.);
            experiments[pos]->props.back()->Qunit = element->value(keys::Qunit, "NULL");
        }
    }

    // adding experiment components/recipe
    if( data_object.contains(keys::expphases) ) {
        for (const auto& element : data_object[keys::expphases])  {
            experiments[pos]->expphases.push_back( new samples::phases );
            experiments[pos]->expphases.back()->idphase = 0;
            experiments[pos]->expphases.back()->phase = element->value(keys::phase, std::string());

            if( element->contains(keys::phIC) ) {
                for (const auto& sub_el : (*element)[keys::phIC])  {
                    experiments[pos]->expphases.back()->phIC.push_back( new samples::components );
                    std::string def_unit; // =  (experiments[pos]->expphases.back()->phase == keys::aqueous ? keys::molal: keys::molratio);
                    experiments[pos]->expphases.back()->phIC.back()->comp = sub_el->value(keys::IC, std::string());
                    experiments[pos]->expphases.back()->phIC.back()->Qnt = sub_el->value(keys::Qnt, 0.);
                    experiments[pos]->expphases.back()->phIC.back()->Qerror = sub_el->value(keys::Qerror, 1.);
                    experiments[pos]->expphases.back()->phIC.back()->Qunit = sub_el->value(keys::Qunit, def_unit);
                }
            }

            // adding phase IC as mole fractions MR
            if( element->contains(keys::phMR) ) {
                for (const auto& sub_el : (*element)[keys::phMR]) {
                    experiments[pos]->expphases.back()->phMR.push_back( new samples::components );
                    std::string def_unit = keys::molratio;
                    // =  (experiments[pos]->expphases.back()->phase == keys::aqueous ? keys::molal: keys::molratio);
                    experiments[pos]->expphases.back()->phMR.back()->comp = sub_el->value(keys::MR, std::string());
                    experiments[pos]->expphases.back()->phMR.back()->Qnt = sub_el->value(keys::Qnt, 0.);
                    experiments[pos]->expphases.back()->phMR.back()->Qerror = sub_el->value(keys::Qerror, 1.);
                    experiments[pos]->expphases.back()->phMR.back()->Qunit = sub_el->value(keys::Qunit, def_unit);
                }
            }

            // adding phase properties
            if( element->contains(keys::phprop) ) {
                for (const auto& sub_el : (*element)[keys::phprop]) {
                    experiments[pos]->expphases.back()->phprop.push_back( new samples::phases::prop );
                    std::string p_name = sub_el->value(keys::property, std::string());
                    experiments[pos]->expphases.back()->phprop.back()->property = p_name;
                    std::string def_unit =default_unit_to_property(p_name);
                    experiments[pos]->expphases.back()->phprop.back()->Qnt = sub_el->value(keys::Qnt, 0.);
                    experiments[pos]->expphases.back()->phprop.back()->Qerror = sub_el->value(keys::Qerror, 1.);
                    experiments[pos]->expphases.back()->phprop.back()->Qunit = sub_el->value(keys::Qunit, def_unit);
                }
            }

            // adding phase dcomps
            if( element->contains(keys::phDC) ) {
                for (const auto& sub_el : (*element)[keys::phDC]) {
                    experiments[pos]->expphases.back()->phDC.push_back( new samples::phases::dcomps );
                    experiments[pos]->expphases.back()->phDC.back()->DC = sub_el->value(keys::MR, std::string());;
                    // adding dependent compoponents properties
                    if( sub_el->contains(keys::DCprop) ) {
                        for (const auto& sub_el2 : (*sub_el)[keys::DCprop])  {
                            experiments[pos]->expphases.back()->phDC.back()->DCprop.push_back( new samples::phases::dcomps::dcprop );
                            std::string def_unit; // = keys::molratio;
                            // =  (experiments[pos]->expphases.back()->phase == keys::aqueous ? keys::molal: keys::molratio);
                            experiments[pos]->expphases.back()->phDC.back()->DCprop.back()->property = sub_el2->value(keys::property, std::string());
                            experiments[pos]->expphases.back()->phDC.back()->DCprop.back()->Qnt = sub_el2->value(keys::Qnt, 0.);
                            experiments[pos]->expphases.back()->phDC.back()->DCprop.back()->Qerror = sub_el2->value(keys::Qerror, 1.);
                            experiments[pos]->expphases.back()->phDC.back()->DCprop.back()->Qunit = sub_el2->value(keys::Qunit, def_unit);
                        }
                    }
                }
            }
        }
    }

}

void Data_Manager::get_distinct_TP( )
{
    std::vector<double> TP[2];
    unsigned int i, j;
    bool isfound = false, isfound2 = false;

    for (i=0; i<experiments.size(); ++i)
    {
        TP[0].push_back(experiments[i]->sT);
        TP[1].push_back(experiments[i]->sP);
    }
    for (i=0; i<TP[0].size(); i++)
    {
        // check if TP pair is presnt more than once in the TP std::vector
        for (j=0; j<TP[0].size(); j++)
        {
            if ((TP[0][i] == TP[0][j]) && (TP[1][i] == TP[1][j]) && (i != j))
            {
                isfound = true;
            }
        }
        // check if TP pair was added to the unique TP pairs container
        for (j=0; j<TP_pairs[0].size(); ++j)
        {
            if ((TP[0][i] == TP_pairs[0][j]) && (TP[1][i] == TP_pairs[1][j]))
            {
                isfound2 = true;
            }
        }
        // add TP pair if it does not repeat itself or was not added already in the container
        if ((!isfound) || (!isfound2))
        {
            TP_pairs[0].push_back(TP[0][i]);
            TP_pairs[1].push_back(TP[1][i]);
        }
        isfound = false;
        isfound2 = false;
    }   
}

std::ostream& operator<<(std::ostream& stream, const Data_Manager::DataSynonyms& data) {
    stream << data.GemsName << " : ";
    for (auto const& item : data.syn) {
        stream << "" << item << "\n";
    }
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const Data_Manager::PhSyn& data) {
    stream << data.GemsName << " : ";
    for (auto const& item : data.syn) {
        stream << " " << item;
    }
    stream << "\nSynDCs\n";
    for (auto const& item : data.SynDCs) {
        stream << "   " << item << "\n";
    }
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const Data_Manager::samples& data_lst) {
    stream << "----------------------------\n";
    stream << data_lst.idsample << " " << data_lst.sample << " " << data_lst.expdataset << " " << data_lst.Type << " " << "\n";
    stream << data_lst.weight << " " << data_lst.sT << " " << data_lst.Tunit << " " << data_lst.sP << " "
           << data_lst.Punit << " " << data_lst.sV << " " << data_lst.Vunit << "\n";

    stream << " sbcomp: \n";
    for (auto const& item : data_lst.sbcomp) {
        stream << "    " << item->comp << " " << item->Qnt << " " << item->Qerror << " " << item->Qunit  << "\n";
    }
    stream << " props: \n";
    for (auto const& item : data_lst.props) {
        stream << "    " << item->prop << " " << item->Qnt << " " << item->Qerror << " " << item->Qunit  << "\n";
    }
    stream << " expphases: \n";
    for (auto const& item_ph : data_lst.expphases) {
        stream << item_ph->idphase << " " << item_ph->phase << " " << "\n";
        stream << " phprop: \n";
        for (auto const& item : item_ph->phprop) {
            stream << "    " << item->property << " " << item->Qnt << " " << item->Qerror << " " << item->Qunit  << "\n";
        }
        stream << " phIC: \n";
        for (auto const& item : item_ph->phIC) {
            stream << "    " << item->comp << " " << item->Qnt << " " << item->Qerror << " " << item->Qunit  << "\n";
        }
        stream << " phMR: \n";
        for (auto const& item : item_ph->phMR) {
            stream << "    " << item->comp << " " << item->Qnt << " " << item->Qerror << " " << item->Qunit  << "\n";
        }
        stream << " phDC: \n";
        for (auto const& item_dc : item_ph->phDC) {
            stream << item_dc->DC << "\n";
            for (auto const& item : item_dc->DCprop) {
                stream << "    " << item->property << " " << item->Qnt << " " << item->Qerror << " " << item->Qunit  << "\n";
            }
        }
    }
    stream << std::endl;
    return stream;
}
