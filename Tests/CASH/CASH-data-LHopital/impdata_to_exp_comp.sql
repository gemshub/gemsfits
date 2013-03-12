-- fills in the experiments table, for more components you have to copy paste
-- the query for H2O and do the necessary changes for the new component in the
-- SELECT  h2o_l <-- name of new component column in the impdata.expcsd table, 
-- WHERE c.name = 'SiO2' <-- name of new component in the public.components table.
--DELETE FROM public.results_aq;
--DELETE FROM public.results_ss;
--DELETE FROM public.exp_comp;
--DELETE FROM public.phase_exp;
--DELETE FROM public.experiments;
--ALTER SEQUENCE experiments_experiment_id_seq RESTART WITH 1; 
-- restarts the serial counter from 1

SELECT SETVAL((SELECT pg_get_serial_sequence('experiments', 'experiment_id')), 
	(SELECT max(experiment_id) FROM experiments)+1, false); 
-- keeps the serial counter at the highest id+1.

INSERT INTO public.experiments (name, temperature, pressure, reference) 
	SELECT sampleid, t_c, p_bar, expsetid FROM impdata.expcsd;

-- fills in the exp_comp table
DROP TABLE impdata.temp1;
CREATE TABLE impdata.temp1
(
  ID serial,
  id_exp integer, 
  id_comp integer,
  quantity real
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
  id_exp integer, 
  id_comp integer,
  quantity real
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
  id_exp integer, 
  id_comp integer,
  quantity real
)
WITH (
  OIDS=TRUE
);
ALTER TABLE impdata.temp3
  OWNER TO postgres;

DELETE FROM impdata.temp3; -- Keeps the amount of H2O in Kg to recalculate the comp in moles
INSERT INTO impdata.temp3 (id_exp) 
SELECT  experiment_id FROM public.experiments 
	WHERE experiment_id > ((SELECT count(*) FROM public.experiments) 
	- (SELECT count(*) FROM impdata.expcsd)) ORDER BY experiment_id;
INSERT INTO impdata.temp3 (quantity) 
	SELECT  h2o_kg FROM impdata.expcsd;

-- H2O
-- adds id_exp column to temp1
DELETE FROM impdata.temp1;
INSERT INTO impdata.temp1 (id_exp) 
SELECT  experiment_id FROM public.experiments 
	WHERE experiment_id > ((SELECT count(*) FROM public.experiments) 
	- (SELECT count(*) FROM impdata.expcsd)) ORDER BY experiment_id;
-- adds column quantity to temp2
DELETE FROM impdata.temp2;
INSERT INTO impdata.temp2 (quantity) 
	SELECT  h2o_kg FROM impdata.expcsd;
-- joins temp1 and temp2
UPDATE impdata.temp1 SET quantity = 1000*temp2.quantity -- H2O in grams
	FROM impdata.temp2 WHERE temp1.id=temp2.id;
UPDATE impdata.temp1 SET id_comp = 
	(SELECT component_id from public.components c WHERE c.name = 'H2O');
-- adds the joined table to exp_comp
INSERT INTO public.exp_comp (id_exp, id_comp, quantity) 
	SELECT id_exp, id_comp, quantity FROM impdata.temp1;

 -- SiO2
DELETE FROM impdata.temp1;
INSERT INTO impdata.temp1 (id_exp) 
SELECT  experiment_id FROM public.experiments 
	WHERE experiment_id > ((SELECT count(*) FROM public.experiments) 
	- (SELECT count(*) FROM impdata.expcsd)) ORDER BY experiment_id;

DELETE FROM impdata.temp2;
INSERT INTO impdata.temp2 (quantity) 
SELECT  sio2 FROM impdata.expcsd;

UPDATE impdata.temp1 SET quantity = temp2.quantity 
	FROM impdata.temp2 WHERE temp1.id=temp2.id;
UPDATE impdata.temp1 SET quantity = temp1.quantity*temp3.quantity*60.0843 
	FROM impdata.temp3 WHERE temp1.id=temp3.id;
UPDATE impdata.temp1 SET id_comp = 
	(SELECT component_id from public.components c WHERE c.name = 'SiO2');

INSERT INTO public.exp_comp (id_exp, id_comp, quantity) 
	SELECT id_exp, id_comp, quantity FROM impdata.temp1;

 -- Ca(OH)2

DELETE FROM impdata.temp1;
INSERT INTO impdata.temp1 (id_exp) 
SELECT  experiment_id FROM public.experiments 	
	WHERE experiment_id > ((SELECT count(*) FROM public.experiments) 
	- (SELECT count(*) FROM impdata.expcsd)) ORDER BY experiment_id;

