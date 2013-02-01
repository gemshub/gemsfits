-- Table: impdata.expcsd
DROP TABLE impdata.expcsd;

CREATE TABLE impdata.expcsd
(
  ExpSetID varchar, -- name of experimental sample
  P_bar real,
  T_C real,
  C_S_solid real, -- C/S ratio in solid part
  si_w_g_L real,
  H2O_L real, -- in litres
  SiO2 real, -- in moles
  CaOH2 real, -- in moles
  AlOH3 real, -- in moles
  NaOH real, -- in moles
  KOH real, -- in moles
  CO2 real, -- in moles
  ende integer
)
WITH (
  OIDS=TRUE
);
ALTER TABLE impdata.expcsd
  OWNER TO postgres;

COPY impdata.expcsd ( ExpSetID, P_bar, T_C, C_S_solid, si_w_g_L,
  H2O_L, SiO2, CaOH2, AlOH3, NaOH, KOH, CO2, ende ) 
FROM '/home/mirondanro/DevGEMSFIT/devDanFit/gemfit/Tests/CASH/CSH-gc65-exp_comp.csv'
WITH DELIMITER ','
CSV HEADER;


