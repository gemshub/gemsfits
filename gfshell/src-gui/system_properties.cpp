#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <sstream>


#include "system_properties.h"

using namespace std;

System_Properties::System_Properties( )
{
    // Initialize pointer to instance of sysparam object
    sysparam = new parameters;

    // pointer to optimization and statistics struct
    optstat = new opti_stat;

    // pointer to printing information struct
    printinfo = new print_info;

}


System_Properties::~System_Properties()
{
    delete sysparam;
    delete optstat;
    delete printinfo;
}


void System_Properties::uploadparam( string phase_name_, string param_file_ )
{
    phase_name = phase_name_;
    param_file = param_file_;

    // Load data from GEMS3K input files into measdata struct
    getparam( sysparam );
}


void System_Properties::getparam( parameters* sysparam )
{	

	// Variable declarations
        vector<string> data;
        string line, allparam, nDC_s, sub_nDC;
        string aIPc_s, aDCc_s, aIPx_s, sub_aIPc, sub_aDCc, sub_aIPx;
        string LsMod_s, LsMdc_s, SpecName_s, ActName_s;
        string sub_LsMod, sub_LsMdc, sub_SpecName, sub_ActName;
        int pos_start, pos_end, nDC;
	unsigned int i;
 	ifstream param_stream;


	// Keywords
	string f9("<DCNL>");	
	string f1("<IPxPH>");
	string f2("<PMc>");
	string f3("<DMc>");
	string f5("<LsMod>");
        string f6("<LsMdc>");
        string f7("<PHNL>");
        string f8("<sMod>");
        string f10("<nDCinPH>");
        string hash("#");


        string dch_file("-dch.dat");
        string ipm_file("-ipm.dat");

        erase_all(param_file,"-lst.dat");
        dch_file = param_file + dch_file;
        ipm_file = param_file + ipm_file;

cout<<"param_file = "<<param_file<<endl;
cout<<"dch_file = "<<dch_file<<endl;
cout<<"ipm_file = "<<ipm_file<<endl;


// ---------------- // INPUT FROM *-dch.dat // ------------------- //
        param_stream.open(dch_file.c_str());
	if( param_stream.fail() )
	{
		cout << "Opening of file "<<param_file<<" failed !!"<<endl;
		exit(1);
	}
	while( getline(param_stream, line) ) 
	{
            if( line.find("#") )
		data.push_back(line);
            else
                data.push_back(" # ");
	} 
	param_stream.close();
	for( i=0; i < data.size(); i++ )
	allparam += data[i];


	//species
        pos_start = allparam.find(f10);
        pos_end   = allparam.find(hash,pos_start);
        nDC_s = allparam.substr((pos_start+f10.length()),(pos_end-pos_start-f10.length()));
        istringstream nDC_ss(nDC_s);
        nDC_ss >> sub_nDC;
        nDC = atoi( sub_nDC.c_str() );



	pos_start = allparam.find(f9);
        pos_end   = allparam.find(hash,pos_start);
	SpecName_s = allparam.substr((pos_start+f9.length()),(pos_end-pos_start-f9.length()));
	istringstream SpecName_ss(SpecName_s);
        for( i=0; i<nDC; i++ )
	{
            SpecName_ss >> sub_SpecName;
		species.push_back(sub_SpecName);
        }

//        for( i=0; i<species.size(); i++ )
//            cout<<"species["<<i<<"] = "<<species[i]<<endl;


	param_stream.close();
// ---------------- // END OF INPUT FROM *.dch.dat // ------------------- //




// ---------------- // INPUT FROM *-ipm.dat // ------------------- //

        param_stream.open(ipm_file.c_str());
	if( param_stream.fail() )
	{
		cout << "Opening of file "<<param_file<<" failed !!"<<endl;
		exit(1);
	}
	while( getline(param_stream, line) ) 
	{
            if( line.find("#") )
                data.push_back(line);
            else
                data.push_back(" # ");
        }
	param_stream.close();
	for( i=0; i < data.size(); i++ )
            allparam += data[i];


//	//charges
//	pos_start = allparam.find(f10);
//	pos_end   = allparam.find(f4,pos_start);
//	SpecCharge_s = allparam.substr((pos_start+f10.length()),(pos_end-pos_start-f10.length()));
//	istringstream SpecCharge_ss(SpecCharge_s);
//	do
//	{
//       SpecCharge_ss >> sub_SpecCharge;
//		charges.push_back(atof(sub_SpecCharge.c_str()));
//	}while(SpecCharge_ss);
//	charges.pop_back();
//	cout<<"charges[0] = "<<charges[0]<<" | charges[1] = "<<charges[1]<<" | charges[2] = "<<charges[2]<<endl;


	// dimensionalities of the parameter arrays
	pos_start = allparam.find(f5);
        pos_end   = allparam.find(hash,pos_start);
	LsMod_s = allparam.substr((pos_start+f5.length()),(pos_end-pos_start-f5.length()));
	istringstream LsMod_ss(LsMod_s);
	for( i=0; i<3; i++)
	{
        LsMod_ss >> sub_LsMod;
		if( i==0 ){	sysparam->rows_aIPc = atoi(sub_LsMod.c_str()); };
		if( i==1 ){	sysparam->cols_aIPx = atoi(sub_LsMod.c_str()); };
		if( i==2 ){	sysparam->cols_aIPc = atoi(sub_LsMod.c_str()); };		
	};	
cout<<"sysparam->rows_aIPc = "<<sysparam->rows_aIPc<<endl;
cout<<"sysparam->cols_aIPc = "<<sysparam->cols_aIPc<<endl;
cout<<"sysparam->cols_aIPx = "<<sysparam->cols_aIPx<<endl;

	pos_start = allparam.find(f6);
        pos_end   = allparam.find(hash,pos_start);
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
        pos_end   = allparam.find(hash,pos_start);
	aIPx_s = allparam.substr((pos_start+f1.length()),(pos_end-pos_start-f1.length()));
	istringstream aIPx_ss(aIPx_s);	
 	do
	{
            aIPx_ss >> sub_aIPx;
//            sysparam->aIPx.push_back(atoi(sub_aIPx.c_str()));
            sysparam->aIPx.push_back( sub_aIPx.c_str() );
        } while(aIPx_ss);
	sysparam->aIPx.pop_back();

        for( int i=0; i< sysparam->aIPx.size(); i++ )
            cout<<"sysparam->aIPx["<<i<<"] = "<<sysparam->aIPx[i]<<endl;
cout<<"sysparam->aIPx[0] = "<<sysparam->aIPx[0]<<", sysparam->aIPx[1] = "<<sysparam->aIPx[1]<<" "<<endl;


	// Get aIPc vector
	pos_start = allparam.find(f2);
        pos_end   = allparam.find(hash,pos_start);
	aIPc_s = allparam.substr((pos_start+f2.length()),(pos_end-pos_start-f2.length()));
	istringstream aIPc_ss(aIPc_s);
  	aIPc_ss.precision(::std::numeric_limits<double>::digits10);
	do
	{
            aIPc_ss >> sub_aIPc;
//            sysparam->aIPc.push_back(atof(sub_aIPc.c_str()));
            sysparam->aIPc.push_back( sub_aIPc.c_str() );
        }while(aIPc_ss);
	sysparam->aIPc.pop_back();
cout<<"sysparam->aIPc[0] = "<<sysparam->aIPc[0]<<", sysparam->aIPc[1] = "<<sysparam->aIPc[1]<<" "<<endl;

	// Get aDCc vector
	pos_start = allparam.find(f3);
        pos_end   = allparam.find(hash,pos_start);
	aDCc_s = allparam.substr((pos_start+f3.length()),(pos_end-pos_start-f3.length()));
	istringstream aDCc_ss(aDCc_s);
	aDCc_ss.precision(::std::numeric_limits<double>::digits10);	
	do
	{
            aDCc_ss >> sub_aDCc;
//            sysparam->aDCc.push_back(atof(sub_aDCc.c_str()));
            sysparam->aDCc.push_back( sub_aDCc.c_str() );
        }while(aDCc_ss);
        sysparam->aDCc.pop_back();
cout<<"sysparam->aDCc[0] = "<<sysparam->aDCc[0]<<", sysparam->aDCc[1] = "<<sysparam->aDCc[1]<<" "<<endl;


        // Activity Model name
        pos_start = allparam.find(f8);
        pos_end   = allparam.find(hash,pos_start);
        ActName_s = allparam.substr((pos_start+f8.length()),(pos_end-pos_start-f8.length()));
	istringstream ActName_ss(ActName_s);
        ActName_ss >> sub_ActName;

        boost::erase_all(sub_ActName, "'");

        cout<<"sub_ActName = "<<sub_ActName<<endl;



        if( ! strncmp( sub_ActName.c_str(), "J", 1) )
        {
            activity_model = ELVIS;
	}
        else if( ! strncmp( sub_ActName.c_str(), "Z", 1) )
	{
            activity_model = Pitzer;
	}
        else if( ! strncmp( sub_ActName.c_str(), "Q", 1) )
	{
            activity_model = EUNIQUAC;
	}
	else
	{
		cout << "Activity model "<< sub_ActName <<" entered is not implemented !! "<<endl;
		cout << " Sorry, bailing out now ... ! "<<endl;
		exit(1);
	}

//	cout<<"activity_model = "<< activity_model <<endl;

	param_stream.close();

cout<<"line 238"<<endl;

}

