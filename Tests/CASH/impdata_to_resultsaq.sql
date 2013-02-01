-- fills in the results_aq table

DROP TABLE impdata.temp1;
CREATE TABLE impdata.temp1
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
ALTER TABLE impdata.temp1
  OWNER TO postgres;
DROP TABLE impdata.temp2;
CREATE TABLE impdata.temp2
(
  ID serial,
  solubility real
)
WITH (
  OIDS=TRUE
);
ALTER TABLE impdata.temp2
  OWNER TO postgres;
DROP TABLE impdata.temp3;
CREATE TABLE impdata.temp3
(
  ID serial,
  log_solubility real
)
WITH (
  OIDS=TRUE
);
ALTER TABLE impdata.temp3
  OWNER TO postgres;
DROP TABLE impdata.temp4;
CREATE TABLE impdata.temp4
(
  ID serial,
  error real
)
WITH (
  OIDS=TRUE
);
ALTER TABLE impdata.temp4
  OWNER TO postgres;
DROP TABLE impdata.temp5;
CREATE TABLE impdata.temp5
(
  ID serial,
  log_error real
)
WITH (
  OIDS=TRUE
);
ALTER TABLE impdata.temp5
  OWNER TO postgres;

--DELETE FROM public.results_aq;
  
 -- pH

DELETE FROM impdata.temp1;
INSERT INTO impdata.temp1 (id_exp) 
	SELECT  experiment_id FROM public.experiments 
	WHERE experiment_id > ((SELECT count(*) FROM public.experiments) 
	- (SELECT count(*) FROM impdata.expcsd)) ORDER BY experiment_id;

DELETE FROM impdata.temp2;
INSERT INTO impdata.temp2 (solubility) 
	SELECT  ph_aq FROM impdata.expresaq;

UPDATE impdata.temp1 SET solubility = 10^(-temp2.solubility) 
	FROM impdata.temp2 WHERE temp1.id=temp2.id;
UPDATE impdata.temp1 SET id_elem = 
	(SELECT element_id from public.elements e WHERE e.name = 'pH');

DELETE FROM impdata.temp3;
INSERT INTO impdata.temp3 (log_solubility)
	SELECT  ph_aq FROM impdata.expresaq;
UPDATE impdata.temp1 SET log_solubility = temp3.log_solubility 
	FROM impdata.temp3 WHERE temp1.id=temp3.id;

DELETE FROM impdata.temp4;
INSERT INTO impdata.temp4 (error) 
	SELECT  ph_error FROM impdata.expresaq;
--UPDATE impdata.temp1 SET error = 10^(-temp4.error) FROM impdata.temp4 WHERE impdata.temp1.id=temp4.id;

DELETE FROM impdata.temp5;
INSERT INTO impdata.temp5 (log_error)
	SELECT  ph_error FROM impdata.expresaq;
--UPDATE impdata.temp1 SET log_error = temp5.log_error FROM impdata.temp5 WHERE temp1.id=temp5.id;

SELECT * FROM impdata.temp1;
INSERT INTO public.results_aq (id_exp, id_elem, solubility, 
	log_solubility, error, log_error) 
	SELECT id_exp, id_elem, solubility, log_solubility, error, log_error 
	FROM impdata.temp1;

 -- Si

DELETE FROM impdata.temp1;
INSERT INTO impdata.temp1 (id_exp) 
	SELECT  experiment_id FROM public.experiments 
	WHERE experiment_id > ((SELECT count(*) FROM public.experiments) 
	- (SELECT count(*) FROM impdata.expcsd)) ORDER BY experiment_id;

DELETE FROM impdata.temp2;
INSERT INTO impdata.temp2 (solubility) 
	SELECT  si_aq_mm FROM impdata.expresaq;

UPDATE impdata.temp1 SET solubility = temp2.solubility*(10^(-3)) 
	FROM impdata.temp2 WHERE temp1.id=temp2.id; -- from mM/l to M/l
