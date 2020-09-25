-- University: FIT VUT 2017
-- Course: IIS Course Project
-- Project: IS for GP
-- Authors: xondru14, xcincu00

SET default_storage_engine = INNODB;

-- Clearing tables' content
# DROP TABLE template_drug CASCADE;
# DROP TABLE template_execution CASCADE;
# DROP TABLE template_insurance CASCADE;
# DROP TABLE account CASCADE;
# DROP TABLE patient CASCADE;
# DROP TABLE extern_med_check CASCADE;
# DROP TABLE med_case CASCADE;
# DROP TABLE visit CASCADE;
# DROP TABLE drug CASCADE;
# DROP TABLE vaccination CASCADE;
# DROP TABLE invoice CASCADE;
# DROP TABLE execution CASCADE;

-- templates --

CREATE TABLE template_drug
(
  drug_code   INT NOT NULL PRIMARY KEY,
  name        VARCHAR(255),
  description VARCHAR(1024),
  cost        INT
)
  ENGINE = INNODB;

CREATE TABLE template_execution
(
  execution_code VARCHAR(255) NOT NULL PRIMARY KEY,
  name           VARCHAR(255),
  cost           INT
)
  ENGINE = INNODB;

CREATE TABLE template_insurance
(
  insurance INT NOT NULL PRIMARY KEY,
  name      VARCHAR(255),
  region    VARCHAR(255),
  contact   VARCHAR(20)
)
  ENGINE = INNODB;

-- tables --

-- ACCOUNT, -- admin will be inserted by default
CREATE TABLE account
(
  id_account INT(11)                 NOT NULL AUTO_INCREMENT PRIMARY KEY,
  username   VARCHAR(100)
             COLLATE utf8_unicode_ci NOT NULL,
  password   VARCHAR(255)
             COLLATE utf8_unicode_ci NOT NULL,
  type       VARCHAR(255)
             COLLATE utf8_unicode_ci NOT NULL CHECK (type = 'doctor' OR type = 'nurse')
)
  ENGINE = INNODB;

-- PATIENT
CREATE TABLE patient
(
  birth_cn     VARCHAR(11)  NOT NULL PRIMARY KEY,
  firstname    VARCHAR(255) NOT NULL,
  lastname     VARCHAR(255) NOT NULL,
  sex          VARCHAR(8) CHECK (sex = 'muž' OR sex = 'žena'),
  city         VARCHAR(255),
  street_house VARCHAR(255),
  email        VARCHAR(255),
  phone        VARCHAR(255),
  removed      CHAR(1) CHECK (removed = 'Y' OR removed = 'N'), -- patient deleted from the database
  external     CHAR(1) CHECK (external = 'Y' OR external = 'N'), -- patient is external
  insurance    INT, -- foreign key

  FOREIGN KEY (insurance)
  REFERENCES template_insurance (insurance)
    ON DELETE CASCADE
  -- CONSTRAINT chk_patient_rc CHECK (REGEXP_LIKE(birth_cn, '[0-9]{6}/[0-9]{3,4}'))
)
  ENGINE = INNODB;

-- VISIT
CREATE TABLE visit
(
id_visit INT(11)     NOT NULL AUTO_INCREMENT PRIMARY KEY,
date     DATETIME    NOT NULL,
description          VARCHAR(4096)
                     COLLATE utf8_unicode_ci,
diagnosis            VARCHAR(4096)
                     COLLATE utf8_unicode_ci NOT NULL,
birth_cn VARCHAR(11) NOT NULL, -- foreign key

FOREIGN KEY (birth_cn)
REFERENCES patient (birth_cn)
ON DELETE CASCADE

)
ENGINE = INNODB;

-- EXTERNAL MED CHECK, -- admin will be inserted by default
CREATE TABLE extern_med_check
(
  id_extern_med_check INT(11)                 NOT NULL AUTO_INCREMENT PRIMARY KEY,
  name                VARCHAR(255)
                      COLLATE utf8_unicode_ci NOT NULL, -- name of the workplace / specialist
  diagnosis           VARCHAR(4096)
                      COLLATE utf8_unicode_ci, -- problem description
  id_visit            INT(11), -- foreign key

  FOREIGN KEY (id_visit)
  REFERENCES visit (id_visit)
    ON DELETE CASCADE
)
  ENGINE = INNODB;

