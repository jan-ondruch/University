/* ******************************** paska.h ********************************* */
/* Soubor:              paska.h - Hlavičkový súbor pre interpretčnú pásku     */
/* Kodovani:            UTF-8                                                 */
/* Datum:               9. 12. 2015                                           */
/*																			  */
/* Predmet:             Formalni jazyky a prekladace (IFJ)                    */
/* Projekt:             Implementace interpretu jazyka IFJ15                  */
/* Varianta zadani:     a/1/I                                                 */
/*																			  */
/* Autori, login:   	Tereza Kabeláčová     		  xkabel06                */
/*                      Adriana Jelenčíková  		  xjelen10                */
/*                      Jan Ondruch 		          xondru14                */
/*						Michal Klhůfek		 	      xklhuf00                */
/*				        Martin Auxt   			      xauxtm00                */
/* ****************************************************************************/

#include "scaner.h"
#include "ial.h"

#ifndef PASKA_H
#define PASKA_H

// Zakladni konstukce
// ------------------

#define STOP_X          0		//  STOP    pro cyklus na pasce dokud neprijde tahle hodnota
#define READ_X      	1		//  CIN     nacteni int ze vstupu
#define WRITE_X         4		//  COUT    pro vysledny vypis na terminal
#define ADD_X           5	  	//  +       pro zpracovani scitani
#define SUCC_X          6	  	//  -       pro zpracovani odcitani
#define MUL_X           7	  	//  *       pro zpracovani nasobeni
#define DIV_X           8	  	//  /       pro zpracovani deleni
#define MENSI_X         9 		//  <       provede porovnani zde je mensi
#define VETSI_X         10		//  >       provede porovnani zde je vetsi
#define MENROVNO_X      11		//  <=      provede porovnani zde je mensi nebo rovno
#define VETROVNO_X      12		//  >=      provede porovnani zde je vetsi nebo rovno
#define NEROVNA_X       13		//  !=      provede porovnani zde se nerovna
#define ROVNA_X         14		//  ==      provede porovnani zde se rovna
#define PRIRAD_X	    15		//  =       priradi odpovidajici hodnotu

// Slozitejsi konstukce
// --------------------

#define GOTOBCK_X		16	//			bude hledat label nekde zpatky (treba for)
#define RET_X	        17	//          navratova hodnota funkce
#define F_RET_X	        18	//          navratova hodnota funkce - hledani 
#define NOP_X           19	//  ;       prazdna instrukce, nic se neprovede
#define NOT_X	  	    20  //  negace	prevraceni bool vyrazu u podminky
#define GOTO_X          21  //  skok    skoc mi na prislusnou label
#define LAB_X			22	//  		vygeneruj label kam budu skakat


// data operandu v pasce
typedef struct
  {
    ID_tokenu typ;
    bStromPtr elemIDptr;  // ukazatel na ID do TS
    bool resultOnTheTape; // jestli je vysledek na predchozim itemu v pasce
    int cislo;            // pokud je potreba ulozit cele cislo
    double desetin;       // pokud je potreba ulozit desetine cislo
    char* data;           // pokud je potreba ulozit retezec
  } tInsOperand;

// prvek pasky
typedef struct tapeItem {
	int insType;					// typ instukce
	tInsOperand tOp1;				// prvni operand
	tInsOperand tOp2;				// druhy operand
	tInsOperand tResult;			// vysledek
	struct tapeItem *jump;			// pointer na skok
	struct tapeItem *lptr;			// predchozi prvek pasky
	struct tapeItem *rptr;			// nasledujici prvek pasky
} *tapeItemPtr;

// cela paska
typedef struct {
	tapeItemPtr frst;
	tapeItemPtr lst;
	tapeItemPtr act;
} tTape;

// deklarace
int generate(tTape *T, int insType, void *tOp1, void *tOp2, void *tResult);
void tTapeInit(tTape *T);


#endif
