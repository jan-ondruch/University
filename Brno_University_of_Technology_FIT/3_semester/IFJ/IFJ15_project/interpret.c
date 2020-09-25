/* ******************************** interpret.c *******************************/
/* Soubor:              interpret.c - Funkce interpretu                       */
/* Kodovani:            UTF-8                                                 */
/* Datum:               13. 12. 2015                                          */
/*                                                                            */
/* Predmet:             Formalni jazyky a prekladace (IFJ)                    */
/* Projekt:             Implementace interpretu jazyka IFJ15                  */
/* Varianta zadani:     a/1/I                                                 */
/*                                                                            */
/* Autori, login:       Tereza Kabeláčová         xkabel06                    */
/*                      Adriana Jelenčíková       xjelen10                    */
/*                      Jan Ondruch               xondru14                    */
/*                      Michal Klhůfek            xklhuf00                    */
/*                      Martin Auxt               xauxtm00                    */
/* ****************************************************************************/

#include <stdio.h>
#include <limits.h>
#include <float.h>

#include "interpret.h"
#include "paska.h"
#include "main.h"
#include "garbage.h"
#include "ial.h"
#include "str.h"

/* hlavni funkce pro interpretacni cast */
int interpret( tTape *T )             
  {
    
    // nastavime zacatek instukcni pasky na zacatek mainu
    while (T->act->insType != STOP_X) {
  
      if (T->act->insType == LAB_X) {
   
        if (!(strcmp(T->act->tOp1.data, "main"))) break;  // nasel main

      }
      T->act = T->act->rptr;
    }

    // pokud vubec nenasel main
    if (T->act->insType == STOP_X) return ERR_CODE_DEF; // check this out later


    while(T->act->insType != STOP_X)              // budeme cyklit do te doby dokud neprijde instrukce STOP_X  !!!!!!!!!!!!!!!! 
     {
        
        // check na endofmain -> zavru program
        if (T->act->insType == LAB_X) {
          if (!(strcmp(T->act->tOp1.data, "endofmain"))) {
            break;
          }
        } 


        ///////////////////////////////////////////////////////////////////////////////////
        ///   ZACATEK PREDELAVANI OPERANDU PRO VYRAZY /////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////
        /// pokud posilam jine instrukce nez pro vyrazy, nemely by si konverzi vsimnout ///
        ///////////////////////////////////////////////////////////////////////////////////

        /* dostali jsme jenom jeden operand, druhy mame v resultu v minule instrukci, musime 
         * vsechno info dostat tak jak ma byt
        */
        if (T->act->tOp1.resultOnTheTape == true || T->act->tOp2.resultOnTheTape == true) {

          /* pri 2*5 + 3*5 mam oba true -> musim brat hodnoty ze 2 predchozich pasek*/
          if (T->act->tOp1.resultOnTheTape == true && T->act->tOp2.resultOnTheTape == true) {

            /* ulozime hodnotu dle typu operandu - vysledku - z predpredchozi instrukce!*/
              if (T->act->lptr->lptr->tResult.typ == CELECISLO) {
                T->act->tOp1.cislo = T->act->lptr->lptr->tResult.cislo;
                T->act->tOp1.typ = CELECISLO;
              }
              else if (T->act->lptr->lptr->tResult.typ == DESETINE) {
                T->act->tOp1.desetin = T->act->lptr->lptr->tResult.desetin;
                T->act->tOp1.typ = DESETINE;
              }
              else if (T->act->lptr->lptr->tResult.typ == RETEZEC) {
                T->act->tOp1.data = T->act->lptr->lptr->tResult.data;
                T->act->tOp1.typ = RETEZEC;
              }
              else {
                return ERR_CODE_INTER;  // nevim jestli se tu dostane vubec
              }

              /* ulozime hodnotu dle typu operandu - vysledku - z predchozi instrukce */
              if (T->act->lptr->tResult.typ == CELECISLO) {
                T->act->tOp2.cislo = T->act->lptr->tResult.cislo;
                T->act->tOp2.typ = CELECISLO;
              } 
              else if (T->act->lptr->tResult.typ == DESETINE) {
                T->act->tOp2.desetin = T->act->lptr->tResult.desetin;
                T->act->tOp2.typ = DESETINE;
              }
              else if (T->act->lptr->tResult.typ == RETEZEC) {
                T->act->tOp2.data = T->act->lptr->tResult.data;
                T->act->tOp2.typ = RETEZEC;
              }
              else {
                return ERR_CODE_INTER;  // nevim jestli se tu dostane vubec
              }

          }

          else {
            /* tOp1 je prazdny -> hodime do nej result */
            if (T->act->tOp1.resultOnTheTape == true) {

              /* ulozime hodnotu dle typu operandu - vysledku */
              if (T->act->lptr->tResult.typ == CELECISLO) {
                T->act->tOp1.cislo = T->act->lptr->tResult.cislo;
                T->act->tOp1.typ = CELECISLO;
              }
              else if (T->act->lptr->tResult.typ == DESETINE) {
                T->act->tOp1.desetin = T->act->lptr->tResult.desetin;
                T->act->tOp1.typ = DESETINE;
              }
              else if (T->act->lptr->tResult.typ == RETEZEC) {
                T->act->tOp1.data = T->act->lptr->tResult.data;
                T->act->tOp1.typ = RETEZEC;
              }
              else {
                return ERR_CODE_INTER;  // nevim jestli se tu dostane vubec
              }
            }

            /* tOp2 je prazdny -> hodime do nej result */
            if (T->act->tOp2.resultOnTheTape == true) {

              /* ulozime hodnotu dle typu operandu - vysledku */
              if (T->act->lptr->tResult.typ == CELECISLO) {
                T->act->tOp2.cislo = T->act->lptr->tResult.cislo;
                T->act->tOp2.typ = CELECISLO;
              } 
              else if (T->act->lptr->tResult.typ == DESETINE) {
                T->act->tOp2.desetin = T->act->lptr->tResult.desetin;
                T->act->tOp2.typ = DESETINE;
              }
              else if (T->act->lptr->tResult.typ == RETEZEC) {
                T->act->tOp2.data = T->act->lptr->tResult.data;
                T->act->tOp2.typ = RETEZEC;
              }
              else {
                return ERR_CODE_INTER;  // nemel by se tu dostat
              }
            }
          }
        }

        ///////////////////////////////////////////////////////////////////////////////////


        /* pokud dostaneme ID -> musime najit hodnotu ve strome */
        if (T->act->tOp1.elemIDptr != NULL) {

          // pokud neni v ID nic -> nebyla definovana hodnota + osetreni proti CIN
          if (T->act->tOp1.elemIDptr->data->isDeclared == false && T->act->insType != READ_X) {
            return ERR_CODE_DEF;
          }

          // pokud neni v ID nic -> nebyla inicializovana hodnota + osetreni proti CIN
          if (T->act->tOp1.elemIDptr->data->isInit == false && T->act->insType != READ_X) {
            return ERR_CODE_INIT;
          }

          /* ulozime hodnotu do operandu dle typu*/
          if (T->act->tOp1.elemIDptr->data->type == KINT || T->act->tOp1.elemIDptr->data->type == CELECISLO) {
            T->act->tOp1.cislo = T->act->tOp1.elemIDptr->data->hodnota.ival;
          }
          else if (T->act->tOp1.elemIDptr->data->type == KDOUBLE || T->act->tOp1.elemIDptr->data->type == DESETINE) {
            T->act->tOp1.desetin = T->act->tOp1.elemIDptr->data->hodnota.dval;
          }
          else if (T->act->tOp1.elemIDptr->data->type == KSTRING || T->act->tOp1.elemIDptr->data->type == RETEZEC) {
            T->act->tOp1.data = T->act->tOp1.elemIDptr->data->hodnota.sval;
          }
          else {
            return ERR_CODE_INTER;  // nevim jestli se tu dostane vubec
          }
        }

        /* pokud dostaneme ID -> musime najit hodnotu ve strome */
        if (T->act->tOp2.elemIDptr != NULL) {

          // pokud neni v ID nic -> nebyla definovana hodnota
          if (T->act->tOp2.elemIDptr->data->isDeclared == false) {
            return ERR_CODE_DEF;
          }

          // pokud neni v ID nic -> nebyla inicializovana hodnota
          if (T->act->tOp2.elemIDptr->data->isInit == false) {
            return ERR_CODE_INIT;
          }

          /* ulozime hodnotu do operandu dle typu*/
          if (T->act->tOp2.elemIDptr->data->type == KINT || T->act->tOp2.elemIDptr->data->type == CELECISLO) {
            T->act->tOp2.cislo = T->act->tOp2.elemIDptr->data->hodnota.ival;
          }
          else if (T->act->tOp2.elemIDptr->data->type == KDOUBLE || T->act->tOp2.elemIDptr->data->type == DESETINE) {
            T->act->tOp2.desetin = T->act->tOp2.elemIDptr->data->hodnota.dval;
          }
          else if (T->act->tOp2.elemIDptr->data->type == KSTRING || T->act->tOp2.elemIDptr->data->type == RETEZEC) {
            T->act->tOp2.data = T->act->tOp2.elemIDptr->data->hodnota.sval;
          }
          else {
            return ERR_CODE_INTER;  // nevim jestli se tu dostane vubec
          } 
        }

        ///////////////////////////////////////////////////////////////////////////////////
        ///   KONEC PREDELAVANI OPERANDU PRO VYRAZY ///////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////
   
   
    
        switch(T->act->insType)                   // rozsekani na case podle hodnot v instType na pasce (napr.  I_READ,I_WRITE,....)
          {
              
// Zakladni konstukce
// ------------------

// +
// ------
            case  ADD_X:
               
              if(T->act->tResult.typ == CELECISLO)                                          // jestli vysledek bude typu INT ()
                {
                  if ( ( T->act->tOp1.typ == CELECISLO ) && ( T->act->tOp2.typ == CELECISLO ) )
                    {
                      T->act->tResult.cislo = T->act->tOp1.cislo + T->act->tOp2.cislo;
                    }  
                  else 
                    { 
                      return (ERR_CODE_COM);
                    }
                }
              else if ( T->act->tResult.typ == DESETINE )                                     // jestli vysledek bude typu DOUBLE
                {                 
                  if ( ( T->act->tOp1.typ == DESETINE ) && ( T->act->tOp2.typ == DESETINE ) )     
                    {
                      T->act->tResult.desetin= T->act->tOp1.desetin + T->act->tOp2.desetin;
                    }
                  else if ( ( T->act->tOp1.typ == DESETINE ) && ( T->act->tOp2.typ == CELECISLO ) )
                    {
                      T->act->tResult.desetin= T->act->tOp1.desetin + T->act->tOp2.cislo;                      
                    }                    
                  else if ( ( T->act->tOp1.typ == CELECISLO ) && ( T->act->tOp2.typ == DESETINE ) )
                    {
                      T->act->tResult.desetin= T->act->tOp1.cislo + T->act->tOp2.desetin;                     
                    }
                  else 
                    { 
                        return (ERR_CODE_INTER);
                    }
                }
              else 
                { 
                    return (ERR_CODE_INTER);
                }           
            break; 
            
// -
// ------            
            case  SUCC_X :
              
              if(T->act->tResult.typ == CELECISLO)                                          // jestli vysledek bude typu INT ()
                {
                  if ( ( T->act->tOp1.typ == CELECISLO ) && ( T->act->tOp2.typ == CELECISLO ) )       
                    {         
                      T->act->tResult.cislo = T->act->tOp1.cislo - T->act->tOp2.cislo;
                    }  
                  else  
                    { 
                      return ERR_CODE_OTH;
                    }
                }
              else if ( T->act->tResult.typ == DESETINE )                                     // jestli vysledek bude typu DOUBLE
                {
                  if ( ( T->act->tOp1.typ == DESETINE ) && ( T->act->tOp2.typ == DESETINE ) )     
                    {
                      T->act->tResult.desetin= T->act->tOp1.desetin - T->act->tOp2.desetin;                   
                    }
                  else if ( ( T->act->tOp1.typ == DESETINE ) && ( T->act->tOp2.typ == CELECISLO ) )
                    {
                      T->act->tResult.desetin= T->act->tOp1.desetin - T->act->tOp2.cislo;           
                    }                    
                  else if ( ( T->act->tOp1.typ == CELECISLO ) && ( T->act->tOp2.typ == DESETINE ) )
                    {
                      T->act->tResult.desetin= T->act->tOp1.cislo - T->act->tOp2.desetin;                    
                    }
                  else 
                    { 
                      return ERR_CODE_INTER;
                    }
                }
              else 
                { 
                  return ERR_CODE_INTER;
                }
            break;               
            
// *
// ------            
            case  MUL_X :

              if(T->act->tResult.typ == CELECISLO)                                          // jestli vysledek bude typu INT ()
                {
                  if ( ( T->act->tOp1.typ == CELECISLO ) && ( T->act->tOp2.typ == CELECISLO ) )
                    {
                      T->act->tResult.cislo = T->act->tOp1.cislo * T->act->tOp2.cislo;
                    }  
                  else  
                    { 
                      return ERR_CODE_COM;
                    }
                }           
              else if ( T->act->tResult.typ == DESETINE )                                     // jestli vysledek bude typu DOUBLE
                {
                  if ( ( T->act->tOp1.typ == DESETINE ) && ( T->act->tOp2.typ == DESETINE ) )     
                    {
                      T->act->tResult.desetin= T->act->tOp1.desetin * T->act->tOp2.desetin;
                    }
                  else if ( ( T->act->tOp1.typ == DESETINE ) && ( T->act->tOp2.typ == CELECISLO ) )
                    {
                      T->act->tResult.desetin= T->act->tOp1.desetin * T->act->tOp2.cislo;                      
                    }                    
                  else if ( ( T->act->tOp1.typ == CELECISLO ) && ( T->act->tOp2.typ == DESETINE ) )
                    {
                      T->act->tResult.desetin= T->act->tOp1.cislo * T->act->tOp2.desetin;                     
                    }
                  else 
                     { 
                      return ERR_CODE_INTER;
                    }
                }
              else 
                { 
                  return ERR_CODE_INTER;
                }
            break;                           
            
// /
// ------            
            case  DIV_X :
                                         
              if(T->act->tResult.typ == DESETINE)
                {                
                  if ( ( T->act->tOp1.typ == CELECISLO ) && ( T->act->tOp2.typ == CELECISLO ) )     
                    {                      
                      if (T->act->tOp2.cislo == 0)                    // osetreni deleni nulou
                        {
                          return ERR_CODE_DIV;
                        }                                              
                      T->act->tResult.desetin= (double)T->act->tOp1.cislo / (double)T->act->tOp2.cislo;             // pretypovani natvrdo
                    }      
                  else if ( ( T->act->tOp1.typ == DESETINE ) && ( T->act->tOp2.typ == CELECISLO ) )
                    {
                      if (T->act->tOp2.cislo == 0)                    // osetreni deleni nulou
                        {
                          return ERR_CODE_DIV;
                        }
                      T->act->tResult.desetin= (double)T->act->tOp1.desetin / (double)T->act->tOp2.cislo;                       
                    } 
                  else if ( ( T->act->tOp1.typ == CELECISLO ) && ( T->act->tOp2.typ == DESETINE ) )
                    {

                      if (T->act->tOp2.desetin == 0)                    // osetreni deleni nulou
                        {
                          return ERR_CODE_DIV;
                        }
                      T->act->tResult.desetin= (double)T->act->tOp1.cislo / (double)T->act->tOp2.desetin; 
                    }                                     
                  else if ( ( T->act->tOp1.typ == DESETINE ) && ( T->act->tOp2.typ == DESETINE ) ) 
                    {

                      if (T->act->tOp2.desetin == 0)                    // osetreni deleni nulou
                        {
                          return ERR_CODE_DIV;
                        }
                      T->act->tResult.desetin= (double)T->act->tOp1.desetin / (double)T->act->tOp2.desetin;                     
                    }                                                                                                                                                      
                  else
                    { 
                      return ERR_CODE_INTER;
                    }
                }
              else 
                { 
                  return ERR_CODE_INTER;
                }           
            break;
            
// <
// ------            
            case  MENSI_X :
                                                     
              if ( ( T->act->tOp1.typ == CELECISLO ) && ( T->act->tOp2.typ == CELECISLO ) )                   // INT  < INT 
                {
                  if ( (T->act->tOp1.cislo) < (T->act->tOp2.cislo) )
                    {
                      T->act->tResult.cislo = 1;                                    
                    }
                  else
                    {
                      T->act->tResult.cislo = 0;                                    
                    }                     
                }
              else if ( ( T->act->tOp1.typ == DESETINE ) && ( T->act->tOp2.typ == CELECISLO ) )              // DOUBLE  < INT
                {
                  if ( (T->act->tOp1.desetin) < (T->act->tOp2.cislo) )
                    {
                      T->act->tResult.cislo = 1;                                    
                    }
                  else
                    {
                      T->act->tResult.cislo = 0;                                    
                    }                                    
                }
              else if ( ( T->act->tOp1.typ == CELECISLO ) && ( T->act->tOp2.typ == DESETINE ) )              //  INT < DOUBLE   
                {
                  if ( (T->act->tOp1.cislo) < (T->act->tOp2.desetin) )
                    {
                      T->act->tResult.cislo = 1;                                    
                    }
                  else
                    {
                      T->act->tResult.cislo = 0;                                    
                    }                                    
                }
              else if ( ( T->act->tOp1.typ == DESETINE ) && ( T->act->tOp2.typ == DESETINE ) )              // DOUBLE < DOUBLE
                {
                  if ( (T->act->tOp1.desetin) < (T->act->tOp2.desetin) )
                    {
                      T->act->tResult.cislo = 1;                                    
                    }
                  else
                    {
                      T->act->tResult.cislo = 0;                                    
                    }                                      
                }
              else if ( ( T->act->tOp1.typ == RETEZEC ) && ( T->act->tOp2.typ == RETEZEC ) )             // cast na porovnani retezcu
                {    
                //  if ( porovnani_retezcu((&(T->act->tOp1.data), &(T->act->tOp2.data))) )      
                  if ( ( strcmp (T->act->tOp1.data, T->act->tOp2.data)) == -1 )       
                    {
                      T->act->tResult.cislo = 1;                                    
                    }
                  else
                    {
                      T->act->tResult.cislo = 0;                                    
                    }
                }                
              else 
                { 
                  return ERR_CODE_INTER;
                }           
            break;
            
// >
// ------            
            case  VETSI_X :
                                                     
              if ( ( T->act->tOp1.typ == CELECISLO ) && ( T->act->tOp2.typ == CELECISLO ) )                   // INT  > INT 
                {
                  if ( (T->act->tOp1.cislo) > (T->act->tOp2.cislo) )
                    {
                      T->act->tResult.cislo = 1;                                    
                    }
                  else
                    {
                      T->act->tResult.cislo = 0;                                    
                    }                   
                }
              else if ( ( T->act->tOp1.typ == DESETINE ) && ( T->act->tOp2.typ == CELECISLO ) )              // DOUBLE  > INT
                {
                  if ( (T->act->tOp1.desetin) > (T->act->tOp2.cislo) )
                    {
                      T->act->tResult.cislo = 1;                                    
                    }
                  else
                    {
                      T->act->tResult.cislo = 0;                                    
                    }                                     
                }
              else if ( ( T->act->tOp1.typ == CELECISLO ) && ( T->act->tOp2.typ == DESETINE ) )              //  INT > DOUBLE   
                {
                  if ( (T->act->tOp1.cislo) > (T->act->tOp2.desetin) )
                    {
                      T->act->tResult.cislo = 1;                                    
                    }
                  else
                    {
                      T->act->tResult.cislo = 0;                                    
                    }                                    
                }
              else if ( ( T->act->tOp1.typ == DESETINE ) && ( T->act->tOp2.typ == DESETINE ) )              // DOUBLE > DOUBLE
                {
                  if ( (T->act->tOp1.desetin) > (T->act->tOp2.desetin) )
                    {
                      T->act->tResult.cislo = 1;                                    
                    }
                  else
                    {
                      T->act->tResult.cislo = 0;                                    
                    }                                     
                }
              else if ( ( T->act->tOp1.typ == RETEZEC ) && ( T->act->tOp2.typ == RETEZEC ) )             // cast na porovnani retezcu
                {                
                  if ( (strcmp(T->act->tOp1.data, T->act->tOp2.data)) == 1 )
//                if ( ( strlen(T->act->tOp1.typ) > strlen(T->act->tOp2.typ) ) == 0 )                    // lexikograficka kontrola retezcu ???
                    {
                      T->act->tResult.cislo = 1;                                    
                    }
                  else
                    {
                      T->act->tResult.cislo = 0;                                    
                    }
                }                
              else 
                { 
                  return ERR_CODE_INTER;
                }                                      
            break;

// <=
// ------            
            case  MENROVNO_X :

              if ( ( T->act->tOp1.typ == CELECISLO ) && ( T->act->tOp2.typ == CELECISLO ) )                   // INT  <= INT 
                {
                  if ( (T->act->tOp1.cislo) <= (T->act->tOp2.cislo) )
                    {
                      T->act->tResult.cislo = 1;                                    
                    }
                  else
                    {
                      T->act->tResult.cislo = 0;                                    
                    }                     
                }
              else if ( ( T->act->tOp1.typ == DESETINE ) && ( T->act->tOp2.typ == CELECISLO ) )              // DOUBLE  <= INT
                {
                  if ( (T->act->tOp1.desetin) <= (T->act->tOp2.cislo) )
                    {
                      T->act->tResult.cislo = 1;                                    
                    }
                  else
                    {
                      T->act->tResult.cislo = 0;                                    
                    }                                    
                }
              else if ( ( T->act->tOp1.typ == CELECISLO ) && ( T->act->tOp2.typ == DESETINE ) )              //  INT <= DOUBLE   
                {
                  if ( (T->act->tOp1.cislo) <= (T->act->tOp2.desetin) )
                    {
                      T->act->tResult.cislo = 1;                                    
                    }
                  else
                    {
                      T->act->tResult.cislo = 0;                                    
                    }                                      
                }
              else if ( ( T->act->tOp1.typ == DESETINE ) && ( T->act->tOp2.typ == DESETINE ) )              // DOUBLE <= DOUBLE
                {
                  if ( (T->act->tOp1.desetin) <= (T->act->tOp2.desetin) )
                    {
                      T->act->tResult.cislo = 1;                                    
                    }
                  else
                    {
                      T->act->tResult.cislo = 0;                                    
                    }                                     
                }
              else if ( ( T->act->tOp1.typ == RETEZEC ) && ( T->act->tOp2.typ == RETEZEC ) )             // cast na porovnani retezcu
                {  
                  if ((( strcmp (T->act->tOp1.data, T->act->tOp2.data)) == -1 ) || (( strcmp (T->act->tOp1.data, T->act->tOp2.data)) == 0))
                    {
                      T->act->tResult.cislo = 1;                                    
                    }
                  else
                    {
                      T->act->tResult.cislo = 0;                                    
                    }
                }                
              else 
                { 
                  return ERR_CODE_INTER;
                }                                                                         
            break;

// >=
// ------           
            case  VETROVNO_X :

              if ( ( T->act->tOp1.typ == CELECISLO ) && ( T->act->tOp2.typ == CELECISLO ) )                   // INT  >= INT 
                {
                  if ( (T->act->tOp1.cislo) >= (T->act->tOp2.cislo) )
                    {
                      T->act->tResult.cislo = 1;                                    
                    }
                  else
                    {
                      T->act->tResult.cislo = 0;                                    
                    }                     
                }
              else if ( ( T->act->tOp1.typ == DESETINE ) && ( T->act->tOp2.typ == CELECISLO ) )              // DOUBLE  >= INT
                {
                  if ( (T->act->tOp1.desetin) >= (T->act->tOp2.cislo) )
                    {
                      T->act->tResult.cislo = 1;                                    
                    }
                  else
                    {
                      T->act->tResult.cislo = 0;                                    
                    }                                       
                }
              else if ( ( T->act->tOp1.typ == CELECISLO ) && ( T->act->tOp2.typ == DESETINE ) )              //  INT >= DOUBLE   
                {
                  if ( (T->act->tOp1.cislo) >= (T->act->tOp2.desetin) )
                    {
                      T->act->tResult.cislo = 1;                                    
                    }
                  else
                    {
                      T->act->tResult.cislo = 0;                                    
                    }                                     
                }
              else if ( ( T->act->tOp1.typ == DESETINE ) && ( T->act->tOp2.typ == DESETINE ) )              // DOUBLE >= DOUBLE
                {
                  if ( (T->act->tOp1.desetin) >= (T->act->tOp2.desetin) )
                    {
                      T->act->tResult.cislo = 1;                                    
                    }
                  else
                    {
                      T->act->tResult.cislo = 0;                                    
                    }                                     
                }
              else if ( ( T->act->tOp1.typ == RETEZEC ) && ( T->act->tOp2.typ == RETEZEC ) )             // cast na porovnani retezcu
                { 
                  if ((( strcmp (T->act->tOp1.data, T->act->tOp2.data)) == 1 ) || (( strcmp (T->act->tOp1.data, T->act->tOp2.data)) == 0))
                    {
                      T->act->tResult.cislo = 1;                                    
                    }
                  else
                    {
                      T->act->tResult.cislo = 0;                                    
                    }
                }                         
            break;

// !=
// ------
            case  NEROVNA_X :                       
                                                     
              if ( ( T->act->tOp1.typ == CELECISLO ) && ( T->act->tOp2.typ == CELECISLO ) )                   // INT  != INT 
                {
                  if ( (T->act->tOp1.cislo) != (T->act->tOp2.cislo) )
                    {
                      T->act->tResult.cislo = 1;                                    
                    }
                  else
                    {
                      T->act->tResult.cislo = 0;                                    
                    }                    
                }
              else if ( ( T->act->tOp1.typ == DESETINE ) && ( T->act->tOp2.typ == CELECISLO ) )              // DOUBLE  != INT
                {
                  if ( (T->act->tOp1.desetin) != (T->act->tOp2.cislo) )
                    {
                      T->act->tResult.cislo = 1;                                    
                    }
                  else
                    {
                      T->act->tResult.cislo = 0;                                    
                    }                                     
                }
              else if ( ( T->act->tOp1.typ == CELECISLO ) && ( T->act->tOp2.typ == DESETINE ) )              //  INT != DOUBLE   
                {
                  if ( (T->act->tOp1.cislo) != (T->act->tOp2.desetin) )
                    {
                      T->act->tResult.cislo = 1;                                    
                    }
                  else
                    {
                      T->act->tResult.cislo = 0;                                    
                    }                                     
                }
              else if ( ( T->act->tOp1.typ == DESETINE ) && ( T->act->tOp2.typ == DESETINE ) )              // DOUBLE != DOUBLE
                {
                  if ( (T->act->tOp1.desetin) != (T->act->tOp2.desetin) )
                    {
                      T->act->tResult.cislo = 1;                                    
                    }
                  else
                    {
                      T->act->tResult.cislo = 0;                                    
                    }                                    
                }
              else if ( ( T->act->tOp1.typ == RETEZEC ) && ( T->act->tOp2.typ == RETEZEC ) )             // cast na porovnani retezcu
                {
                  if ( (strcmp (T->act->tOp1.data,T->act->tOp2.data)) != 0 )
                    {
                      T->act->tResult.cislo = 1;                                    
                    }
                  else
                    {
                      T->act->tResult.cislo = 0;                                    
                    }
                }                  
            break;

// ==
// ------
            case  ROVNA_X :
  
              if ( ( T->act->tOp1.typ == CELECISLO ) && ( T->act->tOp2.typ == CELECISLO ) )                   // INT  == INT 
                {
                  if ( (T->act->tOp1.cislo) == (T->act->tOp2.cislo) )
                    {
                      T->act->tResult.cislo = 1;                                    
                    }
                  else
                    {
                      T->act->tResult.cislo = 0;                                    
                    }                     
                }
              else if ( ( T->act->tOp1.typ == DESETINE ) && ( T->act->tOp2.typ == CELECISLO ) )              // DOUBLE  == INT
                {
                  if ( (T->act->tOp1.desetin) == (T->act->tOp2.cislo) )
                    {
                      T->act->tResult.cislo = 1;                                    
                    }
                  else
                    {
                      T->act->tResult.cislo = 0;                                    
                    }                                      
                }
              else if ( ( T->act->tOp1.typ == CELECISLO ) && ( T->act->tOp2.typ == DESETINE ) )              //  INT == DOUBLE   
                {
                  if ( (T->act->tOp1.cislo) == (T->act->tOp2.desetin) )
                    {
                      T->act->tResult.cislo = 1;                                    
                    }
                  else
                    {
                      T->act->tResult.cislo = 0;                                    
                    }                                      
                }
              else if ( ( T->act->tOp1.typ == DESETINE ) && ( T->act->tOp2.typ == DESETINE ) )              // DOUBLE == DOUBLE
                {
                  if ( (T->act->tOp1.desetin) == (T->act->tOp2.desetin) )
                    {
                      T->act->tResult.cislo = 1;                                    
                    }
                  else
                    {
                      T->act->tResult.cislo = 0;                                    
                    }                                     
                }
              else if ( ( T->act->tOp1.typ == RETEZEC ) && ( T->act->tOp2.typ == RETEZEC ) )             // cast na porovnani retezcu
                {                
                  if ( (strcmp (T->act->tOp1.data,T->act->tOp2.data)) == 0 ) 
                    {
                      T->act->tResult.cislo = 1;                                    
                    }
                  else
                    {
                      T->act->tResult.cislo = 0;                                    
                    }
                }                                                                                
            break;

// -------------------------------------------------------------------------------------------------------------------------------------------

// CIN 
// ----------
            case  READ_X :
              // podle typu promenne, do ktere zapisuju urcim, co budu nacitat
              // pokud nactu treba do stringu int -> sestrelim to chybou

              // READ INTEGER
              if (T->act->tOp1.elemIDptr->data->type == KINT)   // ID = INTEGER
              { 
                int pomocna_int = 0;
                char nactu_int; 
                nactu_int = getchar();
                
                if (nactu_int == '\n')                // pokud je zadano "\n(enter) je to chyba 
                  { 
                    return ERR_CODE_NUM;
                  }                
                while ((nactu_int <= '9' ) && (nactu_int >= '0'))
                  {
                    pomocna_int = pomocna_int*10 + nactu_int - '0'; 
                    if (pomocna_int < 0)
                      {
                        return ERR_CODE_NUM;
                      }
                    if (pomocna_int > INT_MAX)  
                      {
                        return ERR_CODE_NUM;
                      }
                    nactu_int = getchar();
                  }
                if (nactu_int != '\n')                                       // zajisteni toho, ze ni cineho nez cisla a enter neprijde
                  {
                    return ERR_CODE_NUM;
                  }
                // vysledek zapisu do node do ID
                T->act->tOp1.elemIDptr->data->hodnota.ival = pomocna_int;
                T->act->tOp1.elemIDptr->data->isInit = true;  
              break;
              }

              // READ DOUBLE
              else if (T->act->tOp1.elemIDptr->data->type == KDOUBLE)   // ID = DOUBLE
              {    
                char nactu_double; 
                int pomocna_double1 = 0;
                double pomocna_double = 0;
                double exponentys = 0;
                nactu_double = getchar();
                while ((nactu_double <= '9' ) && (nactu_double >= '0'))
                  {      
     
                    pomocna_double1 = (pomocna_double1*10) + nactu_double - '0' ; 
     
                    nactu_double = getchar();
                  }
                if (nactu_double == '\n')  
                  break;
                pomocna_double = pomocna_double1;  
                if ( (nactu_double != '.') && (nactu_double != 'e') && (nactu_double != 'E') ) // pokud to co prislo neni . e E
                  {
                    return ERR_CODE_NUM;
                  }
                else if (((nactu_double == 'e' ) || (nactu_double == 'E')))   // pracuji s casti exponentu
                  {
                    nactu_double = getchar();
                    if (nactu_double == '+')
                      {
                        nactu_double = getchar();
                        if (!((nactu_double <= '9' ) && (nactu_double >= '0')))                                 // musi za tim byt cislo
                          {
                            return ERR_CODE_NUM;
                          }
                        if (((nactu_double <= '9' ) && (nactu_double >= '0')))
                          {
                            while ((nactu_double <= '9' ) && (nactu_double >= '0'))
                              {
                                exponentys = ((exponentys * 10) + nactu_double) - '0';       // vypocet exponentu *10 + prevod na int
                                nactu_double = getchar();
 
                              }           
                            int double_exp = pomocna_double;                                 // pomocne cislo
                            for (int x = exponentys; x > 1; x--) 
                              {
                                pomocna_double = pomocna_double * double_exp;                   // vypocet cisla s exponentem
        
                              }   
                            if (pomocna_double >= DBL_MAX_EXP)            
                              {
                                return ERR_CODE_NUM;
                              }
                          }
                      }
                    else if (nactu_double == '-')          // dobrovolny znak - 
                      {
                        nactu_double = getchar();
                        if (!((nactu_double <= '9' ) && (nactu_double >= '0')))                                 // musi za tim byt cislo
                          {
                            return ERR_CODE_NUM;
                          } 
                        if (((nactu_double <= '9' ) && (nactu_double >= '0')))
                          {
                            while ((nactu_double <= '9' ) && (nactu_double >= '0'))
                              {
                                exponentys = ((exponentys * 10) + nactu_double) - '0';       // vypocet exponentu *10 + prevod na int
                                nactu_double = getchar();  
                              }     
                            int double_exp = pomocna_double;                                 // pomocne cislo
                            for (int x = exponentys; x > 1; x--) 
                              {
                                pomocna_double = pomocna_double * double_exp;                   // vypocet cisla s exponentem         
                              }             
                            pomocna_double = 1/pomocna_double;
                            if (pomocna_double >= 10)            
                              {
                                return ERR_CODE_NUM;
                              }
                          }
                      } 
                    else if ((nactu_double <= '9' ) && (nactu_double >= '0'))
                      {
                        if ((nactu_double <= '9' ) && (nactu_double >= '0'))
                          {
                            while ((nactu_double <= '9' ) && (nactu_double >= '0'))
                              {
                                exponentys = ((exponentys * 10) + nactu_double) - '0';       // vypocet exponentu *10 + prevod na int
                                nactu_double = getchar();
                              }
                            int double_exp = pomocna_double;                                 // pomocne cislo
                            for (int x = exponentys; x > 1; x--) 
                              {
                                pomocna_double = pomocna_double * double_exp;                   // vypocet cisla s exponentem         
                              }                               
                          }                    
                      }  
                    else if ( !((nactu_double <= '9' ) && (nactu_double <= '0')) || !(nactu_double == '+') || !(nactu_double == '-') )    
                      {
                        return ERR_CODE_NUM;
                      }  
                  }   
                else if (nactu_double == '.')                                                             
                  {
                    double krok = 0.1;
                    int exponentys = 0;
                    nactu_double = getchar();
                    if (!((nactu_double <= '9' ) && (nactu_double >= '0')))                                 // musi za tim byt cislo
                      {
                        return ERR_CODE_NUM;
                      } 
                    if (((nactu_double <= '9' ) && (nactu_double >= '0')))
                      {
                        while ((nactu_double <= '9' ) && (nactu_double >= '0'))
                          {
                            pomocna_double = pomocna_double + ( nactu_double - '0' ) * krok;
                            krok = krok * 0.1;
                            nactu_double = getchar();
                          }
                      }
                    if ( ( ( nactu_double == 'e' ) || ( nactu_double == 'E') ) )
                      {
                       nactu_double = getchar();
                        if (nactu_double == '+')
                          {
                            nactu_double = getchar();
                            if (!((nactu_double <= '9' ) && (nactu_double >= '0')))                                 // musi za tim byt cislo
                              {                
                                return ERR_CODE_NUM;
                              }    
                            if (((nactu_double <= '9' ) && (nactu_double >= '0')))
                              {
                                while ((nactu_double <= '9' ) && (nactu_double >= '0'))
                                  {
                                    exponentys = ((exponentys * 10) + (nactu_double - '0'));       // vypocet exponentu *10 + prevod na int
                                    nactu_double = getchar();
                                  }         
                                double double_exp = pomocna_double;                                 // pomocne cislo    
                                for (int x = exponentys; x > 1; x--)     
                                  {
                                    pomocna_double = pomocna_double * double_exp;                   // vypocet cisla s exponentem
                                  }                
                              }                                                                                              
                          }              
                        else if (nactu_double == '-')          // dobrovolny znak - 
                          {
                            nactu_double = getchar();
                            if (!((nactu_double <= '9' ) && (nactu_double >= '0')))                                 // musi za tim byt cislo
                              {
                                return ERR_CODE_NUM;
                              } 
                            if (((nactu_double <= '9' ) && (nactu_double >= '0')))
                              {                    
                                while ((nactu_double <= '9' ) && (nactu_double >= '0'))
                                  {
                                    exponentys = ((exponentys * 10) + nactu_double) - '0';       // vypocet exponentu *10 + prevod na int
                                    nactu_double = getchar();
                                  }     
                                double double_exp = pomocna_double;                                 // pomocne cislo
                                for (int x = exponentys; x > 1; x--) 
                                  {
                                    pomocna_double = pomocna_double * double_exp;                   // vypocet cisla s exponentem         
                                  }             
                                pomocna_double = 1/pomocna_double;
                              }             
                          }
                        else if ((nactu_double <= '9' ) && (nactu_double >= '0'))  
                          {
                            if ((nactu_double <= '9' ) && (nactu_double >= '0'))
                              {
                                while ((nactu_double <= '9' ) && (nactu_double >= '0'))
                                  {
                                    exponentys = ((exponentys * 10) + nactu_double) - '0';       // vypocet exponentu *10 + prevod na int
                                    nactu_double = getchar();
                                  }
                                double double_exp = pomocna_double;                                 // pomocne cislo
                                for (int x = exponentys; x > 1; x--) 
                                  {
                                    pomocna_double = pomocna_double * double_exp;                   // vypocet cisla s exponentem         
                                  }                               
                              }                    
                          }
                        else if ( !((nactu_double <= '9' ) && (nactu_double <= '0')) || !(nactu_double == '+') || !(nactu_double == '-') )    
                          {
                            return ERR_CODE_NUM;
                          }                               
                      }      
                    }             
                T->act->tOp1.elemIDptr->data->hodnota.dval = pomocna_double;
                T->act->tOp1.elemIDptr->data->isInit = true;       
                                               
                break;
              }


// ----------------------------------------------------------------------------------------------


              // READ STRING
              else if (T->act->tOp1.elemIDptr->data->type == KSTRING)   // ID = STRING
                {
  
                  int alokace = 0;
                  char *vysledek;
                  char nactu;
                  nactu = getchar();
                  
                  if (nactu == '\n')                // pokud je zadano "\n(enter) je to chyba 
                    { 
                      return ERR_CODE_NUM;
                    }
                  else
                    {
                      vysledek=(char *)gMalloc(sizeof(char));
                      if(vysledek == NULL) 
                        { 
                           return (ERR_CODE_INTER);
                        }     
                      int poprve = 0;
                      while ((nactu != '"') && (nactu != '\n') && (nactu != EOF))
                        {
                          if (nactu == '\\')                                            // specialni znaky
                            {
                              nactu = getchar();    
                              if (nactu == '"')                                         // \"
                                {                                                
                                  if (poprve == 0)
                                    {
                                      alokace = 3;                                      
                                      *vysledek = nactu + '\0';                         // ulozim specialni znak
                                      poprve = 1;
                                      nactu = getchar();  
                                    } 
                                  else
                                    {
                                      vysledek=(char *)gRealloc(vysledek, alokace * sizeof(char));
                                      if(vysledek == NULL) 
                                        { 
                                          return (ERR_CODE_INTER);
                                        }  
                                      vysledek[alokace-2] = nactu;  
                                      vysledek[alokace-1] = '\0'; 
                                      alokace++;  
                                      nactu = getchar();                                                            
                                    }   
                                }
                              else if (nactu == '\\')                                 // \'
                                {
                                  if (poprve == 0)
                                    {
                                      alokace = 3; 
                                      *vysledek = nactu + '\0';
                                      poprve = 1;
                                      nactu = getchar();  
                                    } 
                                  else
                                    {
                                      vysledek=(char *)gRealloc(vysledek, alokace * sizeof(char));
                                      if(vysledek == NULL) 
                                        { 
                                          return (ERR_CODE_INTER);
                                        } 
                                      vysledek[alokace-2] = nactu;  
                                      vysledek[alokace-1] = '\0'; 
                                      alokace++;  
                                      nactu = getchar(); 
                                    }  
                                }
                              else if (nactu == 'n')                              // \n
                                {
                                  if (poprve == 0)
                                    {
                                      alokace = 3; 
                                      *vysledek = '\n' + '\0';
                                      poprve = 1;
                                      nactu = getchar(); 
                                    }   
                                  else
                                    {
                                      vysledek=(char *)gRealloc(vysledek, alokace * sizeof(char));
                                      if(vysledek == NULL) 
                                        { 
                                          return (ERR_CODE_INTER);
                                        }
                                      vysledek[alokace-2] = '\n';  
                                      vysledek[alokace-1] = '\0'; 
                                      alokace++;  
                                      nactu = getchar(); 
                                    }
                                }  
                              else if (nactu == 't')                  
                                 {
                                   if (poprve == 0)
                                     {
                                       alokace = 3; 
                                        *vysledek = ' ' + '\0';
                                        poprve = 1;
                                      nactu = getchar(); 
                                     }  
                                   else
                                     {
                                       vysledek=(char *)gRealloc(vysledek, alokace * sizeof(char));                                             
                                       if(vysledek == NULL) 
                                         { 
                                           return (ERR_CODE_INTER);
                                         }
                                       vysledek[alokace-2] = '	';
                                       vysledek[alokace-1] = '\0'; 
                                       alokace++;  
                                       nactu = getchar(); 
                                     }
                                 }                          
                               else if (nactu == 'x')
                                 {                                   
                                   char *hexa;
                                   hexa=(char *)gMalloc(sizeof(char) * 2);
                                   nactu = getchar(); 
                                   int zaplatys = 0;// pro kontrolu jestli se nacetl znak \x00 
                                   if (nactu == '0')   // !!!!   // kontrola naceteni prvni nuly
   				                            zaplatys = 666;   // !!!!                                    
                                   if ((( nactu>='0' ) && ( nactu<='9' )) || (( nactu>='A' ) && ( nactu<='F' )) || (( nactu>='a' ) && ( nactu<='f' )))
                                     {   
                                       if (nactu == '0')
                                       zaplatys = 0;                                      
                                       hexa[0] = nactu;      // prvni hodnotu si ulozim do pole na pozici 1
                                       nactu = getchar();
                                       if ( ( nactu == '0' ) && ( zaplatys == 666 ) )  // kontola, zda se nacetla i druha nula zaroven s prvvni                       
                                         zaplatys = 696;   
                                       if ( ((( nactu>='0' ) && ( nactu<='9' )) || (( nactu>='A' ) && ( nactu<='F' )) || (( nactu>='a' ) && ( nactu<='f' ))) && (zaplatys != 696) )
                                         { 
                                           if ( (zaplatys == 0) && (nactu == '0') )                                            
                                             {
                                                // !!!!!!!!!!!! SPECIALNI ZNAK \X00 
                                             }
                                           else
                                             {                                          
                                               hexa[1] = nactu;      // druhou hodnotu si ulozim do pole na hodnotu 2
                                               char *str = hexa;     
                                               char *pEnd;
                                               int r;
                                               r = strtol(str, &pEnd, 16);     // prevedu string na znak, 16 pro prevod z hexa
                                               if (poprve == 0)
                                                 {
                                                  alokace = 3; 
                                                *vysledek = r + '\0';
                                                poprve = 1;
                                              nactu = getchar(); 
                                             }
                                           else
                                             {
                                               vysledek=(char *)gRealloc(vysledek, alokace * sizeof(char));
                                              if(vysledek == NULL) 
                                                  { 
                                                    return (ERR_CODE_INTER);
                                                  }  
                                               vysledek[alokace-2] = r;  
                                                vysledek[alokace-1] = '\0'; 
                                                alokace++;  
                                                nactu = getchar(); 
                                             }  
                                            }                                
                                          }
                                        else      // stavy kdy uzivatel zada neco jineho nez je povoleno \dxx - xx jsou mimo s
                                          {
                                            if (poprve == 0)
                                              {
                                                alokace = 3;           
                                                *vysledek = '\\' + '\0';
                                                poprve = 1;
                                                vysledek=(char *)gRealloc(vysledek, alokace * sizeof(char));
                                                if(vysledek == NULL) 
                                                  { 
                                                    return (ERR_CODE_INTER);
                                                  }  
                                                vysledek[alokace-2] = 'x';  
                                                vysledek[alokace-1] = '\0'; 
                                                alokace++;      
                                                vysledek=(char *)gRealloc(vysledek, alokace * sizeof(char));
                                                if(vysledek == NULL) 
                                                  { 
                                                    return (ERR_CODE_INTER);
                                                  }                                        
                                                vysledek[alokace-2] = hexa[0];  
                                                vysledek[alokace-1] = '\0'; 
                                                alokace++;                                          
                                                vysledek=(char *)gRealloc(vysledek, alokace * sizeof(char));
                                                if(vysledek == NULL) 
                                                  { 
                                                    return (ERR_CODE_INTER);
                                                  } 
                                                vysledek[alokace-2] = nactu;  
                                                vysledek[alokace-1] = '\0'; 
                                                alokace++;        
                                                nactu = getchar(); 
                                              }
                                            else
                                              {          
                                                vysledek=(char *)gRealloc(vysledek, alokace * sizeof(char));
                                                if(vysledek == NULL) 
                                                  { 
                                                    return (ERR_CODE_INTER);
                                                  } 
                                                vysledek[alokace-2] = '\\';  
                                                vysledek[alokace-1] = '\0'; 
                                                alokace++;      
                                                vysledek=(char *)gRealloc(vysledek, alokace * sizeof(char));
                                                if(vysledek == NULL) 
                                                  { 
                                                    return (ERR_CODE_INTER);
                                                  }              
                                                vysledek[alokace-2] = 'x';  
                                                vysledek[alokace-1] = '\0'; 
                                                alokace++;                                       
                                                vysledek=(char *)gRealloc(vysledek, alokace * sizeof(char));
                                                if(vysledek == NULL) 
                                                  { 
                                                    return (ERR_CODE_INTER);
                                                  }                                   
                                                vysledek[alokace-2] = hexa[0];  
                                                vysledek[alokace-1] = '\0'; 
                                                alokace++;                                          
                                                vysledek=(char *)gRealloc(vysledek, alokace * sizeof(char));
                                                if(vysledek == NULL) 
                                                  { 
                                                    return (ERR_CODE_INTER);
                                                  } 
                                                vysledek[alokace-2] = nactu;  
                                                vysledek[alokace-1] = '\0'; 
                                                alokace++;        
                                                nactu = getchar(); 
                                             }  
                                          }                                                   
                                      }                     
                                    else
                                      {
                                        if (poprve == 0)
                                          {          
                                            alokace = 3; 
                                            *vysledek = '\\' + '\0';
                                            poprve = 1;
                                            vysledek=(char *)gRealloc(vysledek, alokace * sizeof(char));
                                            if(vysledek == NULL) 
                                              { 
                                                return (ERR_CODE_INTER);
                                               } 
                                            vysledek[alokace-2] = 'x';  
                                            vysledek[alokace-1] = '\0'; 
                                            alokace++;      
                                            vysledek=(char *)gRealloc(vysledek, alokace * sizeof(char));
                                            if(vysledek == NULL) 
                                              { 
                                                 return (ERR_CODE_INTER);
                                              }                                       
                                            vysledek[alokace-2] = nactu;  
                                            vysledek[alokace-1] = '\0'; 
                                            alokace++;                  
                                            nactu = getchar(); 
                                          }
                                        else
                                          {
                                            vysledek=(char *)gRealloc(vysledek, alokace * sizeof(char));
                                            if(vysledek == NULL) 
                                              { 
                                                  return (ERR_CODE_INTER);
                                               } 
                                            vysledek[alokace-2] = '\\';  
                                            vysledek[alokace-1] = '\0'; 
                                            alokace++;      
                                            vysledek=(char *)gRealloc(vysledek, alokace * sizeof(char));
                                            if(vysledek == NULL) 
                                              { 
                                                 return (ERR_CODE_INTER);
                                              } 
                                            vysledek[alokace-2] = 'x';  
                                            vysledek[alokace-1] = '\0'; 
                                            alokace++;                   
                                            vysledek=(char *)gRealloc(vysledek, alokace * sizeof(char));
                                            if(vysledek == NULL) 
                                              { 
                                                return (ERR_CODE_INTER);
                                              }                                       
                                            vysledek[alokace-2] = nactu;  
                                            vysledek[alokace-1] = '\0'; 
                                            alokace++;                  
                                            nactu = getchar(); 
                                          }                                                                
                                      }                                         
                                  } 
                                else  
                                  { 
                                     return (ERR_CODE_INTER);
                                  }                                             
                              }
                            else  
                              {   
                                if (poprve == 0)
                                  {
                                    alokace = 3; 
                                    *vysledek = nactu + '\0';
                                    poprve = 1;
                                    nactu = getchar(); 
                                  } 
                                else
                                  {     
                                    vysledek=(char *)gRealloc(vysledek, alokace * sizeof(char));
                                    if(vysledek == NULL) 
                                     { 
                                        return (ERR_CODE_INTER);
                                      }                       
                                    vysledek[alokace-2] = nactu;  
                                  vysledek[alokace-1] = '\0'; 
                                  alokace++;  
                                  nactu = getchar();                      
                                } 
                            }
                          }
                        if (nactu == '\n')
                          { 

                          }   
                      }             
                
                T->act->tOp1.elemIDptr->data->hodnota.sval = (char *)gMalloc(sizeof(vysledek));
                strcpy(T->act->tOp1.elemIDptr->data->hodnota.sval,vysledek);
                T->act->tOp1.elemIDptr->data->isInit = true;
                break;
              }

              // typ je nespecifikovan || nejaky jiny
              else {
                return (ERR_CODE_INTER);
              }


// PRIRAD 
// ----
            case  PRIRAD_X :
            
              if ( T->act->tResult.typ == KINT || T->act->tResult.typ == CELECISLO) // i celecislo kvuli auto
              {
                  if ( T->act->lptr->tResult.typ == CELECISLO )
                  {           
                      if (T->act->lptr->tResult.elemIDptr != NULL && !(T->act->lptr->tResult.cislo)) {
                        T->act->tResult.elemIDptr->data->hodnota.ival = T->act->lptr->tResult.elemIDptr->data->hodnota.ival;
                      } 
                      else {
                        T->act->tResult.elemIDptr->data->hodnota.ival = T->act->lptr->tResult.cislo;
                      }
                      // pokud je zaporne -> err
                      if (T->act->tResult.elemIDptr->data->hodnota.ival < 0) {
                        return ERR_CODE_OTH;
                      }
                      if (T->act->lptr->tResult.elemIDptr != NULL) {
                        // promenna nebyla inicializova
                        if (T->act->lptr->tResult.elemIDptr->data->isInit == false) {
                          return ERR_CODE_DEF;
                       }
                      }
                      T->act->tResult.elemIDptr->data->isInit = true; // uz je inicializovana
                  }
                  else if ( T->act->lptr->tResult.typ == DESETINE )
                  {       
                      if (T->act->lptr->tResult.elemIDptr != NULL && !(T->act->lptr->tResult.desetin)) {
                        T->act->tResult.elemIDptr->data->hodnota.ival = (int)T->act->lptr->tResult.elemIDptr->data->hodnota.dval;
                      } 
                      else {
                        T->act->tResult.elemIDptr->data->hodnota.ival = (int)T->act->lptr->tResult.desetin;
                      }
                      // pokud je zaporne -> err
                      if (T->act->tResult.elemIDptr->data->hodnota.dval < 0) {
                        return ERR_CODE_OTH;
                      }
                      if (T->act->lptr->tResult.elemIDptr != NULL) {
                        // promenna nebyla inicializova
                        if (T->act->lptr->tResult.elemIDptr->data->isInit == false) {
                          return ERR_CODE_DEF;
                       }
                      }
                      T->act->tResult.elemIDptr->data->type = CELECISLO; // type uz neni CISLO, ale DESETINE
                      T->act->tResult.elemIDptr->data->isInit = true; // uz je inicializovana       
                  }
                  else {
                    return ERR_CODE_COM;  
                  }
              }
              else if ( T->act->tResult.typ == KDOUBLE || T->act->tResult.typ == DESETINE)  
              { 
                  if ( T->act->lptr->tResult.typ == CELECISLO )
                  {
                      if (T->act->lptr->tResult.elemIDptr != NULL && !(T->act->lptr->tResult.cislo)) {
                        T->act->tResult.elemIDptr->data->hodnota.dval = (double)T->act->lptr->tResult.elemIDptr->data->hodnota.ival;
                      } 
                      else {
                        T->act->tResult.elemIDptr->data->hodnota.dval = (double)T->act->lptr->tResult.cislo;
                      }
                      // pokud je zaporne -> err
                      if (T->act->tResult.elemIDptr->data->hodnota.dval < 0) {
                        return ERR_CODE_OTH;
                      }
                      if (T->act->lptr->tResult.elemIDptr != NULL) {
                        // promenna nebyla inicializova
                        if (T->act->lptr->tResult.elemIDptr->data->isInit == false) {
                          return ERR_CODE_DEF;
                       }
                      }
                      T->act->tResult.elemIDptr->data->type = DESETINE; // type uz neni CISLO, ale DESETINE
                      T->act->tResult.elemIDptr->data->isInit = true; // uz je definovana
                  }
                  else if ( T->act->lptr->tResult.typ == DESETINE )
                  {       
                      if (T->act->lptr->tResult.elemIDptr != NULL && !(T->act->lptr->tResult.desetin)) {
                        T->act->tResult.elemIDptr->data->hodnota.dval = T->act->lptr->tResult.elemIDptr->data->hodnota.dval;
                      } 
                      else {
                        T->act->tResult.elemIDptr->data->hodnota.dval = T->act->lptr->tResult.desetin;
                      }
                      // pokud je zaporne -> err
                      if (T->act->tResult.elemIDptr->data->hodnota.dval < 0) {
                        return ERR_CODE_OTH;
                      }
                      if (T->act->lptr->tResult.elemIDptr != NULL) {
                        // promenna nebyla inicializova
                        if (T->act->lptr->tResult.elemIDptr->data->isInit == false) {
                          return ERR_CODE_DEF;
                       }
                      }
                      T->act->tResult.elemIDptr->data->type = DESETINE;
                      T->act->tResult.elemIDptr->data->isInit = true; // uz je definovana         
                  }
                  else {
                    return ERR_CODE_COM;  
                  }
              }
              else if ( T->act->tResult.typ == KSTRING || T->act->tResult.typ == RETEZEC)    
              {
                  if (T->act->lptr->tResult.elemIDptr != NULL && !(T->act->lptr->tResult.data)) {
                    T->act->tResult.elemIDptr->data->hodnota.sval = (char *)gMalloc(sizeof(T->act->lptr->tResult.elemIDptr->data->hodnota.sval));
                    strcpy(T->act->tResult.elemIDptr->data->hodnota.sval,T->act->lptr->tResult.elemIDptr->data->hodnota.sval);

                    if (T->act->lptr->tResult.elemIDptr != NULL) {
                        // promenna nebyla inicializova
                        if (T->act->lptr->tResult.elemIDptr->data->isInit == false) {
                          return ERR_CODE_DEF;
                       }
                      }
                    T->act->tResult.elemIDptr->data->type = RETEZEC;
                    T->act->tResult.elemIDptr->data->isInit = true; // uz je definovana
                    break;
                  } 
                  else {
                    if (T->act->lptr->tResult.data == NULL) {
                      return ERR_CODE_COM; // spatne tu chytam error!!!
                    }
                    T->act->tResult.elemIDptr->data->hodnota.sval = (char *)gMalloc(sizeof(T->act->lptr->tResult.data));
                    strcpy(T->act->tResult.elemIDptr->data->hodnota.sval,T->act->lptr->tResult.data);
                   
                    if (T->act->lptr->tResult.elemIDptr != NULL) {
                        // promenna nebyla inicializova
                        if (T->act->lptr->tResult.elemIDptr->data->isInit == false) {
                          return ERR_CODE_DEF;
                        }
                      }
                    T->act->tResult.elemIDptr->data->type = RETEZEC;
                    T->act->tResult.elemIDptr->data->isInit = true; // uz je definovana
                    break;
                  }
              }
                                       
            break; 

// COUT <<
// -------------
            case  WRITE_X :
                                                         
  	          if (T->act->tOp1.typ == CELECISLO || T->act->tOp1.typ == KINT) {
	              printf("%d", (T->act->tOp1.cislo));  
              }
	            else if (T->act->tOp1.typ == DESETINE || T->act->tOp1.typ == KDOUBLE) {
          		  printf("%f", (T->act->tOp1.desetin));      
              }
	            else if (T->act->tOp1.typ == RETEZEC || T->act->tOp1.typ == KSTRING) {
  	            printf("%s", (T->act->tOp1.data));
              }
            break;      



// NOT_X <<
// -------------

            case NOT_X :

              // Preseni problemu s if(1) vs if(1+1) -> musim vyjmenovat vsechny operace u expr
              if (T->act->lptr->lptr->insType == ADD_X ||
                  T->act->lptr->lptr->insType == SUCC_X ||
                  T->act->lptr->lptr->insType == MUL_X ||
                  T->act->lptr->lptr->insType == DIV_X ||
                  T->act->lptr->lptr->insType == MENSI_X ||
                  T->act->lptr->lptr->insType == VETSI_X ||
                  T->act->lptr->lptr->insType == MENROVNO_X ||
                  T->act->lptr->lptr->insType == VETROVNO_X ||
                  T->act->lptr->lptr->insType == NEROVNA_X ||
                  T->act->lptr->lptr->insType == ROVNA_X
                  ) {
               // pokud je hodnota vyrazu v IF true
               if (T->act->lptr->lptr->tResult.cislo != 0 && T->act->lptr->lptr->tResult.desetin != 0) {
                  T->act->tResult.cislo = 0;
               }
               // jinak z 0 udelej 1
                else {
                  T->act->tResult.cislo = 1;
                }
              }
              else {
                if (T->act->lptr->tResult.cislo != 0 || T->act->lptr->tResult.desetin > 0.0000000000000000001 || (T->act->lptr->tResult.elemIDptr != NULL)) {
                  if (T->act->lptr->tResult.desetin > 0.00000000000000000001) {  // HAHA
                    T->act->tResult.cislo = 0;
                  }
                  else {
                    T->act->tResult.cislo = 1;
                  }
                  if (T->act->lptr->tResult.elemIDptr != NULL) {  // pokud mame if(a)
                    if (T->act->lptr->tResult.elemIDptr->data->hodnota.ival != 0 || T->act->lptr->tResult.elemIDptr->data->hodnota.dval != 0) {
                      T->act->tResult.cislo = 0;
                    }
                    else {
                     T->act->tResult.cislo = 1; 
                    }
                  }
                  else {
                    T->act->tResult.cislo = 0;  
                  }
               }
               // jinak z 0 udelej 1
                else {
                  T->act->tResult.cislo = 1;
                }
              }
            break;


// NOP_X <<
// -------------

            case NOP_X :

              // promenna nebyla inicializovana: napr: int a; if(a) {...}
              if (T->act->tResult.elemIDptr && T->act->tResult.elemIDptr->data->isInit == false)
              {
                return ERR_CODE_INIT;
              }
              // napr. string a = "sth"; if(a)
              if (T->act->tResult.elemIDptr != NULL && T->act->tResult.elemIDptr->data->type == RETEZEC) {
                  return ERR_CODE_COM;
              }
              // napr if ("str")
              if (T->act->tResult.typ == RETEZEC && T->act->rptr->insType == NOT_X) {
                  return ERR_CODE_COM;  
              }

            break;

// GOTO_X <<
// -------------

            case GOTO_X :

              // pokud je hodnota v tOp1 true -> skaceme na dany label
              // nebo taky pokud je prvni operand NULL
              {
              // pomocny trigger, jestli jsem nasel dal v pasce
              int foundR = false;

              // pomocna promenna na udrzeni stavajici pozice na pasce
              tapeItemPtr temp;
              temp = T->act;
              
              if ((T->act->lptr->tResult.cislo == 1) || (T->act->tOp1.data == NULL)) {
                                
                // presuneme data do tOp1 z tOp2
                if (T->act->tOp1.data == NULL) {
                  T->act->tOp1.data = T->act->tOp2.data;  // data jsou ty labels
                }
                // hledej prislusny label - uloz si ho at muzes porovnavat
                string currLabel;
                currLabel.str = T->act->tOp1.data;

                while((T->act = T->act->rptr)) {

                  string oldLabel;
                  oldLabel.str = T->act->tOp1.data;
                  // pokud jsi ho nasel -> skoc tam
                  if ((T->act->insType) == LAB_X) {

                     // nasel jsi
                    if (strCmpString(&oldLabel,&currLabel) == 0) {
                      // osetr
                      if (!(strcmp(T->act->tOp1.data, "endofmain"))) {
                        T->act = T->act->lptr;  // osetreni, at mi to endofmain spravne najde a ukonci 
                      }
                      foundR = true;  // nebudu hledat vlevo
                      break;
                    }

                  }
                }

                if (foundR == false) {

                  // zpatky nastav aktivni jak byl
                  T->act = temp;
                  currLabel.str = T->act->tOp1.data;

                  /* hledani zpatky na pasce (kvuli funkcim) - nesestreli to IFy etc...? */
                  while((T->act = T->act->lptr)) {

                    string oldLabel;
                    oldLabel.str = T->act->tOp1.data;
                    // pokud jsi ho nasel -> skoc tam
                    if ((T->act->insType) == LAB_X) {
                    
                      // nasel jsi
                      if (strCmpString(&oldLabel,&currLabel) == 0) {
                        break;
                      }

                    }
                  }
                }
              }
              }
              // jinak nedelej nic
            break;


// GOTOBCK_X <<
// -------------

            case GOTOBCK_X :

              // pokud je hodnota v tOp1 true -> skaceme na dany label
              // nebo taky pokud je prvni operand NULL

                // presuneme data do tOp1 z tOp2
                if (T->act->tOp1.data == NULL) {
                  T->act->tOp1.data = T->act->tOp2.data;  // data jsou ty labels
                }

                // hledej prislusny label - uloz si ho at muzes porovnavat
                
                string currLabel;
                currLabel.str = T->act->tOp1.data;

                while((T->act = T->act->lptr)) {

                  string oldLabel;
                  oldLabel.str = T->act->tOp1.data;
                  // pokud jsi ho nasel -> skoc tam
                  if ((T->act->insType) == LAB_X) {
                  
                    // nasel jsi
                    if (strCmpString(&oldLabel,&currLabel) == 0) {
                      break;
                    }

                  }

                }
              // jinak nedelej nic
            break;


// LAB_X <<
// -------------

            case LAB_X :

            break;

// RET_X <<
// -------------

            case RET_X :

            break;

// F_RET_X <<
// -------------

            case F_RET_X :

              {
              // hledej vlevo & vpravo, dokud nenajdes insType RET_X
              tapeItemPtr fRetTemp = T->act;  // stavajici prvek
              bool insFound = false;
              while (T->act->insType != STOP_X) { // RIGHT

                // nesel jsi
                if (T->act->insType == RET_X) {
                  // ulozime vysledek do nasledujici NOP_X
                  
                  // pokud returnujes ID  
                  if (T->act->lptr->tResult.elemIDptr != NULL) {

                    if (T->act->lptr->tResult.elemIDptr->data->type == CELECISLO || T->act->lptr->tResult.elemIDptr->data->type == KINT) {
                      fRetTemp->rptr->tResult.typ = CELECISLO;
                      fRetTemp->rptr->tResult.cislo = T->act->lptr->tResult.elemIDptr->data->hodnota.ival;
                    }
                    else if (T->act->lptr->tResult.elemIDptr->data->type == DESETINE || T->act->lptr->tResult.elemIDptr->data->type == KDOUBLE) {
                      fRetTemp->rptr->tResult.typ = DESETINE;
                      fRetTemp->rptr->tResult.desetin = T->act->lptr->tResult.elemIDptr->data->hodnota.dval;
                    }
                    else if (T->act->lptr->tResult.elemIDptr->data->type == RETEZEC || T->act->lptr->tResult.elemIDptr->data->type == KSTRING) {
                      fRetTemp->rptr->tResult.typ = RETEZEC;
                      fRetTemp->rptr->tResult.data = T->act->lptr->tResult.elemIDptr->data->hodnota.sval;
                    }

                  }

                  else {
                    fRetTemp->rptr->tResult.typ = T->act->lptr->tResult.typ;
                    
                    if (T->act->lptr->tResult.typ == CELECISLO || T->act->lptr->tResult.typ == KINT) {
                      fRetTemp->rptr->tResult.cislo = T->act->lptr->tResult.cislo;
                    }
                    else if (T->act->lptr->tResult.typ == DESETINE || T->act->lptr->tResult.typ == KDOUBLE) {
                      fRetTemp->rptr->tResult.desetin = T->act->lptr->tResult.desetin;
                    }
                    else if (T->act->lptr->tResult.typ == RETEZEC || T->act->lptr->tResult.typ == KSTRING) {
                      fRetTemp->rptr->tResult.data = T->act->lptr->tResult.data;
                    }
                  }

                  insFound = true;
                  break;
                }
                T->act = T->act->rptr;
              }

              T->act = fRetTemp;
              // hledame dal
              if (insFound == false) {

                while (T->act->insType) { // LEFT

                // nasel jsi
                if (T->act->insType == RET_X) {
                  // ulozime vysledek do nasledujici NOP_X
                  
                  // pokud returnujes ID  
                  if (T->act->lptr->tResult.elemIDptr != NULL) {

                    if (T->act->lptr->tResult.elemIDptr->data->type == CELECISLO || T->act->lptr->tResult.elemIDptr->data->type == KINT) {
                      fRetTemp->rptr->tResult.typ = CELECISLO;
                      fRetTemp->rptr->tResult.cislo = T->act->lptr->tResult.elemIDptr->data->hodnota.ival;
                    }
                    else if (T->act->lptr->tResult.elemIDptr->data->type == DESETINE || T->act->lptr->tResult.elemIDptr->data->type == KDOUBLE) {
                      fRetTemp->rptr->tResult.typ = DESETINE;
                      fRetTemp->rptr->tResult.desetin = T->act->lptr->tResult.elemIDptr->data->hodnota.dval;
                    }
                    else if (T->act->lptr->tResult.elemIDptr->data->type == RETEZEC || T->act->lptr->tResult.elemIDptr->data->type == KSTRING) {
                      fRetTemp->rptr->tResult.typ = RETEZEC;
                      fRetTemp->rptr->tResult.data = T->act->lptr->tResult.elemIDptr->data->hodnota.sval;
                    }

                  }

                  else {
                    fRetTemp->rptr->tResult.typ = T->act->lptr->tResult.typ;
                    
                    if (T->act->lptr->tResult.typ == CELECISLO || T->act->lptr->tResult.typ == KINT) {
                      fRetTemp->rptr->tResult.cislo = T->act->lptr->tResult.cislo;
                    }
                    else if (T->act->lptr->tResult.typ == DESETINE || T->act->lptr->tResult.typ == KDOUBLE) {
                      fRetTemp->rptr->tResult.desetin = T->act->lptr->tResult.desetin;
                    }
                    else if (T->act->lptr->tResult.typ == RETEZEC || T->act->lptr->tResult.typ == KSTRING) {
                      fRetTemp->rptr->tResult.data = T->act->lptr->tResult.data;
                    }
                  }
                  insFound = true;
                  break;
                }
                T->act = T->act->lptr;
              }  

              T->act = fRetTemp;  // jsme zpet, kde jsme byli
              // jestli odalokujes ten pointer, tak to cele shodis
              }
              }
            break;             

/*

//DELKA_X 
// --------                                                                    
            case DELKA_X :

              if ( ( T->act->tOp1.typ == RETEZEC ) && ( T->act->tResult.typ == CELECISLO ) )
                {
                  T->act->tResult.cislo = lenght(T->act->tOp1.data);
                } 
              else  
                { 
        //        return (ERR_CODE_INTER);
                }            
            break;       
            
// PODSTRING_X
// -----------
            case PODSTRING_X :

              if ( ( T->act->tOp1.typ == RETEZEC ) && ( T->act->tOp2.typ == CELECISLO ) && (T->act->tResult.typ == CELECISLO ) )
                {
                  T->act->tResult.data = substrig(T->act->tOp1.data, T->act->tOp2.cislo, T->act->tResult.cislo);
                } 
              else  
                { 
        //        return (ERR_CODE_INTER);
                }              
            break;
            
// KONKATENACE_X
// -----------
            case KONKATENACE_X :

              if ( ( T->act->tOp1.typ == RETEZEC ) && ( T->act->tOp2.typ == RETEZEC ) && (T->act->tResult.typ == RETEZEC ) )
                {
                  T->act->tResult.data = strConCat(T->act->tOp1.data, T->act->tOp2.data);
                } 
              else  
                { 
        //        return (ERR_CODE_INTER);
                }              
            break;
            
// VYHLEDANI_X
// -----------
            case VYHLEDANI_X :

              if ( ( T->act->tOp1.typ == RETEZEC ) && ( T->act->tOp2.typ == RETEZEC ) && (T->act->tResult.typ == CELECISLO ) )
                {
                  T->act->tResult.cislo = strFind(T->act->tOp1.data, T->act->tOp2.data);
                } 
              else  
                { 
        //        return (ERR_CODE_INTER);
                }               
            break;

// SORT_X
// -------     
            case SORT_X :
            
              if ( ( T->act->tOp1.typ == RETEZEC ) && (T->act->tResult.typ == RETEZEC ) )
                {
                  T->act->tResult.data = qSort(T->act->tOp1.data);
                } 
              else  
                { 
        //        return (ERR_CODE_INTER);
                }               
            break;
   
*/  

// pokud je nejaky bud (a prijde neco nevhodneho)
// -------------------------------------------------------------------------------------------------------------------------------------------

            default:
              // nemel by tu nikdy dojit
              return ERR_CODE_SYN;  // asi Syntaxe                                          
          }
      
      T->act = T->act->rptr;  // posuneme aktivitu na dalsi prvek
      // + treba funkce na posouvani aktivity pri skoku
                             
      } 
    return ERR_CODE_SUCC;          
  }
