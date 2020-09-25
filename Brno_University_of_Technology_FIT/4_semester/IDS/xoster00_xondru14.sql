-- IDS Project SQL Database
-- authors: xondru14, xoster00


-- Clearing tables content
DROP TABLE ciselnik_leky CASCADE CONSTRAINTS;
DROP TABLE ciselnik_vykon CASCADE CONSTRAINTS;
DROP TABLE ciselnik_pojistovna CASCADE CONSTRAINTS;
DROP TABLE ciselnik_ockovani CASCADE CONSTRAINTS;
DROP TABLE ciselnik_lekar CASCADE CONSTRAINTS;
DROP TABLE pacient CASCADE CONSTRAINTS;
DROP TABLE externi_vysetreni CASCADE CONSTRAINTS;
DROP TABLE lekarsky_pripad CASCADE CONSTRAINTS;
DROP TABLE navsteva CASCADE CONSTRAINTS;
DROP TABLE lek CASCADE CONSTRAINTS;
DROP TABLE ockovani CASCADE CONSTRAINTS;
DROP TABLE faktura CASCADE CONSTRAINTS;
DROP TABLE vykon CASCADE CONSTRAINTS;

DROP SEQUENCE seq_vysetreni;
DROP SEQUENCE seq_lekpripad;
DROP SEQUENCE seq_navsteva;
DROP SEQUENCE seq_lek;
DROP SEQUENCE seq_vykon;

DROP INDEX idx_pac_vyr;

SET serveroutput ON;


-- create sequences
create SEQUENCE seq_vysetreni
start with 1 NOMAXVALUE
  INCREMENT BY 1;
  
  create SEQUENCE seq_lekpripad
start with 1 NOMAXVALUE
  INCREMENT BY 1;
  
  create SEQUENCE seq_navsteva
start with 1 NOMAXVALUE
  INCREMENT BY 1;
  
  create SEQUENCE seq_lek
start with 1 NOMAXVALUE
  INCREMENT BY 1;
  
  create SEQUENCE seq_vykon
start with 1 NOMAXVALUE
  INCREMENT BY 1;


-- Creating ciselniks tables
CREATE TABLE ciselnik_leky
(
    kod_leku          INT NOT NULL,
    nazev_leku        VARCHAR2(100 BYTE) NOT NULL,
    specifikace       VARCHAR2(500),
    cena              INT,
    uhrada_zp         CHAR(1) CHECK (uhrada_zp = 'A' or uhrada_zp = 'N'),

    CONSTRAINT pk_ciselnik_leky PRIMARY KEY (kod_leku)
);


CREATE TABLE ciselnik_vykon
(
    id_vykon        VARCHAR2(6 BYTE) NOT NULL,
    nazov_vykonu    VARCHAR2(100) NOT NULL,
    body            INT,
    cena            INT,

    CONSTRAINT pk_ciselnik_vykon PRIMARY KEY (id_vykon)
);


CREATE TABLE ciselnik_pojistovna
(
    pojistovna      INT NOT NULL,
    nazev           VARCHAR2(50),
    region          VARCHAR2(100),
    kontakt         VARCHAR2(20),

    CONSTRAINT pk_ciselnik_pojistovna PRIMARY KEY (pojistovna)
);


CREATE TABLE ciselnik_ockovani
(
    nazev_ockovaci_latky  VARCHAR2(50) NOT NULL,
    popis                 VARCHAR2(500),

    CONSTRAINT pk_ciselnik_ockovani PRIMARY KEY (nazev_ockovaci_latky)
);


CREATE TABLE ciselnik_lekar
(
    id_lekar      INT NOT NULL,
    jmeno         VARCHAR2(50),
    prijmeni      VARCHAR2(50),
    kontakt       VARCHAR2(13 BYTE),
    specializace  VARCHAR2(30),

    CONSTRAINT pk_ciselnik_lekar PRIMARY KEY (id_lekar)
);


