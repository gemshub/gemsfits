-- Table: impdata.expcsd
DROP TABLE impdata.expresss;

CREATE TABLE impdata.expresss
(
  ExpSetID varchar, -- name of experimental sample
  SampleID varchar,
  C_S_solid real, -- C/S ratio in solid part
  A_S_solid real,
  Si_CSH_mol real, Ca_CSH_mol real, Ca_CSH_error real,
  Al_CSH_mol real, Al_CSH_error real, 
  Na_CSH_mol real, Na_CSH_error real, 
  K_CSH_mol real, K_CSH_error real,
  SH real, CH real,
  AfmCO3_mol real,
  C3AH6mol real,	
  CalVat_mol real,	
  Stratlingite real,
  ende integer
)
WITH (
  OIDS=TRUE
);
ALTER TABLE impdata.expresss
  OWNER TO postgres;

COPY impdata.expresss ( 
ExpSetID, SampleID, C_S_solid, A_S_solid, Si_CSH_mol, Ca_CSH_mol, Ca_CSH_error,
Al_CSH_mol, Al_CSH_error, Na_CSH_mol, Na_CSH_error, K_CSH_mol, K_CSH_error,
SH, CH, AfmCO3_mol, C3AH6mol, CalVat_mol, Stratlingite, ende ) 
FROM '/home/mirondanro/DevGEMSFIT/devDanFit/gemfit/Tests/CASH/CASH-data-LHopital/CASH-EL01-results_ss.csv'
WITH DELIMITER ','
CSV HEADER;


