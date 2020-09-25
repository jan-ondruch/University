/* ******************************** parser.c **********************************/
/* Soubor:              parser.c - Syntaktická analýza                        */
/* Kodovani:            UTF-8                                                 */
/* Datum:               13. 12. 2015                                          */
/*                                                                            */
/* Predmet:             Formalni jazyky a prekladace (IFJ)                    */
/* Projekt:             Implementace interpretu jazyka IFJ15                  */
/* Varianta zadani:     a/1/I                                                 */
/*                                                                            */
/* Autori, login:       Tereza Kabeláčová           xkabel06                  */
/*                      Adriana Jelenčíková         xjelen10                  */
/*                      Jan Ondruch                 xondru14                  */
/*                      Michal Klhůfek              xklhuf00                  */
/*                      Martin Auxt                 xauxtm00                  */
/* ****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "paska.h"
#include "main.h"
#include "parser.h"
#include "str.h"
#include "ial.h"
#include "garbage.h"


token actToken;     // aktualni token
Toperations tOper;  // operace - pomocne promenne etc. 
tTape T;            // T jako Tejp
bStromPtr BTR;      // stromcek
bStromPtr BTR_LOC;  // local strom
TSymbolPtr SYMB;    // tabulka symbolu 

int counterVar = 1;   // counter pro generovani unikatnich labels -> generateVariable
int count_zloz = 0;   // counter pro slozene zavorky -> vytvareni novych frejmu
int count_fpars = 0;  // counter pro pocet parametru funkce
int count_i = 0;      // counter pro indexy argumentu

// ==================================================================
// POMOCNE SEMANTICKE FUNKCE
// ==================================================================

/* dostane aktualni token a dle nej rozhazi data do tOpOne / tResult */
int typeToOperand(token actToken, int resultOnTheTape, int leftVal) {

  bStromPtr TmpTree;  // pomocny strom pro vyhledavani ID
  TmpTree = BTR_LOC;  // zacnu hledat v Local a pujdu vys

  tOper.tOpOne.data = NULL; // vynulovani dat, at nedelaji neporadek
  tOper.tOpOne.elemIDptr = NULL;

  switch(actToken.typ)
  {

    /* podle typu priradime do tOp hodnotu -> pak ten tOp pouziju jako formalni par. v generate() */
    case CELECISLO:

      tOper.tOpOne.typ = actToken.typ;
      if (leftVal != 4) {
        tOper.tOpOne.elemIDptr = NULL;
        tOper.tOpOne.cislo = actToken.cislo;
      }
      if (resultOnTheTape == 0) tOper.tOpOne.resultOnTheTape = false;
      else tOper.tOpOne.resultOnTheTape = true;

      return ERR_CODE_SUCC;
    break;

    case DESETINE:

      tOper.tOpOne.typ = actToken.typ;
      if (leftVal != 4) {
        tOper.tOpOne.elemIDptr = NULL;
        tOper.tOpOne.desetin = actToken.desetin;
      }
      if (resultOnTheTape == 0) tOper.tOpOne.resultOnTheTape = false;
      else tOper.tOpOne.resultOnTheTape = true;

      return ERR_CODE_SUCC;
    break;

    case RETEZEC:

      tOper.tOpOne.typ = actToken.typ;
      if (leftVal != 4) {
        tOper.tOpOne.elemIDptr = NULL;
        tOper.tOpOne.data = actToken.data;
      }
      if (resultOnTheTape == 0) tOper.tOpOne.resultOnTheTape = false;
      else tOper.tOpOne.resultOnTheTape = true;

      return ERR_CODE_SUCC;
    break;

    case ID:

      // zde uz mam <expr> vse najdene ve strome a prirazene -> nemusim nic hledat
      if (leftVal == 0) {
        // vse najdeno
      }
      // je to leva hodnota -> davame do tOper.tResult
      else if (leftVal == 1) {

        while (TmpTree != NULL) {
          // pokud nenajdu - NULL -> jdu o vrstvu vys
          if ((tOper.tResult.elemIDptr = BSearch(TmpTree, actToken.data)) == NULL) {
            TmpTree = TmpTree->data->Higher;
            continue;
          }  // najdu node kde je ID
          // nasel jsem ID
          else {
            tOper.tResult.typ = tOper.tResult.elemIDptr->data->type;
            return ERR_CODE_SUCC;
          }
        }
        return ERR_CODE_DEF;

      }
      // pro COUT a dalsi funkce (musim vyhledat ID ve strome, jinde to za me dela expr)
      else if (leftVal == 3) {

        while (TmpTree != NULL) {
          // pokud nenajdu - NULL -> jdu o vrstvu vys
          if ((tOper.tOpOne.elemIDptr = BSearch(TmpTree, actToken.data)) == NULL) {
            TmpTree = TmpTree->data->Higher;
            continue;
          }  // najdu node kde je ID
          // nasel jsem ID
          else {
            tOper.tOpOne.typ = tOper.tOpOne.elemIDptr->data->type;
            return ERR_CODE_SUCC;
          }
        }
        return ERR_CODE_DEF;

      }

      return ERR_CODE_SUCC;
    break;

    default:
      return ERR_CODE_SYN;
  }
  return ERR_CODE_SYN;
}

/* generuje jedinecne labels */
void generateVariable(string *var)
// generuje jedinecne nazvy identifikatoru
// nazev se sklada ze znaku $ nasledovanym cislem
// postupne se tu generuji prirozena cisla a do nazvu promenne se ukladaji
// v reverzovanem poradi - na funkcnost to nema vliv, ale je jednodussi implementace

{
  strClear(var);
  strAddChar(var, '$');
  int i;
  i = counterVar;
  while (i != 0)
  {
    strAddChar(var, (char)(i % 10 + '0'));
    i = i / 10;
  }
  counterVar ++;
}

// POMOCNE SEMANTICKE FUNKCE - KONEC
// ==================================================================

// ==================================================================
// JEDNOTLIVE FCE LL GRAMATIKY
// ==================================================================


/*
 * <asign> -> "ID" "(" <params_in> ")"
 * <asign> -> <expr> 
 */
int rAsign()
{
  int result;

  // <asign> -> <expr>  "musime pridat za volani if, jestli nahodou to nebyla fce -> pokracujeme dal, else return"
  
  tOper.isFunction = true;
  result = exprMain();
  if (result != ERR_CODE_SUCC) return result;

  // generujeme instrukci na prirazeni hodnoty z expr
  // u funkce -> prirazuju return hodnotu z funkce
    if ((result = generate(&T, PRIRAD_X, NULL, NULL, (void*) &(tOper.tResult))) != ERR_CODE_SUCC) return result;

  return ERR_CODE_SUCC;

  // moznost, ze je to funkce dodelavam v expr

}

