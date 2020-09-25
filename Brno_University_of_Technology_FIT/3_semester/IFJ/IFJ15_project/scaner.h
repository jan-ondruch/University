/* ******************************** scaner.h **********************************/
/* Soubor:              scaner.h - Knihovna pre lexikálny analyzátor          */
/* Kodovani:            UTF-8                                                 */
/* Datum:               9. 12. 2015                                           */
/*                                                                            */
/* Predmet:             Formalni jazyky a prekladace (IFJ)                    */
/* Projekt:             Implementace interpretu jazyka IFJ15                  */
/* Varianta zadani:     a/1/I                                                 */
/*                                                                            */
/* Autori, login:       Tereza Kabeláčová             xkabel06                */
/*                      Adriana Jelenčíková           xjelen10                */
/*                      Michal Klhůfek                xklhuf00                */
/*                      Jan Ondruch                   xondru14                */
/*                      Martin Auxt                   xauxtm00                */
/* ****************************************************************************/

#include <stdbool.h>

#ifndef SCANER_H
#define SCANER_H

typedef enum
  {
    LEX_ERROR = 0,       // 0
      //klicova slova
    KAUTO,               // 1   auto
    KCIN,                // 2   cin
    KCOUT,               // 3   cout
    KDOUBLE,             // 4   double
    KELSE,               // 5   else
    KFOR,                // 6   for
    KIF,                 // 7   if
    KINT,                // 8   int
    KRETURN,             // 9   return
    KSTRING,             // 10  string              
      //operatory 
    MINUS,               // 11  -
    PLUS,                // 12  +  
    MENSI,               // 13  <
    OCOUT,               // 14  <<
    MENSIROVNO,          // 15  <=
    PRIRAD,              // 16  =
    ROVNO,               // 17  ==
    NEROVNO,             // 18  !=
    VETSI,               // 19  >
    OCIN,                // 20  >>
    VETSIROVNO,          // 21  >=
    NASOBENI,            // 22  *
    DELENI,              // 23  /
      //jednotlive znaky
    LZAVORKA,            // 24  (
    PZAVORKA,            // 25  )
    LSLOZENA,            // 26  {
    PSLOZENA,            // 27  }
    CARKA,               // 28  ,
    STREDNIK,            // 29  ;
    radkovyKom,          // 30  //          je to zbytecne tu mit, ale mam 
    Zkomenta,            // 31  /*          to tu jen pro kontrolu, ze jsem 
    Kkomentar,           // 32  */          vzal v potaz vsechn moznosti
      // identifikatory
    CELECISLO,           // 33  int    - cele cislo
    DESETINE,            // 34  double - desetinne cislo
    RETEZEC,             // 35  string - retezec znaku ""
    ID,                  // 36  identifikator 
      // chyba 
      //specialni znaky
    END_OF_FILE,          // 38  
    CHBA                  // 39 chyba pri alokacci
  } ID_tokenu;

// struktura samotneho tokenu
//---------------------------

typedef struct
  {
    ID_tokenu typ;
    int cislo;            // pokud je potreba ulozit cele cislo
    double desetin;       // pokud je potreba ulozit desetine cislo
    char* data;           // pokud je potreba ulozit retezec
  } token;


//hlavicka funkce simulujici lexikalni analyzator
token getNextToken();

#endif