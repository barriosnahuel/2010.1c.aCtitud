/*
 * LdapWrapperHandler.h
 *
 *  Created on: May 7, 2010
 *      Author: Barrios, Nahuel.
 */

#ifndef LDAPWRAPPERHANDLER_H_
#define LDAPWRAPPERHANDLER_H_

#include "LdapWrapper.h"
#include "../Article.h"

/*	Estos dos atributos los defino aca, porque necesito tener acceso
 *  desde el httpServer.c para poder buscar lo que necesito.
 */
#define OPENDS_ATTRIBUTE_ARTICLE_ID "utnArticleID"
#define OPENDS_ATTRIBUTE_ARTICLE_ID_MAX_LENGHT 11
#define OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME "utnArticleGroupName"
#define OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME_MAX_LENGHT 25

#define OPENDS_SELECT_GRUPO_DE_NOTICIA 1
#define OPENDS_SELECT_BODY_Y_HEAD 2

/**
 * Se conecta a OpenDS/PLDAP y deja establecida la conexion para usarla con el LDAPWrapper
 */
int crearConexionLDAP(char* sIp, unsigned int sPort, PLDAP_CONTEXT* pstPLDAPContext,
		PLDAP_CONTEXT_OP* pstPLDAPContextOperations,
		PLDAP_SESSION* pstPLDAPSession,
		PLDAP_SESSION_OP* pstPLDAPSessionOperations);

char* getDNFor(int dArticleID);

/**
 * Busca en OpenDS un articulo en base a su PK, compuesta por el grupo de noticias
 * al que pertenece la noticia, y al ID de la noticia. El criterio que finalmente
 * arma para pasarle a LDAP es: (&(utnArticleID=999)(utnArticleGroupName=XXX)).
 * Si no se encuentra ningun articulo para el message-id especificado, entonces
 * se setea en -1 el articleID para una posterior verificacion (para decidir que
 * tipo de response se tiene que enviar al cliente).
 */
stArticle getArticle(PLDAP_SESSION stPLDAPSession,
		PLDAP_SESSION_OP stPLDAPSessionOperations, char* sGrupoDeNoticias,
		char* sArticleID);

/**
 * Selecciona un conjunto de entries en base al criterio que se le pasa.
 */
VOID selectEntries(char* pczListado[], unsigned int* puiCantidadEntries,
		PLDAP_SESSION stPLDAPSession,
		PLDAP_SESSION_OP stPLDAPSessionOperations, char* sCriterio,
		unsigned int uiTipoDeSelect);

/**
 * Selecciona un listado de articulos, en base al criterio que se le pasa
 * y los agrega uno a uno en pczListado[] el cual es parametro I/O y es un array de stArticle.
 */
VOID selectArticles(stArticle pczListado[], unsigned int* puiCantidadEntries,
		PLDAP_SESSION stPLDAPSession,
		PLDAP_SESSION_OP stPLDAPSessionOperations, char* sCriterio);

/****************************************************************************************
 *	De aca, para abajo, son funciones que actualmente no se usan mas que para testing.	*
 ****************************************************************************************/

/**
 * Insertar una nueva entry en base a los atributos de la estructura stArticle
 * que le pasamos como parametro.
 */
VOID insertEntry(PLDAP_SESSION stSession, PLDAP_SESSION_OP stSessionOperations,
		stArticle stArticulo);

/**
 * Selecciona aquellas entradas de la BD que matcheen con el criterio sCriterio que le
 * pasamos como parametro, y los imprime por stdout.
 */
VOID selectAndPrintEntries(PLDAP_SESSION stPLDAPSession,
		PLDAP_SESSION_OP stPLDAPSessionOperations, char* sCriterio);

/**
 * Esta funcion modifica el articulo de la base que se le pasa como parametro con sus atributos ya modificados.
 * Es decir, actualiza la entry con el articulo que le damos.
 */
VOID updateEntry(PLDAP_SESSION stSession, PLDAP_SESSION_OP stSessionOperations,
		stArticle stArticulo);

/**
 * Elimina una entrada existente (una noticia) a partir del ID del articulo.
 */
VOID deleteEntry(PLDAP_SESSION stSession, PLDAP_SESSION_OP stSessionOperations,
		unsigned int uiArticleID);

#endif /* LDAPWRAPPERHANDLER_H_ */
