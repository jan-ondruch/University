const con = require('../config/connection');
const Promise = require('promise');
const _session = require('../models/session');

let response = {
  status: 200,
  data: []
}

/*
 * Add a new session and players within it.
 * return @session_id
 */
exports.add = (req, res) => {
  let start = new Date()
    .toISOString()
    .slice(0, 19)
    .replace('T', ' ');

  let session = new _session(
    null,
    req.body.session.name,
    start,
    null,
    req.body.session.team_id
  );

  con.query('INSERT INTO session SET ?', session, (err, results) => {
	  if (err) {
    	console.log("Error adding new session.", err);
  	} else {
      // create new session
      req.session.session = results.insertId;

      let session_id = results.insertId;
      let promises = [];

      for (let p of req.body.players) {
        promises.push(new Promise((resolve, reject) => {
          let record = {
            "record_id": null,
            "sessionId": results.insertId,
            "playerId": p.player_id
          };

          con.query('INSERT INTO record SET ?', record, (err, results) => {
            if (err) {
              console.log("Error adding new session.", err);
              reject();
            } else {
              resolve();
            }
          });
        }));
      }

      Promise.all(promises).then(() => {
        response.data = {session_id: session_id};
        res.json(response);
      });
    }
  });
}


/*
 * Get all sessions for a given team.
 * return @sessions
 */
exports.get = (req, res) => {
	let team_id = req.params.team_id;  // given team
	
	con.query('SELECT * from session WHERE team_id = ?', team_id, (err, rows, fields) => {
    if (err) {
      console.log("Error getting sessions.", err);
    } else {
      rows.forEach((r, i) => {
        let timemod = new Date(rows[i].start);
        timemod = timemod.setHours(timemod.getHours() + 4);
        rows[i].start = new Date(timemod).toISOString().slice(0, 19).replace('T', ' ');
      });
    	response.data = rows;
      res.json(response);
    }
	})
}


/*
 * Get players for the given session.
 * return @players
 */
exports.players = (req, res) => {
  let sessionId = req.params.id;

  con.query('SELECT DISTINCT playerId from record WHERE sessionId = ?', sessionId, (err, rows, fields) => {
    if (err) {
      console.log("Error getting session players.", err);
    } else {
      response.data = [];
      let promises = [];

      rows.forEach((r) => {
        promises.push(new Promise((resolve, reject) => {
          con.query('SELECT * from player WHERE player_id = ?', r.playerId, (err, rows, fields) => {
            if (err) {
              console.log("Error getting session players detail.", err)
              reject();
            } else {
              response.data.push(rows[0]);
              resolve();
            }
          });
        }));
      });

      Promise.all(promises).then(() => {
        res.json(response);
      });
    }
  })
}


/*
 * Edit end time of a session.
 * return @session_id
 */
exports.edit = (req, res) => {
  let end = new Date()
    .toISOString()
    .slice(0, 19)
    .replace('T', ' ');

	let session_id = req.params.id;

	con.query('UPDATE session SET end = ? WHERE session_id = ?', [end, session_id], (err, results) => {
	  if (err) {
	    console.log("Error editing session.", err);
	  } else {
		  response.data = {session_id: results.insertId};
      res.json(response);
	  }
	});
}