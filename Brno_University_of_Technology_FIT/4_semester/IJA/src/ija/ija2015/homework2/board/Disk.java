package ija.ija2015.homework2.board;

import java.io.Serializable;

public class Disk  implements Serializable{

	
	protected boolean disk; // white = true
	
	
	public Disk(boolean isWhite) {
		
            this.disk = isWhite == true;
	}
	
	public void turn() {
		
		this.disk = !(this.disk);	
	}
	
	public boolean isWhite() {
		
		return this.disk;
	}
	
	public boolean getDisk(){
		return this.disk;
	}
	
	@Override
	public boolean equals(Object obj) {
		
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		Disk other = (Disk) obj;
		return disk == other.disk;	
	}
	
	@Override
	public int hashCode() {
		
		final int prime = 31;
		int result = 1;
		result = prime * result + (disk ? 1231 : 1237);
		return result;	
	}
	
	
}
