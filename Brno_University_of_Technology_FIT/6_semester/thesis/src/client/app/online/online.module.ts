import { NgModule }      from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { FormsModule }   from '@angular/forms';
 
import { OnlineComponent }  from './online.component';
 
@NgModule({
  imports: [
    BrowserModule,
    FormsModule
  ],
  declarations: [
    OnlineComponent
  ],
  bootstrap: [ OnlineComponent ]
})
export class OnlineModule { }