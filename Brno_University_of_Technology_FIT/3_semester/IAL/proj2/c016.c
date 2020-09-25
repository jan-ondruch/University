
/* c016.c: **********************************************************}
{* Téma:  Tabulka s Rozptýlenými Položkami
**                      První implementace: Petr Přikryl, prosinec 1994
**                      Do jazyka C prepsal a upravil: Vaclav Topinka, 2005
**                      Úpravy: Karel Masařík, říjen 2014
**                      Úpravy: Radek Hranický, říjen 2014
**                      Úpravy: Radek Hranický, listopad 2015
**
** Vytvořete abstraktní datový typ
** TRP (Tabulka s Rozptýlenými Položkami = Hash table)
** s explicitně řetězenými synonymy. Tabulka je implementována polem
** lineárních seznamů synonym.
**
** Implementujte následující procedury a funkce.
**
**  HTInit ....... inicializuje tabulku před prvním použitím
**  HTInsert ..... vložení prvku
**  HTSearch ..... zjištění přítomnosti prvku v tabulce
**  HTDelete ..... zrušení prvku
**  HTRead ....... přečtení hodnoty prvku
**  HTClearAll ... zrušení obsahu celé tabulky (inicializace tabulky
**                 poté, co již byla použita)
**
** Definici typů naleznete v souboru c016.h.
**
** Tabulka je reprezentována datovou strukturou typu tHTable,
** která se skládá z ukazatelů na položky, jež obsahují složky
** klíče 'key', obsahu 'data' (pro jednoduchost typu float), a
** ukazatele na další synonymum 'ptrnext'. Při implementaci funkcí
** uvažujte maximální rozměr pole HTSIZE.
**
** U všech procedur využívejte rozptylovou funkci hashCode.  Povšimněte si
** způsobu předávání parametrů a zamyslete se nad tím, zda je možné parametry
** předávat jiným způsobem (hodnotou/odkazem) a v případě, že jsou obě
** možnosti funkčně přípustné, jaké jsou výhody či nevýhody toho či onoho
** způsobu.
**
** V příkladech jsou použity položky, kde klíčem je řetězec, ke kterému
** je přidán obsah - reálné číslo.
*/

#include "c016.h"

int HTSIZE = MAX_HTSIZE;
int solved;

/*          -------
** Rozptylovací funkce - jejím úkolem je zpracovat zadaný klíč a přidělit
** mu index v rozmezí 0..HTSize-1.  V ideálním případě by mělo dojít
** k rovnoměrnému rozptýlení těchto klíčů po celé tabulce.  V rámci
** pokusů se můžete zamyslet nad kvalitou této funkce.  (Funkce nebyla
** volena s ohledem na maximální kvalitu výsledku). }
*/

int hashCode ( tKey key ) {
	int retval = 1;
	int keylen = strlen(key);
	for ( int i=0; i<keylen; i++ )
		retval += key[i];
	return ( retval % HTSIZE );
}

/*
** Inicializace tabulky s explicitně zřetězenými synonymy.  Tato procedura
** se volá pouze před prvním použitím tabulky.
*/

void htInit ( tHTable* ptrht ) {

	// if not NULL -> initialize all the values on NULL
	if ((*ptrht)) {

		// i < HTSIZE or (HTSIZE+1)? will see...
		for (int i = 0; i < (HTSIZE); i++) {

			(*ptrht)[i] = NULL;

		}
	}

}

/* TRP s explicitně zřetězenými synonymy.
** Vyhledání prvku v TRP ptrht podle zadaného klíče key.  Pokud je
** daný prvek nalezen, vrací se ukazatel na daný prvek. Pokud prvek nalezen není, 
** vrací se hodnota NULL.
**
*/

tHTItem* htSearch ( tHTable* ptrht, tKey key ) {

	// if the table is empty -> return null;
	if (!(*ptrht)) {
		return NULL;
	}

	tHTItem *item;			// item we want to find and eventually return
	int hCode;				// value from the hashCode function

	hCode = hashCode(key);	// get the value

	// if the element does not exist -> return null
	if (!((*ptrht)[hCode])) {
		return NULL;
	}
	// table contains elements - going to look for the element
	else {	

		/* get the hash and look through the table till not NULL */
		item = (*ptrht)[hCode];
		while (item) {

			// hit
			if (item->key == key) {
				return item;
			}
			// we go farther
			else if (item->key != key){
				item = item->ptrnext;
			}
		}

	}

	// didn't find anything -> return NULL
	return NULL;

}
/* 
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vkládá do tabulky ptrht položku s klíčem key a s daty
** data.  Protože jde o vyhledávací tabulku, nemůže být prvek se stejným
** klíčem uložen v tabulce více než jedenkrát.  Pokud se vkládá prvek,
** jehož klíč se již v tabulce nachází, aktualizujte jeho datovou část.
**
** Využijte dříve vytvořenou funkci htSearch.  Při vkládání nového
** prvku do seznamu synonym použijte co nejefektivnější způsob,
** tedy proveďte.vložení prvku na začátek seznamu.
**/

