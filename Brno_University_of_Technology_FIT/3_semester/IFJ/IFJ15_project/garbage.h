/* ******************************** garbage.h *********************************/
/* Soubor:              garbage.h - Hlavičkový súbor pre garbage			  */
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

/* deklarace funkci */
void *gMalloc(int size);
void *gRealloc(void *prevPtr, int size);
void gFree();

/* struktura pro praci s pameti */
typedef struct tElement {
	void *item;
	void *nextItem;
	int itemSize;
} tElem, *tElemPtr;

/* KONEC garbage.h */