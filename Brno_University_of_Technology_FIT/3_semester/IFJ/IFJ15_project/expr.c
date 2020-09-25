/* ******************************** expr.c ************************************/
/* Soubor:              expr.c - Funkce expr (vyhodnocovanie výrazov)		  */
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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "expr.h"
#include "paska.h"
#include "interpret.h"
#include "ial.h"
#include "parser.h"

/* zasobnik pro PSA */
tStack ST;

// ======================================================================
// HASH FUNCKE - psaTable ; chooseRule ; getPsaType ; getInstructionType
// ======================================================================

/* Tabulka PSA */
/* R - redukuj, S - shift, Q - rovno, E - empty */
tPSAsigns psaTable [TSIZE][TSIZE] = {

//								  !=, ==
//							*	+  <,<=
//							/	-  >,>=	(	)  DATY	$
/* 	* /				*/ {	R,	R,	R,	S,	R,	S,	R  },
/* 	+ -				*/ {	S,	R,	R,	S,	R,	S,	R  },
/* != == < <= > >=	*/ {	S,	S,	R,	S,	R,	S,	R  },
/* 	(				*/ {	S,	S,	S,	S,	Q,	S,	E  },
/* 	)				*/ {	R,	R,	R,	E,	R,	E,	R  },
/* 	DATY			*/ {	R,	R,	R,	E,	R,	E,	R  },
/* 	$				*/ {	S,	S,	S,	S,	E,	S,	E  }
};

/* vybira pravidlo z tabulky */
tPSAsigns chooseRule(tPSAtypes tt, tPSAtypes tts) {

	return psaTable[tts][tt];

}

/* Hash na typ tokenu pro PSA tabulku */
int getPSAtype(tPSAtypes *tt_ptr) {

	switch(actToken.typ)
	{

		case NASOBENI:
		case DELENI:
			*tt_ptr = MUL_DIV;
			return ERR_CODE_SUCC;
		break;

		case MINUS:
		case PLUS:
			*tt_ptr = ADD_SUB;
			return ERR_CODE_SUCC;
		break;

		case MENSI:
		case MENSIROVNO:
		case ROVNO:
		case NEROVNO:
		case VETSI:
		case VETSIROVNO:
			*tt_ptr = R_OP;
			return ERR_CODE_SUCC;
		break;

		case LZAVORKA:
			*tt_ptr = LBR;
			return ERR_CODE_SUCC;
		break;

		case PZAVORKA:
			*tt_ptr = RBR;
			return ERR_CODE_SUCC;
		break;

		case CELECISLO:
		case DESETINE:
		case RETEZEC:
		case ID:
			*tt_ptr = DATY;
			return ERR_CODE_SUCC;
		break;

		default:
			return ERR_CODE_SYN;
	}

	// nepovoleny token - zde by se nemel kod nikdy dostat
	return ERR_CODE_SYN;
}

/* hash na operatory v instukcich */
int getInstuctionType(tItemPtr tItem) {

	// prehashuje aktulani token
	switch(tItem->actToken.typ) {

		case NASOBENI:		return MUL_X; 		break;
		case DELENI:		return DIV_X;		break;
		case MINUS:			return SUCC_X;		break;
		case PLUS:			return ADD_X;		break;
		case MENSI:			return MENSI_X;		break;
		case MENSIROVNO:	return MENROVNO_X;	break;	
		case ROVNO:			return ROVNA_X;		break;
		case NEROVNO:		return NEROVNA_X;	break;
		case VETSI:			return VETSI_X;		break;
		case VETSIROVNO:	return VETROVNO_X;	break;

		// nemelo by nikdy zajit do defaultu
		default: return ERR_CODE_SYN;
	}

	return ERR_CODE_SUCC;
}

// HASH FUNCKE - KONEC
// ==================================================================

// ==================================================================
// PREVEDENI NETERMINALU (z Redukce) na OPERANDY -> generate
// ==================================================================

