-- fills in the results_ss table
DROP TABLE impdata.temp1;
CREATE TABLE impdata.temp1
(
  ID serial,
  id_exp integer, 
  id_elem integer,
  amount real,
  log_amount real,
  error real,
  log_error real,
  id_phase integer
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
  amount real
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
  log_amount real
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

DELETE FROM public.results_ss; -- to start with empty table
  
 -- Si

DELETE FROM impdata.temp1;
INSERT INTO impdata.temp1 (id_exp) 
	SELECT  experiment_id FROM public.experiments 
	WHERE experiment_id > ((SELECT count(*) FROM public.experiments) 
	- (SELECT count(*) FROM impdata.expcsd)) ORDER BY experiment_id;

DELETE FROM impdata.temp2;
INSERT INTO impdata.temp2 (amount) 
	SELECT  si_csh_mol FROM impdata.expresss;

UPDATE impdata.temp1 SET amount = temp2.amount FROM impdata.temp2 
	WHERE temp1.id=temp2.id;
UPDATE impdata.temp1 SET id_elem = 
	(SELECT element_id from public.elements e WHERE e.name = 'Si');

--DELETE FROM temp3;
--INSERT INTO temp3 (log_solubility)
--SELECT  ph_aq FROM impdata.expresaq;
--UPDATE temp1 SET log_solubility = temp3.log_solubility FROM temp3 WHERE temp1.id=temp3.id;

DELETE FROM impdata.temp4;
INSERT INTO impdata.temp4 (error) 
	SELECT  ca_csh_error FROM impdata.expresss;
--UPDATE temp1 SET error = temp4.error FROM temp4 WHERE temp1.id=temp4.id;

--DELETE FROM temp5;
--INSERT INTO temp5 (log_error)
--SELECT  ph_error FROM impdata.expresaq;
--UPDATE temp1 SET log_error = temp5.log_error FROM temp5 WHERE temp1.id=temp5.id;

SELECT * FROM impdata.temp1;
INSERT INTO public.results_ss (id_exp, id_elem, amount) 
	SELECT id_exp, id_elem, amount FROM impdata.temp1;


 -- Ca

DELETE FROM impdata.temp1;
INSERT INTO impdata.temp1 (id_exp) 
	SELECT  experiment_id FROM public.experiments 
	WHERE experiment_id > ((SELECT count(*) FROM public.experiments) 
	- (SELECT count(*) FROM impdata.expcsd)) ORDER BY experiment_id;

DELETE FROM impdata.temp2;
INSERT INTO impdata.temp2 (amount) 
	SELECT  ca_csh_mol FROM impdata.expresss;

UPDATE impdata.temp1 SET amount = temp2.amount FROM impdata.temp2 
	WHERE temp1.id=temp2.id;
UPDATE impdata.temp1 SET id_elem = 
	(SELECT element_id from public.elements e WHERE e.name = 'Ca');

--DELETE FROM temp3;
--INSERT INTO temp3 (log_solubility)
--SELECT  ph_aq FROM impdata.expresaq;
--UPDATE temp1 SET log_solubility = temp3.log_solubility FROM temp3 WHERE temp1.id=temp3.id;

DELETE FROM impdata.temp4;
INSERT INTO impdata.temp4 (error) 
	SELECT  ca_csh_error FROM impdata.expresss;
UPDATE impdata.temp1 SET error = temp4.error FROM impdata.temp4 
	WHERE temp1.id=temp4.id;

--DELETE FROM temp5;
--INSERT INTO temp5 (log_error)
--SELECT  ph_error FROM impdata.expresaq;
--UPDATE temp1 SET log_error = temp5.log_error FROM temp5 WHERE temp1.id=temp5.id;

SELECT * FROM impdata.temp1;
INSERT INTO public.results_ss (id_exp, id_elem, amount, error) 
	SELECT id_exp, id_elem, amount, error FROM impdata.temp1;


 -- Al

DELETE FROM impdata.temp1;
INSERT INTO impdata.temp1 (id_exp) 
	SELECT  experiment_id FROM public.experiments 
	WHERE experiment_id > ((SELECT count(*) FROM public.experiments) 
	- (SELECT count(*) FROM impdata.expcsd)) ORDER BY experiment_id;

DELETE FROM impdata.temp2;
INSERT INTO impdata.temp2 (amount) 
	SELECT  al_csh_mol FROM impdata.expresss;

UPDATE impdata.temp1 SET amount = temp2.amount FROM impdata.temp2 
	WHERE temp1.id=temp2.id;
UPDATE impdata.temp1 SET id_elem = 
	(SELECT element_id from public.elements e WHERE e.name = 'Al');

--DELETE FROM temp3;
--INSERT INTO temp3 (log_solubility)
--SELECT  ph_aq FROM impdata.expresaq;
--UPDATE temp1 SET log_solubility = temp3.log_solubility FROM temp3 WHERE temp1.id=temp3.id;

DELETE FROM impdata.temp4;
INSERT INTO impdata.temp4 (error) 
	SELECT  al_csh_error FROM impdata.expresss;
UPDATE impdata.temp1 SET error = temp4.error FROM impdata.temp4 
	WHERE temp1.id=temp4.id;

--DELETE FROM temp5;
--INSERT INTO temp5 (log_error)
--SELECT  ph_error FROM impdata.expresaq;
--UPDATE temp1 SET log_error = temp5.log_error FROM temp5 WHERE temp1.id=temp5.id;

SELECT * FROM impdata.temp1;
INSERT INTO public.results_ss (id_exp, id_elem, amount, error) 
	SELECT id_exp, id_elem, amount, error FROM impdata.temp1;

 -- Na

DELETE FROM impdata.temp1;
INSERT INTO impdata.temp1 (id_exp) 
	SELECT  experiment_id FROM public.experiments 
	WHERE experiment_id > ((SELECT count(*) FROM public.experiments) 
	- (SELECT count(*) FROM impdata.expcsd)) ORDER BY experiment_id;

DELETE FROM impdata.temp2;
INSERT INTO impdata.temp2 (amount) 
SELECT  na_csh_mol FROM impdata.expresss;

UPDATE impdata.temp1 SET amount = temp2.amount FROM impdata.temp2 
	WHERE temp1.id=temp2.id;
UPDATE impdata.temp1 SET id_elem = 
	(SELECT element_id from public.elements e WHERE e.name = 'Na');

--DELETE FROM temp3;
--INSERT INTO temp3 (log_solubility)
--SELECT  ph_aq FROM impdata.expresaq;
--UPDATE temp1 SET log_solubility = temp3.log_solubility FROM temp3 WHERE temp1.id=temp3.id;

DELETE FROM impdata.temp4;
INSERT INTO impdata.temp4 (error) 
	SELECT  na_csh_error FROM impdata.expresss;
UPDATE impdata.temp1 SET error = temp4.error FROM impdata.temp4 
	WHERE temp1.id=temp4.id;

--DELETE FROM temp5;
--INSERT INTO temp5 (log_error)
--SELECT  ph_error FROM impdata.expresaq;
--UPDATE temp1 SET log_error = temp5.log_error FROM temp5 WHERE temp1.id=temp5.id;

SELECT * FROM impdata.temp1;
INSERT INTO public.results_ss (id_exp, id_elem, amount, error) 
	SELECT id_exp, id_elem, amount, error FROM impdata.temp1;


 -- K

DELETE FROM impdata.temp1;
INSERT INTO impdata.temp1 (id_exp) 
	SELECT  experiment_id FROM public.experiments 
	WHERE experiment_id > ((SELECT count(*) FROM public.experiments) 
	- (SELECT count(*) FROM impdata.expcsd)) ORDER BY experiment_id;

DELETE FROM impdata.temp2;
INSERT INTO impdata.temp2 (amount) 
	SELECT  k_csh_mol FROM impdata.expresss;

UPDATE impdata.temp1 SET amount = temp2.amount FROM impdata.temp2 
	WHERE temp1.id=temp2.id;
UPDATE impdata.temp1 SET id_elem = 
	(SELECT element_id from public.elements e WHERE e.name = 'K');

--DELETE FROM temp3;
--INSERT INTO temp3 (log_solubility)
--SELECT  ph_aq FROM impdata.expresaq;
--UPDATE temp1 SET log_solubility = temp3.log_solubility FROM temp3 WHERE temp1.id=temp3.id;

DELETE FROM impdata.temp4;
INSERT INTO impdata.temp4 (error) 
	SELECT  k_csh_error FROM impdata.expresss;
UPDATE impdata.temp1 SET error = temp4.error FROM impdata.temp4 
	WHERE temp1.id=temp4.id;

--DELETE FROM temp5;
--INSERT INTO temp5 (log_error)
--SELECT  ph_error FROM impdata.expresaq;
--UPDATE temp1 SET log_error = temp5.log_error FROM temp5 WHERE temp1.id=temp5.id;

SELECT * FROM impdata.temp1;
INSERT INTO public.results_ss (id_exp, id_elem, amount, error) 
	SELECT id_exp, id_elem, amount, error FROM impdata.temp1;


UPDATE public.results_ss SET id_phase = exp_phase.id_phase FROM public.exp_phase WHERE results_ss.id_exp = exp_phase.id_exp;

DELETE FROM public.results_ss WHERE amount IS NULL; 



-- deletes all rows with empty element fields

-- sql queries for deleting experiments if by mistake we executed the same querie 2 times

-- DELETE FROM public.results_aq WHERE id_exp > ((SELECT count(*) FROM public.experiments) - 2*(SELECT count(*) FROM impdata.expcsd));