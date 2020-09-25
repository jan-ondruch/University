import { Component, ChangeDetectionStrategy } from '@angular/core';
import { CalendarEvent, CalendarEventTitleFormatter } from 'angular-calendar';
import { CustomEventTitleFormatter } from './custom-event-title-formatter.provider';
import { colors } from '../calendar-utils/colors';
import {
  isSameMonth,
  isSameDay
} from 'date-fns';

import { Observable } from 'rxjs/Observable';
import { Subject } from 'rxjs/Subject';
import { map } from 'rxjs/operators/map';

import { DataService } from '../services/data.service';
import { SharedService } from '../services/shared.service';

import { Router, ActivatedRoute } from '@angular/router';
import { Session } from '../data-model';

import { Http } from '@angular/http';

// special decorator for importing components
@Component({
  selector: 'mwl-demo-component',
  changeDetection: ChangeDetectionStrategy.OnPush,
  templateUrl: './calendar.component.html',	// templates have to be present (url or inline)
  providers: [
    {
      provide: CalendarEventTitleFormatter,
      useClass: CustomEventTitleFormatter
    }
  ]
})

export class CalendarComponent {
  sessions: Session[];        // sessions for the currentTeam

  view: string = 'month'; // initial view

  viewDate: Date = new Date();  // current view date

  events$: Observable<Array<CalendarEvent<{session: Session}>>>;

  alive: boolean = true;  // subscription flag

  refresh: Subject<any> = new Subject();  // watch for events change

  activeDayIsOpen: boolean = true;  // flashy UI

  activeRecording: boolean = false; // recording session is active


  constructor(
    private _dataService: DataService,
    private _route: ActivatedRoute,
    private _router: Router,
    private _http: Http,
    private _sharedService: SharedService
  ) {
    // on team change
    this._sharedService.calendar
      .takeWhile(() => this.alive)
      .subscribe(() => {
        this.refresh.next();

        this.fetchEvents();
        this.checkRecording();
    });
  }

  ngOnDestroy(): void {
    this.alive = false;
  }

  dayClicked({ date, events }: { date: Date; events: Array<CalendarEvent<{session: Session}>>; }): void {
    if (isSameMonth(date, this.viewDate)) {
      if (
        (isSameDay(this.viewDate, date) && this.activeDayIsOpen === true) ||
        events.length === 0
      ) {
        this.activeDayIsOpen = false;
      } else {
        this.activeDayIsOpen = true;
        this.viewDate = date;
      }
    }
  }

  /* gets current team id */
  getTeamId = () => this._route.parent.snapshot.paramMap.get('id');
  
  /* fetch events to fill up the calendar */
  fetchEvents(): void {
    this.events$ = this._http.get("/api/sessions/" + this.getTeamId())
      .map(res => {
        let results = this.activeRecording ? res.json().data.slice(0, -1) : res.json().data;
        return results.map((session: Session) => {
          return {
            title: session.name,
            session_id: session.session_id,
            start: new Date(session.start),
            color: colors.red
          }
        })
      });
  }

  /* on event click */
  eventClicked({ event }: { event }): void {
    this._router.navigate(['teams', this.getTeamId(), 'events', event.session_id]);
  }

  /* check if recording is active */
  checkRecording() {
    this._dataService.getRecording()
      .subscribe(
        res => {
          // check if session exists
          this.activeRecording = (res.session_id.length) ? true : false;
        },
        error => console.error(error)
      )
  }
}
