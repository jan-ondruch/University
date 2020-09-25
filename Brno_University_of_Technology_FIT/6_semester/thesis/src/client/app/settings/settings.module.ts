import { NgModule }      from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { FormsModule }   from '@angular/forms';
 
import { SettingsComponent }  from './settings.component';
 
@NgModule({
  imports: [
    BrowserModule,
    FormsModule
  ],
  declarations: [
    SettingsComponent
  ],
  bootstrap: [ SettingsComponent ]
})
export class SettingsModule { }