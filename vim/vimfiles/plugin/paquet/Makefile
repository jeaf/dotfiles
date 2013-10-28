CC     = tcc
CFLAGS = -Wall
RM     = rm.exe

EXEC = paquet.exe
OBJS = miniz.o paquet.o

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $+

miniz.o: miniz.c miniz.h
paquet.o: paquet.c miniz.h

.PHONY: clean
clean:
	$(RM) -f $(EXEC) $(OBJS)

