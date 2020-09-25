/* ******************************** paska.c ********************************* */
/* Soubor:              paska.c - Interpretační paska						  */
/* 								  (pomocne funkce k Interpretu)		          */
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
/*						Michal Klhůfek			      xklhuf00                */
/*				        Martin Auxt   			      xauxtm00                */
/* ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "paska.h"
#include "main.h"
#include "garbage.h"

/* inicializace pasky */
void tTapeInit(tTape *T) {
    
	T->frst = NULL;
	T->lst = NULL;
	T->act = NULL;

}

/* presun aktivity na dalsi prvek */
void tTapeNext(tTape *T) {

	if (T->act) {
		T->act = T->act->rptr;
	}

}

/* vlozi novy prvek na konec intrukcni pasky */
int generate(tTape *T, int insType, void *tOp1, void *tOp2, void *tResult) {

	/* novy prvek pasky a jeho alokace */
	tapeItemPtr item;

	item = gMalloc(sizeof(struct tapeItem));
	if (!item) {
		return ERR_CODE_INTER;
	}


	// ulozime TYP INSTRUKCE
	item->insType = insType;
	////////////////////////

	/* ulozime jestli bude operand prazdny a predame mu vysledek az v interpretu, pokud ano
	 * nic do nej vkladat nebudu 
	 */
	if (tOp1 != NULL) item->tOp1.resultOnTheTape = (*(tInsOperand*)tOp1).resultOnTheTape;
	if (tOp2 != NULL) item->tOp2.resultOnTheTape = (*(tInsOperand*)tOp2).resultOnTheTape;

	if (tResult != NULL) {
		item->tResult = *(tInsOperand*)tResult;
	}
	

	/////////////////////////////////////////////////////////////////////////////////////////
	//////// UKLADANI HODNOT Z OPERANDU NA PASKU ////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////

	/* pokud jsme operand vubec poslali nejaky a musime ho ulozit - tzn nebudeme brat result z predchoziho itemu na pasce */
	if ((tOp1 != NULL) && (item->tOp1.resultOnTheTape == false)) {
		// prvni operand - ulozime data
		// 
		if ((*(tInsOperand*)tOp1).typ == CELECISLO) {	// CELECISLO

			if ((*(tInsOperand*)tOp1).elemIDptr == NULL) {
				item->tOp1.cislo = (*(tInsOperand*)tOp1).cislo;		// ulozime CISLO
			}
			else {
				item->tOp1.elemIDptr = (*(tInsOperand*)tOp1).elemIDptr;	// ulozime pointer
			}
			item->tOp1.typ = (*(tInsOperand*)tOp1).typ;				// ulozime TYP
		}
		else if ((*(tInsOperand*)tOp1).typ == DESETINE) {			// DESETINE

			if ((*(tInsOperand*)tOp1).elemIDptr == NULL) {
				item->tOp1.desetin = (*(tInsOperand*)tOp1).desetin;
			}
			else {
				item->tOp1.elemIDptr = (*(tInsOperand*)tOp1).elemIDptr;
			}
			item->tOp1.typ = (*(tInsOperand*)tOp1).typ;	// ulozime TYP
		}
		else {		

			if ((*(tInsOperand*)tOp1).elemIDptr == NULL) {
				item->tOp1.data = (*(tInsOperand*)tOp1).data;		// RETEZEC
			}
			else {
				item->tOp1.elemIDptr = (*(tInsOperand*)tOp1).elemIDptr;
			}
			item->tOp1.typ = (*(tInsOperand*)tOp1).typ;				// ulozime TYP
		}
	}

	/* pokud jsme operand vubec poslali nejaky a musime ho ulozit - tzn nebudeme brat result z predchoziho itemu na pasce */
	if ((tOp2 != NULL) && (item->tOp2.resultOnTheTape == false)) {
		// druhy operand - ulozime data
		if ((*(tInsOperand*)tOp2).typ == CELECISLO) {				// CELECISLO
			
			if ((*(tInsOperand*)tOp2).elemIDptr == NULL) {
				item->tOp2.cislo = (*(tInsOperand*)tOp2).cislo;		// ulozime CISLO
			}
			else {
				item->tOp2.elemIDptr = (*(tInsOperand*)tOp2).elemIDptr; // ulozime pointer
			}
			item->tOp2.typ = (*(tInsOperand*)tOp2).typ;				// ulozime TYP
		}
		else if ((*(tInsOperand*)tOp2).typ == DESETINE) {			// DESETINE
			
			if ((*(tInsOperand*)tOp2).elemIDptr == NULL) {
				item->tOp2.desetin = (*(tInsOperand*)tOp2).desetin;
			}
			else {
				item->tOp2.elemIDptr = (*(tInsOperand*)tOp2).elemIDptr;
			}
			item->tOp2.typ = (*(tInsOperand*)tOp2).typ;				// ulozime TYP
		}
		else {														// RETEZEC
			
			if ((*(tInsOperand*)tOp2).elemIDptr == NULL) {
				item->tOp2.data = (*(tInsOperand*)tOp2).data;		
			}
			else {
				item->tOp2.elemIDptr = (*(tInsOperand*)tOp2).elemIDptr; // ulozime pointer
			}
			item->tOp2.typ = (*(tInsOperand*)tOp2).typ;				// ulozime TYP
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	//////// KONEC UKLADANI HODNOT Z OPERANDU ///////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////

	// VKLADANI PRVKU NA PASKU A PROVAZANI SEZNAMU //

	/* vlozime prvek do pasky */
	// prvek je prvni v pasce -> nastavime ho na first & na last
	if (T->frst == NULL) {
		T->frst = item;
		T->frst->lptr = NULL;	// vlevo uz nic neni

		T->act = T->frst;		// aktivitu nastavime na prvni
	}
	// jinak ho dame nakonec
	else {
		T->lst->rptr = item;
		item->lptr = T->lst;
	}

	T->lst = item;			// pridany prvek nastavime na last
	T->lst->rptr = NULL;	// posledni prvek vpravo uz nic nema

	return ERR_CODE_SUCC;
}


