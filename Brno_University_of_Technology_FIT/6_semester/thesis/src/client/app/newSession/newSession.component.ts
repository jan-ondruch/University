import { Component } from '@angular/core';
import { Observable } from 'rxjs/Rx';
import { DataService } from '../services/data.service';

import { Router, ActivatedRoute } from '@angular/router';

import { Session, Player } from '../data-model';

import { TimerObservable } from "rxjs/observable/TimerObservable";
import { takeWhile } from 'rxjs/operator/takeWhile';

import { FormGroup, FormControl, Validators } from '@angular/forms';


@Component ({
  selector: 'newSession',
  templateUrl: './newSession.component.html',
  styleUrls: ['./newSession.component.css']
})

export class NewSessionComponent {

	session_id: any;
	players: Player[];
	selectedPlayers: Player[] = [];	// selected players for the session

	interval: number;	// interval for checking tags' status
	alive: boolean = true;
	tagStatus: any = [];	// connectivity status
  tagsNotReady: boolean = false;  // 1+ tags isn't ready

  newSessionForm: FormGroup;
  sessionName: FormControl;


	constructor(
		private _dataService: DataService,
		private _router: Router,
		private _route: ActivatedRoute
	) {
		this.getPlayers();
		this.interval = 5000;
	}

  /* init the form */
  ngOnInit() {
    this.createFormControls();
    this.createForm();
  }

  /* create controls for the form */
  createFormControls() {
    this.sessionName = new FormControl('', [
      Validators.required
    ])
  }

  /* create new session form */
  createForm() {
    this.newSessionForm = new FormGroup({
      sessionName: this.sessionName
    });
  }

	ngOnDestroy() {
		this.alive = false;	// stop subscription (tag checking)
	}

	/* add players in the session */
	addPlayer(player_id) {
		this.selectedPlayers.push(this.players.find(p => p.player_id === player_id));
		this.players = this.players.filter(p => p.player_id !== player_id);
    this.checkSelectedTags();
	}

  /* add all players */
  addAllPlayers() {
    this.players.forEach(p => {
      this.selectedPlayers.push(p);
    });
    this.players = [];
    this.checkSelectedTags();
  }

	/* remove player from the session */
	removePlayer(player_id) {
		this.players.push(this.selectedPlayers.find(p => p.player_id === player_id));
		this.selectedPlayers = this.selectedPlayers.filter(p => p.player_id !== player_id);
    this.checkSelectedTags();
	}

  /* remove all players */
  removeAllPlayers() {
    this.selectedPlayers.forEach(p => {
      this.players.push(p);
    });
    this.selectedPlayers = [];
    this.checkSelectedTags();
  }

	/* gets current team id */
	getTeamId = () => this._route.parent.snapshot.paramMap.get('id');

	/* creates new session and routes to the online view */
	createSession() {
    let name = JSON.stringify(this.newSessionForm.value.sessionName);
    name = name.substring(1, name.length-1);
		
    let new_session = {
			name: name,
			team_id: parseInt(this.getTeamId(), 10)
		};

		this._dataService.createSession(new_session, this.selectedPlayers)
			.subscribe(
				res => this.session_id = res.session_id,
				error => console.error(error),
				() => this._router.navigate(['teams', this.getTeamId(), 'online', this.session_id])
			);
	}

	/* get all players for the given team */
	getPlayers() {
		this._dataService.getPlayers(this.getTeamId())
			.subscribe(
				res => {
					this.players = res;
					this.checkTagsStatus();
				},
				error => console.error(error)
			)
	}

	/* periodically check on tag status */
	checkTagsStatus() {
		this.players.forEach((p) => {
			this.tagStatus.push({
				"tag_id": p.tag_id,
				"online": false
			});
		});

    this.tagStatus.forEach((ts,i) => {
      TimerObservable.create(0, this.interval)
        .takeWhile(() => this.alive)  // until alive
        .subscribe(() => {
          this._dataService.getTagStatus(ts.tag_id)
            .subscribe(
              res => {
                if (res.data !== false)  // tag is online
                  this.tagStatus[i].online = true;  // the tag is up
              },
              error => console.error(error)
						);
				});
		});
	}

  /* check selected tags and their status */
  checkSelectedTags() {
    this.tagsNotReady = false;
    this.selectedPlayers.forEach(p => {
      this.tagStatus.forEach(ts => {
        if (ts.online === false && ts.tag_id === p.tag_id) {
          this.tagsNotReady = true;
        }
      });
    });
  }

  /* check status of one tag and colorize the button accordingly */
  checkTagStatus(tag_id): boolean {
    let status;

    this.tagStatus.forEach(ts => {
      if (ts.tag_id === tag_id) {
        status = ts.online;
      }
    });

    return status;
  }
}