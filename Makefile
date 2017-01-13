CC     = suncc
OPT    = -g -fast -lm -xopenmp
WARN   =
CFLAGS = $(OPT) $(WARN)
OBJECTS = poisson.o

poisson : $(OBJECTS)
	$(CC) $(CFLAGS) -o poisson $(OBJECTS)

clean :
	@rm -f *.o core
realclean : clean
	@rm -f poisson
poisson.o: poisson.c
