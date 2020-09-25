import { Component, Input } from '@angular/core';
import { Observable } from 'rxjs/Rx';
import { ActivatedRoute, Router } from '@angular/router';

import { DataService } from '../services/data.service';
import { SharedService } from '../services/shared.service';

import { Team } from '../data-model';

import { NgbModal, ModalDismissReasons } from '@ng-bootstrap/ng-bootstrap';
import { FormGroup, FormControl, Validators } from '@angular/forms';


@Component ({
  selector: 'teams',
  templateUrl: './teams.component.html',
  styleUrls: ['./teams.component.css']
})

export class TeamsComponent {

	private teams: Team[];	// all teams
  selectedTeam: Team;   // currently selected team
  teamToDelete: Team;   // for the modal window
  
  editTeamForm: FormGroup;    // edit team
  addTeamForm: FormGroup;     // add new team

  editName: FormControl;
  addName: FormControl;

  view: any = "edit"; // view mode - edit | add

  recording: boolean = true;

  modalRef: any;  // ref to the modal window

  successfulEdit: boolean = false;  // successful form edit
  edited: boolean = false;  // timeout

  successfulAdd: boolean = false;  // successful form add
  added: boolean = false;  // timeout

  successfulDelete: boolean = false;  // successful form add
  deleted: boolean = false;  // timeout

  cantRemove: boolean = false;  // cannot remove the last team
  removed: boolean = false;  // timeout

  alertTO: number = 3000; // alert length


	constructor(
		private _dataService: DataService,
		private _router: Router,
		private _route: ActivatedRoute,
    private _sharedService: SharedService,
    private _modalService: NgbModal
	) {
    this.checkRecording();
		this.getTeams();
	}

	/* get all teams available to choose from */
	getTeams() {
		this._dataService.getTeams()
			.subscribe(
				res => {
					this.teams = res;
          this.selectedTeam = this.teams[0];  // selectedTeam
				},
				error => console.error(error),
        () => this.selectedTeam ? this.initEditForm() : this.initAddForm()
			)
	}

  /* init edit form */
  initEditForm() {
    this.createEditFormControls();
    this.createEditForm();
  }

  /* create edit form controls */
  createEditFormControls() {
    this.editName = new FormControl(this.selectedTeam.name, [
      Validators.required
    ]);
  }

  /* create edit form */
  createEditForm() {
    this.editTeamForm = new FormGroup({
      editName: this.editName
    });
    this.view = "edit";
  }

  /* change selectedTeam and the form to the given team */
  onTeamClick(team_id) {
    this.selectedTeam = this.teams.find(t => t.team_id === team_id);
    this.initEditForm();
  }

  /* edits the current team */
  onEditTeam() {
    this.selectedTeam.name = this.editTeamForm.value.editName;

    this._dataService.editTeam(this.selectedTeam)
      .subscribe(
        res => {
          this.selectedTeam = res;
          this.successfulEdit = true;
          this.edited = true;

          //wait 3 Seconds and hide
          setTimeout(function() {
            this.edited = false;
          }.bind(this), this.alertTO);

          this.getTeams();  // nasty complete reload...
        },
        error => console.log(error),
        () => this._sharedService.teamsChange()
      )
  }

  /* init add form */
  initAddForm() {
    this.createAddFormControls();
    this.createAddForm();
  }

  /* create add form controls */
  createAddFormControls() {
    this.addName = new FormControl('', [
      Validators.required
    ]);
  }

  /* create add form */
  createAddForm() {
    this.addTeamForm = new FormGroup({
      addName: this.addName
    });
    this.selectedTeam = new Team();
    this.view = "add";
  }

  /* change form for current teams */
  onCancelNewTeamClick() {
    this.selectedTeam = this.teams[0];
    this.initEditForm();
  }

  /* add new team */
  onAddTeam() {
    this.selectedTeam.team_id = null;
    this.selectedTeam.email = "";
    this.selectedTeam.name = this.addTeamForm.value.addName;

    this._dataService.addTeam(this.selectedTeam)
      .subscribe(
        res => {
          this.selectedTeam = res;
          this.successfulAdd = true;
          this.added = true;

          // wait 3 Seconds and hide
          setTimeout(function() {
            this.added = false;
          }.bind(this), this.alertTO);

          this.getTeams();  // get modified teams list
        },
        error => console.log(error),
        () => this._sharedService.teamsChange()
      )
  }

  /* delete team */
  /* that they are being triggered from the template.
   */
  onDeleteTeam() {
    // forbid removing a team if it's the only one left
    if (this.teams.length <= 1) {
      this.modalRef.close();
      this.cantRemove = true;
      this.removed = true;

      // wait 3 Seconds and hide
      setTimeout(function() {
        this.removed = false;
      }.bind(this), this.alertTO);
    } else {
      this.modalRef.close();
      this._dataService.deleteTeam(this.teamToDelete.team_id)
        .subscribe(
          res => {
            this.successfulDelete = true;
            this.deleted = true;

            // wait 3 Seconds and hide
            setTimeout(function() {
              this.deleted = false;
            }.bind(this), this.alertTO);

            this.getTeams();  // get modified teams list
          },
          error => console.log(error),
          () => this._sharedService.teamsChange()
        )
    }
  }

  /* open the modal */
  open(content) {
    this.teamToDelete = this.selectedTeam;
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