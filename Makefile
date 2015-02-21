what : what.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o what what.o

what.o : what.c
	$(CC) $(CFLAGS) -c what.c

clean :
	$(RM) what *.o tmp.dat

install :
	install what /usr/local/bin
