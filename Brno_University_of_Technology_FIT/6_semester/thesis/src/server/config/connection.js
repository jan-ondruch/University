const mysql = require('mysql');

// create MySQL connection
const con = mysql.createConnection({
    host     : 'localhost',
    user     : 'root',
    password : 'pass1234',
    database : 'carrot'
});

module.exports = con;