// tady nic nemusite editovat, jenom kdyztak v tom PersonComponent muzete psat jQuery
// pokud chcete jQuery pouzit v jinych komponentech (treba 'car'), 
// tak musite 'declare' ty 2 radky nize (treba v souboru car.component.ts), to je vse a bude to slapat
// v podstate vam staci editovat .html a .css file, jestli chcete pouzivat jQuery, 
// tak i tenhle soubor (deklarace + jQuery v xyzComponent)
import { Component } from '@angular/core';

// jQuery import
declare var jquery:any;
declare var $ :any;

@Component ({
   selector: 'person',
   templateUrl: './person.component.html',
   styleUrls: ['./person.component.css']
})

export class PersonComponent {
	// here you type jQuery code
	title = 'Ukazka jQuery';
	  toggleTitle(){
	    $('.title').slideToggle(); //
	  }
}