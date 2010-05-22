OPTFLAGS=-c -o
CFLAGS=-ansi
OPENDSLIBS=-L/usr/local/lib -lldap -llber
LIBS= $(OPENDSLIBS) -lsocket -lnsl -lmemcached

HTTPServer: configuration.o LdapWrapper.o Logger.o Article.o LdapWrapperHandler.o httpServer.o funcionesMemcached.o
	gcc $(CFLAGS) $(LIBS) -o HTTPServer ./configuration.o ./LdapWrapper.o ../Logger/Logger.o ./Article.o ./LdapWrapperHandler.o ./httpServer.o ./funcionesMemcached.o
	@echo '* Finished building target: $@'
	@echo ''

configuration.o: configuration.c configuration.h
	gcc $(CFLAGS) $(OPTFLAGS) configuration.o configuration.c

LdapWrapper.o: LdapWrapper.c LdapWrapper.h
	gcc $(CFLAGS) $(OPTFLAGS) LdapWrapper.o LdapWrapper.c

#	Esto todavia no se esta usando, hay que hacer el logger para Solaris.
#	En el target HTTPServer hay que poner Logger.o y en gcc ../Logger/Logger.o
Logger.o: ../Logger/logger-Solaris.c ../Logger/logger.h
	gcc $(CFLAGS) $(OPTFLAGS) ../Logger/Logger.o ../Logger/logger-Solaris.c

Article.o: Article.c Article.h
	gcc $(CFLAGS) $(OPTFLAGS) Article.o Article.c

LdapWrapperHandler.o: LdapWrapperHandler.c LdapWrapperHandler.h
	gcc $(CFLAGS) $(OPTFLAGS) LdapWrapperHandler.o LdapWrapperHandler.c

httpServer.o: httpServer.c 
	gcc $(CFLAGS) $(OPTFLAGS) httpServer.o httpServer.c

funcionesMemcached.o: funcionesMemcached.c funcionesMemcached.h
	gcc $(CFLAGS) $(OPTFLAGS) funcionesMemcached.o funcionesMemcached.c

#	-------------------------------------------------------
#	Nota:	Podemos ejecutar "make clean" y esto elimina todos los *.o para poder generarlos
#			nuevamente cuando nos dice ya estan compilados con la ultima version.
clean:
	rm *.o
	@echo '* Se borraron todos los archivos ".o" (object).'
	@echo ''
