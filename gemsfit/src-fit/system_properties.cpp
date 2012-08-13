#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <sstream>


#include "system_properties.h"

using namespace std;

System_Properties::System_Properties( System_Specifications system_ ) // constructor
{
	if( system_.meas_db_.empty() )
	{
		cout<<" Class System_Properties not correctly initialized !! "<<endl;
		exit(1); 
	};
	meas_db	    = system_.meas_db_;
//	param_file  = system_.param_file_;
	
	// For parameter optimization do not use printing of results
	printfile = false;

	// For parameter optimization do not use parallelization of the Monte Carlo loop (=true). Instead, execute loop over measurements within objective function in parallel (=false). 
	MC_MPI = false;


	data_meas = new Data_Manager;  // allocate memory for data_meas of class Data Manager

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

cout<<"line 48"<<endl;

	// Variable declarations
	vector<string> data, aIPc_s1, aDCc_s1;
	string line, allparam;
    	string aIPc_s, aDCc_s, aIPx_s, sub_aIPc, sub_aDCc, sub_aIPx, ActName_s;
	string LsMod_s, LsMdc_s, SysName_s, PhaName_s, SpecName_s, SpecCharge_s; 
	string sub_LsMod, sub_LsMdc, sub_SysName, sub_SpecCharge, sub_PhaName, sub_SpecName, sub_ActName;
	int pos_start, pos_end;
	unsigned int i;
 	ifstream param_stream;


	// Keywords to locate in the input file - position
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
	//param_stream.open("NaCl-param.dat");
    cout<<"param_file.c_str() = "<<param_file.c_str()<<endl;
	//param_stream.open(param_file.c_str());	
	param_stream.open("GEMSFIT_input.dat");	
	if( param_stream.fail() )
	{
		cout << "Opening of file "<<param_file<<" failed !!"<<endl;
		exit(1);
	}
	while( getline(param_stream, line) ) 
	{
		data.push_back(line); // puts each line into data (vector<string>)??
	} 
	param_stream.close();
	for( i=0; i < data.size(); i++ )
	allparam += data[i]; // adds each line to the string allparam


	// System name
	pos_start = allparam.find(f7);
	pos_end   = allparam.find(f4,pos_start);
	SysName_s = allparam.substr((pos_start+f7.length()),(pos_end-pos_start-f7.length()));
	istringstream SysName_ss(SysName_s);
	for( i=0; i<1; i++)
	{
        SysName_ss >> sub_SysName;
		system_name = sub_SysName;
	};	
	cout<<"system_name = "<<system_name<<endl;

	// Phase name
	pos_start = allparam.find(f8);
	pos_end   = allparam.find(f4,pos_start); // search from position start on 
	PhaName_s = allparam.substr((pos_start+f8.length()),(pos_end-pos_start-f8.length())); // makes a substring from first arg with a length of the second arg
	istringstream PhaName_ss(PhaName_s); // Input string stream class
	for( i=0; i<1; i++)
	{
        PhaName_ss >> sub_PhaName;
		phase_name = sub_PhaName; // Names stored in phase_name
	};	
	cout<<"phase_name = "<<phase_name<<endl;


	//species
	pos_start = allparam.find(f9);
	pos_end   = allparam.find(f4,pos_start);
	SpecName_s = allparam.substr((pos_start+f9.length()),(pos_end-pos_start-f9.length()));
	istringstream SpecName_ss(SpecName_s);
	do
	{
        SpecName_ss >> sub_SpecName;
		species.push_back(sub_SpecName); // species is a vector
	}while(SpecName_ss);	
	cout<<"species[0] = "<<species[0]<<" | species[1] = "<<species[1]<<" | species[2] = "<<species[2]<<endl;


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
	cout<<"charges[0] = "<<charges[0]<<" | charges[1] = "<<charges[1]<<" | charges[2] = "<<charges[2]<<endl;


	// dimensionalities of the parameter arrays // They are stored in a calss/struct parameters
	pos_start = allparam.find(f5);
	pos_end   = allparam.find(f4,pos_start);
	LsMod_s = allparam.substr((pos_start+f5.length()),(pos_end-pos_start-f5.length()));
	istringstream LsMod_ss(LsMod_s);
	for( i=0; i<3; i++)
	{
        LsMod_ss >> sub_LsMod;
		if( i==0 ){	sysparam->rows_aIPc = atoi(sub_LsMod.c_str()); }; //sysparam a pointer of class parameters  that contain rows_aIPc ...etc
		if( i==1 ){	sysparam->cols_aIPx = atoi(sub_LsMod.c_str()); };
		if( i==2 ){	sysparam->cols_aIPc = atoi(sub_LsMod.c_str()); };		
	};	
	cout<<"sysparam->rows_aIPc = "<<sysparam->rows_aIPc<<endl;
	
	pos_start = allparam.find(f6);
	pos_end   = allparam.find(f4,pos_start);
	LsMdc_s = allparam.substr((pos_start+f6.length()),(pos_end-pos_start-f6.length()));
	istringstream LsMdc_ss(LsMdc_s);
	for( i=0; i<1; i++)
	{
        LsMdc_ss >> sub_LsMdc;
		sysparam->cols_aDCc = atoi(sub_LsMdc.c_str());
	};	
	cout<<"sysparam->cols_aDCc = "<<sysparam->cols_aDCc<<endl;

	// Get aIPx vector
	pos_start = allparam.find(f1);
	pos_end   = allparam.find(f4,pos_start);
	aIPx_s = allparam.substr((pos_start+f1.length()),(pos_end-pos_start-f1.length()));
	istringstream aIPx_ss(aIPx_s);	
 	do
	{
		aIPx_ss >> sub_aIPx;
		//cout<<"sub_aIPx = "<<sub_aIPx<<endl;
		sysparam->aIPx.push_back(atoi(sub_aIPx.c_str())); // vector of type class parameters
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
	if( ! strncmp( sub_ActName.c_str(), "ELVIS", 20 ) ) // string compare 20 max number of characters to compare, if found returns 0
	{
		activity_model = ELVIS;
	}
	else if( ! strncmp( sub_ActName.c_str(), "PITZER", 20 ) )
	{
		activity_model = Pitzer;
	}
	else if( ! strncmp( sub_ActName.c_str(), "EUNIQUAC", 20 ) )
	{
		activity_model = EUNIQUAC;
	}
	else
	{
		cout << "Activity model "<< sub_ActName <<" entered is not implemented !! "<<endl;
		cout << " Sorry, bailing out now ... ! "<<endl;
		exit(1);
	}
	
	cout<<"activity_model = "<< activity_model <<endl;	


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
	catch( long e ) // somehow in catch enters a integer then it means the indexes between GEMSFIT and GEMS3K don't match??
	{
		cout<<endl;
		cout<<" Phase name in GEMSFIT chemical system file has no corresponding phase name in GEMS3K input file !!!! "<<endl;
		cout<<" Phase name given in GEMSFIT_input.dat: "<<phase_name<<endl; 
		cout<<" Can not proceed ... Bailing out now ... "<<endl;
		cout<<endl;
		exit(1);
	} 
		

	// Get parameter array dimension as specified in GEMS3K input file
    node->Get_NPar_NPcoef_MaxOrd_NComp_NP_DC ( &NPar, &NPcoef, &MaxOrd, &NComp, &NP_DC, &index_phase );
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


	// Check if temperature and pressure is in the DATACH lookup array. Input vs GEMS3K
	for( i=0; i<sysdata->pressure.size(); i++ )	// sysdata is a pointer of class measdata
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

cout<<"line 303"<<endl;

}
