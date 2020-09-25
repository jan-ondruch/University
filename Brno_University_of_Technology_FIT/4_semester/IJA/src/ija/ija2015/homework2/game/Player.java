package ija.ija2015.homework2.game;
import ija.ija2015.homework2.board.Field;
import ija.ija2015.homework2.board.Board;
import ija.ija2015.homework2.board.Disk;
import ija.ija2015.homework2.board.BorderField;
import java.io.Serializable;
import java.util.*;

/*
 * Reprezentuje hráče hry. Hráč má bílou nebo černou barvu. 
 * Po vytvoření reprezentuje volného hráče. Součástí hráče je sada kamenů, 
 * které má k dispozici pro vkládání na desku. Volný hráč musí být 
 * inicializován v rámci hrací desky 
 * (vizte metodu init(ija.ija2015.homework2.board.Board)).
 * */

public class Player extends java.lang.Object  implements Serializable {

	private boolean player;	// (true = bílý, false = černý)
	private String name;
	private int score;
	private int turnedCount;
	public Disk[] diskset;
	private Board board;
	private int diskNum = 0;	// check in the 3rd HW - exact manipalation etc
	
	// zasobnik pro uchovani kamenu, ktere pripadne otocime
	List<Field> fieldStack = new ArrayList<Field>();
	
	public Player(boolean isWhite, String playerName) {
		
                this.player = isWhite == true;
		
		this.name=playerName;
		this.score=2;
	}
        
        /* pri nove hre se pocet kamenu vynuluje */
        public void setDiskNum() {
            this.diskNum = 2;   // first two are for initial 2 stones
        }
        
	public String getName(){
		return this.name;
	}
	
	public boolean isWhite() {
		
		return this.player;
	}
	
	public int getTurnedCount(){
		return this.turnedCount;
	}
	
	public void setScore(int minus){
		this.score = this.score - minus;
	}
	
	public int getScore(){
		return this.score;
	}
	
	@Override
	public String toString() {
		if (this.player == true)
			return "white";
		else
			return "black";
	}

	/*
	 * Test, zda je možné vložit nový kámen hráče na dané pole.
	 * Kámen se sady nevybírá ani nevkládá na pole.
	 * */
	public boolean canPutDisk(Field field) {
            
            
		// na poli jiz je kamen
		if (field.getDisk() != null)
			return false;
		
                
		Field tmpField;
		
                // vycisti zasobnik pro ukladani kamenu
		this.fieldStack.clear();
                boolean found = false;
                boolean flg_up = true;
                
                // lokalni tmp stack pro vypomoc v algorytmu
                List<Field> tmpStack = new ArrayList<Field>();

		// iteruj pres vsechny smery okolo pole
		for (Field.Direction dir : Field.Direction.values()) {
                    
                    tmpStack.clear();
                    
			// vedlejsi vubec neco je (neni tam prazdne pole)
			if (field.nextField(dir).getDisk() != null) {

				// cerny hrac -> bile kameny
				if ((field.nextField(dir).getDisk().isWhite() == true) && (this.isWhite() == false)) {
					tmpField = field.nextField(dir);
                                        tmpStack.add(tmpField);
                                        flg_up = true;  // borderfield error catching
                                        
					// cykli dokud nenarazis na okraj (vrat false) nebo na cerny kamen (vrat true)
					while (!(tmpField instanceof BorderField)) {
                                                
						// pozice neni prazdna a kamen na ni je cerny
						if (tmpField.getDisk() != null && 
								tmpField.getDisk().isWhite() == false) { 
                                                        found = true;
                                                        break;
                                                }
						
                                                if (flg_up != true)
                                                    tmpStack.add(tmpField);	// push kamenu na zasobnik
                                                
                                                flg_up = false;
						tmpField = tmpField.nextField(dir);
                                                
                                                // rada konci, stack vymazeme a nic nepridame do hlavniho stacku
                                                if (tmpField.getDisk() == null || tmpField instanceof BorderField) {
                                                    tmpStack.clear();
                                                    break;
                                                }
					}
				}
				
				// bily hrac -> cerne kameny
				if ((field.nextField(dir).getDisk().isWhite() == false) && (this.isWhite() == true)) {
					tmpField = field.nextField(dir);
                                        tmpStack.add(tmpField);
                                        flg_up = true;  // borderfield error catching

					// cykli dokud nenarazis na okraj (vrat false) nebo na bily kamen (vrat true)
					while (!(tmpField instanceof BorderField)) {
						
						// pozice neni prazdna a kamen na ni je bily
						if (tmpField.getDisk() != null && 
								tmpField.getDisk().isWhite() == true) {
                                                        found = true;
                                                        break;
                                                }
                                                
                                                if (flg_up != true)
                                                    tmpStack.add(tmpField);	// push kamenu na zasobnik
                                                
                                                flg_up = false;
						tmpField = tmpField.nextField(dir);
                                                
                                                // rada konci, stack vymazeme a nic nepridame do hlavniho stacku
                                                if (tmpField.getDisk() == null || tmpField instanceof BorderField) {
                                                    tmpStack.clear();
                                                    break;
                                                }
					}
				}
			}

                        if (flg_up == true) continue;
                        fieldStack.addAll(tmpStack);
                        
		}
       
		return found;
		
	}
	