/* dostane Item ze zasobniku a vytahne z nej data - typ + hodnotu, ktere vlozi do instrukce */
int ConvertToInstuction(tItemPtr tIt1, tItemPtr tIt2, tItemPtr tIt3, tInsOperand *tResult)
{

	int result;

	// 2 operandy pasky + typ instrukce
	tInsOperand tOp1;
	tInsOperand tOp2;
	int insType;

	// dej mi typ instrukce (ADD_X e.g.), kterou budes ukladat na pasku
	insType = getInstuctionType(tIt2);

	// CELECISLO op CELECISLO
	if (((tIt1->actToken.typ) == CELECISLO) && ((tIt3->actToken.typ) == CELECISLO)) {

			// ulozeni informaci do instrukci
			
			// normalni "E", ktery jeste nebyl zpracovan
			if (tIt1->resultOnTheTape == false) {

				/* ukladame hodnotu prvku - pokud ID, ulozime adresu (hodnotu nemuzeme, zadne tam zatime nemusi byt*/
				if (tIt1->elemIDptr == NULL) {
					tOp1.cislo 		= tIt1->actToken.cislo;
				}
				else {
					tOp1.elemIDptr 	= tIt1->elemIDptr;	// ulozime pointer do stromu kde to ID je
				}
			}

			// normalni "E", ktery jeste nebyl zpracovan
			if (tIt3->resultOnTheTape == false) {

				/* ukladame hodnotu prvku - pokud ID, ulozime adresu (hodnotu nemuzeme, zadne tam zatime nemusi byt*/
				if (tIt3->elemIDptr == NULL) {
					tOp2.cislo 		= tIt3->actToken.cislo;
				}
				else {
					tOp2.elemIDptr 	= tIt3->elemIDptr;	// ulozime pointer do stromu kde to ID je
				}
			}

			// priradime ostatni hodnoty z Itemu do Operandu
			tOp1.typ = tIt1->actToken.typ;
			tOp2.typ = tIt3->actToken.typ;
			tOp1.resultOnTheTape = tIt1->resultOnTheTape;
			tOp2.resultOnTheTape = tIt3->resultOnTheTape;
			tOp1.elemIDptr = tIt1->elemIDptr;
			tOp2.elemIDptr = tIt3->elemIDptr;

			if (tIt2->actToken.typ == DELENI) {
				tResult->typ = DESETINE;				
			}
			else {
				tResult->typ = CELECISLO;
			}

			// nagenerujeme instrukci - vlozime item na pasku
			if ((result = generate(&T, insType, (void*) &tOp1, (void*) &tOp2, (void*) &(*tResult))) != ERR_CODE_SUCC) return result;

			// vysledek je CELECISLO;		
	}
	
	// CELECISLO op DESETINNE
	else if (((tIt1->actToken.typ) == CELECISLO) && ((tIt3->actToken.typ) == DESETINE)) {

			// ulozeni informaci do instrukci
			
			// normalni "E", ktery jeste nebyl zpracovan
			if (tIt1->resultOnTheTape == false) {

				/* ukladame hodnotu prvku - pokud ID, ulozime adresu (hodnotu nemuzeme, zadne tam zatime nemusi byt*/
				if (tIt1->elemIDptr == NULL) {
					tOp1.cislo 		= tIt1->actToken.cislo;
				}
				else {
					tOp1.elemIDptr 	= tIt1->elemIDptr;	// ulozime pointer do stromu kde to ID je
				}
			}

			// normalni "E", ktery jeste nebyl zpracovan
			if (tIt3->resultOnTheTape == false) {

				/* ukladame hodnotu prvku - pokud ID, ulozime adresu (hodnotu nemuzeme, zadne tam zatime nemusi byt*/
				if (tIt3->elemIDptr == NULL) {
					tOp2.desetin 	= tIt3->actToken.desetin;
				}
				else {
					tOp2.elemIDptr 	= tIt3->elemIDptr;	// ulozime pointer do stromu kde to ID je
				}
			}

			// priradime ostatni hodnoty z Itemu do Operandu
			tOp1.typ = tIt1->actToken.typ;
			tOp2.typ = tIt3->actToken.typ;
			tOp1.resultOnTheTape = tIt1->resultOnTheTape;
			tOp2.resultOnTheTape = tIt3->resultOnTheTape;
			tOp1.elemIDptr = tIt1->elemIDptr;
			tOp2.elemIDptr = tIt3->elemIDptr;

			// v interpretu probehne explicitni konverze na double
			tResult->typ 	= DESETINE;

			// nagenerujeme instrukci - vlozime item na pasku
			if ((result = generate(&T, insType, (void*) &tOp1, (void*) &tOp2, (void*) &(*tResult))) != ERR_CODE_SUCC) return result;

			// vysledek je DESETINNE;		
	}
	// DESETINNE op CELECISLO	
	else if (((tIt1->actToken.typ) == DESETINE) && ((tIt3->actToken.typ) == CELECISLO)) {

			// ulozeni informaci do instrukci
			
			// normalni "E", ktery jeste nebyl zpracovan
			if (tIt1->resultOnTheTape == false) {

				/* ukladame hodnotu prvku - pokud ID, ulozime adresu (hodnotu nemuzeme, zadne tam zatime nemusi byt*/
				if (tIt1->elemIDptr == NULL) {
					tOp1.desetin 	= tIt1->actToken.desetin;
				}
				else {
					tOp1.elemIDptr 	= tIt1->elemIDptr;	// ulozime pointer do stromu kde to ID je
				}
			}

			// normalni "E", ktery jeste nebyl zpracovan
			if (tIt3->resultOnTheTape == false) {

				/* ukladame hodnotu prvku - pokud ID, ulozime adresu (hodnotu nemuzeme, zadne tam zatime nemusi byt*/
				if (tIt3->elemIDptr == NULL) {
					tOp2.cislo 	= tIt3->actToken.cislo;
				}
				else {
					tOp2.elemIDptr 	= tIt3->elemIDptr;	// ulozime pointer do stromu kde to ID je
				}
			}

			// priradime ostatni hodnoty z Itemu do Operandu
			tOp1.typ = tIt1->actToken.typ;
			tOp2.typ = tIt3->actToken.typ;
			tOp1.resultOnTheTape = tIt1->resultOnTheTape;
			tOp2.resultOnTheTape = tIt3->resultOnTheTape;
			tOp1.elemIDptr = tIt1->elemIDptr;
			tOp2.elemIDptr = tIt3->elemIDptr;

			// v interpretu probehne explicitni konverze na double
			tResult->typ 	= DESETINE;

			// nagenerujeme instrukci - vlozime item na pasku
			if ((result = generate(&T, insType, (void*) &tOp1, (void*) &tOp2, (void*) &(*tResult))) != ERR_CODE_SUCC) return result;

			// vysledek je DESETINNE;		
	}
	// DESETINNE op DESETINNE
	else if (((tIt1->actToken.typ) == DESETINE) && ((tIt3->actToken.typ) == DESETINE)) {

			// ulozeni informaci do instrukci
			
			// normalni "E", ktery jeste nebyl zpracovan
			if (tIt1->resultOnTheTape == false) {

				/* ukladame hodnotu prvku - pokud ID, ulozime adresu (hodnotu nemuzeme, zadne tam zatime nemusi byt*/
				if (tIt1->elemIDptr == NULL) {
					tOp1.desetin 	= tIt1->actToken.desetin;
				}
				else {
					tOp1.elemIDptr 	= tIt1->elemIDptr;	// ulozime pointer do stromu kde to ID je
				}
			}

			// normalni "E", ktery jeste nebyl zpracovan
			if (tIt3->resultOnTheTape == false) {

				/* ukladame hodnotu prvku - pokud ID, ulozime adresu (hodnotu nemuzeme, zadne tam zatime nemusi byt*/
				if (tIt3->elemIDptr == NULL) {
					tOp2.desetin 	= tIt3->actToken.desetin;
				}
				else {
					tOp2.elemIDptr 	= tIt3->elemIDptr;	// ulozime pointer do stromu kde to ID je
				}
			}

			// priradime ostatni hodnoty z Itemu do Operandu
			tOp1.typ = tIt1->actToken.typ;
			tOp2.typ = tIt3->actToken.typ;
			tOp1.resultOnTheTape = tIt1->resultOnTheTape;
			tOp2.resultOnTheTape = tIt3->resultOnTheTape;
			tOp1.elemIDptr = tIt1->elemIDptr;
			tOp2.elemIDptr = tIt3->elemIDptr;

			// v interpretu probehne explicitni konverze na double
			tResult->typ 	= DESETINE;

			// nagenerujeme instrukci - vlozime item na pasku
			if ((result = generate(&T, insType, (void*) &tOp1, (void*) &tOp2, (void*) &(*tResult))) != ERR_CODE_SUCC) return result;

			// vysledek je DESETINNE;		
	}
	// RETEZEC op RETEZEC
	else if (((tIt1->actToken.typ) == RETEZEC) && ((tIt3->actToken.typ) == RETEZEC)) {

		if ((tIt2->actToken.typ == MINUS) || (tIt2->actToken.typ == PLUS) || (tIt2->actToken.typ == NASOBENI) || (tIt2->actToken.typ == DELENI)) {

			// pro tyto operace jsou stringy neplatne
			return ERR_CODE_COM;
		}
		else {
			
			// ulozeni informaci do instrukci
			
			// normalni "E", ktery jeste nebyl zpracovan
			if (tIt1->resultOnTheTape == false) {

				/* ukladame hodnotu prvku - pokud ID, ulozime adresu (hodnotu nemuzeme, zadne tam zatime nemusi byt*/
				if (tIt1->elemIDptr == NULL) {
					tOp1.data 	= tIt1->actToken.data;
				}
				else {
					tOp1.elemIDptr 	= tIt1->elemIDptr;	// ulozime pointer do stromu kde to ID je
				}
			}

			// normalni "E", ktery jeste nebyl zpracovan
			if (tIt3->resultOnTheTape == false) {

				/* ukladame hodnotu prvku - pokud ID, ulozime adresu (hodnotu nemuzeme, zadne tam zatime nemusi byt*/
				if (tIt3->elemIDptr == NULL) {
					tOp2.data 	= tIt3->actToken.data;
				}
				else {
					tOp2.elemIDptr 	= tIt3->elemIDptr;	// ulozime pointer do stromu kde to ID je
				}
			}

			// priradime ostatni hodnoty z Itemu do Operandu
			tOp1.typ = tIt1->actToken.typ;
			tOp2.typ = tIt3->actToken.typ;
			tOp1.resultOnTheTape = tIt1->resultOnTheTape;
			tOp2.resultOnTheTape = tIt3->resultOnTheTape;
			tOp1.elemIDptr = tIt1->elemIDptr;
			tOp2.elemIDptr = tIt3->elemIDptr;

			// porovnani dvou stringu -> hodnota 1 nebo 0 -> CELECISLO
			tResult->typ 	= CELECISLO;

			// nagenerujeme instrukci - vlozime item na pasku
			if ((result = generate(&T, insType, (void*) &tOp1, (void*) &tOp2, (void*) &(*tResult))) != ERR_CODE_SUCC) return result;

			// vysledek je RETEZEC;
		}
	}
	
	// neco jineho - nemelo by to zde ani dojit
	else {

		return ERR_CODE_COM;	// semanticka chyba pri vyrazu
	}
	return ERR_CODE_SUCC;
}

