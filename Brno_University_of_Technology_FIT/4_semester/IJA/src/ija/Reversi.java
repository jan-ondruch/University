/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package ija;

import ija.gui.*;
import ija.ija2015.homework2.board.*;
import ija.ija2015.homework2.game.*;

/**
 *
 * @author Andrea
 */
public class Reversi implements Runnable {
	
	Game game = null;
	
	public static void main(String[] args) {	
        Thread t=null;
        
        Reversi reversi = new Reversi();
        t = new Thread(reversi);
        t.start();
		
	} 

	public Reversi(Game game) {
                game.unfreezeDisks();
		this.game = game;
	}

	public Reversi() {
		
	}

	
	@Override
	public void run() {
		Frame frame = new Frame(game);
		frame.setVisible(true);
		frame.dispose();
		return;
		
	}
}
