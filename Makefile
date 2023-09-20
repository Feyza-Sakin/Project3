# Makefile for the CMSC257 Project2
#

my257sh:
	gcc	-g	-Wall	-pthread	shellex.c	csapp.c	-o	sh257

clean:
	rm -v sh257
