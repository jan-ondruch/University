const express = require('express');
const router = express.Router();

const login = require('../controllers/loginController');
const registration = require('../controllers/registrationController');
const teams = require('../controllers/teamsController');
const players = require('../controllers/playersController');
const sessions = require('../controllers/sessionsController');
const online = require('../controllers/onlineController');
const events = require('../controllers/eventsController');


// TEAMS //

// one team
router.route('/teams/:id')
	.get(teams.detail)			// info about one team
	.delete(teams.delete)		// delete one team
	.post(teams.edit)				// edit one team

// all teams
router.route('/teams') 
	.get(teams.get)					// get all teams for the given account
	.post(teams.add)				// add a new team



// PLAYERS //

// one player
router.route('/players/edit/:id')
	.get(players.detail)
	.post(players.edit)				// edit info about one player
	.delete(players.delete)		// delete one player

// all players
router.route('/players/:team_id')
	.get(players.get)				// get all players for a given team
	.post(players.add)			// add a new player



// SESSIONS //

// one session
router.route('/sessions/:id')
	.post(sessions.edit)			// save an exercise

// all sessions
router.route('/sessions/:team_id')
	.get(sessions.get)	// get all sessions for a given team

router.route('/sessions')
	.post(sessions.add)	// add a new session

router.route('/sessions/online/:id')
	.get(sessions.players)		// get players in the current session



// ONLINE
router.route('/online/connect/:id')
	.post(online.connect)    // connect to the data server

router.route('/online/disconnect/:id')
	.post(online.disconnect) // disconnect from the data server

router.route('/online/session')
	.get(online.getRecording)  // get current recording

router.route('/online/tag-status/:id')
	.get(online.getTagStatus)  // get tag status


// compute data for session
router.route('/online/session-data/:id')
	.get(online.getSessionData)  // get current session data


// exercises
router.route('/online/exercises/:id')
	.post(online.endExercise)		// ends an exercise (exercise_id)
	.get(online.getExercise)		// get all exercises (session_id)

router.route('/online/exercises')
	.post(online.addExercise)	// add new exercise

// compute data for exercise
router.route('/online/exercise-data/:id')
	.get(online.getExerciseData) // get current exercise data

// server settings
router.route('/online/server-settings')
  .get(online.getServerSettings)  // get server settings
  .post(online.editServerSettings)  // edit the settings



// LOGIN + LOGOUT
router.route('/login')
	.post(login.login) // login

router.route('/logout')
  .post(login.logout) // logout



// REGISTRATION
router.route('/registration')
  .post(registration.register)  // create new account
  


// EVENTS
router.route('/events/:id')
	.get(events.eventData)	// gets data from one event


module.exports = router