// PREVEDENI NETERMINALU (z Redukce) na OPERANDY - KONEC
// ==================================================================

// ==================================================================
// ZASOBNIKOVE FUNKCE
// ==================================================================

/* Inicializace zasobniku */
void initStack(tStack *L) {

	L->Top = NULL;	// inicializace vrcholu

}

/* vybere prvni terminal od vrcholu zasobniku */
void getTopTerminal(tStack *L, tPSAtypes *tts_ptr) {

    tItemPtr temp = L->Top;	// zaciname od topu hledat 

    if (!temp) return;	// stack je prazdny

    /* cykli dokud nenajedes na terminal, v prechozim cyklu si ho uloz */
    while (temp->data == PSASIGN || temp->data == NONTERM) { 

    	// nontermy taky nechci
    	if(temp->data == NONTERM) {
    		temp = temp->ptr;
    		continue;
    	}
        temp = temp->ptr;	
    }

    *tts_ptr = temp->data;	// returni mi cos nasel
}

/* rekukuj stack dle pravidel */
int reduceStack(tStack *L) {
		
	bStromPtr TmpTree;	// pomocny stromcek
	TmpTree = BTR_LOC;

	/* Na topu nemam neterminal (tzn E) -> predelam Top na E, data mu zustanou - "prilepim mu je" */
	if (L->Top->data != NONTERM && L->Top->data != RBR) {

		L->Top->data = NONTERM;

		/* pokud mame ID (isID == true), vyhledame v TS ten prvek a ulozime hodnotu uzlu */
		if (L->Top->isID == true) {

			// print test na spravnost tokenu co prijde

			while (TmpTree != NULL) {

          		// pokud nenajdu - NULL -> jdu o vrstvu vys
          		if ((L->Top->elemIDptr = BSearch(TmpTree, L->Top->actToken.data)) == NULL) {
            		TmpTree = TmpTree->data->Higher;
            	continue;
          		}  // najdu node kde je ID
          		// nasel jsem ID
          		else {
            		if (L->Top->elemIDptr->data->type == KINT) L->Top->actToken.typ = CELECISLO;
					else if (L->Top->elemIDptr->data->type == KDOUBLE) L->Top->actToken.typ = DESETINE;
					else if (L->Top->elemIDptr->data->type == KSTRING) L->Top->actToken.typ = RETEZEC;
					else if (L->Top->elemIDptr->data->type == KAUTO) L->Top->actToken.typ = CELECISLO; //sportkam
					popSecondItem(&ST);	// vyhodime zobak "<"
					return ERR_CODE_SUCC;
          		}
        	}
        	return ERR_CODE_DEF;

			}

		popSecondItem(&ST);	// vyhodime zobak "<"

		return ERR_CODE_SUCC;
	}	 

	return ERR_CODE_SUCC;
}

