package ija.ija2015.homework2.board;

import java.io.Serializable;

public interface Field extends Serializable {

	
	public static enum Direction implements java.io.Serializable {

		L,
		D,
		R,
		U,
		LU,
		RU,
		RD,
		LD	
	}
	
	boolean putDisk(ija.ija2015.homework2.board.Disk disk);
	void addNextField(Field.Direction dirs, Field field);
	ija.ija2015.homework2.board.Disk getDisk();
	Field nextField(Field.Direction dirs);
        public void freeze();
        public void unfreeze();
        public boolean checkIfFrozen();
		public boolean checkWasFrozen();
		public void setHigh();
		public boolean checkHigh();
		public void unsetMyHigh();
        
}