/*
 * <asign_2> -> "=" <expr>
 * <asign_2> -> "EPSILON"
 */
int rAsign_2()
{
  int result;

  // <asign_2> -> "EPSILON" - cekame strednik
  if (actToken.typ == STREDNIK) return ERR_CODE_SUCC;

  // <asign_2> -> "=" <expr>
  if (actToken.typ != PRIRAD) return ERR_CODE_SYN;

  // dalsi token si precte expr
  result = exprMain();
  if (result != ERR_CODE_SUCC) return result;

  // generujeme instrukci na prirazeni hodnoty z expr

    if ((result = generate(&T, PRIRAD_X, NULL, NULL, (void*) &(tOper.tResult))) != ERR_CODE_SUCC) return result;

  // check stredniku z <expr>
  if (actToken.typ != STREDNIK) return ERR_CODE_SYN;

  return ERR_CODE_SUCC;
}

/*
 * <vars> -> "ID" "=" <asign> ";"
 * <vars> -> <type> "ID" <asign_2> ";"
 */
int rVars()
{
  int result;

  switch (actToken.typ)
  {

    // <vars> -> "ID" "=" <asign> ";"
    case ID:

      // ID do name a potom vyhledam, jestli uz bylo definovane

      // ulozime si ID jako tResult, do ktereho priradime - leftVal je na 1 pac prirazujeme
      if ((result = typeToOperand(actToken, 0, 1)) != ERR_CODE_SUCC) return result;

      // zadame token a cekame "="
      actToken = getNextToken();
      if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
      if (actToken.typ != PRIRAD) return ERR_CODE_SYN;

      // volame <asign>
      result = rAsign();
      if (result != ERR_CODE_SUCC) return result;

      // check stredniku z asign
      if (actToken.typ != STREDNIK) return ERR_CODE_SYN;

      return ERR_CODE_SUCC;

    break;

    // <vars> -> <type> "ID" <asign_2> ";"
    // <type> tu vypisu manualne
    case KINT:
    case KDOUBLE:
    case KSTRING:
    case KAUTO:

      if (actToken.typ == KAUTO) actToken.typ = KINT;  // hacking
      SYMB.type = actToken.typ; // ulozime typ symbolu (i auto)

      // zadame token a cekame "ID"
      actToken = getNextToken();
      if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
      if (actToken.typ != ID) return ERR_CODE_SYN;

        // ulozeni ID do stromu - provazeme ho s typem symbolu
        SYMB.name = gMalloc(sizeof(char) * ((strlen(actToken.data))+1));
        SYMB.name = actToken.data;  // ulozime data

        SYMB.isDeclared = true;
        SYMB.isFunction = false;
        SYMB.pocet_params = 0;

        // ulozime ID s typem do stromu (if not err)

        //vloz symbol do tabulky ak uz nie je deklarovany v aktualnom bloku
        if ((BSearch(BTR_LOC, SYMB.name) == NULL) && (BSearch(BTR, SYMB.name) == NULL)) {

          result = BSInsert(&BTR_LOC, SYMB.name, &SYMB);
          if (result != ERR_CODE_SUCC) return result;

        } 
        else {
          return ERR_CODE_DEF;
        }


      // ulozime si ID jako tResult, do ktereho priradime - leftVal je na 1
      if ((result = typeToOperand(actToken, 0, 1)) != ERR_CODE_SUCC) return result;

      // zadame token a volame rAsign_2
      actToken = getNextToken();
      if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
      result = rAsign_2();
      if (result != ERR_CODE_SUCC) return result;
      
      return ERR_CODE_SUCC;

      // strednik mam uz checkly z rAsign_2
    break;

    default:
      return ERR_CODE_SYN;
  }
  // nepovoleny token -> chyba
  return ERR_CODE_SYN;
}

/* 
 * <params_in_n> -> , <term> <params_in_n> 
 * <params_in_n> -> "EPSILON"
 */
int rParams_in_n()
{
  int result;

  // najdeme uzly pro argumenty a posleme do nich formalni parametry
    bStromPtr calledFunc = BSearch(BTR, SYMB.name);

  // <params_n> -> "EPSILON" - params_in_n je prazdne, cekam ")"
  if (actToken.typ == PZAVORKA) {

    if ((count_i/2) != calledFunc->data->pocet_params) return ERR_CODE_COM;
    return ERR_CODE_SUCC; // zpet do rParams_in
  }

  if (actToken.typ != CARKA) return ERR_CODE_SYN;

  // zadame o token a volame rTerm
  actToken = getNextToken();
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  result = rTerm();
  if (result != ERR_CODE_SUCC) return result; 

    if ((count_i/2) >= calledFunc->data->pocet_params) return ERR_CODE_COM;

    // najdi mi node v podstromu funkce, do ktereho hodnotu z tOper.OpOne hodim
    calledFunc = BSearch(calledFunc->data->Local, calledFunc->data->arg[count_i]);
    if (tOper.tOpOne.typ == CELECISLO) {
      if (calledFunc->data->type != KINT) {
        return ERR_CODE_COM;
      }
      calledFunc->data->hodnota.ival = tOper.tOpOne.cislo;
    }
    else if (tOper.tOpOne.typ == DESETINE) {
      if (calledFunc->data->type != KDOUBLE) {
        return ERR_CODE_COM;
      }
      calledFunc->data->hodnota.dval = tOper.tOpOne.desetin;
    }
    else if (tOper.tOpOne.typ == RETEZEC) {
      if (calledFunc->data->type != KSTRING) {
        return ERR_CODE_COM;
      }
      calledFunc->data->hodnota.sval = tOper.tOpOne.data;
    }
    // nefunkcni
    else if (tOper.tOpOne.typ == ID) {  
      calledFunc = tOper.tOpOne.elemIDptr;
    }

    calledFunc->data->isInit = true;  // automaticky je inicializovana
    calledFunc->data->isDefined = true;  // automaticky je inicializovana
    calledFunc->data->isDeclared = true;  // automaticky je inicializovana

    count_i+=2;

  // zadame o dalsi token a volame rekurzivne rParams_in_n
  actToken = getNextToken();
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  return rParams_in_n();
}

/*
 * <params_in> -> <term> <params_in_n>
 * <params_in> -> "EPSILON" 
 */