DELETE FROM impdata.temp2;
INSERT INTO impdata.temp2 (quantity) 
SELECT  caoh2 FROM impdata.expcsd;

UPDATE impdata.temp1 SET quantity = temp2.quantity 
	FROM impdata.temp2 WHERE temp1.id=temp2.id;
UPDATE impdata.temp1 SET quantity = temp1.quantity*temp3.quantity*74.09268 
	FROM impdata.temp3 WHERE temp1.id=temp3.id;
UPDATE impdata.temp1 SET id_comp = 
	(SELECT component_id from public.components c WHERE c.name = 'Ca(OH)2');

INSERT INTO public.exp_comp (id_exp, id_comp, quantity) 
	SELECT id_exp, id_comp, quantity FROM impdata.temp1;

 -- CaO

DELETE FROM impdata.temp1;
INSERT INTO impdata.temp1 (id_exp) 
SELECT  experiment_id FROM public.experiments 	
	WHERE experiment_id > ((SELECT count(*) FROM public.experiments) 
	- (SELECT count(*) FROM impdata.expcsd)) ORDER BY experiment_id;

DELETE FROM impdata.temp2;
INSERT INTO impdata.temp2 (quantity) 
SELECT  cao FROM impdata.expcsd;

UPDATE impdata.temp1 SET quantity = temp2.quantity 
	FROM impdata.temp2 WHERE temp1.id=temp2.id;
UPDATE impdata.temp1 SET quantity = temp1.quantity*temp3.quantity*56.0778 
	FROM impdata.temp3 WHERE temp1.id=temp3.id;
UPDATE impdata.temp1 SET id_comp = 
	(SELECT component_id from public.components c WHERE c.name = 'CaO');

INSERT INTO public.exp_comp (id_exp, id_comp, quantity) 
	SELECT id_exp, id_comp, quantity FROM impdata.temp1;

 -- Al(OH)3
DELETE FROM impdata.temp1;
INSERT INTO impdata.temp1 (id_exp) 
SELECT  experiment_id FROM public.experiments 
	WHERE experiment_id > ((SELECT count(*) FROM public.experiments) 
	- (SELECT count(*) FROM impdata.expcsd)) ORDER BY experiment_id;

DELETE FROM impdata.temp2;
INSERT INTO impdata.temp2 (quantity) 
SELECT  aloh3 FROM impdata.expcsd;

UPDATE impdata.temp1 SET quantity = temp2.quantity 
	FROM impdata.temp2 WHERE temp1.id=temp2.id;
UPDATE impdata.temp1 SET quantity = temp1.quantity*temp3.quantity*78.0035586 
	FROM impdata.temp3 WHERE temp1.id=temp3.id;
UPDATE impdata.temp1 SET id_comp = 
	(SELECT component_id from public.components c WHERE c.name = 'Al(OH)3');

INSERT INTO public.exp_comp (id_exp, id_comp, quantity) 
	SELECT id_exp, id_comp, quantity FROM impdata.temp1;

 -- CaO_Al2O3
DELETE FROM impdata.temp1;
INSERT INTO impdata.temp1 (id_exp) 
SELECT  experiment_id FROM public.experiments 
	WHERE experiment_id > ((SELECT count(*) FROM public.experiments) 
	- (SELECT count(*) FROM impdata.expcsd)) ORDER BY experiment_id;

DELETE FROM impdata.temp2;
INSERT INTO impdata.temp2 (quantity) 
SELECT  cao_al2o3 FROM impdata.expcsd;

UPDATE impdata.temp1 SET quantity = temp2.quantity 
	FROM impdata.temp2 WHERE temp1.id=temp2.id;
UPDATE impdata.temp1 SET quantity = temp1.quantity*temp3.quantity*158.039078 
	FROM impdata.temp3 WHERE temp1.id=temp3.id;
UPDATE impdata.temp1 SET id_comp = 
	(SELECT component_id from public.components c WHERE c.name = 'CaO_Al2O3');

INSERT INTO public.exp_comp (id_exp, id_comp, quantity) 
	SELECT id_exp, id_comp, quantity FROM impdata.temp1;

 -- NaOH
DELETE FROM impdata.temp1;
INSERT INTO impdata.temp1 (id_exp) 
SELECT  experiment_id FROM public.experiments 
	WHERE experiment_id > ((SELECT count(*) FROM public.experiments) 
	- (SELECT count(*) FROM impdata.expcsd)) ORDER BY experiment_id;