-- Creating main tables
CREATE TABLE pacient
( 
    rodne_cislo          VARCHAR2(11 BYTE) NOT NULL, 
    jmeno                VARCHAR2(50) NOT NULL,
    prijmeni             VARCHAR2(50) NOT NULL,
    pohlavi              VARCHAR2(5) CHECK (pohlavi = 'MUŽ' or pohlavi = 'ŽENA'),
    mesto                VARCHAR2(30),
    ulice                VARCHAR2(30),
    cislo_popisne        INT,
    kontakt              VARCHAR2(13 BYTE),
    vyrazeny             CHAR(1) CHECK (vyrazeny = 'A' or vyrazeny = 'N'),
    datum_prohlidky      DATE,
    id_lekar             INT, -- foreign key 
    pojistovna           INT, -- foreign key

    CONSTRAINT pk_pacient PRIMARY KEY (rodne_cislo),
    CONSTRAINT fk_pacient_ciselnik_pojistovna FOREIGN KEY (pojistovna) REFERENCES ciselnik_pojistovna,
    CONSTRAINT fk_pacient_ciselnik_lekar FOREIGN KEY (id_lekar) REFERENCES ciselnik_lekar,

    CONSTRAINT chk_pacient_rc CHECK (REGEXP_LIKE(rodne_cislo, '[0-9]{6}/[0-9]{3,4}')),
    CONSTRAINT chk_pacient_att CHECK ((id_lekar is NULL and datum_prohlidky is NOT NULL) or
                         (id_lekar is NOT NULL and datum_prohlidky is NULL)) 
);


CREATE TABLE externi_vysetreni 
( 
    id_externi_vysetreni  INT NOT NULL,
    kod_vysetreni         INT NOT NULL,
    nazev_pracoviste      VARCHAR2(60 BYTE) NOT NULL, 
    stanovena_diagnoza    VARCHAR2(500) NOT NULL, 
    datum                 DATE,

    CONSTRAINT pk_externi_vysetreni PRIMARY KEY (id_externi_vysetreni)
);


CREATE TABLE lekarsky_pripad
( 
    id_lekarsky_pripad          INT NOT NULL, 
    popis                       VARCHAR2(500 BYTE),
    diagnoza                    VARCHAR2(10),
    datum                       DATE,
    rodne_cislo                 VARCHAR2(11 BYTE),  -- foreign key
    id_externi_vysetreni        INT, -- foreign key

    CONSTRAINT pk_lekarsky_pripad PRIMARY KEY (id_lekarsky_pripad),
    CONSTRAINT fk_lp_rodne_cislo FOREIGN KEY (rodne_cislo) REFERENCES pacient,
    CONSTRAINT fk_lp_id_externi_vysetreni FOREIGN KEY (id_externi_vysetreni) REFERENCES externi_vysetreni
);


CREATE TABLE navsteva
(
    id_navsteva   INT NOT NULL,
    datum         DATE,
    rodne_cislo   VARCHAR2(11 BYTE),  -- foreign key

    CONSTRAINT pk_navsteva PRIMARY KEY (id_navsteva),
    CONSTRAINT fk_navsteva_rodne_cislo FOREIGN KEY (rodne_cislo) REFERENCES pacient
);  


CREATE TABLE lek
(
    
    id_lek              INT NOT NULL,
    kod_leku            INT NOT NULL,
    --nazev               VARCHAR2(50 BYTE) NOT NULL,
    mnozstvi            INT NOT NULL,
    id_navsteva         INT,  -- foreign key
    --specifikace_leku    VARCHAR2(100 BYTE), -- foreign key

    CONSTRAINT pk_lek PRIMARY KEY (id_lek),
    CONSTRAINT fk_lek_ciselnik_lek FOREIGN KEY (kod_leku) REFERENCES ciselnik_leky,
    CONSTRAINT fk_lek_navsteva FOREIGN KEY (id_navsteva) REFERENCES navsteva
);


CREATE TABLE faktura
(
    cislo_faktury     INT NOT NULL,
    odberatel         VARCHAR2(50 BYTE),
    cena              INT NOT NULL,
    datum_vystaveni   DATE NOT NULL,
    splatnost         DATE NOT NULL,
    forma_uhrady      VARCHAR2(50 BYTE),

    CONSTRAINT pk_faktura PRIMARY KEY (cislo_faktury)
);


CREATE TABLE ockovani
(
    id_navsteva            INT NOT NULL,
    poradove_cislo         INT NOT NULL, 
    typ                    VARCHAR2(50 BYTE),
    nazev_ockovaci_latky   VARCHAR2(50 BYTE), -- foreign key

    CONSTRAINT pk_ockovani PRIMARY KEY (id_navsteva),
    CONSTRAINT fk_ockovani_ciselnik_ockovani FOREIGN KEY (nazev_ockovaci_latky) REFERENCES ciselnik_ockovani
);


