/* ******************************** scaner.c ******************************** */
/* Soubor:              scaner.c - Lexikálny analyzátor                       */
/* Kodovani:            UTF-8                                                 */
/* Datum:               9. 12. 2015                                           */
/*                                                                            */
/* Predmet:             Formalni jazyky a prekladace (IFJ)                    */
/* Projekt:             Implementace interpretu jazyka IFJ15                  */
/* Varianta zadani:     a/1/I                                                 */
/*                                                                            */
/* Autori, login:       Tereza Kabeláčová           xkabel06                  */
/*                      Adriana Jelenčíková         xjelen10                  */
/*                      Michal Klhůfek              xklhuf00                  */
/*                      Jan Ondruch                 xondru14                  */
/*                      Martin Auxt                 xauxtm00                  */
/* ****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <strings.h>
#include "scaner.h"
#include "main.h"
#include "garbage.h"


token getNextToken()
  {
          
    token TOK;				                                // Token, ktery budeme vracet
  	char nactu = fgetc(vstup);  
  	int exponent = 0;
    double krok = 0.1;
    int alokace = 0;

    int d = 15;
      
// --------------------------------------------------------------------------------
//                             TELO LEXIKALNIHO ANALYZATORU
// --------------------------------------------------------------------------------

while ( (nactu == ' ') || (nactu == '	') || (nactu == '\n') || (nactu == '/') )
 {
    if (nactu == ' ')     // pokud je tam mezera
      nactu = fgetc(vstup);
    if (nactu == '	')    // pokud je tam tabulátor
      nactu = fgetc(vstup);
    if (nactu == '\n')    // pokud je tam novy radek
      nactu = fgetc(vstup);        
    if (nactu == '/')         // zacina radkovy komentar
      {            
        nactu = fgetc(vstup); // nactu si dalsi znak pro kontrolu zda to pokracuje *
        if (nactu == '*')  
          {
            nactu = fgetc(vstup);
            do
              {
                if (nactu == '*')
                  {
                    nactu = fgetc(vstup);
                    if (nactu == '/') 
                      {
                        nactu = fgetc(vstup);
                        break;
                      }
                  }
                nactu = fgetc(vstup); 
                if (nactu == EOF)
                  {
                     TOK.typ = LEX_ERROR;              // lexikalni chyba
                     return TOK;
                  }
              }    
            while (nactu != EOF);                     // kontroluji až do konce souboru
          }
        else if (nactu == '/') 
          {        
            while (nactu != '\n')
              nactu = fgetc(vstup);    
            ungetc(nactu,vstup);   
          }
        else
          {   
            d = 1;         
            ungetc(nactu,vstup);   
            break;
          }  
      }                          
    if (nactu == ' ')     // pokud je tam mezera
      nactu = fgetc(vstup);
    if (nactu == '	')    // pokud je tam tabulátor
      nactu = fgetc(vstup);
    if (nactu == '\n')    // pokud je tam novy radek
      nactu = fgetc(vstup);       
  }

// ---------------------------------------------------------------------------- //
//                                   Hlavni telo LA                             //
// ---------------------------------------------------------------------------- //

//                                   konec souboru       
// --------------------------------------------------------------------------------

    if (nactu == -1) 
      {
        TOK.typ = END_OF_FILE;
        return TOK;
      }

//                                   jednoduche znaky      
// --------------------------------------------------------------------------------    

// -      

    else if (nactu == '-') 
      {
        TOK.typ = MINUS;
        TOK.data = "-";
        return TOK;
      } 

// +      

    else if (nactu == '+') 
      {
        TOK.typ = PLUS;
        TOK.data = "+";
        return TOK;
      }          
      
// *    

    else if (nactu == '*') 
      {
        TOK.typ = NASOBENI;
        TOK.data = "*";
        return TOK;
      }           

// /      
      
    else if ( (nactu == '/') || ( d == 1) ) 
      {        
        nactu = fgetc(vstup);
        if ( ( d == 1) )
          {
            TOK.typ = DELENI;
            TOK.data = "/";
            ungetc(nactu,vstup); 
            d = 15;
            return TOK;
          }
        if ((nactu == '/') || (nactu == '*'))
          {
            ungetc(nactu,vstup); 
            ungetc(nactu,vstup); 
          } 
        else
          {
            TOK.typ = DELENI;
            TOK.data = "/";
            return TOK;
          }  
      }

// (      
                   
    else if (nactu == '(') 
      {
        TOK.typ = LZAVORKA;
        TOK.data = "(";
        return TOK;
      }  

// )      
                  
    else if (nactu == ')') 
      {
        TOK.typ = PZAVORKA;
        TOK.data = ")";
        return TOK;
      }             
      
// {    
 
    else if (nactu == '{') 
      {
        TOK.typ = LSLOZENA;
        TOK.data = "{";
        return TOK;
      }             
 
// }      
      
    else if (nactu == '}') 
      {
        TOK.typ = PSLOZENA;
        TOK.data = "}";
        return TOK;
      }   

// ,      
                 
    else if (nactu == ',') 
      {
        TOK.typ = CARKA;
        TOK.data = ",";
        return TOK;
      } 
 
// ;  
               
    else if (nactu == ';') 
      {
        TOK.typ = STREDNIK;
        TOK.data = ";";
        return TOK;
      }

//                                   slozene prikazy       
// ----------------------------------------------------------------------------


// < << <=      
 
    else if (nactu == '<')   
      {
        nactu = fgetc(vstup); 
        if (nactu == '=') 
          { 
            TOK.typ = MENSIROVNO;
            TOK.data = "<=";
            return TOK;
          }
        else if (nactu == '<') 
          {
            TOK.typ = OCOUT;         
            TOK.data = "<<";
            return TOK;
          }
        else if ((nactu != '<') || (nactu != '='))
          {
            ungetc(nactu,vstup);                                  
            TOK.typ = MENSI;
            TOK.data = "<";
            return TOK;
          }
      }     

// > >> >=   
       
    else if (nactu == '>')   
      {
        nactu = fgetc(vstup); 
        if (nactu == '=') 
          { 
            TOK.typ = VETSIROVNO;
            TOK.data = ">=";
            return TOK;
          }
        else if (nactu == '>') 
          { 
            TOK.typ = OCIN;
            TOK.data = ">>";
            return TOK;
          }
        else if ((nactu != '>') || (nactu != '='))
          {
            ungetc(nactu,vstup);                      
            TOK.typ = VETSI;
            TOK.data = ">";
            return TOK;
          }
      }      

// = ==      
 
    else if (nactu == '=')   
      {
        nactu = fgetc(vstup); 
        if (nactu == '=') 
          { 
            TOK.typ = ROVNO;
            TOK.data = "==";
            return TOK;
          }
        else if (nactu != '=')
          {
            ungetc(nactu,vstup);                      
            TOK.typ = PRIRAD;
            TOK.data = "=";
            return TOK;
          }
      }
   
// !=     
   
    else if (nactu == '!')      
      {
        nactu = fgetc(vstup); 
        if (nactu == '=') 
          { 
            TOK.typ = NEROVNO;
            TOK.data = "!=";
            return TOK;
          }
        else if (nactu != '=')
          {  
            ungetc(nactu,vstup);                      
            TOK.typ = LEX_ERROR;              // lexikalni chyba
            return TOK;
          }  
      }   
    
//                                   cela cisla      
// ----------------------------------------------------------------------------             

    else if ((nactu <= '9' ) && (nactu >= '0'))
      {
      
// cele cislo cislo      
      
        TOK.typ = CELECISLO;            // typ je cele cislo
        TOK.cislo = nactu - '0';        // prevedu si to na int
        nactu = fgetc(vstup);           // nactu dalsi znak
        while ((nactu <= '9' ) && (nactu >= '0'))
          {
            TOK.cislo = TOK.cislo*10 + nactu - '0';   // cyklim a nacitam cisla

            if (TOK.cislo < 0)
              {
                TOK.typ = LEX_ERROR; 
                return TOK;
              } 
            nactu = fgetc(vstup);   
          }       
        if ( (nactu != '.') && (nactu != 'e') && (nactu != 'E') ) // pokud to co prislo neni . e E
             {
               if ( (nactu == ' ') || (nactu == '	') || (nactu == ';') || (nactu == ')') || (nactu == '<') || (nactu == '>') || (nactu == '+') || (nactu == '-') || (nactu == '/') || (nactu == '*') || (nactu == '=') || (nactu == '!') || (nactu == ',') || (nactu == '\n'))
                  {
                     ungetc(nactu,vstup);                     // posilam token                
                     return TOK;    
                  }
               else
                  {
                     TOK.typ = LEX_ERROR;                                                  // lexikalni chyba
                     return TOK;  
                  }
                
             }
        else if (((nactu == 'e' ) || (nactu == 'E')))   // pracuji s casti exponentu
          {
            TOK.typ = DESETINE;
            TOK.desetin = TOK.cislo;
            nactu = fgetc(vstup); 
            if (nactu == '+')
              {
                nactu = fgetc(vstup); 
                if (!((nactu <= '9' ) && (nactu >= '0')))                                 // musi za tim byt cislo
                  {
                    TOK.typ = LEX_ERROR;                                                  // lexikalni chyba
                    return TOK;                     
                  }
                if (((nactu <= '9' ) && (nactu >= '0')))
                  {
                    while ((nactu <= '9' ) && (nactu >= '0'))
                      {
                        exponent = ((exponent * 10) + nactu) - '0';       // vypocet exponentu *10 + prevod na int
                        nactu = fgetc(vstup);
                      }           
                    int original = TOK.desetin;        
                    for(int x = exponent; x > 1; x--) 
                      {
                        TOK.desetin = TOK.desetin * original;               // vypocet cisla s exponentem                                 
                        if (TOK.desetin < 0)
                          {
                            TOK.typ = LEX_ERROR; 
                            return TOK;
                          } 
                                                                                     
                      }
                      
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
               if ( (nactu == ' ') || (nactu == '	') || (nactu == ';') || (nactu == ')') || (nactu == '<') || (nactu == '>') || (nactu == '+') || (nactu == '-') || (nactu == '/') || (nactu == '*') || (nactu == '=') || (nactu == '!') || (nactu == ',') || (nactu == '\n'))
                  {
                     ungetc(nactu,vstup);                     // posilam token                
                     return TOK;    
                  }
               else
                  {
                     TOK.typ = LEX_ERROR;                                                  // lexikalni chyba
                     return TOK;  
                  }
                  }
              }
            else if (nactu == '-')          // dobrovolny znak - 
              {
                TOK.typ = DESETINE;
                TOK.desetin = TOK.cislo;
                nactu = fgetc(vstup); 
                if (!((nactu <= '9' ) && (nactu >= '0')))                                 // musi za tim byt cislo
                  {
                    TOK.typ = LEX_ERROR;                                                  // lexikalni chyba        
                    return TOK;    
                  } 
                if (((nactu <= '9' ) && (nactu >= '0')))
                  {
                    while ((nactu <= '9' ) && (nactu >= '0'))
                      {
                        exponent = exponent * 10 + nactu - '0';
                        nactu = fgetc(vstup);
                      }
                    float original = TOK.desetin;  
                    for(int x = exponent; x > 1; x--) 
                      {
                        TOK.desetin = TOK.desetin * original;
                        if (TOK.desetin < 0)
                          {
                            TOK.typ = LEX_ERROR; 
                            return TOK;
                          }                          
                      }
                    TOK.desetin = 1/TOK.desetin;          // aby se vypocitala hodnota zaporneho exponentu, tak 1/cislo
                    
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                    
               if ( (nactu == ' ') || (nactu == '	') || (nactu == ';') || (nactu == ')') || (nactu == '<') || (nactu == '>') || (nactu == '+') || (nactu == '-') || (nactu == '/') || (nactu == '*') || (nactu == '=') || (nactu == '!') || (nactu == ',') || (nactu == '\n'))
                  {
                     ungetc(nactu,vstup);                     // posilam token                
                     return TOK;    
                  }
               else
                  {
                     TOK.typ = LEX_ERROR;                                                  // lexikalni chyba
                     return TOK;  
                  }   
                  }
              }
            else if ((nactu <= '9' ) && (nactu >= '0'))  
              {
                TOK.typ = DESETINE;
                TOK.desetin = TOK.cislo;
                if (((nactu <= '9' ) && (nactu >= '0')))
                  {
                    while ((nactu <= '9' ) && (nactu >= '0'))
                      {
                        exponent = exponent * 10 + nactu - '0';
                        nactu = fgetc(vstup);
                      }
                    int original = TOK.desetin;  
                    for(int x = exponent; x > 1; x--) 
                      {
                        TOK.desetin = TOK.desetin * original;
                        if (TOK.desetin < 0)
                          {
                            TOK.typ = LEX_ERROR; 
                            return TOK;
                          }                                       
                      } 
                      
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                      
                      
               if ( (nactu == ' ') || (nactu == '	') || (nactu == ';') || (nactu == ')') || (nactu == '<') || (nactu == '>') || (nactu == '+') || (nactu == '-') || (nactu == '/') || (nactu == '*') || (nactu == '=') || (nactu == '!') || (nactu == ',') || (nactu == '\n'))
                  {
                     ungetc(nactu,vstup);                     // posilam token                
                     return TOK;    
                  }
               else
                  {
                     TOK.typ = LEX_ERROR;                                                  // lexikalni chyba
                     return TOK;  
                  }                    
                  }                    
               }
             else if (!(((nactu <= '9' ) && (nactu >= '0'))|| (nactu == '+') || (nactu == '-')))    // musi za tim byt cislo
               {
                 TOK.typ = LEX_ERROR;              // lexikalni chyba
                 return TOK;
               }                     
          }                   
             
// desetine cislo
             
        else if (nactu == '.')                                                       
          {
            krok = 0.1;
            exponent = 0;
            TOK.typ = DESETINE;
            TOK.desetin = TOK.cislo;
            nactu = fgetc(vstup);     
            int byl_tu = 0;       
            if (!((nactu <= '9' ) && (nactu >= '0')))                                // musi za tim byt cislo
              {
                TOK.typ = LEX_ERROR;                                                 // lexikalni chyba
                return TOK;                
              }               
            if (((nactu <= '9' ) && (nactu >= '0')))
              {
                while ((nactu <= '9' ) && (nactu >= '0'))
                  {
                    if (byl_tu > 5)                                       // mame max. 6 desetinych mist
                      {
                        TOK.typ = LEX_ERROR; 
                        return TOK;
                      }     
                    TOK.desetin = TOK.desetin + (nactu - '0') * krok;   
                    krok = krok * 0.1;
                    nactu = fgetc(vstup);
                    byl_tu++;                    
                  }
              }
            if (((nactu == 'e' ) || (nactu == 'E')))
              {
                nactu = fgetc(vstup); 
                if (nactu == '+')
                  {
                    nactu = fgetc(vstup); 
                    if (!((nactu <= '9' ) && (nactu >= '0')))                                 // musi za tim byt cislo
                      {
                        TOK.typ = LEX_ERROR;                                                  // lexikalni chyba
                        return TOK;                     
                      } 
                    if (((nactu <= '9' ) && (nactu >= '0')))
                      {
                        while ((nactu <= '9' ) && (nactu >= '0'))
                          {
                            exponent = exponent * 10 + nactu - '0';
                            nactu = fgetc(vstup);
                          }
                        double original = TOK.desetin;        
                        for(int x = exponent; x > 1; x--) 
                          {
                            TOK.desetin = TOK.desetin * original;               // vypocet cisla s exponentem                                                
                            if (TOK.desetin < 0)
                              {
                                TOK.typ = LEX_ERROR; 
                                return TOK;
                              }                                                                                      
                          } 
                          
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                          
                           
               if ( (nactu == ' ') || (nactu == '	') || (nactu == ';') || (nactu == ')') || (nactu == '<') || (nactu == '>') || (nactu == '+') || (nactu == '-') || (nactu == '/') || (nactu == '*') || (nactu == '=') || (nactu == '!') || (nactu == ',') || (nactu == '\n'))
                  {
                     ungetc(nactu,vstup);                     // posilam token                
                     return TOK;    
                  }
               else
                  {
                     TOK.typ = LEX_ERROR;                                                  // lexikalni chyba
                     return TOK;  
                  }
                      }
                  }
                else if (nactu == '-')
                  {
                    nactu = fgetc(vstup); 
                    if (!((nactu <= '9' ) && (nactu >= '0')))                                 // musi za tim byt cislo
                      {
                        TOK.typ = LEX_ERROR;                                                  // lexikalni chyba        
                        return TOK;    
                      } 
                    if (((nactu <= '9' ) && (nactu >= '0')))
                      {
                        while ((nactu <= '9' ) && (nactu >= '0'))
                          {
                            exponent = exponent * 10 + nactu - '0';
                            nactu = fgetc(vstup);
                          }
                        float original = TOK.desetin;                         
                        for(int x = exponent; x > 1; x--) 
                          {
                            TOK.desetin = TOK.desetin * original;
                            if (TOK.desetin < 0)
                              {
                                TOK.typ = LEX_ERROR; 
                                return TOK;
                              }                          
                          }
                        TOK.desetin = 1/TOK.desetin;
                        
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                        
               if ( (nactu == ' ') || (nactu == '	') || (nactu == ';') || (nactu == ')') || (nactu == '<') || (nactu == '>') || (nactu == '+') || (nactu == '-') || (nactu == '/') || (nactu == '*') || (nactu == '=') || (nactu == '!') || (nactu == ',') || (nactu == '\n'))
                  {
                     ungetc(nactu,vstup);                     // posilam token                
                     return TOK;    
                  }
               else
                  {
                     TOK.typ = LEX_ERROR;                                                  // lexikalni chyba
                     return TOK;  
                  }    
                      }
                  }
                else if ((nactu <= '9' ) && (nactu >= '0'))  
                  {
                    if (((nactu <= '9' ) && (nactu >= '0')))
                      {
                        while ((nactu <= '9' ) && (nactu >= '0'))
                          {
                            exponent = exponent * 10 + nactu - '0';
                            nactu = fgetc(vstup);
                          }
                        float original = TOK.desetin;        
                        for(int x = exponent; x > 1; x--) 
                          {
                            TOK.desetin = TOK.desetin * original;
                            if (TOK.desetin < 0)
                              {
                                TOK.typ = LEX_ERROR; 
                                return TOK;
                              }                                       
                          }                           
                        for(int x = exponent; x > 1; x--) 
                          {
                            TOK.desetin = TOK.desetin * original;
                          }
                          
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!                          
                          
               if ( (nactu == ' ') || (nactu == '	') || (nactu == ';') || (nactu == ')') || (nactu == '<') || (nactu == '>') || (nactu == '+') || (nactu == '-') || (nactu == '/') || (nactu == '*') || (nactu == '=') || (nactu == '!') || (nactu == ',') || (nactu == '\n'))
                  {
                     ungetc(nactu,vstup);                     // posilam token                
                     return TOK;    
                  }
               else
                  {
                     TOK.typ = LEX_ERROR;                                                  // lexikalni chyba
                     return TOK;  
                  }                           
                      }                    
                  }
                else if (!(((nactu <= '9' ) && (nactu >= '0'))|| (nactu == '+') || (nactu == '-')))    // musi za tim byt cislo
                  {
                    TOK.typ = LEX_ERROR;              // lexikalni chyba
                    return TOK;
                  }                    
              }
            else
              {
              
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!              
               if ( (nactu == ' ') || (nactu == '	') || (nactu == ';') || (nactu == ')') || (nactu == '<') || (nactu == '>') || (nactu == '+') || (nactu == '-') || (nactu == '/') || (nactu == '*') || (nactu == '=') || (nactu == '!') || (nactu == ',') || (nactu == '\n'))
                  {
                     ungetc(nactu,vstup);                     // posilam token                
                     return TOK;    
                  }
               else
                  {
                     TOK.typ = LEX_ERROR;                                                  // lexikalni chyba
                     return TOK;  
                  }
              }  
          }             
      }  
         
//                                   klicova slova a identifikatory      
// ----------------------------------------------------------------------------      

    else if ( ( ( nactu=='_' ) || (( nactu>='A' ) && ( nactu<='Z' )) || (( nactu>='a' ) && ( nactu<='z' ) ) ) )
      {
        TOK.data=(char *)gMalloc(sizeof(char) * 2);
        if(TOK.data == NULL) 
          {
            TOK.typ = CHBA;
            return TOK;      
          }

        TOK.data[0] = nactu;  
        TOK.data[1] = '\0';
        alokace = 3;    
//        *TOK.data = nactu + '\0';     // ulozim si ukoncovaci znak
        TOK.typ = ID;
        nactu = fgetc(vstup);      
        while (1)
          {
            if ( !( ( nactu=='_' ) || ( ( nactu>='A' ) && ( nactu<='Z' ) ) || (( nactu>='a' ) && ( nactu<='z' ) ) || (( nactu>='1' ) && ( nactu<='9' ) ) ) )
              {
                break;  
              }  
            TOK.data=(char *)gRealloc(TOK.data, alokace * sizeof(char));
            if(TOK.data == NULL) 
              {
                TOK.typ = CHBA;
                return TOK;  
              }
            TOK.data[alokace-2] = nactu;  
            TOK.data[alokace-1] = '\0'; 
            alokace++;  
            nactu = fgetc(vstup);  
          }
        ungetc(nactu,vstup);  


// AUTO
     
        while (strcmp("auto",TOK.data) == 0)
          {
            TOK.typ = KAUTO;          
            break;
          }
          
// CIN
     
        while (strcmp("cin",TOK.data) == 0)
          {
            TOK.typ = KCIN;
            break;
          }
          
// COUT
     
        while (strcmp("cout",TOK.data) == 0)
          {
            TOK.typ = KCOUT;
            break;
          }
          
// DOUBLE
     
        while (strcmp("double",TOK.data) == 0)
          {
            TOK.typ = KDOUBLE;
            break;
          }
          
// ELSE
     
        while (strcmp("else",TOK.data) == 0)
          {
            TOK.typ = KELSE;
            break;
          }
          
// FOR
     
        while (strcmp("for",TOK.data) == 0)
          {
            TOK.typ = KFOR;
            break;
          }
          
// IF
     
        while (strcmp("if",TOK.data) == 0)
          {
            TOK.typ = KIF;
            break;
          }
          
// INT
     
        while (strcmp("int",TOK.data) == 0)
          {
            TOK.typ = KINT;
            break;
          }
          
// RETURN
     
        while (strcmp("return",TOK.data) == 0)
          {
            TOK.typ = KRETURN;
            break;
          }

// STRING
     
        while (strcmp("string",TOK.data) == 0)
          {
            TOK.typ = KSTRING;
            break;
          }


        
        return TOK;  
      }
  
      
//                                   retezce       
// ----------------------------------------------------------------------------           


    else if ( nactu == '"')
      {
        nactu = fgetc(vstup);            
        if (nactu == '"')  
          {      
            TOK.data = " ";           // prazdny retezec
            TOK.typ = RETEZEC;
            return TOK;    
          }  
        else if (nactu == '\n')      // pokud je zadano "\n(enter) je to chyba 
          {
            TOK.typ = LEX_ERROR;
            return TOK;    
          }
        else
          {
            TOK.data=(char *)gMalloc(sizeof(char) * 2);
            if(TOK.data == NULL) 
              {
                TOK.typ = CHBA;
                return TOK;    
              }       
            int poprve = 0;
            while ((nactu != '"') && (nactu != '\n') && (nactu != EOF))
              {
                if (nactu == '\\')                                            // specialni znaky
                  {
                    nactu = fgetc(vstup);     
                    if (nactu == '"')                                         // \"
                      {
                         if (poprve == 0)
                          {
                            TOK.data[0] = nactu;  
                            TOK.data[1] = '\0';
                            alokace = 3;                                      
                         //   *TOK.data = nactu + '\0';                         // ulozim specialni znak
                            TOK.typ = RETEZEC;
                            poprve = 1;
                            nactu = fgetc(vstup); 
                          } 
                        else
                          {
                            TOK.data=(char *)gRealloc(TOK.data, alokace * sizeof(char));
                            if(TOK.data == NULL) 
                              {
                                TOK.typ = CHBA;
                                return TOK;  
                              }   
                            TOK.data[alokace-2] = nactu;  
                            TOK.data[alokace-1] = '\0'; 
                            alokace++;  
                            nactu = fgetc(vstup);                                                           
                          }  
                      }
                    else if (nactu == '\\')                                 // \'
                      {
                        if (poprve == 0)
                          {
                            TOK.data[0] = nactu;  
                            TOK.data[1] = '\0';
                            alokace = 3; 
                   //         *TOK.data = nactu + '\0';
                            TOK.typ = RETEZEC;
                            poprve = 1;
                            nactu = fgetc(vstup); 
                          } 
                        else
                          {
                            TOK.data=(char *)gRealloc(TOK.data, alokace * sizeof(char));
                            if(TOK.data == NULL) 
                              {
                                TOK.typ = CHBA;
                                return TOK;  
                              }  
                            TOK.data[alokace-2] = nactu;  
                            TOK.data[alokace-1] = '\0'; 
                            alokace++;  
                            nactu = fgetc(vstup);                                                           
                          }  
                      }
                    else if (nactu == 'n')                              // \n
                      {
                        if (poprve == 0)
                          {
                            TOK.data[0] = '\n';  
                            TOK.data[1] = '\0';
                            alokace = 3; 
//                            *TOK.data = '\n' + '\0';
                            TOK.typ = RETEZEC;
                            poprve = 1;
                            nactu = fgetc(vstup); 
                          } 
                        else
                          {
                            TOK.data=(char *)gRealloc(TOK.data, alokace * sizeof(char));
                            if(TOK.data == NULL) 
                              {
                                TOK.typ = CHBA;
                                return TOK;  
                              }  
                            TOK.data[alokace-2] = '\n';  
                            TOK.data[alokace-1] = '\0'; 
                            alokace++;  
                            nactu = fgetc(vstup);                                                           
                          }
                      }  
                    else if (nactu == 't')                  
                      {
                        if (poprve == 0)
                          {
                            TOK.data[0] = ' ';  
                            TOK.data[1] = '\0';                         
                            alokace = 3; 
//                            *TOK.data = ' ' + '\0';
                            TOK.typ = RETEZEC;
                            poprve = 1;
                            nactu = fgetc(vstup); 
                          } 
                        else
                          {
                            TOK.data=(char *)gRealloc(TOK.data, alokace * sizeof(char));
                            if(TOK.data == NULL) 
                              {
                                TOK.typ = CHBA;
                                return TOK;  
                              }  
                            TOK.data[alokace-2] = ' ';  
                            TOK.data[alokace-1] = '\0'; 
                            alokace++;  
                            nactu = fgetc(vstup);                                                           
                          }
                      }    
                      
                    else if (nactu == 'x')
                      {
                        char *hexa;
    				            int prvni_je = 0;                             // pro kontrolu jestli se nacetl znak \x00               
                        hexa=(char *)gMalloc(sizeof(char) * 2);
                        nactu = fgetc(vstup);  
                        if (nactu == '0')                         // kontrola naceteni prvni nuly
   				                prvni_je = 666;  
                        if ((( nactu>='0' ) && ( nactu<='9' )) || (( nactu>='A' ) && ( nactu<='F' )) || (( nactu>='a' ) && ( nactu<='f' )))
                          {   
                            hexa[0] = nactu;      // prvni hodnotu si ulozim do pole na pozici 1
                            nactu = fgetc(vstup); 
                            if ( ( nactu == '0' ) && ( prvni_je == 666 ) )         // kontola, zda se nacetla i druha nula zaroven s prvni                  
                              prvni_je = 696;  
                            if (((( nactu>='0' ) && ( nactu<='9' )) || (( nactu>='A' ) && ( nactu<='F' )) || (( nactu>='a' ) && ( nactu<='f' ))) && (prvni_je != 696) )
                              {   
                                hexa[1] = nactu;      // druhou hodnotu si ulozim do pole na hodnotu 2
                                char *str = hexa;     
                                char *pEnd;
                                int r;
                                r = strtol(str, &pEnd, 16);     // prevedu string na znak, 16 pro prevod z hexa
                                if (poprve == 0)
                                  {
                                    TOK.data[0] = r;  
                                    TOK.data[1] = '\0';                                   
                                    alokace = 3; 
//                                  *TOK.data = r + '\0';
                                    TOK.typ = RETEZEC;
                                    poprve = 1;
                                    nactu = fgetc(vstup); 
                                  }
                                else
                                  {
                                    TOK.data=(char *)gRealloc(TOK.data, alokace * sizeof(char));
                                    if(TOK.data == NULL) 
                                      {
                                        TOK.typ = CHBA;
                                        return TOK;  
                                      }  
                                    TOK.data[alokace-2] = r;  
                                    TOK.data[alokace-1] = '\0'; 
                                    alokace++;  
                                    nactu = fgetc(vstup);                                                           
                                  }                                
                              }
                            else      // stavy kdy uzivatel zada neco jineho nez je povoleno \dxx - xx jsou mimo s
                              {
                                if (poprve == 0)
                                  {
                                    TOK.data[0] = '\\';  
                                    TOK.data[1] = '\0';   
                                    alokace = 3; 
//                                    *TOK.data = '\\' + '\0';
                                    TOK.typ = RETEZEC;
                                    poprve = 1;
                                    TOK.data=(char *)gRealloc(TOK.data, alokace * sizeof(char));
                                    if(TOK.data == NULL) 
                                      {
                                        TOK.typ = CHBA;
                                        return TOK;  
                                      }  
                                    TOK.data[alokace-2] = 'x';  
                                    TOK.data[alokace-1] = '\0'; 
                                    alokace++;      
                                    TOK.data=(char *)gRealloc(TOK.data, alokace * sizeof(char));
                                    if(TOK.data == NULL) 
                                      {
                                        TOK.typ = CHBA;
                                        return TOK;  
                                      }                                        
                                    TOK.data[alokace-2] = hexa[0];  
                                    TOK.data[alokace-1] = '\0'; 
                                    alokace++;                                          
                                    TOK.data=(char *)gRealloc(TOK.data, alokace * sizeof(char));
                                    if(TOK.data == NULL) 
                                      {
                                        TOK.typ = CHBA;
                                        return TOK;  
                                      }  
                                    TOK.data[alokace-2] = nactu;  
                                    TOK.data[alokace-1] = '\0'; 
                                    alokace++;        
                                    nactu = fgetc(vstup);
                                  }
                                else
                                  {
                                    TOK.data=(char *)gRealloc(TOK.data, alokace * sizeof(char));
                                    if(TOK.data == NULL) 
                                      {
                                        TOK.typ = CHBA;
                                        return TOK;  
                                      }  
                                    TOK.data[alokace-2] = '\\';  
                                    TOK.data[alokace-1] = '\0'; 
                                    alokace++;      
                                    TOK.data=(char *)gRealloc(TOK.data, alokace * sizeof(char));
                                    if(TOK.data == NULL) 
                                      {
                                        TOK.typ = CHBA;
                                        return TOK;  
                                      }  
                                    TOK.data[alokace-2] = 'x';  
                                    TOK.data[alokace-1] = '\0'; 
                                    alokace++;                                       
                                    TOK.data=(char *)gRealloc(TOK.data, alokace * sizeof(char));
                                    if(TOK.data == NULL) 
                                      {
                                        TOK.typ = CHBA;
                                        return TOK;  
                                      }                                        
                                    TOK.data[alokace-2] = hexa[0];  
                                    TOK.data[alokace-1] = '\0'; 
                                    alokace++;                                          
                                    TOK.data=(char *)gRealloc(TOK.data, alokace * sizeof(char));
                                    if(TOK.data == NULL) 
                                      {
                                        TOK.typ = CHBA;
                                        return TOK;  
                                      }  
                                    TOK.data[alokace-2] = nactu;  
                                    TOK.data[alokace-1] = '\0'; 
                                    alokace++;        
                                    nactu = fgetc(vstup);
                                  }  
                              }          
                          }                     
                        else
                          {
                            if (poprve == 0)
                              {
                                TOK.data[0] = '\\';  
                                TOK.data[1] = '\0';                                 
                                alokace = 3; 
//                                *TOK.data = '\\' + '\0';
                                TOK.typ = RETEZEC;
                                poprve = 1;
                                TOK.data=(char *)gRealloc(TOK.data, alokace * sizeof(char));
                                if(TOK.data == NULL) 
                                  {
                                    TOK.typ = CHBA;
                                    return TOK;  
                                  }  
                                TOK.data[alokace-2] = 'x';  
                                TOK.data[alokace-1] = '\0'; 
                                alokace++;      
                                TOK.data=(char *)gRealloc(TOK.data, alokace * sizeof(char));
                                if(TOK.data == NULL) 
                                  {
                                    TOK.typ = CHBA;
                                    return TOK;  
                                  }                                        
                                TOK.data[alokace-2] = nactu;  
                                TOK.data[alokace-1] = '\0'; 
                                alokace++;                  
                                nactu = fgetc(vstup);                                   
                              }
                            else
                              {
                                TOK.data=(char *)gRealloc(TOK.data, alokace * sizeof(char));
                                if(TOK.data == NULL) 
                                  {
                                    TOK.typ = CHBA;
                                    return TOK;  
                                  }  
                                TOK.data[alokace-2] = '\\';  
                                TOK.data[alokace-1] = '\0'; 
                                alokace++;      
                                TOK.data=(char *)gRealloc(TOK.data, alokace * sizeof(char));
                                if(TOK.data == NULL) 
                                  {
                                    TOK.typ = CHBA;
                                    return TOK;  
                                  }  
                                TOK.data[alokace-2] = 'x';  
                                TOK.data[alokace-1] = '\0'; 
                                alokace++;                   
                                TOK.data=(char *)gRealloc(TOK.data, alokace * sizeof(char));
                                if(TOK.data == NULL) 
                                  {
                                    TOK.typ = CHBA;
                                    return TOK;  
                                  }                                        
                                TOK.data[alokace-2] = nactu;  
                                TOK.data[alokace-1] = '\0'; 
                                alokace++;                  
                                nactu = fgetc(vstup);                                                                  
                              }                                                                
                          }                                         
                      }                                               
                  }
                else  
                  {   
                    if (poprve == 0)
                      {
                        TOK.data[0] = nactu;  
                        TOK.data[1] = '\0';                       
                        alokace = 3; 
//                        *TOK.data = nactu + '\0';
                        TOK.typ = RETEZEC;
                        poprve = 1;
                        nactu = fgetc(vstup); 
                      } 
                    else
                      {     
                        TOK.data=(char *)gRealloc(TOK.data, alokace * sizeof(char));
                        if(TOK.data == NULL) 
                          {
                            TOK.typ = CHBA;
                            return TOK;  
                          }                       
                        TOK.data[alokace-2] = nactu;  
                        TOK.data[alokace-1] = '\0'; 
                        alokace++;  
                        nactu = fgetc(vstup);                     
                      } 
                  }
              }
            if (nactu == '\n')
              {
                TOK.typ = LEX_ERROR;
                return TOK;             
              }
          }
        return TOK;               
      }      
                   
   else
     {
       TOK.typ = LEX_ERROR;
       return TOK; 
     } 
   return TOK;           
  }
