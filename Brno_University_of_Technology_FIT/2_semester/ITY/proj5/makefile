NAME=proj5

$(NAME).pdf: $(NAME).ps
	ps2pdf $(NAME).ps
    
$(NAME).ps: $(NAME).dvi
	dvips $(NAME).dvi
  
$(NAME).dvi: $(NAME).tex
	latex $(NAME).tex
	latex $(NAME).tex

