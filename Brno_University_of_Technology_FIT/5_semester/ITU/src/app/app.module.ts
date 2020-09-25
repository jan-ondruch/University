import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { RouterModule, Router, Routes } from '@angular/router';
import { NgIf } from '@angular/common';
import { HttpModule }    from '@angular/http';
import { FormsModule }   from '@angular/forms';

// Imports for loading & configuring the in-memory web api

import { AppComponent } from './app.component';
import { CarRegistrationComponent } from './car-registration/car-registration.component';
import { PageNotFoundComponent } from './not-found.component';
import { DashboardComponent } from './dashboard/dashboard.component';
import { LoginComponent } from './login/login.component';
import { CarComponent } from './car/car.component';
import { PersonComponent } from './person/person.component';
import { CompanyComponent } from './company/company.component';
import { HistoryComponent } from './history/history.component';


const appRoutes: Routes = [
  { path: 'car-registration', component: CarRegistrationComponent },
  { path: 'login', component: LoginComponent },
  { path: 'dashboard', component: DashboardComponent },
  { path: 'car', component: CarComponent },
  { path: 'person', component: PersonComponent },
  { path: 'company', component: CompanyComponent },
  { path: 'history', component: HistoryComponent },
  { path: '**', component: PageNotFoundComponent },
];

@NgModule({
  declarations: [
    AppComponent,
    CarRegistrationComponent,
    DashboardComponent,
    CarComponent,
    PersonComponent,
    CompanyComponent,
    HistoryComponent,
    LoginComponent,
    PageNotFoundComponent
  ],
  imports: [
  	RouterModule.forRoot(
      appRoutes,
      { enableTracing: true } // <-- debugging purposes only
    ),
    BrowserModule,
    HttpModule,
    FormsModule
  ],
  bootstrap: [AppComponent]
})
export class AppModule { 
  constructor(private router: Router) {}
}
