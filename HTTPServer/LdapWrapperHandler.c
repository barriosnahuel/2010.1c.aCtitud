/*
 * LdapWrapperHandler.c
 *
 *  Created on: May 7, 2010
 *      Author: Barrios, Nahuel.
 */

#include "LdapWrapperHandler.h"

#define APP_NAME_FOR_LOGGER "HTTPServer"

/*	Constantes de OpenDS	*/
#define OPENDS_SCHEMA "ou=so,dn=utn,dn=edu"
#define OPENDS_ATTRIBUTE_OBJECT_CLASS "objectClass"
	#define OPENDS_ATTRIBUTE_OBJECT_CLASS_TOP "top"
	#define OPENDS_ATTRIBUTE_OBJECT_CLASS_ARTICLE "utnArticle"
#define OPENDS_ATTRIBUTE_ARTICLE_HEAD "utnArticleHead"
#define OPENDS_ATTRIBUTE_ARTICLE_BODY "utnArticleBody"

/**
 * Retorna un char* de la forma: "utnArticleID=12345,ou=so,dn=utn,dn=edu"
 */
char* getDNFor(int dArticleID){
	char *sDn;
	asprintf(&sDn, "%s=%d,%s", OPENDS_ATTRIBUTE_ARTICLE_ID, dArticleID, OPENDS_SCHEMA);

	return sDn;
}

VOID updateEntry(PLDAP_SESSION			session
				, PLDAP_SESSION_OP		sessionOp
				, PLDAP_ENTRY_OP		entryOp
				, PLDAP_ATTRIBUTE_OP	attribOp
				, stArticle				article) {

	PLDAP_ENTRY entry = entryOp->createEntry();

	entry->dn = getDNFor(article.uiArticleID);

	/* Agrego el atributo a la entry en modo delete */
	entryOp->editAttribute(entry, attribOp->createAttribute(OPENDS_ATTRIBUTE_ARTICLE_ID, 1, getArticleIDAsString(article)));
	entryOp->editAttribute(entry, attribOp->createAttribute(OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME, 1, article.sNewsgroup));
	entryOp->editAttribute(entry, attribOp->createAttribute(OPENDS_ATTRIBUTE_ARTICLE_HEAD, 1, article.sHead));
	entryOp->editAttribute(entry, attribOp->createAttribute(OPENDS_ATTRIBUTE_ARTICLE_BODY, 1, article.sBody));

	sessionOp->editEntry(session, entry);
}

VOID insertEntry(PLDAP_SESSION 			session
				, PLDAP_SESSION_OP 		sessionOp
				, PLDAP_ENTRY_OP 		entryOp
				, PLDAP_ATTRIBUTE_OP 	attribOp
				, stArticle 			article){

	/* Creo una nueva entry. */
	PLDAP_ENTRY entry = entryOp->createEntry();
	entry->dn = getDNFor(article.uiArticleID);

	/*	Seteo los atributos	*/
	entryOp->addAttribute(entry, attribOp->createAttribute(OPENDS_ATTRIBUTE_OBJECT_CLASS, 2, OPENDS_ATTRIBUTE_OBJECT_CLASS_TOP, OPENDS_ATTRIBUTE_OBJECT_CLASS_ARTICLE));
	entryOp->addAttribute(entry, attribOp->createAttribute(OPENDS_ATTRIBUTE_ARTICLE_ID, 1, getArticleIDAsString(article)));
	entryOp->addAttribute(entry, attribOp->createAttribute(OPENDS_ATTRIBUTE_ARTICLE_HEAD, 1, article.sHead));
	entryOp->addAttribute(entry, attribOp->createAttribute(OPENDS_ATTRIBUTE_ARTICLE_BODY, 1, article.sBody));
	entryOp->addAttribute(entry, attribOp->createAttribute(OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME, 1, article.sNewsgroup));

	/* inserto la entry en el directorio */
	sessionOp->addEntry(session, entry);
}

VOID deleteEntry(PLDAP_SESSION 			session
				, PLDAP_SESSION_OP 		sessionOp
				, PLDAP_ENTRY_OP 		entryOp
				, unsigned int 			uiArticleID) {

	/* creo una nueva entry.
	le agrego los parametros correspondientes */
	PLDAP_ENTRY entry = entryOp->createEntry();

	sessionOp->deleteEntryDn(session, getDNFor(uiArticleID));

/*        entry->dn = getDNFor(uiArticleID);

	/* Se puede eliminar ena entry pasando un objeto entry como parametro
	sessionOp->deleteEntryObj(session, entry);*/
	/* O se puede eliminar una entry pasando el dn correspondiente */

}