int rParams_in()
{
  int result;

    count_i = 1;

  // najdeme uzly pro argumenty a posleme do nich formalni parametry
    bStromPtr calledFunc = BSearch(BTR, SYMB.name);

  // params jsou prazdne -> <params_in> -> "EPSILON", cekame ")"
  if (actToken.typ == PZAVORKA) {

    if ((count_i/2) != calledFunc->data->pocet_params) return ERR_CODE_COM;
    return ERR_CODE_SUCC;
  }

    // main nemuze mit 
    if(strcmp(SYMB.name, "main") == 0){
      return ERR_CODE_COM;
    }

  // token uz mam z rAsign
  // <params_in> -> <term> <params_in_n>
  result = rTerm();
  if (result != ERR_CODE_SUCC) return result; 

    if ((count_i/2) >= calledFunc->data->pocet_params) return ERR_CODE_COM;

    // najdi mi node v podstromu funkce, do ktereho hodnotu z tOper.OpOne hodim
    calledFunc = BSearch(calledFunc->data->Local, calledFunc->data->arg[count_i]);
    
    if (tOper.tOpOne.typ == CELECISLO /*|| tOper.tOpOne.typ == KINT*/) {
      if (tOper.tOpOne.elemIDptr) {
        calledFunc = tOper.tOpOne.elemIDptr;
      }
      if (calledFunc->data->type != KINT) {
        return ERR_CODE_COM;
      }
      calledFunc->data->hodnota.ival = tOper.tOpOne.cislo;
    }
    else if (tOper.tOpOne.typ == DESETINE) {
      if (calledFunc->data->type != KDOUBLE) {
        return ERR_CODE_COM;
      }
      calledFunc->data->hodnota.dval = tOper.tOpOne.desetin;
    }
    else if (tOper.tOpOne.typ == RETEZEC) {
      if (calledFunc->data->type != KSTRING) {
        return ERR_CODE_COM;
      }
      calledFunc->data->hodnota.sval = tOper.tOpOne.data;
    }
    // nefunkcni
    else if (tOper.tOpOne.typ == ID) {  
      calledFunc = tOper.tOpOne.elemIDptr;
    }

    calledFunc->data->isInit = true;      // automaticky je inicializovana
    calledFunc->data->isDefined = true;   // automaticky je inicializovana
    calledFunc->data->isDeclared = true;  // automaticky je inicializovana

    count_i = 3;

  // zadame token a volame <params_in_n>
  actToken = getNextToken();
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  // rule rParams_in_n
  result = rParams_in_n();
  if (result != ERR_CODE_SUCC) return result;

  return ERR_CODE_SUCC;
}

/*
 * <return> -> "RETURN" <expr> ";"
 */
int rReturn()
{
  int result;
  tOper.isReturn = true;
  // volame <expr>
  result = exprMain();
  if (result != ERR_CODE_SUCC) return result;

    // vysledek return mame v NOP_X
    // vygenerujeme RET_X -> vydledek v interpretu vytahneme z predchoziho NOP_X a ulozime do RET_X
    if (!(!(strcmp(tOper.func_st_lab, "main")))) { // pokud nemam main
      tOper.tOpOne.resultOnTheTape = true;
      tOper.tOpOne.elemIDptr = NULL;  // asi
      if ((result = generate(&T, RET_X, (void*) &(tOper.tOpOne), NULL, NULL)) != ERR_CODE_SUCC) return result;
    }

  // jdeme skakat
  // nejprve unikatni label1 generujeme
    string newLabel1;
    strInit(&newLabel1);
    generateVariable(&newLabel1);
    tOper.func_end_lab = newLabel1.str; // ulozime unikatni label jako label konce funkce
    // ulozime label do tOpOne
    tOper.tOpOne.data = tOper.func_end_lab; // ulozime aktualni konec funkce -> novy lbl
    tOper.tOpOne.resultOnTheTape = false;
    tOper.tOpOne.elemIDptr = NULL;
    tOper.tOpOne.typ = RETEZEC;

    /* nevim jestli spravne - pokud mas main, vygeneruj mi skok na "endofmain" */
    if (!(strcmp(tOper.func_st_lab, "main"))) tOper.tOpOne.data = "endofmain";

      // zde generuj GOTO!!!! - value si ale nenesu z expr -> musim ji ulozit nejak jinak
      if ((result = generate(&T, GOTO_X, NULL, (void*) &(tOper.tOpOne), NULL)) != ERR_CODE_SUCC) return result;

  // strednik checknut v <expr>

  return ERR_CODE_SUCC;
}

/*
 * <term> -> "CELECISLO"
 * <term> -> "DESETINNE"
 * <term> -> "RETEZEC"
 * <term> -> "ID"
 */
int rTerm()
{
  int result;

  switch(actToken.typ)
  {

    /* podle typu priradime do tOp hodnotu -> pak ten tOp pouziju jako formalni par. v generate() */
    case CELECISLO:
    case DESETINE:
    case RETEZEC:
    case ID:
      
      // resultOnTheTape je 0 a je to prava hodnota ale pocitame pro cout a dalsi funkce, kde musim ID dohledat ve strome -> leftVal = 3
      if ((result = typeToOperand(actToken, 0, 3)) != ERR_CODE_SUCC) return result;
      return ERR_CODE_SUCC;
    break;

    default:
      return ERR_CODE_SYN;
  }
  // jiny typ -> chyba
  return ERR_CODE_SYN;
}

/*
 * <cout_n> -> "<<" <term> <cout_n>
 * <cout_n> -> "EPSILON"
 */
int rCout_n()
{
  int result;
  // <cout_n> -> "EPSILON"
  if (actToken.typ == STREDNIK) return ERR_CODE_SUCC;

  // cekame "<<", token uz jsme volali
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  if (actToken.typ != OCOUT) return ERR_CODE_SYN;

  // zadame token a volame <term>
  actToken = getNextToken();
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  result = rTerm();
  if (result != ERR_CODE_SUCC) return result;

  // vygenerujeme instrukci pro vypsani <term> hodnoty
    if ((result = generate(&T, WRITE_X, (void*) &(tOper.tOpOne), NULL, NULL)) != ERR_CODE_SUCC) return result;

  // zadame token a rekurzivne volame rCout_n
  actToken = getNextToken();
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  return rCout_n();
}

/*
 * <cout> -> "COUT" "<<" <term> <cout_n> ";"
 */
int rCout()
{
  int result;

  // zadame o token a cekame "<<"
  actToken = getNextToken();
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  if (actToken.typ != OCOUT) return ERR_CODE_SYN;

  // zadame token a volame <term>
  actToken = getNextToken();
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  result = rTerm();
  if (result != ERR_CODE_SUCC) return result;

    // vygenerujeme instrukci pro vypsani <term> hodnoty
    if ((result = generate(&T, WRITE_X, (void*) &(tOper.tOpOne), NULL, NULL)) != ERR_CODE_SUCC) return result;

  // zadame token a volame rCout_n
  actToken = getNextToken();
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  result = rCout_n();
  if (result != ERR_CODE_SUCC) return result;

  // ";" jsme uz volali v rCout_n -> konec rCout
  return ERR_CODE_SUCC;
}

