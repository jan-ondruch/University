import { Injectable } from '@angular/core';
import { Router, CanActivate } from '@angular/router';
import * as moment from 'moment';
 
@Injectable()
export class AuthGuard implements CanActivate {

  public permissionDenied: boolean = false; // set it to notify the login component
 
  constructor(private router: Router) { }
 
  /* permission for route access */
  canActivate() {
    if (localStorage.getItem('id_token')) {
      // check expiration first
      if (this.isLoggedIn()) {  // logged in so return true
        this.permissionDenied = false;
        return true;
      } else {
        return false;
      }
    } else { // not logged in so redirect to login page
      this.permissionDenied = true;
      this.router.navigate(['login']);
      return false;
    }
  }

  /* check if user is still logged in */
  isLoggedIn() {
    return moment().isBefore(this.getExpiration());
  }

  /* log out the user */
  isLoggedOut() {
    return !this.isLoggedIn();
  }

  /* get expiration of the token */
  getExpiration() {
    const expiration = localStorage.getItem("expires_at");
    const expiresAt = JSON.parse(expiration);
    return moment(expiresAt);
  } 
}