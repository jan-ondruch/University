/* ******************************** main.h ************************************/
/* Soubor:              main.h - Hlavičkový súbor pre riadiace                */
/*                      funkcie interpretu                                    */
/* Kodovani:            UTF-8                                                 */
/* Datum:               13. 12. 2015                                          */
/*                                                                            */
/* Predmet:             Formalni jazyky a prekladace (IFJ)                    */
/* Projekt:             Implementace interpretu jazyka IFJ15                  */
/* Varianta zadani:     a/1/I                                                 */
/*                                                                            */
/* Autori, login:       Tereza Kabeláčová             xkabel06                */
/*                      Adriana Jelenčíková           xjelen10                */
/*                      Jan Ondruch                   xondru14                */
/*                      Michal Klhůfek                xklhuf00                */
/*                      Martin Auxt                   xauxtm00                */
/* ****************************************************************************/

#include <stdbool.h>

#ifndef MAIN_H
#define MAIN_H

FILE* vstup;

enum {
  ERR_CODE_SUCC=0,	//Program proběhl správně
  ERR_CODE_LEX=1,	  //Chyba lexikální analýzy
  ERR_CODE_SYN=2,		//Chyba syntaktické analýzy
  ERR_CODE_DEF=3,		//Chyba sémantiky pro nedefinovaná nebo pokus o redefinici proměnné nebo funkce
  ERR_CODE_COM=4,		//Chyba sématiky při výrazech nebo par funkce (int+string)
  ERR_CODE_TYPE=5,  //Chyba typu
  ERR_CODE_SEM=6,	  //Jina semanticka chyba
  ERR_CODE_NUM=7,   //Chyba pri nacitani ciselne hodnoty
  ERR_CODE_INIT=8,	//Neinicializovana promenna
  ERR_CODE_DIV=9,	  //Chyba deleni nulou
  ERR_CODE_OTH=10,	//Ostatni behove chyby
  ERR_CODE_INTER=99	//Interní chyba interpreta
} ERROR_CODE;


void errorys(int result);

#endif