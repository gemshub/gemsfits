How to prepare and import CASH solubility data into PostgreSQL database for use in GEMSFIT

1. The experimental data should be prepared (all samples for one experiment(al series)) as Excel spreadsheet having the same structure as the "CSH-gc65-data.xls" example file. This workbook consists of four spreadsheets, three of which must be exported in .csv format. Frome the "CSH-gc65-data-xls" example, the following files were exported: "CSH-gc65-exp_comp.csv", "CSH-gc65-expresaq.csv" and "CSH-gc65-expresss.csv". All four file names should begin from "CSH-<expsetname>" ("CSH-gc65" in this example).

2. The data must be imported using pgAdmin into the "solcash" database (most recent variant is provided in "solcash.backup" file; restore this file first, if necessary). To restore tables, modify provided SQL script files and execute them using "Execute query" dialog of pgAdmin. Namely, execute 
	impdata-expcsd.sql	to import	CSH-<expsetname>-exp_comp.csv 
	impdata-expresaq.sql	to import	CSH-<expsetname>-results_aq.csv   
	impdata-expresss.sql	to import	CSH-<expsetname>-results_ss.csv
The csv files will be imported, respectively, into work tables 
	impdata.expcsd
	impdata.expresaq
	impdata.expresss

3. After the data for the exeprimental dataset were successfully imported, the following SQL scripts need to be executed in order to distribute the data over the main tables in "solcash" database: 

- to distribute data from table impdata.expcsd into table experiments and exp_comp execute impdata_to_exp_comp.sql
- to distrubute data from table impdata.expresaq into table results_aq execute impdata_to_resultsaq.sql
- to distrubute data from table impdata.expresss into table results_ss execute impdata_to_resultsss.sql
(TBD if all queries should go into one sql file?)

4. The input GEMS3K files for starting GEMSFIT calculations (for now, CSH3T model only) are provided in the "GEMS3K-files" folder. 
   	 
