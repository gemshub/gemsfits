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
 *	@file measdata_modified.cpp
 *
 *	@brief this source file contains the implementation of the functions of data manager class, 
 *	which retrieve measurement data from the measurement data *csv file or PostgreSQL server. 
 *
 *	@author Ferdinand F. Hingerl
 *
 * 	@date 09.04.2012 
 *
 */

/* Processed by ecpg (4.7.0) */
/* These include files are added by the preprocessor */
#include <ecpglib.h>
#include <ecpgerrno.h>
#include <sqlca.h>
/* End of automatic include section */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>

//#include "system_properties.h"
#include "data_manager.h"

void SS_Data_Manager::get_DB(  )
{

    // GEMSFIT logfile
    const char path[200] = "output_GEMSFIT/SS_GEMSFIT.log";
    ofstream fout;
    fout.open(path, ios::app);
    if( fout.fail() )
    { cout<<"Output fileopen error"<<endl; exit(1); }

    fout << "6. in measdata_modified.cpp line 59. Continuing with reading from database." << endl << endl;


     int i=0; // counts in primary experiments table
     int j=0; // counts in secondary tables
     char dbname [ 256 ] ;
     int rows; // rows of primary experiments table
     int rows_c; // rows of secondary tables

     int id_exp_;               int id_exp_i;
     char name_exp_[256];       int name_exp_i;
     int T_exp_;                int T_exp_i;
     int P_exp_;                int P_exp_i;
     char ref_exp_[256];        int ref_exp_i;

     int uT_exp_;               int uT_exp_i;
     int uP_exp_;               int uP_exp_i;

     char name_comp_[256];      int name_comp_i;
     double quant_comp_;        int quant_comp_i;
     char units_comp_[1];       int units_comp_i;

     char name_elem_[256];      int name_elem_i;
     double sol_elem_;          int sol_elem_i;
     double logsol_elem_;       int logsol_elem_i;
     double errsol_elem_;       int errsol_elem_i;


     // Declare cursor variables - SQL comands for geting and selecting data form the db //

     char id_exp_ecpg_1[256] = "declare id_exp cursor for SELECT id FROM "; const char * id_exp_ecpg;
     char name_exp_ecpg_1[256] = "declare name_exp cursor for SELECT name FROM "; const char * name_exp_ecpg;
     char T_exp_ecpg_1[256] = "declare T_exp cursor for SELECT temperature FROM "; const char * T_exp_ecpg;
     char P_exp_ecpg_1[256] = "declare P_exp cursor for SELECT pressure FROM "; const char * P_exp_ecpg;
     char ref_exp_ecpg_1[256] = "declare ref_exp cursor for SELECT reference FROM "; const char * ref_exp_ecpg;

     char unique_TP_ecpg_1[256] = "declare unique_TP cursor for SELECT DISTINCT temperature, pressure FROM "; const char * unique_TP_ecpg;

     char meas[100] = "SELECT COUNT ( * ) id FROM "; const char* meas_table;        // sql query to select all data form experiments table
     char unique_TP[120] ="SELECT COUNT ( * ) pressure FROM ( SELECT DISTINCT temperature, pressure FROM experiments ) AS unique_TP";


     // // // // // // // Connect to database // // // // // // //
     {
         string tcp_psql("tcp:postgresql://");
         string port(":5432/");
         string server_share;

         server_share = tcp_psql + psql_server + port + DBname;
         ECPGconnect(__LINE__, 0, server_share.c_str() , username.c_str() , passwd.c_str() , "con1", 0);
         if (sqlca.sqlwarn[0] == 'W')                                               // W will be set if an warning was issued
         {
             sqlprint();                                                            // prints error message to the standard error
             printf("error code %d, message %s, rows %d, warning %c\n",
             sqlca.sqlcode,sqlca.sqlerrm.sqlerrmc, sqlca.sqlerrd[2],                // sqlcode - error code; sqlca.sqlrrm.sqlerrmc - contains an error description; sqlerrd - used when returning data with cursors
             sqlca.sqlwarn[0]);
         }
         if (sqlca.sqlcode < 0)
         {
             sqlprint();
             printf("You entered following specifications for accessing the PostgreSQL database: \n username %s, \n database %s. \n",
             username.c_str(), DBname.c_str() );

             printf("error code %d, message %s, rows %d, warning %c\n",
             sqlca.sqlcode,sqlca.sqlerrm.sqlerrmc, sqlca.sqlerrd[2],
             sqlca.sqlwarn[0]);
             exit(1);
         }
         if (sqlca.sqlwarn[0] == 'W') sqlprint();
         if (sqlca.sqlcode < 0) sqlprint();
         if (sqlca.sqlcode < 0) exit(1);
     }
    // // // // // // // END Connect to database // // // // // // //


     string tablename("experiments ORDER BY pressure");
     string tablename2("experiments");
     strcat(meas, tablename2.c_str());
     meas_table = meas;

     // Count number of experiments
     { ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, meas_table, ECPGt_EOIT,
     ECPGt_int,&(rows),(long)1,(long)1,sizeof(int),
     ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EORT);

 if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();
 if (sqlca.sqlwarn[0] == 'W') sqlprint();
 if (sqlca.sqlcode < 0) sqlprint();
 if (sqlca.sqlcode < 0) exit(1);}
     fout<<"Number of experiments = "<<rows<<endl;

     // Count number of unique TP_pairs
     { ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, unique_TP, ECPGt_EOIT,
     ECPGt_int,&(rows_c),(long)1,(long)1,sizeof(int),
     ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EORT);

 if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();
 if (sqlca.sqlwarn[0] == 'W') sqlprint();
 if (sqlca.sqlcode < 0) sqlprint();
 if (sqlca.sqlcode < 0) exit(1);}
     fout<<"Number of unique TP_pairs = "<<rows_c<<endl;

     // Initializing the experiments
     for (i=1; i<rows; i++) {
         alldata.push_back(new experiment);
     }

     if (!(rows == alldata.size())) {
        fout << "Size of experiments vector different form number of experiments!! " << alldata.size() << endl;
        exit(1);
     }

     // Initializing unique TP_pairs
     for (i=0; i<rows_c; i++) {
         TP_pairs[0].push_back(0); // temperature
         TP_pairs[1].push_back(0); // pressure
     }

     if (!(rows == alldata.size())) {
        fout << "Size of experiments vector different form number of experiments!! " << alldata.size() << endl;
        exit(1);
     }

     // Declare cursors for experiments table//
     string tablenamex = "experiments ORDER BY id"; // to have order in the experimetns, avoid some strange sql behaviour

     /* declare id_exp cursor for select id from db_meas_table */
     strcat(id_exp_ecpg_1, tablenamex.c_str());
     id_exp_ecpg = id_exp_ecpg_1; // cursor, given to a const char
     { ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, id_exp_ecpg, ECPGt_EOIT, ECPGt_EORT);
 if (sqlca.sqlwarn[0] == 'W') sqlprint();
 if (sqlca.sqlcode < 0) sqlprint();
 if (sqlca.sqlcode < 0) exit(1);}

     /* declare name_exp cursor for select name from db_meas_table */
     strcat(name_exp_ecpg_1, tablenamex.c_str());
     name_exp_ecpg = name_exp_ecpg_1; // cursor, given to a const char
     { ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, name_exp_ecpg, ECPGt_EOIT, ECPGt_EORT);
 if (sqlca.sqlwarn[0] == 'W') sqlprint();
 if (sqlca.sqlcode < 0) sqlprint();
 if (sqlca.sqlcode < 0) exit(1);}

     /* declare T_exp cursor for select temperature from db_meas_table */
     strcat(T_exp_ecpg_1, tablenamex.c_str());
     T_exp_ecpg = T_exp_ecpg_1; // cursor, given to a const char
     { ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, T_exp_ecpg, ECPGt_EOIT, ECPGt_EORT);
 if (sqlca.sqlwarn[0] == 'W') sqlprint();
 if (sqlca.sqlcode < 0) sqlprint();
 if (sqlca.sqlcode < 0) exit(1);}

     /* declare P_exp cursor for select pressure from db_meas_table */
     strcat(P_exp_ecpg_1, tablenamex.c_str());
     P_exp_ecpg = P_exp_ecpg_1; // cursor, given to a const char
     { ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, P_exp_ecpg, ECPGt_EOIT, ECPGt_EORT);
 if (sqlca.sqlwarn[0] == 'W') sqlprint();
 if (sqlca.sqlcode < 0) sqlprint();
 if (sqlca.sqlcode < 0) exit(1);}

     /* declare ref_exp cursor for select reference from db_meas_table */
     strcat(ref_exp_ecpg_1, tablenamex.c_str());
     ref_exp_ecpg = ref_exp_ecpg_1; // cursor, given to a const char
     { ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, ref_exp_ecpg, ECPGt_EOIT, ECPGt_EORT);
 if (sqlca.sqlwarn[0] == 'W') sqlprint();
 if (sqlca.sqlcode < 0) sqlprint();
 if (sqlca.sqlcode < 0) exit(1);}

     /* declare unique_TP cursor for select unique TP pairs from db_meas_table */
     strcat(unique_TP_ecpg_1, tablename.c_str());
     unique_TP_ecpg = unique_TP_ecpg_1; // cursor, given to a const char
     { ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, unique_TP_ecpg, ECPGt_EOIT, ECPGt_EORT);
 if (sqlca.sqlwarn[0] == 'W') sqlprint();
 if (sqlca.sqlcode < 0) sqlprint();
 if (sqlca.sqlcode < 0) exit(1);}

     // // // // // // // Reading unique TP_pairs from the database - they are not in the same order as the experiments // // // // // // //

     // reading from database
     for (j=0; j<rows_c; j++) {

         // reading unique_TP //
         { ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch unique_TP", ECPGt_EOIT,
     ECPGt_int,&(uT_exp_),(long)1,(long)1,sizeof(int),            // temperature
     ECPGt_int,&(uT_exp_i),(long)1,(long)1,sizeof(int),
     ECPGt_int,&(uP_exp_),(long)1,(long)1,sizeof(int),            // pressure
     ECPGt_int,&(uP_exp_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

 if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

 if (sqlca.sqlwarn[0] == 'W') sqlprint();

 if (sqlca.sqlcode < 0) sqlprint();
 if (sqlca.sqlcode < 0) exit(1);}

         if( uT_exp_i==0 ){ TP_pairs[0][j] = uT_exp_; };
         if( uT_exp_i>0 ){ TP_pairs[0][j] = uT_exp_; fout<<"uT_exp was truncated while storing in host variable !!!"<<endl; };

         if( uP_exp_i==0 ){ TP_pairs[1][j] = uP_exp_; };
         if( uP_exp_i>0 ){ TP_pairs[1][j] = uP_exp_; fout<<"uP_exp was truncated while storing in host variable !!!"<<endl; };

//         cout <<j+1 <<" unique TP_pair has temperature = "<< TP_pairs[0][j] << " and pressure = "<< TP_pairs[1][j] << endl;


     }

//    cout << TP_pairs[0].size();

     // close unique_TP cursor cursor
     { ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close unique_TP", ECPGt_EOIT, ECPGt_EORT);

 if (sqlca.sqlwarn[0] == 'W') sqlprint();

 if (sqlca.sqlcode < 0) sqlprint();
 if (sqlca.sqlcode < 0) exit(1);}

     // // // // // // // Reading in the experimetns form the database // // // // // // //

     for (i=0; i<rows; i++)
     {
         // reading id_exp //
         { ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch id_exp", ECPGt_EOIT,
     ECPGt_int,&(id_exp_),(long)1,(long)1,sizeof(int),
     ECPGt_int,&(id_exp_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

 if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

 if (sqlca.sqlwarn[0] == 'W') sqlprint();

 if (sqlca.sqlcode < 0) sqlprint();
 if (sqlca.sqlcode < 0) exit(1);}

         if( id_exp_i==0 ){ alldata[i]->id_exp = id_exp_; };
         if( id_exp_i>0 ){ alldata[i]->id_exp = id_exp_; fout<<"id_exp was truncated while storing in host variable !!!"<<endl; };

        // fout<<"id on row "<<i+1<<" = "<<alldata[i]->id_exp <<endl;

         // reading name_exp //
         { ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch name_exp", ECPGt_EOIT,
     ECPGt_char,&(name_exp_),(long)256,(long)1,(256)*sizeof(char),
     ECPGt_int,&(name_exp_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

 if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

 if (sqlca.sqlwarn[0] == 'W') sqlprint();

 if (sqlca.sqlcode < 0) sqlprint();
 if (sqlca.sqlcode < 0) exit(1);}

         if( name_exp_i==0 ){ alldata[i]->name = name_exp_; };
         if( name_exp_i>0 ){ alldata[i]->name = name_exp_; fout<<"name_exp was truncated while storing in host variable !!!"<<endl; };

//         cout<<"id on row "<<i+1<<" = "<<alldata[i]->name <<endl;

         // reading T_exp //
         { ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch T_exp", ECPGt_EOIT,
     ECPGt_int,&(T_exp_),(long)1,(long)1,sizeof(int),
     ECPGt_int,&(T_exp_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

 if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

 if (sqlca.sqlwarn[0] == 'W') sqlprint();

 if (sqlca.sqlcode < 0) sqlprint();
 if (sqlca.sqlcode < 0) exit(1);}

         if( T_exp_i==0 ){ alldata[i]->TC = T_exp_; };
         if( T_exp_i>0 ){ alldata[i]->TC = T_exp_; fout<<"T_exp was truncated while storing in host variable !!!"<<endl; };

//         cout<<"temp in experiment "<<i+1<<" = "<<alldata[i]->TC <<endl;

         // reading P_exp //
         { ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch P_exp", ECPGt_EOIT,
     ECPGt_int,&(P_exp_),(long)1,(long)1,sizeof(int),
     ECPGt_int,&(P_exp_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

 if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

 if (sqlca.sqlwarn[0] == 'W') sqlprint();

 if (sqlca.sqlcode < 0) sqlprint();
 if (sqlca.sqlcode < 0) exit(1);}

         if( P_exp_i==0 ){ alldata[i]->PG = P_exp_; };
         if( P_exp_i>0 ){ alldata[i]->PG = P_exp_; fout<<"P_exp was truncated while storing in host variable !!!"<<endl; };

//         cout<<"pressure in experiment "<<i+1<<" = "<<alldata[i]->PG <<endl;

         // reading ref_exp //
         { ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch ref_exp", ECPGt_EOIT,
     ECPGt_char,&(ref_exp_),(long)256,(long)1,(256)*sizeof(char),
     ECPGt_int,&(ref_exp_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

 if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

 if (sqlca.sqlwarn[0] == 'W') sqlprint();

 if (sqlca.sqlcode < 0) sqlprint();
 if (sqlca.sqlcode < 0) exit(1);}

         if( ref_exp_i==0 ){ alldata[i]->reference = ref_exp_; };
         if( ref_exp_i>0 ){ alldata[i]->reference = ref_exp_; fout<<"ref_exp was truncated while storing in host variable !!!"<<endl; };

//         cout<<"reference in experiment "<<i+1<<" = "<<alldata[i]->reference <<endl;


// ++++++++++++++++++++++++++  // reading components // +++++++++++++++++++++++++++++++++ //

         std::stringstream ss;
         ss << i+1;
         std::string I = ss.str(); // transforming the i th experiment counter into a string

         /* declare name_comp cursor variables */
         char name_comp_ecpg_1[256] ="declare all_comp cursor for SELECT components.name, exp_comp.quantity, exp_comp.units FROM exp_comp LEFT JOIN components ON components.id = exp_comp.id_comp WHERE exp_comp.id_exp = "; const char * name_comp_ecpg;
         char count_name_comp_ecpg_1[256] ="SELECT COUNT (*) id_comp FROM exp_comp LEFT JOIN components ON components.id = exp_comp.id_comp WHERE exp_comp.id_exp = "; const char * count_name_comp_ecpg;

         strcat(name_comp_ecpg_1, I.c_str()); // adding the I string to the SQL comand
         strcat(count_name_comp_ecpg_1, I.c_str());

         count_name_comp_ecpg = count_name_comp_ecpg_1;
         name_comp_ecpg = name_comp_ecpg_1;

         // cout << name_comp_ecpg << endl;

         // Count number of componentes
         { ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, count_name_comp_ecpg, ECPGt_EOIT,
         ECPGt_int,&(rows_c),(long)1,(long)1,sizeof(int),
         ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EORT);

     if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();
     if (sqlca.sqlwarn[0] == 'W') sqlprint();
     if (sqlca.sqlcode < 0) sqlprint();
     if (sqlca.sqlcode < 0) exit(1);}

//         cout<<"Number of components for experiment " <<i+1<<" = "<<rows_c<<endl;

         // Initializing the experiments
         for (j=0; j<rows_c; j++) {
             alldata[i]->component_name.push_back("");
             alldata[i]->component_amount.push_back(0.0);
             alldata[i]->units.push_back("");
         }

//         cout << alldata[i]->component_name.size() << endl;

         /* declare name_comp cursor for select components and their amount assigned to each experiemnt from the components table */
          { ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, name_comp_ecpg, ECPGt_EOIT, ECPGt_EORT);
      if (sqlca.sqlwarn[0] == 'W') sqlprint();
      if (sqlca.sqlcode < 0) sqlprint();
      if (sqlca.sqlcode < 0) exit(1);}

         // reading from database
         for (j=0; j<rows_c; j++) {

             // reading name_comp //
             { ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch all_comp", ECPGt_EOIT,
         ECPGt_char,&(name_comp_),(long)256,(long)1,(256)*sizeof(char),         // components name
         ECPGt_int,&(name_comp_i),(long)1,(long)1,sizeof(int),
         ECPGt_double,&(quant_comp_),(long)1,(long)1,sizeof(double),            // components quantity
         ECPGt_int,&(quant_comp_i),(long)1,(long)1,sizeof(int),
         ECPGt_char,&(units_comp_),(long)5,(long)1,(5)*sizeof(char),            // components units
         ECPGt_int,&(units_comp_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

     if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

     if (sqlca.sqlwarn[0] == 'W') sqlprint();

     if (sqlca.sqlcode < 0) sqlprint();
     if (sqlca.sqlcode < 0) exit(1);}

             if( name_comp_i==0 ){ alldata[i]->component_name[j] = name_comp_; };
             if( name_comp_i>0 ){ alldata[i]->component_name[j] = name_comp_; fout<<"name_comp was truncated while storing in host variable !!!"<<endl; };

//             cout<<"component nr. "<<j+1<<" from experiment "<<i+1<<" = "<<alldata[i]->component_name[j] <<endl;

             if( quant_comp_i==0 ){ alldata[i]->component_amount[j] = quant_comp_; };
             if( quant_comp_i>0 ){ alldata[i]->component_amount[j] = quant_comp_; fout<<"quant_comp was truncated while storing in host variable !!!"<<endl; };

//             cout<<"component "<<alldata[i]->component_name[j]<<" from experiment "<<i+1<<" = "<<alldata[i]->component_amount[j] <<endl;

             if( units_comp_i==0 ){ alldata[i]->units[j] = units_comp_; };
             if( units_comp_i>0 ){ alldata[i]->units[j] = units_comp_; fout<<"units_comp was truncated while storing in host variable !!!"<<endl; };

//             cout<<"component "<<alldata[i]->component_name[j]<<" from experiment "<<i+1<<" has unit:  "<<alldata[i]->units[j] <<endl;
         }

         // close name_comp cursor cursor
         { ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close all_comp", ECPGt_EOIT, ECPGt_EORT);

     if (sqlca.sqlwarn[0] == 'W') sqlprint();

     if (sqlca.sqlcode < 0) sqlprint();
     if (sqlca.sqlcode < 0) exit(1);}

// ++++++++++++++++++++++++++  // END reading components // +++++++++++++++++++++++++++++++++ //

// ++++++++++++++++++++++++++  // reading results        // +++++++++++++++++++++++++++++++++ //

         /* declare name_comp cursor variables */
         char name_elem_ecpg_1[256] ="declare all_elem cursor for SELECT elements.name, results.solubility, results.log_solubility, results.error FROM results LEFT JOIN elements ON elements.id = results.id_elem WHERE results.id_exp = "; const char * name_elem_ecpg;
         char count_name_elem_ecpg_1[256] ="SELECT COUNT (*) id_elem FROM results LEFT JOIN elements ON elements.id = results.id_elem WHERE results.id_exp = "; const char * count_name_elem_ecpg;

         strcat(name_elem_ecpg_1, I.c_str()); // adding the I string to the SQL comand
         strcat(count_name_elem_ecpg_1, I.c_str());

         count_name_elem_ecpg = count_name_elem_ecpg_1;
         name_elem_ecpg = name_elem_ecpg_1;

         // cout << name_comp_ecpg << endl;

         // Count number of componentes
         { ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, count_name_elem_ecpg, ECPGt_EOIT,
         ECPGt_int,&(rows_c),(long)1,(long)1,sizeof(int),
         ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EORT);

     if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();
     if (sqlca.sqlwarn[0] == 'W') sqlprint();
     if (sqlca.sqlcode < 0) sqlprint();
     if (sqlca.sqlcode < 0) exit(1);}

//         cout<<"Number of elements for experiment " <<i+1<<" = "<<rows_c<<endl;

         // Initializing the elements
         for (j=0; j<rows_c; j++) {
             alldata[i]->name_elem.push_back("");
             alldata[i]->solubility.push_back(0.0);
             alldata[i]->log_solubility.push_back(0.0);
             alldata[i]->error_sol.push_back(0.0);
         }

         /* declare all_elem cursor for select elements and their solubility assigned to each experiemnt from the elements table */
          { ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, name_elem_ecpg, ECPGt_EOIT, ECPGt_EORT);
      if (sqlca.sqlwarn[0] == 'W') sqlprint();
      if (sqlca.sqlcode < 0) sqlprint();
      if (sqlca.sqlcode < 0) exit(1);}


         // reading from database
         for (j=0; j<rows_c; j++) {

             // reading name_elem //
             { ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch all_elem", ECPGt_EOIT,
         ECPGt_char,&(name_elem_),(long)256,(long)1,(256)*sizeof(char),         // elements name
         ECPGt_int,&(name_elem_i),(long)1,(long)1,sizeof(int),
         ECPGt_double,&(sol_elem_),(long)1,(long)1,sizeof(double),            // elements solubility
         ECPGt_int,&(sol_elem_i),(long)1,(long)1,sizeof(int),
         ECPGt_double,&(logsol_elem_),(long)1,(long)1,sizeof(double),            // components units
         ECPGt_int,&(logsol_elem_i),(long)1,(long)1,sizeof(int),
         ECPGt_double,&(errsol_elem_),(long)1,(long)1,sizeof(double),            // components units
         ECPGt_int,&(errsol_elem_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

     if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

     if (sqlca.sqlwarn[0] == 'W') sqlprint();

     if (sqlca.sqlcode < 0) sqlprint();
     if (sqlca.sqlcode < 0) exit(1);}

             if( name_elem_i==0 ){ alldata[i]->name_elem[j] = name_elem_; };
             if( name_elem_i>0 ){ alldata[i]->name_elem[j] = name_elem_; fout<<"name_elem was truncated while storing in host variable !!!"<<endl; };

//             cout<<"element nr. "<<j+1<<" from experiment "<<i+1<<" = "<<alldata[i]->name_elem[j] <<endl;

             if( sol_elem_i==0 ){ alldata[i]->solubility[j] = sol_elem_; };
             if( sol_elem_i>0 ){ alldata[i]->solubility[j] = sol_elem_; fout<<"sol_elem was truncated while storing in host variable !!!"<<endl; };

//             cout<<"the solubility of element "<<alldata[i]->name_elem[j] <<" from experiment "<<i+1<<" = "<<alldata[i]->solubility[j] <<endl;

             if( logsol_elem_i==0 ){ alldata[i]->log_solubility[j] = logsol_elem_; };
             if( logsol_elem_i>0 ){ alldata[i]->log_solubility[j] = logsol_elem_; fout<<"logsol_elem was truncated while storing in host variable !!!"<<endl; };

//             cout<<"the log solubility of element "<<alldata[i]->name_elem[j] <<" from experiment "<<i+1<<" = "<<alldata[i]->log_solubility[j] <<endl;

             if( errsol_elem_i==0 ){ alldata[i]->error_sol[j] = errsol_elem_; };
             if( errsol_elem_i>0 ){ alldata[i]->error_sol[j] = errsol_elem_; fout<<"errsol_elem was truncated while storing in host variable !!!"<<endl; };

//             cout<<"the solubility error of element "<<alldata[i]->name_elem[j]<<" from experiment "<<i+1<<" = "<<alldata[i]->error_sol[j] <<endl;
         }

         // close all_elem cursor cursor
         { ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close all_elem", ECPGt_EOIT, ECPGt_EORT);

     if (sqlca.sqlwarn[0] == 'W') sqlprint();

     if (sqlca.sqlcode < 0) sqlprint();
     if (sqlca.sqlcode < 0) exit(1);}

// ++++++++++++++++++++++++++  // END reading results // +++++++++++++++++++++++++++++++++ //

     }


     int c_comp= alldata[alldata.size()-1]->component_name.size()-1;
     fout << "TEST: There is "<< alldata[alldata.size()-1]->component_amount[c_comp] <<" "<< alldata[alldata.size()-1]->units[c_comp] <<" " <<alldata[alldata.size()-1]->component_name[c_comp]<<" in experiment " << alldata.size() <<" " << alldata[alldata.size()-1]->name << endl << endl;


     // close cursors for reading form experiments table //

     { ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close id_exp", ECPGt_EOIT, ECPGt_EORT);

 if (sqlca.sqlwarn[0] == 'W') sqlprint();

 if (sqlca.sqlcode < 0) sqlprint();
 if (sqlca.sqlcode < 0) exit(1);}

     { ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close name_exp", ECPGt_EOIT, ECPGt_EORT);

 if (sqlca.sqlwarn[0] == 'W') sqlprint();

 if (sqlca.sqlcode < 0) sqlprint();
 if (sqlca.sqlcode < 0) exit(1);}

     { ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close T_exp", ECPGt_EOIT, ECPGt_EORT);

 if (sqlca.sqlwarn[0] == 'W') sqlprint();

 if (sqlca.sqlcode < 0) sqlprint();
 if (sqlca.sqlcode < 0) exit(1);}

     { ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close P_exp", ECPGt_EOIT, ECPGt_EORT);

 if (sqlca.sqlwarn[0] == 'W') sqlprint();

 if (sqlca.sqlcode < 0) sqlprint();
 if (sqlca.sqlcode < 0) exit(1);}

     { ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close ref_exp", ECPGt_EOIT, ECPGt_EORT);

 if (sqlca.sqlwarn[0] == 'W') sqlprint();

 if (sqlca.sqlcode < 0) sqlprint();
 if (sqlca.sqlcode < 0) exit(1);}


     // disconnecting form the database //
    { ECPGdisconnect(__LINE__, "ALL");

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

    fout << "7. in measdata_modified.cpp line 598. Finised reading experimental data form the database. " << endl;

    fout.close();

    // Unique TP file
    const char path2[200] = "output_GEMSFIT/TP.csv";
//    ofstream fout;
    fout.open(path2, ios::app);
    if( fout.fail() )
    { cout<<"Output fileopen error"<<endl; exit(1); }

    for (i=0; i<TP_pairs[1].size(); ++i)
    {
        fout <<TP_pairs[1][i]<<";"<<TP_pairs[0][i]<<endl;
    }

    fout << TP_pairs[1].size() <<endl;
    fout.close();
}

// ----------------------------------------------- END SS_Data_Manager get data function CLASS -----------------------------------------------//

void Data_Manager::get_CSV( measdata* sysdata )
{
	int i, j, pos, cols, rows;
	string line, alldata, headline;
 	ifstream data_stream;
	vector<string> data;

	// GEMSFIT logfile
	const char path[200] = "output_GEMSFIT/GEMSFIT.log";
	ofstream fout;
	fout.open(path, ios::app);						
	if( fout.fail() )
	{ cout<<"Output fileopen error"<<endl; exit(1); }



	// datatypes that should be stored in measurement data file
	vector<string> columns;
	columns.push_back("ende");
	columns.push_back("temperature");
	columns.push_back("pressure");
	columns.push_back("molality_1");
	columns.push_back("molality_2");
	columns.push_back("code");
	columns.push_back("val");
	columns.push_back("other_1");
	columns.push_back("other_2");
	columns.push_back("other_3");
	columns.push_back("other_4");
	columns.push_back("error");
	columns.push_back("species_1_1");
	columns.push_back("species_1_2");
	columns.push_back("species_1_3");
	columns.push_back("species_1_4");
	columns.push_back("species_2_1");
	columns.push_back("species_2_2");
	columns.push_back("species_2_3");
	columns.push_back("species_2_4");
	columns.push_back("stoic_1_1");
	columns.push_back("stoic_1_2");	
	columns.push_back("stoic_1_3");	
	columns.push_back("stoic_1_4");	
	columns.push_back("stoic_2_1");	
	columns.push_back("stoic_2_2");
	columns.push_back("stoic_2_3");	
	columns.push_back("stoic_2_4");	
	columns.push_back("id");
	
	// Find column number which corresponds to each datatype
	// Read parameter file into string 
	data_stream.open( CSVfile.c_str() );	
	if( data_stream.fail() )
	{
		cout << "Opening of file "<<CSVfile<<" failed !!"<<endl;
		exit(1);
    }
	getline(data_stream, headline);
	replace_all( headline, "\"", "");
	replace_all( headline, ";", " ");

	string buf;                  // buffer string
	stringstream ss( headline ); // Insert the string into a stream

	vector<string> tokens;       // Create vector to hold strings

	while (ss >> buf)
		tokens.push_back(buf);

	cols = (int) tokens.size();


	// read data
	data.clear();
	i=0;
	while( getline(data_stream, line, ';') ) 
	{
		data.push_back(line);
		fout << data.at(i)<<endl;
		i++; 
	} 
	data_stream.close();
	
	cols =  ( cols - 1 );

	try
	{
		if( cols < 3 )
			throw 1;

		rows = data.size() / cols;
		
		if( ! (data.size() % cols) )
			throw 2;

	}
	catch ( int excp )	
	{

		fout << endl;
		cout << " Something wrong with reading the CSV file ! " << endl;
		switch( excp )
		{
			case 1:
			{
				cout << " At least three columns are needed as measurement input (temperature, pressure and molality_1 ) " << endl;
				cout << " You entered only "<<cols<<" columns . " << endl;
				cout << " Sorry can't proceed, bailing out now ... " << endl;
				cout << endl;
				exit(1);
			}
			case 2:
			{
				cout << " The number of data elements from the measurement file does not match the column number, " << endl;
				cout << " data.size() = " << data.size() << endl;
				cout << " rows = " << data.size() / cols << endl;		
				cout << " Something went wrong here ... " << endl;
				cout << " Sorry can't proceed, bailing out now ... " << endl;
				cout << endl;				
				exit(1);
			}
		}
	}

	// loop over columns that ought to be present in measurement file
    for( j=0; j<columns.size(); j++ )
	{
		// loop over measurement data cols
		for( i=0; i<cols; i++ )
		{
			// find position of datatype in measurement file
			if( ! columns.at(j).compare( tokens.at(i) ) )
			{	
				pos = i;
			}
		}
		for( i=0; i<rows; i++ )
		{
			switch( pos )
			{
/*	order and datatype of measurement data:
            int_v 		ende;+ The program has unexpectedly finished.
			double_v 	temperature;
			double_v 	pressure;
			double_v 	molality_1;
			double_v 	molality_2;
			int_v 		code;
			double_v 	val;
			double_v 	other_1;
			double_v 	other_2;
			double_v 	other_3;
			double_v 	other_4;
			double_v 	error;
			string_v 	species_1_1;
			string_v 	species_1_2;
			string_v 	species_1_3;
			string_v 	species_1_4;
			string_v 	species_2_1;
			string_v 	species_2_2;
			string_v 	species_2_3;
			string_v 	species_2_4;
			int_v 		stoic_1_1;	
			int_v 		stoic_1_2;	
			int_v 		stoic_1_3;	
			int_v 		stoic_1_4;	
			int_v 		stoic_2_1;	
			int_v 		stoic_2_2;	
			int_v 		stoic_2_3;	
			int_v 		stoic_2_4;	
			string_v 	id;
*/
				case 0:
					sysdata->ende.push_back(  atoi(data.at(pos + i * cols ).c_str()) );
					break;
				case 1:
					sysdata->temperature.push_back( atof(data.at( pos + i * cols ).c_str()) );
					break;
				case 2:
					sysdata->pressure.push_back( atof(data.at( pos + i * cols ).c_str()) );
					break;
				case 3:
					sysdata->molality_1.push_back( atof(data.at( pos + i * cols ).c_str()) );
					break;
				case 4:
					sysdata->molality_2.push_back( atof(data.at( pos + i * cols ).c_str()) );
					break;
				case 5:
					sysdata->code.push_back( atoi(data.at( pos + i * cols ).c_str()) );
					break;
				case 6:
					sysdata->val.push_back( atof(data.at( pos + i * cols ).c_str()) );
					break;
				case 7:
					sysdata->other_1.push_back( atof(data.at( pos + i * cols ).c_str()) );
					break;
				case 8:
					sysdata->other_2.push_back( atof(data.at( pos + i * cols ).c_str()) );
					break;
				case 9:
					sysdata->other_3.push_back( atof(data.at( pos + i * cols ).c_str()) );
					break;
				case 10:
					sysdata->other_4.push_back( atof(data.at( pos + i * cols ).c_str()) );
					break;
				case 11:
					sysdata->error.push_back( atof(data.at( pos + i * cols ).c_str()) );
					break;
				case 12:
					sysdata->species_1_1.push_back( data.at( pos + i * cols ) );
					break;
				case 13:
					sysdata->species_1_2.push_back( data.at( pos + i * cols ) );
                    break;
				case 14:
					sysdata->species_1_3.push_back( data.at( pos + i * cols ) );
					break;
				case 15:
					sysdata->species_1_4.push_back( data.at( pos + i * cols ) );
					break;
				case 16:
					sysdata->species_2_1.push_back( data.at( pos + i * cols ) );
					break;
				case 17:
					sysdata->species_2_2.push_back( data.at( pos + i * cols ) );
					break;
				case 18:
					sysdata->species_2_3.push_back( data.at( pos + i * cols ) );
					break;
				case 19:
					sysdata->species_2_4.push_back( data.at( pos + i * cols ) );
					break;
				case 20:
					sysdata->stoic_1_1.push_back( atoi(data.at( pos + i * cols ).c_str()) );
					break;
				case 21:
					sysdata->stoic_1_2.push_back( atoi(data.at( pos + i * cols ).c_str()) );
					break;
				case 22:
					sysdata->stoic_1_3.push_back( atoi(data.at( pos + i * cols ).c_str()) );
                    break;
				case 23:
					sysdata->stoic_1_4.push_back( atoi(data.at( pos + i * cols ).c_str()) );
					break;
				case 24:
					sysdata->stoic_2_1.push_back( atoi(data.at( pos + i * cols ).c_str()) );
					break;
				case 25:
					sysdata->stoic_2_2.push_back( atoi(data.at( pos + i * cols ).c_str()) );
					break;
				case 26:
					sysdata->stoic_2_3.push_back( atoi(data.at( pos + i * cols ).c_str()) );
					break;
				case 27:
					sysdata->stoic_2_4.push_back( atoi(data.at( pos + i * cols ).c_str()) );
					break;
				case 28:
					sysdata->id.push_back( data.at( pos + i * cols ) );
					break;
			} 
		}
	}		


	// Remove apostrophes from species strings
	for( int i=0; i< (int) sysdata->species_1_1.size(); i++ )
	{
		erase_all(sysdata->species_1_1.at(i), "'");
		erase_all(sysdata->species_1_2.at(i), "'");
		erase_all(sysdata->species_1_3.at(i), "'");
		erase_all(sysdata->species_1_4.at(i), "'");
		erase_all(sysdata->species_1_1.at(i), "\"");
		erase_all(sysdata->species_1_2.at(i), "\"");
		erase_all(sysdata->species_1_3.at(i), "\"");
		erase_all(sysdata->species_1_4.at(i), "\"");
	}


#ifdef GEMSFIT_DEBUG
	for( i=0; i<sysdata->temperature.size(); i++ )
	{
		cout << sysdata->temperature.at(i) << endl;
		cout << sysdata->species_1_1.at(i) << endl;	
	}
#endif

	// check if all read-in vectors have the same length
	if( sysdata->temperature.size() != sysdata->pressure.size() || sysdata->temperature.size() != sysdata->molality_1.size() || sysdata->species_1_1.size() != sysdata->temperature.size() || sysdata->temperature.size() != sysdata->species_1_2.size() || sysdata->temperature.size() != sysdata->stoic_1_1.size() || sysdata->temperature.size() != sysdata->stoic_1_2.size() )
	{	 
		cout << endl;
		cout << " Length of read-in vectors of measurement data do not match !!! " << endl;
		cout << " Sorry can't proceed, bailing out now ... " << endl;
		cout << endl;
		exit(1);
	}


	fout.close();
}



//void System_Properties::get_DB( measdata* sysdata, string tablename )
void Data_Manager::get_DB( measdata* sysdata )
{

	// GEMSFIT logfile
	const char path[200] = "output_GEMSFIT/GEMSFIT.log";
	ofstream fout;
	fout.open(path, ios::app);						
	if( fout.fail() )
	{ cout<<"Output fileopen error"<<endl; exit(1); }


	 int i=0;
	 char dbname [ 256 ] ;	
	 int rows ;	 
	 char id_ [ 25 ] ;	 	    int id_i ;	 
	 double temperature_ ;	    int temperature_i ;	 
	 double pressure_ ;	        int pressure_i ;	 
	 double molality_1_ ;	    int molality_1_i ;	 
	 double molality_2_ ;	    int molality_2_i ;	 
	 int code_ ;	 		    int code_i ;	 
	 double val_ ;	 		    int val_i ;	 
	 double other_1_ ;	  	    int other_1_i ;	 
	 double other_2_ ;	  	    int other_2_i ;	 
	 double other_3_ ;	 	    int other_3_i ;	 
	 double other_4_ ;	  	    int other_4_i ;	 
	 double error_ ;	 	    int error_i ;	 
	 char species_1_1_ [ 25 ] ;	int species_1_1_i ;	 
	 char species_1_2_ [ 25 ] ;	int species_1_2_i ;	 
	 char species_1_3_ [ 25 ] ;	int species_1_3_i ;	 
	 char species_1_4_ [ 25 ] ;	int species_1_4_i ;	 
	 char species_2_1_ [ 25 ] ;	int species_2_1_i ;	 
	 char species_2_2_ [ 25 ] ;	int species_2_2_i ;	 
	 char species_2_3_ [ 25 ] ;	int species_2_3_i ;	 
	 char species_2_4_ [ 25 ] ;	int species_2_4_i ;	 
	 int stoic_1_1_ ;	 		int stoic_1_1_i ;	 
	 int stoic_1_2_ ;	 		int stoic_1_2_i ;	 
	 int stoic_1_3_ ;	 		int stoic_1_3_i ;	 
	 int stoic_1_4_ ;	 		int stoic_1_4_i ;	 
	 int stoic_2_1_ ;	 		int stoic_2_1_i ;	 
	 int stoic_2_2_ ;	 		int stoic_2_2_i ;	 
	 int stoic_2_3_ ;	 		int stoic_2_3_i ;	 
	 int stoic_2_4_ ;	 		int stoic_2_4_i ;	 
	 int ende_ ;	 			int ende_i ; 
     char meas1[100] = "SELECT COUNT ( * ) ende FROM "; const char* meas_table;
	 char id_ecpg_1[256] = "declare f_id cursor for select id from "; const char * id_ecpg;	
	 char temperature_ecpg_1[256] = "declare f_temperature cursor for select temperature from "; const char * temperature_ecpg;	
	 char pressure_ecpg_1[256] = "declare f_pressure cursor for select pressure from "; const char * pressure_ecpg;	
	 char molality_1_ecpg_1[256] = "declare f_molality_1 cursor for select molality_1 from "; const char * molality_1_ecpg;	
	 char molality_2_ecpg_1[256] = "declare f_molality_2 cursor for select molality_2 from "; const char * molality_2_ecpg;	
	 char code_ecpg_1[256] = "declare f_code cursor for select code from "; const char * code_ecpg;	
	 char val_ecpg_1[256] = "declare f_val cursor for select val from "; const char * val_ecpg;	
	 char other_1_ecpg_1[256] = "declare f_other_1 cursor for select other_1 from "; const char * other_1_ecpg;	
	 char other_2_ecpg_1[256] = "declare f_other_2 cursor for select other_2 from "; const char * other_2_ecpg;	
	 char other_3_ecpg_1[256] = "declare f_other_3 cursor for select other_3 from "; const char * other_3_ecpg;	
	 char other_4_ecpg_1[256] = "declare f_other_4 cursor for select other_4 from "; const char * other_4_ecpg;	
	 char error_ecpg_1[256] = "declare f_error cursor for select error from "; const char * error_ecpg;	
	 char species_1_1_ecpg_1[256] = "declare f_species_1_1 cursor for select species_1_1 from "; const char * species_1_1_ecpg;	
	 char species_1_2_ecpg_1[256] = "declare f_species_1_2 cursor for select species_1_2 from "; const char * species_1_2_ecpg;	
	 char species_1_3_ecpg_1[256] = "declare f_species_1_3 cursor for select species_1_3 from "; const char * species_1_3_ecpg;	
	 char species_1_4_ecpg_1[256] = "declare f_species_1_4 cursor for select species_1_4 from "; const char * species_1_4_ecpg;	
	 char species_2_1_ecpg_1[256] = "declare f_species_2_1 cursor for select species_2_1 from "; const char * species_2_1_ecpg;	
	 char species_2_2_ecpg_1[256] = "declare f_species_2_2 cursor for select species_2_2 from "; const char * species_2_2_ecpg;	
	 char species_2_3_ecpg_1[256] = "declare f_species_2_3 cursor for select species_2_3 from "; const char * species_2_3_ecpg;	
	 char species_2_4_ecpg_1[256] = "declare f_species_2_4 cursor for select species_2_4 from "; const char * species_2_4_ecpg;	
	 char stoic_1_1_ecpg_1[256] = "declare f_stoic_1_1 cursor for select stoic_1_1 from "; const char * stoic_1_1_ecpg;	
	 char stoic_1_2_ecpg_1[256] = "declare f_stoic_1_2 cursor for select stoic_1_2 from "; const char * stoic_1_2_ecpg;	
	 char stoic_1_3_ecpg_1[256] = "declare f_stoic_1_3 cursor for select stoic_1_3 from "; const char * stoic_1_3_ecpg;	
	 char stoic_1_4_ecpg_1[256] = "declare f_stoic_1_4 cursor for select stoic_1_4 from "; const char * stoic_1_4_ecpg;	
	 char stoic_2_1_ecpg_1[256] = "declare f_stoic_2_1 cursor for select stoic_2_1 from "; const char * stoic_2_1_ecpg;	
	 char stoic_2_2_ecpg_1[256] = "declare f_stoic_2_2 cursor for select stoic_2_2 from "; const char * stoic_2_2_ecpg;	
	 char stoic_2_3_ecpg_1[256] = "declare f_stoic_2_3 cursor for select stoic_2_3 from "; const char * stoic_2_3_ecpg;	
	 char stoic_2_4_ecpg_1[256] = "declare f_stoic_2_4 cursor for select stoic_2_4 from "; const char * stoic_2_4_ecpg;	
	 char ende_ecpg_1[256] = "declare f_ende cursor for select ende from "; const char * ende_ecpg;	
/* exec sql end declare section */
	
  	strcat(meas1, tablename.c_str());
	
	meas_table = meas1;	

	// // // // // // // Connect to database // // // // // // // 
	{  
        string tcp_psql("tcp:postgresql://");
        string port(":5432/"); 
        string server_share;
        server_share = tcp_psql + psql_server + port + DBname;
		ECPGconnect(__LINE__, 0, server_share.c_str() , username.c_str() , passwd.c_str() , "con1", 0); 
		if (sqlca.sqlwarn[0] == 'W') 
		{
			sqlprint();
			printf("error code %d, message %s, rows %d, warning %c\n", 
			sqlca.sqlcode,sqlca.sqlerrm.sqlerrmc, sqlca.sqlerrd[2], 
			sqlca.sqlwarn[0]);
		}
		if (sqlca.sqlcode < 0)
		{ 
			sqlprint(); 
			printf("You entered following specifications for accessing the PostgreSQL database: \n username %s, \n database %s. \n", 
			username.c_str(), DBname.c_str() );

			printf("error code %d, message %s, rows %d, warning %c\n", 
			sqlca.sqlcode,sqlca.sqlerrm.sqlerrmc, sqlca.sqlerrd[2], 
			sqlca.sqlwarn[0]);
			exit(1);
		}
		if (sqlca.sqlwarn[0] == 'W') sqlprint();
		if (sqlca.sqlcode < 0) sqlprint();
		if (sqlca.sqlcode < 0) exit(1);		
	}


	fout << "succesfully established connection to postgresql server ... " << endl;


	// Count number of rows 
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, meas_table, ECPGt_EOIT, 
	ECPGt_int,&(rows),(long)1,(long)1,sizeof(int), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EORT);

if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();
if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}
	fout<<"rows = "<<rows<<endl;
	

	 // Initialize vectors
	char init[25]="NULL\0";
	for( i=0;i<rows;i++ )
	{ 
		sysdata->ende.push_back(0);
		sysdata->id.push_back(init);
		sysdata->temperature.push_back(0.0);
		sysdata->pressure.push_back(0.0);
		sysdata->molality_1.push_back(0.0);
		sysdata->molality_2.push_back(0.0);
		sysdata->code.push_back(0);
		sysdata->val.push_back(0.0);
		sysdata->other_1.push_back(0.0);
		sysdata->other_2.push_back(0.0);
		sysdata->other_3.push_back(0.0);
		sysdata->other_4.push_back(0.0);
		sysdata->error.push_back(0.0);
		sysdata->species_1_1.push_back(init);
		sysdata->species_1_2.push_back(init);
		sysdata->species_1_3.push_back(init);
		sysdata->species_1_4.push_back(init);
		sysdata->species_2_1.push_back(init);
		sysdata->species_2_2.push_back(init);
		sysdata->species_2_3.push_back(init);
		sysdata->species_2_4.push_back(init);
		sysdata->stoic_1_1.push_back(0);	
		sysdata->stoic_1_2.push_back(0);	
		sysdata->stoic_1_3.push_back(0);	
		sysdata->stoic_1_4.push_back(0);	
		sysdata->stoic_2_1.push_back(0);	
		sysdata->stoic_2_2.push_back(0);	
		sysdata->stoic_2_3.push_back(0);	
		sysdata->stoic_2_4.push_back(0);	
	}

	// Check for current database name
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "select current_database ( )", ECPGt_EOIT, 
	ECPGt_char,(dbname),(long)256,(long)1,(256)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EORT);

if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

//	fout<<"current database: "<<dbname<<endl;

	
	// Declare cursor functions for retrieving all values from the meas table
	/* declare f_id cursor for select id from db_meas_table */	
	strcat(id_ecpg_1, tablename.c_str());
	id_ecpg = id_ecpg_1; 
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, id_ecpg, ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}


	/* declare f_temperature cursor for select temperature from db_meas_table */
	strcat(temperature_ecpg_1, tablename.c_str());
	temperature_ecpg = temperature_ecpg_1; 
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, temperature_ecpg, ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}


	/* declare f_pressure cursor for select pressure from db_meas_table */
	strcat(pressure_ecpg_1, tablename.c_str());
	pressure_ecpg = pressure_ecpg_1;  
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, pressure_ecpg, ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}


	/* declare f_molality_1 cursor for select molality_1 from db_meas_table */
	strcat(molality_1_ecpg_1, tablename.c_str());
	molality_1_ecpg = molality_1_ecpg_1;   
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, molality_1_ecpg, ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}


	/* declare f_molality_2 cursor for select molality_2 from db_meas_table */
	strcat(molality_2_ecpg_1, tablename.c_str());
	molality_2_ecpg = molality_2_ecpg_1;    
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, molality_2_ecpg, ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}


	/* declare f_code cursor for select code from db_meas_table */
	strcat(code_ecpg_1, tablename.c_str());
	code_ecpg = code_ecpg_1;     
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, code_ecpg, ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}


	/* declare f_val cursor for select val from db_meas_table */
	strcat(val_ecpg_1, tablename.c_str());
	val_ecpg = val_ecpg_1;      
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, val_ecpg, ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}


	/* declare f_other_1 cursor for select other_1 from db_meas_table */
	strcat(other_1_ecpg_1, tablename.c_str());
	other_1_ecpg = other_1_ecpg_1;       
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, other_1_ecpg, ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}


	/* declare f_other_2 cursor for select other_2 from db_meas_table */
	strcat(other_2_ecpg_1, tablename.c_str());
	other_2_ecpg = other_2_ecpg_1;       
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, other_2_ecpg, ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}


	/* declare f_other_3 cursor for select other_3 from db_meas_table */
 	strcat(other_3_ecpg_1, tablename.c_str());
	other_3_ecpg = other_3_ecpg_1;       
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, other_3_ecpg, ECPGt_EOIT, ECPGt_EORT);