/*
 * <cin_n> -> ">>" ID <cin_n>
 * <cin_n> -> "EPSILON"
 */
int rCin_n()
{
  int result;

  // <cin_n> -> "EPSILON"
  if (actToken.typ == STREDNIK) return ERR_CODE_SUCC;

  // cekame ">>", token jsme uz volali
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  if (actToken.typ != OCIN) return ERR_CODE_SYN;

  // zadame token a cekame "ID"
  actToken = getNextToken();
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  if (actToken.typ != ID) return ERR_CODE_SYN;

    // generujeme instrukci pro ID
    if ((result = typeToOperand(actToken, 0, 3)) != ERR_CODE_SUCC) return result;

  // vygenerujeme instrukci pro zapsani hodnoty do ID - do node ve strome
    if ((result = generate(&T, READ_X, (void*) &(tOper.tOpOne), NULL, NULL)) != ERR_CODE_SUCC) return result;

  // zadame token a rekurzivne volame rCin_n
  actToken = getNextToken();
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  return rCin_n();
}

/*
 * <cin> -> "CIN" ">>" ID <cin_n> ";"
 */
int rCin()
{
  int result;

  // zadame o token a cekame ">>"
  actToken = getNextToken();
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  if (actToken.typ != OCIN) return ERR_CODE_SYN;

  // zadame token a cekame "ID"
  actToken = getNextToken();
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  if (actToken.typ != ID) return ERR_CODE_SYN;

  // generujeme instrukci pro ID
    if ((result = typeToOperand(actToken, 0, 3)) != ERR_CODE_SUCC) return result;

  // vygenerujeme instrukci pro zapsani hodnoty do ID - do node ve strome
    if ((result = generate(&T, READ_X, (void*) &(tOper.tOpOne), NULL, NULL)) != ERR_CODE_SUCC) return result;

  // zadame token a volame rCin_n
  actToken = getNextToken();
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  result = rCin_n();
  if (result != ERR_CODE_SUCC) return result;

  // ";" jsme uz volali v rCin_n -> konec rCin
  return ERR_CODE_SUCC;
}

/*
 * <if> -> "IF" "(" <expr> ")" <stat_list> "ELSE" <stat_list>
 */
int rIf()
{
  int result;

  // volame <expr> - token si zada expr sam () kontroluju v expr
  tOper.expr_iftrg = true; 
  result = exprMain();
  if (result != ERR_CODE_SUCC) return result;

  // IF -> 1. prevratime hodnotu vyrazu
    if ((result = generate(&T, NOT_X, NULL, NULL, NULL)) != ERR_CODE_SUCC) return result;
  // nejprve unikatni label1 generujeme
    string newLabel1;
    strInit(&newLabel1);
    generateVariable(&newLabel1);
    // ulozime label do tOpOne
    tOper.tOpOne.data = newLabel1.str; // nemusim prvni alokovat misto v .data ??
    tOper.tOpOne.resultOnTheTape = false;
    tOper.tOpOne.elemIDptr = NULL;
    tOper.tOpOne.typ = RETEZEC;
  // a vygenerujeme GOTO (skoci, pokud byl puvodni vyraz nepravdivy) - NOT_X to prevrati -> true ->skok
    if ((result = generate(&T, GOTO_X, (void*) &(tOper.tOpOne), NULL, NULL)) != ERR_CODE_SUCC) return result;

  // zadame token a volame rStatList - musime ale checknout LSLOZENOU - uz jsme volali v expr
  if (actToken.typ != LSLOZENA) return ERR_CODE_SYN;
  result = rStatList();
  if (result != ERR_CODE_SUCC) return result;

  // zavolame token a cekame ELSE
  actToken = getNextToken();
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  if (actToken.typ != KELSE) return ERR_CODE_LEX;

  // nejprve unikatni label1 generujeme
    string newLabel2;
    strInit(&newLabel2);
    generateVariable(&newLabel2);
    // ulozime label do tOpOne
    tOper.tOpOne.data = newLabel2.str; // nemusim prvni alokovat misto v .data ??
    tOper.tOpOne.resultOnTheTape = false;
    tOper.tOpOne.elemIDptr = NULL;
    tOper.tOpOne.typ = RETEZEC;

  // instrukce pro skok -> zde skaceme vzdy (prisli jsme s blocku 1 v IF)
    if ((result = generate(&T, GOTO_X, NULL, (void*) &(tOper.tOpOne), NULL)) != ERR_CODE_SUCC) return result;
  // label1 - zde skocime, pokud byla podminka false
  
  tOper.tOpOne.data = newLabel1.str; // ulozime newLabel1 do label instrukce
  tOper.tOpOne.resultOnTheTape = false;
  tOper.tOpOne.elemIDptr = NULL;
  tOper.tOpOne.typ = RETEZEC;

    if ((result = generate(&T, LAB_X, (void*) &(tOper.tOpOne), NULL, NULL)) != ERR_CODE_SUCC) return result;

  // zadame token a volame rStatList - musime ale checknout LSLOZENOU
  actToken = getNextToken();
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  if (actToken.typ != LSLOZENA) return ERR_CODE_SYN;
  result = rStatList();
  if (result != ERR_CODE_SUCC) return result;

  // instrukce pro konec podminky -> skocime zde, if jsme prosli prvni blok
    // label2 - zde skocime, pokud byla podminka true
    tOper.tOpOne.data = newLabel2.str; // ulozime newLabel1 do label instrukce
    tOper.tOpOne.resultOnTheTape = false;
    tOper.tOpOne.elemIDptr = NULL;
    tOper.tOpOne.typ = RETEZEC;

    if ((result = generate(&T, LAB_X, (void*) &(tOper.tOpOne), NULL, NULL)) != ERR_CODE_SUCC) return result;

  return ERR_CODE_SUCC;
}

/*
 * <for> -> "FOR" "(" <type> "ID" <asign_2> ";" <expr> ";" "ID" "="
 * <expr> ")" <stat_list>
 */