	/*
	 * Test prázdnosti sady kamenů, které má hráč k dispozici.
	 * */
	public boolean emptyPool() {
	
            return this.diskset == null;
	}
	
	
	/*
	 * Vloží nový kámen hráče na dané pole, pokud to pravidla umožňují. 
	 * Pokud lze vložit, vybere kámen ze sady a vloží na pole.
	 * */
	public boolean putDisk(Field field) {		
         this.turnedCount = 0;       
                this.fieldStack.clear();
                
		// uz nemame k dispozici dalsi kameny
		if (emptyPool() == true) {
			return false;
                }
		
		// test vlozeni kamene na dane pole
		if (canPutDisk(field) == true) {
                    
                        if (field.putDisk(this.diskset[this.diskNum]) == false) {
                            return false;
                        }
			this.diskNum += 1;
			
			
			// otoc kameny
			for (Field fld : this.fieldStack) {
                                /* frozen*/
                                if (fld.checkIfFrozen()) {
                                    System.out.println("Neotocim sutr - Player!");
                                    continue;   // is frozen, we cannot use it
                                }
                                
				fld.getDisk().turn();
				this.turnedCount += 1;
			}
			this.score += this.turnedCount + 1;
			return true;
		}
		else {
			return false;
		}
		
		
	}
	
	/*
	 * Inicializace hráče v rámci hrací desky. Vytvoří sadu kamenů 
	 * o příslušné velikosti a umístí své počáteční kameny na desku.
	 * */
	public void init(Board board) {
	
		this.board = board;
		this.diskset = new Disk[this.board.size*this.board.size];
		
		// vytvor novou sadu kamenu pro hrace
                // sutru vytvorime i tak 2x vice ...
		for(int i = 0; i < this.diskset.length; i++) {
			
			if (this.player == true)	// white
				this.diskset[i] = new Disk(true);
			else				// black
				this.diskset[i] = new Disk(false);	
		}

		// umisti pocatectni kameny
		if (this.player == true) {	// white
			this.board.getField(this.board.size/2, this.board.size/2).putDisk(this.diskset[this.diskNum]);
			this.diskNum += 1;
			this.board.getField((this.board.size/2) + 1, (this.board.size/2) + 1).putDisk(this.diskset[this.diskNum]);
			this.diskNum += 1;
		}
		else {	// black
                        this.board.getField((this.board.size/2) + 1, this.board.size/2).putDisk(this.diskset[this.diskNum]);
			this.diskNum += 1;
			this.board.getField(this.board.size/2, (this.board.size/2) + 1).putDisk(this.diskset[this.diskNum]);
			this.diskNum += 1;
		}
		
	}
        
        /*
         * Vrat filedstack pro GUI
        * */
        public List<Field> getFieldStack() {
            
            return this.fieldStack;
            
        }
		
		
}
