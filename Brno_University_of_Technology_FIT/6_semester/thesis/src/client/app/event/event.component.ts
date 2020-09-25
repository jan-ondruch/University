import { Component } from '@angular/core';
import { Observable } from 'rxjs/Rx';
import { DataService } from '../services/data.service';

import { Router, ActivatedRoute } from '@angular/router';

import { DatePipe } from '@angular/common';
import { DecimalPipe } from '@angular/common';

import { colors } from '../calendar-utils/colors';
import { SlicePipe } from '@angular/common';

@Component({
  selector: 'event',
  templateUrl: './event.component.html',
  styleUrls: ['./event.component.css']
})

export class EventComponent {

	view: any = "overview";	// overview || exercise
  viewEx: any = "ex_overview";  // view for exercises
	subView: any = "distance";	// distance || hr || ...

  selectedExerciseIndex: number;  // index of array where the selected exercise is
  order: string = "ascend";  // ascend || descend

  eventData: any; // all event data
  exercisesReady: boolean = false;

  hr_types: any = ['red', 'orange', 'green', 'blue', 'grey'];  // hr zones types
  speed_types: any = ['deep', 'ocean', 'light'];  // speed zones types
  speed_names: any = ['sprint', 'run', 'walk']; // speed zones names


  constructor(
  	private _dataService: DataService,
  	private _router: Router,
  	private _route: ActivatedRoute
  ) { }

  ngOnInit() { 
    this.getEventData();
  }

  /* get current team id */
  getTeamId = () => this._route.parent.snapshot.paramMap.get('id');

  /* gets event id */
  getEventId = () => this._route.snapshot.paramMap.get('eid');  

  /* get all data of the whole event */
  getEventData() {
    this._dataService.getEventData(this.getEventId())
     .subscribe(
        res => {
          this.eventData = res;
          console.log(res)

          // add overview to the list of exercises
          let overview = {
            "exercise_id": -1,
            "name": "Complete session"
          };
          this.eventData.exercises.unshift(overview);
          this.exercisesReady = true;
        },
        error => console.error(error)
     )
  }

  /* view: change mode */
  onOverviewClick() {
    this.view = "overview";
  }

  onComparisonClick() {
    this.view = "comparison";
    this.viewEx = "ex_overview";
  }

  /* subview: comparison opts switch */
  onSubViewDistanceClick() {
    this.subView = "distance";
  }

  onSubViewCurrentHRClick() {
    this.subView = "avg_hr";
  }

  onSubViewSprintDistanceClick() {
    this.subView = "sprint_distance";
  }

  /* change order of the displayed players' data */
  onChangeOrderClick() {
    this.order = this.order === "ascend" ? "descend" : "ascend";
  }

  /* exercise is selected, change view mode to the given exercise */
  onSelectedExerciseClick(event) {
    let exercise_id = parseInt(event.target.value, 10);
    if (exercise_id === -1) {  // session overview
      this.viewEx = "ex_overview";
    } else {
      this.viewEx = "exercises";
      this.eventData.exercises.find((e,i) => {
        if (e.exercise_id === exercise_id) {
          this.selectedExerciseIndex = i-1; // -1 for sessino overview
        };
      });
    }
  }
}
