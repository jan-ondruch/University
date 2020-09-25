import { Component } from '@angular/core';			// basic functionality
import { Observable } from 'rxjs/Rx';

import { DataService } from '../services/data.service';
import { SharedService } from '../services/shared.service';

import { Router, ActivatedRoute } from '@angular/router';

import { Session, Exercise } from '../data-model';
import { takeWhile } from 'rxjs/operator/takeWhile';

import { DatePipe } from '@angular/common';
import { DecimalPipe } from '@angular/common';

import { ChangeDetectionStrategy } from '@angular/core';
import { CalendarEvent } from 'angular-calendar';
import { colors } from '../calendar-utils/colors';

import { SlicePipe } from '@angular/common';

/* special decorator for importing components */
@Component({
  selector: 'dashboard',
  templateUrl: './dashboard.component.html',	// templates have to be present (url or inline)
  styleUrls: ['./dashboard.component.css']		// poss to import multiple files
})

export class DashboardComponent {

	sessions: Session[];				// sessions for the currentTeam
	lastSession: Session;				// last recorded session
	lastSessionData: any;				// data of the last session

  recording: boolean = true;  // active recording

  alive: boolean = true;

  hr_types: any = ['red', 'orange', 'green', 'blue', 'grey'];  // hr zones types
  speed_types: any = ['deep', 'ocean', 'light'];  // speed zones types
  speed_names: any = ['sprint', 'run', 'walk']; // speed zones names

	
  constructor(
		private _dataService: DataService,
		private _router: Router,
		private _route: ActivatedRoute,
    private _sharedService: SharedService
	) {
    // on team change
    this._sharedService.currentTeam
      .takeWhile(() => this.alive)
      .subscribe(() => {
        this.checkRecording();
    })
	}

  ngOnDestroy() {
    this.alive = false;
  }

  /* update the calendar */
  updateCalendar() {
    this._sharedService.calendarChange();
  }

	/* gets current team id */
	public getTeamId = () => this._route.parent.snapshot.paramMap.get('id');

	/* get all sessions for the current team */
	getSessions() {
    this.updateCalendar();
		this._dataService.getSessions(this.getTeamId())
			.subscribe(
				res => {
					this.sessions = res;

					// check for no sessions
					if (this.sessions.length > 0) {
            // check if recording is active
            if (this.recording === true && this.sessions.length > 1) {
              this.lastSession = this.sessions[this.sessions.length-2];
              // remove from sessions the last one
              this.sessions.pop();
            }
            else if (this.recording === true && this.sessions.length === 1) {
              this.lastSession = null,
              this.sessions = [];
            }

            else {
						  this.lastSession = this.sessions[this.sessions.length-1];
            }
          }
					else {
						this.lastSession = null;
            this.sessions = [];
					}
					if (this.lastSession) {
						this.getLastSessionData();
					}
				},
				error => console.error(error)
			)
	}

	/* route to the selected session */
	onSessionClick(session_id) {
		this._router.navigate(['teams', this.getTeamId(), 'events', session_id]);
	}

  /* last session detail click */
  onLastSessionClick() {
    this._router.navigate(['teams', this.getTeamId(), 'events', this.lastSession.session_id]);
  }

	/* get data from the last session */
	getLastSessionData() {
		this._dataService.getEventData(this.lastSession.session_id)
      .subscribe(
				res => {
					this.lastSessionData = res;
				},
				error => console.error(error)
			)
	}

  /* check if recording is active, then get sessions, that already finished */
  checkRecording() {
    this._dataService.getRecording()
      .subscribe(
        res => {
          // check if session exists
          this.recording = (res.session_id.length) ? true : false;
          this.getSessions();
        },
        error => console.error(error)
      )
  }
}