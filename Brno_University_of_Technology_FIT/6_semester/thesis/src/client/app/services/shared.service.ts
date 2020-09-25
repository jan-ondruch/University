import { Injectable } from '@angular/core';

import { Observable, Observer } from 'rxjs/Rx';
import { BehaviorSubject } from 'rxjs/BehaviorSubject';
import { Subject } from 'rxjs/Subject';

@Injectable()
export class SharedService {

  // 1|change of a team: navigation -> dashboard, calendar (parent -> child)
  private currentTeamSource = new BehaviorSubject<void>(null);
  currentTeam = this.currentTeamSource.asObservable();

  // 2|recording is active: online -> navigation
  private activeRecordingSource = new BehaviorSubject<void>(null);
  recording = this.activeRecordingSource.asObservable();

  // 3|teams changed: teams -> navigation
  private teamsChangeSource = new BehaviorSubject<void>(null);
  teams = this.teamsChangeSource.asObservable();

  // 4|update calendar: dashboard -> calendar
  private calendarChangeSource = new BehaviorSubject<void>(null);
  calendar = this.calendarChangeSource.asObservable();


  constructor(
  ) { }

  teamChange() {  // 1
    this.currentTeamSource.next(null);
  }

  activeRecording() { // 2
    this.activeRecordingSource.next(null);
  }

  teamsChange() { // 3
    this.teamsChangeSource.next(null);
  }

  calendarChange() {  // 4
    this.calendarChangeSource.next(null);
  }
}