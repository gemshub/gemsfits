-- Table: impdata.expcsd
DROP TABLE impdata.expresss;

CREATE TABLE impdata.expresss
(
  ExpSetID varchar, -- name of experimental sample
  C_S_solid real, -- C/S ratio in solid part
  Si_CSH_mol real, Ca_CSH_mol real, Ca_CSH_error real,
  Al_CSH_mol real, Al_CSH_error real, 
  Na_CSH_mol real, Na_CSH_error real, 
  K_CSH_mol real, K_CSH_error real,
  CL_avg real, CL_avg_error real, 
  SH real, CH real, Afm real, HG real, Cal real,
  ende integer
)
WITH (
  OIDS=TRUE
);
ALTER TABLE impdata.expresss
  OWNER TO postgres;

COPY impdata.expresss ( 
ExpSetID, C_S_solid, Si_CSH_mol, Ca_CSH_mol, Ca_CSH_error,
Al_CSH_mol, Al_CSH_error, Na_CSH_mol, Na_CSH_error, K_CSH_mol, K_CSH_error,
CL_avg, CL_avg_error, SH, CH, Afm, HG, Cal, ende
 ) 
FROM '/home/parallels/DevGEMSFIT/devDanFit/gemsfit/Tests/CASH/CSH-gc65-results_ss.csv'
WITH DELIMITER ','
CSV HEADER;


