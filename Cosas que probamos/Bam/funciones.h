#ifndef _FUNCIONES_H
#define _FUNCIONES_H

#include "../LDAP/LdapWrapperHandler.h"
#include "../Logger/logger.h"
#include "../util.h"

void PantallaInicio(void);

void PantallaFin(void);

void centrar(char *);

void retardo(int iSegundos);

int SeleccionarComando(char *czComando);

/**
 * Realiza el proceso de ir a buscar un Articulo a la BD y armar el correspondiente String (en sResponse)
 * para enviar luego al cliente que solicito el articulo.
 * Nota: El parametro sComandoRecibido sera de la forma "COMANDO parametro", a pesar de que el usuario haya
 * escrito en el comando al cliente "comando         parametro". (Porque asi lo hacemos en el cliente).
 */
char* processArticleCommand(  char** sResponse
							, PLDAP_SESSION stPLDAPSession
							, PLDAP_SESSION_OP stPLDAPSessionOperations
							, char* sParametroDelComando);



#endif
