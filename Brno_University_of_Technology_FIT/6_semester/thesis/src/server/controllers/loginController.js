const con = require('../config/connection');
const WebSocket = require('ws');
const path = require('path');

const jwt = require('jsonwebtoken');
const fs = require('fs');

let response = {
  status: 200,
  data: []
}

const RSA_PRIVATE_KEY = fs.readFileSync(path.resolve(__dirname, '../rsa/key.pem'), 'utf8');


/*
 * Login to the system having RSA as a guard.
 */
exports.login = (req, res) => {
  let email = req.body.email;
  let password = req.body.password;

  con.query('SELECT * from account WHERE email = ?', email, (err, results) => {
    if (err) {
      console.log("Error getting session players.", err);
    } else {
      if (results.length > 0) {
        if (results[0].password === password) {
          req.session.email = email;

          /* set server settings */
          let server_settings_promise = new Promise((resolve, reject) => {
            con.query('SELECT * from settings WHERE email = ?', req.session.email, (err, rows) => {
              if (err) {
                console.log("Error getting settings.", err);
                reject();
              } else {
                req.session.SERVER = rows[0].server;
                req.session.PORT = rows[0].port;
                req.session.API_KEY = rows[0].api_key;

                resolve();
              }
            })
          });

          server_settings_promise.then(() => {
            const jwtBearerToken = jwt.sign({}, RSA_PRIVATE_KEY, {
              algorithm: 'RS256',
              expiresIn: '1d',
              subject: req.session.email
            });

            response.data = {idToken: jwtBearerToken, expiresIn: 1440};
            res.json(response);
          });
        }
        else {
          response.data = {};
          res.json(response);
        }
      } else {
        response.data = {};
        res.json(response);
      }
    }
  })
}

/* 
 * Logout the account.
 */
exports.logout = (req, res) => {
  delete req.session.email;
}