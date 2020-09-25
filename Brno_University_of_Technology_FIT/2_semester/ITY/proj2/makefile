#
# Projekt: proj2.tex
# Autor:   Jan Ondruch (xondru14)
# Datum:   22.3.2015
# LaTeX Makefile

FILE=proj2

all: $(FILE).pdf

$(FILE).pdf: 
	latex $(FILE).tex
	latex $(FILE).tex
	dvips -t a4 $(FILE).dvi
	ps2pdf -sPAPERSIZE=a4 $(FILE).ps
