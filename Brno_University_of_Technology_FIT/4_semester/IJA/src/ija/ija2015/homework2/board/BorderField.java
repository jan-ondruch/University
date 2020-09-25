package ija.ija2015.homework2.board;

import java.io.Serializable;

public class BorderField implements Field {

	
	public BorderField() {}
	
        @Override
	public void addNextField(Field.Direction dirs, Field field) {}
	
        @Override
	public Field nextField(Field.Direction dirs) {
		
		return null;	
	}
        
        
        @Override
        public void freeze() {   
        }
        
        @Override
        public void unfreeze() {
        }
        
        @Override
        public boolean checkIfFrozen() {
            return false;
        }
	
        
        @Override
	public boolean putDisk(Disk disk) {
		return false;		
	}
	
        @Override
	public Disk getDisk() {
		
		return null;
	}
	
	@Override
	public boolean checkWasFrozen(){return false;}
	
	 public void setHigh(){
             
	 }
	 @Override
	 public boolean checkHigh(){
		 return false;
	 }
	
	 @Override
	  public void unsetMyHigh(){
		
	 }
	
}
