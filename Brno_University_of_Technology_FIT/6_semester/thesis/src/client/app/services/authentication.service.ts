import { Injectable } from '@angular/core';
import { Http, Headers, Response, RequestOptions } from '@angular/http';
import { Observable } from 'rxjs';
import * as moment from 'moment';

import 'rxjs/add/operator/map';

 
@Injectable()
export class AuthenticationService {
  public successfulyLoggedOut: boolean = false;

  headers: Headers;
  options: RequestOptions;
 
  constructor(private _http: Http) {

    this.headers = new Headers({ 'Content-Type': 'application/json' });
    this.options = new RequestOptions({ headers: this.headers }); 
  }
 
  /* log into the app */
  login(email: string, password: string): Observable<boolean> {
    let body = JSON.stringify({ email: email, password: password });
    return this._http.post("/api/login", body, this.options)
      .map((response: Response) => {
        // login successful if there's a jwt token in the response
        let token = response.json().data;

        if (token.idToken) {  // the login has been successful
          const expiresAt = moment().add(token.expiresIn, 'second');

          // save the token into localStorage
          localStorage.setItem('id_token', token.idToken);
          localStorage.setItem("expires_at", JSON.stringify(expiresAt.valueOf()) );
          
          return true;
        } else {
          return false;
        }
      });
  }
 
  /* logout from the app */
  logout(): void {
    // clear token remove user from local storage to log user out
    localStorage.removeItem("id_token");
    localStorage.removeItem("expires_at");
  }
}