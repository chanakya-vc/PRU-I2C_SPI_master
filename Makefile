# * Makefile for PRU firmware
# * Builds only with CLPRU 2.0.0B* and above

# Tools to be used
CC = clpru
LD = lnkpru

# Default installation location for TI code generation tools (TI-CGT PRU) is
# in your home directory (if you're sane).
# Change if you have installed at some other location
CGTDIR ?= $(HOME)/ti-cgt-pru_2.1.2

INCLUDEDIR = -I$(CGTDIR)/include -I$(CGTDIR)/lib -I./include

# Compiler Options
# -v3				PRU version 3
# --c99 			C99 support
# --gcc 			Enable GCC extensions
# -O3				Optimization level maximum
# --printf_support=minimal 	Minimal printf
# -ppd				Generate dependencies *.pp
# -ppa				Continue after generating deps
# -DDEBUG			Enable debug
CFLAGS= -v3 -s -al -O3 --c99 --gcc --printf_support=minimal --symdebug:none $(INCLUDEDIR) -ppd -ppa

# Linker Options
# -cr 				Link using RAM auto init model (loader assisted)
# -x				Reread libs until no unresolved symbols found
LDFLAGS=-cr --diag_warning=225 -lam335x_pru.cmd -x -i$(CGTDIR)/lib

pru0_spi: pru0_spi.obj
	$(LD) $(LDFLAGS) $^ -o $@

%.obj: %.c
	$(CC) $(CFLAGS) -c $< -ea=.s

.PHONY: clean
clean:
	rm -f *.lst *.obj *.pp *.s pru0_spi

-include $(patsubst %.obj,%.pp,$(OBJS))
