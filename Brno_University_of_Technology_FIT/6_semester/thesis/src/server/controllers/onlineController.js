const con = require('../config/connection');
const WebSocket = require('ws');
var moment = require('moment');

const {
  HR_ZONES,
  SPEED_ZONES,
  HR_INIT,
  SPRINT_THRESHOLD,
  SESSION_TIMEOUT,
  MAX_SPEED,
  data_sampling_freq
} = require('../models/sessionData');

const playerData = require('../models/playerData');
const _settings = require('../models/settings');
const _exercise = require('../models/exercise');

let response = {
  status: 200,
  data: []
}

let SessionsCons = [];  // all sessions connections


/*
 * Edit server name and port and API key.
 */
exports.editServerSettings = (req, res) => {
  let settings = new _settings(
    req.body.settings_id,
    req.body.server,
    req.body.port,
    req.body.api_key,
    req.session.email
  );

  let que = 'UPDATE settings SET server = ?, port = ?, api_key = ? WHERE email = ?';
  let cols = [
    settings.server,
    settings.port,
    settings.api_key,
    req.session.email
  ];
  con.query(que, cols, (err, results) => {
    if (err) {
      console.log("Error updating settings.", err);
    } else {
      // set the current values
      req.session.SERVER = req.body.server;
      req.session.PORT = req.body.port;
      req.session.API_KEY = req.body.api_key;

      response.data = settings;
      res.json(response);
    }
  });
}


/*
 * Get settings for the given account.
 */
exports.getServerSettings = (req, res) => {
  con.query('SELECT * FROM settings WHERE email = ?', req.session.email, (err, rows, fields) => {
    if (err) {
      console.log("Error getting settings.", err);
    }
    else {
      response.data = rows[0];
      res.json(response);
    }
  })  
}


/* websocket connections for one session */
class SessionConnection {

  constructor(email, session_id, players, server, port, api_key) {
    this.email = email;
    this.session_id = session_id;
    this.exercise_id = null;
    this.players = players;
    this.server = server;
    this.port = port;
    this.api_key = api_key;

    // connections
    this.cons = [];     // all ws connections (for each player one)

    // batch saving interval
    this.saveSessionDataBatchInterval;  // interval for saving session distances data

    // timeout set for maximum recording of 2 hours
    this.sessionSavingTimeout;


    // online session data
    this.team_data = {
      "avg_distance": 0,
      "avg_hr": 0,
      "speed_zones": [], 
      "hr_zones": []
    };

    this.session_data = {
      "team": this.team_data, // team data
      "players": [], // player_data
      "duration": null, // session duration
      "name": null,  // session name
    };
    
    // online exercise data
    this.exercise_data_start = {
      "start": null,
      "avg_distance": 0,
      "speed_zones": [],
      "hr_zones": [],
    }

    this.exercise_data = {
      "avg_distance": 0,
      "avg_hr": 0,
      "speed_zones": [],
      "hr_zones": [],
      "duration": null
    }

    this.initCons();
  }


  /* initialize cons and watch for new messages */
  initCons() {
    this.players.forEach((p) => {

      // create new connection for a tag
      let tagConnection = new TagConnection(p.tag_id, p.player_id, this.server, this.port, this.api_key);
      this.cons.push(tagConnection);  // keep all the tag connections
    })

    // get distances every x seconds
    this.saveSessionDataBatchInterval = setInterval(() => {
      this.saveSessionData();
    }, data_sampling_freq)

    this.initOnlineSessionData();
    this.closeSessionTimeout();
  }

  /* close recording after the specified timeout */
  closeSessionTimeout() {
    setTimeout(() => {
      let end = new Date()
        .toISOString()
        .slice(0, 19)
        .replace('T', ' ');

      // close and save exercise if running
      if (this.exercise_id !== null) {
        saveExercise(end, this.exercise_id, session_id, (insertId) => {
          this.closeCons();
          removeConnection(this.session_id);
        });
      } else {
        this.closeCons();
        removeConnection(this.session_id);
      }
    }, SESSION_TIMEOUT);
  }

