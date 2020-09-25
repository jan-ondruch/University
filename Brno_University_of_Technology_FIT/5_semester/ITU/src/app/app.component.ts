import { Component, OnInit } from '@angular/core';
import { Location } from '@angular/common';
import { Router } from '@angular/router';

// jQuery import
declare var jquery:any;
declare var $ :any;

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})

export class AppComponent {
	constructor(private router: Router) { }
	
	changeMode() {
		var color = $('body').css("background-color");
		console.log(color);
		if (color === 'rgb(204, 204, 204)') {
			$('body').css('background-color', "#fff");
		}
		else {
			$('body').css('background-color', "#ccc");
		}
	}
}