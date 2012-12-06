/*
*	 Copyright (C) 2012 by Ferdinand F. Hingerl (hingerl@hotmail.com)
*    Modified for fitting standard state properties (2012)
*    by G. Dan Miron (mirondanro@yahoo.com)
*
*	 This file is part of the thermodynamic fitting program GEMSFIT.
*
*    GEMSFIT is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    GEMSFIT is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU  General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with GEMSFIT.  If not, see <http://www.gnu.org/licenses/>.
*/


/**
 *	@file system_properties.cpp
 *
 *	@brief this source file contains implementations of the System_Properties and SS_System_Properties class,
 *	which retrievs and stores data related to the chemical system setup.    
 *
 *	@author Ferdinand F. Hingerl, G. Dan Miron
 *
 * 	@date 09.04.2012 
 *
 */

#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <sstream>


#include "system_properties.h"

using namespace std;

SS_System_Properties::SS_System_Properties( )
{
    // GEMSFIT logfile
    const char path[200] = "output_GEMSFIT/SS_GEMSFIT.log";
    ofstream fout;
    fout.open(path, ios::app);
    if( fout.fail() )
    { cout<<"Output fileopen error"<<endl; exit(1); }

    // file containing the input parameters of the system and of the optimization class
    param_file  = "SS_INPUT/SS_GEMSFIT_input.dat";

    // For parameter optimization do not use printing of results
    printfile = false;

    // For parameter optimization do not use parallelization of the Monte Carlo loop (=true). Instead, execute loop over measurements within objective function in parallel (=false).
    MC_MPI = false;

    // Initialize data storage class
    fout << "2. system_properties.cpp line 63. Creating new SS_Data_Manager data_meas" << endl;
    data_meas = new SS_Data_Manager;

    // Initialize pointer to instance of sysprop object
    // std_prop is a struc containing the standard state properties at TP of exp and at reference value + other associated variables
    sysprop = new std_prop;
    fout << "8. system_properties.cpp line 68. getsysprop( sysprop ); reads system properties form the input file." << endl;
    getsysprop( sysprop );

    fout.close();

}

SS_System_Properties::~SS_System_Properties()
{
    // delete data_meas;
    delete sysprop;
}

