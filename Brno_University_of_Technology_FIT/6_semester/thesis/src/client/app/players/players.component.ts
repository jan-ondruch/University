import { Component } from '@angular/core';
import { Observable } from 'rxjs/Rx';
import { DataService } from '../services/data.service';
import { ActivatedRoute, Router } from '@angular/router';

import { Player } from '../data-model';

import { DatePipe } from '@angular/common';

import { NgbModal, ModalDismissReasons } from '@ng-bootstrap/ng-bootstrap';

import { FormGroup, FormControl, Validators } from '@angular/forms';

@Component ({
  selector: 'players',
  templateUrl: './players.component.html',
  styleUrls: ['./players.component.css']
})

export class PlayersComponent {

	players: Player[];	// all players for the currentTeam
	selectedPlayer: Player;		// currently selected player
  playerToDelete: Player;

	editPlayerForm: FormGroup;		// edit player
	addPlayerForm: FormGroup;			// add new player

  editFirstname: FormControl;
  editLastname: FormControl;
  editYear: FormControl;
  editMonth: FormControl;
  editDay: FormControl;
  editWeight: FormControl;
  editTagId: FormControl;

  addFirstname: FormControl;
  addLastname: FormControl;
  addYear: FormControl;
  addMonth: FormControl;
  addDay: FormControl;
  addWeight: FormControl;
  addTagId: FormControl;
	
	view: any = "edit";	// view mode - edit | add

  recording: boolean = true;  // recording is active

  modalRef: any;  // modal window reference

  successfulEdit: boolean = false;  // successful form edit
  edited: boolean = false;  // timeout

  successfulAdd: boolean = false;  // successful form add
  added: boolean = false;  // timeout

  successfulDelete: boolean = false;  // successful form add
  deleted: boolean = false;  // timeout

  // dates
  years: any = [];
  months: any = [];
  days: any = [];

  alertTO: number = 3000;


	constructor(
		private _dataService: DataService,
		private _router: Router,
		private _route: ActivatedRoute,
    private _modalService: NgbModal
	) {
    this.checkRecording();
    this.setDates();
		this.getPlayers();
	}

	/* gets current team id */
	getTeamId = () => this._route.parent.snapshot.paramMap.get('id');

	/* get all players for the given team */
	getPlayers() {
		this._dataService.getPlayers(this.getTeamId())
			.subscribe(	// observables are lazy - if we don't have subscribe here,
				res => {	// no data will be issued (promises aren't lazy, they will issue
					this.players = res; // the services call)
					this.selectedPlayer = this.players[0];	// selectedPlayer
				},
				error => console.error(error),
				() => this.selectedPlayer ? this.initEditForm() : this.initAddForm()
			)
	}

  /* setup dates */
  setDates(): void {
    for (let i = 0; i < 89; i++) {
      this.years.push(2018-i);
    }
    for (let i = 0; i < 12; i++) {
      this.months.push(1+i);
    }
    for (let i = 0; i < 31; i++) {
      this.days.push(1+i);
    }
  }

  /* init edit form */
  initEditForm() {
    this.createEditFormControls();
    this.createEditForm();
  }

  /* create edit form controls */
  createEditFormControls() {
    let year = this.selectedPlayer.birth.substring(0, 4);
    let month = this.selectedPlayer.birth.substring(5, 7);
    let day = this.selectedPlayer.birth.substring(8, 10);

    // remove '0' from the date
    if (month.charAt(0) === '0') month = month.substr(1);
    if (day.charAt(0) === '0') day = day.substr(1);

    this.editFirstname = new FormControl(this.selectedPlayer.firstname, [
      Validators.required,
      Validators.pattern('^[a-zA-Z]+$')
    ]);
    this.editLastname = new FormControl(this.selectedPlayer.lastname, [
      Validators.required,
      Validators.pattern('^[a-zA-Z]+$')
    ]);
    this.editYear = new FormControl(year, [
      Validators.required
    ]);
    this.editMonth = new FormControl(month, [
      Validators.required
    ]);
    this.editDay = new FormControl(day, [
      Validators.required
    ]);
    this.editWeight = new FormControl(this.selectedPlayer.weight, [
      Validators.required,
      Validators.min(10),
      Validators.max(250),
      Validators.pattern('^[0-9]+$')
    ]);
    this.editTagId = new FormControl(this.selectedPlayer.tag_id, [
      Validators.required,
      Validators.pattern('^[0-9]+$')
    ]);
  }

  /* create edit form */
  createEditForm() {
    this.editPlayerForm = new FormGroup({
      editFirstname: this.editFirstname,
      editLastname: this.editLastname,
      editYear: this.editYear,
      editMonth: this.editMonth,
      editDay: this.editDay,
      editWeight: this.editWeight,
      editTagId: this.editTagId
    });
    this.view = "edit";
  }

