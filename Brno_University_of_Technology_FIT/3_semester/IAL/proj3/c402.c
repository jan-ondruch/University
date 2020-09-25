
/* c402.c: ********************************************************************}
{* Téma: Nerekurzivní implementace operací nad BVS 
**                                     Implementace: Petr Přikryl, prosinec 1994
**                                           Úpravy: Petr Přikryl, listopad 1997
**                                                     Petr Přikryl, květen 1998
**			  	                        Převod do jazyka C: Martin Tuček, srpen 2005
**                                         Úpravy: Bohuslav Křena, listopad 2009
**                                         Úpravy: Karel Masařík, říjen 2013
**                                         Úpravy: Radek Hranický, říjen 2014
**                                         Úpravy: Radek Hranický, listopad 2015
**
** S využitím dynamického přidělování paměti, implementujte NEREKURZIVNĚ
** následující operace nad binárním vyhledávacím stromem (předpona BT znamená
** Binary Tree a je u identifikátorů uvedena kvůli možné kolizi s ostatními
** příklady):
**
**     BTInit .......... inicializace stromu
**     BTInsert ........ nerekurzivní vložení nového uzlu do stromu
**     BTPreorder ...... nerekurzivní průchod typu pre-order
**     BTInorder ....... nerekurzivní průchod typu in-order
**     BTPostorder ..... nerekurzivní průchod typu post-order
**     BTHeight ........ výpočet výšky stromu
**     BTDisposeTree ... zruš všechny uzly stromu
**
** U všech funkcí, které využívají některý z průchodů stromem, implementujte
** pomocnou funkci pro nalezení nejlevějšího uzlu v podstromu. Tyto pomocné
** funkce jsou:
**
**     Leftmost_Preorder
**     Leftmost_Inorder
**     Leftmost_Postorder
**
** Ve vaší implementaci si můžete definovat pomocné zásobníky pro uložení
** ukazetelů na uzly stromu (tStackP)
** nebo pro uložení booleovských hodnot TRUE/FALSE (tStackB).
** Pro práci s pomocnými zásobníky můžete použít následující funkce:
**
**     SInitP / SInitB ....... inicializace zásobníku
**     SPushP / SPushB ....... vložení prvku na vrchol zásobníku
**     SPopP / SPopB ......... odstranění prvku z vrcholu zásobníku
**     STopP / STopB ......... získání hodonty prvku na vrcholu zásobníku
**     STopPopP / STopPopB ... kombinace předchozích dvou funkcí 
**     SSizeP / SSizeB ....... zjištění počtu prvků v zásobníku
**     SEmptyP / SEmptyB ..... zjištění, zda je zásobník prázdný 
** 
** Pomocné funkce pro práci ze zásobníky je zakázáno upravovat!
** 
** Přesné definice typů naleznete v souboru c402.h. Uzel stromu je typu tBTNode,
** ukazatel na něj je typu tBTNodePtr. Jeden uzel obsahuje položku int Cont,
** která současně slouží jako užitečný obsah i jako vyhledávací klíč 
** a ukazatele na levý a pravý podstrom (LPtr a RPtr).
**
** Příklad slouží zejména k procvičení nerekurzivních zápisů algoritmů
** nad stromy. Než začnete tento příklad řešit, prostudujte si důkladně
** principy převodu rekurzivních algoritmů na nerekurzivní. Programování
** je především inženýrská disciplína, kde opětné objevování Ameriky nemá
** místo. Pokud se Vám zdá, že by něco šlo zapsat optimálněji, promyslete
** si všechny detaily Vašeho řešení. Povšimněte si typického umístění akcí
** pro různé typy průchodů. Zamyslete se nad modifikací řešených algoritmů
** například pro výpočet počtu uzlů stromu, počtu listů stromunebo pro
** vytvoření zrcadlového obrazu stromu (pouze popřehazování ukazatelů
** bez vytváření nových uzlů a rušení starých).
**
** Při průchodech stromem použijte ke zpracování uzlu funkci BTWorkOut().
** Pro zjednodušení práce máte předem připraveny zásobníky pro hodnoty typu
** bool a tBTNodePtr. Pomocnou funkci BTWorkOut ani funkce pro práci
** s pomocnými zásobníky neupravujte 
** Pozor! Je třeba správně rozlišovat, kdy použít dereferenční operátor *
** (typicky při modifikaci) a kdy budeme pracovat pouze se samotným ukazatelem 
** (např. při vyhledávání). V tomto příkladu vám napoví prototypy funkcí.
** Pokud pracujeme s ukazatelem na ukazatel, použijeme dereferenci.
**/

