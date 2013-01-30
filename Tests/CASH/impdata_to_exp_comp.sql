-- fills in the experiments table
--DELETE FROM public.exp_comp;
--DELETE FROM public.experiments;
--ALTER SEQUENCE experiments_experiment_id_seq RESTART WITH SELECT COUNT(*) FROM experiments;

SELECT SETVAL((SELECT pg_get_serial_sequence('experiments', 'experiment_id')), (SELECT max(experiment_id) FROM experiments)+1, false);

INSERT INTO public.experiments (temperature, pressure, reference) SELECT t_c, p_bar, expsetid FROM impdata.expcsd;


-- fills in the exp_comp table
DROP TABLE temp1;
CREATE TABLE temp1
(
  ID serial,
  id_exp integer, 
  id_comp integer,
  quantity real
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
  id_exp integer, 
  id_comp integer,
  quantity real
)
WITH (
  OIDS=TRUE
);
ALTER TABLE temp2
  OWNER TO postgres;

 -- H2O

DELETE FROM temp1;
INSERT INTO temp1 (id_exp) 
SELECT  experiment_id FROM public.experiments WHERE experiment_id > ((SELECT count(*) FROM public.experiments) - (SELECT count(*) FROM impdata.expcsd)) ORDER BY experiment_id;

DELETE FROM temp2;
INSERT INTO temp2 (quantity) 
SELECT  h2o_l FROM impdata.expcsd;

UPDATE temp1 SET quantity = temp2.quantity FROM temp2 WHERE temp1.id=temp2.id;
UPDATE temp1 SET id_comp = (SELECT component_id from public.components c WHERE c.name = 'H2O');

SELECT * FROM temp1;

INSERT INTO public.exp_comp (id_exp, id_comp, quantity) SELECT id_exp, id_comp, quantity FROM temp1;

 -- SiO2

DELETE FROM temp1;
INSERT INTO temp1 (id_exp) 
SELECT  experiment_id FROM public.experiments WHERE experiment_id > ((SELECT count(*) FROM public.experiments) - (SELECT count(*) FROM impdata.expcsd)) ORDER BY experiment_id;

DELETE FROM temp2;
INSERT INTO temp2 (quantity) 
SELECT  sio2 FROM impdata.expcsd;

UPDATE temp1 SET quantity = temp2.quantity FROM temp2 WHERE temp1.id=temp2.id;
UPDATE temp1 SET id_comp = (SELECT component_id from public.components c WHERE c.name = 'SiO2');

SELECT * FROM temp1;

INSERT INTO public.exp_comp (id_exp, id_comp, quantity) SELECT id_exp, id_comp, quantity FROM temp1;

 -- Ca(OH)2

DELETE FROM temp1;
INSERT INTO temp1 (id_exp) 
SELECT  experiment_id FROM public.experiments WHERE experiment_id > ((SELECT count(*) FROM public.experiments) - (SELECT count(*) FROM impdata.expcsd)) ORDER BY experiment_id;

DELETE FROM temp2;
INSERT INTO temp2 (quantity) 
SELECT  caoh2 FROM impdata.expcsd;

UPDATE temp1 SET quantity = temp2.quantity FROM temp2 WHERE temp1.id=temp2.id;
UPDATE temp1 SET id_comp = (SELECT component_id from public.components c WHERE c.name = 'Ca(OH)2');

SELECT * FROM temp1;

INSERT INTO public.exp_comp (id_exp, id_comp, quantity) SELECT id_exp, id_comp, quantity FROM temp1;

-- sql queries for deleting experiments if by mistake we executed the same querie 2 times

--DELETE FROM public.exp_comp WHERE id_exp > ((SELECT count(*) FROM public.experiments) - 2*(SELECT count(*) FROM impdata.expcsd));

--DELETE FROM public.experiments WHERE experiment_id > ((SELECT count(*) FROM public.experiments) - 2*(SELECT count(*) FROM impdata.expcsd));

