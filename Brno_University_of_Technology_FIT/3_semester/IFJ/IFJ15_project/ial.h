/* ******************************** ial.h *************************************/
/* Soubor:              ial.h - Hlavičkový súbor pre algoritmy				  */
/* Kodovani:            UTF-8                                                 */
/* Datum:               13. 12. 2015                                          */
/*																			  */
/* Predmet:             Formalni jazyky a prekladace (IFJ)                    */
/*						Algoritmy (IAL)										  */
/* Projekt:             Implementace interpretu jazyka IFJ15                  */
/* Varianta zadani:     a/1/I                                                 */
/*																			  */
/* Autori, login:   	Tereza Kabeláčová     		  xkabel06                */
/*                      Adriana Jelenčíková  		  xjelen10                */
/*				        Martin Auxt   			      xauxtm00                */
/*                      Jan Ondruch 		          xondru14                */
/*						Michal Klhůfek			      xklhuf00                */
/* ****************************************************************************/

#ifndef IAL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "scaner.h"
#include "main.h"

#define SIZE 8 // zacatecni velikost pole, pri realokaci se alokuje +SIZE

// hodnota symbolu v uzlu
typedef struct Value{
	int ival;
	double dval;
	char* sval;    
}value;


// struktura symbolu - uzel stromu
typedef struct symbol{
	ID_tokenu type;
	char* name;  //key
	value hodnota;
	int h;  			//docasna

	struct bStrom *Higher;
	struct bStrom *Local;

	int vrstva; 
	bool isFunction;
	bool isDeclared;
	bool isInit;
	bool isDefined;    //je funkcia definovana? ||  mozu byt prototypy??
	char **arg;
	int pocet_params;
	int alloc_pam;
	bool isHead;

}*TSymbol, TSymbolPtr;


//struktura pre strom
typedef struct bStrom{
	char* key;
	TSymbol data;

	struct bStrom *lptr;
	struct bStrom *rptr;
}*bStromPtr;


/* deklarace funkci */
void BSInit(bStromPtr *root);
void TSInit(TSymbol *symbol);
int BSInsert(bStromPtr *tree, char* key, TSymbol symbol);
bStromPtr BSearch(bStromPtr tree, char* val);
int kontrola(bStromPtr uzol, TSymbol symbol);

// vestavene funkce
/*
char* concat(char *str1, char *str2);
char* sort(char *str);
void quickSort( int a[], int l, int r);
int quickS( int a[], int l, int r);
int lenght(char *str);
int find(tString *str, tString *search);
char* substr(char* str, int i, int n);


int BSRemove (bStromPtr *strom, char *key);
int BSDispose(bStromPtr *uzol);

bStromPtr search(bStromPtr tree, char* val);
*/

#endif
#define IAL_H