	/* change selectedPlayer and the form to the given player */
	onPlayerClick(player_id) {
		this.selectedPlayer = this.players.find(p => p.player_id === player_id);
		this.initEditForm();
	}

	/* edits the current player */
	onEditPlayer() {
  	this.selectedPlayer.firstname = this.editPlayerForm.value.editFirstname;
    this.selectedPlayer.lastname = this.editPlayerForm.value.editLastname;

    // append '0' if necessary 
    let month = this.editPlayerForm.value.editMonth;
    let day = this.editPlayerForm.value.editDay;

    if (month.length === 1) month = "0" + month;
    if (day.length === 1) day = "0" + day;
    this.selectedPlayer.birth = this.editPlayerForm.value.editYear + "-" + month + "-" + day;
    
    this.selectedPlayer.weight = parseInt(this.editPlayerForm.value.editWeight, 10);
    this.selectedPlayer.tag_id = parseInt(this.editPlayerForm.value.editTagId, 10);
  	
    this._dataService.editPlayer(this.selectedPlayer)
  		.subscribe(
  			res => {
  				this.selectedPlayer = res;
          this.successfulEdit = true;
          this.edited = true;

          //wait 3 Seconds and hide
          setTimeout(function() {
            this.edited = false;
          }.bind(this), this.alertTO);

  				this.getPlayers();	// nasty complete reload...
  			},
  			error => console.log(error)
  		)
	}

	/* init add form */
  initAddForm() {
    this.createAddFormControls();
    this.createAddForm();
  }

  /* create add form controls */
  createAddFormControls() {
    this.addFirstname = new FormControl('', [
      Validators.required,
      Validators.pattern('^[a-zA-Z]+$')
    ]);
    this.addLastname = new FormControl('', [
      Validators.required,
      Validators.pattern('^[a-zA-Z]+$')
    ]);
    this.addYear = new FormControl('', [
      Validators.required
    ]);
    this.addMonth = new FormControl('', [
      Validators.required
    ]);
    this.addDay = new FormControl('', [
      Validators.required
    ]);
    this.addWeight = new FormControl('', [
      Validators.required,
      Validators.min(10),
      Validators.max(250),
      Validators.pattern('^[0-9]+$')
    ]);
    this.addTagId = new FormControl('', [
      Validators.required,
      Validators.pattern('^[0-9]+$')
    ]);
  }

  /* create add form */
  createAddForm() {
    this.addPlayerForm = new FormGroup({
      addFirstname: this.addFirstname,
      addLastname: this.addLastname,
      addYear: this.addYear,
      addMonth: this.addMonth,
      addDay: this.addDay,
      addWeight: this.addWeight,
      addTagId: this.addTagId
    });

    this.selectedPlayer = new Player();
    this.view = "add";
  }

	/* change form for current players */
	onCancelNewPlayerClick() {
    this.selectedPlayer = this.players[0];
    this.initEditForm();
	}

	/* add new player */
	onAddPlayer() {
    this.selectedPlayer.player_id = null;
    this.selectedPlayer.firstname = this.addPlayerForm.value.addFirstname;
    this.selectedPlayer.lastname = this.addPlayerForm.value.addLastname;

    // append '0' if necessary 
    let month = this.addPlayerForm.value.addMonth;
    let day = this.addPlayerForm.value.addDay;

    if (month.length === 1) month = "0" + month;
    if (day.length === 1) day = "0" + day;

    this.selectedPlayer.birth = this.addPlayerForm.value.addYear + "-" + month + "-" + day;
    this.selectedPlayer.weight =  parseInt(this.addPlayerForm.value.addWeight, 10);
    this.selectedPlayer.tag_id =  parseInt(this.addPlayerForm.value.addTagId, 10);
    this.selectedPlayer.team_id = parseInt(this.getTeamId(), 10);

    this._dataService.addPlayer(this.selectedPlayer)
			.subscribe(
				res => {
					this.selectedPlayer = res;
          this.successfulAdd = true;
          this.added = true;

          //wait 3 Seconds and hide
          setTimeout(function() {
            this.added = false;
          }.bind(this), this.alertTO);

					this.getPlayers();	// nasty complete reload...
				},
				error => console.log(error)
			)
	}

	/* delete player */
	/* good practice to name methods "onSth..." to indicate,
   * that they are being triggered from the template.
   */
	onDeletePlayer() {
    this.modalRef.close();
		this._dataService.deletePlayer(this.selectedPlayer.player_id)
			.subscribe(
				res => {
					this.successfulDelete = true;
          this.deleted = true;

          //wait 3 Seconds and hide
          setTimeout(function() {
            this.deleted = false;
          }.bind(this), this.alertTO);

					this.getPlayers();	// nasty..
				},
				error => console.log(error)
			)
	}

  /* open the modal */
  open(content) {
    this.playerToDelete = this.selectedPlayer;
    this.modalRef = this._modalService.open(content);
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
}