/* 
*	 Copyright (C) 2012 by Ferdinand F. Hingerl (hingerl@hotmail.com)
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
 *	@file plot_class.cpp
 *
 *	@brief this source file contains the implementation of the PlotFit class, 
 *	which ia a wrapper for the dislin librarby and contains functions for data plotting.   
 *
 *	@author Ferdinand F. Hingerl
 *
 * 	@date 09.04.2012 
 *
 */
#include "plot_class.h"


using namespace std;


// Constructor
PlotFit::PlotFit()
{
	
	// allocate dynamic memory for my_plotting_info struct
	plotting_info = new my_plotting_info;
	
	// Populate member variables
	set_plotfit_vars();
	
}



// Destructor
PlotFit::~PlotFit()
{

	// free dynamic memory for my_plotting_info struct
	delete plotting_info;

}



void PlotFit::set_plotfit_vars()
{
		string OptParamFile("GEMSFIT_input.dat");
		int pos_start, pos_end;
		unsigned int i;
	 	ifstream param_stream;
		vector<string> data;
		string line, allparam;
		string s0("<PrintTemperatures>");
		string s1("<PrintPressures>");
		string s2("<PrintMolalities>");
		string s3("<PrintFormat>");
		string s4("<PrintFilename>");
		string s5("<PrintMeasValueCode>");
		string s6("<PrintLabelXaxis>");
		string s7("<PrintLabelYaxis>");
		string s8("<PrintHead>");

		string hash("#");

		// Open configuration file for GEMSFIT run
		param_stream.open(OptParamFile.c_str());	
		if( param_stream.fail() )
		{
			cout << "Opening of file "<<OptParamFile<<" failed !!"<<endl;
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


				// Printing: temperatures
		string PrintTemps_s;
		std::vector<std::string> tokens;
		pos_start = allparam.find(s0);
		pos_end   = allparam.find(s1,pos_start);
		PrintTemps_s = allparam.substr((pos_start+s0.length()),(pos_end-pos_start-s0.length()));
		boost::split( tokens, PrintTemps_s, boost::is_any_of(" ") );

		if( PrintTemps_s.empty() )
		{
			fout<<"Note: no temperatures for printing specified ..."<<endl;
		}
		
		BOOST_FOREACH( const std::string& i, tokens ) 
		{
		    plotting_info->print_temperatures.push_back(atof(i.c_str()));
		}
		for( i=0; i< (int) plotting_info->print_temperatures.size(); i++ )
			fout<<"plotting_info->print_temperatures["<<i<<"] = "<<plotting_info->print_temperatures[i]<<endl;


		// Printing: pressures
		string PrintPress_s, sub_PrintPress;
		pos_start = allparam.find(s1);
		pos_end   = allparam.find(s2,pos_start);
		PrintPress_s = allparam.substr((pos_start+s1.length()),(pos_end-pos_start-s1.length()));
		istringstream PrintPress_ss(PrintPress_s);
		do
		{
		    PrintPress_ss >> sub_PrintPress;
			if( sub_PrintPress.empty() )
			{ 
				fout<<"Note: no pressures for printing specified ..."<<endl; break; 
			}
			else
			{
				plotting_info->print_pressures.push_back(atof(sub_PrintPress.c_str()));
			}
		}while(PrintPress_ss);	
		plotting_info->print_pressures.pop_back();
		for( i=0; i< (int) plotting_info->print_pressures.size(); i++ )
			fout<<"plotting_info->print_pressures["<<i<<"] = "<<plotting_info->print_pressures[i]<<endl;


		// Printing: molalities
		string PrintMols_s, sub_PrintMols;
		pos_start = allparam.find(s2);
		pos_end   = allparam.find(s3,pos_start);
		PrintMols_s = allparam.substr((pos_start+s2.length()),(pos_end-pos_start-s2.length()));
		istringstream PrintMols_ss(PrintMols_s);
		do
		{
		    PrintMols_ss >> sub_PrintMols;
			if( sub_PrintMols.empty() )
			{ 
				fout<<"Note: no molalities for printing specified ..."<<endl; break; 
			}
			else if( sub_PrintMols == "-" ) 
			{
				// skip and do nothing
			}
			else 
			{
				plotting_info->print_molalities.push_back(atof(sub_PrintMols.c_str()));
			}
		}while(PrintMols_ss);	
		plotting_info->print_molalities.pop_back();
		for( i=0; i< (int) plotting_info->print_molalities.size(); i++ )
			fout<<"plotting_info->print_molalities["<<i<<"] = "<<plotting_info->print_molalities[i]<<endl;


		// Printing: output file format
		string PrintForm_s;
		pos_start = allparam.find(s3);
		pos_end   = allparam.find(s4,pos_start);
		PrintForm_s = allparam.substr((pos_start+s3.length()),(pos_end-pos_start-s3.length()));
		if( PrintForm_s.empty() )
		{
			fout<<"Note: no output format for printing specified ..."<<endl;
		}
		plotting_info->print_format = PrintForm_s;
		fout<<"print_format = "<<plotting_info->print_format<<endl;


		// Printing: output filename
		string PrintFname_s;
		pos_start = allparam.find(s4);
		pos_end   = allparam.find(s5,pos_start);
		PrintFname_s = allparam.substr((pos_start+s4.length()),(pos_end-pos_start-s4.length()));
		if( PrintFname_s.empty() )
		{
			fout<<"Note: no filename for printing specified ..."<<endl;
		}
		plotting_info->print_filename = PrintFname_s + "." + plotting_info->print_format;
		fout<<"plotting_info->print_filename = "<<plotting_info->print_filename<<endl;


		// Printing: code of measurement values that need to be printed (e.g. 0 -> only activity coefficient data will be printed)
		string PrintCode_s;
		pos_start = allparam.find(s5);
		pos_end   = allparam.find(s6,pos_start);
		PrintCode_s = allparam.substr((pos_start+s5.length()),(pos_end-pos_start-s5.length()));
		if( PrintCode_s.empty() )
		{
			fout<<"Note: no code given for measurement values which have to be printed ..."<<endl;
		}
		plotting_info->print_code = atoi(PrintCode_s.c_str());
		fout<<"plotting_info->print_code = "<<plotting_info->print_code<<endl;


		// Printing: label of x-axis
		string PrintXaxis_s;
		pos_start = allparam.find(s6);
		pos_end   = allparam.find(s7,pos_start);
		PrintXaxis_s = allparam.substr((pos_start+s6.length()),(pos_end-pos_start-s6.length()));
		if( PrintXaxis_s.empty() )
		{
			fout<<"Note: no label for x-axis for printing specified ..."<<endl;
		}
		plotting_info->print_xlabel = PrintXaxis_s;
		fout<<"plotting_info->print_xlabel = "<<plotting_info->print_xlabel<<endl;


		// Printing: label of y-axis
		string PrintYaxis_s;
		pos_start = allparam.find(s7);
		pos_end   = allparam.find(s8,pos_start);
		PrintYaxis_s = allparam.substr((pos_start+s7.length()),(pos_end-pos_start-s7.length()));
		if( PrintYaxis_s.empty() )
		{
			fout<<"Note: no label for y-axis for printing specified ..."<<endl;
		}
		plotting_info->print_ylabel = PrintYaxis_s;
		fout<<"plotting_info->print_ylabel = "<<plotting_info->print_ylabel<<endl;


		// Printing: headline of plot
		string PrintHead_s;
		pos_start = allparam.find(s8);
		pos_end   = allparam.find(hash,pos_start);
		PrintHead_s = allparam.substr((pos_start+s8.length()),(pos_end-pos_start-s8.length()));
		if( PrintHead_s.empty() )
		{
			fout<<"Note: no headline of plot for printing specified ..."<<endl;
		}
		plotting_info->print_head = PrintHead_s;
		fout<<"plotting_info->print_head = "<<plotting_info->print_head<<endl;


		param_stream.close();
		fout.close();
}



void PlotFit::print_qqplot( const std::vector<double> &residuals_v, const std::vector<double> &quantiles_v )
{

	plotting_info->print_filename = "output_GEMSFIT/QQ_plot.";
	plotting_info->print_filename = plotting_info->print_filename + plotting_info->print_format;
	plotting_info->print_xlabel   = "Normal quantiles";
	plotting_info->print_ylabel   = "Residual quantiles";
	plotting_info->print_head     = "Q-Q Plot of Residuals";				

	int i, ic;
	float XA, XE, XOR, XSTEP, YA, YE, YOR, YSTEP; 
	int len_new = 0;
	len_new = (int) quantiles_v.size(); 
	vector<double> xrayv;
	vector<double> y1rayv;
	float* xray = new float[ len_new ];
	float* y1ray = new float[ len_new ];
	float xray2[2];
	float yray2[2];


	// Transfer values form plot_array to xray and y1ray float vectors. In addition trnafer them into two double vector which will be used to find min and max values
	for( i=0; i<len_new; i++ )		
	{	
		xray[i] = quantiles_v[i];
		xrayv.push_back( quantiles_v[i] );		
		y1ray[i] = residuals_v[i];		
		y1rayv.push_back( residuals_v[i] );	
	}


	// Define graph parameters( needed by GRAF routine ) 
	XA 		= floor( *min_element( xrayv.begin(), xrayv.end() ) );
	XE 		= ceil( *max_element( xrayv.begin(), xrayv.end() ) );
	XOR 	= XA;
	XSTEP 	= 1.0; //(XE-XA)/10; 
	YA 		= floor( *min_element( y1rayv.begin(), y1rayv.end()) * 1.1 );
	YE 		= ceil( *max_element( y1rayv.begin(), y1rayv.end()) * 1.1 );
	if( YA < -YE )
		YA = YA;
	else
		YA = -YE;
	
	YOR 	= YA;
	YSTEP 	= 1.0; //(YE-YA)/10; 
#ifdef GEMSFIT_DEBUG
	cout<<" XA = "<<XA<<", XE = "<<XE<<", XOR = "<<XOR<<", XSTEP = "<<XSTEP<<", YA = "<<YA<<", YE = "<<YE<<", YOR = "<<YOR<<", YSTEP = "<<YSTEP<<endl; 
#endif


	xray2[0] = XA; xray2[1] = XE;
	yray2[0] = YA; yray2[1] = YE;


// ------------------------------------------------------------------------------------ //
// Plot difference between computed and measured values

// Level 0	
	dislin::setpag("da4l");
	dislin::metafl(plotting_info->print_format.c_str());
	dislin::setfil(plotting_info->print_filename.c_str());


	// Initialize DISLIN object -> set Level to 1
	dislin::disini();

// Level 1

	dislin::pagera();											// L 1 2 3
//	dislin::complx();

	// Axis labeling
	dislin::name( plotting_info->print_xlabel.c_str(), "x" );	// L 1 2 3
	dislin::name( plotting_info->print_ylabel.c_str(), "y" );	// L 1 2 3


	// Title attributes
	dislin::titlin( plotting_info->print_head.c_str(), 2 );		// L 1 2 3
	//dislin::titlin( "measured versus computed values", 3 );


	// PSFONT: can only be used for ps,eps, pdf and svg formats !!
	dislin::psfont("Helvetica-Bold"); 							// L 1 2 3
	//dislin::hwfont(); // -> take system default font


	// Axis attributes
	dislin::axspos( 450, 1700 );								// L 1
	dislin::axslen( 1600, 1600 );								// L 1
	// for DINA4 portrait ("da4p") take these values:		
	//dislin::axspos( 200, 2600 );
	//dislin::axslen( 1350, 1150 );


	dislin::labdig(-1,"xy");										// L 1 2 3
	// Ticks between numbers in axes
	dislin::ticks(5,"x");										// L 1 2 3
	dislin::ticks(5,"y");										// L 1 2 3


	ic=dislin::intrgb(0.95,0.95,0.95);
	dislin::axsbgd(ic);											// L 1 2 3	

	
	// Set line thickness of frame surrounding graph
	dislin::frame(3);	

	// Push all table parts to the center
	dislin::center();

	// GRAF sets level to 2
	dislin::graf( XA, XE, XOR, XSTEP, YA, YE, YOR, YSTEP );  	// L 1 

// Level 2

	
	// Scatter plot: add every datapoint and draw circle around it with radius 0.01
	dislin::color("red");										// L 1 2 3
	for ( i=0; i<len_new; i++ )
	{
		dislin::rlcirc ( xray[i], y1ray[i], 0.1 );				// L 1 2 3
	}

	
	dislin::thkcrv( 1.5 );
	dislin::color("fore");
	//dislin::curve( y1ray, y1ray, len_new);
	dislin::curve( xray2, yray2, 2);


	dislin::setrgb(0.7,0.7,0.7); 								// L 1 2 3
	dislin::grid(1,1);											// L 2 3



	dislin::color("fore");										// L 1 2 3
	dislin::height(50); 										// L 1 2 3
	dislin::title();											// L 2 3


	// Terminate DISLIN
	dislin::disfin();											// L 1 2 3
	
}




// Residuals plotting function using DISLIN library: Computed versus Measured values
void PlotFit::print_vectors_curve( const int &id, const std::vector<System_Properties*> *systems, const std::vector< std::vector<double> > &plot_array )
{


	plotting_info->print_filename = "output_GEMSFIT/" + plotting_info->print_filename;
	
	if( id == 0 ) // plot part of the fitting result : call done by function ActivityModel::print_results(...)
	{
		
	}
	else if( id == 1 ) // plot residuals
	{
		plotting_info->print_filename = "output_GEMSFIT/residuals_curve.";
		plotting_info->print_filename = plotting_info->print_filename + plotting_info->print_format;
		plotting_info->print_xlabel   = "molality";
		plotting_info->print_ylabel   = "residuals";
		plotting_info->print_head     = "System residuals";				
	}


	int i, ic;
	float XA, XE, XOR, XSTEP, YA, YE, YOR, YSTEP; 
	double a, b;

	int len = 0;
        len = (int) systems->at(0)->computed_values_v.size();
	int len_new = 0;
        len_new = (int) plot_array.size();

	float* xray = new float[ len_new ];
	float* y1ray = new float[ len_new ];
	float* y2ray = new float[ len_new ];
	float* e1ray = new float[ len_new ];
	float* e2ray = new float[ len_new ];

	for( i=0; i<len_new; i++ )		
	{	
//		cout<<"after sort: plot_array["<<i<<"][0] = "<<plot_array[i][0]<<", plot_array["<<i<<"][2] = "<<plot_array[i][2]<<endl;
		xray[i] = plot_array[i][0];			
		y1ray[i] = plot_array[i][1];	// computed values		
		y2ray[i] = plot_array[i][2];	// measured values
		e1ray[i] = 0.5*plot_array[i][3]*plot_array[i][2];		// error bars 	
		e2ray[i] = 0.5*plot_array[i][3]*plot_array[i][2];		// error bars		
//cout << " y1ray["<<i<<"] = "<< y1ray[i] << endl;
//cout << " y2ray["<<i<<"] = "<< y2ray[i] << endl;
//cout << " e1ray["<<i<<"] = "<< e1ray[i] << endl;
//cout << " e2ray["<<i<<"] = "<< e2ray[i] << endl;
	}

// ------------------------------------------------------------------------------------ //
	
	dislin::setpag("da4l");
	dislin::metafl(plotting_info->print_format.c_str());
	dislin::setfil(plotting_info->print_filename.c_str());

	dislin::disini();
	dislin::pagera();
	dislin::complx();

	//dislin::hwfont(); // -> take system default font
	dislin::psfont("Helvetica-Bold"); // can only be used for ps,eps, pdf and svg formats !!


	// Axis attributes
	dislin::axspos( 450, 1700 );
	dislin::axslen( 2200, 1200 );
	// for DINA4 portrait ("da4p") take these values:		
	//dislin::axspos( 200, 2600 );
	//dislin::axslen( 1350, 1150 );
	dislin::name( plotting_info->print_xlabel.c_str(), "x" );
	dislin::name( plotting_info->print_ylabel.c_str(), "y" );

	// Title attributes
	dislin::titlin( plotting_info->print_head.c_str(), 2 );
	//dislin::titlin( "measured versus computed values", 3 );

	ic=dislin::intrgb(0.95,0.95,0.95);
	dislin::axsbgd(ic);


	XA 		= *min_element(systems->at(0)->sysdata->molality_1.begin(), systems->at(0)->sysdata->molality_1.end());
	XE 		= *max_element(systems->at(0)->sysdata->molality_1.begin(), systems->at(0)->sysdata->molality_1.end()) + 1;
	XOR 	= 0.;
	XSTEP 	= (XE-XA)/10; 
	a = *min_element(systems->at(0)->computed_values_v.begin(), systems->at(0)->computed_values_v.end());
	b = *min_element(systems->at(0)->measured_values_v.begin(), systems->at(0)->measured_values_v.end());
	YA 		= ( a < b )? a : b;
	a = *max_element(systems->at(0)->computed_values_v.begin(), systems->at(0)->computed_values_v.end());
	b = *max_element(systems->at(0)->measured_values_v.begin(), systems->at(0)->measured_values_v.end());
	YE 		= ( a > b )? a : b;
	YOR 	= 0.;
	YSTEP 	= (YE-YA)/10; 
#ifdef GEMSFIT_DEBUG
	cout<<" XA = "<<XA<<", XE = "<<XE<<", XOR = "<<XOR<<", XSTEP = "<<XSTEP<<", YA = "<<YA<<", YE = "<<YE<<", YOR = "<<YOR<<", YSTEP = "<<YSTEP<<endl; 
#endif

// zuru suru ;-)	
	XA = 0.0;
	XE = 4.5;
	XOR = 0.0;
	XSTEP = 0.5; 
//	XSTEP = 0.5;
	YA = 0.2;
	YOR = 0.2;
	YE = 1.4;
	YSTEP = 0.1; 
//	YSTEP = 0.5;
// zuru suru ;-)

	dislin::labdig(1,"x");
	dislin::ticks(2,"x");
	dislin::ticks(2,"y");


	dislin::graf( XA, XE, XOR, XSTEP, YA, YE, YOR, YSTEP ); 
	dislin::setrgb(0.7,0.7,0.7);
	dislin::grid(1,1);

	// Computed values
	dislin::color("red");
	dislin::curve(xray, y1ray, len);
	
	// Measured values
	dislin::setrgb(0,0,0); // black color  
	// Plot measured data with error bars
	dislin::marker( 21 );		// symbol type for measurement data 
	dislin::hsymbl( 20 ); 		// symbol size
	dislin::errbar(xray, y2ray, e1ray, e2ray, len);

	if( id == 0 ) // plot part of the fitting result : call done by function ActivityModel::print_results(...)
	{
		// Legend attributes
		dislin::color("fore");
		char cbuf[100];
		dislin::legini( cbuf, 2, 10 );
		dislin::legtit( " " );
		//dislin::legpos( 1000, 1000 );	
		dislin::leglin( cbuf, "computed", 1 );
		dislin::leglin( cbuf, "measured", 2 );		
		dislin::patcyc( 1, 7);
		dislin::patcyc( 2, 4);
		dislin::legend( cbuf, 8 ); // 8 -> plot legend in upper left corner of axis system			
	}


	dislin::color("fore");
	dislin::height(50);
	dislin::title();


	dislin::disfin();


	// Free memory
	delete[] xray;
	delete[] y1ray;
	delete[] y2ray;
	delete[] e1ray;
	delete[] e2ray;
// ------------------------------------------------------------------------------------ //

}




// Generic scatter plot function using DISLIN library
void PlotFit::print_vectors_scatter( const int &id, const std::vector<System_Properties*> *systems, const std::vector< std::vector<double> > &plot_array )
{

	plotting_info->print_filename = "output_GEMSFIT/Scatter_" + plotting_info->print_filename;
	
	if( id == 0 ) // plot part of the fitting result : call done by function ActivityModel::print_results(...)
	{
		
	}
	else if( id == 1 ) // plot residuals
	{
		plotting_info->print_filename = "output_GEMSFIT/residuals_scatter.";
		plotting_info->print_filename = plotting_info->print_filename + plotting_info->print_format;
		plotting_info->print_xlabel   = "molality";
		plotting_info->print_ylabel   = "residuals";
		plotting_info->print_head     = "System residuals";				
	}

	int i, ic;
	float XA, XE, XOR, XSTEP, YA, YE, YOR, YSTEP; 
	int len_new = 0;
	len_new = (int) plot_array.size(); 
	vector<double> xrayv;
	vector<double> y1rayv;
	vector<double> y2rayv;
	float* xray = new float[ len_new ];
	float* y1ray = new float[ len_new ];
	float* y2ray = new float[ len_new ];


	// Transfer values form plot_array to xray and y1ray float vectors. In addition trnafer them into two double vector which will be used to find min and max values
	for( i=0; i<len_new; i++ )		
	{	
#ifdef GEMSFIT_DEBUG
		cout<<"scatter plot residuals: plot_array["<<i<<"][0] = "<<plot_array[i][0]<<", plot_array["<<i<<"][1] = "<<plot_array[i][1]<<endl;
#endif
		xray[i] = plot_array[i][0];
		xrayv.push_back( plot_array[i][0] );		
		y1ray[i] = plot_array[i][1];		
		y1rayv.push_back( plot_array[i][1] );	
		y2ray[i] = plot_array[i][2];		
		y2rayv.push_back( plot_array[i][2] );	
	}

// zuru suru ;-)
	YA = -0.02;
	YE = 0.02;	 
// zuru suru ;-)


	// Define graph parameters( needed by GRAF routine ) 
	XA 		= floor( *min_element( xrayv.begin(), xrayv.end() ) );
	XE 		= ceil( *max_element( xrayv.begin(), xrayv.end() ) );
	XOR 	= 0.;
	XSTEP 	= (XE-XA)/10; 
//	YA 		= floor( *min_element( y1rayv.begin(), y1rayv.end()) * 1.1 );
//	YE 		= ceil( *max_element( y1rayv.begin(), y1rayv.end()) * 1.1 );
	YOR 	= YA;
	YSTEP 	= (YE-YA)/10; 
#ifdef GEMSFIT_DEBUG
	cout<<" XA = "<<XA<<", XE = "<<XE<<", XOR = "<<XOR<<", XSTEP = "<<XSTEP<<", YA = "<<YA<<", YE = "<<YE<<", YOR = "<<YOR<<", YSTEP = "<<YSTEP<<endl; 
#endif


// ------------------------------------------------------------------------------------ //
// Plot difference between computed and measured values

// Level 0	
	dislin::setpag("da4l");
	dislin::metafl(plotting_info->print_format.c_str());
	dislin::setfil(plotting_info->print_filename.c_str());


	// Initialize DISLIN object -> set Level to 1
	dislin::disini();

// Level 1

	dislin::pagera();											// L 1 2 3
//	dislin::complx();

	// Axis labeling
	dislin::name( plotting_info->print_xlabel.c_str(), "x" );	// L 1 2 3
	dislin::name( plotting_info->print_ylabel.c_str(), "y" );	// L 1 2 3


	// Title attributes
	dislin::titlin( plotting_info->print_head.c_str(), 2 );		// L 1 2 3
	//dislin::titlin( "measured versus computed values", 3 );


	// PSFONT: can only be used for ps,eps, pdf and svg formats !!
	dislin::psfont("Helvetica-Bold"); 							// L 1 2 3
	//dislin::hwfont(); // -> take system default font


	// Axis attributes
	dislin::axspos( 450, 1700 );								// L 1
	dislin::axslen( 2200, 1200 );								// L 1
	// for DINA4 portrait ("da4p") take these values:		
	//dislin::axspos( 200, 2600 );
	//dislin::axslen( 1350, 1150 );


	dislin::labdig(1,"x");
	dislin::labdig(3,"y");										// L 1 2 3
	// Ticks between numbers in axes
	dislin::ticks(4,"x");										// L 1 2 3
	dislin::ticks(4,"y");										// L 1 2 3


	ic=dislin::intrgb(0.95,0.95,0.95);
	dislin::axsbgd(ic);											// L 1 2 3	

	
	// Set line thickness of frame surrounding graph
	dislin::frame(3);	

	// GRAF sets level to 2
	dislin::graf( XA, XE, XOR, XSTEP, YA, YE, YOR, YSTEP );  	// L 1 

// Level 2

	
	// Scatter plot: add every datapoint and draw circle around it with radius 0.01
	dislin::color("red");										// L 1 2 3
	for ( i=0; i<len_new; i++ )
	{
		dislin::rlcirc ( xray[i], y1ray[i], 0.06 );				// L 1 2 3
	}


	/*dislin::color("fore");
	float ori[2];
	ori[0] = 0.;
	ori[1] = 0.;
	float end[2];
	end[0] = (float) XE;
	end[1] = (float) 0.;	  
	dislin::curve( ori, end, 2 );*/


	dislin::setrgb(0.7,0.7,0.7); 								// L 1 2 3
	dislin::grid(1,1);											// L 2 3


	if( id == 0 ) // plot part of the fitting result : call done by function ActivityModel::print_results(...)
	{
		// Legend attributes
		char cbuf[100];
		dislin::color("fore");										// L 1 2 3
		dislin::legini( cbuf, 1, 10 ); 								// L 1 2 3
		dislin::legtit( " " ); 										// L 2 3
		//dislin::legpos( 1000, 1000 );	
		dislin::leglin( cbuf, "residuals", 1 ); 					// L 1 2 3
		dislin::patcyc( 1, 7);										// L 1 2 3

		// 8 -> plot legend in upper left corner of axis system 
		dislin::legend( cbuf, 8 ); 									// L 2 3	
	}

	dislin::color("fore");										// L 1 2 3
	dislin::height(50); 										// L 1 2 3
	dislin::title();											// L 2 3		


	// Terminate DISLIN
	dislin::disfin();											// L 1 2 3

// Level 0

// ------------------------------------------------------------------------------------ //
// 	Plot percentage deviation of computed values from measured values 

	YA 		= floor( *min_element( y2rayv.begin(), y2rayv.end()) * 1.1 );
	YE 		= ceil( *max_element( y2rayv.begin(), y2rayv.end()) * 1.1 );
	YOR 	= YA;
	YSTEP 	= (YE-YA)/10; 

	if( id == 1 ) // plot percentage residuals
	{
		plotting_info->print_filename = "output_GEMSFIT/residuals_percentage_scatter.";
		plotting_info->print_filename = plotting_info->print_filename + plotting_info->print_format;
		plotting_info->print_xlabel   = "molality";
		plotting_info->print_ylabel   = "( computed / measured ) * 100 - 100";
		plotting_info->print_head     = "System residuals in percent";				
	}

// Level 0	
	dislin::setpag("da4l");
	dislin::metafl(plotting_info->print_format.c_str());
	dislin::setfil(plotting_info->print_filename.c_str());


	// Initialize DISLIN object -> set Level to 1
	dislin::disini();

// Level 1

	dislin::pagera();											// L 1 2 3
//	dislin::complx();

	// Axis labeling
	dislin::name( plotting_info->print_xlabel.c_str(), "x" );	// L 1 2 3
	dislin::name( plotting_info->print_ylabel.c_str(), "y" );	// L 1 2 3


	// Title attributes
	dislin::titlin( plotting_info->print_head.c_str(), 2 );		// L 1 2 3
	//dislin::titlin( "measured versus computed values", 3 );


	// PSFONT: can only be used for ps,eps, pdf and svg formats !!
	dislin::psfont("Helvetica-Bold"); 							// L 1 2 3
	//dislin::hwfont(); // -> take system default font


	// Axis attributes
	dislin::axspos( 450, 1700 );								// L 1
	dislin::axslen( 2200, 1200 );								// L 1
	// for DINA4 portrait ("da4p") take these values:		
	//dislin::axspos( 200, 2600 );
	//dislin::axslen( 1350, 1150 );


	dislin::labdig(1,"x");										// L 1 2 3
	// Ticks between numbers in axes
	dislin::ticks(4,"x");										// L 1 2 3
	dislin::ticks(4,"y");										// L 1 2 3


	ic=dislin::intrgb(0.95,0.95,0.95);
	dislin::axsbgd(ic);											// L 1 2 3	


	// Set line thickness of frame surrounding graph
	dislin::frame(3);	

	
	// GRAF sets level to 2
	dislin::graf( XA, XE, XOR, XSTEP, YA, YE, YOR, YSTEP );  	// L 1 

// Level 2

	
	// Scatter plot: add every datapoint and draw circle around it with radius 0.01
	dislin::color("red");										// L 1 2 3
	for ( i=0; i<len_new; i++ )
	{
		dislin::rlcirc ( xray[i], y2ray[i], 0.05 );				// L 1 2 3
	}


	dislin::setrgb(0.7,0.7,0.7); 								// L 1 2 3
	dislin::grid(1,1);											// L 2 3


/*	// Legend attributes
	dislin::color("fore");										// L 1 2 3
	dislin::legini( cbuf, 1, 10 ); 								// L 1 2 3
	dislin::legtit( " " ); 										// L 2 3
	//dislin::legpos( 1000, 1000 );	
	dislin::leglin( cbuf, "residuals", 1 ); 					// L 1 2 3
	dislin::patcyc( 1, 7);										// L 1 2 3

	// 8 -> plot legend in upper left corner of axis system 
	dislin::legend( cbuf, 8 ); 									// L 2 3	
*/

	dislin::color("fore");										// L 1 2 3
	dislin::height(50); 										// L 1 2 3
	dislin::title();											// L 2 3


	// Terminate DISLIN
	dislin::disfin();											// L 1 2 3

// Level 0




	// Free memory
	delete[] xray;
	delete[] y1ray;
	delete[] y2ray;
// ------------------------------------------------------------------------------------ //

//exit(1);

}



// Print vectors: wrapper for CURVE function from DISLIN for plotting results from sensitivity analysis
void PlotFit::print_vectors_curve( const std::vector<double> &optv_, const std::vector< std::vector<double> > &plot_array, double sensitivity_points )
{
//	plotting_info->print_filename = "output_GEMSFIT/" + plotting_info->print_filename;
	
	plotting_info->print_filename = "output_GEMSFIT/sensitivities.";
	plotting_info->print_filename = plotting_info->print_filename + plotting_info->print_format;
	plotting_info->print_xlabel   = "parameter value";
	plotting_info->print_ylabel   = "sum of squared residuals";
	plotting_info->print_head     = "Sensitivity of SSR with respect to parameter value";				

	int i, ic, k;
	float XA, XE, XOR, XSTEP, YA, YE, YOR, YSTEP; 

	int len = 0;
	len = (int) plot_array[0].size(); 
cout<<"len = "<<len<<endl;

	vector<double> xrayv;
	vector<double> y1rayv;
	vector<double> y2rayv;

	xrayv.resize( len );
	y1rayv.resize( len );
	y2rayv.resize( len );

	float* xray  = new float[ len ];
	float* y1ray = new float[ len ];
	float* y2ray = new float[ len ];


	for( k=0; k< (int) optv_.size(); k++ )		
	{	
		// ------------------------------------------------------------------------------------ //

		// Level 0	
		dislin::setpag("da4l");
		dislin::metafl(plotting_info->print_format.c_str());
		std::string ks = boost::lexical_cast<std::string>(k);
		string filename(plotting_info->print_filename + "_param_" + ks + "." + plotting_info->print_format);

	cout<<"filename = "<<filename<<endl;

		dislin::setfil(filename.c_str());


		// Initialize DISLIN object -> set Level to 1
		dislin::disini();


		// Transfer values form plot_array to xray and y1ray float vectors. 
		// In addition transfer them into two double vector which will be used to find min and max values
		for( i=0; i<len; i++ )		
		{	
			xray[i]   = plot_array[2*k][i];
			y1ray[i]  = plot_array[2*k+1][i];		
	#ifdef GEMSFIT_DEBUG
			cout<<" plot_array[2*k][i] = "<<plot_array[2*k][i]<<" plot_array[2*k+1][i] = "<<plot_array[2*k+1][i]<<endl; 
	#endif
			xrayv[i]  = plot_array[2*k][i];		
			y1rayv[i] = plot_array[2*k+1][i];	
		}

		// Define graph parameters( needed by GRAF routine ) 
		XA 		= *min_element( xrayv.begin(), xrayv.end() );
		XE 		= *max_element( xrayv.begin(), xrayv.end() );
		XOR 	= XE;
		XSTEP 	= (XE-XA)/10; 	

		YA 		= floor( *min_element( y1rayv.begin(), y1rayv.end()) );
		YE 		= ceil( *max_element( y1rayv.begin(), y1rayv.end()) * 1.1 );
		YOR 	= YA;
		YSTEP 	= (YE-YA)/10; 

	#ifdef GEMSFIT_DEBUG
		cout<<" XA = "<<XA<<", XE = "<<XE<<", XOR = "<<XOR<<", XSTEP = "<<XSTEP<<", YA = "<<YA<<", YE = "<<YE<<", YOR = "<<YOR<<", YSTEP = "<<YSTEP<<endl; 
	#endif


	// Level 1

		dislin::pagera();											// L 1 2 3
	//	dislin::complx();

		// Axis labeling
		dislin::name( plotting_info->print_xlabel.c_str(), "x" );	// L 1 2 3
		dislin::name( plotting_info->print_ylabel.c_str(), "y" );	// L 1 2 3


		// Title attributes
		dislin::titlin( plotting_info->print_head.c_str(), 2 );		// L 1 2 3
		//dislin::titlin( "measured versus computed values", 3 );


		// PSFONT: can only be used for ps,eps, pdf and svg formats !!
		dislin::psfont("Helvetica-Bold"); 							// L 1 2 3
		//dislin::hwfont(); // -> take system default font


		// Axis attributes
		dislin::axspos( 450, 1700 );								// L 1
		dislin::axslen( 2200, 1200 );								// L 1
		// for DINA4 portrait ("da4p") take these values:		
		//dislin::axspos( 200, 2600 );
		//dislin::axslen( 1350, 1150 );


		dislin::labdig(-1,"x");										// L 1 2 3
		// Ticks between numbers in axes
		dislin::ticks(4,"x");										// L 1 2 3
		dislin::ticks(4,"y");										// L 1 2 3


		ic=dislin::intrgb(0.95,0.95,0.95);
		dislin::axsbgd(ic);											// L 1 2 3	


		// Set line thickness of frame surrounding graph
		dislin::frame(3);	

		// GRAF sets level to 2
		dislin::graf( XA, XE, XOR, XSTEP, YA, YE, YOR, YSTEP );  	// L 1 

	// Level 2

	//	sensitivity_points	

		// Curve plot
		dislin::color("red");										// L 1 2 3
		dislin::curve(xray, y1ray, len);
	
		dislin::setrgb(0.7,0.7,0.7); 									// L 1 2 3
		dislin::grid(1,1);										// L 2 3


		// Legend attributes
		char cbuf[100];
		dislin::color("fore");										// L 1 2 3
		dislin::legini( cbuf, 1, 10 ); 									// L 1 2 3
		dislin::legtit( " " ); 										// L 2 3
		//dislin::legpos( 1000, 1000 );	
		dislin::leglin( cbuf, "residuals", 1 ); 							// L 1 2 3
		dislin::patcyc( 1, 7);										// L 1 2 3

		// 8 -> plot legend in upper left corner of axis system 
		dislin::legend( cbuf, 8 ); 									// L 2 3	


		dislin::color("fore");										// L 1 2 3
		dislin::height(50); 										// L 1 2 3
		dislin::title();											// L 2 3

		// End graf and set level back to 1
		// dislin::endgrf();
		// Level1
 
		// Terminate DISLIN
		dislin::disfin();											// L 1 2 3


	// ------------------------------------------------------------------------------------ //

	}

	// Level 0

	// Free memory
	delete[] xray;
	delete[] y1ray;
	delete[] y2ray;


}



// Generate and plot histogram
void PlotFit::print_histogram( std::vector<double> &optv_, double** MC_fitted_parameters_all, int MC_runs, int MC_number_of_bars )
{
	int i, j, k, ic;
	int num_digits = 0;	
	float XA, XE, XOR, XSTEP, YA, YE, YOR, YSTEP; 
	float width_bars, low;
	string filename("output_GEMSFIT/MonteCarlo_histogram_parameter_");

	// allocate dynamic memory
	vector<double> xrayv;
	vector<double> frequencyv;

	xrayv.resize( MC_runs );
	frequencyv.resize( MC_number_of_bars );

	float *xray   	    = new float[ MC_runs ];
	float *xhistogram   = new float[ MC_number_of_bars ];
	float *frequency    = new float[ MC_number_of_bars ];
	float *yzero        = new float[ MC_number_of_bars ];



	// Make histogram for every parameter		
	for( j=0; j< (int) optv_.size(); j++ )	
	{
		// Rescale fitted values to values greater than 1 to ease plotting
		for( i=0; i<MC_runs; i++ )	
		{
			xrayv[i] = MC_fitted_parameters_all[ i ][ j ];
#ifdef GEMSFIT_DEBUG
cout<<"before MC_fitted_parameters_all["<<i<<"][1] = "<<MC_fitted_parameters_all[ i ][ 1 ]<<endl;
#endif
		}

		XA 	= *min_element( xrayv.begin(), xrayv.end() );
		XE 	= *max_element( xrayv.begin(), xrayv.end() );
		XSTEP 	= (XE-XA)/10; 	

		num_digits = 0;
		while(XSTEP < 1) {
		    num_digits++;
		    XSTEP *= 10;
		}
#ifdef GEMSFIT_DEBUG
cout<<"num_digits = "<<num_digits<<endl;
#endif
		for( i=0; i<MC_runs; i++ )	
		{
			MC_fitted_parameters_all[ i ][ j ] = MC_fitted_parameters_all[ i ][ j ] * pow(10, num_digits);
#ifdef GEMSFIT_DEBUG
cout<<"after MC_fitted_parameters_all["<<i<<"][1] = "<<MC_fitted_parameters_all[ i ][ 1 ]<<endl;
#endif
		}


		for( i=0; i<MC_runs; i++ )	
		{
			xray[i]  = MC_fitted_parameters_all[ i ][ j ];
			xrayv[i] = MC_fitted_parameters_all[ i ][ j ];
		}


		// prepare histogram
		width_bars =  ( *max_element( xrayv.begin(), xrayv.end() ) -  *min_element( xrayv.begin(), xrayv.end() ) ) / MC_number_of_bars;
		low = *min_element( xrayv.begin(), xrayv.end() );
		for( i=0; i<MC_number_of_bars; i++ )
		{
			frequency[i]  = 0.;
			frequencyv[i] = 0.;
			yzero[i]      =	0.;	
		}

		for( i=0; i<MC_number_of_bars; i++ )
		{
			// position of bars on x axis
			xhistogram[i] = low + (i+0.5)*width_bars;
			for( k=0; k<MC_runs; k++ )
			{	 
				if( xray[k] >= (low + i*width_bars) && xray[k] < (low + (i+1)*width_bars) )
				{
					// count how many results are within the bar
					frequency[i] += 1;
					frequencyv[i] += 1;
		                }
			}
		}


		// Define graph parameters( needed by GRAF routine ) 
// graf ( 0.0, 10.0, 0.0, 1.0, 0.0, 5.0, 0.0, 1.0 );
		XA 	= *min_element( xrayv.begin(), xrayv.end() );
		XE 	= *max_element( xrayv.begin(), xrayv.end() );
		XOR 	= XA;
		XSTEP 	= (XE-XA)/10; 	

		YA 	= 0.;
		YE 	= ceil( *max_element( frequencyv.begin(), frequencyv.end()) * 1.1 );
		YOR 	= YA;
		YSTEP 	= (YE-YA)/10; 

	#ifdef GEMSFIT_DEBUG
		cout<<" XA = "<<XA<<", XE = "<<XE<<", XOR = "<<XOR<<", XSTEP = "<<XSTEP<<", YA = "<<YA<<", YE = "<<YE<<", YOR = "<<YOR<<", YSTEP = "<<YSTEP<<endl; 
	#endif


	// Level 0	
		dislin::setpag("da4l");
		dislin::metafl( plotting_info->print_format.c_str() );
	
		std::string js = boost::lexical_cast<std::string>(j);	
		string filename_( filename + js + "." + plotting_info->print_format );
		dislin::setfil( filename_.c_str() );



		// Initialize DISLIN object -> set Level to 1
		dislin::disini();
	// Level 1

		// Axis labeling
		std::string ns = boost::lexical_cast<std::string>(num_digits);	
		std::string xlab( "parameter value * " );
		std::string xlab_( xlab + "1E-" + ns );
		dislin::name( xlab_.c_str(), "x" );			// L 1 2 3
		dislin::name( "relative frequency", "y" );	// L 1 2 3


		// Title attributes
		dislin::titlin( "Histogram: parameter values versus its frequency", 2 );		// L 1 2 3
		//dislin::titlin( "measured versus computed values", 3 );


		// PSFONT: can only be used for ps,eps, pdf and svg formats !!
		dislin::psfont("Helvetica-Bold"); 							// L 1 2 3
		//dislin::hwfont(); // -> take system default font


		// Axis attributes
		dislin::axspos( 450, 1700 );								// L 1
		dislin::axslen( 2200, 1200 );								// L 1

		dislin::labdig(-1,"x");
													// L 1 2 3
		// Ticks between numbers in axes
		dislin::ticks(4,"x");									// L 1 2 3
		dislin::ticks(4,"y");									// L 1 2 3


		ic=dislin::intrgb(0.95,0.95,0.95);
		dislin::axsbgd(ic);									// L 1 2 3	
		
		// Set line thickness of frame surrounding graph
		dislin::frame(3);	

		// GRAF sets level to 2
		dislin::graf( XA, XE, XOR, XSTEP, YA, YE, YOR, YSTEP );  				// L 1 
	// Level 2

		// Bars: label color
		dislin::color ( "black" );
		// Bars: labels
		dislin::height ( 30 );
		dislin::labels ( "delta", "bars" );
		//dislin::labpos ( "center", "bars" );
		// Bars: shading pattern
		dislin::shdpat( 3 );
		// Bars: width
		dislin::barwth( 0.75 );
		// Bars: color
		dislin::color ( "blue" );
		// Plot bars
		dislin::bars( xhistogram, yzero, frequency, MC_number_of_bars );

		// Reset color
		dislin::color ( "fore" );

		// Terminate DISLIN
		dislin::disfin();											// L 1 2 3

	}
	
	// free dynamic memory
	delete[] xray;
	delete[] xhistogram;
	delete[] frequency;
	delete[] yzero;

}



void PlotFit::print_sensitivity( const int rows, const int cols, const arma::mat &SensitivityMatrix, std::vector< double > &molality )
{
	int i, k, ic, num_digits;
	float XA, XE, XOR, XSTEP, YA, YE, YOR, YSTEP; 

	// Allocate dynamic memory for plotting
	std::vector<float> y1rayv;
	y1rayv.resize( rows );
	float* xray  = new float[ rows ];
	float* y1ray = new float[ rows ];
	float* y2ray = new float[ rows ];

	// load values from Armadillo matrix into dynamic memory array

	// cols: number of fitted parameters
	for( k=0; k< cols; k++ )
	{

		// rows: length of computed values vactor	
		for( i=0; i< rows; i++ )
		{	
			xray[i]   = molality[i];
			y1ray[i]  = SensitivityMatrix(i,k);
			y1rayv[i] = SensitivityMatrix(i,k);

cout<<"xray["<<i<<"] = "<<xray[i]<<endl;	 
cout<<"y1ray["<<i<<"] = "<<y1ray[i]<<endl;
		}

		// Define graph parameters( needed by GRAF routine ) 
		XA 		= *min_element( molality.begin(), molality.end() );
		XE 		= *max_element( molality.begin(), molality.end() );
		XSTEP 	= 1.0; 	

		YA 		= *min_element( y1rayv.begin(), y1rayv.end());
		YE 		= *max_element( y1rayv.begin(), y1rayv.end()) * 1.1;
		YSTEP 	= 1.0; 

		if( YA <= 1.0 )
		{
			num_digits = 0;
			while(YA < 1.0) {
				num_digits++;
				YA *= 10;
			}
			YA      = -num_digits;
		}else
		{
			num_digits = 0;
			while(YA > 1.0) {
				num_digits++;
				YA /= 10;
			}
			YA      = num_digits;
		}

		if( YE <= 1.0 )
		{
			num_digits = 0;
			while(YE < 1.0) {
				num_digits++;
				YE *= 10;
			}
			YE      = -num_digits+1.;
		}else
		{
			num_digits = 0;
			while(YE > 1.) {
				num_digits++;
				YE /= 10;
			}
			YE      = num_digits+1.;
		}

		XOR 	= XA;
		YOR 	= YA;

	#ifdef GEMSFIT_DEBUG
		cout<<" XA = "<<XA<<", XE = "<<XE<<", XOR = "<<XOR<<", XSTEP = "<<XSTEP<<", YA = "<<YA<<", YE = "<<YE<<", YOR = "<<YOR<<", YSTEP = "<<YSTEP<<endl; 
	#endif


		// ------------------------------------------------------------------------------------ //

	// Level 0	
		dislin::setpag("da4l");
		dislin::metafl(plotting_info->print_format.c_str());		

		std::string ks = boost::lexical_cast<std::string>(k);
		string filename( "output_GEMSFIT/Sensitivity_param_" + ks + "." + plotting_info->print_format );
		dislin::setfil(filename.c_str());

		// Initialize DISLIN object -> set Level to 1
		dislin::disini();	  

	// Level 1
		dislin::pagera();											// L 1 2 3
		dislin::pagfll(255);
		dislin::frmclr(0);		
		dislin::axclrs(0,"all","xy");

		// PSFONT: can only be used for ps,eps, pdf and svg formats !!
		dislin::psfont("Helvetica-Bold"); 							// L 1 2 3

		// Axis attributes
		dislin::axspos( 450, 1700 );								// L 1
		dislin::axslen( 2200, 1200 );								// L 1

		// Axis labeling
		dislin::name( " molality ", "x" );	// L 1 2 3
		dislin::name( " gradient at measurement point ", "y" );	// L 1 2 3

		dislin::axsscl("log","y");

		dislin::labdig(-1,"xy");										// L 1 2 3

		dislin::labels("log","y");



		// Title attributes
		dislin::titlin( " Sensitivity at each measurement point ", 2 );		// L 1 2 3



		// Ticks between numbers in axes
		dislin::ticks(5,"x");										// L 1 2 3
		//dislin::ticks(4,"y");										// L 1 2 3


		//ic=dislin::intrgb(0.95,0.95,0.95);
		//dislin::axsbgd(ic);											// L 1 2 3	


		// Set line thickness of frame surrounding graph
		dislin::frame(3);	

		// GRAF sets level to 2
		dislin::graf( XA, XE, XOR, XSTEP, YA, YE, YOR, YSTEP );  	// L 1 

	// Level 2

	//	sensitivity_points	

		// Scatter plot: add every datapoint and draw circle around it with radius 0.01
		dislin::color("red");										// L 1 2 3
		for ( i=0; i<rows; i++ )
		{
			dislin::rlcirc ( xray[i], y1ray[i], 0.07 );				// L 1 2 3
		}

		dislin::setrgb(0.7,0.7,0.7); 									// L 1 2 3
		dislin::grid(1,1);										// L 2 3


		// Legend attributes
		//char cbuf[100];
		//dislin::color("fore");										// L 1 2 3
		//dislin::legini( cbuf, 1, 10 ); 									// L 1 2 3
		//dislin::legtit( " " ); 										// L 2 3
		//dislin::legpos( 1000, 1000 );	
		//dislin::leglin( cbuf, "residuals", 1 ); 							// L 1 2 3
		//dislin::patcyc( 1, 7);										// L 1 2 3

		// 8 -> plot legend in upper left corner of axis system 
		//dislin::legend( cbuf, 8 ); 									// L 2 3	


		dislin::color("fore");										// L 1 2 3
		dislin::height(50); 										// L 1 2 3
		dislin::title();											// L 2 3

		// End graf and set level back to 1
		// dislin::endgrf();
		// Level1
 
		// Terminate DISLIN
		dislin::disfin();											// L 1 2 3


	}// end loop over number of fitted parameters


	
	
	
	// Free dynamic memory
	delete[] xray;
	delete[] y1ray;
	delete[] y2ray;

}



void PlotFit::print_SSR_contour( const int param_pair[2], const int num_param, const int points_per_axis, float* SSR_matrix, double* param_1_ray, double* param_2_ray )
{

	int i, jj, k, ll, num_digits;
	float zlev;
	float XA, XE, XOR, XSTEP, YA, YE, YOR, YSTEP, ZA, ZE, ZOR, ZSTEP; 
	float* xray  = new float[ points_per_axis ];
	float* y1ray = new float[ points_per_axis ];

	vector<float> xrayv;
	xrayv.resize( points_per_axis );		
	vector<float> y1rayv;
	y1rayv.resize( points_per_axis );		
	vector<float> zmatv_row_max;
	vector<float> zmatv_row_min;
	vector<float> zmatv_row;
	zmatv_row.resize( points_per_axis );
	zmatv_row_min.resize( points_per_axis );
	zmatv_row_max.resize( points_per_axis );


	// Extract values to be plotted
	for( jj=0; jj< points_per_axis; jj++ )	
	{		
		for( ll=0; ll< points_per_axis; ll++ )
		{
	
			xray[jj] 	 = param_1_ray[jj];
			y1ray[ll] 	 = param_2_ray[ll];			
#ifdef GEMSFIT_DEBUG
cout<<"SSR_matrix["<<jj<<"]["<<ll<<"] = "<<SSR_matrix[ points_per_axis*jj + ll ]<<endl;		
#endif
			zmatv_row[ll] = SSR_matrix[ points_per_axis*jj + ll ];
			xrayv[jj]	 = xray[jj];
			y1rayv[ll] 	 = y1ray[ll];
		}
		zmatv_row_min[jj] = *min_element( zmatv_row.begin(), zmatv_row.end() );				
		zmatv_row_max[jj] = *max_element( zmatv_row.begin(), zmatv_row.end() );				
	}		

	// Define graph parameters( needed by GRAF routine ) 
	XA 		= *min_element( xrayv.begin(), xrayv.end() );
	XE 		= *max_element( xrayv.begin(), xrayv.end() );
	XSTEP 	= (XE-XA)/10; 	
	XOR 	= XA;
	YA 		= *min_element( y1rayv.begin(), y1rayv.end());
	YE 		= *max_element( y1rayv.begin(), y1rayv.end()) * 1.1;
	YSTEP 	= (YE-YA)/10;
	YOR 	= YA;
	ZA		= *min_element( zmatv_row_min.begin(), zmatv_row_min.end() );				
	ZE		= *max_element( zmatv_row_max.begin(), zmatv_row_max.end() );				
	ZSTEP 	= (ZE-ZA)/10;
	ZOR 	= ZA;


#ifdef GEMSFIT_DEBUG
cout<<" XA = "<<XA<<", XE = "<<XE<<", XOR = "<<XOR<<", XSTEP = "<<XSTEP<<", YA = "<<YA<<", YE = "<<YE<<", YOR = "<<YOR<<", YSTEP = "<<YSTEP<<", ZA = "<<ZA<<", ZE = "<<ZE<<", ZOR = "<<ZOR<<", ZSTEP = "<<ZSTEP<<endl; 
#endif

	// Rescale all values to values greater 1
	int num_digitsX = 0;
	while(XSTEP < 1) {
		num_digitsX++;
		XSTEP *= 10;
	}
	int num_digitsY = 0;
	while(YSTEP < 1) {
		num_digitsY++;
		YSTEP *= 10;
	}

	for( jj=0; jj< points_per_axis; jj++ )	
	{
		xray[jj] 	 = xray[jj]*pow(10, num_digitsX);
		xrayv[jj]	 = xray[jj];
	}		

	for( ll=0; ll< points_per_axis; ll++ )
	{
		y1ray[ll] 	 = y1ray[ll]*pow(10, num_digitsY);			
		y1rayv[ll] 	 = y1ray[ll];
	}

	XA 		= *min_element( xrayv.begin(), xrayv.end() );
	XE 		= *max_element( xrayv.begin(), xrayv.end() );
	XSTEP 	= (XE-XA)/10; 	
	XOR 	= XA;
	
	YA 		= *min_element( y1rayv.begin(), y1rayv.end());
	YE 		= *max_element( y1rayv.begin(), y1rayv.end()) * 1.1;
	YSTEP 	= (YE-YA)/10;
	YOR 	= YA;

#ifdef GEMSFIT_DEBUG
cout<<" XA = "<<XA<<", XE = "<<XE<<", XOR = "<<XOR<<", XSTEP = "<<XSTEP<<", YA = "<<YA<<", YE = "<<YE<<", YOR = "<<YOR<<", YSTEP = "<<YSTEP<<endl; 
#endif


	// Prepare DISLIN input
	std::string digXs = boost::lexical_cast<std::string>(num_digitsX);
	std::string digYs = boost::lexical_cast<std::string>(num_digitsY);			
	std::string ks = boost::lexical_cast<std::string>(param_pair[0]);
	std::string is = boost::lexical_cast<std::string>(param_pair[1]);
	string xax("parameter X" + is +" * 1E" + digXs );
	string yax("parameter X" + ks +" * 1E" + digYs );
	string tit2("Parameters " + is + " and " + ks );



	// Generate contour plot
	// output format
	dislin::metafl( plotting_info->print_format.c_str() );		

	// output filename
	string filename_c( "output_GEMSFIT/SSR_contour_param_" + is + "_" + ks + "." + plotting_info->print_format );
	dislin::setfil(filename_c.c_str());

	dislin::setpag("da4l");
	dislin::disini();
	dislin::complx();
	dislin::pagera();

	// PSFONT: can only be used for ps,eps, pdf and svg formats !!
	dislin::psfont("Helvetica-Bold"); 							// L 1 2 3

	dislin::titlin("Sum of Squared Residuals",1);
	dislin::titlin(tit2.c_str(),3);

	dislin::name(xax.c_str(),"x");
	dislin::name(yax.c_str(),"y");

	dislin::intax();
	dislin::axspos( 450, 1700 );								// L 1
	dislin::axslen( 1600, 1600 );								// L 1
	// Push all table parts to the center
	dislin::center();

	dislin::graf( XA, XE, XOR, XSTEP, YA, YE, YOR, YSTEP );

	dislin::height(30);
	for (i = 0; i < 100; i++)
	{ 
		zlev = -2.+i*0.5;
		dislin::setclr ((i+1) * 25);
		if (i == 4)
		  dislin::labels("none","contur"); 
		else
		  dislin::labels("float","contur");

		dislin::contur( xray, points_per_axis, y1ray, points_per_axis, (float *) SSR_matrix, zlev );
	}

	dislin::height(50);
	dislin::color("fore");
	dislin::title();

	dislin::disfin();





	// Generate 3D coloured plot
	// page format
	dislin::setpag("da4l");

	// output format
	dislin::metafl(plotting_info->print_format.c_str());		

	// output filename
	ks = boost::lexical_cast<std::string>(param_pair[0]);
	is = boost::lexical_cast<std::string>(param_pair[1]);
	string filename( "output_GEMSFIT/SSR_3D_param_" + is + "_" + ks + "." + plotting_info->print_format );
	dislin::setfil(filename.c_str());

	// Initialize DISLIN object -> set Level to 1
	dislin::disini();	  

// Level 1
	dislin::pagera();											// L 1 2 3

	// PSFONT: can only be used for ps,eps, pdf and svg formats !!
	dislin::psfont("Helvetica-Bold"); 							// L 1 2 3

	dislin::titlin("Sum of Squared Residuals",1);
	dislin::titlin(tit2.c_str(),3);

	dislin::name(xax.c_str(),"x");
	dislin::name(yax.c_str(),"y");
	dislin::name("sum of squared residuals","z");


	// Axis attributes
	dislin::axspos( 300, 1850 );								// L 1
	dislin::ax3len( 1600, 1600, 1400 );							// L 1

	dislin::labdig(-1,"xy");									// L 1 2 3

	// Set line thickness of frame surrounding graph
	dislin::frame(3);	

	dislin::intax();
	dislin::autres( points_per_axis, points_per_axis );

	// Push all table parts to the center
	dislin::center();

	// GRAF sets level to 2
	dislin::graf3( XA, XE, XOR, XSTEP, YA, YE, YOR, YSTEP, ZA, ZE, ZOR, ZSTEP );  	// L 1 
	
	dislin::crvmat( (float *) SSR_matrix, points_per_axis, points_per_axis, 1, 1 );

	dislin::height(50);
	dislin::color("fore");
	dislin::title();
	dislin::disfin();					


	


	// Free dynamic memory
	delete[] xray;
	delete[] y1ray;

//exit(1);

}













