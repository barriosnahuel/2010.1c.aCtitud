OPTFLAGS=-c -o
CFLAGS=-ansi
OPENDSLIBS=-L/usr/local/lib -lldap -llber
LIBS= $(OPENDSLIBS) -lsocket -lnsl

HTTPServer: configuration.o LdapWrapper.o httpServer.o 
	gcc $(CFLAGS) $(LIBS) -o HTTPServer ./configuration.o ./LdapWrapper.o ./httpServer.o
	@echo '* Finished building target: $@'
	@echo ''

configuration.o: configuration.c configuration.h
	gcc $(CFLAGS) $(OPTFLAGS) configuration.o configuration.c

LdapWrapper.o: LdapWrapper.c LdapWrapper.h
	gcc $(CFLAGS) $(OPTFLAGS) LdapWrapper.o LdapWrapper.c

httpServer.o: httpServer.c 
	gcc $(CFLAGS) $(OPTFLAGS) httpServer.o httpServer.c


#	-------------------------------------------------------
#	Nota:	Podemos ejecutar "make clean" y esto elimina todos los *.o para poder generarlos
#			nuevamente cuando nos dice ya estan compilados con la ultima version.
clean:
	rm *.o
	@echo '* Se borraron todos los archivos ".o" (object).'
	@echo ''
		