// Function reads tandard state properties of the chemical system. This function reads from the input file the system properties
// and retrives from GEMS3K the standard state thermodynamic properites.
void SS_System_Properties::getsysprop( std_prop* sysprop )
{

    // Variable declarations
    vector<string> data;
    string line, allparam;
    string SysName_s, SpecName_s, LogSolubility_s;
    string sub_SysName, sub_SpecName, sub_LogSolubility;
    int pos_start, pos_end;
    unsigned int i, j;
    ifstream param_stream;

    // Keywords
    string f7("<SystemName>");
    string f8("<LogSolubility>");
    string f9("<FitSpeciesName>");
    string f4("#");


    // Read parameter file into string
    param_stream.open(param_file.c_str());
    if( param_stream.fail() )
    {
        cout << "Opening of file "<<param_file<<" failed !!"<<endl;
        exit(1);
    }
    while( getline(param_stream, line) )
    {
        data.push_back(line);
    }
    param_stream.close();
    for( i=0; i < data.size(); i++ )
    allparam += data[i];

    // GEMSFIT logfile
    const char path[200] = "output_GEMSFIT/SS_GEMSFIT.log";
    ofstream fout;
    fout.open(path, ios::app);
    if( fout.fail() )
    { cout<<"Output fileopen error"<<endl; exit(1); }

    // System name
    pos_start = allparam.find(f7);
    pos_end   = allparam.find(f4,pos_start);
    SysName_s = allparam.substr((pos_start+f7.length()),(pos_end-pos_start-f7.length()));
    istringstream SysName_ss(SysName_s);
    for( i=0; i<1; i++)
    {
        SysName_ss >> sub_SysName;
        system_name = sub_SysName;
    }
    fout<<endl<<"system_name = "<<system_name<<endl;

    // Log Solubility
    pos_start = allparam.find(f8);
    pos_end   = allparam.find(f4,pos_start);
    LogSolubility_s = allparam.substr((pos_start+f8.length()),(pos_end-pos_start-f8.length()));
    istringstream LogSolubility_ss(LogSolubility_s);
    for( i=0; i<1; i++)
    {
        LogSolubility_ss >> sub_LogSolubility;
        sysprop->log_solubility = atoi(sub_LogSolubility.c_str());
    }

    //species
    pos_start = allparam.find(f9);
    pos_end   = allparam.find(f4,pos_start);
    SpecName_s = allparam.substr((pos_start+f9.length()),(pos_end-pos_start-f9.length()));
    istringstream SpecName_ss(SpecName_s);
    do
    {
        SpecName_ss >> sub_SpecName;
        to_fit_species.push_back(sub_SpecName);
    }while(SpecName_ss);
    to_fit_species.pop_back(); // goes back a spot in the vector so there is no doubling of the last entry. Don't know whit it doubles??
    for ( i=0; i<to_fit_species.size(); i++)
    {
    fout<<"species["<<i<<"] = "<<to_fit_species[i]<<"; ";
    }

    // Assert that the parameter given in the GEMSFIT chemical system input file are compatible with the corresponding values in the GEMS3K input file
    long index_species = -1;
    char input_system_file_list_name[256];

    // get system file list name // system name is the same ast the GMS3K input .lst file
    strcpy(input_system_file_list_name, system_name.c_str());

    // call GEM_init to read GEMS3K input files
    TNode* node  = new TNode();
    // call GEM_init     --> read in input files
    if( (node->GEM_init( input_system_file_list_name )) == 1 )
    {
        cout<<" .. ERROR occurred while reading input files !!! ..."<<endl;
        exit(1);
    }

    for ( i=0; i<to_fit_species.size(); ++i )
    {
    // Get form GEMS the index of to_fit_species of interest
    try
    {
        index_species = node->DC_name_to_xCH( to_fit_species[i].c_str() );
        if( index_species < 0 )
        {
            throw index_species;
        }
        else
        {
            fit_species_ind.push_back(index_species);
        }
    }
    catch( long e )
    {
        cout<<endl;
        cout<<" Phase name in GEMSFIT chemical system file has no corresponding phase name in GEMS3K input file !!!! "<<endl;
        cout<<" Phase name given in GEMSFIT_input.dat: "<<to_fit_species[i]<<endl;
        cout<<" Can not proceed ... Bailing out now ... "<<endl;
        cout<<endl;
        exit(1);
    }
    }

    fout << endl << "Number of species to be fitted : " << fit_species_ind.size() << endl;

    // Check if temperature and pressure is in the DATACH lookup array
    for( i=0; i<data_meas->TP_pairs[0].size(); i++ )
    {
        if( (node->check_TP( (data_meas->TP_pairs[0][i]+273.15), (100000*data_meas->TP_pairs[1][i])) == false &&  node->check_grid_TP( data_meas->TP_pairs[0][i]+273.15, 100000 * data_meas->TP_pairs[1][i] ) < 0. ) /*&& node->check_grid_TP( sysdata->temperature.at(i), 100000 * sysdata->pressure.at(i) ) < 0*/ )
        {
            fout<<endl;
            fout<<" Measurement data: temperature "<<data_meas->TP_pairs[0][i]<<" together with pressure "<<data_meas->TP_pairs[1][i]<<" does not appear in the DATACH structure of GEMS3K !!!!"<<endl;
            //cout<<" bailing out now ... "<<endl;
            fout<<endl;
            //exit(1);
        }
    }


    // Get STD G0 at TP of the experiments and the std G0 at 273 and 1 bar and at the unique TP_pairs of the experiments
    fout << endl << "9. system_properties.cpp line 211. Getting G0 at 25 deg. C and 1 bar and at the unique TP pairs of the experiments into sysprop member of SS_System_properties : std_prop" << endl;

    // initializing std_gibbs for each specie std_gibbs[0] = the value for first specie
    for (i=0; i<fit_species_ind.size(); i++)
    {
        sysprop->std_gibbs.push_back(0.0);
    //        cout.setf(ios::fixed); // print without scientific notation
        sysprop->std_gibbs[i] =  node->DC_G0(fit_species_ind[i], 1e+05, 298.15, false); // Get G0 at 1 bar (1e+05 Pa) and 25 degrees C (298.15 K)
    //        cout << "The standard GO for " << to_fit_species[i] << " with index "<< fit_species_ind[i] << " is " << sysprop->std_gibbs[i] << endl;
    }
    //    cout<< sysprop->std_gibbs.size() << endl;

    // Gets the G0 at T and P of experiment for each specie
    for (i=0; i<fit_species_ind.size(); i++) // loops trough all species
    {
        vector<double> temp_v1, temp_v2;
     //            cout << " For " << to_fit_species[i] << endl;
        for (j=0; j<data_meas->TP_pairs[0].size(); j++) { // loops trough all unique TP_pairs
            temp_v1.push_back(0.0);
            temp_v2.push_back(0.0);
     //            cout << " P = " << data_meas->TP_pairs[1][j]*100000 << " T = " << data_meas->TP_pairs[0][j]+273.15 << " ";
            temp_v1[j] = node->DC_G0(fit_species_ind[i], data_meas->TP_pairs[1][j]*100000, data_meas->TP_pairs[0][j]+273.15, false);
     //            cout << temp_v[j] << endl;
            temp_v2[j]= temp_v1[j] - sysprop->std_gibbs[i]; // a. deltaG T,P = GT,P - G298, 1 (N=0) N – number of iterations
        }
        sysprop->std_gibbsTP.push_back(temp_v1);
        sysprop->dif_gibbs.push_back(temp_v2);
    }
    //    cout.setf(ios::fixed); // print without scientific notation
    //    cout << "The STD G0 at T= " << data_meas->TP_pairs[0][0] <<" P= " << data_meas->TP_pairs[1][0] << " of " << to_fit_species[2] << " = " << sysprop->std_gibbsTP[2][0] << endl;

    delete node;

    param_stream.close();

    fout.close();

}

