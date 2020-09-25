const con = require('../config/connection');
const _player = require('../models/player');

let response = {
  status: 200,
  data: []
}

/*
 * Get all players for a given team, that were not deleted.
 * return @players
 */ 
exports.get = (req, res) => {
	let team_id = req.params.team_id;	// given team

	con.query('SELECT * from player WHERE (team_id = ? AND removed = "n")', [team_id], (err, rows, fields) => {
    if (err) {
      console.log("Error getting players.", err);
    } else {
    	response.data = rows;
      res.json(response);
    }
	})
}


/*
 * Get details about one player.
 * return @player
 */ 
exports.detail = (req, res) => {
  let player_id = req.params.id;

  con.query('SELECT * FROM player WHERE player_id = ?', player_id, (err, rows, fields) => {
    if (err) {
      console.log("Error getting players detail.", err);
    }
    else {
      response.data = rows[0];
      res.json(response);
    }
  })
}


/*
 * Edit information about one player.
 * return @player
 */ 
exports.edit = (req, res) => {
  let player = new _player(
    req.params.id,
    req.body.firstname,
    req.body.lastname,
    req.body.birth.substring(0,10),
    req.body.weight,
    req.body.tag_id,
    "n",
    req.body.team_id
  );

  let que = 'UPDATE player SET firstname = ?, lastname = ?, birth = ?, weight = ?, tag_id = ? WHERE player_id = ?';
	let cols = [
    player.firstname, 
    player.lastname, 
    player.birth, 
    player.weight, 
    player.tag_id,
    player.player_id
  ];
  con.query(que, cols, (err, result) => {
	  if (err) {
	    console.log("Error editing player.", err);
	  } else {
		  response.data = player;
      res.json(response);
	  }
	});
}


/*
 * Add new player.
 */ 
exports.add = (req, res) => {
  let player = new _player(
    null,
    req.body.firstname,
    req.body.lastname,
    req.body.birth,
    req.body.weight,
    req.body.tag_id,
    "n",
    req.params.team_id
  );

  con.query('INSERT INTO player SET ?', player, (err, results) => {
    if (err) {
      console.log("Error adding player.", err);
    } else {
      response.data = player;
      res.json(response);
    }
  });
}


/*
 * Delete a player.
 */ 
exports.delete = (req, res) => {
	let player_id = req.params.id;

  con.query('UPDATE player SET removed = "y" WHERE player_id = ?', player_id, (err, results) => {
  	if (err) {
    	console.log("Error deleting player.", err);
  	}
  	else {
  		response.data = {player_id: results.insertId};
      res.json(response);
  	}
  });
}