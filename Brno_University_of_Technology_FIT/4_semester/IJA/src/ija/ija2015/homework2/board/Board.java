package ija.ija2015.homework2.board;

import ija.ija2015.homework2.board.Field;
import java.io.Serializable;

/*
 * Inicializuje desku. Vytvoří a uloží si všechna pole. 
 * Pro všechna pole nastaví jejich okolí. 
 * Na neaktivních polích jsou umístěny instance třídy BorderField. 
 * Objekty aktivních polí jsou získány metodou Rules.createField(int, int).
 * */

public class Board extends java.lang.Object  implements Serializable {

	public int size;
	public Rules rules;
	
	
	/*
	 * Inicializuje desku.
	 * */
	public Board(Rules rules) {
		
		this.rules = rules;
		this.size = this.rules.getSize();	
	}
	
	/*
	 * Vrátí pole umístěné na uvedených indexech row a col.
	 * */
	public Field getField(int row, int col) {
		
		return this.rules.getField(row, col);
	}
	
	/*
	 * Vrací velikost (rozměr) desky.
	 * */
	public int getSize() {
		
		return this.size;	
	}
	
	/*
	 * Vrací objekt pravidel.
	 * */
	public Rules getRules() {
		
		return this.rules;
	}
	
}