  /* initialize online data structures */
  initOnlineSessionData() {
    // speed zones
    for (let i = 0; i < 4; i++) {
      this.session_data.team.speed_zones.push(new playerData.zone(SPEED_ZONES[i]));
    };

    // hr zones
    for (let i = 0; i < 6; i++) {
      this.session_data.team.hr_zones.push(new playerData.zone(HR_ZONES[i]))
    }

    let promise_players = [];

    // Player structures
    promise_players = new Promise((resolve, reject) => {
      let sql = 'SELECT DISTINCT playerId from record WHERE sessionId = ?';
      con.query(sql, this.session_id, (err, rows) => {
        if (err) {
          console.log("Error getting record data.", err);
          reject();
        } else {
          rows.forEach((r) => {
            this.session_data.players.push(new playerData.playerData(r.playerId));
          });

          // prepare speed zones
          this.session_data.players.forEach((p, j) => {
            for (let i = 0; i < 4; i++) {
              this.session_data.players[j].speed_zones.push(new playerData.zone(SPEED_ZONES[i]));
            }
          });

          // prepare hr zones
          this.session_data.players.forEach((p, j) => {
            for (let i = 0; i < 6; i++) {
              this.session_data.players[j].hr_zones.push(new playerData.zone(HR_ZONES[i]))
            }
          });

          resolve();
        }
      });
    });

    promise_players.then(() => {
      // PLAYERS DATA - firstname, lastname, tag_id, age
      this.session_data.players.forEach((p, i) => {
        let sql = 'SELECT * from player WHERE player_id = ?';
        con.query(sql, p.player_id, (err, rows) => {
          if (err) {
            console.log("Error getting players.", err);
          } else {
            this.session_data.players[i].firstname = rows[0].firstname;
            this.session_data.players[i].lastname = rows[0].lastname;
            this.session_data.players[i].tag_id = rows[0].tag_id;

            // get the age
            let birth_year = rows[0].birth.substring(0, 4); // get the year
            let curr_year = new Date().getFullYear();
            this.session_data.players[i].age = curr_year - birth_year;
          }
        });
      });
    });
  }

  /* save distances from all tags every x seconds */
  saveSessionData() {
    this.cons.forEach((tc) => {

      // don't save if the tag is offline
      if (tc.getStatus() === true) {
        let sessionId = parseInt(this.session_id);
        let playerId = tc.getPlayerId();

        let sp = {
          "record_id": null,
          "sessionId": sessionId,
          "playerId": playerId,
          "timestamp": Date.now(),
          "distance": tc.getDistance(),
          "speed": tc.getSpeed(),
          "hr": tc.getHR()
        }

        let query = 'INSERT INTO record SET ?';
        con.query(query, sp, (err, results) => {
          if (err) {
            console.log("Error editing session data.", err);
          }
        });
      }
    })
  }

  /* close all connections and stop session batches */
  closeCons() {
    clearInterval(this.saveSessionDataBatchInterval);

    this.cons.forEach((tc) => {  // TagConnection
      tc.close();
    });
  }

  /* get session_id */
  getSessionId() {
    return this.session_id;
  }

  /* get exercise_id */
  getExerciseId() {
    return this.exercise_id;
  }

  /* get users email */
  getEmail() {
    return this.email;
  }

  /* set new exercise */
  setExercise(exercise_id) {
    this.exercise_id = exercise_id;

    this.initOnlineExerciseData();
  }

  /* init online exercise data */
  initOnlineExerciseData() {
    this.exercise_data_start.speed_zones = [];
    this.exercise_data.speed_zones = [];
    this.exercise_data_start.hr_zones = [];
    this.exercise_data.hr_zones = [];

    for (let i = 0; i < 4; i++) {
      this.exercise_data_start.speed_zones.push(new playerData.zone(SPEED_ZONES[i]));
      this.exercise_data.speed_zones.push(new playerData.zone(SPEED_ZONES[i]));
    };
    for (let i = 0; i < 6; i++) {
      this.exercise_data_start.hr_zones.push(new playerData.zone(HR_ZONES[i]));
      this.exercise_data.hr_zones.push(new playerData.zone(HR_ZONES[i]));
    }

    // start
    this.exercise_data_start.start = Date.now();

    // avg_distance
    this.exercise_data_start.avg_distance = this.session_data.team.avg_distance;

    // zones
    for (let i = 0; i < 4; i++) {
      this.exercise_data_start.speed_zones[i].count = this.session_data.team.speed_zones[i].count;
      this.exercise_data_start.speed_zones[i].percentage = this.session_data.team.speed_zones[i].percentage;
    }
    for (let i = 0; i < 6; i++) {
      this.exercise_data_start.hr_zones[i].count = this.session_data.team.hr_zones[i].count;
      this.exercise_data_start.hr_zones[i].percentage = this.session_data.team.hr_zones[i].percentage;
    }
  }