UPDATE impdata.temp1 SET id_elem = 
	(SELECT element_id from public.elements e WHERE e.name = 'Si');

DELETE FROM impdata.temp3;
INSERT INTO impdata.temp3 (log_solubility)
SELECT lgsi_aq FROM impdata.expresaq;
UPDATE impdata.temp1 SET log_solubility = temp3.log_solubility 
	FROM impdata.temp3 WHERE temp1.id=temp3.id;

DELETE FROM impdata.temp4;
INSERT INTO impdata.temp4 (error) 
	SELECT  lgsi_error FROM impdata.expresaq;
UPDATE impdata.temp1 SET error = 10^temp4.error 
	FROM impdata.temp4 WHERE temp1.id=temp4.id;

DELETE FROM impdata.temp5;
INSERT INTO impdata.temp5 (log_error)
	SELECT  lgsi_error FROM impdata.expresaq;
UPDATE impdata.temp1 SET log_error = temp5.log_error 
	FROM impdata.temp5 WHERE temp1.id=temp5.id;

SELECT * FROM impdata.temp1;
INSERT INTO public.results_aq (id_exp, id_elem, solubility, log_solubility, 
	error, log_error) 
	SELECT id_exp, id_elem, solubility, log_solubility, error, log_error 
	FROM impdata.temp1;

 -- Ca

DELETE FROM impdata.temp1;
INSERT INTO impdata.temp1 (id_exp) 
	SELECT  experiment_id FROM public.experiments 
	WHERE experiment_id > ((SELECT count(*) FROM public.experiments) 
	- (SELECT count(*) FROM impdata.expcsd)) ORDER BY experiment_id;

DELETE FROM impdata.temp2;
INSERT INTO impdata.temp2 (solubility) 
	SELECT  ca_aq_mm FROM impdata.expresaq;

UPDATE impdata.temp1 SET solubility = temp2.solubility*(10^(-3)) 
	FROM impdata.temp2 WHERE temp1.id=temp2.id;
UPDATE impdata.temp1 SET id_elem = 
	(SELECT element_id from public.elements e WHERE e.name = 'Ca');

DELETE FROM impdata.temp3;
INSERT INTO impdata.temp3 (log_solubility)
	SELECT lgca_aq FROM impdata.expresaq;
UPDATE impdata.temp1 SET log_solubility = temp3.log_solubility 
	FROM impdata.temp3 WHERE temp1.id=temp3.id;

DELETE FROM impdata.temp4;
INSERT INTO impdata.temp4 (error) 
	SELECT  lgca_error FROM impdata.expresaq;
UPDATE impdata.temp1 SET error = 10^temp4.error 
	FROM impdata.temp4 WHERE temp1.id=temp4.id;

DELETE FROM impdata.temp5;
INSERT INTO impdata.temp5 (log_error)
	SELECT  lgca_error FROM impdata.expresaq;
UPDATE impdata.temp1 SET log_error = temp5.log_error FROM impdata.temp5 
	WHERE temp1.id=temp5.id;

SELECT * FROM impdata.temp1;
INSERT INTO public.results_aq (id_exp, id_elem, solubility, log_solubility, 
	error, log_error) 
	SELECT id_exp, id_elem, solubility, log_solubility, error, log_error 
	FROM impdata.temp1;

 -- Al

DELETE FROM impdata.temp1;
INSERT INTO impdata.temp1 (id_exp) 
	SELECT  experiment_id FROM public.experiments 
	WHERE experiment_id > ((SELECT count(*) FROM public.experiments) 
	- (SELECT count(*) FROM impdata.expcsd)) ORDER BY experiment_id;

DELETE FROM impdata.temp2;
INSERT INTO impdata.temp2 (solubility) 
	SELECT  al_aq_mm FROM impdata.expresaq;

