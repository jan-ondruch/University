import { Component } from '@angular/core';
import { Observable } from 'rxjs/Rx';

import { DataService } from '../services/data.service';
import { SharedService } from '../services/shared.service';

import { Router, ActivatedRoute } from '@angular/router';
import { Player, Exercise } from '../data-model';

import { TimerObservable } from "rxjs/observable/TimerObservable";
import { Subject } from 'rxjs/Subject';
import 'rxjs/add/operator/takeUntil';
import { takeWhile } from 'rxjs/operator/takeWhile';

import { DatePipe } from '@angular/common';
import { DecimalPipe } from '@angular/common';

import { FormGroup, FormControl, Validators } from '@angular/forms';

@Component ({
  selector: 'online',
  templateUrl: './online.component.html',
  styleUrls: ['./online.component.css']
})

export class OnlineComponent {

	// crete class for view and subview (kinda enum)
	view: any = "overview";
	subView: any = "distance";
	
	players: Player[];
	exercise: Exercise = new Exercise();

	interval: number;	// interval of updating data
	aliveSession: Subject<boolean> = new Subject<boolean>(); // subscription for live data activation
	aliveExercise: boolean = false;	// exercise is being recorded


  endEntireSession: boolean = false;  // end the session flag

	sessionData: any = [];		// all session data needed
	exerciseData: any = [];	// all exercise data needed

  order: string = "ascend";  // ascend || descend

  hr_types: any = ['red', 'orange', 'green', 'blue', 'grey'];  // hr zones types
  speed_types: any = ['deep', 'ocean', 'light'];  // speed zones types
  speed_names: any = ['sprint', 'run', 'walk']; // speed zones names

  newExerciseForm: FormGroup;
  exerciseName: FormControl;


	constructor(
		private _dataService: DataService,
		private _router: Router,
		private _route: ActivatedRoute,
    private _sharedService: SharedService
	) {
		this.getSessionPlayers();
		this.aliveSession.next(true);
		this.interval = 3000;
	}

	ngOnInit() {
		this.connectWSockets();
    this.createFormControls();
    this.createForm();
	}

	ngOnDestroy() {
		this.aliveExercise = false;	// takeWhile unsub
		this.aliveSession.next(false);	// unsubscribe
	}

  /* create controls for the form */
  createFormControls() {
    this.exerciseName = new FormControl('', [
      Validators.required
    ])
  }

  /* create new session form */
  createForm() {
    this.newExerciseForm = new FormGroup({
      exerciseName: this.exerciseName
    });
  }

	/* connect websockets for the given session */
	connectWSockets() {
		this._dataService.connectWSockets(this.getSessionId())
			.subscribe(
				res => {
					if (res.exercise !== null) {	// already recording an exercise
						this.exercise = res.exercise;
						this.aliveExercise = true;
						this.getExerciseData();
					} else {
						this.aliveExercise = false;
					}

					this.getSessionData();

          // notify the shared service for change
          this._sharedService.activeRecording();
				},
				error => console.log(error)
			)
	}

	/* disconnect the sockets after the session is saved */
	disconnectWSockets() {
		// not safe probably, disconnectWS can be faster an this will fail! (use promise...)
		if (this.aliveExercise) {
      this.saveExercise();	// save exercise if still being recorded
      this.aliveExercise = false; // takeWhile unsub
      this.endEntireSession = true;
    } else {
      this.finalDisconnect();
      this._sharedService.activeRecording();
      this._router.navigate(['teams', this.getTeamId(), 'dashboard']);
    }

    this.aliveSession.next(false);  // unsubscribe
	}

	/* get pre-processed data for the session */
	getSessionData() {
		TimerObservable.create(0, this.interval)
			.takeUntil(this.aliveSession)	// until alive (rewrite to simpler takeWhile)
			.subscribe(() => {
				this._dataService.getSessionData(this.getSessionId())
					.subscribe(
						res => {
							this.sessionData = res;
						},
						error => console.log(error)	
					)
			})
	}

	/* get pre-processed data for the exercise */
	getExerciseData() {
		TimerObservable.create(0, this.interval)
			.takeWhile(() => this.aliveExercise)	// until alive
			.subscribe(() => {
				this._dataService.getExerciseData(this.exercise.exercise_id)
					.subscribe(
						res => {
							this.exerciseData = res;
						},
						error => console.log(error)	
					)
			})
	}

	/* gets current team id */
	/* put this function into some shared model as public, then you just import it everywhere ...*/
	getTeamId = () => this._route.parent.snapshot.paramMap.get('id');

	/* gets current team id */
	getSessionId = () => this._route.snapshot.paramMap.get('sid');	

	/* view: overview and comparison switch */
	onOverviewClick() {
		this.view = "overview";
	}

  onComparisonClick() {
    this.view = "comparison";
  }

	/* subview: comparison opts switch */
	onSubViewDistanceClick() {
		this.subView = "distance";
	}

  onSubViewCurrentHRClick() {
    this.subView = "hr_curr";
  }

  onSubViewSprintDistanceClick() {
    this.subView = "sprint_distance";
  }

  /* change order of the displayed players' data */
  onChangeOrderClick() {
    this.order = this.order === "ascend" ? "descend" : "ascend";
  }

	/* save the whole session */
	saveSession() {
		this._dataService.saveSession(this.getSessionId())
			.subscribe(
				res => {},
				error => console.error(error),
				() => {
					this.disconnectWSockets();
				}
			)
	}

	/* get players in the current session */
	getSessionPlayers() {
		this._dataService.getSessionPlayers(this.getSessionId())
			.subscribe(
				res => {
          this.players = res
        },
				error => console.error(error),
			)
	}

	/* create new exercise */
	createExercise() {
    let name = JSON.stringify(this.newExerciseForm.value.exerciseName);
    name = name.substring(1, name.length-1);

		let new_exercise = {
			name: name,
			session_id: parseInt(this.getSessionId(), 10)
		};

    this.newExerciseForm.reset();

		this._dataService.createExercise(new_exercise, this.players)
			.subscribe(
				res => {
					this.exercise.exercise_id = res.exercise_id;
					this.exercise.name = res.name;
					this.aliveExercise = true;

					this.getExerciseData();
				},
				error => console.error(error)
			);
	}

	/* save recorded exercise */
	saveExercise() {
		this._dataService.saveExercise(this.exercise.exercise_id)
			.subscribe(
				res => {
					this.aliveExercise = false;

          // the whole session ends, disconnect the session
          if (this.endEntireSession === true) {
            this.finalDisconnect();
            this._sharedService.activeRecording();
            this._router.navigate(['teams', this.getTeamId(), 'dashboard']);
          }
				},
				error => console.error(error)
			)
	}

  /* end session */
  finalDisconnect() {
    this._dataService.disconnectWSockets(this.getSessionId())
      .subscribe(
        res => {},
        error => console.log(error)
      )
  }

  /* get HR color for overview */
  getHRColor(color) {
    if (color < 57) return '#d4d4d4';
    else if (color >= 57 && color < 66.5) return '#00c3ff';
    else if (color >= 66.5 && color < 76) return '#00AF34';
    else if (color >= 76 && color < 86) return '#ff9900';
    else return '#fc0000';
  }
}