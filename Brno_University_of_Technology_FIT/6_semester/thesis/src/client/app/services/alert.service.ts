import { Injectable } from '@angular/core';
import { Router } from '@angular/router';
 
@Injectable()
export class AlertService {
  public regSuccess: boolean = false; // set it to notify the login component
  public logoutSuccess: boolean = false;
 
  constructor(private _router: Router ) { }
 
  /* registration successful */
  regSet() {
    this.regSuccess = true;
    this._router.navigate(['login']);
  }

  /* reset registration status */
  regReset() {
    this.regSuccess = false;
  }

  /* successful logout */
  logoutSet() {
    this.logoutSuccess = true;
    this._router.navigate(['login']);
  }

  /* reset logout status */
  logoutReset() {
    this.logoutSuccess = false;
  }
}