#include "c402.h"
int solved;

void BTWorkOut (tBTNodePtr Ptr)		{
/*   ---------
** Pomocná funkce, kterou budete volat při průchodech stromem pro zpracování
** uzlu určeného ukazatelem Ptr. Tuto funkci neupravujte.
**/
			
	if (Ptr==NULL) 
    printf("Chyba: Funkce BTWorkOut byla volána s NULL argumentem!\n");
  else 
    printf("Výpis hodnoty daného uzlu> %d\n",Ptr->Cont);
}
	
/* -------------------------------------------------------------------------- */
/*
** Funkce pro zásobník hotnot typu tBTNodePtr. Tyto funkce neupravujte.
**/

void SInitP (tStackP *S)  
/*   ------
** Inicializace zásobníku.
**/
{
	S->top = 0;  
}	

void SPushP (tStackP *S, tBTNodePtr ptr)
/*   ------
** Vloží hodnotu na vrchol zásobníku.
**/
{ 
                 /* Při implementaci v poli může dojít k přetečení zásobníku. */
  if (S->top==MAXSTACK) 
    printf("Chyba: Došlo k přetečení zásobníku s ukazateli!\n");
  else {  
		S->top++;  
		S->a[S->top]=ptr;
	}
}	

tBTNodePtr STopPopP (tStackP *S)
/*         --------
** Odstraní prvek z vrcholu zásobníku a současně vrátí jeho hodnotu.
**/
{
                            /* Operace nad prázdným zásobníkem způsobí chybu. */
	if (S->top==0)  {
		printf("Chyba: Došlo k podtečení zásobníku s ukazateli!\n");
		return(NULL);	
	}	
	else {
		return (S->a[S->top--]);
	}	
}

tBTNodePtr STopP (tStackP *S)
/*         --------
** Vrátí hodnotu prvku na vrcholu zásobníku
**/
{
                            /* Operace nad prázdným zásobníkem způsobí chybu. */
	if (S->top==0)  {
		printf("Chyba: Došlo k podtečení zásobníku s ukazateli!\n");
		return(NULL);	
	}	
	else {
		return (S->a[S->top]);
	}	
}

void SPopP (tStackP *S)
/*         --------
** Odstraní prvek z vrcholu zásobníku
**/
{
                            /* Operace nad prázdným zásobníkem způsobí chybu. */
	if (S->top==0)  {
		printf("Chyba: Došlo k podtečení zásobníku s ukazateli!\n");
	}	
	else {
		S->top--;
	}	
}

int SSizeP (tStackP *S) {
/*   -------
** Vrátí počet prvků v zásobníku
**/
  return(S->top);
}

bool SEmptyP (tStackP *S)
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
{
  return(S->top==0);
}	

/* -------------------------------------------------------------------------- */
/*
** Funkce pro zásobník hotnot typu bool. Tyto funkce neupravujte.
*/

void SInitB (tStackB *S) {
/*   ------
** Inicializace zásobníku.
**/

	S->top = 0;  
}	

void SPushB (tStackB *S,bool val) {
/*   ------
** Vloží hodnotu na vrchol zásobníku.
**/
                 /* Při implementaci v poli může dojít k přetečení zásobníku. */
	if (S->top==MAXSTACK) 
		printf("Chyba: Došlo k přetečení zásobníku pro boolean!\n");
	else {
		S->top++;  
		S->a[S->top]=val;
	}	
}

