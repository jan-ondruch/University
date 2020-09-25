var express = require('express');
var app = express();
var ejs = require('ejs');
var mysql = require('mysql');
var bodyParser = require('body-parser');
var session = require('express-session');
var MySQLStore = require('express-mysql-session')(session);
var Promise = require('promise');
var sanitizer = require('sanitizer');

// create MySQL connection
var con = mysql.createConnection({
  host     : 'xxx',
  user     : 'yyy',
  password : 'zzz',
  database : 'www'
});

// MySQL session store
var sessionStore = new MySQLStore({
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

// set the process port
app.set('port', (process.env.PORT || 5000));

//put your static files (js, css, images) into /public directory
app.use(express.static(__dirname + '/public'));

// to resolve post data & json
app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());

// initializign session
app.use(session({
  key: 'session_cookie_name',
  secret: 'strongsecretcodehere',
  store: sessionStore,
  resave: false, // don't save session if unmodified
  saveUninitialized: false // don't create session until something stored
}));

// views is directory for all template files
app.set('views', __dirname + '/views'); //set path to *.ejs files
app.set('view engine', 'ejs');  //tell Express we're using EJS

// db connect
con.connect(function(err) {
  if(!err) {
      console.log("Database is connected ... nn");
  } else {
      console.log("Error connecting database ... nn");    
  }
});

// allow CORS
app.use(function(req, res, next) {
  res.header("Access-Control-Allow-Origin", "*");
  res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  next();
});


///////////////////////
// ADMIN //////////////
///////////////////////

// get the admin page and load all the existing accounts
app.get('/admin', checkAuthAdmin, function(req, res) {
  con.query('SELECT * from account WHERE type != ?', 'admin', function(err, rows, fields) {
    if (err) {
      console.log("error ocurred", err);
      res.send({
        "code": 400,
        "failed": "error ocurred"
      })
    }
    else {
      res.render('pages/admin', {
        results : rows
      })
    }
  });
});

// create new account, insert it into a db and return to the admin page
app.post('/new-account', checkAuthAdmin, function(req, res) {
  var account = {
    "username":req.body.username,
    "password":req.body.password,
    "type":req.body.type
  }
  con.query('INSERT INTO account SET ?', account, function (err, results) {
    if (err) {
      console.log("error ocurred", err);
      res.send({
        "code": 400,
        "failed": "error ocurred"
      })
    } else {
      res.redirect('/admin');
    }
  });
});

// edit accout and return to an existing admin page
app.post('/edit-account', checkAuthAdmin, function(req, res) {
  var sql = "UPDATE account SET password = ? WHERE username = ?";
  var vars = [req.body.password, req.body.username];
  con.query(sql, vars, function (err, result) {
    if (err) {
      console.log("error ocurred", err);
      res.send({
        "code": 400,
        "failed": "error ocurred"
      })
    } else {
      res.redirect('/admin');
    }
  });
});

// delete account
app.post('/delete-account', checkAuthAdmin, function(req, res) {
  var sql = "DELETE FROM account WHERE username = ?";
  con.query(sql, [req.body.username], function (err, result) {
    if (err) {
      console.log("error ocurred",err);
      res.send({
        "code": 400,
        "failed":"error ocurred"
      })
    } else {
      res.redirect('/admin');
    }
  });
});

///////////////////////
// END OF ADMIN ///////
///////////////////////


///////////////////////
// LOGIN //////////////
///////////////////////

// default path to login
app.get('/', function(req, res) {
  res.redirect('/login')
});

// checkAuth functions
// admin
function checkAuthAdmin(req, res, next) {
  if (!req.session.username || req.session.username !== 'admin') {
    res.render('pages/login', {
      err_msg: "K přístupu na tuto stránku se musíte přihlásit jako admin."
    })
  } else {
    next();
  }
}

// user + admin
function checkAuthUserAdmin(req, res, next) {
  if (!req.session.username) {
    res.render('pages/login', {
      err_msg: "K přístupu na tuto stránku se musíte přihlásit."
    })
  } else {
    next();
  }
}

// user
function checkAuthUser(req, res, next) {
  if (req.session.username === 'admin') {
    res.render('pages/login', {
      err_msg: "K přístupu na tuto stránku se musíte přihlásit jako lékař nebo sestra."
    })
  } else {
    next();
  }
}

// doctor
function checkAuthDoctor(req, res, next) {
  if (req.session.username !== 'lekar') {
    res.render('pages/login', {
      err_msg: "K přístupu na tuto stránku se musíte přihlásit jako lékař (účet 'lekar')."
    })
  } else {
    next();
  }
}

// return login page
app.get('/login', function(req, res) {
  res.render('pages/login');
});

// return to a login page in case we get back to the login-user POST request
app.get('/login-user', function(req, res) {
  res.redirect('/login')
});

// deal with logging to various accounts
app.post('/login-user', function(req, res) {
  var password = req.body.password;
  var username = req.body.username;
  con.query('SELECT * FROM account WHERE username = ?',[username], function (err, results, fields) {
    if (err) {
      console.log('Error while performing Query.');
    } 
    else { 
      if (results.length > 0) {
        // redirect to admin page if admin
        if (username === "admin" && password === "heslo") {
          req.session.username = results[0].username;
          res.redirect('/admin');
        }
        else if (results[0].password == password) {
          req.session.username = results[0].username;
          res.redirect('/home');
        }
        else {
          // no match
          var err_msg = "Nesprávné heslo."
          res.render('pages/login', {
            err_msg : err_msg
          })
        }
      }
      else {
        // username does not exist
        var err_msg = "Zadané uživatelské jméno neexistuje."
        res.render('pages/login', {
          err_msg : err_msg
        })
      }
    }
  });
});

// loggs out the account
app.post('/logout-user', function(req, res) {
  delete req.session.username;
  res.redirect('/login');
});

///////////////////////
// END OF LOGIN ///////
///////////////////////


///////////////////////
// HOME ///////////////
///////////////////////

// get the admin page and load all the existing accounts
app.get('/home', checkAuthUser, function(req, res) {
  var username = req.session.username;
  var patients, insurances, account;

  if (!username)
    res.redirect('/login')

  // select all the patients in the db
  con.query('SELECT * from patient', function(err, rows, fields) {
    if (err) {
      console.log("error ocurred", err);
      res.send({
        "code": 400,
        "failed": "error ocurred"
      })
    }

    patients = rows;
    // select all insurance companies codes
    con.query('SELECT * from template_insurance', function(err, rows, fields) {
      if (err) {
        console.log("error ocurred", err);
        res.send({
          "code": 400,
          "failed": "error ocurred"
        })
      }

      insurances = rows;
      // select the logged account
      con.query('SELECT * from account WHERE username = ?', username, function(err, rows, fields) {
        if (err) {
          console.log("error ocurred", err);
          res.send({
            "code": 400,
            "failed": "error ocurred"
          })
        } else {
          account = rows;

          res.render('pages/home', {
            patients : patients,
            insurances: insurances,
            type: account[0].type,
            user: account[0].username
          })
        }
      });
    });
  });
});

// create new patient, insert it into a db and return to the admin page
app.post('/new-patient', checkAuthUser, function(req, res) {
  if (req.body.external === undefined) req.body.external = 'N';
  
  var patient = {
    "birth_cn": req.body.birth_cn,
    "firstname": req.body.firstname,
    "lastname": req.body.lastname,
    "sex": req.body.sex,
    "city": req.body.city,
    "street_house": req.body.street_house,
    "email": req.body.email,
    "phone": req.body.phone,
    "removed": 'N',
    "external": req.body.external,
    "insurance": req.body.insurance,
  }


  con.query('INSERT INTO patient SET ?', patient, function (err, results) {
    if (err) {
      console.log("error ocurred", err);
      res.redirect('/home');
    } else {
      res.redirect('/home');
    }
  });
});

// edit patient and return to an existing admin page
app.post('/edit-patient', checkAuthUser, function(req, res) {
  var sql = "UPDATE patient SET lastname = ? , city = ? , street_house = ?, insurance = ? , email = ? , phone = ? WHERE birth_cn = ?";
  var vars = [
    req.body.lastname, 
    req.body.city,
    req.body.street_house,
    req.body.insurance,
    req.body.email,
    req.body.phone,
    req.body.birth_cn
  ];
  con.query(sql, vars, function (err, result) {
    if (err) {
      console.log("error ocurred", err);
      res.send({
        "code": 400,
        "failed": "error ocurred"
      })
    } else {
      res.redirect('/home');
    }
  });
});

// delete patient
app.post('/delete-patient', checkAuthUser, function(req, res) {
  var sql = "UPDATE patient SET removed = ? WHERE birth_cn = ?";
  con.query(sql, ['Y', req.body.birth_cn], function (err, result) {
    if (err) {
      console.log("error ocurred",err);
      res.send({
        "code": 400,
        "failed":"error ocurred"
      })
    } else {
      res.redirect('/home');
    }
  });
});

///////////////////////
// END OF HOME ////////
///////////////////////


///////////////////////
// VISIT  /////////////
///////////////////////

// save visit and plan visit
app.post('/save-visit', checkAuthUser, function(req, res) {

  var save_planned_visit = false;
  var planned_by_nurse = false;
  var new_visit = false;

  // planned visit nurse
  if (req.body.datetime !== undefined) {
    var date = new Date(Date.parse(req.body.datetime)).toISOString().slice(0, 19).replace('T', ' ') || new Date().toISOString().slice(0, 19).replace('T', ' ');
    planned_by_nurse = true;
  } else {
    var date = new Date().toISOString().slice(0, 19).replace('T', ' ');
    var timemod = new Date(date);
    timemod = timemod.setHours(timemod.getHours() + 1);
    date = new Date(timemod).toISOString();
    new_visit = true;
  }

  var birth_cn = req.body.birth_cn;
  var description = req.body.description || "Doplní lékař...";
  var diagnosis = req.body.diagnosis || "Doplní lékař...";
  var externals = req.body.externals || [];
  var drugs = req.body.drugs || [];
  var executions = req.body.executions || [];
  var insurance = req.body.insurance;
  var username = req.session.username;
  if (!username)
    res.redirect('/login')

  // XSS prevention
  description = sanitizer.escape(description);
  description = sanitizer.sanitize(description);
  diagnosis = sanitizer.escape(diagnosis);
  diagnosis = sanitizer.sanitize(diagnosis);

  // save planned visit
  if (req.body.visit !== undefined) {
    save_planned_visit = true;
  }

  if (save_planned_visit) {
    var date = req.body.visit.date;
  }
  var visit = {
    "id_visit": "NULL",
    "date": date,
    "description": description,
    "diagnosis": diagnosis,
    "birth_cn": birth_cn
  };

  var invoice = {
    "invoice_number": "NULL",
    "seller": "IIS praktický lékař",
    "buyer": insurance,
    "amount": 0,
    "issue_date": date,
    "due_date": "2018-01-31 23:59"  // easter egg
  };


  // is saved planned visit, remove the planned version
  if (save_planned_visit) {
    var sql = "DELETE FROM visit WHERE id_visit = ?";
    con.query(sql, req.body.visit.id_visit, function (err, result) {
      if (err) {
        console.log("error ocurred",err);
        res.send({
          "code": 400,
          "failed":"error ocurred"
        })
      }
    });    
  }

  // insert new visit
  con.query('INSERT INTO visit SET ?', visit, function (err, results) {
    if (err) {
      console.log("error ocurred", err);
      res.send({
        "code": 400,
        "failed":"error ocurred"
      })
    }
    // get insurance name for its code
    con.query('SELECT name from template_insurance WHERE insurance = ?', insurance, function (err, rows, fields) {
      if (err)
        console.log('Error while performing Query.');

      invoice.buyer = rows[0].name;
    });

    // get the visit_id
    con.query('SELECT * from visit ORDER BY id_visit DESC LIMIT 1', function (err, rows, fields) {
    if (err)
      console.log('Error while performing Query. [select visit]');

      visit = rows;

      var promises = [];

      for (var j = 0; j < executions.length; j++) {
        promises.push(new Promise(function (resolve, reject) {
          con.query('SELECT cost from template_execution WHERE execution_code = ?', executions[j].execution_code, function(err, rows, fields) {
            if (err) {
              console.log('Error while performing Query.');
              reject();
            }
            invoice.amount += rows[0].cost; // add to the total sum of one invoice
            resolve();
          });
        }))
      }

      // after all the items are summed up, only then insert the invoice
      Promise.all(promises).then(function() {
        // insert new invoice
        con.query('INSERT INTO invoice SET ?', invoice, function (err, results) {
          if (err)
            console.log('Error while performing Query. [insert invoice]');

          // get the invoice_number
          con.query('SELECT * from invoice ORDER BY invoice_number DESC LIMIT 1', function (err, rows, fields) {
          if (err)
            console.log('Error while performing Query. [select invoice]');

            invoice = rows;

            // insert externals
            for (var i = 0; i < externals.length; i++) {
              var external = {
                "id_extern_med_check": "NULL",
                "name": externals[i].name,
                "diagnosis": externals[i].diagnosis,
                "id_visit": visit[0].id_visit
              }
              con.query('INSERT INTO extern_med_check SET ?', external, function (err, results) {
                if (err)
                  console.log('Error while performing Query. [insert external]');
              });
            }

            // insert drugs
            for (var i = 0; i < drugs.length; i++) {
              var drug = {
                "id_drug": "NULL",
                "amount": drugs[i].amount,
                "drug_code": drugs[i].drug_code,
                "id_visit": visit[0].id_visit
              }
              con.query('INSERT INTO drug SET ?', drug, function (err, results) {
                if (err)
                  console.log('Error while performing Query. [insert drug]');
              });
            }
          
            // insert executions
            for (var i = 0; i < executions.length; i++) {
              var execution = {
                "id_execution": "NULL",
                "execution_code": executions[i].execution_code,
                "id_visit": visit[0].id_visit,
                "invoice_number": invoice[0].invoice_number
              }
              con.query('INSERT INTO execution SET ?', execution, function (err, results) {
                if (err)
                  console.log('Error while performing Query. [insert execution]');
              });
            }

            var url = '/visits?birth_cn=' + birth_cn;
            return res.status(200).send({result: 'redirect', url: url})
          });
        });
      })
    });
  });
});

// delete visit
app.post('/delete-visit', checkAuthUser, function(req, res) {
  var username = req.session.username;
  var id_visit = req.body.id_visit;
  var birth_cn = req.body.birth_cn;

  if (!username)
    res.redirect('/login')

  var sql = "DELETE FROM visit WHERE id_visit = ?";
  con.query(sql, id_visit, function (err, result) {
    if (err) {
      console.log("error ocurred",err);
      res.send({
        "code": 400,
        "failed":"error ocurred"
      })
    } else {
      var url = '/visits?birth_cn=' + birth_cn;
      return res.status(200).send({result: 'redirect', url: url})
    }
  });
});

// new visit
app.get('/new-visit', checkAuthDoctor, function(req, res) {
  if (req.query.birth_cn === undefined) {
    res.redirect('/login')
  }
  else {
    var planned = false;
    if (req.query.id_visit !== undefined) {
      var id_visit = req.query.id_visit;
      planned = true;
    }
    var username = req.session.username;
    var birth_cn = req.query.birth_cn;
    var patient, account, drugs, executions, visit, planned_executions;

    if (!username)
      res.redirect('/login')

    // select the chosen patient
    con.query('SELECT * from patient WHERE birth_cn = ?', birth_cn, function(err, rows, fields) {
    if (err)
      console.log('Error while performing Query.');

      patient = rows;

      // non-existent route
      if (patient.length <= 0) {
        res.redirect('/home')
      }
      
      // select template_drug
      con.query('SELECT * from template_drug', function(err, rows, fields) {
      if (err)
        console.log('Error while performing Query.');

        drugs = rows;

        // select template_execution
        con.query('SELECT * from template_execution', function(err, rows, fields) {
        if (err)
          console.log('Error while performing Query.');

          executions = rows;
        
          // select the logged account
          con.query('SELECT * from account WHERE username = ?', username, function(err, rows, fields) {
            if (err)
              console.log('Error while performing Query.');
    
            account = rows;

            if (planned) {
              // select current visit
              con.query('SELECT * from visit WHERE id_visit = ?', id_visit, function(err, rows, fields) {
                if (err)
                  console.log('Error while performing Query. [visit_id]');

                visit = rows;

                con.query('SELECT * from execution WHERE id_visit = ?', id_visit, function(err, rows, fields) {
                  if (err)
                    console.log('Error while performing Query. [executions]');

                  planned_executions = rows;

                  res.render('pages/new-visit', {
                    patient : patient,
                    drugs: drugs,
                    executions: executions,
                    type: account[0].type,
                    user: account[0].username,
                    visit: visit,
                    planned_executions: planned_executions
                  })
                })
              })
            } else {
              res.render('pages/new-visit', {
                patient : patient,
                drugs: drugs,
                executions: executions,
                type: account[0].type,
                user: account[0].username
              })
            }
          });
        });
      });
    });
  }
});

// open visit
app.post('/open-visit', checkAuthDoctor, function(req, res) {
  var username = req.session.username;
  var birth_cn = req.body.birth_cn;
  var id_visit = req.body.id_visit;

  if (!username)
    res.redirect('/login')

  var url = '/new-visit?birth_cn=' + birth_cn + '&id_visit=' + id_visit;
  return res.status(200).send({result: 'redirect', url: url})
});

///////////////////////
// END OF NEW VISIT ///
///////////////////////


///////////////////////
// INVOICES ///////////
///////////////////////

app.get('/invoices', checkAuthDoctor, function(req, res) {
  var username = req.session.username;
  var invoices, insurances, executions;

  if (!username)
    res.redirect('/login')

  // select all invoices
  var selectInvoices = new Promise(function(resolve, reject) {
    con.query('SELECT * from invoice', function(err, rows, fields) {
      if (err) {
        console.log('Error while performing Query.');
        reject();
      }
      else {
        resolve(rows);
      }
    });
  });

  // on fulfill
  selectInvoices.then(function(invoices) {
    // select all insurance companies codes
    con.query('SELECT * from template_insurance', function (err, rows, fields) {
      if (err)
        console.log('Error while performing Query.');

      insurances = rows;

      con.query('SELECT * from account WHERE username = ?', username, function (err, rows, fields) {
        if (err)
          console.log('Error while performing Query.');

        account = rows;
        res.render('pages/invoices', {
          invoices : invoices,
          insurances: insurances,
          type: account[0].type,
          user: account[0].username
        })
      });
    });
  });
});

///////////////////////
// END OF INVOICES ////
///////////////////////


///////////////////////
// VISIT HISTORY //////
///////////////////////

// get the patient
app.get('/visits', checkAuthUserAdmin, checkAuthUser, function(req, res) {
  if (req.query.birth_cn === undefined) {
    res.redirect('/login')
  }
  else {
  var username = req.session.username;
  var birth_cn = req.query.birth_cn;
  var patient, account, visits, execution, results, template_execution;

  if (!username)
    res.redirect('/login')


  var promises_others = [];

  promises_others.push(new Promise(function (resolve, reject) {
    // select the active user
    con.query('SELECT * from account WHERE username = ?', username, function (err, rows, fields) {
      if (err) {
        console.log('Error while performing Query.');
        reject();
      }
      account = rows;
      resolve();
    });
  }))

  promises_others.push(new Promise(function (resolve, reject) {
    // select the active user
    con.query('SELECT * from template_execution', function (err, rows, fields) {
      if (err) {
        console.log('Error while performing Query.');
        reject();
      }
      template_execution = rows;
      resolve();
    });
  }))

  promises_others.push(new Promise(function (resolve, reject) {

    // select the chosen patient
    con.query('SELECT * from patient WHERE birth_cn = ?', birth_cn, function(err, rows, fields) {
      if (err) {
        console.log('Error while performing Query.');
        reject();
      }
      else {
        patient = rows;
  
        // non-existent route
        if (patient.length <= 0) {
          res.redirect('/home')
        }
  
        // select all visits
        con.query('SELECT * from visit WHERE birth_cn = ?', birth_cn, function(err, rows, fields) {
          if (err) {
            console.log('Error while performing Query.');
            reject();
          }
          else {
            visits = rows;
            resolve();
          }
        });
      }
    });
  }))

  // now we have the visits
  Promise.all(promises_others).then(function () {

    results = []; // results of all visits

    // wrap all visits, when they finish - voila!
    var promise_wrap_visits = new Promise(function (resolve, reject) {
      
    var promises_visits = [];

    // get execution code from execution to access its name in template_execution
    for (var i = 0; i < visits.length; i++) {

      promises_visits.push(new Promise(function (resolve, reject) {

        // final arrays to save stuff in
        var executions_name = [];
        var drugs_name = [];
        var drugs_amount = [];
        var extern_med_checks_name = [];
        var extern_med_checks_diagnosis = [];
        
        // wait for them all to finish
        var promises_all = [];

        promises_all.push(new Promise(function (resolve, reject) {

          // EXECUTIONS
          var executions_code = [];  // found codes in the set of executions
          var promises_executions = new Promise(function (resolve, reject) {
            con.query('SELECT execution_code from execution WHERE id_visit = ?', visits[i].id_visit, function(err, rows, fields) {
              if (err) {
                console.log('Error while performing Query. [execution]');
                reject();
              }
              else {
                var p = [];

                for (var k = 0; k < rows.length; k++) {
                  p.push(new Promise(function(resolve) {
                    executions_code.push(rows[k].execution_code);  
                    resolve();
                  }))
                }
                if (rows.length === 0) {
                  resolve();
                }

                Promise.all(p).then(function() {
                  resolve();
                })
              }
            });
          });

          // got all the execution codes and let's find the corresponding names for them
          promises_executions.then(function() {
            if (executions_code.length === 0) resolve();
            
            var promises_template_executions = [];
            for (var j = 0; j < executions_code.length; j++) {

              // promises for template_executions
              promises_template_executions.push(new Promise(function (resolve, reject) {
                con.query('SELECT name from template_execution WHERE execution_code = ?', 
                  executions_code[j], function(err, rows, fields) {
                  if (err) {
                    console.log('Error while performing Query. [template_execution]');
                    reject();
                  }
                  else {
                    executions_name.push(rows[0].name);
                    resolve();
                  }
                });
              }));
            } // end FOR promises_executions

            Promise.all(promises_template_executions).then(function() {
              resolve();
            })
          });
        }));  // EXECUTIONS end


        promises_all.push(new Promise(function (resolve, reject) {

          // DRUGS
          var drugs_code = [];  // found codes in the set of drugs
          var promises_drugs = new Promise(function (resolve, reject) {
            con.query('SELECT * from drug WHERE id_visit = ?', visits[i].id_visit, function(err, rows, fields) {
              if (err) {
                console.log('Error while performing Query. [drug]');
                reject();
              }
              else {
                var p = [];

                for (var k = 0; k < rows.length; k++) {
                  p.push(new Promise(function(resolve) {
                    drugs_code.push(rows[k].drug_code);  
                    drugs_amount.push(rows[k].amount);
                    resolve();
                  })) 
                }
                if (rows.length === 0) {
                  resolve();
                }

                Promise.all(p).then(function() {
                  resolve();
                })
              }
            });
          });

          // got all the drug codes and let's find the corresponding names for them
          promises_drugs.then(function() {
            if (drugs_code.length === 0) resolve();
            
            var promises_template_drugs = [];
            for (var j = 0; j < drugs_code.length; j++) {

              // promises for template_executions
              promises_template_drugs.push(new Promise(function (resolve, reject) {
                con.query('SELECT * from template_drug WHERE drug_code = ?', 
                  drugs_code[j], function(err, rows, fields) {
                  if (err) {
                    console.log('Error while performing Query. [template_drugs]');
                    reject();
                  }
                  else {
                    drugs_name.push(rows[0].name);
                    resolve();
                  }
                });
              }))
            } // end FOR promises_drugs
            
            Promise.all(promises_template_drugs).then(function() {
              resolve();
            })

          });
        }));  // DRUGS end
        

        promises_all.push(new Promise(function (resolve, reject) {

          // EXTERN MED CHECKS
          var promises_externals = new Promise(function (resolve, reject) {
            con.query('SELECT * from extern_med_check WHERE id_visit = ?', 
              visits[i].id_visit, function(err, rows, fields) {
              if (err) {
                console.log('Error while performing Query. [extern_med_check]');
              }
              else {
                var p = [];

                for (var k = 0; k < rows.length; k++) {
                  p.push(new Promise(function(resolve) {
                    extern_med_checks_name.push(rows[k].name);
                    extern_med_checks_diagnosis.push(rows[k].diagnosis);  
                    resolve();
                  }))
                }
                if (rows.length === 0) {
                  resolve();
                }
                
                Promise.all(p).then(function() {
                  resolve();
                })
              }
            });
          })

          promises_externals.then(function() {
            resolve();
          })
        })); // EXTERN MED CHECKS end

        // take them all!
        Promise.all(promises_all).then(function() {

          // console.log(executions_name);
          // console.log(drugs_name);
          // console.log(drugs_amount);
          // console.log(extern_med_checks_name);
          // console.log(extern_med_checks_diagnosis);

          // insert executions
          var drugs = [];
          var extern_med_checks = [];
          var executions = [];

          var ps = [];

          ps.push(new Promise(function(resolve) {
            var p = [];

            for (var i = 0; i < executions_name.length; i++) {
              p.push(new Promise(function(resolve) {
                executions.push({
                  "name": executions_name[i]
                });
                resolve();
              }))
            }
            if (executions_name.length === 0) {
              resolve();
            }

            Promise.all(p).then(function() {
              resolve();
            })
          }))

          ps.push(new Promise(function(resolve) {
            var p = [];
  
            // insert drugs
            for (var i = 0; i < drugs_name.length; i++) {
              p.push(new Promise(function(resolve) {
                drugs.push({
                  "name":drugs_name[i],
                  "amount":drugs_amount[i]
                });
                resolve();
              }))
            }
            if (drugs_name.length === 0) {
              resolve();
            }
  
            Promise.all(p).then(function() {
              resolve();
            })
          }))
  
          ps.push(new Promise(function(resolve) {
            var p = [];
  
            // insert extern med checks
            for (var i = 0; i < extern_med_checks_name.length; i++) {
              p.push(new Promise(function(resolve) {
                extern_med_checks.push({
                  "name":extern_med_checks_name[i],
                  "diagnosis":extern_med_checks_diagnosis[i]
                });
                resolve();
              }))
            }
            if (extern_med_checks_name.length === 0) {
              resolve();
            }
  
            Promise.all(p).then(function() {
              resolve();
            })
          }))

          Promise.all(ps).then(function() {
            // save all in results
            results.push({
              "visit":"",
              "executions":executions,
              "drugs":drugs,
              "extern_med_checks":extern_med_checks
            });
            // console.log(results);
  
            resolve();  // resolve visit
          });
        })  // end PROMISE all
      }));  // end PROMISE visits
    } // end FOR visits

    // all visits have been resolved
    Promise.all(promises_visits).then(function() {
      resolve();  // resolve visits wrap
    })
    
    }) // end PROMISE visits_wrap

    promise_wrap_visits.then(function() { // end PROMISE others
      var p = [];

      // insert extern med checks
      for (var i = 0; i < visits.length; i++) {
        p.push(new Promise(function(resolve) {
          results[i].visit = visits[i];
          resolve();
        }))
      }
      if (visits.length === 0) {
        p.push(new Promise(function(resolve) {
          resolve();
        }))
      }
      
      Promise.all(p).then(function() {

        res.render('pages/visits', {
          results : results,
          patient: patient,
          template_execution: template_execution,
          type: account[0].type,
          user: account[0].username
        })
      });
    }) 
  })  // end PROMISE others
  }
});

///////////////////////
// END OF HISTORY /////
///////////////////////

// to keep the con alive
setInterval(function () {
  con.query('SELECT 1');
}, 5000);

// catch all invalid routes and redirect them
app.all('*', function(req, res) {
  res.redirect("/");
});

app.listen(app.get('port'), function() {
  console.log('Node app is running on port', app.get('port'));
});