CREATE TABLE vykon
(
    poradie_vykonu      INT NOT NULL,
    id_vykon            VARCHAR2(6 BYTE) NOT NULL, -- foreign key
    pocet               INT,
    id_navsteva         INT,  -- foreign key 
    cislo_faktury       INT,  -- foreign key

    CONSTRAINT pk_vykon PRIMARY KEY (poradie_vykonu),
    CONSTRAINT fk_vykon_ciselnik_vykon FOREIGN KEY (id_vykon) REFERENCES ciselnik_vykon,
    CONSTRAINT fk_vykon_navsteva FOREIGN KEY (id_navsteva) REFERENCES navsteva,
    CONSTRAINT fk_vykon_faktura FOREIGN KEY (cislo_faktury) REFERENCES faktura
);




-- triggers
-- netrivialni - automaticke generovani noveho klice ze sekvence, pokud je NULL (part 1 z triggeru)
  CREATE OR REPLACE TRIGGER tr_vysetreni
  BEFORE INSERT ON externi_vysetreni
  FOR EACH ROW
DECLARE 
  id_externi_vysetreni externi_vysetreni.id_externi_vysetreni%TYPE;
BEGIN
  id_externi_vysetreni := :NEW.id_externi_vysetreni;
  IF ( id_externi_vysetreni IS NULL ) THEN
  :NEW.id_externi_vysetreni := seq_vysetreni.nextval;
  END IF;
END tr_vysetreni;
/

  CREATE OR REPLACE TRIGGER tr_lek_pripad
  BEFORE INSERT ON lekarsky_pripad
  FOR EACH ROW
DECLARE 
  id_lekarsky_pripad lekarsky_pripad.id_lekarsky_pripad%TYPE;
BEGIN
  id_lekarsky_pripad := :NEW.id_lekarsky_pripad;
  IF ( id_lekarsky_pripad IS NULL ) THEN
  :NEW.id_lekarsky_pripad := seq_lekpripad.nextval;
  END IF;
END tr_lek_pripad;
/

  CREATE OR REPLACE TRIGGER tr_navsteva
  BEFORE INSERT ON navsteva
  FOR EACH ROW
DECLARE 
  id_navsteva navsteva.id_navsteva%TYPE;
BEGIN
  id_navsteva := :NEW.id_navsteva;
  IF ( id_navsteva IS NULL ) THEN
  :NEW.id_navsteva := seq_navsteva.nextval;
  END IF;
END tr_navsteva;
/

  CREATE OR REPLACE TRIGGER tr_lek
  BEFORE INSERT ON lek
  FOR EACH ROW
DECLARE 
  id_lek lek.id_lek%TYPE;
BEGIN
  id_lek := :NEW.id_lek;
  IF ( id_lek IS NULL ) THEN
  :NEW.id_lek := seq_lek.nextval;
  END IF;
END tr_lek;
/


  CREATE OR REPLACE TRIGGER tr_vykon
  BEFORE INSERT ON vykon
  FOR EACH ROW
DECLARE 
  poradie_vykonu vykon.poradie_vykonu%TYPE;
BEGIN
  poradie_vykonu := :NEW.poradie_vykonu;
  IF ( poradie_vykonu IS NULL ) THEN
  :NEW.poradie_vykonu := seq_vykon.nextval;
  END IF;
END tr_vykon;
/


-- kontrola rodneho cisla (formatu) (triggers part 2)
-- exception pri spatnem formatu
-- zmena formatu na cesky (kvuli kontrole rc)
ALTER session SET nls_date_format='dd.mm.yy';

CREATE OR REPLACE TRIGGER tr_rc
  BEFORE INSERT OR UPDATE OF rodne_cislo ON pacient
  FOR EACH ROW
DECLARE
  rc pacient.rodne_cislo%TYPE;
  rc_conv INT;
  den   INT;
  mesic INT;
  rok   INT;
  datum_conv DATE;
BEGIN

  rc := :NEW.rodne_cislo;  
  rc_conv := SUBSTR(rc, 1,6);
    
  rok   := SUBSTR(rc_conv, 1, 2);
  mesic := SUBSTR(rc_conv, 3, 2);
  den   := SUBSTR(rc_conv, 5, 6);
  --DBMS_OUTPUT.put_line(den || ',' || mesic || ',' || rok);
  -- prevod rodneho cisla u zen - odecteni 50
  IF (mesic > 50) THEN
    mesic := mesic - 50;
  END IF;
  BEGIN
    datum_conv := TO_DATE(den||'.'||mesic||'.'||rok,'dd.mm.yy');
  EXCEPTION
    WHEN OTHERS THEN
       Raise_Application_Error(-20211, 'Spatny format rodneho cisla.');
  END;
  
