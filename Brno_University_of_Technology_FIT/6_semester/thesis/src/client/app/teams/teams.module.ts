import { NgModule }      from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { FormsModule }   from '@angular/forms';
 
import { TeamsComponent }  from './teams.component';
 
@NgModule({
  imports: [
    BrowserModule,
    FormsModule
  ],
  declarations: [
    TeamsComponent
  ],
  bootstrap: [ TeamsComponent ]
})
export class TeamsModule { }