// ----------------------------------------------- END SS_System_Properties CLASS -----------------------------------------------//



System_Properties::System_Properties( )
{

	param_file  = "GEMSFIT_input.dat";
	
	// For parameter optimization do not use printing of results
	printfile = false;

	// For parameter optimization do not use parallelization of the Monte Carlo loop (=true). Instead, execute loop over measurements within objective function in parallel (=false). 
	MC_MPI = false;

	// Initialize data storage class
	data_meas = new Data_Manager;

	// Initialize pointer to instance of sysparam object
	sysparam = new parameters;
	getparam( sysparam );

}


System_Properties::~System_Properties()
{
	delete data_meas;
	delete sysparam;
}


void System_Properties::getparam( parameters* sysparam )
{	

	// Variable declarations
	vector<string> data, aIPc_s1, aDCc_s1;
	string line, allparam;
    string aIPc_s, aDCc_s, aIPx_s, sub_aIPc, sub_aDCc, sub_aIPx, ActName_s;
	string LsMod_s, LsMdc_s, SysName_s, PhaName_s, SpecName_s, SpecCharge_s; 
	string sub_LsMod, sub_LsMdc, sub_SysName, sub_SpecCharge, sub_PhaName, sub_SpecName, sub_ActName;
	int pos_start, pos_end;
	unsigned int i;
 	ifstream param_stream;


	// Keywords
	string f7("<SystemName>");		
	string f8("<PhaseName>");		
	string f9("<DepComp>");	
	string f10("<DepCompCharge>");		
	string f11("<ActivityModel>");
	string f1("<IPxPH>");
	string f2("<PMc>");
	string f3("<DMc>");
	string f4("#");
	string f5("<LsMod>");
	string f6("<LsMdc>");		
	string sd("F");


	// Read parameter file into string 
	param_stream.open(param_file.c_str());	
	if( param_stream.fail() )
	{
		cout << "Opening of file "<<param_file<<" failed !!"<<endl;
		exit(1);
	}
	while( getline(param_stream, line) ) 
	{
		data.push_back(line);
	} 
	param_stream.close();
	for( i=0; i < data.size(); i++ )
	allparam += data[i];

	// GEMSFIT logfile
	const char path[200] = "output_GEMSFIT/GEMSFIT.log";
	ofstream fout;
	fout.open(path, ios::app);						
	if( fout.fail() )
	{ cout<<"Output fileopen error"<<endl; exit(1); }


	// System name
	pos_start = allparam.find(f7);
	pos_end   = allparam.find(f4,pos_start);
	SysName_s = allparam.substr((pos_start+f7.length()),(pos_end-pos_start-f7.length()));
	istringstream SysName_ss(SysName_s);
	for( i=0; i<1; i++)
	{
        SysName_ss >> sub_SysName;
		system_name = sub_SysName;
	}	
	fout<<"system_name = "<<system_name<<endl;

	// Phase name
	pos_start = allparam.find(f8);
	pos_end   = allparam.find(f4,pos_start);
	PhaName_s = allparam.substr((pos_start+f8.length()),(pos_end-pos_start-f8.length()));
	istringstream PhaName_ss(PhaName_s);
	for( i=0; i<1; i++)
	{
        PhaName_ss >> sub_PhaName;
		phase_name = sub_PhaName;
	}	
	fout<<"phase_name = "<<phase_name<<endl;


	//species
	pos_start = allparam.find(f9);
	pos_end   = allparam.find(f4,pos_start);
	SpecName_s = allparam.substr((pos_start+f9.length()),(pos_end-pos_start-f9.length()));
	istringstream SpecName_ss(SpecName_s);
	do
	{
        SpecName_ss >> sub_SpecName;
		species.push_back(sub_SpecName);
	}while(SpecName_ss);	
	fout<<"species[0] = "<<species[0]<<" | species[1] = "<<species[1]<<" | species[2] = "<<species[2]<<endl;


	//charges
	pos_start = allparam.find(f10);
	pos_end   = allparam.find(f4,pos_start);
	SpecCharge_s = allparam.substr((pos_start+f10.length()),(pos_end-pos_start-f10.length()));
	istringstream SpecCharge_ss(SpecCharge_s);
	do
	{
        SpecCharge_ss >> sub_SpecCharge;
		charges.push_back(atof(sub_SpecCharge.c_str()));
	}while(SpecCharge_ss);
	charges.pop_back();
	fout<<"charges[0] = "<<charges[0]<<" | charges[1] = "<<charges[1]<<" | charges[2] = "<<charges[2]<<endl;


	// dimensionalities of the parameter arrays
	pos_start = allparam.find(f5);
	pos_end   = allparam.find(f4,pos_start);
	LsMod_s = allparam.substr((pos_start+f5.length()),(pos_end-pos_start-f5.length()));
	istringstream LsMod_ss(LsMod_s);
	for( i=0; i<3; i++)
	{
        LsMod_ss >> sub_LsMod;
		if( i==0 ){	sysparam->rows_aIPc = atoi(sub_LsMod.c_str()); };
		if( i==1 ){	sysparam->cols_aIPx = atoi(sub_LsMod.c_str()); };
		if( i==2 ){	sysparam->cols_aIPc = atoi(sub_LsMod.c_str()); };		
	}	
	fout<<"sysparam->rows_aIPc = "<<sysparam->rows_aIPc<<endl;
	
	pos_start = allparam.find(f6);
	pos_end   = allparam.find(f4,pos_start);
	LsMdc_s = allparam.substr((pos_start+f6.length()),(pos_end-pos_start-f6.length()));
	istringstream LsMdc_ss(LsMdc_s);
	for( i=0; i<1; i++)
	{
        LsMdc_ss >> sub_LsMdc;
		sysparam->cols_aDCc = atoi(sub_LsMdc.c_str());
	}	
	fout<<"sysparam->cols_aDCc = "<<sysparam->cols_aDCc<<endl;

	// Get aIPx vector
	pos_start = allparam.find(f1);
	pos_end   = allparam.find(f4,pos_start);
	aIPx_s = allparam.substr((pos_start+f1.length()),(pos_end-pos_start-f1.length()));
	istringstream aIPx_ss(aIPx_s);	
 	do
	{
		aIPx_ss >> sub_aIPx;
		//cout<<"sub_aIPx = "<<sub_aIPx<<endl;
		sysparam->aIPx.push_back(atoi(sub_aIPx.c_str()));
	} while(aIPx_ss);
	sysparam->aIPx.pop_back();
	//cout<<sysparam->aIPx[0]<<" "<<sysparam->aIPx[1]<<" "<<sysparam->aIPx[2]<<" "<<sysparam->aIPx[3]<<endl;


	// Get aIPc vector and find first guesses (with indices)
	// Fill vectors aIPc, aIPc_fit_ind, aIPc_fit_val
	pos_start = allparam.find(f2);
	pos_end   = allparam.find(f4,pos_start);
	aIPc_s = allparam.substr((pos_start+f2.length()),(pos_end-pos_start-f2.length()));
	istringstream aIPc_ss(aIPc_s);
  	aIPc_ss.precision(::std::numeric_limits<double>::digits10);
	do
	{
		aIPc_ss >> sub_aIPc;
		aIPc_s1.push_back(sub_aIPc);
		//cout<<"sub_aIPc = "<<sub_aIPc<<endl;
		if( aIPc_s1[aIPc_s1.size()-1].compare(0,1,sd) == 0 )
		{
			sysparam->aIPc_fit_ind.push_back(aIPc_s1.size()-1);
			sysparam->aIPc_fit_val.push_back(atof(aIPc_s1[aIPc_s1.size()-1].erase(0,1).c_str()));
			sub_aIPc = aIPc_s1[aIPc_s1.size()-1];			
		};
			sysparam->aIPc.push_back(atof(sub_aIPc.c_str()));
	}while(aIPc_ss);
	sysparam->aIPc.pop_back();
	//cout<<sysparam->aIPc[8]<<" "<<sysparam->aIPc[9]<<" "<<sysparam->aIPc[10]<<" "<<sysparam->aIPc[11]<<endl;
	//cout<<"sysparam->aIPc_fit_ind[0] = "<<sysparam->aIPc_fit_ind[0]<<" "<<sysparam->aIPc_fit_ind[1]<<endl; //" "<<sysparam->aDCc[10]<<" "<<sysparam->aDCc[11]<<endl;

	// Get aDCc vector and find first guesses (with indices)
	// Fill vectors aDCc, aDCc_fit_ind, aDCc_fit_val
	pos_start = allparam.find(f3);
	pos_end   = allparam.find(f4,pos_start);
	aDCc_s = allparam.substr((pos_start+f3.length()),(pos_end-pos_start-f3.length()));
	istringstream aDCc_ss(aDCc_s);
	aDCc_ss.precision(::std::numeric_limits<double>::digits10);	
	do
	{
		aDCc_ss >> sub_aDCc;
		aDCc_s1.push_back(sub_aDCc);
		if( aDCc_s1[aDCc_s1.size()-1].compare(0,1,sd) == 0 )
		{
			sysparam->aDCc_fit_ind.push_back(aDCc_s1.size()-1);
			sysparam->aDCc_fit_val.push_back(atof(aDCc_s1[aDCc_s1.size()-1].erase(0,1).c_str()));
			sub_aDCc = aDCc_s1[aDCc_s1.size()-1];			
		};
			sysparam->aDCc.push_back(atof(sub_aDCc.c_str()));
	}while(aDCc_ss);
	sysparam->aDCc.pop_back();
	//cout<<"sysparam->aDCc_fit_ind[0] = "<<sysparam->aDCc_fit_ind[0]<<" "<<sysparam->aDCc_fit_ind[1]<<endl; //" "<<sysparam->aDCc[10]<<" "<<sysparam->aDCc[11]<<endl;

	// Activity Model name
	pos_start = allparam.find(f11);
	pos_end   = allparam.find(f4,pos_start);
	ActName_s = allparam.substr((pos_start+f11.length()),(pos_end-pos_start-f11.length()));
	istringstream ActName_ss(ActName_s);
        ActName_ss >> sub_ActName;
/*	if( ! strncmp( sub_ActName.c_str(), "ELVIS", 20 ) )
	{
		activity_model = ELVIS;
	}
    else*/ if( ! strncmp( sub_ActName.c_str(), "PITZER", 20 ) )
	{
		activity_model = Pitzer;
	}
	else if( ! strncmp( sub_ActName.c_str(), "EUNIQUAC", 20 ) )
	{
		activity_model = EUNIQUAC;
	}	
	else if( ! strncmp( sub_ActName.c_str(), "SIT", 20 ) )
	{
		activity_model = SIT;
	}
	else
	{
		cout << "Activity model "<< sub_ActName <<" entered is not implemented !! "<<endl;
		cout << " Sorry, bailing out now ... ! "<<endl;
		exit(1);
	}
	
	fout<<"activity_model = "<< activity_model <<endl;	


	// Assert that the parameter given in the GEMSFIT chemical system input file are compatible with the corresponding values in the GEMS3K input file 
	long NPar, NPcoef, MaxOrd, NComp, NP_DC, index_phase = -1;
	char input_system_file_list_name[256];


	// get system file list name
	strcpy(input_system_file_list_name, system_name.c_str());

	// call GEM_init to read GEMS3K input files
	TNode* node  = new TNode();
	// call GEM_init     --> read in input files
	if( (node->GEM_init( input_system_file_list_name )) == 1 )
	{
		cout<<" .. ERROR occurred while reading input files !!! ..."<<endl;
		exit(1);
	}

	// Get index of phase of interest
	try
	{	
		index_phase = node->Ph_name_to_xCH( phase_name.c_str() );
		if( index_phase < 0 )
			throw index_phase;
	}
	catch( long e )
	{
		cout<<endl;
		cout<<" Phase name in GEMSFIT chemical system file has no corresponding phase name in GEMS3K input file !!!! "<<endl;
		cout<<" Phase name given in GEMSFIT_input.dat: "<<phase_name<<endl; 
		cout<<" Can not proceed ... Bailing out now ... "<<endl;
		cout<<endl;
		exit(1);
	} 
		

	// Get parameter array dimension as specified in GEMS3K input file
	node->Get_NPar_NPcoef_MaxOrd_NComp_NP_DC ( NPar, NPcoef, MaxOrd, NComp, NP_DC, index_phase );
	if( (sysparam->cols_aDCc * charges.size()) != (NComp*NP_DC) )
	{
		cout<<endl;     	   
		cout<<" node class: vector aDCc does not have the dimensions specified by the GEMS3K input file (NComp*NP_DC) !!!! "<<endl;
		cout<<" 		GEMSFIT:	sysparam->cols_aDCc = "<<sysparam->cols_aDCc<<", charges.size() = "<<charges.size()<<endl;
		cout<<" 		GEMS3K:		NP_DC = "<<NP_DC<<", NComp = "<<NComp<<endl;
		cout<<" bailing out now ... "<<endl;
		cout<<endl;     	   
		exit(1);
	}

	if( (sysparam->rows_aIPc * sysparam->cols_aIPc) != (NPar*NPcoef) )
	{
		cout<<endl;     	   
		cout<<" node class: vector aIPc does not have the dimensions specified by the GEMS3K input file (NPar*NPcoef) !!!! "<<endl;
		cout<<" 		GEMSFIT:	sysparam->rows_aIPc = "<<sysparam->rows_aIPc<<", sysparam->cols_aIPc = "<<sysparam->cols_aIPc<<endl;
		cout<<" 		GEMS3K:		NPar = "<<NPar<<", NPcoef = "<<NPcoef<<endl;
		cout<<" bailing out now ... "<<endl;
		cout<<endl;     	   
		exit(1);
	}

	if( (sysparam->rows_aIPc * sysparam->cols_aIPx) != (NPar*MaxOrd) )
	{
		cout<<endl;     	   
		cout<<" node class: vector aIPx does not have the dimensions specified by the GEMS3K input file (NPar*MaxOrd) !!!! "<<endl;
		cout<<" 		GEMSFIT:	sysparam->rows_aIPc = "<<sysparam->rows_aIPc<<", sysparam->cols_aIPx = "<<sysparam->cols_aIPx<<endl;
		cout<<" 		GEMS3K:		NPar = "<<NPar<<", MaxOrd = "<<MaxOrd<<endl;
		cout<<" bailing out now ... "<<endl;
		cout<<endl;     	   
		exit(1);
	}


	// Check if temperature and pressure is in the DATACH lookup array
	for( i=0; i<sysdata->pressure.size(); i++ )	
	{	
        if( (node->check_TP( sysdata->temperature.at(i), 100000 * sysdata->pressure.at(i)) == false &&  node->check_grid_TP( sysdata->temperature.at(i), 100000 * sysdata->pressure.at(i) ) < 0. ) /*&& node->check_grid_TP( sysdata->temperature.at(i), 100000 * sysdata->pressure.at(i) ) < 0*/ )
		{
			cout<<endl;
			cout<<" Measurement data: temperature "<<sysdata->temperature.at(i)<<" together with pressure "<<100000 * sysdata->pressure.at(i)<<" does not appear in the DATACH structure of GEMS3K !!!!"<<endl;
			//cout<<" bailing out now ... "<<endl;
			cout<<endl;     	   
			//exit(1);
		}	 		 				
	}

	delete node;

	param_stream.close();

	fout.close();

}