int rFor() 
{
  int result;
  bStromPtr TmpTree;  // pomocny stromecek

  // vytvorime novy ramec pro FOR{hlavicka {telo}}

  /* vytvorime novy ramec + nazev unikatni vygenerujeme */
    count_zloz++; // novy frejm
    // nejprve unikatni nazev frejmu generujeme
    string forFrame1;
    strInit(&forFrame1);
    generateVariable(&forFrame1);

    BSInit(&TmpTree); // pomocny strom
    SYMB.Higher = BTR_LOC;
    SYMB.isFunction = false;
    SYMB.name = "0hlavicka";

    result = (BSInsert(&TmpTree, SYMB.name, &SYMB));
    if (result != ERR_CODE_SUCC) return result;

    // ukaladani infa do noveho lokalniho stromu
    SYMB.name = forFrame1.str;

    SYMB.Local = TmpTree;
    result = (BSInsert(&BTR_LOC, forFrame1.str, &SYMB));
    if (result != ERR_CODE_SUCC) return result;

    // Temp strom ulozime jako lokalni
    BTR_LOC = TmpTree;

  // zadame token a cekame "("
  actToken = getNextToken();
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  if (actToken.typ != LZAVORKA) return ERR_CODE_SYN;

  // zadame token a volame <type>
  actToken = getNextToken();
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  result = rType();
  if (result != ERR_CODE_SUCC) return result;

    if (actToken.typ == KAUTO) actToken.typ = KINT;  // hacking
    SYMB.type = actToken.typ; // ulozime typ symbolu (i auto)

  // zadame token a cekame "ID"
  actToken = getNextToken();
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  if (actToken.typ != ID) return ERR_CODE_SYN;

    // ulozeni ID do stromu - provazeme ho s typem symbolu
    SYMB.name = gMalloc(sizeof(char) * ((strlen(actToken.data))+1));
    SYMB.name = actToken.data;  // ulozime data

    SYMB.isDeclared = true;
    SYMB.isFunction = false;
    SYMB.pocet_params = 0;  //treba to vobec?

    // ulozime ID s typem do stromu (if not err)

    //vloz symbol do tabulky ak uz nie je deklarovany v aktualnom bloku
    if ((BSearch(BTR_LOC, SYMB.name) == NULL) && (BSearch(BTR, SYMB.name) == NULL)) {

      result = BSInsert(&BTR_LOC, SYMB.name, &SYMB);
      if (result != ERR_CODE_SUCC) return result;

    } 
    else {
      return ERR_CODE_DEF;
    }
  
  // ID jako operand tResult, do ktereho priradime vysledek z expr (v Asign_2)
    if ((result = typeToOperand(actToken, 0, 1)) != ERR_CODE_SUCC) return result;

  // zadame token a volame <asign_2>
  actToken = getNextToken();
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  result = rAsign_2();  // hodnotu do ID priradime v Asign_2
  if (result != ERR_CODE_SUCC) return result;

  // ";" uz mame volano a checknuto v rAsign_2
  // volame <expr> - podminka
  // 
  // generujeme navesti pro skok na test podminky
    string newLabel1;
    strInit(&newLabel1);
    generateVariable(&newLabel1);
    // ulozime label do tOpOne
    tOper.tOpOne.data = newLabel1.str; // nemusim prvni alokovat misto v .data ??
    tOper.tOpOne.resultOnTheTape = false;
    tOper.tOpOne.elemIDptr = NULL;
    tOper.tOpOne.typ = RETEZEC; 

    // generovani navesti
    if ((result = generate(&T, LAB_X, (void*) &(tOper.tOpOne), NULL, NULL)) != ERR_CODE_SUCC) return result;

  result = exprMain();
  if (result != ERR_CODE_SUCC) return result;

  // checkneme STREDNIK z <expr>
  if (actToken.typ != STREDNIK) return ERR_CODE_SYN;

  // FOR - contidion -> 1. prevratime hodnotu vyrazu
    if ((result = generate(&T, NOT_X, NULL, NULL, NULL)) != ERR_CODE_SUCC) return result;

  // a pote GOTO na Label2
    string newLabel2;
    strInit(&newLabel2);
    generateVariable(&newLabel2);
    // ulozime label2 do tOpOne
    tOper.tOpOne.data = newLabel2.str; // nemusim prvni alokovat misto v .data ??
    tOper.tOpOne.resultOnTheTape = false;
    tOper.tOpOne.elemIDptr = NULL;
    tOper.tOpOne.typ = RETEZEC;
  // a vygenerujeme GOTO (skoci, pokud byl puvodni vyraz nepravdivy) - NOT_X to prevrati -> true ->skok
    if ((result = generate(&T, GOTO_X, (void*) &(tOper.tOpOne), NULL, NULL)) != ERR_CODE_SUCC) return result;
  
  // do resultu prechoziho prvni dame 1
    tOper.tOpOne.typ = CELECISLO;
    tOper.tOpOne.cislo = 1;
    if ((result = generate(&T, NOP_X, NULL, NULL, (void*) &(tOper.tOpOne))) != ERR_CODE_SUCC) return result;

  // jeste musime obejit podminku -> skocime az za ni
    string newLabel3;
    strInit(&newLabel3);
    generateVariable(&newLabel3);
    // ulozime label3 do tOpOne
    tOper.tOpOne.data = newLabel3.str; // nemusim prvni alokovat misto v .data ??
    tOper.tOpOne.resultOnTheTape = false;
    tOper.tOpOne.elemIDptr = NULL;
    tOper.tOpOne.typ = RETEZEC;
  // a vygenerujeme GOTO - skoci vzdy - obejdeme podminku
    if ((result = generate(&T, GOTO_X, (void*) &(tOper.tOpOne), NULL, NULL)) != ERR_CODE_SUCC) return result;

  // label 4 -> vracime se zpet pro inkrementaci
    string newLabel4;
    strInit(&newLabel4);
    generateVariable(&newLabel4);
    // ulozime label do tOpOne
    tOper.tOpOne.data = newLabel4.str; // nemusim prvni alokovat misto v .data ??
    tOper.tOpOne.resultOnTheTape = false;
    tOper.tOpOne.elemIDptr = NULL;
    tOper.tOpOne.typ = RETEZEC; 

    // generovani navesti
    if ((result = generate(&T, LAB_X, (void*) &(tOper.tOpOne), NULL, NULL)) != ERR_CODE_SUCC) return result;


  /// ----------- INKREMENTACNI CAST ------------ ///

  // zadame token a cekame "ID"
  actToken = getNextToken();
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  if (actToken.typ != ID) return ERR_CODE_SYN;

  // ID jako leva hodnota, do ktere priradim result z expr -> prirazeni generuju uz zde (nemam Asign)
    if ((result = typeToOperand(actToken, 0, 1)) != ERR_CODE_SUCC) return result;

  // zadame token a cekame "="
  actToken = getNextToken();
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  if (actToken.typ != PRIRAD) return ERR_CODE_SYN;

  // volame <expr>
  result = exprMain();
  if (result != ERR_CODE_SUCC) return result;

    // generujeme instrukci na prirazeni hodnoty z expr
    if ((result = generate(&T, PRIRAD_X, NULL, NULL, (void*) &(tOper.tResult))) != ERR_CODE_SUCC) return result;


  /// ----------- KONEC INKREMENTACNI CAST ------------ /// 

  // a skaceme na L1 - inkrementovali jsme -> jdeme testovat podminku 

    tOper.tOpOne.data = newLabel1.str;
    tOper.tOpOne.resultOnTheTape = false;
    tOper.tOpOne.elemIDptr = NULL;
    tOper.tOpOne.typ = RETEZEC;

    // skaceme vzdy natvrdo zpet
    if ((result = generate(&T, GOTOBCK_X, NULL, (void*) &(tOper.tOpOne), NULL)) != ERR_CODE_SUCC) return result;

    // ulozime label3 do tOpOne
    tOper.tOpOne.data = newLabel3.str; // nemusim prvni alokovat misto v .data ??
    tOper.tOpOne.resultOnTheTape = false;
    tOper.tOpOne.elemIDptr = NULL;
    tOper.tOpOne.typ = RETEZEC; 

    // generovani navesti
    if ((result = generate(&T, LAB_X, (void*) &(tOper.tOpOne), NULL, NULL)) != ERR_CODE_SUCC) return result;

  // PZAVORKA checkla v <expr>

  // checkneme LSLOZENOU volanou v <expr>
  if (actToken.typ != LSLOZENA) return ERR_CODE_SYN;
  result = rStatList();
  if (result != ERR_CODE_SUCC) return result;

  // generujeme skok zpatky na podminku - na navesti L4 - vzdy probehne
  // instrukce pro skok -> zde skaceme vzdy
    tOper.tOpOne.data = newLabel4.str; // nemusim prvni alokovat misto v .data ??
    tOper.tOpOne.resultOnTheTape = false;
    tOper.tOpOne.elemIDptr = NULL;
    tOper.tOpOne.typ = RETEZEC;   

    if ((result = generate(&T, GOTOBCK_X, NULL, (void*) &(tOper.tOpOne), NULL)) != ERR_CODE_SUCC) return result;

  // zde label2 pro ukonceni FORu
    tOper.tOpOne.data = newLabel2.str;
    tOper.tOpOne.resultOnTheTape = false;
    tOper.tOpOne.elemIDptr = NULL;
    tOper.tOpOne.typ = RETEZEC;

    if ((result = generate(&T, LAB_X, (void*) &(tOper.tOpOne), NULL, NULL)) != ERR_CODE_SUCC) return result;

    BTR_LOC = BTR_LOC->data->Higher;  // konec ramce pro FOR

  return ERR_CODE_SUCC;
}