if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}


	/* declare f_other_4 cursor for select other_4 from db_meas_table */
	strcat(other_4_ecpg_1, tablename.c_str());
	other_4_ecpg = other_4_ecpg_1;       
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, other_4_ecpg, ECPGt_EOIT, ECPGt_EORT);


if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}


	/* declare f_error cursor for select error from db_meas_table */
	strcat(error_ecpg_1, tablename.c_str());
	error_ecpg = error_ecpg_1;        
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, error_ecpg, ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}


	/* declare f_species_1_1 cursor for select species_1_1 from db_meas_table */
	strcat(species_1_1_ecpg_1, tablename.c_str());
	species_1_1_ecpg = species_1_1_ecpg_1;         
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, species_1_1_ecpg, ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}


	/* declare f_species_1_2 cursor for select species_1_2 from db_meas_table */
	strcat(species_1_2_ecpg_1, tablename.c_str());
	species_1_2_ecpg = species_1_2_ecpg_1;         
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, species_1_2_ecpg, ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}


	/* declare f_species_1_3 cursor for select species_1_3 from db_meas_table */
	strcat(species_1_3_ecpg_1, tablename.c_str());
	species_1_3_ecpg = species_1_3_ecpg_1;         
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, species_1_3_ecpg, ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}


	/* declare f_species_1_4 cursor for select species_1_4 from db_meas_table */
	strcat(species_1_4_ecpg_1, tablename.c_str());
	species_1_4_ecpg = species_1_4_ecpg_1;         
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, species_1_4_ecpg, ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}


	/* declare f_species_2_1 cursor for select species_2_1 from db_meas_table */
	strcat(species_2_1_ecpg_1, tablename.c_str());
	species_2_1_ecpg = species_2_1_ecpg_1;         
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, species_2_1_ecpg, ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}


	/* declare f_species_2_2 cursor for select species_2_2 from db_meas_table */
	strcat(species_2_2_ecpg_1, tablename.c_str());
	species_2_2_ecpg = species_2_2_ecpg_1;         
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, species_2_2_ecpg, ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}


	/* declare f_species_2_3 cursor for select species_2_3 from db_meas_table */
	strcat(species_2_3_ecpg_1, tablename.c_str());
	species_2_3_ecpg = species_2_3_ecpg_1;         
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, species_2_3_ecpg, ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}


	/* declare f_species_2_4 cursor for select species_2_4 from db_meas_table */
	strcat(species_2_4_ecpg_1, tablename.c_str());
	species_2_4_ecpg = species_2_4_ecpg_1;         
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, species_2_4_ecpg, ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}


	/* declare f_stoic_1_1 cursor for select stoic_1_1 from db_meas_table */
	strcat(stoic_1_1_ecpg_1, tablename.c_str());
	stoic_1_1_ecpg = stoic_1_1_ecpg_1;         
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, stoic_1_1_ecpg, ECPGt_EOIT, ECPGt_EORT);
 
