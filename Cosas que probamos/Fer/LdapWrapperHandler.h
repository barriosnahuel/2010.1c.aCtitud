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

char* getDNFor(int dArticleID);

/**
 * Selecciona un conjunto de entries en base al criterio que se le pasa.
 */
VOID selectEntries(PLDAP_SESSION			stPLDAPSession
					, PLDAP_SESSION_OP		stPLDAPSessionOperations
					, char* 				sCriterio);

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
