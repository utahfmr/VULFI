LTX = pdflatex

all:
	$(LTX) vulfi
	bibtex vulfi
	$(LTX) vulfi
	$(LTX) vulfi
	#~ dvipdfm vulfi.dvi
	rm -f *.aux *.log *.toc *.dvi *.bbl *.blg *.backup
	
clean:
	rm -f *.aux *.log *.toc *.dvi *.bbl *.blg *.idx *.out *~ *.backup
