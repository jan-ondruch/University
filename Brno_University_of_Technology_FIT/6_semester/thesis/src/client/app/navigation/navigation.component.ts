import { Component } from '@angular/core';
import { Location } from '@angular/common';
import { Observable } from 'rxjs/Rx';

import { DataService } from '../services/data.service';
import { SharedService } from '../services/shared.service';
import { AuthenticationService } from '../services/authentication.service';
import { AlertService } from '../services/alert.service';

import { Router, ActivatedRoute } from '@angular/router';

import { Team } from '../data-model';

@Component ({
  selector: 'navigation',
  templateUrl: './navigation.component.html',
  styleUrls: ['./navigation.component.css']
})

export class NavigationComponent {
	teams: Team[];				// all teams
	currentTeam: Team;		// currently active team

	mode: string = 'new-session';	// new-session || recording
	session_id: string;	// currently recorded session, if exists

  page: string = 'dashboard'; // currently selected page

  alive: boolean = true;  // subscription flag

  isNavbarCollapsed: boolean = true;


	constructor(
		private _dataService: DataService,
		private _router: Router,
		private _route: ActivatedRoute,
    private _sharedService: SharedService,
    private _authenticationService: AuthenticationService,
    private _alertService: AlertService
	) { 
		this.getTeams();

    Observable.from(this._router.events)
      .takeWhile(() => this.alive)
      .subscribe(() => {
        this.setPageName();
      });	// watch for changes

    // watch for active recording change
    this._sharedService.recording
      .takeWhile(() => this.alive)
      .subscribe(() => {
        this.checkRecording();
      })

    // watch for team changes from teams component
    this._sharedService.teams
      .takeWhile(() => this.alive)
      .subscribe(() => {
        this.getTeams();
      })
	}

  ngOnDestroy() { 
    this.alive = false; // unsub
  }

  /* rearrange array */
  move(from, to) {
    this.teams.splice(to, 0, this.teams.splice(from, 1)[0]);
  }

	/* currently selected team for the dashboard */
	setCurrentTeam() {
		this.currentTeam = this.teams.find(
			team => team.team_id === parseInt(
				this._route.snapshot.paramMap.get('id'), 10))

    // this happens when the team is deleted in teamsComponent
    if (this.currentTeam === undefined) {
      this.currentTeam = this.teams[0]; // select the first team
      let id = this.currentTeam.team_id;
      this._router.navigate(['teams', id, 'teams']);
    } else {
      let from = this.teams.indexOf(this.currentTeam);
      this.move(from, 0);
    }
	}

	/* selected team from the dropdown list */
	onSelectedTeam(event: any) {
		this.currentTeam = this.teams.find(
			t => t.name === event.target.value)

		// set sessions for the currentTeam
		let id = this.currentTeam.team_id;
    
    // notify the shared service for change
    this._sharedService.teamChange();

		this._router.navigate(['teams', id, 'dashboard']);
	}

	/* get all teams available to choose from */
	getTeams() {
		this._dataService.getTeams()
			.subscribe(
				res => this.teams = res,
				error => console.error(error),
				() => this.setCurrentTeam()
			)
	}

	/* dashboard routing */
	onDashboardClick() {
		this._router.navigate(['dashboard'], {relativeTo: this._route});
	}

	/* new session routing */
	onNewSessionClick() {
		this._router.navigate(['new-session'], {relativeTo: this._route});
	}

	/* browse teams */
	onTeamsClick() {
		this._router.navigate(['teams'], {relativeTo: this._route});
	}

  /* go to settings */
  onSettingsClick() {
    this._router.navigate(['settings'], {relativeTo: this._route});
  }

  /* route to players component on players click */
  onPlayersClick() {
    this._router.navigate(['teams', this.currentTeam.team_id, 'players']);
  }

  /* logs out the current user */
  onLogoutClick() {
    this._alertService.logoutSet();
  }

  /* check if recording is active */
  checkRecording() {
    this._dataService.getRecording()
      .subscribe(
        res => {
          this.session_id = res.session_id;

          // check if session exists
          this.mode = (this.session_id.length) ? 'recording' : 'new-session';
        },
        error => console.error(error)
      )
  }

  /* gets back to the current recording */
  onRecordingClick() {
    this._router.navigate(['teams', this.currentTeam.team_id, 'online', this.session_id])
  }

  /* set page name */
  setPageName() {
    this.page = this._route.snapshot.children[0].url[0].path;
  }
}