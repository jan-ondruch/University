/* ******************************** expr.h ************************************/
/* Soubor:              expr.h - Hlavičkový súbor pre expr					  */
/*								 (vyhodnocovanie výrazov)					  */
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

#include "scaner.h"
#include "main.h"
#include "parser.h"
#include "garbage.h"
#include "ial.h"

#define	TSIZE	7

/* typy hodnot v PSA tabulce */
typedef enum {
	R,	// 0 REDUKUJ 	>
	S,	// 1 SHIFT 		< 
	Q,	// 2 eQual 		=
	E,  // 3 EMPTY 
	I   // 4 IRRELEVANT - for terminals

} tPSAsigns;


/* operatory, typy v PSA */
/* nemenit poradi v enumu - primo indexuje pravidla */
typedef enum {
	MUL_DIV,	// 0 * /
	ADD_SUB,	// 1 + -
	R_OP,		// 2 < > <> >= == !=
	LBR,		// 3 (
	RBR,		// 4 )
	DATY,		// 5 int double string id
	DOLAR,		// 6 $
	PSASIGN,	// 7 psasign (for rules)
	NONTERM		// 8 nonterm - application of <expr> pravidla

} tPSAtypes;

/* polozka zasobniku */
typedef struct tItem {

	bool resultOnTheTape;
	bool isID;					// pokud je token ID - nastavime ho pozdeni na true, abychom ho dohledali v TS
	bStromPtr elemIDptr;		// ukazatel na ID do TS
	tPSAtypes data;				// typ tokenu
	token actToken;				// aktualni token - co je v nem za data
	struct tItem *ptr;			// ukazatel na nasledujici prvek zasobniku

} *tItemPtr;

/* zasobnik */
typedef struct {

	tItemPtr Top;	// vrchol zasobniku (ukazatel)
} tStack;

int getPSAtype(tPSAtypes *tt_ptr);
void initStack(tStack *L);
void disposeStack(tStack *L);
void getTopTerminal(tStack *L, tPSAtypes *tts_ptr);
int reduceStack(tStack *L);
void popItem(tStack *L);
void popSecondItem(tStack *L);
tPSAsigns chooseRule(tPSAtypes tt, tPSAtypes tts);
int appRule(tStack *L, tPSAtypes tt, tPSAtypes tts, tPSAtypes *tts_ptr);
int pushItem(tStack *L, tPSAtypes tt, tPSAsigns rule);
int pushSecondItem(tStack *L, tPSAtypes tt, tPSAsigns rule);




