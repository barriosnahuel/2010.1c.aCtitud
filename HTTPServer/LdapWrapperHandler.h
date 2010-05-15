/*
 * LdapWrapperHandler.h
 *
 *  Created on: May 7, 2010
 *      Author: Barrios, Nahuel.
 */

#ifndef LDAPWRAPPERHANDLER_H_
#define LDAPWRAPPERHANDLER_H_

#include "LdapWrapper.h"
#include "Article.h"

/*	Estos dos atributos los defino aca, porque necesito tener acceso
 *  desde el httpServer.c para poder buscar lo que necesito.
 */
#define OPENDS_ATTRIBUTE_ARTICLE_ID "utnArticleID"
#define OPENDS_ATTRIBUTE_ARTICLE_ID_MAX_LENGHT 11
#define OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME "utnArticleGroupName"
#define OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME_MAX_LENGHT 25

#define OPENDS_SELECT_GRUPO_DE_NOTICIA 1
#define OPENDS_SELECT_BODY_Y_HEAD 2

char* getDNFor(int dArticleID);

stArticle getArticle( PLDAP_SESSION 		stPLDAPSession
					, PLDAP_SESSION_OP 		stPLDAPSessionOperations
					, char* 				sGrupoDeNoticias
					, char* 				sArticleID);

/**
 * Selecciona un conjunto de entries en base al criterio que se le pasa.
 */
VOID selectEntries(	  char*					pczListado[]
                   	, unsigned int*			puiCantidadEntries
					, PLDAP_SESSION 		stPLDAPSession
					, PLDAP_SESSION_OP 		stPLDAPSessionOperations
					, char* 				sCriterio
					, unsigned int			uiTipoDeSelect);

/**
 * Insertar una nueva entry en base a los atributos de la estructura stArticle que le pasamos.
 */
VOID insertEntry(PLDAP_SESSION			session
				, PLDAP_SESSION_OP		sessionOp
				, PLDAP_ENTRY_OP		entryOp
				, PLDAP_ATTRIBUTE_OP	attribOp
				, stArticle 			article);

/**
 * Esta funcion modifica el articulo de la base que se le pasa como parametro con sus atributos ya modificados.
 * Es decir, actualiza la entry con el articulo que le damos.
 */
VOID updateEntry(PLDAP_SESSION			session
				, PLDAP_SESSION_OP		sessionOp
				, PLDAP_ENTRY_OP		entryOp
				, PLDAP_ATTRIBUTE_OP	attribOp
				, stArticle 			article);

/**
 * Eliminar una entrada existente a partir del ID del articulo.
 */
VOID deleteEntry(PLDAP_SESSION		session
			, PLDAP_SESSION_OP		sessionOp
			, PLDAP_ENTRY_OP		entryOp
			, unsigned int 			uiArticleID);


#endif /* LDAPWRAPPERHANDLER_H_ */