UPDATE impdata.temp1 SET solubility = temp2.solubility*(10^(-3)) 
	FROM impdata.temp2 WHERE temp1.id=temp2.id;
UPDATE impdata.temp1 SET id_elem = 
	(SELECT element_id from public.elements e WHERE e.name = 'Al');

DELETE FROM impdata.temp3;
INSERT INTO impdata.temp3 (log_solubility)
	SELECT lgal_aq FROM impdata.expresaq;
UPDATE impdata.temp1 SET log_solubility = temp3.log_solubility 
	FROM impdata.temp3 WHERE temp1.id=temp3.id;

DELETE FROM impdata.temp4;
INSERT INTO impdata.temp4 (error) 
SELECT  lgal_error FROM impdata.expresaq;
UPDATE impdata.temp1 SET error = 10^temp4.error FROM impdata.temp4 
	WHERE temp1.id=temp4.id;

DELETE FROM impdata.temp5;
INSERT INTO impdata.temp5 (log_error)
SELECT  lgal_error FROM impdata.expresaq;
UPDATE impdata.temp1 SET log_error = temp5.log_error FROM impdata.temp5 
	WHERE temp1.id=temp5.id;

SELECT * FROM impdata.temp1;
INSERT INTO public.results_aq (id_exp, id_elem, solubility, log_solubility, 
	error, log_error) 
	SELECT id_exp, id_elem, solubility, log_solubility, error, log_error 
	FROM impdata.temp1;

 -- Na

DELETE FROM impdata.temp1;
INSERT INTO impdata.temp1 (id_exp) 
	SELECT  experiment_id FROM public.experiments 
	WHERE experiment_id > ((SELECT count(*) FROM public.experiments) 
	- (SELECT count(*) FROM impdata.expcsd)) ORDER BY experiment_id;

DELETE FROM impdata.temp2;
INSERT INTO impdata.temp2 (solubility) 
	SELECT  na_aq_mm FROM impdata.expresaq;

UPDATE impdata.temp1 SET solubility = temp2.solubility*(10^(-3)) 
	FROM impdata.temp2 WHERE temp1.id=temp2.id;
UPDATE impdata.temp1 SET id_elem = 
	(SELECT element_id from public.elements e WHERE e.name = 'Na');

DELETE FROM impdata.temp3;
INSERT INTO impdata.temp3 (log_solubility)
	SELECT lgna_aq FROM impdata.expresaq;
UPDATE impdata.temp1 SET log_solubility = temp3.log_solubility 
	FROM impdata.temp3 WHERE temp1.id=temp3.id;

DELETE FROM impdata.temp4;
INSERT INTO impdata.temp4 (error) 
	SELECT  lgna_error FROM impdata.expresaq;
UPDATE impdata.temp1 SET error = 10^temp4.error FROM impdata.temp4 
	WHERE temp1.id=temp4.id;

DELETE FROM impdata.temp5;
INSERT INTO impdata.temp5 (log_error)
	SELECT  lgna_error FROM impdata.expresaq;
UPDATE impdata.temp1 SET log_error = temp5.log_error FROM impdata.temp5 
	WHERE temp1.id=temp5.id;

SELECT * FROM impdata.temp1;
INSERT INTO public.results_aq (id_exp, id_elem, solubility, log_solubility, 
	error, log_error) 
	SELECT id_exp, id_elem, solubility, log_solubility, error, log_error 
	FROM impdata.temp1;

 -- K

DELETE FROM impdata.temp1;
INSERT INTO impdata.temp1 (id_exp) 
	SELECT  experiment_id FROM public.experiments 
	WHERE experiment_id > ((SELECT count(*) FROM public.experiments) 
	- (SELECT count(*) FROM impdata.expcsd)) ORDER BY experiment_id;

DELETE FROM impdata.temp2;
INSERT INTO impdata.temp2 (solubility) 
	SELECT  k_aq_mm FROM impdata.expresaq;

