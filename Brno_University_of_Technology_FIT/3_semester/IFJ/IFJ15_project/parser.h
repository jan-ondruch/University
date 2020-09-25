/* ******************************** parser.h **********************************/
/* Soubor:              parser.h - Hlavičkový súbor pre syntaktická analýza	  */
/* Kodovani:            UTF-8                                                 */
/* Datum:               13. 12. 2015                                          */
/*																			  */
/* Predmet:             Formalni jazyky a prekladace (IFJ)                    */
/* Projekt:             Implementace interpretu jazyka IFJ15                  */
/* Varianta zadani:     a/1/I                                                 */
/*																			  */
/* Autori, login:   	Tereza Kabeláčová     		  xkabel06                */
/*                      Adriana Jelenčíková  		  xjelen10                */
/*                      Jan Ondruch 		          xondru14                */
/*						Michal Klhůfek			      xklhuf00                */
/*				        Martin Auxt   			      xauxtm00                */
/* ****************************************************************************/

#include "paska.h"
#include "ial.h"

#ifndef PARSER_H
#define PARSER_H

/* struktura pro pomocne promenne, triggery etc. */
typedef struct {
	bool isReturn;		// jestli funkce volala return
	bool scopeOut;		// ideme pryc z diffu (kvuli skoupum)
	bool isFunction;	// pokud mame funkci ve vyrazu -> true (abychom rozlisili kdy funkci volame a kdy vyraz.)
	char *func_end_lab;	// unikatni label pro konce funkci -> generujeme nove a nove labels 
	char *func_st_lab;	// nazev funkce - label pro zacatek funkce
    bool expr_iftrg;    // triggery na odlisne konce <expr> u if a for 
    bool expr_fortrg;   //
    tInsOperand tOpOne; // operand pro prirazovani z rType -> posleme ho potom v generate() jako prvni Operand.
    tInsOperand tResult;// vysledek pro instrukce - do ceho budou davat vysledky	 
} Toperations;

// globalni struktury
extern token actToken;
extern Toperations tOper;
extern tTape T;
extern bStromPtr BTR;
extern bStromPtr BTR_LOC;
extern TSymbolPtr SYMB;

// syntakticke funkce
int parser();
int rProgram();
int rFunc();
int rParams();
int rParams_n();
int rDiff();
int rType();
int rStatList();
int rStat();
int rFor();
int rIf();
int rCin();
int rCin_n();
int rCout();
int rCout_n();
int rTerm();
int rReturn();
int rParams_in();
int rParams_in_n();
int rVars();
int rAsign_2();
int rAsign();
int exprMain();

// sematicke funkce
int typeToOperand(token actToken, int typeToOperand, int leftVal);

#endif