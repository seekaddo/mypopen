##
## @file Makefile
## mypopen GNU-like popen Library Makefile
## Beispiel 2
##
## @author Dennis Addo ic16b026@technikum-wien.at
## @author Robert Niedermayer ic16b089@technikum-wien.at
## @date 2017/03/24
##
## @version 1.o $
##
## @todo
##
## URL: $HeadURL: https://cis.technikum-wien.at/documents/bic/2/bes/semesterplan/lu/beispiel2.html $
##
## Last Modified: $Author: Dennis $
##

##
## ------------------------------------------------------------- variables --
##

CC=gcc52
CFLAGS=-DDEBUG -Wall -pedantic -Werror -Wextra -Wstrict-prototypes \
		-Wwrite-strings -fno-common -g -O3 -std=gnu11
CP=cp
CD=cd
MV=mv
GREP=grep
DOXYGEN=doxygen
PTEST=./popentest

OBJECTS=mypopen.o

EXCLUDE_PATTERN=footrulewidth

##
## ----------------------------------------------------------------- rules --
##

%.o: %.c
	$(CC) $(CFLAGS) -c $<

##
## --------------------------------------------------------------- targets --
##

.PHONY: all pptest buildntest
all: $(OBJECTS) /usr/local/lib/libpopentest.a
	$(CC) $^ -o popentest

pptest:
	./popentest


#quick and clean running of the popentest (build + test)
buildntest:
	make all && $(PTEST)


.PHONY: clean
clean:
	$(RM) *.o *~ popentest

.PHONY: doc distclean
distclean: clean
	$(RM) -r doc

doc: html pdf

.PHONY: html pdf
html:
	$(DOXYGEN) doxygen.dcf

pdf: html
	$(CD) doc/pdf && \
	$(MV) refman.tex refman_save.tex && \
	$(GREP) -v $(EXCLUDE_PATTERN) refman_save.tex > refman.tex && \
	$(RM) refman_save.tex && \
	make && \
	$(MV) refman.pdf refman.save && \
	$(RM) *.pdf *.html *.tex *.aux *.sty *.log *.eps *.out *.ind *.idx \
	      *.ilg *.toc *.tps Makefile && \
	$(MV) refman.save refman.pdf

##
## ---------------------------------------------------------- dependencies --
##

##
## =================================================================== eof ==
##