UPDATE impdata.temp1 SET solubility = temp2.solubility*(10^(-3)) 
	FROM impdata.temp2 WHERE temp1.id=temp2.id;
UPDATE impdata.temp1 SET id_elem = 
	(SELECT element_id from public.elements e WHERE e.name = 'K');

DELETE FROM impdata.temp3;
INSERT INTO impdata.temp3 (log_solubility)
	SELECT lgk_aq FROM impdata.expresaq;
UPDATE impdata.temp1 SET log_solubility = temp3.log_solubility 
	FROM impdata.temp3 WHERE temp1.id=temp3.id;

DELETE FROM impdata.temp4;
INSERT INTO impdata.temp4 (error) 
	SELECT  lgk_error FROM impdata.expresaq;
UPDATE impdata.temp1 SET error = 10^temp4.error FROM impdata.temp4 
	WHERE temp1.id=temp4.id;

DELETE FROM impdata.temp5;
INSERT INTO impdata.temp5 (log_error)
	SELECT  lgk_error FROM impdata.expresaq;
UPDATE impdata.temp1 SET log_error = temp5.log_error FROM impdata.temp5 
	WHERE temp1.id=temp5.id;

SELECT * FROM impdata.temp1;
INSERT INTO public.results_aq (id_exp, id_elem, solubility, log_solubility, 
	error, log_error) 
	SELECT id_exp, id_elem, solubility, log_solubility, error, log_error 
	FROM impdata.temp1;

 -- C

DELETE FROM impdata.temp1;
INSERT INTO impdata.temp1 (id_exp) 
	SELECT  experiment_id FROM public.experiments 
	WHERE experiment_id > ((SELECT count(*) FROM public.experiments) 
	- (SELECT count(*) FROM impdata.expcsd)) ORDER BY experiment_id;

DELETE FROM impdata.temp2;
INSERT INTO impdata.temp2 (solubility) 
	SELECT  c_aq_mm FROM impdata.expresaq;

UPDATE impdata.temp1 SET solubility = temp2.solubility*(10^(-3)) 
	FROM impdata.temp2 WHERE temp1.id=temp2.id;
UPDATE impdata.temp1 SET id_elem = 
	(SELECT element_id from public.elements e WHERE e.name = 'C');

DELETE FROM impdata.temp3;
INSERT INTO impdata.temp3 (log_solubility)
	SELECT lgc_aq FROM impdata.expresaq;
UPDATE impdata.temp1 SET log_solubility = temp3.log_solubility 
	FROM impdata.temp3 WHERE temp1.id=temp3.id;

DELETE FROM impdata.temp4;
INSERT INTO impdata.temp4 (error) 
	SELECT  lgc_error FROM impdata.expresaq;
UPDATE impdata.temp1 SET error = 10^temp4.error FROM impdata.temp4 
	WHERE temp1.id=temp4.id;

DELETE FROM impdata.temp5;
INSERT INTO impdata.temp5 (log_error)
	SELECT  lgc_error FROM impdata.expresaq;
UPDATE impdata.temp1 SET log_error = temp5.log_error FROM impdata.temp5 
	WHERE temp1.id=temp5.id;

SELECT * FROM impdata.temp1;
INSERT INTO public.results_aq (id_exp, id_elem, solubility, log_solubility, 
	error, log_error) 
	SELECT id_exp, id_elem, solubility, log_solubility, error, log_error 
	FROM impdata.temp1;

DELETE FROM public.results_aq WHERE (solubility, log_solubility) IS NULL; 
-- deletes empty element fields

--DROP TABLE temp1, temp2, temp3, temp4, temp5;

-- sql queries for deleting experiments if by mistake we executed the same querie 2 times

-- DELETE FROM public.results_aq WHERE id_exp > ((SELECT count(*) FROM public.experiments) - 2*(SELECT count(*) FROM impdata.expcsd));