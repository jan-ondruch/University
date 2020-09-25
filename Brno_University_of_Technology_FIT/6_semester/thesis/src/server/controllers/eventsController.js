const con = require('../config/connection');
const WebSocket = require('ws');
var moment = require('moment');

const { HR_ZONES, SPEED_ZONES, HR_MAX, HR_INIT, SPRINT_THRESHOLD } = require('../models/sessionData');
const playerDataModel = require('../models/playerData');

let response = {
    status: 200,
    data: []
}

/*
 * Get basic data from the last event and calculate averages.
 */
exports.eventData = (req, res) => {
  let sessionId = req.params.id;

  // metrics for one player
  function exercise(exercise_id) {
    this.exercise_id = exercise_id;
    this.distance = 0;
    this.sprint_distance = 0;
    this.speed_zones = [];
    this.hr_zones = [];
    this.relative_hr = 0;
    this.avg_hr = 0;
  };

  // metrics for the whole team
  function exercise_data(exercise_id, name, duration, start, end) {
    this.exercise_id = exercise_id;
    this.name = name;
    this.duration = duration;
    this.start = start;
    this.end = end;
    this.avg_distance = 0;
    this.avg_hr = 0;
    this.relative_hr = 0;
    this.speed_zones = [];
    this.hr_zones = [];
  };

  function playerData(player_id) {
    this.player_id = player_id;
    this.firstname = null;
    this.lastname = null;
    this.age = 0;
    this.tag_id = null;
    this.distance = 0;
    this.avg_hr = 0;
    this.relative_hr = 0;
    this.sprint_distance = 0;
    this.speed_zones = [];
    this.hr_zones = [];
    this.exercises = []; 
  };

  // metrics for the whole team
  let session_data = {
    "name": null,
    "duration": null,
    "players_count": 0,
    "avg_hr": 0,
    "relative_hr": 0,
    "avg_distance": 0,
    "avg_sprint_distance": 0,
    "speed_zones": [],
    "hr_zones": []
  }

  // move this to data model file
  let event_data = {
    "session": session_data, // session data
    "players": [], // players data
    "exercises": [] // summary of the whole team
  }


  // speed zones
  for (let i = 0; i < 4; i++) {
    event_data.session.speed_zones.push(new playerDataModel.zone(SPEED_ZONES[i]));
  };

  for (let i = 0; i < 6; i++) {
    event_data.session.hr_zones.push(new playerDataModel.zone(HR_ZONES[i]))
  }

  // PLAYERS - player_id, distance, players_count
  let players_data_promise = [];

  players_data_promise.push(new Promise((resolve, reject) => {
    let sql = 'SELECT DISTINCT playerId from record WHERE sessionId = ?';
    con.query(sql, sessionId, (err, rows) => {
      if (err) {
        console.log("Error getting record data.", err);
        reject();
      } else {
        rows.forEach((r) => {
          event_data.players.push(new playerData(r.playerId));
        });

        event_data.players.forEach((p, j) => {
          for (let i = 0; i < 4; i++) {
            event_data.players[j].speed_zones.push(new playerDataModel.zone(SPEED_ZONES[i]));
          }

          for (let i = 0; i < 6; i++) {
            event_data.players[j].hr_zones.push(new playerDataModel.zone(HR_ZONES[i]))
          }
        })

        event_data.session.players_count = event_data.players.length;
        resolve();
      }
    });
  }));

  players_data_promise.push(new Promise((resolve, reject) => {
    // get all data from exercises
    let sql = 'SELECT * from exercise WHERE session_id = ?';
    con.query(sql, sessionId, (err, rows) => {
      if (err) {
        console.log("Error getting exercises.", err);
      } else {
        rows.forEach((r) => {
          let duration = moment(r.end) - moment(r.start);

          // +2 hours... but now is summer time, so in winter it will be fucked prolly...
          let start = r.start.getTime() + 2*60*60*1000;
          let end = r.end.getTime() + 2*60*60*1000;

          // save for the whole exercise in general, also for players
          event_data.exercises.push(new exercise_data(r.exercise_id, r.name, duration, start, end));

          // set hr and speed zones for exercises
          for (let i = 0; i < 4; i++) {
            event_data.exercises[event_data.exercises.length-1].speed_zones.push(new playerDataModel.zone(SPEED_ZONES[i]));
          }
          for (let i = 0; i < 6; i++) {
            event_data.exercises[event_data.exercises.length-1].hr_zones.push(new playerDataModel.zone(HR_ZONES[i]))
          }

          event_data.players.forEach((p) => {
            p.exercises.push(new exercise(r.exercise_id));
          });
        });

        // save speed_zones
        event_data.players.forEach((p, j) => {
          p.exercises.forEach((e, k) => {
            for (let i = 0; i < 4; i++) {
              event_data.players[j].exercises[k].speed_zones.push(new playerDataModel.zone(SPEED_ZONES[i]));
            }

            for (let i = 0; i < 6; i++) {
              event_data.players[j].exercises[k].hr_zones.push(new playerDataModel.zone(HR_ZONES[i]))
            }
          });
        });

        resolve();
      }
    })
  }));



  // player structures are ready, we can start filling in more data
  Promise.all(players_data_promise).then(() => {

    let promises_data = [];

    // SESSION - name, duration
    promises_data.push(new Promise((resolve, reject) => {

      con.query('SELECT * from session WHERE session_id = ?', sessionId, (err, rows) => {
        if (err) {
          console.log("Error getting session data.", err);
          reject();
        } else {
          let session = rows[0];

          event_data.session.name = session.name;
          event_data.session.duration = moment(session.end) - moment(session.start); // ms
          
          resolve();
        }
      })
    }));


    // PLAYERS, TEAM - speed_zones
    promises_data.push(new Promise((resolve, reject) => {

      let promises_speed_zones = [];
      let players_in_zones = [0,0,0,0]; // 3 speed zones

      for (let i = 0; i < event_data.session.speed_zones.length-1; i++) {

        promises_speed_zones.push(new Promise((resolve, reject) => {
          let r1 = event_data.session.speed_zones[i].value;
          let r2 = event_data.session.speed_zones[i+1].value;

          let sql = 'SELECT COUNT(speed) as p_speed_count, playerId FROM record WHERE (sessionId = ? AND speed BETWEEN ? AND ?) GROUP BY playerId';
          con.query(sql, [sessionId, r1, r2], (err, rows) => {
            if (err) {
              console.log("Error getting session_speed data.", err);
              reject();
            } else {
              if (rows[0] !== undefined) {
                players_in_zones[i] = rows.length;
                // save count (needed, if the saving rate isn't 1000ms)
                event_data.players.forEach((p, k) => {
                  for (let j = 0; j < rows.length; j++) {
                    if (rows[j].playerId === p.player_id) {
                      event_data.players[k].speed_zones[i].count = rows[j].p_speed_count; // player
                      event_data.session.speed_zones[i].count += rows[j].p_speed_count;  // team
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
        event_data.session.speed_zones.forEach((z) => {
          sum_team += z.count;  // 100% of values
        });
        event_data.session.speed_zones.forEach((z, i) => {
          event_data.session.speed_zones[i].percentage = z.count * 100 / sum_team;
          z.count = z.count / players_in_zones[i];
        });

        // players
        event_data.players.forEach((p, i) => {
          let sum_player = 0;

          event_data.players[i].speed_zones.forEach((z) => {
            sum_player += z.count;
          });

          event_data.players[i].speed_zones.forEach((z, j) => {
            event_data.players[i].speed_zones[j].percentage = z.count * 100 / sum_player;
          });
        });
        resolve();
      });
    }));


    // PLAYERS, TEAM - hr_zones
    promises_data.push(new Promise((resolve, reject) => {

      let promises_hr_zones = [];
      let players_in_zones = [0,0,0,0,0,0]; // 5 hr zones

      for (let i = 0; i < event_data.session.hr_zones.length-1; i++) {

        promises_hr_zones.push(new Promise((resolve, reject) => {
          let r1 = event_data.session.hr_zones[i].value;
          let r2 = event_data.session.hr_zones[i+1].value;

          let sql = 'SELECT COUNT(hr) as p_hr_count, playerId FROM record WHERE (sessionId = ? AND hr BETWEEN ? AND ?) GROUP BY playerId';
          con.query(sql, [sessionId, r1, r2], (err, rows) => {
            if (err) {
              console.log("Error getting session_hr data.", err);
              reject();
            } else {
              if (rows[0] !== undefined) {
                players_in_zones[i] = rows.length;
                // save count (needed, if the saving rate isn't 1000ms)
                event_data.players.forEach((p, k) => {
                  for (let j = 0; j < rows.length; j++) {
                    if (rows[j].playerId === p.player_id) {
                      event_data.players[k].hr_zones[i].count = rows[j].p_hr_count; // player
                      event_data.session.hr_zones[i].count += rows[j].p_hr_count;  // team
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
        event_data.session.hr_zones.forEach((z) => {
          sum_team += z.count;  // 100% of values
        });
        event_data.session.hr_zones.forEach((z, i) => {
          event_data.session.hr_zones[i].percentage = z.count * 100 / sum_team;
          z.count = z.count / players_in_zones[i];
        });

        // players
        event_data.players.forEach((p, i) => {
          let sum_player = 0;

          event_data.players[i].hr_zones.forEach((z) => {
            sum_player += z.count;
          });

          event_data.players[i].hr_zones.forEach((z, j) => {
            event_data.players[i].hr_zones[j].percentage = z.count * 100 / sum_player;
          });
        });
        resolve();
      });
    }));


    // PLAYERS DATA - firstname, lastname, tag_id, age
    promises_data.push(new Promise((resolve, reject) => {
      
      let promises_players = [];

      event_data.players.forEach((p, i) => {

        promises_players.push(new Promise((resolve, reject) => {
          let sql = 'SELECT * from player WHERE player_id = ?';
          con.query(sql, p.player_id, (err, rows) => {
            if (err) {
              console.log("Error getting players.", err);
            } else {
              if (rows[0] !== undefined) {
                event_data.players[i].firstname = rows[0].firstname;
                event_data.players[i].lastname = rows[0].lastname;
                event_data.players[i].tag_id = rows[0].tag_id;

                // get the age
                let birth_year = rows[0].birth.substring(0, 4); // get the year
                let curr_year = new Date().getFullYear();
                event_data.players[i].age = curr_year - birth_year;
              }

              resolve();
            }
          })
        }));
      });

      // resolve all players data
      Promise.all(promises_players).then(() => {
        resolve();
      })
    }));


    // TEAM - avg_hr, relative_hr
    promises_data.push(new Promise((resolve, reject) => {
      let sql = 'SELECT AVG(hr) as avg_hr FROM record WHERE sessionId = ?';
      con.query(sql, sessionId, (err, rows) => {
        if (err) {
          console.log("Error getting record data.", err);
          reject();
        } else {
          if (rows[0] !== undefined && rows[0].avg_hr !== null) {
            event_data.session.avg_hr = rows[0].avg_hr;
            event_data.session.relative_hr = event_data.session.avg_hr / HR_MAX * 100;
          }
          else {
            event_data.session.avg_hr = 0;
            event_data.session.relative_hr = 0;
          }

          resolve();
        }
      });
    }));


    // PLAYERS, TEAM - distance, avg_distance
    promises_data.push(new Promise((resolve, reject) => {

      let promises_players = [];

      let active_players_counter = 0;

      event_data.players.forEach((p, i) => {

        promises_players.push(new Promise((resolve, reject) => {
          let sql = 'SELECT distance FROM record WHERE (sessionId = ? AND playerID = ?) ORDER BY record_id DESC LIMIT 1';
          con.query(sql, [sessionId, p.player_id], (err, rows) => {
            if (err) {
              console.log("Error getting players.", err);
              reject();
            } else {
              // distance for each player
              if (rows[0] !== undefined && rows[0].distance !== null) {
                event_data.players[i].distance = rows[0].distance; 
                active_players_counter++;
              }
              else event_data.players[i].distance = 0;

              resolve();
            }
          })
        }));
      });

      // resolve all players data
      Promise.all(promises_players).then(() => {
        // compute team average
        event_data.session.avg_distance = 0;
        event_data.players.forEach(p => {
          event_data.session.avg_distance += p.distance;
        });

        // average only active players with non-zero distance
        if (active_players_counter > 0) 
          event_data.session.avg_distance = event_data.session.avg_distance / active_players_counter;
        else event_data.session.avg_distance = 0;

        resolve();
      })
    }));


    // PLAYERS: avg_hr, relative_hr
    promises_data.push(new Promise((resolve, reject) => {

      let promises_players = [];

      let active_players_counter = 0;

      event_data.players.forEach((p, i) => {

        promises_players.push(new Promise((resolve, reject) => {
          let sql = 'SELECT AVG(hr) as avg_hr FROM record WHERE (sessionId = ? AND playerID = ?)';
          con.query(sql, [sessionId, p.player_id], (err, rows) => {
            if (err) {
              console.log("Error getting players.", err);
              reject();
            } else {
              // avg_hr for each player
              if (rows[0] !== undefined && rows[0].avg_hr !== null) {
                event_data.players[i].avg_hr = rows[0].avg_hr;
                event_data.players[i].relative_hr = rows[0].avg_hr / (HR_INIT - event_data.players[i].age) * 100;
                active_players_counter++;
              }

              resolve();
            }
          })
        }));
      });

      // resolve all players data
      Promise.all(promises_players).then(() => {
        resolve();
      })
    }));


    // SESSION: PLAYERS, TEAM - sprint_distance, avg_sprint_distance
    promises_data.push(new Promise((resolve, reject) => {
      let sql = 'SELECT SUM(speed) as sum_sprint_distance, playerId FROM record WHERE (sessionId = ? AND speed >= ?) GROUP BY playerId';
      con.query(sql, [sessionId, SPRINT_THRESHOLD], (err, rows) => {
        if (err) {
          console.log("Error getting record data.", err);
          reject();
        } else {
          let promises_players = [];

          if (rows[0] !== undefined) {
            event_data.players.forEach((p, i) => {

              promises_players.push(new Promise((resolve, reject) => {
                for (let j = 0; j < rows.length; j++) {
                  if (rows[j].playerId === p.player_id)
                    event_data.players[i].sprint_distance = rows[j].sum_sprint_distance; // s = v * t; T is 1000ms = 1 second in this case = 1!
                }
                resolve();
              }));
            });
          } else {
            resolve();
          }

          Promise.all(promises_players).then(() => {
            event_data.players.forEach((p, i) => {
              event_data.session.avg_sprint_distance += p.sprint_distance; 
            });

            if (event_data.session.players_count !== 0) {
              event_data.session.avg_sprint_distance = event_data.session.avg_sprint_distance / event_data.session.players_count;
            } else {
              event_data.session.avg_sprint_distance = 0;
            }
             
            resolve();
          })
        }
      });
    }));



    // EXERCISES: TEAM - avg_hr, relative_hr
    promises_data.push(new Promise((resolve, reject) => {

      let promises_exercise_team = [];

      // no exercises were recorded
      if (!event_data.exercises.length) {
        resolve();
      }

      // get all data from exercise_player
      event_data.exercises.forEach((e, i) => {

        promises_exercise_team.push(new Promise((resolve, reject) => {
          let sql = 'SELECT AVG(hr) as avg_hr FROM record WHERE (sessionId = ? AND timestamp BETWEEN ? AND ?)';
          con.query(sql, [sessionId, e.start, e.end], (err, rows) => {
            if (err) {
              console.log("Error getting record data.", err);
              reject();
            } else {
              if (rows[0] !== undefined && rows[0].avg_hr !== null) {
                event_data.exercises[i].avg_hr = rows[0].avg_hr;
              }
              else event_data.exercises[i].avg_hr = 0;

              resolve();
            }
          });
        }));
      });

      Promise.all(promises_exercise_team).then(() => {

        // get relative hr for team
        for (let m in event_data.exercises) {
          event_data.exercises[m].relative_hr = event_data.exercises[m].avg_hr / HR_MAX * 100;
        }

        resolve();
      });
    }));  // end of promise_data


    // EXERCISES: PLAYERS, TEAM - distance, avg_distance
    promises_data.push(new Promise((resolve, reject) => {

      let promises_exercises = [];

      event_data.players.forEach((p, i) => {
        p.exercises.forEach((e, j) => {

          // start and end of exercise
          // derive them from exercises
          let start;
          let end;

          for (let ex in event_data.exercises) {
            if (event_data.exercises[ex].exercise_id === e.exercise_id) {
              start = event_data.exercises[ex].start;
              end = event_data.exercises[ex].end;
            }
          }

          promises_exercises.push(new Promise((resolve, reject) => {
            // select top value between the timestamps and deduct the last recorded value and the first recorded one
            let sql = 'SELECT (SELECT distance FROM record WHERE (sessionId = ? AND playerId = ? AND (timestamp BETWEEN ? AND ?)) ORDER BY record_id LIMIT 1) as first, (SELECT distance FROM record WHERE (sessionId = ? AND playerId = ? AND (timestamp BETWEEN ? AND ?)) ORDER BY record_id DESC LIMIT 1) as last';
            con.query(sql, [sessionId, p.player_id, start, end, sessionId, p.player_id, start, end], (err, rows) => {
              if (err) {
                console.log("Error getting players.", err);
                reject();
              } else {
                // distance for each player
                if (rows[0] !== undefined && rows[0].first !== null && rows[0].last !== null) {
                  event_data.players[i].exercises[j].distance = rows[0].last - rows[0].first; 
                }
                else event_data.players[i].exercises[j].distance = 0;

                resolve();
              }
            })
          }));

        }); // eoe
      }); // eop

      // resolve all players data
      Promise.all(promises_exercises).then(() => {
        // compute team average
        event_data.players.forEach((p, i) => {
          p.exercises.forEach((e, j) => {

            for (let m in event_data.exercises) {
              // add player exericises to the total sum of each exercise
              if (event_data.exercises[m].exercise_id === event_data.players[i].exercises[j].exercise_id) {
                event_data.exercises[m].avg_distance += event_data.players[i].exercises[j].distance;
              }
            }

          }); // eop
        }); // eoe

        // divide the sum by the number of players
        for (let m in event_data.exercises) {
          event_data.exercises[m].avg_distance = event_data.exercises[m].avg_distance / event_data.session.players_count;
        }

        resolve();
      });
    }));


    // EXERCISES: TEAM - speed_zones
    promises_data.push(new Promise((resolve, reject) => {

      let promises_speed_zones = [];
      let players_in_zones = [0,0,0,0]; // 3 speed zones

      event_data.exercises.forEach((e, j) => {

        // start and end of exercise
        // derive them from exercises
        let start;
        let end;

        for (let ex in event_data.exercises) {
          if (event_data.exercises[ex].exercise_id === e.exercise_id) {
            start = event_data.exercises[ex].start;
            end = event_data.exercises[ex].end;
          }
        }

        for (let i = 0; i < event_data.session.speed_zones.length-1; i++) {

          promises_speed_zones.push(new Promise((resolve, reject) => {
            let r1 = event_data.exercises[j].speed_zones[i].value;
            let r2 = event_data.exercises[j].speed_zones[i+1].value;

            let sql = 'SELECT COUNT(speed) as p_speed_count FROM record WHERE (sessionId = ? AND speed BETWEEN ? AND ? AND timestamp BETWEEN ? AND ?)';
            con.query(sql, [sessionId, r1, r2, start, end], (err, rows) => {
              if (err) {
                console.log("Error getting session_speed data.", err);
                reject();
              } else {
                if (rows[0] !== undefined) {
                  players_in_zones[i] = rows.length;
                  // save count (needed, if the saving rate isn't 1000ms)
                  event_data.exercises[j].speed_zones[i].count = rows[0].p_speed_count;  // team
                }

                resolve();  // resolve one range
              }
            });
          }));
        };
      }); // end of forEach

      // resolve all the speed ranges
      Promise.all(promises_speed_zones).then(() => {
        // team
        event_data.exercises.forEach((e, j) => {
          let sum_team = 0;
          event_data.exercises[j].speed_zones.forEach((z) => {
            sum_team += z.count;  // 100% of values
          });
          event_data.exercises[j].speed_zones.forEach((z, i) => {
            event_data.exercises[j].speed_zones[i].percentage = z.count * 100 / sum_team;
            z.count = z.count / players_in_zones[i];
          });
        });

        resolve();
      });
    }));


    // EXERCISES: TEAM - hr_zones
    promises_data.push(new Promise((resolve, reject) => {

      let promises_speed_zones = [];
      let players_in_zones = [0,0,0,0,0,0]; // 5 hr zones

      event_data.exercises.forEach((e, j) => {

        // start and end of exercise
        // derive them from exercises
        let start;
        let end;

        for (let ex in event_data.exercises) {
          if (event_data.exercises[ex].exercise_id === e.exercise_id) {
            start = event_data.exercises[ex].start;
            end = event_data.exercises[ex].end;
          }
        }

        for (let i = 0; i < event_data.session.hr_zones.length-1; i++) {

          promises_speed_zones.push(new Promise((resolve, reject) => {
            let r1 = event_data.exercises[j].hr_zones[i].value;
            let r2 = event_data.exercises[j].hr_zones[i+1].value;

            let sql = 'SELECT COUNT(hr) as hr_count FROM record WHERE (sessionId = ? AND hr BETWEEN ? AND ? AND timestamp BETWEEN ? AND ?)';
            con.query(sql, [sessionId, r1, r2, start, end], (err, rows) => {
              if (err) {
                console.log("Error getting session_speed data.", err);
                reject();
              } else {
                if (rows[0] !== undefined) {
                  players_in_zones[i] = rows.length;
                  // save count (needed, if the saving rate isn't 1000ms)
                  event_data.exercises[j].hr_zones[i].count = rows[0].hr_count;  // team
                }

                resolve();  // resolve one range
              }
            });
          }));
        };
      }); // end of forEach

      // resolve all the hr ranges
      Promise.all(promises_speed_zones).then(() => {
        // team
        event_data.exercises.forEach((e, j) => {
          let sum_team = 0;
          event_data.exercises[j].hr_zones.forEach((z) => {
            sum_team += z.count;  // 100% of values
          });
          event_data.exercises[j].hr_zones.forEach((z, i) => {
            event_data.exercises[j].hr_zones[i].percentage = z.count * 100 / sum_team;
            z.count = z.count / players_in_zones[i];
          });
        });

        resolve();
      });
    }));


    // EXERCISES: PLAYERS - sprint_distance, avg_hr, relative_hr
    promises_data.push(new Promise((resolve, reject) => {

      let promises_exercises = [];

      event_data.players.forEach((p, i) => {
        p.exercises.forEach((e, j) => {

          // start and end of exercise
          // derive them from exercises
          let start;
          let end;

          for (let ex in event_data.exercises) {
            if (event_data.exercises[ex].exercise_id === e.exercise_id) {
              start = event_data.exercises[ex].start;
              end = event_data.exercises[ex].end;
            }
          }

          promises_exercises.push(new Promise((resolve, reject) => {
            let sql = 'SELECT SUM(speed) as sum_sprint_distance, AVG(hr) as avg_hr FROM record WHERE (sessionId = ? AND playerId = ? AND timestamp BETWEEN ? AND ?)';
            con.query(sql, [sessionId, p.player_id, start, end], (err, rows) => {
              if (err) {
                console.log("Error getting players.", err);
                reject();
              } else {
                // save sprint_distance and avg_hr
                if (rows[0] !== undefined) {
                  if (rows[0].sum_sprint_distance !== null) {
                    event_data.players[i].exercises[j].sprint_distance = rows[0].sum_sprint_distance; 
                  }
                  if (rows[0].avg_hr !== null) {
                    event_data.players[i].exercises[j].avg_hr = rows[0].avg_hr;
                    event_data.players[i].exercises[j].relative_hr = event_data.players[i].exercises[j].avg_hr / (HR_INIT - event_data.players[i].age) * 100;
                  }
                }

                resolve();
              }
            })
          }));

        }); // eoe
      }); // eop

      // resolve all players data
      Promise.all(promises_exercises).then(() => {
        resolve();
      });
    }));


    // EXERCISES: PLAYERS - speed_zones
    promises_data.push(new Promise((resolve, reject) => {

      let promises_exercises = [];

      event_data.players.forEach((p, n) => {
        p.exercises.forEach((e, j) => {

          // start and end of exercise
          // derive them from exercises
          let start;
          let end;

          for (let ex in event_data.exercises) {
            if (event_data.exercises[ex].exercise_id === e.exercise_id) {
              start = event_data.exercises[ex].start;
              end = event_data.exercises[ex].end;
            }
          }

          promises_exercises.push(new Promise((resolve, reject) => {
      
            let promises_speed_zones = [];
      
            for (let i = 0; i < event_data.session.speed_zones.length-1; i++) {
      
              promises_speed_zones.push(new Promise((resolve, reject) => {
                let r1 = event_data.session.speed_zones[i].value;
                let r2 = event_data.session.speed_zones[i+1].value;
        
                // one player one zone
                let sql = 'SELECT COUNT(speed) as p_speed_count FROM record WHERE (sessionId = ? AND playerId = ? AND (timestamp BETWEEN ? AND ?) AND (speed BETWEEN ? AND ?))';
                con.query(sql, [sessionId, p.player_id, start, end, r1, r2], (err, rows) => {
                  if (err) {
                    console.log("Error getting session_speed data.", err);
                    reject();
                  } else {
                    if (rows[0] !== undefined && rows[0].p_speed_count !== null) {
                      // save count (needed, if the saving rate isn't 1000ms)
                      event_data.players[n].exercises[j].speed_zones[i].count = rows[0].p_speed_count;
                    }
      
                    resolve();  // resolve one range
                  }
                });
              }));
            };

            // resolve all the speed ranges
            Promise.all(promises_speed_zones).then(() => {
              // percentage
              let sum_player = 0;
              event_data.players[n].exercises[j].speed_zones.forEach((z) => {
                sum_player += z.count;
              });

              event_data.players[n].exercises[j].speed_zones.forEach((z, l) => {
                event_data.players[n].exercises[j].speed_zones[l].percentage = z.count * 100 / sum_player;
              });

              resolve();
            });
          }));
        }); // eoe
      }); // eop


      Promise.all(promises_exercises).then(() => {
        resolve();
      });
    }));


    // EXERCISES: PLAYERS - hr_zones
    promises_data.push(new Promise((resolve, reject) => {

      let promises_exercises = [];

      event_data.players.forEach((p, n) => {
        p.exercises.forEach((e, j) => {

          // start and end of exercise
          // derive them from exercises
          let start;
          let end;

          for (let ex in event_data.exercises) {
            if (event_data.exercises[ex].exercise_id === e.exercise_id) {
              start = event_data.exercises[ex].start;
              end = event_data.exercises[ex].end;
            }
          }

          promises_exercises.push(new Promise((resolve, reject) => {
      
            let promises_hr_zones = [];
      
            for (let i = 0; i < event_data.session.hr_zones.length-1; i++) {
      
              promises_hr_zones.push(new Promise((resolve, reject) => {
                let r1 = event_data.session.hr_zones[i].value;
                let r2 = event_data.session.hr_zones[i+1].value;
        
                // one player one zone
                let sql = 'SELECT COUNT(hr) as p_hr_count FROM record WHERE (sessionId = ? AND playerId = ? AND (timestamp BETWEEN ? AND ?) AND (hr BETWEEN ? AND ?))';
                con.query(sql, [sessionId, p.player_id, start, end, r1, r2], (err, rows) => {
                  if (err) {
                    console.log("Error getting exercise_hr data.", err);
                    reject();
                  } else {
                    if (rows[0] !== undefined && rows[0].p_hr_count !== null) {
                      // save count (needed, if the saving rate isn't 1000ms)
                      event_data.players[n].exercises[j].hr_zones[i].count = rows[0].p_hr_count;
                    }
      
                    resolve();  // resolve one range
                  }
                });
              }));
            };

            // resolve all the hr ranges
            Promise.all(promises_hr_zones).then(() => {
              // percentage
              let sum_player = 0;
              event_data.players[n].exercises[j].hr_zones.forEach((z) => {
                sum_player += z.count;
              });

              event_data.players[n].exercises[j].hr_zones.forEach((z, l) => {
                event_data.players[n].exercises[j].hr_zones[l].percentage = z.count * 100 / sum_player;
              });

              resolve();
            });
          }));
        }); // eoe
      }); // eop


      Promise.all(promises_exercises).then(() => {
        resolve();
      });
    }));


    // now we have all the data ready, send it all!
    Promise.all(promises_data).then(() => {
      response.data = event_data;
      res.json(response);
    });

  }, () => {
    console.log('rejected! :(');
  });
}