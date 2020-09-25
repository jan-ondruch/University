package ija.ija2015.homework2.game;

import ija.ija2015.homework2.board.Field;
import ija.ija2015.homework2.board.Rules;
import ija.ija2015.homework2.board.BoardField;
import ija.ija2015.homework2.board.BorderField;
import java.io.Serializable;

/*
 * Reprezentuje pravidla inicializace hry Reversi.
 * */

public class ReversiRules implements Rules {

	private int size;
	public Field [][] fields;
	
	
	/*
	 * Inicializace pravidel
	 * */
	public ReversiRules(int size) {
		
		this.size = size;
		fields = new Field[(this.size + 2)][(this.size + 2)];
		
		for (int row = 0; row <= this.size + 1; row++) {
			for (int col = 0; col <= this.size + 1; col++) {
			
				if (row == 0 || col == 0 || row == this.size + 1 || col == this.size + 1) {
					fields[row][col] = new BorderField();
				}
				else {	
					fields[row][col] = this.createField(row, col);
				}	
			}
		}
		
		for (int row = 0; row <= this.size + 1; row++) {
            for (int col = 0; col <= this.size + 1; col++) {
            	
                Field actualField = fields[row][col];
                
                // instanceof testuje na BoardField, abych nevolal metodu do BorderField
                if (actualField instanceof BoardField) {
                    actualField.addNextField(Field.Direction.U, fields[row - 1][col]);
                    actualField.addNextField(Field.Direction.D, fields[row + 1][col]);
                    actualField.addNextField(Field.Direction.L, fields[row][col - 1]);
                    actualField.addNextField(Field.Direction.LU, fields[row - 1][col - 1]);
                    actualField.addNextField(Field.Direction.LD, fields[row + 1][col - 1]);
                    actualField.addNextField(Field.Direction.R, fields[row][col + 1]);
                    actualField.addNextField(Field.Direction.RU, fields[row - 1][col + 1]);
                    actualField.addNextField(Field.Direction.RD, fields[row + 1][col + 1]);      
                }
            }
        }
		
	}
		
	/*
	 * Vrací velikost desky.
	 * */
	public int getSize() {
		
		return this.size;
	}

	/*
	 * Vrací počet kamenů jednotlivých hráčů.
	 * */
	public int numberDisks() {

		return this.size*this.size;
	}

	/*
	 * Vytvoří odpovídající pole na zadaných indexech.
	 * */
	public Field createField(int row, int col) {

		return new BoardField(row, col); 	
	}
	
	/*
	 * Vrátí pole umístěné na uvedených indexech row a col.
	 * */
	public Field getField(int row, int col) {
		
		return fields[row][col];
	}

}
