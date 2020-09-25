const express = require('express');
const bodyParser = require('body-parser');
const path = require('path');
const http = require('http');
const session = require('express-session');
const MySQLStore = require('express-mysql-session')(session);

const app = express();

// API file for interacting with MySQL
const api = require('./routes/index')

const con = require('./config/connection');

// MySQL session store
let sessionStore = new MySQLStore({
  checkExpirationInterval: 900000,
  expiration: 86400000,
  createDatabaseTable: true,
  schema: {
    tableName: 'sessions'
,    columnNames: {
      session_id: 'session_id',
      expires: 'expires',
      data: 'data'
    }
  }
}, con);

// Parsers
app.use(bodyParser.json())
app.use(bodyParser.urlencoded({ extended: true}))

// initializign session
app.use(session({
  key: 'session_cookie_name',
  secret: 'strongsecretcodehere',
  store: sessionStore,
  resave: false, // don't save session if unmodified
  saveUninitialized: false // don't create session until something stored
}));


// allow CORS
app.use(function(req, res, next) {
  res.header("Access-Control-Allow-Origin", "*");
  res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  next();
});

// Angular DIST output folder
app.use(express.static(path.join(__dirname, '../dist')))

// API location - handle API requests here
app.use('/api', api)

// Send all other requests to the Angular app
app.get('*', (req, res) => {
    res.sendFile(path.join(__dirname, '../dist/index.html'))
})

//Set Port
const port = process.env.PORT || '3000'
app.set('port', port)

const server = http.createServer(app);

server.listen(port, () => console.log(`Running on localhost:${port}`))