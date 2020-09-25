import { NgModule }      from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { FormsModule }   from '@angular/forms';
 
import { LoginComponent }  from './login.component';
 
@NgModule({
  imports: [
    BrowserModule,
    FormsModule
  ],
  declarations: [
    LoginComponent
  ],
  bootstrap: [ LoginComponent ]
})
export class LoginModule { }