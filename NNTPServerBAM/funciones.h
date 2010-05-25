#ifndef _FUNCIONES_H
#define _FUNCIONES_H

#include "../LDAP/LdapWrapperHandler.h"
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

char* processBodyCommand(  char** sResponse
							, PLDAP_SESSION stPLDAPSession
							, PLDAP_SESSION_OP stPLDAPSessionOperations
							, char* sParametroDelComando);

char* processHeadCommand(  char** sResponse
							, PLDAP_SESSION stPLDAPSession
							, PLDAP_SESSION_OP stPLDAPSessionOperations
							, char* sParametroDelComando);

/**
 * Realizo el proceso de ir a buscar a la BD el listado de grupos de noticias.
 * Arma el correspondiente String (en sResponse) para enviarlo al cliente.
 */
char* processListNewsgroupsCommand(	  char**			psResponse
									, PLDAP_SESSION 	stPLDAPSession
									, PLDAP_SESSION_OP 	stPLDAPSessionOperations);

/**
 * Realiza el proceso de ir a buscar a la BD el listado de noticias para un grupo dado y
 * armar el correspondiente String (en sResponse) para enviarlo al cliente.
 * Nota: El parametro sComandoRecibido sera de la forma "COMANDO parametro", a pesar de que el usuario haya
 * escrito en el comando al cliente "comando         parametro". (Porque asi lo hacemos en el cliente).
 */
char* processListGroupCommand(  char** 				psResponse
								, PLDAP_SESSION 	stPLDAPSession
								, PLDAP_SESSION_OP 	stPLDAPSessionOperations
								, char* 			sGrupoDeNoticias);

/**
 * En base al response code (del protocolo NNTP, RFC 3977) obtiene un char*
 * con el mensaje correspondiente. Esta funcion la usamos para los mensajes
 * constantes como el 430 - no article with that message-id.
 */
char* getMessageForResponseCode(unsigned int uiResponseCode);

#endif
