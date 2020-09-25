import { Injectable } from '@angular/core';

import { Http, Response, Headers, RequestOptions } from '@angular/http';
import { Observable, Observer } from 'rxjs/Rx';

import { AuthenticationService } from './authentication.service';
import { Team } from '../data-model';

import 'rxjs/add/operator/map';


@Injectable()
export class DataService {

  headers: Headers;
  options: RequestOptions;


  constructor(
    private _http: Http,
    private _authenticationService: AuthenticationService
  ) {
    this.headers = new Headers({ 'Content-Type': 'application/json' });
    this.options = new RequestOptions({ headers: this.headers });    
  }

  /* get all teams */
  getTeams(): Observable<Team[]> {
    return this._http.get("/api/teams", this.options)
  		.map((response: Response) => <any>response.json().data);
  }

  /* get all sessions for the given team */
  getSessions(team_id) {
    return this._http.get("/api/sessions/" + team_id)
      .map(res => res.json().data)
  }

  /* get all players in the given team */
  getPlayers(team_id) {
    return this._http.get("/api/players/" + team_id)
     .map(res => res.json().data)
  }

  /* creates new session */
  createSession(session, players) {
    let body = JSON.stringify({
      "session": session,
      "players": players
    });

    return this._http.post("/api/sessions", body, this.options)
      .map(res => res.json().data)
  }

  /* save session recording */
  saveSession(session_id) {
    return this._http.post("/api/sessions/" + session_id, this.options)
      .map(res => res.json().data)
  }

  /* get all players in the given team and the current session */
  getSessionPlayers(session_id) {
    return this._http.get("/api/sessions/online/" + session_id)
     .map(res => res.json().data)
  }

  /* creates new exercise */
  createExercise(exercise, players) {
    let body = JSON.stringify({
      "exercise": exercise,
      "players": players
    });

    return this._http.post("/api/online/exercises", body, this.options)
      .map(res => res.json().data)
  }

  /* save exercise recording */
  saveExercise(exercise_id) {
    return this._http.post("/api/online/exercises/" + exercise_id, this.options)
      .map(res => res.json().data)
  }

  /* get all exercises of one session */
  // I don't think I am using it
  getExercises(session_id) {
    return this._http.get("/api/online/exercises/" + session_id)
      .map(res => res.json().data)
  }

  /* edit one player information */
  editPlayer(player) {
    let body = JSON.stringify(player);
    return this._http.post("/api/players/edit/" + player.player_id, body, this.options)
      .map(res => res.json())
  }

  /* add new player */
  addPlayer(player) {
    let body = JSON.stringify(player);
    return this._http.post("/api/players/" + player.team_id, body, this.options)
      .map(res => res.json())
  }

  /* delete the player */
  deletePlayer(player_id) {
    return this._http.delete("/api/players/edit/" + player_id, this.options)
      .map(res => res.json())
  }

  /* edit one team information */
  editTeam(team) {
    let body = JSON.stringify(team);
    return this._http.post("/api/teams/" + team.team_id, body, this.options)
      .map(res => res.json())
  }

  /* add new team */
  addTeam(team) {
    let body = JSON.stringify(team);
    return this._http.post("/api/teams/", body, this.options)
      .map(res => res.json())
  }

  /* delete team */
  deleteTeam(team_id) {
    return this._http.delete("/api/teams/" + team_id, this.options)
      .map(res => res.json())
  }

  /* connect all websockets for the given session */
  connectWSockets(session_id) {  // retry on error add feature (retryWhen)
    return this._http.post("/api/online/connect/" + session_id, this.options)
      .map(res => res.json().data)
  }

  /* connect all websockets for the given session */
  disconnectWSockets(session_id) {
    return this._http.post("/api/online/disconnect/" + session_id, this.options)
      .map(res => res.json())
  }

  /* get currently recorded session */
  getRecording() {
    return this._http.get("/api/online/session")
      .map(res => res.json().data)
  }

  /* get online data for all players in the current session */
  getSessionData(session_id) {
    return this._http.get("/api/online/session-data/" + session_id, this.options)
      .map(res => res.json().data)
  }

  /* get online data for all players in the current exercise */
  getExerciseData(exercise_id) {
    return this._http.get("/api/online/exercise-data/" + exercise_id, this.options)
      .map(res => res.json().data)
  }

  /* get data from one event */
  getEventData(session_id) {
    return this._http.get("/api/events/" + session_id, this.options)
      .map(res => res.json().data)
  }

  /* check whether tag is online or offline */
  getTagStatus(tag_id) {
    return this._http.get("/api/online/tag-status/" + tag_id, this.options)
      .map(res => res.json().data)
  }

  /* get server settings */
  getServerSettings() {
    return this._http.get("/api/online/server-settings")
      .map(res => res.json().data)
  }

  /* edit server settings */
  editServerSettings(server_info) {
    let body = JSON.stringify(server_info);
    return this._http.post("/api/online/server-settings", body, this.options)
      .map(res => res.json().data)
  }

  /* create new account */
  registerAccount(account) {
    let body = JSON.stringify(account);
    return this._http.post("/api/registration", body, this.options)
      .map(res => res.json())
  }

  /* debug only */
  private handleError(error: any): Promise<any> {
    console.error('An error occurred', error);	// debug purposes only
    return Promise.reject(error.message || error);
 	}
}