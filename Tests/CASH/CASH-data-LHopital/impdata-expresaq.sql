-- Table: impdata.expcsd
DROP TABLE impdata.expresaq;

CREATE TABLE impdata.expresaq
(
  ExpSetID varchar, -- name of experimental sample
  SampleID varchar,
  C_S_solid real, -- C/S ratio in solid part
  A_S_solid real,
  pH_aq real, pH_error real, 
  Si_aq_mM real, lgSi_aq real, lgSi_error real, 
  Ca_aq_mM real, lgCa_aq real, lgCa_error real, 
  Al_aq_mM real, lgAl_aq real, lgAl_error real,
  Na_aq_mM real, lgNa_aq real, lgNa_error real, 
  K_aq_mM real,  lgK_aq real,  lgK_error real,
  C_aq_mM real,  lgC_aq real,  lgC_error real,
  ende integer
)
WITH (
  OIDS=TRUE
);
ALTER TABLE impdata.expresaq
  OWNER TO postgres;

COPY impdata.expresaq ( 
ExpSetID, SampleID, C_S_solid, A_S_solid, pH_aq, pH_error, Si_aq_mM, lgSi_aq, lgSi_error, 
Ca_aq_mM, lgCa_aq, lgCa_error, Al_aq_mM, lgAl_aq, lgAl_error,
Na_aq_mM, lgNa_aq, lgNa_error, K_aq_mM, lgK_aq, lgK_error,
C_aq_mM, lgC_aq, lgC_error, ende
 ) 
FROM '/home/parallels/DevGEMSFIT/devDanFit/gemsfit/Tests/CASH/CASH-data-LHopital/CASH-EL01-results_aq.csv'
WITH DELIMITER ','
CSV HEADER;


