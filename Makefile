CC:=gcc
CFLAGS:=-g -o test
SOURCE:=test.c

default: debug

debug:
	$(CC) $(CFLAGS) $(SOURCE)

arm: CC:=arm-linux-gnueabi-gcc-11
arm: CFLAGS+=-static
arm:
	$(CC) $(CFLAGS) $(SOURCE)

ppc: CC:=powerpc-linux-gnu-gcc-9
ppc: CFLAGS+=-static
ppc:
	$(CC) $(CFLAGS) $(SOURCE)