  /* close exercise */
  closeExercise() {
    this.exercise_id = null;
  }
}


/* one player connection */
class TagConnection {

  constructor(tag_id, player_id, server, port, api_key) {
    this.tag_id = tag_id;
    this.player_id = player_id;
    this.server = server;
    this.port = port;
    this.api_key = api_key;
    this.status = true;  // if the tag is online

    this.ws;

    // distance measurement
    this.firstValue = true;
    this.distance = 0;  // calculated total distance
    this.x1;
    this.y1;
    this.x2;
    this.y2;

    // speed measurement
    this.t1;
    this.t2;
    this.speed = 0; // calculated speed

    // hr measurement
    this.hr = 0;

    this.subscribe();
    this.message();
  }

  /* subscribe to a given tag on open */
  subscribe() {
    this.ws = new WebSocket(`ws://${this.server}:${this.port}`);

    this.ws.onopen = () => this.ws.send(
      JSON.stringify({
        "headers": {
           "X-ApiKey": this.api_key
        },
        "method": "subscribe",
        "resource":"/feeds/" + this.tag_id
      })
    );

    this.ws.onerror = (err) => {
      console.log("Caught flash policy server socket error.")
    }
  }

  /* on new message */
  message() {
    // DATA RECORDING VARS //
    let batch = {"data": []};
    let i = 0;

    this.ws.onmessage = (res) => {
      
      /* ------ DATA RECORDING ------------------- */
      /* - uncomment when rec simulator data ----- */
      /* ----------------------------------------- */
      // let temp = JSON.parse(res.data);
      // let timestamp = temp.body.datastreams[0].at;
      // let hr = Math.floor(Math.random() * 120) + 60;  // 60 - 180 HR

      // temp.body.datastreams.splice(3, 0, {
      //   "id":"hr",
      //   "current_value": hr.toString(),
      //   "at": timestamp
      // });
      // // console.log(temp.body);
      
      // batch.data.push(temp);
      // i++;
      // // console.log(batch[0]);

      // if (i >= 250) { // cca 1 minute
      //   i = 0;
      //   var fs = require('fs');
      //   fs.appendFile('./rawdata_d16.js', JSON.stringify(batch), (err) => {
      //     if (err) {
      //       console.log(err)
      //     }
      //     else {
      //       console.log('writing!');
      //     }
      //   });
      // }

      /* ----------------------------------------- */
      /* ----------------------------------------- */

      let data = JSON.parse(res.data);  // parse the ws data

      // check if the tag is connected
      // if not, don't send any data
      let result = JSON.parse(res.data);
      if (result.data === false) {
        this.status = false;  // the tag is not connected
        return;
      }

      this.status = true; // the tag is ok and connected

      this.calcData(data);  // calculate distance
    };
  }

