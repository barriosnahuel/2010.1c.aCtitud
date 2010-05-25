OPTFLAGS=-c -o
CFLAGS=-ansi
OPENDSLIBS=-L/usr/local/lib -lldap -llber
LIBS= $(OPENDSLIBS) -lssl

NNTPServerBam: logger-Debian-C.o configuration.o conexion.o util.o funciones.o Article.o LdapWrapper-Debian.o LdapWrapperHandler.o nntpServerBam.o
	gcc $(CFLAGS) $(LIBS) -o NNTPServerBam ../Logger/logger-Debian-C.o ../Article.o ../LDAP/LdapWrapper-Debian.o ../LDAP/LdapWrapperHandler.o ../util.o ./configuration.o ./conexion.o ./funciones.o ./nntpServerBam.o
	@echo '* Finished building target: $@'
	@echo ''

Article.o: ../Article.c ../Article.h
	gcc $(CFLAGS) $(OPTFLAGS) ../Article.o ../Article.c	

conexion.o: conexion.c conexion.h
	gcc $(CFLAGS) $(OPTFLAGS) conexion.o conexion.c

configuration.o: configuration.c configuration.h ../Logger/logger.h
	gcc $(CFLAGS) $(OPTFLAGS) configuration.o configuration.c

funciones.o: funciones.c funciones.h
	gcc $(CFLAGS) $(OPTFLAGS) funciones.o funciones.c

logger-Debian-C.o: ../Logger/logger-Debian.c ../Logger/logger.h
	gcc $(CFLAGS) $(OPTFLAGS) ../Logger/logger-Debian-C.o ../Logger/logger-Debian.c

LdapWrapper-Debian.o: ../LDAP/LdapWrapper-Debian.c ../LDAP/LdapWrapper.h
	gcc $(CFLAGS) $(OPTFLAGS) ../LDAP/LdapWrapper-Debian.o ../LDAP/LdapWrapper-Debian.c

LdapWrapperHandler.o: ../LDAP/LdapWrapperHandler.c ../LDAP/LdapWrapperHandler.h
	gcc $(CFLAGS) $(OPTFLAGS) ../LDAP/LdapWrapperHandler.o ../LDAP/LdapWrapperHandler.c

nntpServerBam.o: nntpServerBam.c
	gcc $(OPTFLAGS) nntpServerBam.o nntpServerBam.c

util.o: ../util.c ../util.h
	gcc $(CFLAGS) $(OPTFLAGS) ../util.o ../util.c

clean:
	rm *.o
	@echo '* Se borraron todos los archivos ".o" (object).'
	@echo ''