void htInsert ( tHTable* ptrht, tKey key, tData data ) {


	// if null -> return;
	if (!(*ptrht)) {
		return;
	}
	else {

		// if the element already existst = is not NULL -> overwrite data
		if ((htSearch(ptrht, key))) {
			tHTItem *overItem = htSearch(ptrht, key);
			overItem->data = data;
			return;
		}
		else {

			tHTItem *newItem;

			/* get hash first */
			int hCode;
			hCode = hashCode(key);

			/* we create a new item and put it in the beginning of the list */
			newItem = malloc(sizeof (struct tHTItem));
			if (newItem == NULL) {
				return;
			}

			// we save all the data
			newItem->key = key;
			newItem->data = data;
			// connect it with the rest of the table/list
			newItem->ptrnext = (*ptrht)[hCode];
			(*ptrht)[hCode] = newItem;

		}
		return;
	}
	// should be covered all??

}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato funkce zjišťuje hodnotu datové části položky zadané klíčem.
** Pokud je položka nalezena, vrací funkce ukazatel na položku
** Pokud položka nalezena nebyla, vrací se funkční hodnota NULL
**
** Využijte dříve vytvořenou funkci HTSearch.
*/

tData* htRead ( tHTable* ptrht, tKey key ) {

	// if null -> return;
	if (!(*ptrht)) {
		return NULL;
	}
	// we look for the element
	else {

		tHTItem *temp;
		temp = htSearch(ptrht, key);

		/* if we find it -> return the data, else return NULL*/
		if (temp) {
			return (&(temp->data));	// check out the result later, if it's ok
		}
		else {
			return NULL;
		}
	}

}

/*
** TRP s explicitně zřetězenými synonymy.
** Tato procedura vyjme položku s klíčem key z tabulky
** ptrht.  Uvolněnou položku korektně zrušte.  Pokud položka s uvedeným
** klíčem neexistuje, dělejte, jako kdyby se nic nestalo (tj. nedělejte
** nic).
**
** V tomto případě NEVYUŽÍVEJTE dříve vytvořenou funkci HTSearch.
*/

void htDelete ( tHTable* ptrht, tKey key ) {

	// if null -> return;
	if (!(*ptrht)) {
		return;
	}

	// we go to search for the item
	else {

		tHTItem *currItem;			// item we want to find and eventually return
		tHTItem *nextItem;
		int hCode;				// value from the hashCode function

		hCode = hashCode(key);	// get the value

		/* if the element exists */
		if ((*ptrht)[hCode]) {

			nextItem = currItem = (*ptrht)[hCode];

			/* as long as the element exists -> iterate over the synonyms and if found -> delete appropriately*/

			while(currItem) {

				// we found the element we want to delete 
				if (currItem->key == key) {

					if (currItem != (*ptrht)[hCode]) {
						nextItem = nextItem->ptrnext;
						nextItem = currItem->ptrnext;	// go farther
					}
					else {
						(*ptrht)[hCode] = currItem->ptrnext;	//go farther
						// in synonyms //
					}

					// we free the found element & leave
					free(currItem);
					return;
				}
				// we didn't find it
				nextItem = currItem;

				// and set currItem to the next one
				currItem = currItem->ptrnext;
			}

			// end it if we find nothing to remove - the element
			nextItem = NULL;

			return;
		}

		// else situation has already been covered ...
	}
	
}

/* TRP s explicitně zřetězenými synonymy.
** Tato procedura zruší všechny položky tabulky, korektně uvolní prostor,
** který tyto položky zabíraly, a uvede tabulku do počátečního stavu.
*/

void htClearAll ( tHTable* ptrht ) {

	// if null -> return;
	if (!(*ptrht)) {
		return;
	}

	// for realeasing the mem //
	tHTItem *temp;

	/* cycle through the whole table and remove the elements */
	for (int i = 0; i < (HTSIZE); i++) {

		// if there is no element on the "i" th position -> go for the next one
		if (!((*ptrht)[i])) {
			continue;
		}

		// go through and remove synonyms
		while ((*ptrht)[i]) {

			temp = (*ptrht)[i];					// save the current element
			(*ptrht)[i] = (*ptrht)[i]->ptrnext;	// move to the next one

			free(temp);	// release the mem
		}
	}

	// does not point anywhere anymore
	temp = NULL;

}
