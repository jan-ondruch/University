import { NgModule }      from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { FormsModule }   from '@angular/forms'; // <-- NgModel lives here
 
import { CompanyComponent }  from './company.component';
 
@NgModule({
  imports: [
    BrowserModule,
    FormsModule // <-- import the FormsModule before binding with [(ngModel)]
  ],
  declarations: [
    CompanyComponent
  ],
  bootstrap: [ CompanyComponent ]
})
export class CompanyModule { }