END tr_rc;
/


-- procedures
-- zkontroluj planovane prohlidky - pokud se jedna o cloveka starsiho 
-- 60 let, upozornime, ze je treba naplanovat kontrolu 
CREATE OR REPLACE PROCEDURE zkontroluj_prohlidky IS
  rok INTEGER;
  cur_rok INTEGER;
BEGIN

SELECT to_char(sysdate, 'YYYY') INTO cur_rok FROM dual; -- get current date
cur_rok := cur_rok - 60;
cur_rok := SUBSTR(cur_rok, 3, 4); -- last 2 chars

FOR rec IN (
  SELECT *
    FROM pacient
  WHERE pacient.datum_prohlidky IS NULL)
  LOOP
  rok := SUBSTR(rec.rodne_cislo, 1, 2); -- first 2 chars
  -- porovnani data narozeni cloveka se soucasnym datem
  IF (cur_rok > rok) THEN
    DBMS_OUTPUT.put_line('Potreba naplanovat kontrolu u pacienta: '|| 
    rec.jmeno ||' ' || rec.prijmeni ||', rc: '|| rec.rodne_cislo);
  END IF;
  END LOOP;
  
END; 
/


-- zmen bydliste u pacienta z danym rc
CREATE OR REPLACE PROCEDURE bydliste_pacient(rc IN VARCHAR2, mesto_i IN VARCHAR2, 
        ulice_i IN VARCHAR2, cislo_popisne_i IN VARCHAR2) IS
  pac_row pacient%ROWTYPE;
  
BEGIN
  SELECT * INTO pac_row FROM pacient WHERE pacient.rodne_cislo = rc;
  
  DBMS_OUTPUT.put_line('Stare bydliste: ' || pac_row.mesto || ', ' ||
  pac_row.ulice || ', ' || pac_row.cislo_popisne);
  

  UPDATE pacient
  SET
    pacient.mesto = mesto_i,
    pacient.ulice = ulice_i,
    pacient.cislo_popisne = cislo_popisne_i
  WHERE pacient.rodne_cislo = rc;

  DBMS_OUTPUT.put_line('Nove bydliste: ' || mesto_i || ', ' ||
  ulice_i || ', ' || cislo_popisne_i);
  
  EXCEPTION
  WHEN NO_DATA_FOUND THEN
    Raise_Application_Error(-20212, 'Zadna data.');

END;
/


-- Test input data
INSERT INTO CISELNIK_LEKY VALUES('123',
                                 'Bioparox',
                                 'Antibiotikum proti zápalu', 
                                 '250',
                                 'N'
);

INSERT INTO CISELNIK_LEKY VALUES('456',
                                 'Paralen',
                                 'Tlumí bolest, snižuje teplotu, pomáhá při chřipce. Nedráždí žaludek, krátkodobě mohou užívat těhotné a kojící ženy', 
                                 '30',
                                 'N'
);

INSERT INTO CISELNIK_LEKY VALUES('789',
                                 'Parsen Forte',
                                 'Prášky na spaní', 
                                 '142',
                                 'N'
);

INSERT INTO CISELNIK_LEKY VALUES('321',
                                 'Akneroxid',
                                 'Gel k lokální (místní) aplikaci na kůži k zevní léčbě acne vulgaris akné, (trudovitosti)', 
                                 '150',
                                 'N'
);

INSERT INTO CISELNIK_POJISTOVNA VALUES('213',
                                       'Revírní bratrská pokladna', 
                                       'Brno',
                                       '420123456789'
);

INSERT INTO CISELNIK_POJISTOVNA VALUES('111',
                                       'Všeobecná zdravotní pojišťovna ČR', 
                                       'Praha',
                                       '420123321456'
);

INSERT INTO CISELNIK_POJISTOVNA VALUES('205',
                                       'Česká průmyslová zdravotní pojišťovna', 
                                       'Praha',
                                       '420123321654'
);

INSERT INTO CISELNIK_OCKOVANI VALUES('Imovax',
                                     'Očkování proti vzteklině'
);

