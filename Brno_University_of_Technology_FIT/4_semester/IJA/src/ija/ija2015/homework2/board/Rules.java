package ija.ija2015.homework2.board;

import java.io.Serializable;

/*
 * Rozhraní reprezentující pravidla inicializace hry.
 * */

public interface Rules extends Serializable {

	int getSize();
	int numberDisks();
	Field createField(int row, int col);
	Field getField(int row, int col);
	
}
