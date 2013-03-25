-- Table: impdata.expcsd
DROP TABLE impdata.expcsd;

CREATE TABLE impdata.expcsd
(
  ExpSetID varchar, -- name of experimental sample
  SampleID varchar, -- name of runs
  P_bar real,
  T_C real,
  C_S_solid real, -- C/S ratio in solid part
  A_S_solid real, -- A/S ratio
  si_w_g_kg real,
  H2O_kg real, -- in kg
  SiO2 real, -- in moles
  CaOH2 real, -- in moles
  CaO real, -- in moles
  AlOH3 real, -- in moles
  CaO_Al2O3 real, -- in moles
  NaOH real, -- in moles
  KOH real, -- in moles
  CO2 real, -- in moles
  N2 real, -- moles
  ende integer
)
WITH (
  OIDS=TRUE
);
ALTER TABLE impdata.expcsd
  OWNER TO postgres;

COPY impdata.expcsd ( ExpSetID, SampleID, P_bar, T_C, C_S_solid, A_S_solid, si_w_g_kg,
  H2O_kg, SiO2, CaOH2, CaO, AlOH3, CaO_Al2O3, NaOH, KOH, CO2, N2, ende ) 
FROM '/home/parallels/DevGEMSFIT/devDanFit/gemsfit/Tests/CASH/CASH-data-LHopital/CASH-EL01-exp_comp.csv'
WITH DELIMITER ','
CSV HEADER;