INSERT INTO CISELNIK_LEKAR VALUES('23845462',
                                  'Jan',
                                  'Chytrý',
                                  '+420123123123',
                                  'Gynekolog'
);

INSERT INTO PACIENT VALUES('900820/5021',
                           'Andrea',
                           'Moudrá',
                           'ŽENA',
                           'Brno',
                           'Pod Mostem',
                           '42',
                           '+420321321321',
                           'N',
                           NULL,
                           '23845462',
                           '213'
);

INSERT INTO PACIENT VALUES('771224/6231',
                           'David',
                           'Vymyslel',
                           'MUŽ',
                           'Pardubice',
                           'Nad Mostem',
                           '43',
                           '+420111222333',
                           'N',
                           NULL,
                           '23845462',
                           '213'
);

INSERT INTO PACIENT VALUES('771224/6242',
                           'Petr',
                           'Vymyslel',
                           'MUŽ',
                           'Brno',
                           'U Mostu',
                           '20',
                           '+420222222333',
                           'N',
                           NULL,
                           '23845462',
                           '111'
);

INSERT INTO PACIENT VALUES('310202/2021',
                           'Michaela',
                           'Výmyslová',
                           'ŽENA',
                           'Hradec Králové',
                           'Při Mostu',
                           '1123',
                           '+420333222333',
                           'N',
                           NULL,
                           '23845462',
                           '111'
);

INSERT INTO PACIENT VALUES('880821/3021',
                           'Ivana',
                           'Nová',
                           'ŽENA',
                           'Brno',
                           'U Mostu',
                           '920',
                           '+420444222333',
                           'N',
                           NULL,
                           '23845462',
                           '205'
);

INSERT INTO PACIENT VALUES('850920/4021',
                           'Lucie',
                           'Okatá',
                           'ŽENA',
                           'Brno',
                           'U Mostu',
                           '922',
                           '+420555222333',
                           'N',
                           TO_DATE('2.2.2016 8:00', 'DD.MM.YYYY HH24:MI:SS'),
                           NULL,
                           '205'
);

INSERT INTO EXTERNI_VYSETRENI VALUES(null,
                                      '2023',
                                     'Oční ambulance',
                                     'Slepota, pacient nevidí ani svůj nos.',
                                     TO_DATE('23.3.2016 14:30', 'DD.MM.YYYY HH24:MI:SS')
);


INSERT INTO EXTERNI_VYSETRENI VALUES(null,'2024',
                                     'ORL',
                                     'Pacient má ucpaný nos.',
                                     TO_DATE('1.1.2016 10:45', 'DD.MM.YYYY HH24:MI:SS')
);

INSERT INTO EXTERNI_VYSETRENI VALUES(null,'2025',
                                     'Plastická chirurgie',
                                     'Pacient má křivý nos.',
                                     TO_DATE('11.2.2015 9:40', 'DD.MM.YYYY HH24:MI:SS')
);

INSERT INTO EXTERNI_VYSETRENI VALUES(null,'2026',
                                     'Čuchová ambulance',
                                     'Slepota, pacient nevidí ani svůj puch.',
                                     TO_DATE('23.3.2016 14:31', 'DD.MM.YYYY HH24:MI:SS')
);

INSERT INTO EXTERNI_VYSETRENI VALUES(null,
                                      '2023',
                                     'Oční ambulance',
                                     'Slepota, pacient nevidí ani svůj nos.',
                                     TO_DATE('23.3.2016 14:37', 'DD.MM.YYYY HH24:MI:SS')
);



INSERT INTO LEKARSKY_PRIPAD VALUES(null,
                                   'Pacientka si st?žuje na bolesti hlavy a zhoršený zrak.',
                                   NULL,
                                   TO_DATE('23.3.2016 11:15', 'DD.MM.YYYY HH24:MI:SS'),
                                   '850920/4021',
                                   '1'
);  

  

INSERT INTO LEKARSKY_PRIPAD VALUES(null,
                                   'Pacientka si stěžuje na nesouměrnost svého obličeje',
                                   NULL,
                                   TO_DATE('20.2.2016 12:15', 'DD.MM.YYYY HH24:MI:SS'),
                                   '880821/3021',
                                   '2'
);  

INSERT INTO LEKARSKY_PRIPAD VALUES(null,
                                   'Pacient si stěžuje na neschopnost rozlišovat čichové podněty',
                                   NULL,
                                   TO_DATE('23.3.2016 13:15', 'DD.MM.YYYY HH24:MI:SS'),
                                   '771224/6231',
                                   '3'
);

