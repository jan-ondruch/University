import { NgModule }      from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { FormsModule }   from '@angular/forms';
 
import { PlayersComponent }  from './players.component';
 
@NgModule({
  imports: [
    BrowserModule,
    FormsModule
  ],
  declarations: [
    PlayersComponent
  ],
  bootstrap: [ PlayersComponent ]
})
export class PlayersModule { }