  /* measure distance, speed and set current HR*/
  calcData(data) {
    // extract the current HR from the JSON object
    this.hr = parseInt(data.body.datastreams[3].current_value);

    // A2 [x2, y2], timestamp
    this.x2 = parseFloat(data.body.datastreams[0].current_value);
    this.y2 = parseFloat(data.body.datastreams[1].current_value);
    this.t2 = moment(data.body.datastreams[0].at);

    // if the first points are measured, start from zero distance
    if (this.firstValue === true) { 
      this.x1 = this.x2; this.y1 = this.y2; // distance
      this.t1 = this.t2;  // time
      this.t2+= 0.001;  // avoid deviding by ZERO
      this.firstValue = false  // switchflag
    };

    // distance A1 <---> A2 ==> += ==> total distance
    let x1_x2 = Math.sqrt(Math.pow((this.x1 - this.x2), 2) + Math.pow((this.y1 - this.y2), 2));
    this.distance += x1_x2;
    
    // calculate timestamp difference
    let t1_t2 = this.t2 - this.t1;  // ms difference
    t1_t2 = t1_t2 / 1000; // convert to seconds
    
    // prevent getting wrong data - there is some inaccurate data, causing the speed to go like 150m/s+
    let speed = x1_x2 / t1_t2; // calculate speed
    if (speed < MAX_SPEED) {
      this.speed = speed;
    }

    // save the current point as the previous one for the next iteration
    this.x1 = this.x2;
    this.y1 = this.y2;
    this.t1 = this.t2;
  }

  /* get tag's distance */
  getDistance() {
    return this.distance;
  }

  /* get tag's speed */
  getSpeed() {
    return this.speed;
  }

  /* get tag's hr */
  getHR() {
    return this.hr;
  }

  /* get player_id */
  getPlayerId() {
    return this.player_id;
  }

  /* get tag status */
  getStatus() {
    return this.status;
  }

  /* close the tag connection */
  close() {
    this.ws.close();
  }
}


/* 
 * Check first, if the connection for the sessions doesn't already exist,
 * if it does, return "warning message" and don't do anything.
 */
let checkIfExists = (sessionId, email) => (
  ((SessionsCons.find((wsc) => wsc.getSessionId() === sessionId &&
    wsc.getEmail() === email)) !== undefined) ? 
  true : false
)


/* 
 * Get players for the corresponding session,
 * for every incoming connection (different email account), create a new instance
 * of a connection.
 */
exports.connect = (req, res) => {
  let sessionId = req.params.id;
  let email = req.session.email;

  if (checkIfExists(sessionId, email) === true) { // already exists
    
    // check if exercise is being recorded
    if (req.session.exercise) {

      // get exercise name
      con.query('SELECT * FROM exercise WHERE exercise_id = ?', req.session.exercise, (err, rows) => {
        if (err) {
          console.log("Error getting exercise name.", err);
        }
        else {
          response.data = {exercise: rows[0]}
          res.json(response);
        }
      })
    } else {  // exercise does not exist
      response.data = {exercise: null};
      res.json(response);
    }
  }
  else {  // start new session
    con.query('SELECT * from record WHERE sessionId = ?', sessionId, (err, rows) => {
      if (err) {
        console.log("Error getting session players.", err);
      } else {
        let players = [];
        let promises = [];

        for (let p of rows) {
          promises.push(new Promise((resolve, reject) => {
            con.query('SELECT * from player WHERE player_id = ?', p.playerId, (err, rows) => {
              if (err) {
                console.log("Error getting session players detail.", err)
                reject();
              } else {
                players.push(rows[0]);
                resolve();
              }
            });
          }));
        }

        Promise.all(promises).then(() => {
          // create new session connection instance
          let wsc = new SessionConnection(req.session.email, sessionId, players, req.session.SERVER, req.session.PORT, req.session.API_KEY);
          SessionsCons.push(wsc);  // keep all the sessions
        });
      }
    })

    response.data = {exercise: null};
    res.json(response);
  }
}


/*
 * Disconnect all ws in the given session.
 */
exports.disconnect = (req, res) => {
  let session_id = req.params.id;

  // find the connections that we want to close,
  // among the connections we have and close it
  let SessionCon = SessionsCons.find((wsc) => wsc.getSessionId() === session_id);
  removeConnection(session_id);

  SessionCon.closeCons();
  
  response.data = {msg: "disconnected!"};
  res.json(response);
}


/*
 * Remove session from the array of sessions.
 */
let removeConnection = (session_id) => {
  SessionsCons = SessionsCons.filter((wsc) => wsc.getSessionId() !== session_id);
}


/*
 * Get tag status.
 * Returns if online of offline.
 */
