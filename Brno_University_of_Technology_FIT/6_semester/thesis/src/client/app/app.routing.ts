import { Routes, RouterModule } from '@angular/router';
 
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

import { AuthGuard } from './auth/index';
 
const appRoutes: Routes = [
  { 
    path: '',
    component: HomeComponent
  },
  { 
    path: 'login',
    component: LoginComponent
  },
  {
    path: 'registration',
    component: RegistrationComponent
  },
  {
    path: 'teams/:id',
    component: NavigationComponent,
    canActivate: [AuthGuard],
    children: [
      {
        path: 'dashboard',
        component: DashboardComponent
      },
      {
        path: 'new-session',
        component: NewSessionComponent
      },
      {
        path: 'teams',
        component: TeamsComponent
      },
      {
        path: 'players',
        component: PlayersComponent
      },
      {
        path: 'online/:sid',
        component: OnlineComponent,
      },
      {
        path: 'events/:eid',
        component: EventComponent
      },
      {
        path: 'settings',
        component: SettingsComponent
      }
    ]
  },
  {
    path: '**',
    component: HomeComponent 
  }
];
 
export const routing = RouterModule.forRoot(appRoutes);