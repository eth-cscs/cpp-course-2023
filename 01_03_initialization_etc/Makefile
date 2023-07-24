.PHONY = all pdf html
.DEFAULT_GOAL := all

SRCS := $(wildcard *.md)
PDFS := $(SRCS:%.md=%.pdf)
HTMLS := $(SRCS:%.md=%.html)
MARP = marp

%.pdf: %.md
	${MARP} --allow-local-files --theme ../slides-support/themes/cscs.css --pdf $< -o $@

%.html: %.md
	${MARP} --allow-local-files --theme ../slides-support/themes/cscs.css $< -o $@

pdf: ${PDFS}

html: ${HTMLS}

all: pdf html