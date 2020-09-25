import { Component } from '@angular/core';
import { DataService } from '../services/data.service';
import { Router, ActivatedRoute } from '@angular/router';

import { Account, Team } from '../data-model';

import { FormGroup, FormControl, Validators } from '@angular/forms';

import { AuthenticationService } from '../services/authentication.service';
import { AlertService } from '../services/alert.service';

import { AuthGuard } from '../auth/index';

@Component ({
  selector: 'login',
  templateUrl: './login.component.html',
  styleUrls: ['./login.component.css']
})

export class LoginComponent {

  account: Account;

	teams: Team[];
	authResp: string = ""; // authentication response
  denied: boolean = false;  // access grant

  loginForm: FormGroup;
  email: FormControl;
  password: FormControl;

  successfulReg: boolean = false;  // successful registration
  registered: boolean = false;  // timeout

  successfulLogout: boolean = false;
  loggedout: boolean = false;

  alertTO: number = 3000; // alert timeout


	constructor(
		private _dataService: DataService,
		private _router: Router,
		private _route: ActivatedRoute,
    private _authenticationService: AuthenticationService,
    private _authGuard: AuthGuard,
    private _alertService: AlertService
	) { }

  ngOnInit() {
    if (this._authGuard.permissionDenied === true) {
      this.denied = true;
    }

    // alert for successful registration
    if (this._alertService.regSuccess === true) {
      this.successfulReg = true;
      this.registered = true;

      // wait 3 Seconds and hide
      setTimeout(function() {
        this._alertService.regReset();
        this.registered = false;
      }.bind(this), this.alertTO);
    }

    // alert for successful logout
    if (this._alertService.logoutSuccess === true) {
      this.successfulLogout = true;
      this.loggedout = true;

      // wait 3 Seconds and hide
      setTimeout(function() {
        this._alertService.logoutReset();
        this.loggedout = false;
      }.bind(this), this.alertTO);
    }

    // reset login status
    this._authenticationService.logout();

    this.createFormControls();
    this.createForm();
  }

  /* in case the component is closed before the timeout */
  ngOnDestroy() {
    this._alertService.regReset();
    this.registered = false;
    
    this._alertService.logoutReset();
    this.loggedout = false;
  }

  /* create controls for the form */
  createFormControls() {
    this.account = new Account();
    this.account.email = "";  // prefill here for debug
    this.account.password = "";

    this.email = new FormControl(this.account.email, [
      Validators.required
    ]);
    this.password = new FormControl(this.account.password, [
      Validators.required
    ]);
  }

  /* create new session form */
  createForm() {
    this.loginForm = new FormGroup({
      email: this.email,
      password: this.password
    });
  }

  /* login is valid, navigates to the first team in the array of teams */
	validLogin() {
		let id = this.teams[0].team_id;
		this._router.navigate(['teams', id, 'dashboard']);
	}

	/* simple authentication */
	authenticate() {
    this.account.email = this.loginForm.value.email;
    this.account.password = this.loginForm.value.password;

		this._authenticationService.login(this.account.email, this.account.password)
			.subscribe(
				res => {
          if (res === true) {
            // login successful
            this.getTeams();
          } else {
            // login failed
            this.authResp = 'invalid';
          }
				},
				error => console.error(error)
			)
	}

	// get all teams to resolve the dashboard path
	getTeams() {
		this._dataService.getTeams()
			.subscribe(
				res => this.teams = res,
				error => console.error(error),
        () => this.validLogin()
			);
	}

}