-- DRUG
CREATE TABLE drug
(
  id_drug   INT(11)                         NOT NULL AUTO_INCREMENT PRIMARY KEY,
  amount    INT COLLATE utf8_unicode_ci     NOT NULL,
  drug_code INT COLLATE utf8_unicode_ci     NOT NULL, -- foreign key
  id_visit  INT(11) COLLATE utf8_unicode_ci NOT NULL, -- foreign key

  FOREIGN KEY (drug_code)
  REFERENCES template_drug (drug_code)
    ON DELETE CASCADE,

  FOREIGN KEY (id_visit)
  REFERENCES visit (id_visit)
    ON DELETE CASCADE

)
  ENGINE = INNODB;

-- INVOICE
CREATE TABLE invoice
(
  invoice_number INT(11)                     NOT NULL AUTO_INCREMENT PRIMARY KEY,
  seller         VARCHAR(1024)
                 COLLATE utf8_unicode_ci     NOT NULL,
  buyer          VARCHAR(4096)
                 COLLATE utf8_unicode_ci     NOT NULL,
  amount         INT COLLATE utf8_unicode_ci NOT NULL,
  issue_date     DATETIME                    NOT NULL,
  due_date       DATETIME                    NOT NULL

)
  ENGINE = INNODB;

-- EXECUTION
CREATE TABLE execution
(
  id_execution   INT(11) NOT NULL AUTO_INCREMENT PRIMARY KEY,
  execution_code VARCHAR(255) NOT NULL, -- foreign key
  id_visit       INT NOT NULL, -- foreign key
  invoice_number INT, -- foreign key

  FOREIGN KEY (execution_code)
  REFERENCES template_execution (execution_code)
    ON DELETE CASCADE,

  FOREIGN KEY (id_visit)
  REFERENCES visit (id_visit)
    ON DELETE CASCADE,

  FOREIGN KEY (invoice_number)
  REFERENCES invoice (invoice_number)
    ON DELETE CASCADE

)
  ENGINE = INNODB;

-- inserts --
INSERT INTO account (id_account, username, password, type) VALUES (
  NULL,
  'admin',
  'heslo',
  'admin'
);

INSERT INTO account (id_account, username, password, type) VALUES (
  NULL,
  'lekar',
  'heslo',
  'doctor'
);

INSERT INTO account (id_account, username, password, type) VALUES (
  NULL,
  'sestra',
  'heslo',
  'nurse'
);

INSERT INTO template_drug VALUES (
  '123',
  'Bioparox',
  'Antibiotikum proti zápalu',
  '250'
);

INSERT INTO template_drug VALUES (
  '456',
  'Paralen',
  'Tlumí bolest, snižuje teplotu, pomáhá při chřipce. Nedráždí žaludek, krátkodobě mohou užívat těhotné a kojící ženy',
  '30'
);

INSERT INTO template_drug VALUES (
  '654',
  'Parsen Forte',
  'Prášky na spaní',
  '142'
);

INSERT INTO template_drug VALUES (
  '487',
  'Hypnox',
  'Prášky na spaní',
  '160'
);

INSERT INTO template_drug VALUES (
  '442',
  'Prospan sirup',
  'Sirup na kašel',
  '84'
);

INSERT INTO template_drug VALUES (
  '321',
  'Akneroxid',
  'Gel k lokální (místní) aplikaci na kůži k zevní léčbě acne vulgaris akné, (trudovitosti)',
  '150'
);

INSERT INTO template_insurance VALUES (
  '213',
  'Revírní bratrská pokladna',
  'Brno',
  '420123456789'
);

INSERT INTO template_insurance VALUES (
  '111',
  'Všeobecná zdravotní pojišťovna ČR',
  'Praha',
  '420123321456'
);

INSERT INTO template_insurance VALUES (
  '205',
  'Česká průmyslová zdravotní pojišťovna',
  'Praha',
  '420123321654'
);

INSERT INTO patient VALUES (
  '841221/5511',
  'Petr',
  'Novák',
  'muž',
  'Brno',
  'Pod Mostem 99',
  'novak@gg.cn',
  '+420 123 456 789',
  'N',
  'N',
  '213'
);

INSERT INTO patient VALUES (
  '550512/5436',
  'Jan',
  'Konečný',
  'muž',
  'Brno',
  'Nad Mostem 42',
  'konecny@gg.al',
  '+420 123 456 189',
  'N',
  'N',
  '205'
);

