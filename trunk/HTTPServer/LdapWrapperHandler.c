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

VOID updateEntry(PLDAP_SESSION			stSession
				, PLDAP_SESSION_OP		stSessionOperations
				, stArticle				stArticulo) {

	PLDAP_ENTRY_OP stEntryOperations= newLDAPEntryOperations();
	PLDAP_ATTRIBUTE_OP stAttributeOperations= newLDAPAttributeOperations();

	PLDAP_ENTRY entry = stEntryOperations->createEntry();

	entry->dn = getDNFor(stArticulo.uiArticleID);

	/* Agrego el atributo a la entry en modo delete */
	stEntryOperations->editAttribute(entry, stAttributeOperations->createAttribute(OPENDS_ATTRIBUTE_ARTICLE_ID, 1, getArticleIDAsString(stArticulo)));
	stEntryOperations->editAttribute(entry, stAttributeOperations->createAttribute(OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME, 1, stArticulo.sNewsgroup));
	stEntryOperations->editAttribute(entry, stAttributeOperations->createAttribute(OPENDS_ATTRIBUTE_ARTICLE_HEAD, 1, stArticulo.sHead));
	stEntryOperations->editAttribute(entry, stAttributeOperations->createAttribute(OPENDS_ATTRIBUTE_ARTICLE_BODY, 1, stArticulo.sBody));

	stSessionOperations->editEntry(stSession, entry);
}

VOID insertEntry(PLDAP_SESSION 			stSession
				, PLDAP_SESSION_OP 		stSessionOperations
				, stArticle 			stArticulo){

	PLDAP_ENTRY_OP stEntryOperations= newLDAPEntryOperations();
	PLDAP_ATTRIBUTE_OP stAttributeOperations= newLDAPAttributeOperations();

	/* Creo una nueva entry. */
	PLDAP_ENTRY entry = stEntryOperations->createEntry();
	entry->dn = getDNFor(stArticulo.uiArticleID);

	/*	Seteo los atributos	*/
	stEntryOperations->addAttribute(entry, stAttributeOperations->createAttribute(OPENDS_ATTRIBUTE_OBJECT_CLASS, 2, OPENDS_ATTRIBUTE_OBJECT_CLASS_TOP, OPENDS_ATTRIBUTE_OBJECT_CLASS_ARTICLE));
	stEntryOperations->addAttribute(entry, stAttributeOperations->createAttribute(OPENDS_ATTRIBUTE_ARTICLE_ID, 1, getArticleIDAsString(stArticulo)));
	stEntryOperations->addAttribute(entry, stAttributeOperations->createAttribute(OPENDS_ATTRIBUTE_ARTICLE_HEAD, 1, stArticulo.sHead));
	stEntryOperations->addAttribute(entry, stAttributeOperations->createAttribute(OPENDS_ATTRIBUTE_ARTICLE_BODY, 1, stArticulo.sBody));
	stEntryOperations->addAttribute(entry, stAttributeOperations->createAttribute(OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME, 1, stArticulo.sNewsgroup));

	/* inserto la entry en el directorio */
	stSessionOperations->addEntry(stSession, entry);
}

VOID deleteEntry(PLDAP_SESSION 			stSession
				, PLDAP_SESSION_OP 		stSessionOperations
				, unsigned int 			uiArticleID) {

	PLDAP_ENTRY_OP stEntryOperations= newLDAPEntryOperations();

	/* creo una nueva entry.
	le agrego los parametros correspondientes */
	PLDAP_ENTRY entry= stEntryOperations->createEntry();

	stSessionOperations->deleteEntryDn(stSession, getDNFor(uiArticleID));

/*        entry->dn = getDNFor(uiArticleID);

	/* Se puede eliminar ena entry pasando un objeto entry como parametro
	sessionOp->deleteEntryObj(session, entry);*/
	/* O se puede eliminar una entry pasando el dn correspondiente */

}

