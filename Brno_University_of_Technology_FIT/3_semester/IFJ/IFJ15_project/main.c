/* ******************************** main.c ************************************/
/* Soubor:              main.c - Riadiace funkcie interpretu                  */
/* Kodovani:            UTF-8                                                 */
/* Datum:               13. 12. 2015                                          */
/*                                                                            */
/* Predmet:             Formalni jazyky a prekladace (IFJ)                    */
/* Projekt:             Implementace interpretu jazyka IFJ15                  */
/* Varianta zadani:     a/1/I                                                 */
/*                                                                            */
/* Autori, login:       Tereza Kabeláčová            xkabel06                 */
/*                      Adriana Jelenčíková          xjelen10                 */
/*                      Jan Ondruch                  xondru14                 */
/*                      Michal Klhůfek               xklhuf00                 */
/*                      Martin Auxt                  xauxtm00                 */
/* ****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <strings.h>
#include "scaner.h"
#include "garbage.h"
#include "main.h"
#include "parser.h"
#include "interpret.h"

/* funkce pro vypis error codu */
void errorys(int result)

{
   switch (result)
     {

/*1*/ case ERR_CODE_LEX:
         fprintf(stderr, "chyba v programu v ramci lexikalni analyzy \n");
         break;
/*2*/ case ERR_CODE_SYN:
         fprintf(stderr, "chyba v programu v ramci syntakticke analyzy\n");
         break;
/*3*/ case ERR_CODE_DEF:
         fprintf(stderr, "semanticka chyba v programu\n");
         break;
/*4*/ case ERR_CODE_COM:
         fprintf(stderr, "semanticka chyba typove kompantibility\n");
         break;
/*5*/ case ERR_CODE_TYPE:
         fprintf(stderr, "chyba pri odvozovani datoveho typu promenne\n");
         break;
/*6*/ case ERR_CODE_SEM:
         fprintf(stderr, "ostatni semanticke chyby\n");
         break;
/*7*/ case ERR_CODE_NUM:
         fprintf(stderr, "behova chyba pri nacitani ciselne hodnoty ze vstupu\n");
         break;
/*8*/ case ERR_CODE_INIT:
         fprintf(stderr, "behova chyba pri praci s neinicializovanou promenou\n");
         break;
/*9*/ case ERR_CODE_DIV:
         fprintf(stderr, "behova chyba deleni nulou\n");
         break;
/*10*/case ERR_CODE_OTH:
         fprintf(stderr, "ostatni behove chyby\n");
         break;
/*99*/case ERR_CODE_INTER:
         fprintf(stderr, "interni chyba interpretu\n");
         break;
    }
}    


int main(int argc, char** argv)
{

   if (argc == 1)
   {
      return ERR_CODE_INTER;      
   }

   if ((vstup = fopen(argv[1], "r")) == NULL)
   {
      return ERR_CODE_INTER;
   }      
   
   int result; // vysledek programu

   if ((result = parser()) != ERR_CODE_SUCC) {
      
      fclose(vstup);

      errorys(result);

      return result;

   }
   
   if ((result = interpret(&T)) != ERR_CODE_SUCC) {

      errorys(result);

   }

   gFree();             // uvolnime vsechnu alokovanou pamet
   fclose(vstup);       // zavreme input soubor
   return result;
}