import { NgModule }      from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { FormsModule }   from '@angular/forms';
import { HttpClientModule } from '@angular/common/http';
 
import { DashboardComponent }  from './dashboard.component';
import { CalendarComponent } from '../calendar/calendar.component';
 
@NgModule({
  imports: [
    BrowserModule,
    HttpClientModule,
    FormsModule
  ],
  declarations: [
    DashboardComponent,
    CalendarComponent
  ],
  bootstrap: [ DashboardComponent ]
})
export class DashboardModule { }