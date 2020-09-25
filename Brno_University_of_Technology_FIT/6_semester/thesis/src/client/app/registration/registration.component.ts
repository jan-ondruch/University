import { Component } from '@angular/core';
import { DataService } from '../services/data.service';
import { Router, ActivatedRoute } from '@angular/router';

import { Account, Team } from '../data-model';
import { SharedService } from '../services/shared.service';
import { AlertService } from '../services/alert.service';

import { FormGroup, FormControl, Validators } from '@angular/forms';


@Component ({
  selector: 'registration',
  templateUrl: './registration.component.html',
  styleUrls: ['./registration.component.css']
})

export class RegistrationComponent {

  account: Account;

  registrationForm: FormGroup;
  email: FormControl;
  password: FormControl;

  alreadyUsed: boolean = false; // already used email flag


  constructor(
    private _dataService: DataService,
    private _router: Router,
    private _route: ActivatedRoute,
    private _sharedService: SharedService,
    private _alertService: AlertService
  ) { }

  ngOnInit() {
    this.createFormControls();
    this.createForm();
  }

  /* create controls for the form */
  createFormControls() {
    this.account = new Account();
    this.account.email = "";
    this.account.password = "";

    this.email = new FormControl(this.account.email, [
      Validators.required,
      Validators.email
    ]);
    this.password = new FormControl(this.account.password, [
      Validators.required
    ]);
  }

  /* create new session form */
  createForm() {
    this.registrationForm = new FormGroup({
      email: this.email,
      password: this.password
    });
  }

  /* simple authentication */
  register() {
    this.account.email = this.registrationForm.value.email;
    this.account.password = this.registrationForm.value.password;

    this._dataService.registerAccount(this.account)
      .subscribe(
        res => {
          if (Object.keys(res.data).length === 0 && res.data.constructor === Object)  {
            this.alreadyUsed = true;
          } else {
            this._alertService.regSet();
          } 
        },
        error => {
          console.log(error);
        }
      )
  }
}