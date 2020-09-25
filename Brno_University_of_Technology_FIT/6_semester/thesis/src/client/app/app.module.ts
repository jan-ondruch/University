import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { RouterModule, Router, Routes } from '@angular/router';
import { NgIf } from '@angular/common';
import { HttpModule }    from '@angular/http';
import { FormsModule }   from '@angular/forms';
import { ReactiveFormsModule } from '@angular/forms';
import { HttpClientModule } from '@angular/common/http';

import { DataService } from './services/data.service';
import { SharedService } from './services/shared.service';
import { AuthenticationService } from './services/authentication.service';
import { AlertService } from './services/alert.service';

import { AuthGuard } from './auth/index';

import { routing } from './app.routing';

import { AppComponent } from './app.component';
import { HomeComponent } from './home/home.component';
import { LoginComponent } from './login/login.component';
import { RegistrationComponent } from './registration/registration.component';
import { NavigationComponent } from './navigation/navigation.component';
import { DashboardComponent } from './dashboard/dashboard.component';
import { NewSessionComponent } from './newSession/newSession.component';
import { OnlineComponent } from './online/online.component';
import { PlayersComponent } from './players/players.component';
import { TeamsComponent } from './teams/teams.component';
import { EventComponent } from './event/event.component';
import { SettingsComponent } from './settings/settings.component';

import { ArraySortPipe } from './array-sort.pipe';
import { TimeToDatePipe } from './time-to-date.pipe';

import { NgbModule } from '@ng-bootstrap/ng-bootstrap';

import { AngularFontAwesomeModule } from 'angular-font-awesome';

import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { CalendarModule } from 'angular-calendar';
import { CommonModule } from '@angular/common';
import { DemoUtilsModule } from './calendar-utils/module';
import { CalendarComponent } from './calendar/calendar.component';

@NgModule({
  declarations: [
    AppComponent,
    HomeComponent,
    LoginComponent,
    RegistrationComponent,
    NavigationComponent,
    DashboardComponent,
    NewSessionComponent,
    OnlineComponent,
    PlayersComponent,
    TeamsComponent,
    EventComponent,
    SettingsComponent,
    ArraySortPipe,
    TimeToDatePipe,
    CalendarComponent
  ],
  imports: [
    BrowserModule,
    HttpClientModule,
    HttpModule,
    FormsModule,
    ReactiveFormsModule,
    NgbModule.forRoot(),
    AngularFontAwesomeModule,
    BrowserAnimationsModule,
    CalendarModule.forRoot(),
    CommonModule,
    DemoUtilsModule,
    routing
  ],
  providers: [
    DataService,
    SharedService,
    AuthenticationService,
    AlertService,
    AuthGuard
  ],
  bootstrap: [AppComponent]
})

export class AppModule {
	constructor(private router: Router) {}
}