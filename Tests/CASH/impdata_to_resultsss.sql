-- fills in the results_ss table
DROP TABLE temp1;
CREATE TABLE temp1
(
  ID serial,
  id_exp integer, 
  id_elem integer,
  amount real,
  log_amount real,
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
  amount real
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
  log_amount real
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

--DELETE FROM public.results_ss;
  
 -- Si

DELETE FROM temp1;
INSERT INTO temp1 (id_exp) 
SELECT  experiment_id FROM public.experiments WHERE experiment_id > ((SELECT count(*) FROM public.experiments) - (SELECT count(*) FROM impdata.expcsd)) ORDER BY experiment_id;

DELETE FROM temp2;
INSERT INTO temp2 (amount) 
SELECT  si_csh_mol FROM impdata.expresss;

UPDATE temp1 SET amount = temp2.amount FROM temp2 WHERE temp1.id=temp2.id;
UPDATE temp1 SET id_elem = (SELECT element_id from public.elements e WHERE e.name = 'Si');

--DELETE FROM temp3;
--INSERT INTO temp3 (log_solubility)
--SELECT  ph_aq FROM impdata.expresaq;
--UPDATE temp1 SET log_solubility = temp3.log_solubility FROM temp3 WHERE temp1.id=temp3.id;

DELETE FROM temp4;
INSERT INTO temp4 (error) 
SELECT  ca_csh_error FROM impdata.expresss;
--UPDATE temp1 SET error = temp4.error FROM temp4 WHERE temp1.id=temp4.id;

--DELETE FROM temp5;
--INSERT INTO temp5 (log_error)
--SELECT  ph_error FROM impdata.expresaq;
--UPDATE temp1 SET log_error = temp5.log_error FROM temp5 WHERE temp1.id=temp5.id;

SELECT * FROM temp1;
INSERT INTO public.results_ss (id_exp, id_elem, amount) 
SELECT id_exp, id_elem, amount FROM temp1;


 -- Ca

DELETE FROM temp1;
INSERT INTO temp1 (id_exp) 
SELECT  experiment_id FROM public.experiments WHERE experiment_id > ((SELECT count(*) FROM public.experiments) - (SELECT count(*) FROM impdata.expcsd)) ORDER BY experiment_id;

DELETE FROM temp2;
INSERT INTO temp2 (amount) 
SELECT  ca_csh_mol FROM impdata.expresss;

UPDATE temp1 SET amount = temp2.amount FROM temp2 WHERE temp1.id=temp2.id;
UPDATE temp1 SET id_elem = (SELECT element_id from public.elements e WHERE e.name = 'Ca');

--DELETE FROM temp3;
--INSERT INTO temp3 (log_solubility)
--SELECT  ph_aq FROM impdata.expresaq;
--UPDATE temp1 SET log_solubility = temp3.log_solubility FROM temp3 WHERE temp1.id=temp3.id;

DELETE FROM temp4;
INSERT INTO temp4 (error) 
SELECT  ca_csh_error FROM impdata.expresss;
UPDATE temp1 SET error = temp4.error FROM temp4 WHERE temp1.id=temp4.id;

--DELETE FROM temp5;
--INSERT INTO temp5 (log_error)
--SELECT  ph_error FROM impdata.expresaq;
--UPDATE temp1 SET log_error = temp5.log_error FROM temp5 WHERE temp1.id=temp5.id;

SELECT * FROM temp1;
INSERT INTO public.results_ss (id_exp, id_elem, amount, error) 
SELECT id_exp, id_elem, amount, error FROM temp1;


-- sql queries for deleting experiments if by mistake we executed the same querie 2 times

-- DELETE FROM public.results_aq WHERE id_exp > ((SELECT count(*) FROM public.experiments) - 2*(SELECT count(*) FROM impdata.expcsd));