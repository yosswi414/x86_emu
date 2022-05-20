.PHONY: default

default: lea.lst

lea.lst: lea.c makefile
	gcc -c -g -o lea.o $<
	objdump -d -S -M intel lea.o > $@
