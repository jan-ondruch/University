/* ******************************** ial.c *************************************/
/* Soubor:              ial.c - Funkce pre algoritmy						  */
/* Kodovani:            UTF-8                                                 */
/* Datum:               13. 12. 2015                                          */
/*																			  */
/* Predmet:             Formalni jazyky a prekladace (IFJ)                    */
/*						Algoritmy (IAL)										  */
/* Projekt:             Implementace interpretu jazyka IFJ15                  */
/* Varianta zadani:     a/1/I                                                 */
/*																			  */
/* Autori, login:   	Tereza Kabeláčová     		  xkabel06                */
/*                      Adriana Jelenčíková  		  xjelen10                */
/*				        Martin Auxt   			      xauxtm00                */
/*                      Jan Ondruch 		          xondru14                */
/*						Michal Klhůfek			      xklhuf00                */
/* ****************************************************************************/

#include "ial.h"
#include "garbage.h"
#include "main.h"
#include "parser.h"

/* inicializace stromu*/
void BSInit(bStromPtr *root){
    *root = NULL;
}

/* vlozeni noveho prvku do stromu*/
int BSInsert(bStromPtr *uzol, char* key, TSymbol symbol){

int res;
int err;

if(*uzol  == NULL){

	*uzol = gMalloc(sizeof(struct bStrom));
		if(*uzol == NULL){
			return ERR_CODE_OTH;
		}

	(*uzol)->data = gMalloc(sizeof(struct symbol));
		if((*uzol)->data == NULL){
			return ERR_CODE_OTH;
		}
	
	(*uzol)->data->name = (char *)gMalloc(sizeof(char)*1024);
		if((*uzol)->data->name == NULL){
				return ERR_CODE_OTH;
		} 	

	(*uzol)->key = (char *)gMalloc(sizeof(char) * 1024);
		if((*uzol)->key == NULL){
			return ERR_CODE_OTH;
		}

		(*uzol)->lptr = NULL;
		(*uzol)->rptr = NULL;
		(*uzol)->key = key;

		(*uzol)->data->type = symbol->type;
		(*uzol)->data->name = symbol->name;
		(*uzol)->data->Local = symbol->Local;
		(*uzol)->data->pocet_params = symbol->pocet_params;
		(*uzol)->data->isDeclared = symbol->isDeclared;
		(*uzol)->data->isDefined = symbol->isDefined;
		(*uzol)->data->isInit = symbol->isInit;
		(*uzol)->data->isFunction = symbol->isFunction;   
		(*uzol)->data->h = symbol->h;
		(*uzol)->data->Higher = symbol->Higher;
		(*uzol)->data->vrstva = symbol->vrstva;
		(*uzol)->data->Higher = symbol->Higher;

		if ((*uzol)->data->isFunction) {
				(*uzol)->data->arg = gMalloc(sizeof(char *) * (symbol->alloc_pam));
			for(int l = 0; l < symbol->pocet_params * 2;l++){
				(*uzol)->data->arg[l] = gMalloc(sizeof(char*) * ((strlen(symbol->name)) + 1));
				(*uzol)->data->arg[l] = symbol->arg[l];
			}
		}
		if(symbol->isFunction){
			err = kontrola((*uzol), symbol);

			if(err != ERR_CODE_SUCC) return ERR_CODE_DEF;
			
		}
}
else{
res= strcmp( (*uzol)->key, key );

	if(res < 0){
		 BSInsert(&((*uzol)->rptr),key, symbol);	
	}
	else if(res > 0){
		 BSInsert(&((*uzol)->lptr),key, symbol);
	}
	else{

		if(symbol->isFunction &&  symbol->isDefined){
				BTR_LOC = (*uzol)->data->Local;
		}

		if(symbol->isFunction){
			if(symbol->pocet_params != (*uzol)->data->pocet_params)	return ERR_CODE_COM;
			for(int i = 0; i < symbol->pocet_params*2; i++){
				if(strcmp(symbol->arg[i], (*uzol)->data->arg[i]) != 0) return ERR_CODE_COM;
			}
		}
		return ERR_CODE_SUCC;
	}

}

return ERR_CODE_SUCC;
}


