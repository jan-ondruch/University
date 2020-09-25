package ija.ija2015.homework2.game;

import ija.ija2015.homework2.board.Board;
import ija.ija2015.homework2.board.Field;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.FileOutputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import javax.swing.Timer;

/*
 * Reprezentuje hru. Při inicializaci se vždy přiřadí hrací deska.
 * */

public class Game extends java.lang.Object implements Serializable {

	private Board board;
	public Player player1, player2;		// player1 = white, player2 = black
	private boolean player1_move;		// bily je na rade, true - na rade, false - neni
	
        private boolean freeze;
        private boolean defrost;
        
        private int freezeStart;
        private int freezeTime;
        private int freezeCnt;
	
	/*
	 * Inicializuje hru.
	 * */
	public Game(Board board) {
		
		this.board = board;
		this.player1_move = true;	// zacina bily hrac
                
                this.freeze = false;
                this.defrost = false;
         
	}
		
        public void setFreeze(int a, int b, int c) {
            
            this.freezeStart = (int)(Math.random() * a);
            this.freezeTime = (int)(Math.random() * b);
            this.freezeCnt = c;
            this.freeze = true;
            this.freezeDisks();
            
        }
        
        private void freezeDisks() {
            
        ActionListener frStart = new ActionListener() {
        @Override
        public void actionPerformed(ActionEvent evt) {
            
            int frozen = 0;
            int i, j;
            Field temp;

            while (frozen < freezeCnt) {
                i = 1 + (int)(Math.random() * board.getSize());
                j = 1 + (int)(Math.random() * board.getSize());

                temp = board.getField(i, j);
                if ((temp.getDisk() != null ) && !temp.checkIfFrozen()) {
                    board.getField(i, j).freeze();
                    frozen++;
                }
            }

            ActionListener frEnd = new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent evt) {
                    //System.out.println("Cele to odmrznu po skonceni tahu");
                    defrost = true;
                }
            };

                Timer timer = new Timer(freezeTime * 1000 ,frEnd);
                timer.setRepeats(false);
                timer.start();
        }
        };
        
            Timer timer = new Timer(this.freezeStart * 1000,frStart);
            timer.setRepeats(false);
            timer.start();
        }
        
        public boolean getDefrostTime() {
            return this.defrost;
        }
    
        public void unfreezeDisks() {
            this.disableFreezing();
            for (int i = 1; i <= this.board.getSize(); i++)
                for (int j = 1; j <= this.board.getSize(); j++) {
                    this.board.getField(i, j).unfreeze();
                }
        
            try {
                //this.gui.freezeButtonsOff();
            } catch (NullPointerException e) {}
        }
        
        public void disableFreezing() {
            this.freeze = false;
        }
        
	/*
	 * Přidá hráče a současně vyvolá jeho inicializaci. 
	 * Pokud hráč dané barvy již existuje, nedělá nic a vrací false. 
	 * */
	public boolean addPlayer(Player player) {
		
		// white player1 = true
		if (this.player1 == null) {
			if (player.isWhite()) {
				this.player1 = player;
				
				player.init(this.board);
				return true;
			}
		}
		// black player2 = false
		if (this.player2 == null) {
			if (!(player.isWhite())) {
				this.player2 = player;
				
				player.init(this.board);
				return true;
			}
		}
		
		return false;
	}
	
	/*
	 * Vrátí aktuálního hráče, který je na tahu.
	 * */
	public Player currentPlayer() {
		
		if (this.player1_move == true) 
			return player1;
		else 
			return player2;
		
	}
	
	/*
	 * Změní aktuálního hráče.
	 * */
	public Player nextPlayer() {
		
		this.player1_move = !this.player1_move;
		
		if (currentPlayer() == this.player1)
			return this.player2;
		else
			return this.player1;
		
	}
	
	public Board getBoard() {
		
		return this.board;
	}
	
	public Player getPlayer1(){
		return this.player1;
	}
	
	public Player getPlayer2(){
		return this.player2;
	}
}