if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}


	/* declare f_stoic_1_2 cursor for select stoic_1_2 from db_meas_table */
 	strcat(stoic_1_2_ecpg_1, tablename.c_str());
	stoic_1_2_ecpg = stoic_1_2_ecpg_1;         
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, stoic_1_2_ecpg, ECPGt_EOIT, ECPGt_EORT);
 
if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}


	/* declare f_stoic_1_3 cursor for select stoic_1_3 from db_meas_table */
	strcat(stoic_1_3_ecpg_1, tablename.c_str());
	stoic_1_3_ecpg = stoic_1_3_ecpg_1;         
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, stoic_1_3_ecpg, ECPGt_EOIT, ECPGt_EORT);
 
if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}


	/* declare f_stoic_1_4 cursor for select stoic_1_4 from db_meas_table */
	strcat(stoic_1_4_ecpg_1, tablename.c_str());
	stoic_1_4_ecpg = stoic_1_4_ecpg_1;         
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, stoic_1_4_ecpg, ECPGt_EOIT, ECPGt_EORT);
 
if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}


	/* declare f_stoic_2_1 cursor for select stoic_2_1 from db_meas_table */
	strcat(stoic_2_1_ecpg_1, tablename.c_str());
	stoic_2_1_ecpg = stoic_2_1_ecpg_1;         
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, stoic_2_1_ecpg, ECPGt_EOIT, ECPGt_EORT);
 
