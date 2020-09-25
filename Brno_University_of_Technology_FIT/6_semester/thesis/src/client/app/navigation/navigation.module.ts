import { NgModule }      from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { FormsModule }   from '@angular/forms';
 
import { NavigationComponent }  from './navigation.component';

import { NgbModule } from '@ng-bootstrap/ng-bootstrap';

 
@NgModule({
  imports: [
    BrowserModule,
    FormsModule,
    NgbModule
  ],
  declarations: [
    NavigationComponent
  ],
  bootstrap: [ NavigationComponent ]
})
export class NavigationModule { }