/*
 * LdapWrapperHandler-Win.c
 *
 *  Created on: Jun 13, 2010
 *      Author: Barrios, Nahuel.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "LdapWrapperHandler-Win.hpp"

/*	ToDo: Ver en todos los lugares donde haya asprintf(...) de reemplazarlo por otra forma porque esa funcion usa malloc. 
Se tendria que usar lo del heap seguramente.	*/

/*	Constantes de OpenDS	*/
#define OPENDS_SCHEMA "ou=so,dn=utn,dn=edu"
#define OPENDS_ATTRIBUTE_OBJECT_CLASS "objectClass"
	#define OPENDS_ATTRIBUTE_OBJECT_CLASS_TOP "top"
	#define OPENDS_ATTRIBUTE_OBJECT_CLASS_ARTICLE "utnArticle"
#define OPENDS_ATTRIBUTE_ARTICLE_HEAD "utnArticleHead"
#define OPENDS_ATTRIBUTE_ARTICLE_BODY "utnArticleBody"


int crearConexionLDAP(	char* sIp
						, unsigned int uiPort
						, PLDAP_CONTEXT* pstPLDAPContext
						, PLDAP_CONTEXT_OP* pstPLDAPContextOperations
						, PLDAP_SESSION* pstPLDAPSession
						, PLDAP_SESSION_OP* pstPLDAPSessionOperations) {
	

	/*	Seteo sOpenDSLocation bajo el formato:	ldap://localhost:4444	*/
	char *sOpenDSLocation= (char*)malloc(sizeof(char)*(9+15+5));/*	ToDo: Cambiar el malloc por lo que corresponde en Windows!!	*/
	strcpy(sOpenDSLocation, "ldap://");
	strcat(sOpenDSLocation, sIp);
	strcat(sOpenDSLocation, ":");
	char* sPort= (char*)malloc(sizeof(char)*6);/*	ToDo: Cambiar el malloc por lo que corresponde en Windows!!	*/
	itoa(uiPort, sPort, 2);
	strcat(sOpenDSLocation, sPort);
	/*asprintf(&sOpenDSLocation, "ldap://%s:%d", sIp, uiPort);*/

	/* Inicializamos el contexto. */
	(*pstPLDAPContextOperations)->initialize(*pstPLDAPContext, sOpenDSLocation);
	(*pstPLDAPSession) = (*pstPLDAPContextOperations)->newSession(
			*pstPLDAPContext, "cn=Directory Manager", "password");

	/* Se inicia la session. Se establece la conexion con el servidor LDAP. */
	(*pstPLDAPSessionOperations)->startSession(*pstPLDAPSession);

	/*	TODO: Ver alguna forma de retornar false cuando no me pueda conectar bien a la BD	*/
	
	return 1;
}


/**
 * Retorna un char* de la forma: "utnArticleID=12345,ou=so,dn=utn,dn=edu"
 */
char* getDNFor(int dArticleID){
	char *sDn;
	/*asprintf(&sDn, "%s=%d,%s", OPENDS_ATTRIBUTE_ARTICLE_ID, dArticleID, OPENDS_SCHEMA);*/

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
			/*INT     index = 0; --- TODO cambio acá para probar */
			DWORD index = 0;

			printf("\tAttribute: %s - values: %d\n", field->name, (int)field->valuesSize);

			for(; index < (field->valuesSize); index++)
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
