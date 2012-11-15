COPY (id,temperature,pressure,molality_1,molality_2,code,val,other_1,other_2,other_3,other_4,error,species_1_1,species_1_2,species_1_3,species_1_4,species_2_1,species_2_2,species_2_3,species_2_4,stoic_1_1,stoic_1_2,stoic_1_3,stoic_1_4,stoic_2_1,stoic_2_2,stoic_2_3,stoic_2_4,"Reference",ende)
FROM '/home/dmiron/nacl_298k.csv'
WITH DELIMITER ';'
CSV HEADER