void System_Properties::uploadparam_GEMSFIT()
{

    // Variable declarations
    vector<string> data;
    string line, allparam, nDC_s, sub_nDC;
    string aIPc_s, aDCc_s, aIPx_s, sub_aIPc, sub_aDCc, sub_aIPx;
    string LsMod_s, LsMdc_s, SpecName_s, ActName_s;
    string sub_LsMod, sub_LsMdc, sub_SpecName, sub_ActName;
    int pos_start, pos_end, nDC;
    unsigned int i;
    ifstream param_stream;


    // Keywords
    string f9("<DCNL>");
    string f1("<IPxPH>");
    string f2("<PMc>");
    string f3("<DMc>");
    string f5("<LsMod>");
    string f6("<LsMdc>");
    string f7("<PHNL>");
    string f8("<sMod>");
    string f10("<nDCinPH>");
    string hash("#");


// ---------------- // INPUT FROM GEMSFIT_input.dat // ------------------- //
    param_stream.open("GEMSFIT_input.dat");
    if( param_stream.fail() )
    {
            cout << "Opening of file "<<param_file<<" failed !!"<<endl;
            exit(1);
    }
    while( getline(param_stream, line) )
    {
        if( line.find("#") )
            data.push_back(line);
        else
            data.push_back(" # ");
    }
    param_stream.close();
    for( i=0; i < data.size(); i++ )
    allparam += data[i];


    //species
    pos_start = allparam.find(f10);
    pos_end   = allparam.find(hash,pos_start);
    nDC_s = allparam.substr((pos_start+f10.length()),(pos_end-pos_start-f10.length()));
    istringstream nDC_ss(nDC_s);
    nDC_ss >> sub_nDC;
    nDC = atoi( sub_nDC.c_str() );



    pos_start = allparam.find(f9);
    pos_end   = allparam.find(hash,pos_start);
    SpecName_s = allparam.substr((pos_start+f9.length()),(pos_end-pos_start-f9.length()));
    istringstream SpecName_ss(SpecName_s);
    for( i=0; i<nDC; i++ )
    {
        SpecName_ss >> sub_SpecName;
            species.push_back(sub_SpecName);
    }


//	//charges
//	pos_start = allparam.find(f10);
//	pos_end   = allparam.find(f4,pos_start);
//	SpecCharge_s = allparam.substr((pos_start+f10.length()),(pos_end-pos_start-f10.length()));
//	istringstream SpecCharge_ss(SpecCharge_s);
//	do
//	{
//       SpecCharge_ss >> sub_SpecCharge;
//		charges.push_back(atof(sub_SpecCharge.c_str()));
//	}while(SpecCharge_ss);
//	charges.pop_back();
//	cout<<"charges[0] = "<<charges[0]<<" | charges[1] = "<<charges[1]<<" | charges[2] = "<<charges[2]<<endl;


    // dimensionalities of the parameter arrays
    pos_start = allparam.find(f5);
    pos_end   = allparam.find(hash,pos_start);
    LsMod_s = allparam.substr((pos_start+f5.length()),(pos_end-pos_start-f5.length()));
    istringstream LsMod_ss(LsMod_s);
    for( i=0; i<3; i++)
    {
    LsMod_ss >> sub_LsMod;
            if( i==0 ){	sysparam->rows_aIPc = atoi(sub_LsMod.c_str()); };
            if( i==1 ){	sysparam->cols_aIPx = atoi(sub_LsMod.c_str()); };
            if( i==2 ){	sysparam->cols_aIPc = atoi(sub_LsMod.c_str()); };
    };
cout<<"sysparam->rows_aIPc = "<<sysparam->rows_aIPc<<endl;
cout<<"sysparam->cols_aIPc = "<<sysparam->cols_aIPc<<endl;
cout<<"sysparam->cols_aIPx = "<<sysparam->cols_aIPx<<endl;

    pos_start = allparam.find(f6);
    pos_end   = allparam.find(hash,pos_start);
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
    pos_end   = allparam.find(hash,pos_start);
    aIPx_s = allparam.substr((pos_start+f1.length()),(pos_end-pos_start-f1.length()));
    istringstream aIPx_ss(aIPx_s);
    do
    {
        aIPx_ss >> sub_aIPx;
//            sysparam->aIPx.push_back(atoi(sub_aIPx.c_str()));
        sysparam->aIPx.push_back( sub_aIPx.c_str() );
    } while(aIPx_ss);
    sysparam->aIPx.pop_back();

    for( int i=0; i< sysparam->aIPx.size(); i++ )
        cout<<"sysparam->aIPx["<<i<<"] = "<<sysparam->aIPx[i]<<endl;
cout<<"sysparam->aIPx[0] = "<<sysparam->aIPx[0]<<", sysparam->aIPx[1] = "<<sysparam->aIPx[1]<<" "<<endl;


    // Get aIPc vector
    pos_start = allparam.find(f2);
    pos_end   = allparam.find(hash,pos_start);
    aIPc_s = allparam.substr((pos_start+f2.length()),(pos_end-pos_start-f2.length()));
    istringstream aIPc_ss(aIPc_s);
    aIPc_ss.precision(::std::numeric_limits<double>::digits10);
    do
    {
        aIPc_ss >> sub_aIPc;
//            sysparam->aIPc.push_back(atof(sub_aIPc.c_str()));
        sysparam->aIPc.push_back( sub_aIPc.c_str() );
    }while(aIPc_ss);
    sysparam->aIPc.pop_back();
cout<<"sysparam->aIPc[0] = "<<sysparam->aIPc[0]<<", sysparam->aIPc[1] = "<<sysparam->aIPc[1]<<" "<<endl;

    // Get aDCc vector
    pos_start = allparam.find(f3);
    pos_end   = allparam.find(hash,pos_start);
    aDCc_s = allparam.substr((pos_start+f3.length()),(pos_end-pos_start-f3.length()));
    istringstream aDCc_ss(aDCc_s);
    aDCc_ss.precision(::std::numeric_limits<double>::digits10);
    do
    {
        aDCc_ss >> sub_aDCc;
//            sysparam->aDCc.push_back(atof(sub_aDCc.c_str()));
        sysparam->aDCc.push_back( sub_aDCc.c_str() );
    }while(aDCc_ss);
    sysparam->aDCc.pop_back();
cout<<"sysparam->aDCc[0] = "<<sysparam->aDCc[0]<<", sysparam->aDCc[1] = "<<sysparam->aDCc[1]<<" "<<endl;


    // Activity Model name
    pos_start = allparam.find(f8);
    pos_end   = allparam.find(hash,pos_start);
    ActName_s = allparam.substr((pos_start+f8.length()),(pos_end-pos_start-f8.length()));
    istringstream ActName_ss(ActName_s);
    ActName_ss >> sub_ActName;

    boost::erase_all(sub_ActName, "'");

    cout<<"sub_ActName = "<<sub_ActName<<endl;



    if( ! strncmp( sub_ActName.c_str(), "J", 1) )
    {
        activity_model = ELVIS;
    }
    else if( ! strncmp( sub_ActName.c_str(), "Z", 1) )
    {
        activity_model = Pitzer;
    }
    else if( ! strncmp( sub_ActName.c_str(), "Q", 1) )
    {
        activity_model = EUNIQUAC;
    }
    else
    {
            cout << "Activity model "<< sub_ActName <<" entered is not implemented !! "<<endl;
            cout << " Sorry, bailing out now ... ! "<<endl;
            exit(1);
    }

}
