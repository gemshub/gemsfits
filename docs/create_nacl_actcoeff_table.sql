CREATE TABLE nacl_act_coeff_298k_1bar
(
  id character varying(20),
  temperature double precision,
  pressure double precision,
  molality_1 double precision,
  molality_2 double precision,
  code integer,
  val double precision,
  other_1 double precision,
  other_2 double precision,
  other_3 double precision,
  other_4 double precision,
  error double precision,
  species_1_1 character varying(20),
  species_1_2 character varying(20),
  species_1_3 character varying(20),
  species_1_4 character varying(20),
  species_2_1 character varying(20),
  species_2_2 character varying(20),
  species_2_3 character varying(20),
  species_2_4 character varying(20),
  stoic_1_1 integer,
  stoic_1_2 integer,
  stoic_1_3 integer,
  stoic_1_4 integer,
  stoic_2_1 integer,
  stoic_2_2 integer,
  stoic_2_3 integer,
  stoic_2_4 integer,
  "Reference" text,
  ende integer
)
WITH (
  OIDS=FALSE
);
ALTER TABLE nacl_act_coeff_298k_1bar OWNER TO dmitrii;
