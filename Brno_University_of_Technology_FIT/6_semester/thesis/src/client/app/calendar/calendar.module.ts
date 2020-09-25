import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { DemoUtilsModule } from '../calendar-utils/module';
import { CalendarComponent }  from './calendar.component';
import { HttpClientModule } from '@angular/common/http';

@NgModule({
  imports: [
  ],
  declarations: [
    CalendarComponent
  ],
  bootstrap: [ CalendarComponent ]
})
export class CalendarModule { }