/* kontrola, jestli souhlasi formalni parametry funkce a jestli jsou funkce spravne deklarovany a definovany*/
int kontrola(bStromPtr uzol, TSymbol symbol){
	bStromPtr tmp;
	TSymbolPtr SYMB;

	tmp = gMalloc(sizeof(struct bStrom));
	if((tmp = BSearch(uzol, uzol->data->name)) != NULL){
		if(tmp->data->pocet_params == uzol->data->pocet_params){
			for(int l = 0; l < symbol->pocet_params*2;l++){
					if(l % 2 == 0){
						if(strcmp(uzol->data->arg[l],"int")==0){
							SYMB.type = 8;
						}
						else if(strcmp(uzol->data->arg[l], "double") == 0){
							SYMB.type = 4;
						}
						else if(strcmp(uzol->data->arg[l],"string")==0){
							SYMB.type = 10;
						}
					}					
					else{
						SYMB.name = gMalloc(sizeof(struct symbol));
						SYMB.isFunction = false;
						SYMB.isDeclared = true;
						SYMB.isInit = true;
						SYMB.Local = NULL;
						SYMB.Higher = uzol;
						SYMB.name = uzol->data->arg[l];

						if(!BSearch((uzol->data->Local), SYMB.name))
							BSInsert(&(uzol->data->Local),SYMB.name,&SYMB);
						else return ERR_CODE_DEF;
					}	

				if(strcmp(tmp->data->arg[l],symbol->arg[l]) != 0)
					{return ERR_CODE_DEF;}

			}
				
		}
		else return ERR_CODE_DEF;

	}
	return ERR_CODE_SUCC;
}


/* hledani uzlu ve stromu*/
bStromPtr BSearch(bStromPtr tree, char* val)
{


	if(!(tree))
	    {
	        return NULL;
	    }
	    int result = strcmp(tree->key, val);

	    if(result > 0)
	    {
	        BSearch((tree)->lptr, val);
	    }
	    else if(result < 0)
	    {
	        BSearch((tree)->rptr, val);
	    }
	    else if(result == 0)
	    {
	        return tree;
	    }
	    else 
	    {
	    	return NULL;
	 	}
}



/// VESTAVENE FUNKCE ////

int quickS( int a[], int l, int r) {
   int pivot, i, j, t;
   pivot = a[l];
   i = l; j = r+1;

   while( 1)
   {
   	do ++i; while( a[i] <= pivot && i <= r );
   	do --j; while( a[j] > pivot );
   	if( i >= j ) break;
   	t = a[i]; a[i] = a[j]; a[j] = t;
   }

   t = a[l]; a[l] = a[j]; a[j] = t;
   return j;
}

void quickSort( int a[], int l, int r)      //l pociatocne, r koniec
{
   int j;
   if( l < r )
   {
    j = quickS( a, l, r);
    quickSort( a, l, j-1);
    quickSort( a, j+1, r);
   }
}

char* sort(char *str){
    char *s,*tmp;
    tmp = gMalloc(sizeof(char) + strlen(str));
    int i,b[strlen(str)],l;
    s = str;
    for(i=0; i<strlen(str);i++){
      b[i] = *s;
      s++;
    }
    quickSort(b,0,i-1);
    for(l = 0; l < i; ++l){
      tmp[l]=b[l];
    }
    tmp[l] = '\0';
    return tmp;
}

int lenght(char *str){
    const char *s;
    for(s = str; *s;++s);
    return (s-str);                    //vrati dlzku stringu
}

char* concat(char *str1, char *str2){
    int newlen = strlen(str2);
    char *tmp, *tmp1;
    tmp = gMalloc(strlen(str1));
    tmp1 = gMalloc(strlen(str2));
    strcpy(tmp,str1);
    strcpy(tmp1,str2);
    if(lenght(str1) < lenght(str2))
    {
        if((tmp = (char*) gRealloc(tmp, newlen + (lenght(str2)))) == NULL)
          return NULL;
    }
    strcat(tmp, tmp1);
    return tmp;

}

int find(char *str, char *search){
    char *tmp, *tmp1, *tmp3;
    int i=0,j;
    int pocet=0;
    if(strcmp(search, "") == 0)        //ak bol zadany prazdny retazec okamzite vrati 0
      return 0;
    tmp = str;
    tmp1 = search;
    while(i < strlen(str)){
      if(*tmp == *tmp1){
        tmp3 = tmp;       //priradi v najdeny retazec
        for(j = 0; j < strlen(search); j++){
            if(*tmp3 == *tmp1){     //kontroluje ci je zhoda v obidvoch retazcoch
              tmp3++;               //odobere pismeno z najdeneho retazca
              tmp1++;               //odobere pismeno z hladaneho retazca
            }
            else{
              break;
            }
        }
        if(j == strlen(search)){
            pocet = i;
            return pocet;
        }
        else {
            pocet = -1;
            tmp1=search; //ked nenaslo zhodu s dalsim pismenom vrati naspet odobrate pismeno
        }
      }
      tmp++;    //odobere prve pismeno
      i++;
    }
    if(pocet == 0)
        pocet = -1;
    return pocet;
}

char* substr(char* str, int i, int n){
    char *tmp, *tmp1;
    tmp = gMalloc(n+1);
    tmp1 = gMalloc(strlen(str)+1);
    strcpy(tmp1,str);
    tmp1[i+n] = '\0';
    strcpy(tmp, &tmp1[i]);
    tmp[n+1] = '\0';
    free(tmp1);
    return tmp;
}