bool STopPopB (tStackB *S) {
/*   --------
** Odstraní prvek z vrcholu zásobníku a současně vrátí jeho hodnotu.
**/
                            /* Operace nad prázdným zásobníkem způsobí chybu. */
	if (S->top==0) {
		printf("Chyba: Došlo k podtečení zásobníku pro boolean!\n");
		return(NULL);	
	}	
	else {  
		return(S->a[S->top--]); 
	}	
}

bool STopB (tStackB *S)
/*         --------
** Vrátí hodnotu prvku na vrcholu zásobníku
**/
{
                            /* Operace nad prázdným zásobníkem způsobí chybu. */
	if (S->top==0)  {
		printf("Chyba: Došlo k podtečení zásobníku s ukazateli!\n");
		return(NULL);	
	}	
	else {
		return (S->a[S->top]);
	}	
}

void SPopB (tStackB *S)
/*         --------
** Odstraní prvek z vrcholu zásobníku
**/
{
                            /* Operace nad prázdným zásobníkem způsobí chybu. */
	if (S->top==0)  {
		printf("Chyba: Došlo k podtečení zásobníku s ukazateli!\n");
	}	
	else {
		S->top--;
	}	
}

int SSizeB (tStackB *S) {
/*   -------
** Vrátí počet prvků v zásobníku
**/
  return(S->top);
}

bool SEmptyB (tStackB *S) {
/*   -------
** Je-li zásobník prázdný, vrátí hodnotu true.
**/
  return(S->top==0);
}

/* -------------------------------------------------------------------------- */
/*
** Následuje jádro domácí úlohy - funkce, které máte implementovat. 
*/

void BTInit (tBTNodePtr *RootPtr)	{
/*   ------
** Provede inicializaci binárního vyhledávacího stromu.
**
** Inicializaci smí programátor volat pouze před prvním použitím binárního
** stromu, protože neuvolňuje uzly neprázdného stromu (a ani to dělat nemůže,
** protože před inicializací jsou hodnoty nedefinované, tedy libovolné).
** Ke zrušení binárního stromu slouží procedura BTDisposeTree.
**	
** Všimněte si, že zde se poprvé v hlavičce objevuje typ ukazatel na ukazatel,	
** proto je třeba při práci s RootPtr použít dereferenční operátor *.
**/
	
		
	 *RootPtr = NULL;	// should suffice? -> check when testing

}

void BTInsert (tBTNodePtr *RootPtr, int Content) {
/*   --------
** Vloží do stromu nový uzel s hodnotou Content.
**
** Z pohledu vkládání chápejte vytvářený strom jako binární vyhledávací strom,
** kde uzly s hodnotou menší než má otec leží v levém podstromu a uzly větší
** leží vpravo. Pokud vkládaný uzel již existuje, neprovádí se nic (daná hodnota
** se ve stromu může vyskytnout nejvýše jednou). Pokud se vytváří nový uzel,
** vzniká vždy jako list stromu. Funkci implementujte nerekurzivně.
**/
	
	// new node we wanna insert //
	tBTNodePtr node;


	// there is no root - root is empty
	if (!(*RootPtr)) {

		// allocation of the new node we want to insert
		node = malloc(sizeof(struct tBTNode));
		if (!node) {
			return;
		}

		// fill the node
		node->Cont = Content;
		node->LPtr = NULL;
		node->RPtr = NULL;
	
		*RootPtr = node;	// save the node
	}
	// we have the root -> we have to save it to a propper place in the tree
	// check left + right following the key (Content)
	// create the new node eventually & save it
	else {
		
		node = (*RootPtr);
		tBTNodePtr nodeF;	// father node

		while(node) {

			nodeF = node;	// save the father node

			// the node already exists
			if (node->Cont == Content) {
				return;
			}
			else {

				// the key is smaller -> go right
				if ((node->Cont) < Content) {
					node = node->RPtr;
				}
				// the key is bigger -> go left
				else {
					node = node->LPtr;
				}
			}
		}

		// allocation of the new node we want to insert
		node = malloc(sizeof(struct tBTNode));
		if (!node) {
			return;
		}

		// fill the node
		node->Cont = Content;
		node->LPtr = NULL;
		node->RPtr = NULL;		

		// check where to save the node in relation to the father node
		// and save it
		if ((nodeF->Cont) < Content) {
			nodeF->RPtr = node;
		}
		else {
			nodeF->LPtr = node;
		}
	}

	// all situations should be covered
}