if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}


	/* declare f_stoic_2_2 cursor for select stoic_2_2 from db_meas_table */
	strcat(stoic_2_2_ecpg_1, tablename.c_str());
	stoic_2_2_ecpg = stoic_2_2_ecpg_1;         
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, stoic_2_2_ecpg, ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}


	/* declare f_stoic_2_3 cursor for select stoic_2_3 from db_meas_table */
	strcat(stoic_2_3_ecpg_1, tablename.c_str());
	stoic_2_3_ecpg = stoic_2_3_ecpg_1;         
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, stoic_2_3_ecpg, ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}


	/* declare f_stoic_2_4 cursor for select stoic_2_4 from db_meas_table */
	strcat(stoic_2_4_ecpg_1, tablename.c_str());
	stoic_2_4_ecpg = stoic_2_4_ecpg_1;         
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, stoic_2_4_ecpg, ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}


	/* declare f_ende cursor for select ende from db_meas_table */
	strcat(ende_ecpg_1, tablename.c_str());
	ende_ecpg = ende_ecpg_1;         
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, ende_ecpg, ECPGt_EOIT, ECPGt_EORT);
 
if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}



	// Retrieve values form database into struct measdata
	for( i=0;i<rows;i++ ) 
	{
		{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch f_id", ECPGt_EOIT, 
	ECPGt_char,(id_),(long)25,(long)1,(25)*sizeof(char), 
	ECPGt_int,&(id_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

		//printf("current id: %s\n", id_);	
		if( id_i==0  ){	sysdata->id[i] = id_; };
		if( id_i>0  ){	sysdata->id[i] = id_; fout<<"id was truncated while storing in host variable !!!"<<endl; };
//		fout<<"sysdata->id["<<i<<"] = "<<sysdata->id[i]<<endl;

		{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch f_temperature", ECPGt_EOIT, 
	ECPGt_double,&(temperature_),(long)1,(long)1,sizeof(double), 
	ECPGt_int,&(temperature_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

		//printf("current temperature: %f\n", temperature_);	
		if( temperature_i==0 ){ sysdata->temperature[i] = temperature_; };
		if( temperature_i>0 ){ sysdata->temperature[i] = temperature_; fout<<"temperature was truncated while storing in host variable !!!"<<endl; };
//		fout<<"sysdata->temperature["<<i<<"] = "<<sysdata->temperature[i]<<endl;

		{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch f_pressure", ECPGt_EOIT, 
	ECPGt_double,&(pressure_),(long)1,(long)1,sizeof(double), 
	ECPGt_int,&(pressure_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

		//printf("current pressure: %f\n", pressure_);	
		if( pressure_i==0 ){ sysdata->pressure[i] = pressure_; };
		if( pressure_i>0 ){ sysdata->pressure[i] = pressure_; fout<<"pressure was truncated while storing in host variable !!!"<<endl; };
//		fout<<"sysdata->pressure["<<i<<"] = "<<sysdata->pressure[i]<<endl;

		{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch f_molality_1", ECPGt_EOIT, 
	ECPGt_double,&(molality_1_),(long)1,(long)1,sizeof(double), 
	ECPGt_int,&(molality_1_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

		//printf("current molality_1: %f\n", molality_1_);	
		if( molality_1_i==0 ){ sysdata->molality_1[i] = molality_1_; };	
		if( molality_1_i>0 ){ sysdata->molality_1[i] = molality_1_; fout<<"molality_1 was truncated while storing in host variable !!!"<<endl; };
//		fout<<"sysdata->molality_1["<<i<<"] = "<<sysdata->molality_1[i]<<endl;

		{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch f_molality_2", ECPGt_EOIT, 
	ECPGt_double,&(molality_2_),(long)1,(long)1,sizeof(double), 
	ECPGt_int,&(molality_2_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

		//printf("current molality_2: %f\n", molality_2_);	
		if( molality_2_i==0 ){ sysdata->molality_2[i] = molality_2_; };
		if( molality_2_i>0 ){ sysdata->molality_2[i] = molality_2_; fout<<"molality_2 was truncated while storing in host variable !!!"<<endl; };
//		fout<<"sysdata->molality_2["<<i<<"] = "<<sysdata->molality_2[i]<<endl;

		{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch f_code", ECPGt_EOIT, 
	ECPGt_int,&(code_),(long)1,(long)1,sizeof(int), 
	ECPGt_int,&(code_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

		//printf("current code: %i\n", code_);	
		if( code_i==0 ){ sysdata->code[i] = code_; };
		if( code_i>0 ){ sysdata->code[i] = code_; fout<<"code was truncated while storing in host variable !!!"<<endl; };
//		cout<<"sysdata->code["<<i<<"] = "<<sysdata->code[i]<<endl;

		{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch f_val", ECPGt_EOIT, 
	ECPGt_double,&(val_),(long)1,(long)1,sizeof(double), 
	ECPGt_int,&(val_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

		//printf("current val: %i\n", val_);	
		if( val_i==0 ){ sysdata->val[i] = val_; };
		if( val_i>0 ){ sysdata->val[i] = val_; fout<<"val was truncated while storing in host variable !!!"<<endl; };
//		fout<<"sysdata->val["<<i<<"] = "<<sysdata->val[i]<<endl;

		{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch f_other_1", ECPGt_EOIT, 
	ECPGt_double,&(other_1_),(long)1,(long)1,sizeof(double), 
	ECPGt_int,&(other_1_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

		//printf("current other_1: %d\n", other_1_);	
		if( other_1_i==0 ){ sysdata->other_1[i] = other_1_; };
		if( other_1_i>0 ){ sysdata->other_1[i] = other_1_; fout<<"other_1 was truncated while storing in host variable !!!"<<endl; };
//		fout<<"sysdata->other_1["<<i<<"] = "<<sysdata->other_1[i]<<endl;

		{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch f_other_2", ECPGt_EOIT, 
	ECPGt_double,&(other_2_),(long)1,(long)1,sizeof(double), 
	ECPGt_int,&(other_2_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

		//printf("current other_2: %d\n", other_2_);	
		if( other_2_i==0 ){ sysdata->other_2[i] = other_2_; };
		if( other_2_i>0 ){ sysdata->other_2[i] = other_2_; fout<<"other_2 was truncated while storing in host variable !!!"<<endl; };
//		fout<<"sysdata->other_2["<<i<<"] = "<<sysdata->other_2[i]<<endl;

		{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch f_other_3", ECPGt_EOIT, 
	ECPGt_double,&(other_3_),(long)1,(long)1,sizeof(double), 
	ECPGt_int,&(other_3_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

		//printf("current other_3: %d\n", other_3_);	
		if( other_3_i==0 ){ sysdata->other_3[i] = other_3_; };
		if( other_3_i>0 ){ sysdata->other_3[i] = other_3_; fout<<"other_3 was truncated while storing in host variable !!!"<<endl; };
//		fout<<"sysdata->other_3["<<i<<"] = "<<sysdata->other_3[i]<<endl;

		{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch f_other_4", ECPGt_EOIT, 
	ECPGt_double,&(other_4_),(long)1,(long)1,sizeof(double), 
	ECPGt_int,&(other_4_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

		//printf("current other_4: %d\n", other_4_);	
		if( other_4_i==0 ){ sysdata->other_4[i] = other_4_; };
		if( other_4_i>0 ){ sysdata->other_4[i] = other_4_; fout<<"other_4 was truncated while storing in host variable !!!"<<endl; };
//		fout<<"sysdata->other_4["<<i<<"] = "<<sysdata->other_4[i]<<endl;

		{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch f_error", ECPGt_EOIT, 
	ECPGt_double,&(error_),(long)1,(long)1,sizeof(double), 
	ECPGt_int,&(error_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

		//printf("current error: %d\n", error_);	
		if( error_i==0 ){ sysdata->error[i] = error_; };
		if( error_i>0 ){ sysdata->error[i] = error_; fout<<"error was truncated while storing in host variable !!!"<<endl; };
//		fout<<"sysdata->error["<<i<<"] = "<<sysdata->error[i]<<endl;

		{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch f_species_1_1", ECPGt_EOIT, 
	ECPGt_char,(species_1_1_),(long)25,(long)1,(25)*sizeof(char), 
	ECPGt_int,&(species_1_1_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

		//printf("current species_1_1: %s\n", species_1_1_);	
		if( species_1_1_i==0 ){ sysdata->species_1_1[i] = species_1_1_; };
		if( species_1_1_i>0 ){ sysdata->species_1_1[i] = species_1_1_; fout<<"species_1_1 was truncated while storing in host variable !!!"<<endl; };
//		fout<<"sysdata->species_1_1["<<i<<"] = "<<sysdata->species_1_1[i]<<endl;

		{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch f_species_1_2", ECPGt_EOIT, 
	ECPGt_char,(species_1_2_),(long)25,(long)1,(25)*sizeof(char), 
	ECPGt_int,&(species_1_2_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

		//printf("current species_1_2: %s\n", species_1_2_);	
		if( species_1_2_i==0 ){ sysdata->species_1_2[i] = species_1_2_; };
		if( species_1_2_i>0 ){ sysdata->species_1_2[i] = species_1_2_; fout<<"species_1_2 was truncated while storing in host variable !!!"<<endl; };
//		fout<<"sysdata->species_1_2["<<i<<"] = "<<sysdata->species_1_2[i]<<endl;

		{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch f_species_1_3", ECPGt_EOIT, 
	ECPGt_char,(species_1_3_),(long)25,(long)1,(25)*sizeof(char), 
	ECPGt_int,&(species_1_3_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

		//printf("current species_1_3: %s\n", species_1_3_);	
		if( species_1_3_i==0 ){ sysdata->species_1_3[i] = species_1_3_; };
		if( species_1_3_i>0 ){ sysdata->species_1_3[i] = species_1_3_; fout<<"species_1_3 was truncated while storing in host variable !!!"<<endl; };
//		fout<<"sysdata->species_1_3["<<i<<"] = "<<sysdata->species_1_3[i]<<endl;

		{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch f_species_1_4", ECPGt_EOIT, 
	ECPGt_char,(species_1_4_),(long)25,(long)1,(25)*sizeof(char), 
	ECPGt_int,&(species_1_4_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

		//printf("current species_1_4: %s\n", species_1_4_);	
		if( species_1_4_i==0 ){ sysdata->species_1_4[i] = species_1_4_; };
		if( species_1_4_i>0 ){ sysdata->species_1_4[i] = species_1_4_; fout<<"species_1_4 was truncated while storing in host variable !!!"<<endl; };
//		fout<<"sysdata->species_1_4["<<i<<"] = "<<sysdata->species_1_4[i]<<endl;

		{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch f_species_2_1", ECPGt_EOIT, 
	ECPGt_char,(species_2_1_),(long)25,(long)1,(25)*sizeof(char), 
	ECPGt_int,&(species_2_1_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

		//printf("current species_2_1: %s\n", species_2_1_);	
		if( species_2_1_i==0 ){ sysdata->species_2_1[i] = species_2_1_; };
		if( species_2_1_i>0 ){ sysdata->species_2_1[i] = species_2_1_; fout<<"species_2_1 was truncated while storing in host variable !!!"<<endl; };
//		fout<<"sysdata->species_2_1["<<i<<"] = "<<sysdata->species_2_1[i]<<endl;

		{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch f_species_2_2", ECPGt_EOIT, 
	ECPGt_char,(species_2_2_),(long)25,(long)1,(25)*sizeof(char), 
	ECPGt_int,&(species_2_2_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

		//printf("current species_2_2: %s\n", species_2_2_);	
		if( species_2_2_i==0 ){ sysdata->species_2_2[i] = species_2_2_; };
		if( species_2_2_i>0 ){ sysdata->species_2_2[i] = species_2_2_; fout<<"species_2_2 was truncated while storing in host variable !!!"<<endl; };
//		fout<<"sysdata->species_2_2["<<i<<"] = "<<sysdata->species_2_2[i]<<endl;

		{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch f_species_2_3", ECPGt_EOIT, 
	ECPGt_char,(species_2_3_),(long)25,(long)1,(25)*sizeof(char), 
	ECPGt_int,&(species_2_3_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

		//printf("current species_2_3: %s\n", species_2_3_);	
		if( species_2_3_i==0 ){ sysdata->species_2_3[i] = species_2_3_; };
		if( species_2_3_i>0 ){ sysdata->species_2_3[i] = species_2_3_; fout<<"species_2_3 was truncated while storing in host variable !!!"<<endl; };
//		fout<<"sysdata->species_2_3["<<i<<"] = "<<sysdata->species_2_3[i]<<endl;

		{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch f_species_2_4", ECPGt_EOIT, 
	ECPGt_char,(species_2_4_),(long)25,(long)1,(25)*sizeof(char), 
	ECPGt_int,&(species_2_4_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

		//printf("current species_2_4: %s\n", species_2_4_);	
		if( species_2_4_i==0 ){ sysdata->species_2_4[i] = species_2_4_; };
		if( species_2_4_i>0 ){ sysdata->species_2_4[i] = species_2_4_; fout<<"species_2_4 was truncated while storing in host variable !!!"<<endl; };
//		fout<<"sysdata->species_2_4["<<i<<"] = "<<sysdata->species_2_4[i]<<endl;

		{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch f_stoic_1_1", ECPGt_EOIT, 
	ECPGt_int,&(stoic_1_1_),(long)1,(long)1,sizeof(int), 
	ECPGt_int,&(stoic_1_1_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

		//printf("current stoic_1_1: %i\n", stoic_1_1_);	
		if( stoic_1_1_i==0 ){ sysdata->stoic_1_1[i] = stoic_1_1_; };
		if( stoic_1_1_i>0 ){ sysdata->stoic_1_1[i] = stoic_1_1_; fout<<"stoic_1_1 was truncated while storing in host variable !!!"<<endl; };
//		fout<<"sysdata->stoic_1_1["<<i<<"] = "<<sysdata->stoic_1_1[i]<<endl;

		{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch f_stoic_1_2", ECPGt_EOIT, 
	ECPGt_int,&(stoic_1_2_),(long)1,(long)1,sizeof(int), 
	ECPGt_int,&(stoic_1_2_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

		//printf("current stoic_1_2: %i\n", stoic_1_2_);	
		if( stoic_1_2_i==0 ){ sysdata->stoic_1_2[i] = stoic_1_2_; };
		if( stoic_1_2_i>0 ){ sysdata->stoic_1_2[i] = stoic_1_2_; fout<<"stoic_1_2 was truncated while storing in host variable !!!"<<endl; };
//		fout<<"sysdata->stoic_1_2["<<i<<"] = "<<sysdata->stoic_1_2[i]<<endl;

		{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch f_stoic_1_3", ECPGt_EOIT, 
	ECPGt_int,&(stoic_1_3_),(long)1,(long)1,sizeof(int), 
	ECPGt_int,&(stoic_1_3_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

		//printf("current stoic_1_3: %i\n", stoic_1_3_);	
		if( stoic_1_3_i==0 ){ sysdata->stoic_1_3[i] = stoic_1_3_; };
		if( stoic_1_3_i>0 ){ sysdata->stoic_1_3[i] = stoic_1_3_; fout<<"stoic_1_3 was truncated while storing in host variable !!!"<<endl; };
//		fout<<"sysdata->stoic_1_3["<<i<<"] = "<<sysdata->stoic_1_3[i]<<endl;

		{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch f_stoic_1_4", ECPGt_EOIT, 
	ECPGt_int,&(stoic_1_4_),(long)1,(long)1,sizeof(int), 
	ECPGt_int,&(stoic_1_4_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

		//printf("current stoic_1_4: %i\n", stoic_1_4_);	
		if( stoic_1_4_i==0 ){ sysdata->stoic_1_4[i] = stoic_1_4_; };
		if( stoic_1_4_i>0 ){ sysdata->stoic_1_4[i] = stoic_1_4_; fout<<"stoic_1_4 was truncated while storing in host variable !!!"<<endl; };
//		fout<<"sysdata->stoic_1_4["<<i<<"] = "<<sysdata->stoic_1_4[i]<<endl;

		{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch f_stoic_2_1", ECPGt_EOIT, 
	ECPGt_int,&(stoic_2_1_),(long)1,(long)1,sizeof(int), 
	ECPGt_int,&(stoic_2_1_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

		//printf("current stoic_2_1: %i\n", stoic_2_1_);	
		if( stoic_2_1_i==0 ){ sysdata->stoic_2_1[i] = stoic_2_1_; };
		if( stoic_2_1_i>0 ){ sysdata->stoic_2_1[i] = stoic_2_1_; fout<<"stoic_2_1 was truncated while storing in host variable !!!"<<endl; };
//		fout<<"sysdata->stoic_2_1["<<i<<"] = "<<sysdata->stoic_2_1[i]<<endl;

		{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch f_stoic_2_2", ECPGt_EOIT, 
	ECPGt_int,&(stoic_2_2_),(long)1,(long)1,sizeof(int), 
	ECPGt_int,&(stoic_2_2_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

		//printf("current stoic_2_2: %i\n", stoic_2_2_);	
		if( stoic_2_2_i==0 ){ sysdata->stoic_2_2[i] = stoic_2_2_; };
		if( stoic_2_2_i>0 ){ sysdata->stoic_2_2[i] = stoic_2_2_; fout<<"stoic_2_2 was truncated while storing in host variable !!!"<<endl; };
//		fout<<"sysdata->stoic_2_2["<<i<<"] = "<<sysdata->stoic_2_2[i]<<endl;

		{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch f_stoic_2_3", ECPGt_EOIT, 
	ECPGt_int,&(stoic_2_3_),(long)1,(long)1,sizeof(int), 
	ECPGt_int,&(stoic_2_3_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

		//printf("current stoic_2_3: %i\n", stoic_2_3_);	
		if( stoic_2_3_i==0 ){ sysdata->stoic_2_3[i] = stoic_2_3_; };
		if( stoic_2_3_i>0 ){ sysdata->stoic_2_3[i] = stoic_2_3_; fout<<"stoic_2_3 was truncated while storing in host variable !!!"<<endl; };
//		fout<<"sysdata->stoic_2_3["<<i<<"] = "<<sysdata->stoic_2_3[i]<<endl;

		{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch f_stoic_2_4", ECPGt_EOIT, 
	ECPGt_int,&(stoic_2_4_),(long)1,(long)1,sizeof(int), 
	ECPGt_int,&(stoic_2_4_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

		//printf("current stoic_2_4: %i\n", stoic_2_4_);	
		if( stoic_2_4_i==0 ){ sysdata->stoic_2_4[i] = stoic_2_4_; };
		if( stoic_2_4_i>0 ){ sysdata->stoic_2_4[i] = stoic_2_4_; fout<<"stoic_2_4 was truncated while storing in host variable !!!"<<endl; };
//		fout<<"sysdata->stoic_2_4["<<i<<"] = "<<sysdata->stoic_2_4[i]<<endl;

		{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "fetch f_ende", ECPGt_EOIT, 
	ECPGt_int,&(ende_),(long)1,(long)1,sizeof(int), 
	ECPGt_int,&(ende_i),(long)1,(long)1,sizeof(int), ECPGt_EORT);

if (sqlca.sqlcode == ECPG_NOT_FOUND) sqlprint();

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

		//printf("current intval: %i\n", ende_);	
		if( ende_i==0 ){ sysdata->ende[i] = ende_; };
		if( ende_i>0 ){ sysdata->ende[i] = ende_; fout<<"ende was truncated while storing in host variable !!!"<<endl; };
//		fout<<"sysdata->ende["<<i<<"] = "<<sysdata->ende[i]<<endl;

	}
	

	// Close cursors and database connection
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close f_id", ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close f_temperature", ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close f_pressure", ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close f_molality_1", ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close f_molality_2", ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close f_code", ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close f_val", ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close f_other_1", ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close f_other_2", ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close f_other_3", ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close f_other_4", ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close f_error", ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close f_species_1_1", ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close f_species_1_2", ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close f_species_1_3", ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close f_species_1_4", ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close f_species_2_1", ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close f_species_2_2", ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close f_species_2_3", ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close f_species_2_4", ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close f_stoic_1_1", ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}
	
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close f_stoic_1_2", ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}
	
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close f_stoic_1_3", ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}
	
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close f_stoic_1_4", ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}
	
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close f_stoic_2_1", ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}
	
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close f_stoic_2_2", ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}
	
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close f_stoic_2_3", ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}
	
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close f_stoic_2_4", ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}
	
	{ ECPGdo(__LINE__, 0, 0, NULL, 0, ECPGst_normal, "close f_ende", ECPGt_EOIT, ECPGt_EORT);

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}

	{ ECPGdisconnect(__LINE__, "ALL");

if (sqlca.sqlwarn[0] == 'W') sqlprint();

if (sqlca.sqlcode < 0) sqlprint();
if (sqlca.sqlcode < 0) exit(1);}


// Remove apostrophes from species 
	for( int i=0; i< (int) sysdata->species_1_1.size(); i++ )
	{
		erase_all(sysdata->species_1_1.at(i), "'");
		erase_all(sysdata->species_1_2.at(i), "'");
		erase_all(sysdata->species_1_3.at(i), "'");
		erase_all(sysdata->species_1_4.at(i), "'");
	}


	fout.close();
}
