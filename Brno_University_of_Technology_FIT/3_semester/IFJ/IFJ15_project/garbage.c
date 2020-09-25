/* ******************************** garbage.c *********************************/
/* Soubor:              garbage.c - Funkce garbage (správa pamäte)			  */
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

#include <stdlib.h>
#include <stdio.h>
#include "garbage.h"

/* ukazatele nejsou inicializovani */
tElemPtr First = NULL;
tElemPtr Last = NULL;

/*
 * Pridani alokovane polozky do seznamu tElement
 * velikost alok. mista v bytech = parametr size
 */

void *gMalloc(int size) {
	tElemPtr ptr;
	ptr = malloc(sizeof(struct tElement));
	if(ptr == NULL) return 0;
	if(ptr) {
		/* inicializace prvku */
		ptr->item = NULL;
		ptr->nextItem = NULL;
		ptr->itemSize = size;

		/* ptr je prvni prvek v seznamu */
		if(!First) {
			First = ptr;
			Last = ptr;	// jediny prvek -> oba na nej ukazuji
		}
		/* ptr je jiny prvek */
		else {
			Last->nextItem = ptr;	// posunu ukazatel na novy prvek - je posledni
			Last = ptr;	// ptr je posledni 
		}

		ptr->item = malloc(size);
		if(ptr->item == NULL) return 0;
		if(!(ptr->item)) {
			return 0;
		}

		return ptr->item;
	}
	return 0;
}

/*
 * Realloc - misto pro novy prvek 
 */

void *gRealloc(void *prevPtr, int size) {
	tElemPtr ptr; 
	ptr = First;

	/* prochazej dokud neprojdes i prvni prvek */
	while(ptr) {
		if(ptr->item == prevPtr) {
			ptr->item = realloc(prevPtr, size);
			if(!(ptr->item)) {
				return 0;
			}

			ptr->itemSize = size;	// nova velikost
			break;	
		}

		ptr = ptr->nextItem;
	}

	/* dalsi polozka neni v seznamu -> alokujeme misto */
	if(!ptr) {
		return gMalloc(size);
	}

	return ptr->item;
}

/*
 * Uvolneni mista, co drive alokovala gMalloc
 */

void gFree() {
	tElemPtr ptr;
	ptr = First;
	/* uvolnuj dokud nesmazes i prvni prvek */
	while(First) {	
		free(First->item);	// uvoneni prvku
		ptr = First->nextItem;	// posun na dalsi prvek
		free(First);
		First = ptr;
	}
}

/* KONEC garbage.c */