/*
 * <stat> -> "EPSILON"
 * <stat> -> <stat_list> <stat>
 * <stat> -> <for> <stat>
 * <stat> -> <return>
 * <stat> -> <if> <stat>
 * <stat> -> <cin> <stat>
 * <stat> -> <cout> <stat>
 * <stat> -> <vars> <stat>
 */
int rStat()
{
  int result;

  switch (actToken.typ)
  {

    // <stat> -> "EPSILON" - "}" uzavira rStatList
    case PSLOZENA:
      return ERR_CODE_SUCC;
    break;

    // <stat> -> <stat_list> <stat>
    case LSLOZENA:
      result = rStatList();
      if (result != ERR_CODE_SUCC) return result;

      // zadame dalsi token a rekurzivni volame <stat>
      actToken = getNextToken();
      if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
      return rStat();
    break;

    // <stat> -> <for> <stat>
    case KFOR:
      result = rFor();
      if (result != ERR_CODE_SUCC) return result;

      // zadame dalsi token a rekurzivni volame <stat>
      actToken = getNextToken();
      if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
      return rStat();
    break;

    // <stat> -> <return> <stat>
    case KRETURN:
      result = rReturn();
      if (result != ERR_CODE_SUCC) return result;

      // zadame dalsi token A volame rekurzivne <stat>
      actToken = getNextToken();
      if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
      return rStat();
    break;

    // <stat> -> <if> <stat>
    case KIF:
      result = rIf();
      if (result != ERR_CODE_SUCC) return result;

      // zadame dalsi token a rekurzivni volame <stat>
      actToken = getNextToken();
      if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
      return rStat();
    break;

    // <stat> -> <cin> <stat>
    case KCIN:
      result = rCin();
      if (result != ERR_CODE_SUCC) return result;

      // zadame dalsi token a rekurzivni volame <stat>
      actToken = getNextToken();
      if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
      return rStat();
    break;

    // <stat> -> <cout> <stat>
    case KCOUT:
      result = rCout();
      if (result != ERR_CODE_SUCC) return result;

      // zadame dalsi token a rekurzivni volame <stat>
      actToken = getNextToken();
      if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
      return rStat();
    break;

    // <stat> -> <vars> <stat>
    // vars predict: ID, INT, DOUBLE, STRING, AUTO
    // volame default - az ve <vars> zkontrolujeme co nam prislo
    default:
      result = rVars(); // pokud posilame ptakovinu -> result bude spatny
      if (result != ERR_CODE_SUCC) return result;

      // zadame dalsi token a rekurzivni volame <stat>
      actToken = getNextToken();
      if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
      return rStat();
  }
  // jiny token nez ocekavane
  return ERR_CODE_SYN;
}

/* <stat_list> -> "{" <stat> "}" */
int rStatList()
{
  int result;

  if(tOper.scopeOut == 0) {

    bStromPtr TmpTree;  // pomocny stromecek
    /* vytvorime novy ramec + nazev unikatni vygenerujeme */
      count_zloz++; // novy frejm
      // nejprve unikatni nazev frejmu generujeme
      string newFrame1;
      strInit(&newFrame1);
      generateVariable(&newFrame1);

      BSInit(&TmpTree); // pomocny strom
      SYMB.Higher = BTR_LOC;
      SYMB.isFunction = false;
      SYMB.name = "0hlavicka";

      result = (BSInsert(&TmpTree, SYMB.name, &SYMB));
      if (result != ERR_CODE_SUCC) return result;

      // ukaladani infa do noveho lokalniho stromu
      SYMB.name = newFrame1.str;

      SYMB.Local = TmpTree;
      result = (BSInsert(&BTR_LOC, newFrame1.str, &SYMB));
      if (result != ERR_CODE_SUCC) return result;

      // Temp strom ulozime jako lokalni
      BTR_LOC = TmpTree;
    }
    else tOper.scopeOut = 0;
  // "{" je uz checknuta
  // zadame dalsi token & volame <stat>
  actToken = getNextToken();
  
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;

  result = rStat();
  if (result != ERR_CODE_SUCC) return result;

  // dalsi token - "}" - uz byl volany ve <stat>
  if (actToken.typ != PSLOZENA) return ERR_CODE_SYN;

    BTR_LOC = BTR_LOC->data->Higher;

  return ERR_CODE_SUCC;
}

