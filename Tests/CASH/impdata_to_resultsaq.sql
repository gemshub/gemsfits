-- fills in the results_aq table
DROP TABLE temp1;
CREATE TABLE temp1
(
  ID serial,
  id_exp integer, 
  id_elem integer,
  solubility real,
  log_solubility real,
  error real,
  log_error real
)
WITH (
  OIDS=TRUE
);
ALTER TABLE temp1
  OWNER TO postgres;

DROP TABLE temp2;
CREATE TABLE temp2
(
  ID serial,
  solubility real
)
WITH (
  OIDS=TRUE
);
ALTER TABLE temp2
  OWNER TO postgres;

DROP TABLE temp3;
CREATE TABLE temp3
(
  ID serial,
  log_solubility real
)
WITH (
  OIDS=TRUE
);
ALTER TABLE temp3
  OWNER TO postgres;

DROP TABLE temp4;
CREATE TABLE temp4
(
  ID serial,
  error real
)
WITH (
  OIDS=TRUE
);
ALTER TABLE temp4
  OWNER TO postgres;

DROP TABLE temp5;
CREATE TABLE temp5
(
  ID serial,
  log_error real
)
WITH (
  OIDS=TRUE
);
ALTER TABLE temp3
  OWNER TO postgres;

--DELETE FROM public.results_aq;
  
 -- pH

DELETE FROM temp1;
INSERT INTO temp1 (id_exp) 
SELECT  experiment_id FROM public.experiments WHERE experiment_id > ((SELECT count(*) FROM public.experiments) - (SELECT count(*) FROM impdata.expcsd)) ORDER BY experiment_id;

DELETE FROM temp2;
INSERT INTO temp2 (solubility) 
SELECT  ph_aq FROM impdata.expresaq;

UPDATE temp1 SET solubility = temp2.solubility FROM temp2 WHERE temp1.id=temp2.id;
UPDATE temp1 SET id_elem = (SELECT element_id from public.elements e WHERE e.name = 'pH');

DELETE FROM temp3;
INSERT INTO temp3 (log_solubility)
SELECT  ph_aq FROM impdata.expresaq;
UPDATE temp1 SET log_solubility = temp3.log_solubility FROM temp3 WHERE temp1.id=temp3.id;

DELETE FROM temp4;
INSERT INTO temp4 (error) 
SELECT  ph_error FROM impdata.expresaq;
UPDATE temp1 SET error = temp4.error FROM temp4 WHERE temp1.id=temp4.id;

DELETE FROM temp5;
INSERT INTO temp5 (log_error)
SELECT  ph_error FROM impdata.expresaq;
UPDATE temp1 SET log_error = temp5.log_error FROM temp5 WHERE temp1.id=temp5.id;

SELECT * FROM temp1;
INSERT INTO public.results_aq (id_exp, id_elem, solubility, log_solubility, error, log_error) 
SELECT id_exp, id_elem, solubility, log_solubility, error, log_error FROM temp1;


 -- Si

DELETE FROM temp1;
INSERT INTO temp1 (id_exp) 
SELECT  experiment_id FROM public.experiments WHERE experiment_id > ((SELECT count(*) FROM public.experiments) - (SELECT count(*) FROM impdata.expcsd)) ORDER BY experiment_id;

DELETE FROM temp2;
INSERT INTO temp2 (solubility) 
SELECT  si_aq_mm FROM impdata.expresaq;

UPDATE temp1 SET solubility = temp2.solubility FROM temp2 WHERE temp1.id=temp2.id;
UPDATE temp1 SET id_elem = (SELECT element_id from public.elements e WHERE e.name = 'Si');

DELETE FROM temp3;
INSERT INTO temp3 (log_solubility)
SELECT lgsi_aq FROM impdata.expresaq;
UPDATE temp1 SET log_solubility = temp3.log_solubility FROM temp3 WHERE temp1.id=temp3.id;

DELETE FROM temp4;
INSERT INTO temp4 (error) 
SELECT  ph_error FROM impdata.expresaq;
--UPDATE temp1 SET error = temp4.error FROM temp4 WHERE temp1.id=temp4.id;

DELETE FROM temp5;
INSERT INTO temp5 (log_error)
SELECT  lgsi_error FROM impdata.expresaq;
UPDATE temp1 SET log_error = temp5.log_error FROM temp5 WHERE temp1.id=temp5.id;

SELECT * FROM temp1;
INSERT INTO public.results_aq (id_exp, id_elem, solubility, log_solubility, error, log_error) 
SELECT id_exp, id_elem, solubility, log_solubility, error, log_error FROM temp1;

 -- Ca

DELETE FROM temp1;
INSERT INTO temp1 (id_exp) 
SELECT  experiment_id FROM public.experiments WHERE experiment_id > ((SELECT count(*) FROM public.experiments) - (SELECT count(*) FROM impdata.expcsd)) ORDER BY experiment_id;

DELETE FROM temp2;
INSERT INTO temp2 (solubility) 
SELECT  ca_aq_mm FROM impdata.expresaq;

UPDATE temp1 SET solubility = temp2.solubility FROM temp2 WHERE temp1.id=temp2.id;
UPDATE temp1 SET id_elem = (SELECT element_id from public.elements e WHERE e.name = 'Ca');

DELETE FROM temp3;
INSERT INTO temp3 (log_solubility)
SELECT lgca_aq FROM impdata.expresaq;
UPDATE temp1 SET log_solubility = temp3.log_solubility FROM temp3 WHERE temp1.id=temp3.id;

DELETE FROM temp4;
INSERT INTO temp4 (error) 
SELECT  ph_error FROM impdata.expresaq;
--UPDATE temp1 SET error = temp4.error FROM temp4 WHERE temp1.id=temp4.id;

DELETE FROM temp5;
INSERT INTO temp5 (log_error)
SELECT  lgca_error FROM impdata.expresaq;
UPDATE temp1 SET log_error = temp5.log_error FROM temp5 WHERE temp1.id=temp5.id;

SELECT * FROM temp1;
INSERT INTO public.results_aq (id_exp, id_elem, solubility, log_solubility, error, log_error) 
SELECT id_exp, id_elem, solubility, log_solubility, error, log_error FROM temp1;

--DROP TABLE temp1, temp2, temp3, temp4, temp5;


-- sql queries for deleting experiments if by mistake we executed the same querie 2 times

-- DELETE FROM public.results_aq WHERE id_exp > ((SELECT count(*) FROM public.experiments) - 2*(SELECT count(*) FROM impdata.expcsd));