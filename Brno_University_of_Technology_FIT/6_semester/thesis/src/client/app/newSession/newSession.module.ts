import { NgModule }      from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { FormsModule }   from '@angular/forms';
 
import { NewSessionComponent }  from './newSession.component';
 
@NgModule({
  imports: [
    BrowserModule,
    FormsModule
  ],
  declarations: [
    NewSessionComponent
  ],
  bootstrap: [ NewSessionComponent ]
})
export class NewSessionModule { }