stArticle getArticle( PLDAP_SESSION 		stPLDAPSession
					, PLDAP_SESSION_OP 		stPLDAPSessionOperations
					, char* 				sGrupoDeNoticias
					, char* 				sArticleID){
	LoguearDebugging("-->getArticle()", APP_NAME_FOR_LOGGER);
	/* hago una consulta en una determinada rama aplicando la siguiente condicion */

	unsigned int uiMaxNumberOfCharacters=
				strlen(OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME)
				+OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME_MAX_LENGHT
				+strlen(OPENDS_ATTRIBUTE_ARTICLE_ID)
				+OPENDS_ATTRIBUTE_ARTICLE_ID_MAX_LENGHT;
	char* sCriterio= (char*)malloc(sizeof(char)*(uiMaxNumberOfCharacters));
	memset(sCriterio, 0, uiMaxNumberOfCharacters);
	sprintf(sCriterio, "(&(%s= %s)(%s= %s))", OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME, sGrupoDeNoticias, OPENDS_ATTRIBUTE_ARTICLE_ID, sArticleID);

	printf("sCriterio vale: %s\n", sCriterio);

	PLDAP_RESULT_SET resultSet      = stPLDAPSessionOperations->searchEntry(stPLDAPSession, OPENDS_SCHEMA, sCriterio);
	PLDAP_ITERATOR iterator         = NULL;
	PLDAP_RECORD_OP recordOp        = newLDAPRecordOperations();

	stArticle stArticleToReturn;

	/* itero sobre los registros obtenidos a traves de un iterador que conoce la implementacion del recordset */
	iterator = resultSet->iterator;
	if(!(iterator->hasNext(resultSet)))
		printf("No hay ninguna entry que mostrar.\n");
	while(iterator->hasNext(resultSet)) {
		PLDAP_RECORD record = iterator->next(resultSet);

		/* Itero sobre los campos de cada uno de los record */
		while(recordOp->hasNextField(record)) {
			PLDAP_FIELD field = recordOp->nextField(record);

			if(strcmp(field->name, OPENDS_ATTRIBUTE_ARTICLE_BODY)==0)
				stArticleToReturn.sBody= (char*)field->valuesSize;
			else if(strcmp(field->name, OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME)==0)
				stArticleToReturn.sNewsgroup= (char*)field->valuesSize;
			else if(strcmp(field->name, OPENDS_ATTRIBUTE_ARTICLE_HEAD)==0)
				stArticleToReturn.sHead= (char*)field->valuesSize;
			else if(strcmp(field->name, OPENDS_ATTRIBUTE_ARTICLE_ID)==0)
				stArticleToReturn.uiArticleID= (int)field->valuesSize;

			/* se libera la memoria utilizada por el field si este ya no es necesario. */
			freeLDAPField(field);
		}
		/* libero los recursos consumidos por el record */
		freeLDAPRecord(record);
	}

	LoguearDebugging("<-- getArticle()", APP_NAME_FOR_LOGGER);
	return stArticleToReturn;
}

/**
 * Se realiza una consulta al directorio en una determinada rama. Para iterar sobre los resultados se utiliza un
 * patron Iterator que recorre cada una de las entries
 */
VOID selectEntries(	  PLDAP_SESSION 		stPLDAPSession
					, PLDAP_SESSION_OP 		stPLDAPSessionOperations
					, char* 				sCriterio){

	/* hago una consulta en una determinada rama aplicando la siguiente condicion */

	PLDAP_RESULT_SET resultSet      = stPLDAPSessionOperations->searchEntry(stPLDAPSession, OPENDS_SCHEMA, sCriterio);
	PLDAP_ITERATOR iterator         = NULL;
	PLDAP_RECORD_OP recordOp        = newLDAPRecordOperations();

	/* itero sobre los registros obtenidos a traves de un iterador que conoce la implementacion del recordset */
	iterator = resultSet->iterator;
	if(!(iterator->hasNext(resultSet)))
		printf("No hay ninguna entry que mostrar.\n");
	while(iterator->hasNext(resultSet)) {
		PLDAP_RECORD record = iterator->next(resultSet);
		printf("dn: %s\n", record->dn);

		/* Itero sobre los campos de cada uno de los record */
		while(recordOp->hasNextField(record)) {
			PLDAP_FIELD field = recordOp->nextField(record);
			INT     index = 0;

			printf("\tattribute: %s - values: %d\n", field->name, (int)field->valuesSize);

			for(; index < field->valuesSize; index++)
				printf("\t\tValue[%d]: %s\n", index, field->values[index]);

			/* se libera la memoria utilizada por el field si este ya no es necesario. */
			freeLDAPField(field);
		}
		/* libero los recursos consumidos por el record */
		freeLDAPRecord(record);
	}
	printf("\n--------------------------------------------------------------------------\n\n");

    /* libero los recursos */
    freeLDAPIterator(iterator);
    freeLDAPRecordOperations(recordOp);
}
