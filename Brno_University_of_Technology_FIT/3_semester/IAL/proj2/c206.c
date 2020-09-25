	
/* c206.c **********************************************************}
{* Téma: Dvousměrně vázaný lineární seznam
**
**                   Návrh a referenční implementace: Bohuslav Křena, říjen 2001
**                            Přepracované do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Bohuslav Křena, říjen 2015
**
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou
** typu tDLList (DL znamená Double-Linked a slouží pro odlišení
** jmen konstant, typů a funkcí od jmen u jednosměrně vázaného lineárního
** seznamu). Definici konstant a typů naleznete v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu
** s výše uvedenou datovou částí abstrakce tvoří abstraktní datový typ
** obousměrně vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu před prvním použitím,
**      DLDisposeList ... zrušení všech prvků seznamu,
**      DLInsertFirst ... vložení prvku na začátek seznamu,
**      DLInsertLast .... vložení prvku na konec seznamu, 
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek, 
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku, 
**      DLDeleteFirst ... zruší první prvek seznamu,
**      DLDeleteLast .... zruší poslední prvek seznamu, 
**      DLPostDelete .... ruší prvek za aktivním prvkem,
**      DLPreDelete ..... ruší prvek před aktivním prvkem, 
**      DLPostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vloží nový prvek před aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na další prvek seznamu,
**      DLPred .......... posune aktivitu na předchozí prvek seznamu, 
**      DLActive ........ zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce
** explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam 
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální proměnná -- příznak ošetření chyby */
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
    
	L->First = NULL;
	L->Last = NULL;
	L->Act = NULL;

}

void DLDisposeList (tDLList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free. 
**/
	
	tDLElemPtr iterator;	// pomocny prvek

	/* rusime vzdy prvni prvek a posouvame se listem nakonec */
	while((iterator = L->First)) {
		L->First = iterator->rptr;
		free(iterator);
	}

	/* pocatecni stav listu po inicializaci */
	L->First = NULL;
	L->Last = NULL;
	L->Act = NULL;

}

void DLInsertFirst (tDLList *L, int val) {
/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	
	
	tDLElemPtr item;	// novy prvek

	item = (malloc(sizeof(struct tDLElem)));
	if(!item) {
		DLError();	// nepovedeny malloc
		return;
	}

	item->data = val;
	item->lptr = NULL;	// vlevo zadny prvek neni
	item->rptr = L->First;	// novy prvek ukazuje na prvni

	if(L->First) {	// seznam neni prazdny
		L->First->lptr = item;
	}	
	else {
		L->Last = item;	// posledni prvek je novy pridany
	}

	L->First = item;

}

void DLInsertLast(tDLList *L, int val) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/ 	
	
	tDLElemPtr item;

	item = (malloc(sizeof(struct tDLElem)));
	if(!item) {
		DLError();
		return;
	}

	item->data = val;
	item->lptr = L->Last;	// prvek ukazuje na posledni prvek
	item->rptr = NULL;	// pravy pointer null - vpravo uz dalsi prvek neni

	if(!L->First) {	// seznam je prazdny
		L->First = item;
	} 
	else {
		L->Last->rptr = item;
	}

	L->Last = item;	// posledni prvek je novy pridany

}

void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	
	L->Act = L->First;

}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
	
	L->Act = L->Last;

}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/

	if(!L->First) {
		DLError();	// seznam je prazdny
		return;
	}

	*val = L->First->data;	// vrat hodnotu

}

void DLCopyLast (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
	
	if(!L->First) {
		DLError();	// seznam je prazdny
		return;
	}

	*val = L->Last->data;

}

void DLDeleteFirst (tDLList *L) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
	
	if(!L || !L->First) {
		return;
	}

	if(L->First == L->Act) {
		L->Act = NULL;	// aktivita se ztraci
	}

	tDLElemPtr iterator;	// pomocny prvek

	iterator = L->First->rptr;

	free(L->First);	// odstran
	
	L->First = iterator;	// preved first na druhy prvek
	L->First->lptr = NULL;	// vlevo zadny prvek uz neni
	
	/*prvni prvek byl zaroven i poslednim, iterator ukazuje nikam */
	if(!iterator->rptr) {
		L->Last = L->First;
	}

}

void DLDeleteLast (tDLList *L) {
/*
** Zruší poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/ 
	
	if(!L || !L->First) {
		return;
	}
	
	if(L->Last == L->Act) {
		L->Act = NULL;	// aktivita se ztraci
	}

	tDLElemPtr iterator;

	iterator = L->Last->lptr;

	free(L->Last);	// odstan

	L->Last = iterator;	// predposledni na posledni prvek

	if(!L->Last) {	// je prazdny
		L->First = NULL;
	}
	else {
		L->Last->rptr = NULL;	// vpravo uz zadny prvek neni
	}

}

void DLPostDelete (tDLList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/

	if(!L->Act || (L->Act == L->Last)) {
		return;
	}	

	tDLElemPtr iterator;

	iterator = L->Act->rptr;

	/* aktivni prvek je uprostred seznamu */
	if(iterator->rptr) {
		L->Act->rptr = iterator->rptr;
		iterator->rptr->lptr = L->Act;
	}
	/* je nakonci */
	else {
		L->Act->rptr = NULL;
		L->Last = L->Act;
	}

	free(iterator);

}

void DLPreDelete (tDLList *L) {
/*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/
	
	if(!L->Act || (L->First == L->Act)) {
		return;
	}

	tDLElemPtr iterator;

	iterator = L->Act->lptr;

	/* je uprostred */ 
	if(iterator->lptr) {
		L->Act->lptr = iterator->lptr;
		iterator->lptr->rptr = L->Act;
	}
	else {
		L->Act->lptr = NULL;
		L->First = L->Act;
	}

	free(iterator);

}

void DLPostInsert (tDLList *L, int val) {
/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	
	if(!L->Act) {
		return;
	}

	tDLElemPtr item;

	item = malloc(sizeof(struct tDLElem));
	if(!item) {
		DLError();
		return;
	}

	item->data = val;

	/* uprostred */
	if(L->Act->rptr) {
		item->rptr = L->Act->rptr;
		item->rptr->lptr = item;
	}
	/* nakonci */
	else {
		L->Last = item;
	}

	L->Act->rptr = item;
	item->lptr = L->Act;

}

void DLPreInsert (tDLList *L, int val) {
/*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
	
	if(!L->Act) {
		return;
	}

	tDLElemPtr item;

	item = malloc(sizeof(struct tDLElem));
	if(!item) {
		DLError();
		return;
	}

	item->data = val;

	/* uprostred */
	if(L->Act->lptr) {
		item->lptr = L->Act->lptr;
		item->lptr->rptr = item;
	}
	/* nazacatku */
	else {
		L->First = item;
	}

	L->Act->lptr = item;
	item->rptr = L->Act;

}

void DLCopy (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
	
	if(!L->Act) {
		DLError();
		return;
	}	

	*val = L->Act->data;
	
}

void DLActualize (tDLList *L, int val) {
/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/
	
	if(!L->Act) {
		return;
	}

	L->Act->data = val;

}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/
	
	if(!L->Act) {
		return;
	}

	L->Act = L->Act->rptr;

}


void DLPred (tDLList *L) {
/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/
	
	if(!L->Act) {
		return;
	}

	L->Act = L->Act->lptr;

}

int DLActive (tDLList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/
	
	return(L->Act)? TRUE : FALSE;

}

/* Konec c206.c*/