stArticle getArticle( PLDAP_SESSION 		stPLDAPSession
					, PLDAP_SESSION_OP 		stPLDAPSessionOperations
					, char* 				sGrupoDeNoticias
					, char* 				sArticleID
					, int ficheroCliente ){
	
	LoguearDebugging("--> getArticle()", APP_NAME_FOR_LOGGER);
	/* hago una consulta en una determinada rama aplicando la siguiente condicion */

	unsigned int uiMaxNumberOfCharacters=
				strlen(OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME)
				+OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME_MAX_LENGHT
				+strlen(OPENDS_ATTRIBUTE_ARTICLE_ID)
				+OPENDS_ATTRIBUTE_ARTICLE_ID_MAX_LENGHT;
	char* sCriterio= (char*)malloc(sizeof(char)*(uiMaxNumberOfCharacters));
	memset(sCriterio, 0, uiMaxNumberOfCharacters);
	sprintf(sCriterio, "(&(%s= %s)(%s= %s))", OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME, sGrupoDeNoticias, OPENDS_ATTRIBUTE_ARTICLE_ID, sArticleID);

	char* sDebugMessage;
	asprintf(&sDebugMessage, "El criterio por el que se busco en OpenDS es: %s", sCriterio);
	LoguearDebugging(sDebugMessage, APP_NAME_FOR_LOGGER);

	PLDAP_RESULT_SET resultSet      = stPLDAPSessionOperations->searchEntry(stPLDAPSession, OPENDS_SCHEMA, sCriterio);
	PLDAP_ITERATOR iterator         = NULL;
	PLDAP_RECORD_OP recordOp        = newLDAPRecordOperations();

	stArticle stArticleToReturn;

	/* itero sobre los registros obtenidos a traves de un iterador que conoce la implementacion del recordset */
	iterator = resultSet->iterator;
	if(iterator->hasNext(resultSet)) {
		LoguearDebugging("Se encontro un articulo con la condicion especificada.", APP_NAME_FOR_LOGGER);

		PLDAP_RECORD record = iterator->next(resultSet);

		/* Itero sobre los campos de cada uno de los record */
		while(recordOp->hasNextField(record)) {
			LoguearDebugging("Itero por un campo de un articulo.", APP_NAME_FOR_LOGGER);

			PLDAP_FIELD field = recordOp->nextField(record);
			if(strcmp(field->name, OPENDS_ATTRIBUTE_ARTICLE_BODY)==0)
				asprintf(&(stArticleToReturn.sBody), "%s", (char*)field->values[0]);
			else if(strcmp(field->name, OPENDS_ATTRIBUTE_ARTICLE_HEAD)==0)
				asprintf(&(stArticleToReturn.sHead), "%s", (char*)field->values[0]);

			/* se libera la memoria utilizada por el field si este ya no es necesario. */
			freeLDAPField(field);
		}
		/* libero los recursos consumidos por el record */
		freeLDAPRecord(record);

		asprintf(&(stArticleToReturn.sNewsgroup), "%s", sGrupoDeNoticias);
		stArticleToReturn.uiArticleID= atoi(sArticleID);
		LoguearInformacion("Se seteo bien el articulo recuperado de OpenDS.", APP_NAME_FOR_LOGGER);
	}
	else {
		char* cadenaProtocolo = (char*)malloc(sizeof(char)*300);
		int lenProtocolo;
		int bytesEnviadosProtocolo;
		printf("No hay ninguna entry que mostrar.\n");
		cadenaProtocolo = "HTTP/1.1 404 Not Found\nContent-type: text/html\n\n";
		lenProtocolo = strlen(cadenaProtocolo);
					
		if((bytesEnviadosProtocolo = send(ficheroCliente, cadenaProtocolo, lenProtocolo, 0)) == -1) {
			LoguearError("No se pudo enviar el 404 Not Found al cliente.", APP_NAME_FOR_LOGGER);
		}
	}
		

	LoguearDebugging("<-- getArticle()", APP_NAME_FOR_LOGGER);
	return stArticleToReturn;
}

/**
 * Se realiza una consulta al directorio en una determinada rama. Para iterar sobre los resultados se utiliza un
 * patron Iterator que recorre cada una de las entries.
 * El uiTipoDeSelect, se indica para establecer cuales atributos de una entry se quieren traer:
 * 		1:	sGrupoDeNoticia
 * 		2:	sBody, sHead
 */
