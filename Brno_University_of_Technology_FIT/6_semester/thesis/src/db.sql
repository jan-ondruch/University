-- test database for Sporterizer --


-- DROP TABLE record CASCADE;
-- DROP TABLE exercise CASCADE;
-- DROP TABLE session CASCADE;
-- DROP TABLE player CASCADE;
-- DROP TABLE team CASCADE;
-- DROP TABLE settings CASCADE;
-- DROP TABLE account CASCADE;

-- account
CREATE TABLE account (
  email VARCHAR(100) NOT NULL PRIMARY KEY,
  password VARCHAR(30) NOT NULL
) ENGINE = INNODB;

-- settings
CREATE TABLE settings (
  settings_id INT(6) UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
  server VARCHAR(60) NOT NULL,
  port VARCHAR(8),
  api_key VARCHAR(60),
  email VARCHAR(100),

  FOREIGN KEY (email) REFERENCES account (email) ON DELETE CASCADE
) ENGINE = INNODB;

-- teams
CREATE TABLE team (
  team_id INT(6) UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
  name VARCHAR(60) NOT NULL,
  email VARCHAR(100),

  FOREIGN KEY (email) REFERENCES account (email) ON DELETE CASCADE
) ENGINE = INNODB;

-- players
CREATE TABLE player (
  player_id INT(6) UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
  firstname VARCHAR(30) NOT NULL,
  lastname VARCHAR(50) NOT NULL,
  birth VARCHAR(20) NOT NULL,  -- YYYY-MM-DD
  weight INT(3) NOT NULL,
  tag_id INT(3),
  removed VARCHAR(1) NOT NULL,
  team_id INT(6) UNSIGNED,

  FOREIGN KEY (team_id) REFERENCES team (team_id) ON DELETE CASCADE
) ENGINE = INNODB;

-- session
CREATE TABLE session (
  session_id INT(6) UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
  name VARCHAR(50) NOT NULL,
  start DATETIME NOT NULL, -- date + time
  end DATETIME,
  team_id INT(6) UNSIGNED,

  FOREIGN KEY (team_id) REFERENCES team (team_id) ON DELETE CASCADE
) ENGINE = INNODB;

-- exercise
CREATE TABLE exercise (
  exercise_id INT(6) UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
  name VARCHAR(50) NOT NULL,
  start DATETIME NOT NULL, -- date + time
  end DATETIME,
  session_id INT(6) UNSIGNED,

  FOREIGN KEY (session_id) REFERENCES session (session_id) ON DELETE CASCADE
) ENGINE = INNODB;

-- record
CREATE TABLE record (
  record_id INT(10) UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
  sessionId INT(6) UNSIGNED NOT NULL,
  playerId INT(6) UNSIGNED NOT NULL,
  timestamp BIGINT(30) UNSIGNED,
  distance FLOAT(8) UNSIGNED,
  speed FLOAT(8) UNSIGNED,
  hr FLOAT(8) UNSIGNED,

  FOREIGN KEY (sessionId) REFERENCES session (session_id) ON DELETE CASCADE,
  FOREIGN KEY (playerId) REFERENCES player (player_id) ON DELETE CASCADE
) ENGINE = INNODB;


INSERT INTO account VALUES (
  "lorem@ipsum.dolor",
  "secret"
);

INSERT INTO settings VALUES (
  1,
  "localhost",
  "4000",
  "171555a8fe71148a165392904",
  "lorem@ipsum.dolor"
);

INSERT INTO team VALUES (
  1,
  "Cleveland Cavaliers",
  "lorem@ipsum.dolor"
);

INSERT INTO team VALUES (
  10,
  "Los Angeles Lakers",
  "lorem@ipsum.dolor"
);

INSERT INTO team VALUES (
  11,
  "Miami Heat",
  "lorem@ipsum.dolor"
);

INSERT INTO player VALUES (
  1,
  "Kobe",
  "Bryant",
  "1978-08-23",
  90,
  12,
  'n',
  1
);

INSERT INTO player VALUES (
  10,
  "LeBron",
  "James",
  "1984-12-30",
  110,
  13,
  'n',
  1
);

INSERT INTO player VALUES (
  11,
  "Kyrie",
  "Irving",
  "1987-11-22",
  100,
  14,
  'n',
  1
);

INSERT INTO player VALUES (
  12,
  "James",
  "Harden",
  "1985-05-04",
  105,
  15,
  'n',
  1
);

INSERT INTO player VALUES (
  13,
  "Kevin",
  "Durant",
  "1981-11-09",
  110,
  12,
  'n',
  1
);

INSERT INTO player VALUES (
  14,
  "Kawhi",
  "Leonard",
  "1988-05-02",
  103,
  13,
  'n',
  1
);

INSERT INTO player VALUES (
  15,
  "Stephen",
  "Curry",
  "1987-09-09",
  85,
  14,
  'n',
  1
);

INSERT INTO player VALUES (
  16,
  "Russel",
  "Westbrook",
  "1982-08-04",
  99,
  12,
  'n',
  1
);