OPTFLAGS=-g
CFLAGS=-ansi

HTTPServer: LdapWrapper.o httpServer.o
	gcc $(CFLAGS) -lsocket -lnsl -o HTTPServer ./LdapWrapper.o ./httpServer.o
	@echo '* Finished building target: $@'
	@echo ''

LdapWrapper.o: LdapWrapper.c
	gcc $(CFLAGS) -c -o LdapWrapper.o LdapWrapper.c

httpServer.o: httpServer.c 
	gcc $(CFLAGS) -c -o httpServer.o httpServer.c


#	-------------------------------------------------------
#	Nota:	Podemos ejecutar "make clean" y esto elimina todos los *.o para poder generarlos
#			nuevamente cuando nos dice ya estan compilados con la ultima version.
clean:
	rm *.o
	@echo '* Se borraron todos los archivos ".o" (object).'
	@echo ''
		