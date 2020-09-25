/* ******************************** str.h *************************************/
/* Soubor:              str.h - Prace s retezci (modul vypujcen z             */
/*                                              ukazkoveho interpretu)        */
/*                                                                            */
/* Kodovani:            UTF-8                                                 */
/* Datum:               9. 12. 2015                                           */
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

#ifndef STR_H
#define STR_H

typedef struct
{
  char* str;		// misto pro dany retezec ukonceny znakem '\0'
  int length;		// skutecna delka retezce
  int allocSize;	// velikost alokovane pameti
} string;


int strInit(string *s);
void strFree(string *s);

void strClear(string *s);
int strAddChar(string *s1, char c);
int strCopyString(string *s1, string *s2);
int strCmpString(string *s1, string *s2);
int strCmpConstStr(string *s1, char *s2);

char *strGetStr(string *s);
int strGetLength(string *s);

#endif