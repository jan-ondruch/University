IJA projekt 2016
FIT VUT

Projekt: Reversi game
Autori: Andrea Osterova - xoster00
	Jan Ondruch - xondru14

Spusteni: v root adresari projektu ...

	  ant compile
	  ant run

Dokumentace: automaticky generovana

Popis hry
Specifikace požadavků

    aplikace bude implementovat základní pravidla hry, která jsou uvedena zde
    rozšíření pravidel
        možnost zvolit velikost hrací desky (6, 8, 10, 12); implicitní hodnota je 8
        možnost zvolit "zamrzání" kamenů
            pokud je zvoleno, nastaví se časové intervaly I a B (v sekundách) a počet kamenů C
            po uplynutí náhodně vygenerované doby z časového intervalu (0,I) se zablokuje C náhodně vybraných kamenů na náhodně vygenerovanou dobu z intervalu (0,B)
            kameny se odblokují po uplynutí doby blokování, ale až po ukončení aktuálního tahu
            zablokované kameny nelze použít pro tvorbu řad (tj. zajímání soupeřových kamenů)
    aplikace bude umožňovat
        vytvořit a hrát více her současně
        uložit a načíst rozehranou partii
        operaci undo (historii tahů není třeba ukládat)
        zvolit soupeře
            člověk nebo počítač
            v případě počítače implementujte dva různé (triviální !) algoritmy; před hrou se zvolí konkrétní algoritmus
            návrh aplikace musí umožňovat jednoduše přidávat další algoritmy
    součástí aplikace bude grafické uživatelské rozhraní zobrazující
        hrací desku
        hrací kameny, které má hráč k dispozici
        hrací kameny, které jsou vloženy na desku
        výsledné skóre po ukončení hry

