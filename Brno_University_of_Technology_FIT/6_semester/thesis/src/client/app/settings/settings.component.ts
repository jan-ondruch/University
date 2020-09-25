import { Component } from '@angular/core';
import { Observable } from 'rxjs/Rx';
import { ActivatedRoute, Router } from '@angular/router';
import { DataService } from '../services/data.service';

import { TimerObservable } from "rxjs/observable/TimerObservable";
import { takeWhile } from 'rxjs/operator/takeWhile';

import { Player, Settings } from '../data-model';

import { FormGroup, FormControl, Validators } from '@angular/forms';

@Component ({
  selector: 'settings',
  templateUrl: './settings.component.html',
  styleUrls: ['./settings.component.css']
})

export class SettingsComponent {

  serverInfo: Settings;

  serverForm: FormGroup;    // edit player
  server: FormControl;
  port: FormControl;
  api_key: FormControl;

  recording: boolean = true;  // active recording

  players: Player[];
  interval: number = 5000; // interval for checking tags' status
  alive: boolean = true;  // subscription detonator ...tick...
  tagStatus: any = [];  // connectivity status

  successfulEdit: boolean = false;  // successful form edit
  edited: boolean = false;  // timeout


	/* Create an instance of the DataService through dependency injection */
	constructor(
		private _dataService: DataService,
		private _router: Router,
		private _route: ActivatedRoute
	) {
    this.checkRecording();
    this.getPlayers();
    this.getServerSettings();
	}

  ngOnDestroy() {
    this.alive = false; // stop subscription (tag checking)
  }

  /* create controls for the form */
  createFormControls() {
    this.server = new FormControl(this.serverInfo.server, [
      Validators.required
    ]),
    this.port = new FormControl(this.serverInfo.port, [
      Validators.required
    ]),
    this.api_key = new FormControl(this.serverInfo.api_key)
  }

  /* init form with the default settings */
  createServerForm() {
    this.serverForm = new FormGroup({
      server: this.server,
      port: this.port,
      api_key: this.api_key
    });
  }

  /* get server settings */
  getServerSettings() {
    this._dataService.getServerSettings()
      .subscribe(
        res => {
          this.serverInfo = res;
          // create the form
          this.createFormControls();
          this.createServerForm();
        },
        error => console.error(error)
      )
  }

  /* edit server on form submit button */
  onEditServerSettings() {
    this.serverInfo = this.serverForm.value;  // map the values

    this._dataService.editServerSettings(this.serverInfo)
      .subscribe(
        res => {
          this.successfulEdit = true;
          this.edited = true;

          // wait 3 Seconds and hide
          setTimeout(function() {
            this.edited = false;
          }.bind(this), 5000);
        },
        error => console.error(error),
        () => this.checkRecording()
      )
  }

  /* check if recording is active so the server setting cannot be changed */
  checkRecording() {
    this._dataService.getRecording()
      .subscribe(
        res => {
          // check if session exists
          this.recording = (res.session_id.length) ? true : false;
        },
        error => console.error(error)
      )
  }

  /* gets current team id */
  getTeamId = () => this._route.parent.snapshot.paramMap.get('id');

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
        "online": false,
        "battery": 0
      });
    });

    this.tagStatus.forEach((ts,i) => {
      TimerObservable.create(0, this.interval)
        .takeWhile(() => this.alive)  // until alive
        .subscribe(() => {
          this._dataService.getTagStatus(ts.tag_id)
            .subscribe(
              res => {
                if (res.data !== false) { // the tag is up
                  this.tagStatus[i].online = true;
                  this.tagStatus[i].battery = res.body.datastreams[4].current_value.slice(0,-1);
                }
                else { // the tag is offline
                  this.tagStatus[i].online = false;
                  this.tagStatus[i].battery = "0";
                }
              },
              error => console.error(error)
            );
        });
    });
  }
}