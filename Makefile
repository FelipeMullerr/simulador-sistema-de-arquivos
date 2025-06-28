CC=gcc
CFLAGS=-Wall -Wextra -std=c99
TARGET=sistemaArquivos
OBJS=main.o arvoreBinaria.o comandos.o blocos.o

$(TARGET): $(OBJS)
    $(CC) $(OBJS) -o $(TARGET)

main.o: main.c comandos.h blocos.h
    $(CC) $(CFLAGS) -c main.c

arvoreBinaria.o: arvoreBinaria.c arvoreBinaria.h estruturas.h enums.h
    $(CC) $(CFLAGS) -c arvoreBinaria.c

comandos.o: comandos.c comandos.h arvoreBinaria.h blocos.h
    $(CC) $(CFLAGS) -c comandos.c

blocos.o: blocos.c blocos.h enums.h
    $(CC) $(CFLAGS) -c blocos.c

clean:
    rm -f *.o $(TARGET)

.PHONY: clean