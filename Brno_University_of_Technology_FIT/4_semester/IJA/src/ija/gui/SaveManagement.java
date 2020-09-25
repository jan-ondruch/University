/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package ija.gui;

import ija.ija2015.homework2.board.Board;
import ija.ija2015.homework2.board.Field;
import ija.ija2015.homework2.board.Rules;
import ija.ija2015.homework2.game.Game;
import ija.ija2015.homework2.game.Player;
import ija.ija2015.homework2.game.ReversiRules;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.util.ArrayList;

/**
 *
 * @author Andrea
 */
public class SaveManagement  implements Serializable{
	private ArrayList<Player> players = new ArrayList<>();
	Boolean activePlayer;
	Board board;
	int sizeBoard;
	ReversiRules rules;
	Game game ;
	//public static Field [][] Fields;
	
	

	public void putGame(Game game){
		this.game = game;
	
	}
	
	 public void saveGame(String name, Game game)
    {
        try
        {           
            FileOutputStream saveFile = new FileOutputStream("C:\\Users\\Andrea\\Documents\\NetBeansProjects\\ija\\src\\ija\\saves/"+name);
			try (ObjectOutputStream save = new ObjectOutputStream(saveFile)) {
				/*save.writeObject(this.players);
				save.writeObject(this.activePlayer);
				save.writeObject(this.board);
				save.writeObject(this.sizeBoard);
				save.writeObject(this.rules);*/
				//save.writeObject(this.Fields);
				save.writeObject(game);
			}
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
    }
	 
	 public Game loadGame(String name){
		  try {
            FileInputStream saveFile = new FileInputStream("C:\\Users\\Andrea\\Documents\\NetBeansProjects\\ija\\src\\ija\\saves/"+name);
			  try (ObjectInputStream save = new ObjectInputStream(saveFile)) {
				  /*this.players = (ArrayList<Player>)save.readObject();
				  this.activePlayer = (Boolean)save.readObject();
				  this.board = (Board)save.readObject();
				  this.sizeBoard = (Integer)save.readObject();
				  this.rules = (ReversiRules)save.readObject();*/
				 game = (Game)save.readObject();
			  } 
        } catch (Exception exc) {
            exc.printStackTrace();
        }
		  
		 return game;
	 }
	 
	
	
}