INSERT INTO patient VALUES (
  '310202/2021',
  'Michaela',
  'Výmyslová',
  'žena',
  'Hradec Králové',
  'Při Mostu 45',
  'vymyslovam@mail.gg',
  '+420 333 222 333',
  'N',
  'N',
  '111'
);

INSERT INTO patient VALUES (
  '850920/4021',
  'Lucie',
  'Okatá',
  'žena',
  'Brno',
  'U Mostu 12',
  'okatal@mail.gg',
  '+421 555 222 333',
  'N',
  'Y',
  '205'
);

INSERT INTO patient VALUES (
  '621201/6821',
  'Jean Claude',
  'Van Damme',
  'muž',
  'Tromsø',
  'U nádraží 2',
  'jcvd@moviestar.gg',
  '+421 555 222 211',
  'N',
  'N',
  '111'
);

INSERT INTO visit VALUES (
NULL,
'2017-12-13 11:15:00',
'Pacient si stěžuje na bolesti hlavy a zhoršený zrak.',
'Pacient má otřes mozku.',
'550512/5436'
);

INSERT INTO visit VALUES (
NULL,
'2018-01-28 07:30:00',
'Doplní lékař...',
'Doplní lékař...',
'550512/5436'
);

INSERT INTO visit VALUES (
NULL,
'2018-03-09 10:20:00',
'Doplní lékař...',
'Doplní lékař...',
'550512/5436'
);

INSERT INTO visit VALUES (
NULL,
'2017-01-18 15:10:00',
'Pacient si stěžuje na neschopnost rozlišovat čichové podněty. Také nevidí, u písmen na tabuli namísto "Masarykova Universita" přečetl "VUT FIT"',
'Pacient má rýmu a taktéř se mu zhoršil zrak.',
'310202/2021'
);

INSERT INTO visit VALUES (
NULL,
'2018-03-05 11:05:00',
'Doplní lékař...',
'Doplní lékař...',
'310202/2021'
);

INSERT INTO visit VALUES (
NULL,
'2018-05-12 11:45:00',
'Doplní lékař...',
'Doplní lékař...',
'310202/2021'
);

INSERT INTO visit VALUES (
NULL,
'2017-11-25 10:00:00',
'Pacient si stěžuje na neschopnost rozlišovat čichové pachy',
'Pacient má ucpaný nos.',
'850920/4021'
);

INSERT INTO visit VALUES (
NULL,
'2018-04-21 09:00:00',
'Doplní lékař...',
'Doplní lékař...',
'850920/4021'
);

INSERT INTO extern_med_check VALUES (
  NULL,
  'Oční ambulance',
  'Slepota, pacient nevidí ani svůj nos.',
  '11'
);

INSERT INTO extern_med_check VALUES (
  NULL,
  'ORL',
  'Pacient má ucpaný nos.',
  '11'
);

INSERT INTO extern_med_check VALUES (
  NULL,
  'Plastická chirurgie',
  'Pacient má křivý nos.',
  '11'
);

INSERT INTO extern_med_check VALUES (
  NULL,
  'Čuchová ambulance',
  'Prohlédněte pacienta, nic necítí.',
  '21'
);

INSERT INTO drug VALUES (
NULL,
  '2',
  '123',
  '11'
);

INSERT INTO drug VALUES (
  NULL,
  '3',
  '123',
  '21'
);

INSERT INTO invoice VALUES (
  NULL,
  'IIS praktický lékař',
  'Revírní bratrská pokladna',
  '2750',
  '2017-10-18 10:00:00',
  '2017-11-18 10:00:00'
);

INSERT INTO template_execution VALUES (
'60a',
'Komplexní předoperační vyšetření pacienta.',
2500
);

INSERT INTO template_execution VALUES (
'75b',
'Vystavení potvrzení o způsobilosti na práci a studium v zahraničí.',
600
);

INSERT INTO template_execution VALUES (
'252b',
'Očkování proti chřipce',
1000
);

INSERT INTO template_execution VALUES (
  '70d',
  'Očkování proti malárii.',
  1250
);

INSERT INTO template_execution VALUES (
  '72d',
  'Očkování proti tetanu.',
  450
);

INSERT INTO template_execution VALUES (
'50a',
'Vyšetření a potvrzení pro řidičský průkaz.',
500
);

INSERT INTO template_execution VALUES (
'22b',
'Laboratorní vyšetření krve.',
200
);

INSERT INTO execution VALUES (
  NULL,
  '60a',
  '11',
  '1'
);
