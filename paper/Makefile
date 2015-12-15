TARGET=ritchie-proposal
FIGS=

%.pdf: %.tex
	pdflatex $<
	pdflatex $<
	bibtex $*
	pdflatex $<
	pdflatex $<

%.pdf: %.dot
	neato -Gstart=6 -Tpdf < $< > $@
	mv $@ bak.$@
	pdfcrop bak.$@ $@

%.png: %.dot
	neato -Gstart=6 -Tpng < $< > $@

all: $(TARGET).pdf

$(TARGET).pdf: $(FIGS) $(TARGET).tex

figs: $(FIGS)

preview: $(TARGET).pdf
	evince $(TARGET).pdf

clean: cleantex cleanfigs

cleanfigs:
	rm -fr $(FIGS)

cleantex:
	rm -f $(TARGET).bbl $(TARGET).blg $(TARGET).log $(TARGET).out *.aux