/* vymaze top stacku - jeden prvek */
void popItem(tStack *L) {

	tItemPtr temp = L->Top;

	// JUST RETURN!?
    if (!temp) return;	// stack je prazdny
    	    
    L->Top = L->Top->ptr;	// top je predposledni prvek	

}

/* vyhod pryc predposledni prvek stacku */
void popSecondItem(tStack *L) {

	tItemPtr temp = L->Top->ptr;	// predposledni prvek

	if(!temp) return;	// asi jeste jestli existuje ten 3.

	L->Top->ptr = L->Top->ptr->ptr;	// navazeme na 3. od topu

}

/* pushnuti prvku na zasobnik */
int pushItem(tStack *L, tPSAtypes tt, tPSAsigns rule) {
	
	tItemPtr item;
	item = NULL;

	if ((item = gMalloc(sizeof(struct tItem))) == NULL)
		return ERR_CODE_INTER;

	item->isID = false;			// item neni ID - pokud je, prepisu jej nize v podmince
	item->resultOnTheTape = false; 
	item->elemIDptr = NULL;
	item->data = tt;			// zapis hashovany typ
	item->actToken = actToken;	// uloz token
	item->ptr = L->Top;			// ukazatel upravime
	L->Top = item;				// nyni je novy prvek Top prvek



	/* pokud neni terminal */
	if (tt == PSASIGN) {

		L->Top->actToken.cislo = rule;

	}
	/* ulozime data z tokenu pro semantiku */
	else if (actToken.typ == CELECISLO) {

		L->Top->actToken.cislo = actToken.cislo;
	}
	else if (actToken.typ == DESETINE) {

		L->Top->actToken.desetin = actToken.desetin;
	}
	else if (actToken.typ == RETEZEC) {

		L->Top->actToken.data = actToken.data;
	}
	else if (actToken.typ == ID) {

		L->Top->actToken.data = actToken.data;
		L->Top->isID = true;
	}
	//////////////////////////////////////////////////////

	return ERR_CODE_SUCC;
}

