OPTFLAGS=-c -o
CFLAGS=-ansi
OPENDSLIBS=-L/usr/local/lib -lldap -llber
LIBS= $(OPENDSLIBS) -lsocket -lnsl -lmemcached

HTTPServer: configuration.o LdapWrapper-Solaris.o Logger.o util.o Article.o LdapWrapperHandler.o httpServer.o  funcionesMemcached.o
	gcc $(CFLAGS) $(LIBS) -o HTTPServer ./configuration.o ../util.o ../LDAP/LdapWrapper-Solaris.o ../Logger/Logger.o ../Article.o ../LDAP/LdapWrapperHandler.o ./httpServer.o ./funcionesMemcached.o
	@echo '* Finished building target: $@'
	@echo ''
#######################################################################################

configuration.o: configuration.c configuration.h
	gcc $(CFLAGS) $(OPTFLAGS) configuration.o configuration.c
#######################################################################################

funcionesMemcached.o: funcionesMemcached.c funcionesMemcached.h
	gcc $(CFLAGS) $(OPTFLAGS) funcionesMemcached.o funcionesMemcached.c
#######################################################################################

LdapWrapper-Solaris.o: ../LDAP/LdapWrapper-Solaris.c ../LDAP/LdapWrapper.h
	gcc $(CFLAGS) $(OPTFLAGS) ../LDAP/LdapWrapper-Solaris.o ../LDAP/LdapWrapper-Solaris.c
#######################################################################################

Logger.o: ../Logger/logger-Solaris.c ../Logger/logger.h
	gcc $(CFLAGS) $(OPTFLAGS) ../Logger/Logger.o ../Logger/logger-Solaris.c
#######################################################################################

Article.o: ../Article.c ../Article.h
	gcc $(CFLAGS) $(OPTFLAGS) ../Article.o ../Article.c
#######################################################################################

LdapWrapperHandler.o: ../LDAP/LdapWrapperHandler.c ../LDAP/LdapWrapperHandler.h
	gcc $(CFLAGS) $(OPTFLAGS) ../LDAP/LdapWrapperHandler.o ../LDAP/LdapWrapperHandler.c
#######################################################################################

httpServer.o: httpServer.c 
	gcc $(CFLAGS) $(OPTFLAGS) httpServer.o httpServer.c
#######################################################################################

util.o: ../util.c ../util.h
	gcc $(CFLAGS) $(OPTFLAGS) ../util.o ../util.c

#	-------------------------------------------------------
#	Nota:	Podemos ejecutar "make clean" y esto elimina todos los *.o para poder generarlos
#			nuevamente cuando nos dice ya estan compilados con la ultima version.
clean:
	rm *.o
	@echo '* Se borraron todos los archivos ".o" (object).'
	@echo ''
		