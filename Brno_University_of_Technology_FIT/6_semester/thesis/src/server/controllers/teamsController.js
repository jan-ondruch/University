const con = require('../config/connection');
const _team = require('../models/team');

let response = {
  status: 200,
  data: []
}

/*
 * Get all teams for a given account.
 * @return teams
 */
exports.get = (req, res) => {
  let email = req.session.email;  // session global variable

  con.query('SELECT * from team WHERE email = ?', [email], (err, rows) => {
    if (err) {
      console.log("Error getting teams.", err)
    } else {
      response.data = rows;
      res.json(response);
    }
  })
}


/*
 * Get data about one team.
 * @return team
 */
exports.detail = (req, res) => {
  let team_id = req.params.id;

  con.query('SELECT * FROM team WHERE team_id = ?', team_id, (err, rows) => {
    if (err) {
      console.log("Error getting team details.", err);
    } else {
      response.data = rows[0];
      res.json(response);
    }
  })
}


/*
 * Update a team.
 * @return team
 */
exports.edit = (req, res) => {
  let name = req.body.name;
  let team_id = req.params.id;

  let team = new _team(
    team_id,
    name,
    req.session.email
  );

  con.query('UPDATE team SET name = ? WHERE team_id = ?', [name, team_id], (err, result) => {
    if (err) {
      console.log("Error editing team.", err);
    } else {
      response.data = team;
      res.json(response);
    }
  });
}


/*
 * Add a new team.
 * @return team
 */
exports.add = (req, res) => {
  let team = new _team(
    null,
    req.body.name,
    req.session.email
  );

  con.query('INSERT INTO team SET ?', team, (err, results) => {
	  if (err) {
    	console.log("Error adding a team.", err);
  	} else {
      response.data = team;
      res.json(response);
    }
  });
}


/*
 * Delete a team.
 * @return team
 */
exports.delete = (req, res) => {
  let team_id = req.params.id;

  con.query('DELETE FROM team WHERE team_id = ?', team_id, (err, results) => {
    if (err) {
      console.log("Error deleting team.", err);
      response.data = {};
      res.json(response);
  	} 
  	else {
  		response.data = {team_id: results.insertId};
      res.json(response);
  	}
  });
}