/* pushneme novy token ("<") na predposledni pozici */
int pushSecondItem(tStack *L, tPSAtypes tt, tPSAsigns rule) {

	tItemPtr item;
	item = NULL;

	if ((item = gMalloc(sizeof(struct tItem))) == NULL)
		return ERR_CODE_INTER;

	item->data = tt;	// zapis, ze je to PSASIGN

	item->ptr = L->Top->ptr;
	L->Top->ptr = item;

	return ERR_CODE_SUCC;

}

// ZASOBNIKOVE FUNKCE - KONEC
// ==================================================================

// ==================================================================
// APLIKACE PRAVIDEL - S, R, Q 	+	HLAVNI LOGIKA ZPRACOVANI EXPR
// ==================================================================

/* vybira pravidlo, co se bude delat */
int appRule(tStack *L, tPSAtypes tt, tPSAtypes tts, tPSAtypes *tts_ptr) {

	int result;
	tInsOperand tResult;		// vysledek z E op E -> E
	tResult.elemIDptr = NULL;	// vysledek neni zadne ID ve vyrazech! perhaps...

	tPSAsigns rule = chooseRule(tt, tts); 

	// SHIFT - nashiftujeme operaci "<" a vstupni token na stack
	if (rule == S) {

		// SHIFT "<"
		if ((result = pushItem(&ST, PSASIGN, S)) != ERR_CODE_SUCC) return result;	

		// token samotny pushnu v hlavni funkci
		return ERR_CODE_SUCC;

	}
 
 	// REDUKUJ - smaz mi vsechno na zasobniku - do topu az po "<" vcetne a nashiftuj tam pravidlo, ktere jsi pouzil + token na prave strane je porad stejny - nevolam dalsi 
	else if (rule == R) {

		// dokud mas co redukovat, redukuj
		while(true) {
			
			// "$E" -> konec redukce, koncime (zaroven i STREDNIK, abychom to neodstrelili driv)
			if (L->Top->data == NONTERM && L->Top->ptr->data == DOLAR && (actToken.typ == STREDNIK || actToken.typ == LSLOZENA)) {
					
				// zde muzu skoncit pri a = 5 + 10; -> resultOnTheTape bude 1 a je to leva hodnota -> 0
				// !!! ve funkci typeToOperand musime nastavit resultOnTheTape na 1 !!!

				// leftval 4 -> nesestrelime pointer v typeToOperand
					if (L->Top->elemIDptr != NULL) {
						if ((result = typeToOperand(L->Top->actToken, 0, 4)) != ERR_CODE_SUCC) return result;
						tOper.tOpOne.elemIDptr = L->Top->elemIDptr;	// predame odkaz do stromu
					}
					else {
					// zde muzu skoncit pri a = 10; nebo return 0; -> resultOnTheTape bude 0 a je to leva hodnota -> 0
						if ((result = typeToOperand(L->Top->actToken, 1, 0)) != ERR_CODE_SUCC) return result;
					}
				
				// hodime na pasku
				if ((result = generate(&T, NOP_X, NULL, NULL, (void*) &(tOper.tOpOne))) != ERR_CODE_SUCC) return result;// v parseru vysledek priradime

				break;
			}

			// redukuj
			if ((result = reduceStack(&ST)) != ERR_CODE_SUCC) return result;

			// Top je "E" + budeme shiftovat -> hodime "<" pred "E"
			tts_ptr = &tts;
			getTopTerminal(&ST, tts_ptr);

			// spatne pravidlo - napr. je konec redukce, ale neco bylo spatne, takze v zasobniku neni "$E"
			if (chooseRule(tt, tts) == E) {
				// mame v expr jen e.g. "return 0" -> "$E$" by to zabilo, i kdyz je to spravne
				if (L->Top->data == NONTERM && L->Top->ptr->data == DOLAR && (actToken.typ == STREDNIK || actToken.typ == LSLOZENA)) {

					// leftval 4 -> nesestrelime pointer v typeToOperand
					if (L->Top->elemIDptr != NULL) {
						if ((result = typeToOperand(L->Top->actToken, 0, 4)) != ERR_CODE_SUCC) return result;
						tOper.tOpOne.elemIDptr = L->Top->elemIDptr;	// predame odkaz do stromu
					}
					else {
					// zde muzu skoncit pri a = 10; nebo return 0; -> resultOnTheTape bude 0 a je to leva hodnota -> 0
						if ((result = typeToOperand(L->Top->actToken, 0, 0)) != ERR_CODE_SUCC) return result;
					}
					// hodime na pasku
					if ((result = generate(&T, NOP_X, NULL, NULL, (void*) &(tOper.tOpOne))) != ERR_CODE_SUCC) return result;

				break;
				}
				// mame konec FOR - cekame "$E)	 <->	{" PODMINKA JE ALE CINKLA
				if (L->Top->data == NONTERM && L->Top->ptr->data == DOLAR && tt == RBR) {
					actToken = getNextToken();	// zavolame "{", ktery by jinak nebyl volany
					if (actToken.typ == CHBA) {
						return ERR_CODE_OTH;
					}
					if (actToken.typ == LEX_ERROR) {
						return ERR_CODE_LEX;
					}
					break;
				}

				return ERR_CODE_SYN;
			}

			if ((L->Top->data == NONTERM) && (chooseRule(tt, tts) == S)) {
				if ((result = pushSecondItem(&ST, PSASIGN, S)) != ERR_CODE_SUCC) return result;	// pushnu SHIFT pred "E"
				break;	// nashiftoval jsi, uz neredukuj
			}

			// mame 2 zavorky -> jen ji tam pushneme (v hlavni fci)
			if ((L->Top->data == NONTERM) && (chooseRule(tt, tts) == Q)) {
				break;	// jen ji tam presunu ")"
			}
			
			// pravidlo E op E -> E
			if (L->Top->data == NONTERM && L->Top->ptr->ptr->data == NONTERM) {
				// nageneruju instukce a vysledek hodim do jednoho "E"


				/* prvky 1 a 3 (2 operandy) musime poslat naopak, jelikoz mame naopak zasobnik */
				if ((result = ConvertToInstuction(L->Top->ptr->ptr, L->Top->ptr, L->Top, &tResult)) != ERR_CODE_SUCC) return result;


				// < E + E - necham prvni E a reknu, ze vysledek na na tape na predchozim prvku
				popItem(&ST);
				popItem(&ST);
				popSecondItem(&ST);

				/* chyceni spatneho generovani instrukci, pokud jsme uz na $E a na konci expr*/
				if (L->Top->data == NONTERM && L->Top->ptr->data == DOLAR && (actToken.typ == STREDNIK || actToken.typ == LSLOZENA)) break;

				L->Top->resultOnTheTape = true;
				L->Top->actToken.typ = tResult.typ;
				
			}

			// pravidlo (E) -> E
			if ((L->Top->data == RBR) && (L->Top->ptr->data == NONTERM) && (L->Top->ptr->ptr->data == LBR)) {

				popItem(&ST);	// vyhodim ")"
				popSecondItem(&ST); // vyhodim "("
				popSecondItem(&ST);	// vyhodim "<"
			}

		}
	}

	// pokud jsem dostal error
	else {

		return ERR_CODE_SYN;

	}

	return ERR_CODE_SUCC;

}