/*
 * <type> -> "INT"
 * <type> -> "DOUBLE"
 * <type> -> "STRING"
 * <type> -> "AUTO"
 */
int rType() 
{
  // int result neni treba, z type zadny Non-Terminal nevolam
  switch (actToken.typ)
  {
    case KINT:
    case KDOUBLE:
    case KSTRING:
    case KAUTO: // beware, ve funcki nemuze byt auto (prolly)

      return ERR_CODE_SUCC;
    break;

    default:
      return ERR_CODE_SYN;
  }
  // jiny token -> syntactic error
  return ERR_CODE_SYN;
}

/* 
 * <diff> -> <stat_list>
 * <diff> -> ";"
*/
int rDiff()
{
  int result;
  bStromPtr TmpTree;

  switch (actToken.typ)
  {
    case STREDNIK:
      SYMB.isDeclared = true;  // pri deklaraci
      result = BSInsert(&BTR, SYMB.name, &SYMB);
      if (result != ERR_CODE_SUCC) return result;

      TmpTree = BSearch(BTR, SYMB.name);

      if (TmpTree) TmpTree->data->Local->data->Higher = TmpTree;

      return ERR_CODE_SUCC;
    break;

    case LSLOZENA:  // cekame "{"

      // funkce uz byla definovana
      if (SYMB.isDefined == true) return ERR_CODE_DEF;

      // pokus o redefinici vestavene fce
      if((strcmp(SYMB.name,"length") ==0) || (strcmp(SYMB.name,"substr")==0) || (strcmp(SYMB.name,"concat")==0) || (strcmp(SYMB.name,"find")==0) || (strcmp(SYMB.name,"sort")==0))
          return ERR_CODE_DEF;

      SYMB.isDefined = true;  // pri definici
      result = BSInsert(&BTR, SYMB.name, &SYMB);
      if (result != ERR_CODE_SUCC) return result;

      TmpTree = BSearch(BTR, SYMB.name);

      if (TmpTree) TmpTree->data->Local->data->Higher = TmpTree;

      tOper.scopeOut = 1;

      result = rStatList();
      if (result != ERR_CODE_SUCC) return result;
      return ERR_CODE_SUCC;
    break;

    default:
      return ERR_CODE_SYN;
  }
  // pokud dostanu jiny token -> syntakticka chyba
  return ERR_CODE_SYN;
}

/* 
 * <params_n> -> "," <type> "ID" <params_n> 
 * <params_n> -> "EPSILON"
*/
int rParams_n()
{
  int result;
  
  // <params_n> -> "EPSILON" - params_n je prazdne, cekam ")"
  if (actToken.typ == PZAVORKA) return ERR_CODE_SUCC; // vracim se do rParams

  // jedeme dal podle <params_n> -> "," <type> "ID" <params_n>
  // token uz "," uz mame zavolany

  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  if (actToken.typ != CARKA) return ERR_CODE_SYN;
  // zadame o token a volame rType
  actToken = getNextToken();
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  result = rType();
  if (result != ERR_CODE_SUCC) return result;

    if (actToken.typ == KAUTO) actToken.typ = KINT;  // hacking

    // prisiel dalsi parameter
    count_fpars++;
    count_i++;
    
    if ((count_i == SYMB.alloc_pam - 1) || (count_i > SYMB.alloc_pam - 1)) {
  
      SYMB.arg = (char **)gRealloc(SYMB.arg, ((SYMB.alloc_pam)*2*sizeof(char*)));
      SYMB.alloc_pam *= 2; 
    }    

    SYMB.arg[count_i] = gMalloc(sizeof(char*) * ((strlen(actToken.data)) + 1));
    strcpy(SYMB.arg[count_i],actToken.data);

  // zadame o dalsi token -> cekame ID
  actToken = getNextToken();
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  if (actToken.typ != ID) return ERR_CODE_SYN;

    // ulozime ID
    SYMB.arg[count_i + 1] = gMalloc(sizeof(char*) * ((strlen(actToken.data)) + 1));
    strcpy(SYMB.arg[count_i + 1],actToken.data);
    count_i++;

  // zadame o dalsi token a volame rekurzivne rParams_n
  actToken = getNextToken();
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  return rParams_n(); // dobre se ty hodnoty budou prepisovat...?
}

/* 
 * <params> -> <type> "ID" <params_n> 
 * <params> -> "EPSILON"
*/
int rParams()
{
  int result;

    count_fpars = 0;

    if (SYMB.arg == NULL) SYMB.arg = gMalloc(sizeof(char *) * (SYMB.alloc_pam));  // defaultne pro 5 alokuju

  // <params> -> "EPSILON" - params je prazdne, cekam ")"
  if (actToken.typ == PZAVORKA) return ERR_CODE_SUCC; // vracim se do rFunc
  // else jedu podle <params> -> <type> "ID" <params_n>

  // volame rType - token uz jsem volal ve rFunc
  result = rType();
  if (result != ERR_CODE_SUCC) return result;

    if (actToken.typ == KAUTO) actToken.typ = KINT;  // hacking

    SYMB.arg[count_i] = gMalloc(sizeof(char*) * ((strlen(actToken.data)) + 1)); 
    strcpy(SYMB.arg[count_i], actToken.data);
    count_fpars++;
    count_i++;

  // zadame o dalsi token -> cekame ID
  actToken = getNextToken();
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  if (actToken.typ != ID) return ERR_CODE_SYN;

    // ulozime ID
    SYMB.arg[count_i] = gMalloc(sizeof(char*) * ((strlen(actToken.data)) + 1));
    strcpy(SYMB.arg[count_i],actToken.data);

  // zadame o dalsi token -> bud EPSILON (7.pravidlo -> ")") nebo <params_n> (carka)
  actToken = getNextToken();
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  // rule rParams_n
  result = rParams_n();
  if (result != ERR_CODE_SUCC) return result;

  return ERR_CODE_SUCC;
  // konec rParams (tady by program nemel nikdy dojit)
}