/*                                  PREORDER                                  */

void Leftmost_Preorder (tBTNodePtr ptr, tStackP *Stack)	{
/*   -----------------
** Jde po levě větvi podstromu, dokud nenarazí na jeho nejlevější uzel.
**
** Při průchodu Preorder navštívené uzly zpracujeme voláním funkce BTWorkOut()
** a ukazatele na ně is uložíme do zásobníku.
**/

	// go as down (left) as poss
	while(ptr) {

		SPushP(Stack, ptr);	// save ptr
		BTWorkOut(ptr);	// magic to apply = print out the vals

		ptr = ptr->LPtr;	// go to the next node
	}
	
}

void BTPreorder (tBTNodePtr RootPtr)	{
/*   ----------
** Průchod stromem typu preorder implementovaný nerekurzivně s využitím funkce
** Leftmost_Preorder a zásobníku ukazatelů. Zpracování jednoho uzlu stromu
** realizujte jako volání funkce BTWorkOut(). 
**/

	tStackP ST;				// stack of pointers
	tBTNodePtr temp;		// temp pointer...

	// stack initialization
	SInitP(&ST);

	// go left as much as poss
	Leftmost_Preorder(RootPtr, &ST);

	// repeat the process until the stack is not empty
	while(!SEmptyP(&ST)) {

		temp = STopPopP(&ST);	// get the top
		Leftmost_Preorder(temp->RPtr, &ST);	
	}

}


/*                                  INORDER                                   */ 

void Leftmost_Inorder(tBTNodePtr ptr, tStackP *Stack)		{
/*   ----------------
** Jde po levě větvi podstromu, dokud nenarazí na jeho nejlevější uzel.
**
** Při průchodu Inorder ukládáme ukazatele na všechny navštívené uzly do
** zásobníku. 
**/
	
	while(ptr) {

		// save the leftmost nodes in the stack
		SPushP(Stack, ptr);
		ptr = ptr->LPtr;
	}

	
}

void BTInorder (tBTNodePtr RootPtr)	{
/*   ---------
** Průchod stromem typu inorder implementovaný nerekurzivně s využitím funkce
** Leftmost_Inorder a zásobníku ukazatelů. Zpracování jednoho uzlu stromu
** realizujte jako volání funkce BTWorkOut(). 
**/

	tStackP ST;					// stack of pointers
	tBTNodePtr temp;			// temp pointer...

	// stack initialization
	SInitP(&ST);

	// go left as much as poss
	Leftmost_Inorder(RootPtr, &ST);

	// repeat the process until the stack is not empty
	while(!SEmptyP(&ST)) {

		temp = STopPopP(&ST);	// get the top

		// fill up the node and go to the next one
		// do I have to check RPtr?? - I will see...
		BTWorkOut(temp);
		Leftmost_Inorder(temp->RPtr, &ST);	
	}

	// do I have to dispose the stack or what...? - chck later
}

/*                                 POSTORDER                                  */ 

void Leftmost_Postorder (tBTNodePtr ptr, tStackP *StackP, tStackB *StackB) {
/*           --------
** Jde po levě větvi podstromu, dokud nenarazí na jeho nejlevější uzel.
**
** Při průchodu Postorder ukládáme ukazatele na navštívené uzly do zásobníku
** a současně do zásobníku bool hodnot ukládáme informaci, zda byl uzel
** navštíven poprvé a že se tedy ještě nemá zpracovávat. 
**/

	while(ptr) {

		// 2 stack implementation -> saving the nodes
		SPushP(StackP, ptr);
		SPushB(StackB, TRUE);

		ptr = ptr->LPtr;
	}

}