INSERT INTO LEKARSKY_PRIPAD VALUES(null,
                                   'Pacient si stěžuje na neschopnost rozlišovat čichové pachy',
                                   NULL,
                                   TO_DATE('23.3.2016 13:16', 'DD.MM.YYYY HH24:MI:SS'),
                                   '850920/4021',
                                   '4'
);

INSERT INTO LEKARSKY_PRIPAD VALUES(null,
                                   'Pacient si stěžuje na neschopnost rozlišovat čichové pachy',
                                   NULL,
                                   TO_DATE('23.3.2016 13:17', 'DD.MM.YYYY HH24:MI:SS'),
                                   '850920/4021',
                                   '5'
);

INSERT INTO NAVSTEVA VALUES(null,
                            TO_DATE('23.3.2016 10:50', 'DD.MM.YYYY HH24:MI:SS'),
                            '900820/5021'
);

INSERT INTO NAVSTEVA VALUES(null,
                            TO_DATE('1.4.2016 12:10', 'DD.MM.YYYY HH24:MI:SS'),
                            '310202/2021'
);

INSERT INTO NAVSTEVA VALUES(null,
                            TO_DATE('2.4.2016 13:10', 'DD.MM.YYYY HH24:MI:SS'),
                            '771224/6231'
);

INSERT INTO LEK VALUES(null,
                       '123',
                       '2',
                       '1'
);

INSERT INTO LEK VALUES(null,
                       '123',
                       '2',
                       '2'

);

INSERT INTO OCKOVANI VALUES('1',
                            '1',
                            'Očkování proti malárii.',
                            'Imovax'
);

INSERT INTO FAKTURA VALUES('032016',
                           'Revírní bratrská pokladna',
                           '5000',
                           TO_DATE('21.3.2016 13:30', 'DD.MM.YYYY HH24:MI:SS'),
                           TO_DATE('21.4.2016 13:30', 'DD.MM.YYYY HH24:MI:SS'),
                           NULL
);

INSERT INTO CISELNIK_VYKON VALUES('60a', 
                                  'Komplexné predoperačné vyšetrenie pacienta',
                                  3,
                                  2500
);

INSERT INTO CISELNIK_VYKON VALUES('75b', 
                                  'Vystavenie potvrdenia o spôsobilosti na prácu a štúdium v zahraničí',
                                  1,
                                  600
);

INSERT INTO CISELNIK_VYKON VALUES('252b', 
                                  'Očkovanie',
                                  2,
                                  1000
);

INSERT INTO VYKON VALUES(null,
                         '60a',
                         '1',
                         '1', 
                         '032016'
);

INSERT INTO VYKON VALUES(null,
                         '75b',
                         '1',
                         '2',  
                         '032016'
);

INSERT INTO VYKON VALUES(null,
                         '252b',
                         '1',
                         '3', 
                         '032016'
);


-- SELECT statements

-- vypis vsech pacientu a nazev pojistoven, u kterych jsou registrovani
-- spojeni dvou tabulek
SELECT p.jmeno, p.prijmeni, c.nazev
FROM pacient p, ciselnik_pojistovna c
WHERE p.pojistovna = c.pojistovna;

-- vypis poctu pacientu registrovanych u pojistovny Revirni bratrska pokladna
-- group by
SELECT c.nazev, COUNT(*) as pocet_registrovanych_pac
FROM pacient p, ciselnik_pojistovna c
WHERE p.pojistovna = c.pojistovna --AND c.nazev='Revírní bratrská pokladna'
GROUP BY c.nazev;

-- vypis pacienty, kterym byl nekdy predepsan lek Bioparox v ciselniku
-- ulozeni pod kodem 123 a pocet doposud predepsanych kusu toho leku
-- spojeni trech tabulek
SELECT p.jmeno, p.prijmeni, l.mnozstvi as Bioparox_pocet_kusu
FROM pacient p, navsteva n, lek l
WHERE p.rodne_cislo = n.rodne_cislo AND n.id_navsteva=l.id_navsteva
  AND l.kod_leku='123';
  
-- vypis pacienty ktery nikdy nebyli ockovani
-- exists, spojeni 2 tabulek
SELECT p.jmeno, p.prijmeni, p.rodne_cislo
FROM pacient p, navsteva nv
WHERE p.rodne_cislo=nv.rodne_cislo
  AND NOT EXISTS (
    SELECT *
    FROM ockovani o
    WHERE nv.id_navsteva=o.id_navsteva);