exports.getTagStatus = (req, res) => {
  let tag_id = req.params.id;

  let ws = new WebSocket(`ws://${req.session.SERVER}:${req.session.PORT}`);

  ws.onopen = () => ws.send(
    JSON.stringify({
      "headers": {
         "X-ApiKey": req.session.API_KEY
      },
      "method": "subscribe",
      "resource":"/feeds/" + tag_id
    })
  );

  // message received = send to to angular to determine what it is (online || offline)
  ws.onmessage = (res) => {
    ws.close(); // aaand close it
    returnStatus(res);
  };

  ws.onerror = (err) => {
    response.data = {data: false};
    res.json(response);
  };

  function returnStatus(resp) {
    response.data = JSON.parse(resp.data);
    res.json(response);
  }
}


/*
 * Get current recording for the given account - session_id and exercise_id, if exist.
 */
exports.getRecording = (req, res) => {
  let email = req.session.email;

  // find the connection and then get the session_id from it
  let SessionCon = SessionsCons.find((wsc) => wsc.getEmail() === email);
  
  // if exists, otherwise send empty string
  let session_id = "";
  if (SessionCon) {
    session_id = SessionCon.getSessionId();
  }

  response.data = {session_id};
  res.json(response);
}


/* get various data from the current session and calculate averages */
exports.getSessionData = (req, res) => {
  let sessionId = req.params.id;

  // current session instance
  let SC = SessionsCons.find(
    (wsc) => Number(wsc.getSessionId()) === req.session.session);

  let promises_data = [];


  // SESSION - duration, name
  promises_data.push(new Promise((resolve, reject) => {

    con.query('SELECT * from session WHERE session_id = ?', sessionId, (err, rows) => {
      if (err) {
        console.log("Error getting session data.", err)
        reject();
      } else {
        let session = rows[0];

        SC.session_data.name = session.name;
        SC.session_data.duration = moment().subtract(2, 'h') - moment(session.start); // ms
        resolve();
      }
    })
  }));


  // PLAYERS - sprint_distance
  promises_data.push(new Promise((resolve, reject) => {
    let sql = 'SELECT SUM(speed) as sum_sprint_distance, playerId FROM record WHERE (sessionId = ? AND speed >= ?) GROUP BY playerId';
    con.query(sql, [sessionId, SPRINT_THRESHOLD], (err, rows) => {
      if (err) {
        console.log("Error getting record data.", err);
        reject();
      } else {
        if (rows[0] !== undefined) {
          SC.session_data.players.forEach((p, i) => {
            for (let j = 0; j < rows.length; j++) {
              if (rows[j].playerId === p.player_id)
                SC.session_data.players[i].sprint_distance = rows[j].sum_sprint_distance; // s = v * t; T is 1000ms = 1 second in this case = 1!
            }
          });
        }
        resolve();
      }
    });
  }));


  // PLAYERS, TEAM - hr_zones
  promises_data.push(new Promise((resolve, reject) => {

    let players_in_zones = [0,0,0,0,0,0]; // 5 hr zones

    // null structs
    SC.session_data.players.forEach((p, k) => {
      for (let i = 0; i < SC.session_data.team.hr_zones.length-1; i++) {
        SC.session_data.players[k].hr_zones[i].count = 0;
        SC.session_data.team.hr_zones[i].count = 0;
      }
    });

    let promises_hr_zones = [];

    // hr team = hr_sum / data_count
    let data_count = 0; // number of all data for hr team avg
    let hr_sum = 0; // total sum of all hrs

    // for all ranges (rewrite to forEach func)
    for (let i = 0; i < SC.session_data.team.hr_zones.length-1; i++) {

      promises_hr_zones.push(new Promise((resolve, reject) => {
        let r1 = SC.session_data.team.hr_zones[i].value;
        let r2 = SC.session_data.team.hr_zones[i+1].value;

        let sql = 'SELECT COUNT(hr) as p_hr_count, playerId FROM record WHERE (sessionId = ? AND hr BETWEEN ? AND ?) GROUP BY playerId';
        con.query(sql, [sessionId, r1, r2], (err, rows) => {
          if (err) {
            console.log("Error getting session_hr data.", err);
            reject();
          } else {
            if (rows[0] !== undefined) {
              players_in_zones[i] = rows.length;
              // save count (needed, if the saving rate isn't 1000ms)
              SC.session_data.players.forEach((p, k) => {
                for (let j = 0; j < rows.length; j++) {
                  if (rows[j].playerId === p.player_id) {
                    SC.session_data.players[k].hr_zones[i].count = rows[j].p_hr_count; // player
                    SC.session_data.team.hr_zones[i].count += rows[j].p_hr_count;  // team
                  }
                }
              });
            }
            resolve();  // resolve one range
          }
        });
      }));
    };

    // resolve all the hr ranges
    Promise.all(promises_hr_zones).then(() => {
      // team
      let sum_team = 0;
      SC.session_data.team.hr_zones.forEach((z) => {
        sum_team += z.count;  // 100% of values
      });
      SC.session_data.team.hr_zones.forEach((z, i) => {
        SC.session_data.team.hr_zones[i].percentage = z.count * 100 / sum_team;
        if (players_in_zones[i] === 0) z.count = z.count;
        else z.count = z.count / players_in_zones[i];
      });

      // players
      SC.session_data.players.forEach((p, i) => {
        let sum_player = 0;

        SC.session_data.players[i].hr_zones.forEach((z) => {
          sum_player += z.count;
        });

        SC.session_data.players[i].hr_zones.forEach((z, j) => {
          SC.session_data.players[i].hr_zones[j].percentage = z.count * 100 / sum_player;
        });
      });
      resolve();
    });
  }));


  // PLAYERS, TEAM - speed_zones
  promises_data.push(new Promise((resolve, reject) => {

    let players_in_zones = [0,0,0,0]; // 5 hr zones

    // null structs
    SC.session_data.players.forEach((p, k) => {
      for (let i = 0; i < SC.session_data.team.speed_zones.length-1; i++) {
        SC.session_data.players[k].speed_zones[i].count = 0;
        SC.session_data.team.speed_zones[i].count = 0;
      }
    });

    let promises_speed_zones = [];

    // for all ranges (rewrite to forEach func)
    for (let i = 0; i < SC.session_data.team.speed_zones.length-1; i++) {

      promises_speed_zones.push(new Promise((resolve, reject) => {
        let r1 = SC.session_data.team.speed_zones[i].value;
        let r2 = SC.session_data.team.speed_zones[i+1].value;

        let sql = 'SELECT COUNT(speed) as p_speed_count, playerId FROM record WHERE (sessionId = ? AND speed BETWEEN ? AND ?) GROUP BY playerId';
        con.query(sql, [sessionId, r1, r2], (err, rows) => {
          if (err) {
            console.log("Error getting session_speed data.", err);
            reject();
          } else {
            if (rows[0] !== undefined) {
              players_in_zones[i] = rows.length;
              // save count (needed, if the saving rate isn't 1000ms)
              SC.session_data.players.forEach((p, k) => {
                for (let j = 0; j < rows.length; j++) {
                  if (rows[j].playerId === p.player_id) {
                    SC.session_data.players[k].speed_zones[i].count = rows[j].p_speed_count; // player
                    SC.session_data.team.speed_zones[i].count += rows[j].p_speed_count;  // team
                  }
                }
              });
            }

            resolve();  // resolve one range
          }
        });
      }));
    };

    // resolve all the speed ranges
    Promise.all(promises_speed_zones).then(() => {

      // team
      let sum_team = 0;
      SC.session_data.team.speed_zones.forEach((z) => {
        sum_team += z.count;  // 100% of values
      });
      SC.session_data.team.speed_zones.forEach((z, i) => {
        SC.session_data.team.speed_zones[i].percentage = z.count * 100 / sum_team;
        if (players_in_zones[i] === 0) z.count = z.count;
        else z.count = z.count / players_in_zones[i];
      });

      // players
      SC.session_data.players.forEach((p, i) => {
        let sum_player = 0;

        SC.session_data.players[i].speed_zones.forEach((z) => {
          sum_player += z.count;
        });

        SC.session_data.players[i].speed_zones.forEach((z, j) => {
          SC.session_data.players[i].speed_zones[j].percentage = z.count * 100 / sum_player;
        });
      });
      resolve();
    });
  }));


  // PLAYERS - hr_curr, hr_percentage
  promises_data.push(new Promise((resolve, reject) => {

    // for each player in the session, get his current hr
    let sc = SessionsCons.find((sc) => sc.getSessionId() === sessionId)
    sc.cons.forEach((tc) => { // for each tag_connection
      for (let j in SC.session_data.players) {
        if (SC.session_data.players[j].player_id === tc.player_id) {
          SC.session_data.players[j].hr_curr = tc.getHR();
          SC.session_data.players[j].hr_percentage = tc.getHR() / (HR_INIT - SC.session_data.players[j].age) * 100;
        }
      }
    });
    resolve();
  }));


  // PLAYERS, TEAM - distance, avg_distance
  promises_data.push(new Promise((resolve, reject) => {

    let promises_players = [];

    let active_players_counter = 0;

    SC.session_data.players.forEach((p, i) => {

      promises_players.push(new Promise((resolve, reject) => {
        let sql = 'SELECT distance FROM record WHERE (sessionId = ? AND playerID = ?) ORDER BY record_id DESC LIMIT 1';
        con.query(sql, [sessionId, p.player_id], (err, rows) => {
          if (err) {
            console.log("Error getting players.", err);
            reject();
          } else {
            // distance for each player
            if (rows[0] !== undefined && rows[0].distance !== null) {
              SC.session_data.players[i].distance = rows[0].distance; 
              active_players_counter++; 
            }
            else SC.session_data.players[i].distance = 0;

            resolve();
          }
        })
      }));
    });

    // resolve all players data
    Promise.all(promises_players).then(() => {
      // compute team average
      SC.session_data.team.avg_distance = 0;
      SC.session_data.players.forEach(p => {
        SC.session_data.team.avg_distance += p.distance;
      });

      // average only active players with non-zero distance
      if (active_players_counter > 0) 
        SC.session_data.team.avg_distance = SC.session_data.team.avg_distance / active_players_counter;
      else SC.session_data.team.avg_distance = 0;

      resolve();
    })
  }));


  // TEAM - avg_hr
  promises_data.push(new Promise((resolve, reject) => {
    let sql = 'SELECT AVG(hr) as avg_hr FROM record WHERE sessionId = ?';
    con.query(sql, sessionId, (err, rows) => {
      if (err) {
        console.log("Error getting record data.", err);
        reject();
      } else {
        if (rows[0] !== undefined && rows[0].avg_hr !== null) SC.session_data.team.avg_hr = rows[0].avg_hr;
        else SC.session_data.team.avg_hr = 0;

        resolve();
      }
    });
  }));


  // now we have all the data ready, send it all!
  Promise.all(promises_data).then(() => {
    response.data = SC.session_data;
    res.json(response);
  });
}


