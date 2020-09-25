const con = require('../config/connection');
const WebSocket = require('ws');

const _account = require('../models/account');
const _settings = require('../models/settings');
const _team = require('../models/team');

let response = {
  status: 200,
  data: []
}

/*
 * Register new account.
 * New settings and new team have to be created, too.
 */
exports.register = (req, res) => {
  let account = new _account(
    req.body.email,
    req.body.password
  );

  // new accout
  con.query('SELECT email FROM account WHERE email = ?', req.body.email, (err, results) => {
    if (results.length > 0) { // email already exists
      response.data = {};
      res.json(response);
    } else {
      con.query('INSERT INTO account SET ?', account, (err, results) => {
        if (err) {
          console.log("Error adding account.", err);
        } else {
          let settings = new _settings(
            null,
            "localhost",
            "4000",
            "",
            req.body.email
          );

          // new settings
          con.query('INSERT INTO settings SET ?', settings, (err, results) => {
            if (err) {
              console.log("Error adding settings.", err);
            } else {
              let team = new _team(
                null,
                "Team 1",
                req.body.email
              );

              // new team
              con.query('INSERT INTO team SET ?', team, (err, results) => {
                if (err) {
                  console.log("error ocurred", err);
                } else {
                  response.data = team;
                  res.json(response);
                }
              });
            }
          });
        }
      });
    }
  });
}