-- vypis externych vysetreni ktere absolvoval urcity pacient (Okatá)
-- a kolikrat navstivil dane externi zarizeni
-- group by a agregacni funkce, spojeni 3 tabulek
SELECT p.prijmeni,p.rodne_cislo, ev.nazev_pracoviste,
  COUNT(*) as pocet_navstev
FROM pacient p, lekarsky_pripad lp, externi_vysetreni ev
WHERE p.prijmeni='Okatá' AND p.rodne_cislo=lp.rodne_cislo 
  AND lp.id_externi_vysetreni=ev.id_externi_vysetreni
GROUP BY p.prijmeni, p.rodne_cislo, ev.nazev_pracoviste;

-- umozni lekarovi zobrazit nazov, body a cenu jednotlivych vykonov
-- pri ktorych sa cena pohybuje nad 750kc
-- spojeni dvou tabulek
SELECT id_vykon as kod, nazov_vykonu as nazev, body as body, cena as cena
FROM vykon NATURAL LEFT JOIN ciselnik_vykon
where cena > 750;

-- umozni lekarovi vidiet pacientov, ktori navstvili ordinaciu v aktualnom
-- mesici
-- IN select
SELECT *
FROM pacient WHERE rodne_cislo IN (
SELECT rodne_cislo FROM navsteva WHERE datum BETWEEN TO_DATE('2016-03-31','YYYY-MM-DD') AND TO_DATE('2016-04-30','YYYY-MM-DD')
);



-- permissions setup
GRANT ALL ON ciselnik_leky TO xoster00;
GRANT ALL ON ciselnik_vykon TO xoster00;
GRANT ALL ON ciselnik_pojistovna TO xoster00;
GRANT ALL ON ciselnik_ockovani TO xoster00;
GRANT ALL ON ciselnik_lekar TO xoster00;
GRANT ALL ON pacient TO xoster00;
GRANT ALL ON externi_vysetreni TO xoster00;
GRANT ALL ON lekarsky_pripad TO xoster00;
GRANT ALL ON navsteva TO xoster00;
GRANT ALL ON lek TO xoster00;
GRANT ALL ON ockovani TO xoster00;
GRANT ALL ON faktura TO xoster00;
GRANT ALL ON vykon TO xoster00;


-- volani procedur
-- kontrola prohlidky u lidi nad 60 let
EXECUTE zkontroluj_prohlidky;

-- zmena bydliste u pacienta
EXECUTE bydliste_pacient('771224/6231', 'Chobotov', 'V Popelnici', '42');
SELECT * FROM Pacient; -- zobrazeni zmenenych hodnot


-- EXPLAIN PLAN output shows how Oracle runs the SQL statement 
-- when the statement was explained
-- explain plan bez indexu (part 1)
EXPLAIN PLAN 
  INTO PLAN_TABLE FOR
  
  SELECT pacient.vyrazeny, count(*) 
  FROM pacient
  WHERE pacient.vyrazeny = 'N' 
  GROUP BY(pacient.vyrazeny);
  
  SELECT PLAN_TABLE_OUTPUT FROM TABLE(DBMS_XPLAN.DISPLAY());

-- explain plan s indexem (part 2)
-- result: snizeni "Cost (%CPU) diky pouziti indexu
CREATE INDEX idx_pac_vyr ON pacient(vyrazeny);

EXPLAIN PLAN 
  INTO PLAN_TABLE FOR
  
  SELECT pacient.vyrazeny, count(*) 
  FROM pacient 
  WHERE pacient.vyrazeny = 'N' 
  GROUP BY(pacient.vyrazeny);
  
  SELECT PLAN_TABLE_OUTPUT FROM TABLE(DBMS_XPLAN.DISPLAY());


-- materialized views
DROP MATERIALIZED VIEW mv_one;
CREATE MATERIALIZED VIEW mv_one 
CACHE -- for small lookup tables, when data is accessed frequently - optimalization purposes
AS
-- cena vsech vydaju za vykonu
SELECT SUM(cena) as cena_vsech_vykonu
FROM vykon NATURAL LEFT JOIN ciselnik_vykon
WHERE cena is not null;

GRANT ALL ON mv_one TO xoster00;
SELECT * FROM mv_one;