/*
 * Add a new exercise.
 */
exports.addExercise = (req, res) => {
  let start = new Date()
    .toISOString()
    .slice(0, 19)
    .replace('T', ' ');


  let exercise = new _exercise(
    null,
    req.body.exercise.name,
    start,
    null,
    req.body.exercise.session_id
  );

  con.query('INSERT INTO exercise SET ?', exercise, (err, results) => {
    if (err) {
      console.log("Error creating new exercise.", err);
    } else {
      // new exercise is being recorded
      req.session.exercise = results.insertId;

      exercise.exercise_id = results.insertId;
      response.data = exercise;

      // insert the exercise_id for the corresponding session
      let SessionCon = SessionsCons.find(
        (wsc) => Number(wsc.getSessionId()) === req.session.session);
      SessionCon.setExercise(req.session.exercise);

      res.json(response);
    }
  });
}


/*
 * End an exercise.
 */
exports.endExercise = (req, res) => {
  let end = new Date()
    .toISOString()
    .slice(0, 19)
    .replace('T', ' ');

  let exercise_id = req.session.exercise;
  let session_id = req.session.session;

  saveExercise(end, exercise_id, session_id, (insertId) => {
    req.session.exercise = null;  // no exercise is being recorded now

    response.data = {exercise_id: insertId};
    res.json(response);
  });
}