VOID selectEntries(	  char*					pczListado[]
					, unsigned int*			puiCantidadEntries
					, PLDAP_SESSION 		stPLDAPSession
					, PLDAP_SESSION_OP 		stPLDAPSessionOperations
					, char* 				sCriterio
					, unsigned int			uiTipoDeSelect){

	/* hago una consulta en una determinada rama aplicando la siguiente condicion */

	PLDAP_RESULT_SET resultSet      = stPLDAPSessionOperations->searchEntry(stPLDAPSession, OPENDS_SCHEMA, sCriterio);
	PLDAP_ITERATOR iterator         = NULL;
	PLDAP_RECORD_OP recordOp        = newLDAPRecordOperations();

	/* itero sobre los registros obtenidos a traves de un iterador que conoce la implementacion del recordset */
	iterator = resultSet->iterator;
	if(!(iterator->hasNext(resultSet)))
		LoguearInformacion("No hay ninguna entry para el criterio especificado.", APP_NAME_FOR_LOGGER);
	for(*puiCantidadEntries= 0; iterator->hasNext(resultSet); (*puiCantidadEntries)++) {
		PLDAP_RECORD record = iterator->next(resultSet);

		/* Itero sobre los campos de cada uno de los record */
		unsigned int i;
		while(recordOp->hasNextField(record)) {

			PLDAP_FIELD field = recordOp->nextField(record);
			if(strcmp(field->name, OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME)==0)
				asprintf(&(pczListado[*puiCantidadEntries]), "%s", (char*)field->values[0]);

			/* se libera la memoria utilizada por el field si este ya no es necesario. */
			freeLDAPField(field);
		}
		/* libero los recursos consumidos por el record */
		freeLDAPRecord(record);
	}

    /* libero los recursos */
    freeLDAPIterator(iterator);
    freeLDAPRecordOperations(recordOp);
}

/**
 * Se realiza una consulta al directorio en una determinada rama. Para iterar sobre los resultados se utiliza un
 * patron Iterator que recorre cada una de las entries.
 * El uiTipoDeSelect, se indica para establecer cuales atributos de una entry se quieren traer:
 * 		1:	sGrupoDeNoticia
 * 		2:	sBody, sHead
 */
VOID selectArticles(  stArticle			pczListado[]
					, unsigned int*			puiCantidadEntries
					, PLDAP_SESSION 		stPLDAPSession
					, PLDAP_SESSION_OP 		stPLDAPSessionOperations
					, char* 				sCriterio){

	/* hago una consulta en una determinada rama aplicando la siguiente condicion */

	PLDAP_RESULT_SET resultSet      = stPLDAPSessionOperations->searchEntry(stPLDAPSession, OPENDS_SCHEMA, sCriterio);
	PLDAP_ITERATOR iterator         = NULL;
	PLDAP_RECORD_OP recordOp        = newLDAPRecordOperations();

	/* itero sobre los registros obtenidos a traves de un iterador que conoce la implementacion del recordset */
	iterator = resultSet->iterator;
	if(!(iterator->hasNext(resultSet)))
		LoguearInformacion("No hay ninguna entry para el criterio especificado.", APP_NAME_FOR_LOGGER);
	for(*puiCantidadEntries= 0; iterator->hasNext(resultSet); (*puiCantidadEntries)++) {
		LoguearDebugging("Itero por un articulo.", APP_NAME_FOR_LOGGER);
		PLDAP_RECORD record = iterator->next(resultSet);
		stArticle stArticle;

		/* Itero sobre los campos de cada uno de los record */
		unsigned int i;
		while(recordOp->hasNextField(record)) {
			LoguearDebugging("Itero por un campo de un articulo.", APP_NAME_FOR_LOGGER);

			PLDAP_FIELD field = recordOp->nextField(record);
			if(strcmp(field->name, OPENDS_ATTRIBUTE_ARTICLE_BODY)==0)
				asprintf(&(stArticle.sBody), "%s", (char*)field->values[0]);
			else if(strcmp(field->name, OPENDS_ATTRIBUTE_ARTICLE_HEAD)==0)
				asprintf(&(stArticle.sHead), "%s", (char*)field->values[0]);
			else if(strcmp(field->name, OPENDS_ATTRIBUTE_ARTICLE_ID)==0)
				stArticle.uiArticleID= atoi((char*)field->values[0]);
			else if(strcmp(field->name, OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME)==0)
				asprintf(&(stArticle.sNewsgroup), "%s", (char*)field->values[0]);

			/* se libera la memoria utilizada por el field si este ya no es necesario. */
			freeLDAPField(field);
		}
		pczListado[*puiCantidadEntries]= stArticle;

		/* libero los recursos consumidos por el record */
		freeLDAPRecord(record);
	}

    /* libero los recursos */
    freeLDAPIterator(iterator);
    freeLDAPRecordOperations(recordOp);
}

/**
 * Se realiza una consulta al directorio en una determinada rama. Para iterar sobre los resultados se utiliza un
 * patron Iterator que recorre cada una de las entries, y se las imprime por stdin.
 */
VOID selectAndPrintEntries(	  PLDAP_SESSION 		stPLDAPSession
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

			printf("\tAttribute: %s - values: %d\n", field->name, (int)field->valuesSize);

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