DELETE FROM impdata.temp2;
INSERT INTO impdata.temp2 (quantity) 
SELECT  naoh FROM impdata.expcsd;

UPDATE impdata.temp1 SET quantity = temp2.quantity 
	FROM impdata.temp2 WHERE temp1.id=temp2.id;
UPDATE impdata.temp1 SET quantity = temp1.quantity*temp3.quantity*39.99710928 
	FROM impdata.temp3 WHERE temp1.id=temp3.id;
UPDATE impdata.temp1 SET id_comp = 
	(SELECT component_id from public.components c WHERE c.name = 'NaOH');

INSERT INTO public.exp_comp (id_exp, id_comp, quantity) 
	SELECT id_exp, id_comp, quantity FROM impdata.temp1;

 -- KOH
DELETE FROM impdata.temp1;
INSERT INTO impdata.temp1 (id_exp) 
SELECT  experiment_id FROM public.experiments 
	WHERE experiment_id > ((SELECT count(*) FROM public.experiments) 
	- (SELECT count(*) FROM impdata.expcsd)) ORDER BY experiment_id;

DELETE FROM impdata.temp2;
INSERT INTO impdata.temp2 (quantity) 
SELECT  koh FROM impdata.expcsd;

UPDATE impdata.temp1 SET quantity = temp2.quantity 
	FROM impdata.temp2 WHERE temp1.id=temp2.id;
UPDATE impdata.temp1 SET quantity = temp1.quantity*temp3.quantity*56.10564 
	FROM impdata.temp3 WHERE temp1.id=temp3.id;
UPDATE impdata.temp1 SET id_comp = 
	(SELECT component_id from public.components c WHERE c.name = 'KOH');

INSERT INTO public.exp_comp (id_exp, id_comp, quantity) 
	SELECT id_exp, id_comp, quantity FROM impdata.temp1;

 -- CO2
DELETE FROM impdata.temp1;
INSERT INTO impdata.temp1 (id_exp) 
SELECT  experiment_id FROM public.experiments 
	WHERE experiment_id > ((SELECT count(*) FROM public.experiments) 
	- (SELECT count(*) FROM impdata.expcsd)) ORDER BY experiment_id;

DELETE FROM impdata.temp2;
INSERT INTO impdata.temp2 (quantity) 
SELECT  co2 FROM impdata.expcsd;

UPDATE impdata.temp1 SET quantity = temp2.quantity 
	FROM impdata.temp2 WHERE temp1.id=temp2.id;
UPDATE impdata.temp1 SET quantity = temp1.quantity*temp3.quantity*44.0095 
	FROM impdata.temp3 WHERE temp1.id=temp3.id;
UPDATE impdata.temp1 SET id_comp = 
	(SELECT component_id from public.components c WHERE c.name = 'CO2');

INSERT INTO public.exp_comp (id_exp, id_comp, quantity) 
	SELECT id_exp, id_comp, quantity FROM impdata.temp1;

 -- N2
DELETE FROM impdata.temp1;
INSERT INTO impdata.temp1 (id_exp) 
SELECT  experiment_id FROM public.experiments 
	WHERE experiment_id > ((SELECT count(*) FROM public.experiments) 
	- (SELECT count(*) FROM impdata.expcsd)) ORDER BY experiment_id;

DELETE FROM impdata.temp2;
INSERT INTO impdata.temp2 (quantity) 
SELECT  n2 FROM impdata.expcsd;

UPDATE impdata.temp1 SET quantity = temp2.quantity 
	FROM impdata.temp2 WHERE temp1.id=temp2.id;
UPDATE impdata.temp1 SET quantity = temp1.quantity*temp3.quantity*28.01344
 
	FROM impdata.temp3 WHERE temp1.id=temp3.id;
UPDATE impdata.temp1 SET id_comp = 
	(SELECT component_id from public.components c WHERE c.name = 'N2');

INSERT INTO public.exp_comp (id_exp, id_comp, quantity) 
	SELECT id_exp, id_comp, quantity FROM impdata.temp1;


-- deletes all rows empty element fields
DELETE FROM public.exp_comp WHERE quantity IS NULL OR quantity=0; 

-- sql queries for deleting experiments if by mistake 
-- we executed the same query 2 times
--DELETE FROM public.exp_comp WHERE id_exp > ((SELECT count(*) FROM public.experiments) - 2*(SELECT count(*) FROM impdata.expcsd));
--DELETE FROM public.experiments WHERE experiment_id > ((SELECT count(*) FROM public.experiments) - 2*(SELECT count(*) FROM impdata.expcsd));