/*
 * Save exercise to database.
 */
let saveExercise = (end, exercise_id, session_id, callback) => {
  con.query('UPDATE exercise SET end = ? WHERE exercise_id = ?', [end, exercise_id], (err, results) => {
    if (err) {
      console.log("Error editing session.", err);
      // NO ERROR HERE HANDLE CAUSE OF NOT REQ, RES
    } else {
      // close saving
      let SessionCon = SessionsCons.find(
        (wsc) => Number(wsc.getSessionId()) === session_id);
      SessionCon.closeExercise();

      callback(results.insertId);
    }
  });
}


/*
 * Get currently recorded exercise.
 */
exports.getExercise = (req, res) => {
  let session_id = req.params.id;

  con.query('SELECT * from exercise WHERE session_id = ?', session_id, (err, rows) => {
    if (err) {
      console.log("Error getting exercises.", err);
    } else {
      response.data = rows;
      res.json(response);
    }
  })
}


/* get various data from the current exercise and calculate averages */
// almost the same as the getSessionData function -> maybe write one general for both...
exports.getExerciseData = (req, res) => {

  let SC = SessionsCons.find(
    (wsc) => Number(wsc.getSessionId()) === req.session.session);

  let promises_data = [];

  // EXERCISE - duration
  promises_data.push(new Promise((resolve, reject) => {

    con.query('SELECT start from exercise WHERE exercise_id = ?', req.session.exercise, (err, rows, fields) => {
      if (err) {
        console.log("Error getting exercise data.", err)
      } else {
        SC.exercise_data.duration = moment().subtract(2, 'h') - moment(rows[0].start); // ms

        resolve();
      }
    })
  }));


  // TEAM - avg_distance
  promises_data.push(new Promise((resolve, reject) => {
    if (isNaN(SC.session_data.team.avg_distance)) SC.session_data.team.avg_distance = 0;
    if (isNaN(SC.exercise_data_start.avg_distance)) SC.exercise_data_start.avg_distance = 0;

    SC.exercise_data.avg_distance = SC.session_data.team.avg_distance - SC.exercise_data_start.avg_distance;
    resolve();
  }));


  // TEAM - speed_zones
  promises_data.push(new Promise((resolve, reject) => {
    for (let i = 0; i < 4; i++) {
      SC.exercise_data.speed_zones[i].count = SC.session_data.team.speed_zones[i].count - SC.exercise_data_start.speed_zones[i].count; 
    }

    // count percetages
    // team
    let sum_team = 0;
    SC.exercise_data.speed_zones.forEach((z) => {
      sum_team += z.count;  // 100% of values
    });
    SC.exercise_data.speed_zones.forEach((z, i) => {
      SC.exercise_data.speed_zones[i].percentage = z.count * 100 / sum_team;
    });
    
    resolve();
  }));


  // TEAM - hr_zones
  promises_data.push(new Promise((resolve, reject) => {
    for (let i = 0; i < 6; i++) {
      SC.exercise_data.hr_zones[i].count = SC.session_data.team.hr_zones[i].count - SC.exercise_data_start.hr_zones[i].count;
    }

    // count percetages
    // team
    let sum_team = 0;
    SC.exercise_data.hr_zones.forEach((z) => {
      sum_team += z.count;  // 100% of values
    });
    SC.exercise_data.hr_zones.forEach((z, i) => {
      SC.exercise_data.hr_zones[i].percentage = z.count * 100 / sum_team;
    });
    
    resolve();
  }));


  // TEAM - avg_hr
  promises_data.push(new Promise((resolve, reject) => {

    let sql = 'SELECT AVG(hr) as avg_hr FROM record WHERE (sessionId = ? AND timestamp > ?)';
    con.query(sql, [req.session.session, SC.exercise_data_start.start], (err, rows) => {
      if (err) {
        console.log("Error getting record data.", err)
        reject();
      } else {
        if (rows[0] !== undefined && rows[0].avg_hr !== null) SC.exercise_data.avg_hr = rows[0].avg_hr;
        else SC.exercise_data.avg_hr = 0;

        resolve();
      }
    });
  }));


  // now we have all the data ready
  Promise.all(promises_data).then(() => {
    
    response.data = SC.exercise_data;
    res.json(response);
  });
}