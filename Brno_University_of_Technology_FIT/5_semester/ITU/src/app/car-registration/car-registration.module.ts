import { NgModule }      from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { FormsModule }   from '@angular/forms'; // <-- NgModel lives here
 
import { CarRegistrationComponent }  from './car-registration.component';
 
@NgModule({
  imports: [
    BrowserModule,
    FormsModule // <-- import the FormsModule before binding with [(ngModel)]
  ],
  declarations: [
    CarRegistrationComponent
  ],
  bootstrap: [ CarRegistrationComponent ]
})
export class CarRegistrationModule { }