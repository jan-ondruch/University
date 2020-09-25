package ija.ija2015.homework2.board;

import ija.ija2015.homework2.board.Disk;
import ija.ija2015.homework2.board.Field;
import java.io.Serializable;


public class BoardField extends java.lang.Object implements Field {

	private final int row;
	private final int col; 
	
	private Disk disk;
	private final Field[] sideFields;
        
        //transient private GuiField gField;
        private boolean isFrozen = false;
		private boolean wasFrozen = false;
		private boolean high = false;

	public BoardField(int row, int col) {
		
		this.row = row;
		this.col = col;	
		this.disk = null;
		this.sideFields = new Field[8];
                this.isFrozen = false;
	}
	@Override
	 public void setHigh(){
		 this.high = true;
	 }
	 
	 @Override
	 public void unsetMyHigh(){
		 this.high = false;
	 }
	 
        @Override
	 public boolean checkHigh(){
		 return this.high;
	 }
        
        @Override
        public void freeze() {
            
            this.isFrozen = true;
			this.wasFrozen = true;
            System.out.println("Freezing: " + this.row + ", " + this.col);
            
        }
        
        @Override
        public void unfreeze() {
            
            this.isFrozen = false;
            
            try {
            } catch (NullPointerException e) {}
            
        }
        
        @Override
        public boolean checkIfFrozen() {
            return this.isFrozen;
        }
		
        @Override
		public boolean checkWasFrozen(){
			return this.wasFrozen;
		}
        
        
        @Override
	public void addNextField(Field.Direction dirs, Field field) {
		
		this.sideFields[dirs.ordinal()] = field;
	}
	
	
        @Override
	public Field nextField(Field.Direction dirs) {

		return this.sideFields[dirs.ordinal()];
	}
	
	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		BoardField other = (BoardField) obj;
		if (col != other.col)
			return false;
		if (disk == null) {
			if (other.disk != null)
				return false;
		} else if (!disk.equals(other.disk))
			return false;
		return row == other.row;
	}
	
	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + col;
		result = prime * result + ((disk == null) ? 0 : disk.hashCode());
		result = prime * result + row;
		return result;
	}
	
	/*
	 * Vloží na pole kámen. Při vkládání se ověřuje soulad s pravidly hry.
	 * */
        @Override
	public boolean putDisk(Disk disk) {
		
		if (this.disk == null) {
			this.disk = disk;
			return true;
		}
		else {
			return false;
		}
	}
	
	/*
	 * Vrací kámen, který je vložen na pole.
	 * */
        @Override
	public Disk getDisk() {
		
		if (this.disk != null) return this.disk;
		else return null;
	}
	

	
}
