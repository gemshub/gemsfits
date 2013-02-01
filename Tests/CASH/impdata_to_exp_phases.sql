
-- fills in the exp_phases table
DROP TABLE impdata.temp1;
CREATE TABLE impdata.temp1
(
  ID serial,
  id_exp integer, 
  id_phase integer,
  amount real
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
  id_phase integer,
  amount real
)
WITH (
  OIDS=TRUE
);
ALTER TABLE impdata.temp2
  OWNER TO postgres;




DELETE FROM public.exp_phase;
---DELETE FROM public.phase_elem;
-- C-S-solid
-- adds id_exp column to temp1
DELETE FROM impdata.temp1;
INSERT INTO impdata.temp1 (id_exp) 
SELECT  experiment_id FROM public.experiments WHERE experiment_id > ((SELECT count(*) FROM public.experiments) - (SELECT count(*) FROM impdata.expcsd)) ORDER BY experiment_id;
-- adds column amount to temp2
DELETE FROM impdata.temp2;
INSERT INTO impdata.temp2 (amount) 
SELECT  c_s_solid FROM impdata.expcsd;
-- joins temp1 and temp2
UPDATE impdata.temp1 SET amount = temp2.amount FROM impdata.temp2 WHERE temp1.id=temp2.id;
UPDATE impdata.temp1 SET id_phase = (SELECT phase_id from public.phases p WHERE p.name = 'C-S-H-3T-2');
-- adds the joined table to exp_comp
INSERT INTO public.exp_phase (id_exp, id_phase, amount) SELECT id_exp, id_phase, amount FROM impdata.temp1;


