#include "data_manager.h"

// Constructor
Data_Manager::Data_Manager( )
{ 

	// Read parameters for database connection
	get_db_specs();
	
	// Read measurement data from PosgreSQL server
	sysdata = new measdata;

	if( !datasource )
	{
		get_CSV( sysdata );
	}
	else
	{
		get_DB( sysdata );
	}

	// read species from database and form vectors of independent component names
	get_ic( sysdata );

}


// Destructor		
Data_Manager::~Data_Manager( )
{
	// Delete measurement data struct
	delete sysdata;
}


void Data_Manager::get_db_specs( )
{
	int pos_start, pos_end, i;
	string line, alldata;
 	ifstream data_stream;
	vector<string> data;

	string f1("<DatDB>");
	string f2("<DatTable>");
	string f3("<DatUsername>");
	string f4("<DatPasswd>");
	string f5("<DatSource>");
	string f6("<DatCSVfile>");
	string f7("<DatServer>");
	string hash("#");

	// Read parameter file into string 
	data_stream.open( "GEMSFIT_input.dat" );	
	if( data_stream.fail() )
	{
		cout << "Opening of file GEMSFIT_input.dat failed !!"<<endl;
		exit(1);
	}
	while( getline(data_stream, line) ) 
	{
		data.push_back(line);
	} 
	data_stream.close();
	for( i=0; i < (int) data.size(); i++ )
	alldata += data[i];


	// get measurement data from CSV file (0) or PostgreSQL database (1)
	// Database name
	string sub_datasource;
	pos_start = alldata.find(f5);
	pos_end   = alldata.find(hash,pos_start);
	string datasource_s = alldata.substr((pos_start+f5.length()),(pos_end-pos_start-f5.length()));
	istringstream datasource_ss(datasource_s);
        datasource_ss >> sub_datasource;
		datasource = atoi( sub_datasource.c_str() );
	cout<<"datasource = "<<datasource<<endl;

	if( datasource == 0 )
	{
		// name of CSV file containing measurement data
		// Database name
		string sub_csv;
		pos_start = alldata.find(f6);
		pos_end   = alldata.find(hash,pos_start);
		string csv_s = alldata.substr((pos_start+f6.length()),(pos_end-pos_start-f6.length()));
		istringstream csv_ss(csv_s);
		csv_ss >> sub_csv;
			CSVfile = sub_csv;
		cout<<"CSVfile = "<<CSVfile<<endl;
	}
	else if( datasource == 1 )
	{
		// Database name
		string sub_dbname;
		pos_start = alldata.find(f1);
		pos_end   = alldata.find(hash,pos_start);
		string dbname_s = alldata.substr((pos_start+f1.length()),(pos_end-pos_start-f1.length()));
		istringstream dbname_ss(dbname_s);
		dbname_ss >> sub_dbname;
			DBname = sub_dbname;
		cout<<"DBname = "<<DBname<<endl;


		// Table name
		string sub_table;
		pos_start = alldata.find(f2);
		pos_end   = alldata.find(hash,pos_start);
		string table_s = alldata.substr((pos_start+f2.length()),(pos_end-pos_start-f2.length()));
		istringstream table_ss(table_s);
		table_ss >> sub_table;
			tablename = sub_table;
		cout<<"tablename = "<<tablename<<endl;


		// User name
		string sub_username;
		pos_start = alldata.find(f3);
		pos_end   = alldata.find(hash,pos_start);
		string username_s = alldata.substr((pos_start+f3.length()),(pos_end-pos_start-f3.length()));
		istringstream username_ss(username_s);
		username_ss >> sub_username;
			username = sub_username;
		cout<<"username = "<<username<<endl;


		// Password
		string sub_passwd;
		pos_start = alldata.find(f4);
		pos_end   = alldata.find(hash,pos_start);
		string passwd_s = alldata.substr((pos_start+f4.length()),(pos_end-pos_start-f4.length()));
		istringstream passwd_ss(passwd_s);
		passwd_ss >> sub_passwd;
			passwd = sub_passwd;
		cout<<"passwd = "<<passwd<<endl;
	
		// Server
		string sub_server;
		pos_start = alldata.find(f7);
		pos_end   = alldata.find(hash,pos_start);
		string server_s = alldata.substr((pos_start+f7.length()),(pos_end-pos_start-f7.length()));
		istringstream server_ss(server_s);
		server_ss >> sub_server;
			psql_server = sub_server;
		cout<<"psql_server = "<<psql_server<<endl;
	}
	else
	{
		cout << endl;
		cout << " You entered code for datasource = " << datasource << " is not implemented. Enter either (0) for CSV file or (1) for PostgreSQL database. " << endl;
		cout << " ... bailing out now ... " << endl;
		cout << endl;
	}

}


void Data_Manager::get_ic( measdata* sysdata )
{
	size_t pos1, pos2, pos3, pos4;
	int i;

	cout << " species_1_1[0] = " << sysdata->species_1_1[0] << endl;

	sysdata->ic_1.resize( (int) sysdata->species_1_1.size() ); 
	sysdata->ic_2.resize( (int) sysdata->species_1_1.size() ); 
	sysdata->ic_3.resize( (int) sysdata->species_1_1.size() ); 
	sysdata->ic_4.resize( (int) sysdata->species_1_1.size() ); 

	for( i=0; i< (int) sysdata->species_1_1.size(); i++ )
	{
	  	pos1 = 0;	pos2 = 0;	pos3 = 0;	pos4 = 0;
		pos1 = sysdata->species_1_1[i].find("+");    
		if( pos1 > 100 )
			pos1 = sysdata->species_1_1[i].find("-");

		pos2 = sysdata->species_1_2[i].find("+");    
		if( pos2 > 100 )
			pos2 = sysdata->species_1_2[i].find("-");

		pos3 = sysdata->species_2_1[i].find("+");    
		if( pos3 > 100 )
			pos3 = sysdata->species_2_1[i].find("-"); 
		
		pos4 = sysdata->species_2_2[i].find("+");    
		if( pos4 > 100 )
			pos4 = sysdata->species_2_2[i].find("-");   

		sysdata->ic_1[i] = sysdata->species_1_1[i].substr( 0, pos1 );   
		sysdata->ic_2[i] = sysdata->species_1_2[i].substr( 0, pos2 );   
		sysdata->ic_3[i] = sysdata->species_2_1[i].substr( 0, pos3 );   
		sysdata->ic_4[i] = sysdata->species_2_2[i].substr( 0, pos4 );   

	}

}







