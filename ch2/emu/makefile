.PHONY: default run

default: helloworld.bin px86.o

helloworld.bin: helloworld.asm makefile
	nasm -f bin $< -o $@

main.o: main.c makefile
	gcc -Wall -c -o $@ $<

px86.o: main.o makefile
	gcc -o $@ $<

run: helloworld.bin px86.o makefile
	./px86.o helloworld.bin