/* hlavni funkce pro vyrazy */
int exprMain() {
	
	int result; // pro chytani return hodnot z fci

	// pomocne promenne pro chytani typu tokenu //
	tPSAtypes *tt_ptr;	// token type ptr
	tPSAtypes *tts_ptr;	// token type stack ptr

	tPSAtypes tt;	// token type
	tPSAtypes tts;	// token type stack - typ tokenu ze zasobniku

	tt_ptr = &tt;	// mame kam zapisovat
	tts_ptr = &tts;
	//////////////////////////////////////////////


	/* inicializace stacku */
	initStack(&ST);
	
	/* hodime "$" na Top zasobniku */
	tt = DOLAR;
	if ((result = pushItem(&ST, tt, I)) != ERR_CODE_SUCC) {
		return result;
	}

	bool kill_operator = true;	// pokud je prvni poslany token neplatny

	/* jdeme volat tokeny dokud nenarazim na ";" nebo err */
	do {

		/* zadame o dalsi token v expr, pokud ";" -> konec expr */
		actToken = getNextToken();

		if (actToken.typ == CHBA) {
			return ERR_CODE_OTH;
		}
		if (actToken.typ == LEX_ERROR) {
			return ERR_CODE_LEX;
		}

		/* sestrelime + - / * < > <= >= == != KCOUT etc (vsechno krome nize vypsanych) kdyz nam dojdou jako 1.token*/
		if ((kill_operator == true) && (actToken.typ != LZAVORKA && actToken.typ != ID &&
			actToken.typ != CELECISLO && actToken.typ != DESETINE &&
			actToken.typ != RETEZEC
			))
		{
			return ERR_CODE_SYN;
		}

		kill_operator = false;

		//////////////////////////////////////////////////////
		/////////// IF WE HAVE FUNC //////////////////////////
		//////////////////////////////////////////////////////	

		/* pridame "if" pokud dostaneme ID - ale je to funkce */
		// tzn prohledame ID ve strome -> pokud je to nazev fce -> specialne ulozime;
		// a pokud je to ID promenne -> ulozim ukazatel na ni
		if (actToken.typ == ID && tOper.isFunction == true) {

			tOper.isFunction = false;	// sestrelime trigger
			// if ID nenajdes ve strome ze je to funkce -> nic nedelej
			// else - jdi zpracovat funkci
			bStromPtr elemIDptr;	// pro vyhledani noudu ID

			if ((elemIDptr = BSearch(BTR, actToken.data)) == NULL) {		// nenasel jsem ID funkce
				// --> nic nedelam
			}
			else {

				// ulozime do SYMB nazev funkce co volam
				SYMB.name = actToken.data;

				  // pokud bude volani funkce, dodelame pravidlo "(" <params_in> ")"
				actToken = getNextToken();
				if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
				if (actToken.typ != LZAVORKA) return ERR_CODE_SYN;

				// zadame token a volame <params_in>
				actToken = getNextToken();
				if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;

				result = rParams_in();
				if (result != ERR_CODE_SUCC) return result;

				/* volame funkci -> GOTO_X - skacu na label tOper.func_st_lab - toho ID funkce, co mi ted prisel 
				 * v parser.c -> za tOper.func_end_lab jeste jeden lab -> na skok zpet, kde jsme volali fci
				 * v expr.c -> label za GOTO_X, kde skocime zpatky z volane funkce
				 */
			
					// GOTO_X - skacu na volanou funkci
					tOper.tOpOne.data = SYMB.name; 		// aktualni nazev funkce
      				tOper.tOpOne.resultOnTheTape = false;
      				tOper.tOpOne.elemIDptr = NULL;
      				tOper.tOpOne.typ = RETEZEC;

      					if ((result = generate(&T, GOTO_X, NULL, (void*) &(tOper.tOpOne), NULL)) != ERR_CODE_SUCC) return result;

					// LAB_X - zde skocim zpet z volane funkce
					tOper.tOpOne.data = "groover"; // zatim natvrdo - moznost volat maximalne 1 funkci
    				tOper.tOpOne.resultOnTheTape = false;
    				tOper.tOpOne.elemIDptr = NULL;
    				tOper.tOpOne.typ = RETEZEC;

    					if ((result = generate(&T, LAB_X, (void*) &(tOper.tOpOne), NULL, NULL)) != ERR_CODE_SUCC) return result;

    				// najdi RET_X a hodnotu potom uloz do nasledujici NOP_X
    					if ((result = generate(&T, F_RET_X, NULL, NULL, NULL)) != ERR_CODE_SUCC) return result;

    				// generujeme NOP_X, do ktereho pridame hodnotu pres F_RET_X, ktery nam ji najde v RET_X
    				
    					if ((result = generate(&T, NOP_X, NULL, NULL, (void*) &(tOper.tOpOne))) != ERR_CODE_SUCC) return result;// v parseru vysledek priradime

				// PZAVORKA byla uz volana a checknuta

				// checkneme ";" - ve vars jenom checkneme, ze jsme ho checkli
				actToken = getNextToken();
				if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
				if (actToken.typ != STREDNIK) return ERR_CODE_SYN;

				// do NOP musime dat return value z funkce - abychom ji potom v parseru priradili

				return ERR_CODE_SUCC;
			}

		}

		//////////////////////////////////////////////////////
		/////////// NORMALNI EXPR ////////////////////////////
		//////////////////////////////////////////////////////

		// mame IF nebo FOR -> special treatment
		if (tOper.expr_iftrg) {
			if (actToken.typ != LZAVORKA) {
				return ERR_CODE_SYN;
			}
			tOper.expr_iftrg = false;
		}
		if (tOper.expr_fortrg) {
			tOper.expr_fortrg = false;
		}

		// STREDNIK || LSLOZENA -> konec expr
		if (actToken.typ == STREDNIK || actToken.typ == LSLOZENA) {

			tt = DOLAR;
			getTopTerminal(&ST, tts_ptr);
			// jdeme pouzit pravidla -> zde by se mela vybrat redukce a redukovat se vsechno			
			if ((result = appRule(&ST, tt, tts, tts_ptr)) != ERR_CODE_SUCC) {
				return result;
			}

			// konec expr -> break na podmince ve while
		}

	  	else {
	  		/* zjistim co mam za token na vstupu a prehashuju */
			if (((result = getPSAtype(tt_ptr)) != ERR_CODE_SUCC)) {
				return result;
			}

			/* zde uz musim porovnavat s topem stacku a pouzit pravidla */
	  		getTopTerminal(&ST, tts_ptr);
	  		/* jdeme vybrat pravidlo */
	  		if ((result = appRule(&ST, tt, tts, tts_ptr)) != ERR_CODE_SUCC) {
				return result;
			}
	
	  		/* pushnu prehashovany token na stack */
	  		if ((result = pushItem(&ST, tt, I)) != ERR_CODE_SUCC) {
				return result;
			}

		}

  	} while (ST.Top->data != DOLAR && (actToken.typ != STREDNIK && actToken.typ != LSLOZENA));

  	return ERR_CODE_SUCC;

}

// APLIKACE PRAVIDEL + LOGIKA - KONEC
// ==================================================================

// konec expr.c //