void BTPostorder (tBTNodePtr RootPtr)	{
/*           -----------
** Průchod stromem typu postorder implementovaný nerekurzivně s využitím funkce
** Leftmost_Postorder, zásobníku ukazatelů a zásobníku hotdnot typu bool.
** Zpracování jednoho uzlu stromu realizujte jako volání funkce BTWorkOut(). 
**/

	// those 2 stacks + temp...
	tStackP StackP;
	tStackB StackB;
	tBTNodePtr temp;

	// stack initialization
	SInitP(&StackP);
	SInitB(&StackB);

	// go left as much as poss
	Leftmost_Postorder(RootPtr, &StackP, &StackB);

	// repeat the process until the stack is not empty
	while(!SEmptyP(&StackP)) {

		// look what do we have on the top of the stack
		temp = STopPopP(&StackP);
		SPushP(&StackP, temp); 

		// comes from the left, goes right
		if (STopPopB(&StackB)) {

			SPushB(&StackB, FALSE);
			Leftmost_Postorder(temp->RPtr, &StackP, &StackB);
		}
		else {

			temp = STopPopP(&StackP);
			BTWorkOut(temp);
		}

	}

	// Leftmost covers the NULL situation, should be all right

}


int BTHeight (tBTNodePtr RootPtr) {	
/*   ----------
** Vypočítá výšku BVS bez použití rekurze
**
** Návratová hodnota je výška stromu. Funkci implementujte nerekurzivně
** bez deklarování jakékoli další pomocné funkce, která není v zadání.
** Využijte pomocných zásobníků. Je doporučeno použít jeden ze zásobníků
** pro průběžné ukládání cesty od kořene stromu. Počet uzlů na takovéto
** cestě můžete zjistit použitím funkce SSizeP. Výška stromu je rovna
** délce (počtu hran) nejdelší cesty  od kořene k lisu.
**
** Výška prázdného stromu však není definována. V případě prázdného stromu
** bude funkce vracet hodnotu -1.  
**/
	
	tStackP ST;	// stacK

	int h = 0;				// tree height
	bool leftTr = false;	// traverse check

	// stack initialization
	SInitP(&ST);

	// tree is empty
	if(!RootPtr) {
		return -1;
	}

	while(1) {

		// push the right nodes / leafs
		while (RootPtr != NULL && leftTr == false) {

			SPushP(&ST, RootPtr);
			RootPtr = RootPtr->LPtr;

		}

		// set up new height of the tree
		if (h < SSizeP(&ST)) {
			h = SSizeP(&ST);
		}
		// the whole tree has been checked
		if (SEmptyP(&ST)) {
			break;
		}

		// we only peek = get only the value of the top
		tBTNodePtr temp = STopPopP(&ST);
		SPushP(&ST, temp);	// put the val back

		/* going through the right traverse */
		if (temp->RPtr != NULL && temp->RPtr != RootPtr) {
			RootPtr = temp->RPtr;
			leftTr = false;
		}
		else {
			RootPtr = STopPopP(&ST);	// remove the node
			leftTr = true;				// back on the left trav
		}

	}

	// heigh - 1 due to the first root node, whose height is 0 
	return (h - 1);

}


void BTDisposeTree (tBTNodePtr *RootPtr)	{
/*   -------------
** Zruší všechny uzly stromu a korektně uvolní jimi zabranou paměť.
**
** Funkci implementujte nerekurzivně s využitím zásobníku ukazatelů.
**/

	// stack and temp...
	tStackP ST;
	tBTNodePtr temp;

	if ((*RootPtr)) {

		// stack initialization
		SInitP(&ST);

		// main cycle -> removing all the nodes 
		do {

			// root's NULL && stack still has elements -> pop the top
			if ((!(*RootPtr)) && (!SEmptyP(&ST))) {

				*RootPtr = STopPopP(&ST);
			}
			// goes left, disposes and "rights" pushes to the stack
			else {

				if ((*RootPtr)->RPtr) {
					SPushP(&ST, (*RootPtr)->RPtr);	// push the right one
				}

				// save the root node & go to the next
				temp = (*RootPtr);
				*RootPtr = (*RootPtr)->LPtr;

				free(temp);	// disposes the node
			}

		} while ((*RootPtr) || (!SEmptyP(&ST)));	
		// till there is stuff to remove //

	}

	/*else situation -> if the tree ain't exist, there is nothing to remove, so should be all right */

}

/* konec c402.c */

