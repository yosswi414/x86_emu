.PHONY: default

default: casm-c-sample.bin casm-c-sample.lst 2-cmpl.o

casm-c-sample.bin: casm-c-sample.c makefile
	gcc -Wl,--entry=func,--oformat=binary -nostdlib -fno-asynchronous-unwind-tables -o $@ $<

casm-c-sample.lst: casm-c-sample.c makefile
	gcc -c -g -o casm-c-sample.o $<
	# objdump
	# -d : display the assembler mnemonics
	# -s : display the full contents (including C code)
	# -M intel : select intel syntax mode (at&t mode: "-M att")
	objdump -d -S -M intel casm-c-sample.o > $@

2-cmpl.o: 2-cmpl.c makefile
	gcc -o $@ $<
