LDAPLIB=-lldap  -llber

LIBDIR=/usr/lib
LIBS=-L$(LIBDIR) $(LDAPLIB) $(EXTRALDFLAGS)

OPTFLAGS=-g
CFLAGS=$(OPTFLAGS) -ansi 

CC=gcc

PROGS=wrapp


all:$(PROGS)
	rm *.o
wrapp:Ldap.c
	$(CC) $(CFLAGS) -c  LdapWrapper.c -o LdapWrapper.o
	$(CC) $(CFLAGS) -c  Ldap.c -o Ldap.o
	$(CC)  Ldap.o LdapWrapper.o $(LIBS) -o Ldap
clean:
	rm $($PROG) *.o