/* <func> -> <type> "ID" "(" <params> ")" <diff> <func> */
int rFunc() 
{
  int result;
  TSymbolPtr TmpTree;

    count_zloz = 0; // pro kazdou funkci pocitame znova
    count_i = 0;

  // volame rule rType (token uz jsem volal v parse || v rekurzi func)
  // bude to ok i pri rekurzi volat takhle token dopredu...?
  
  result = rType();
  if (result != ERR_CODE_SUCC) return result;

    if (actToken.typ == KAUTO) actToken.typ = KINT;  // hacking
    SYMB.type = actToken.typ; // ulozime typ

  // zadame o dalsi token -> cekame ID
  actToken = getNextToken();
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  if (actToken.typ != ID) return ERR_CODE_SYN;

    //id symbolu
    SYMB.name = gMalloc(sizeof(char) * 1024);
    if (SYMB.name == NULL) return ERR_CODE_OTH;
    SYMB.name = actToken.data;

    // pridani hlavicky
    if (BSearch(BTR, SYMB.name) == NULL) {
      BSInit(&BTR_LOC);
    
      TmpTree.Local = NULL;
      TmpTree.isFunction = false;
      TmpTree.name = gMalloc(sizeof(struct symbol));
      TmpTree.name = "0hlavicka";

      result = BSInsert(&BTR_LOC, TmpTree.name, &TmpTree);
      if (result != ERR_CODE_SUCC) return result;
    }

    // check out pozdeji, mozna to neni ok
    SYMB.Local= BTR_LOC;
    SYMB.Higher = NULL; 

    SYMB.isFunction = true; // mame funkci

    // jestli uz byla / nebyla definovana / deklarovana fce
    if (BSearch(BTR, actToken.data) == NULL) {
      SYMB.isDeclared = false;
      SYMB.isDefined = false;
    }
    if ((BSearch(BTR, actToken.data) != NULL) && (BSearch(BTR, actToken.data)->data->isDeclared) && !(BSearch(BTR, actToken.data)->data->isDefined)) SYMB.isDefined = false;

    // ulozime nazev funkce (pro label)
    tOper.func_st_lab = actToken.data;

  // zadame o dalsi token -> cekame (
  actToken = getNextToken();
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  if (actToken.typ != LZAVORKA) return ERR_CODE_SYN;

  // zadame o dalsi token -> volame <params>
  actToken = getNextToken();
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  result = rParams();
  if (result != ERR_CODE_SUCC) return result;

    SYMB.pocet_params = count_fpars; // ulozime pocet parametru funkce

  // PZAVORKA byla uz volana a checknuta

  // zadame o token a jdeme na <diff>
  actToken = getNextToken();
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;

    // odchyceni, jestil mame definici nebo deklaraci fce
    if (actToken.typ == STREDNIK) { // DEKLARACE
      // nejspis musim ihned volat rekurzivne funkci, at nize negeneruju nesmyslne labels
      // zadame o dalsi token a volame rekurzivne rFunc
       
      result = rDiff();
      if (result != ERR_CODE_SUCC) return result;
      goto noDefinition;

    }
    else if (actToken.typ == LSLOZENA) {// DEFINICE 
      // vygenerujeme label funkce == jeji nazev (ID)

      tOper.tOpOne.data = tOper.func_st_lab; // nazev funkce
      tOper.tOpOne.resultOnTheTape = false;
      tOper.tOpOne.elemIDptr = NULL;
      tOper.tOpOne.typ = RETEZEC;

      if ((result = generate(&T, LAB_X, (void*) &(tOper.tOpOne), NULL, NULL)) != ERR_CODE_SUCC) return result;

    }


  result = rDiff();
  if (result != ERR_CODE_SUCC) return result;

  // kontrola, jestli jsme volali return
  if(tOper.isReturn == false) {
    return ERR_CODE_INIT;
  }

  tOper.isReturn = false;
    // generujeme label konce funkce uz u return - zde jen volam value, co jsem si ulozil do tOper.func_end_lab 

    // ulozime label do tOpOne
    tOper.tOpOne.data = tOper.func_end_lab; // pridat do return func_end_lab!!  -check
    tOper.tOpOne.resultOnTheTape = false;
    tOper.tOpOne.elemIDptr = NULL;
    tOper.tOpOne.typ = RETEZEC; 

    // pokud mame konec mainu -> dame do label "endofmain"
    if (!(strcmp(tOper.func_st_lab, "main"))) {
      tOper.tOpOne.data = "endofmain";
    }

    // generovani navesti
    if ((result = generate(&T, LAB_X, (void*) &(tOper.tOpOne), NULL, NULL)) != ERR_CODE_SUCC) return result;

    // generovani GOTO_X zpet tam, odkud jsme funkci volali (pokud to bude main -> program skonci o instrukci vyse)
    tOper.tOpOne.data = "groover"; // zatim natvrdo - moznost volat maximalne 1 funkci
    tOper.tOpOne.resultOnTheTape = false;
    tOper.tOpOne.elemIDptr = NULL;
    tOper.tOpOne.typ = RETEZEC;  
    
    if ((result = generate(&T, GOTO_X, NULL, (void*) &(tOper.tOpOne), NULL)) != ERR_CODE_SUCC) return result;

    // goto here, pokud nemame definici
    noDefinition:

  // zadame o dalsi token a volame rekurzivne rFunc
  actToken = getNextToken();
  if (actToken.typ == CHBA) return ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) return ERR_CODE_LEX;
  if (actToken.typ == END_OF_FILE) return ERR_CODE_SUCC;  // konec programu, vracime se zpet do rProgram

  return rFunc();
}

/* <s> -> <func> */
int rProgram() 
{
  int result;
  // volame rule rFunc, token uz mam z parse
  result = rFunc();
  if (result != ERR_CODE_SUCC) return result;

  return ERR_CODE_SUCC; // vse ok
}

/* prase to cele nastartuje */
int parser()
{
  int result;
  
  BSInit(&BTR);       // inicializace stromu
  SYMB.alloc_pam = 5; // pocatecni pocet formalnich parametru funkce

  tTapeInit(&T); // inicializace pasky
  // na pasku hodime prazdny prvek (aby nam to nesestrelilo IF, kdyz se ptame na lptr->lptr)
  if ((result = generate(&T, NOP_X, NULL, NULL, NULL)) != ERR_CODE_SUCC) return result;

  //nejake inicializace tabulky + instrukci
  actToken = getNextToken();
  
  // chyba pri nacteni prvniho lexemu (i prazdny program by mel zde skoncit) 
  // zacneme s programem a pravidlem <s> -> <func>
  if (actToken.typ == CHBA) result = ERR_CODE_OTH; if (actToken.typ == LEX_ERROR) result = ERR_CODE_LEX;
  else result = rProgram();
  if (result != ERR_CODE_SUCC) return result;


  // zavreme pasku -> posledni instrukce
  if ((result = generate(&T, STOP_X, NULL, NULL, NULL)) != ERR_CODE_SUCC) return result; 
  // 
  // nejake uklizeni prolly

  return result;
}

// JEDNOTLIVE FCE LL GRAMATIKY - KONEC
// ==================================================================

// konec parser.c //