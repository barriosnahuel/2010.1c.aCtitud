/*
 ============================================================================
 Name        : Ldap.c
 Author      : Diego Marafetti
 Version     : 0.1
 Copyright   : Your copyright notice
 Description : El objetivo de este codigo es demostrar el uso de la API
			   que actua de wrapper sobre la API original de OpenLDAP.
 ============================================================================
 */


#include <stdio.h>
#include "LdapWrapper.h"



/**
 * Esta funcion muestra como insertar una nueva entry
 * con nuevos atributos y valores
 *
 */
VOID insertEntry(PLDAP_SESSION session, PLDAP_SESSION_OP sessionOp, PLDAP_ENTRY_OP entryOp, PLDAP_ATTRIBUTE_OP attribOp) {

	/* creo una nueva entry.
	 le agrego los parametros correspondientes */
	PLDAP_ENTRY entry = entryOp->createEntry();

	entry->dn = "utnArticleID=12345,ou=so,dn=utn,dn=edu";
	entryOp->addAttribute(entry, attribOp->createAttribute("objectclass", 2, "top", "utnArticle"));
	entryOp->addAttribute(entry, attribOp->createAttribute("utnArticleID", 1, "12345"));
	entryOp->addAttribute(entry, attribOp->createAttribute("utnArticleHead", 1, "Head del articulo"));
    entryOp->addAttribute(entry, attribOp->createAttribute("utnArticleBody", 1, "Body del articulo"));
    entryOp->addAttribute(entry, attribOp->createAttribute("utnArticleGroupName", 1, "Group del articulo"));
	sessionOp->addEntry(session, entry);

}



/**
 * Se muestra como eliminar una entrada existente a partir
 * de un dn conocido.
 *
 */
VOID delEntry(PLDAP_SESSION session, PLDAP_SESSION_OP sessionOp, PLDAP_ENTRY_OP entryOp, PLDAP_ATTRIBUTE_OP attribOp) {

	/* creo una nueva entry.
	 le agrego los parametros correspondientes */
	PLDAP_ENTRY entry = entryOp->createEntry();

	entry->dn = "utnArticleID=12345,ou=so,dn=utn,dn=edu";

	/* Se puede eliminar ena entry pasando un objeto
	 entry como parametro */
	sessionOp->deleteEntryObj(session, entry);


	/* O se puede eliminar una entry pasando
	 el dn correspondiente */
	sessionOp->deleteEntryDn(session, "utnArticleID=12345,ou=so,dn=utn,dn=edu");
}






/**
 * Se muestra como modificar el valor de un atributo en una entry
 * Es necesario conocer el dn.
 *
 */
VOID modifyEntry(PLDAP_SESSION session, PLDAP_SESSION_OP sessionOp, PLDAP_ENTRY_OP entryOp, PLDAP_ATTRIBUTE_OP attribOp) {


	PLDAP_ENTRY entry = entryOp->createEntry();

	entry->dn = "utnArticleID=12345,ou=so,dn=utn,dn=edu";

	/* agrego el atributo a la entry
	// en modo delete */
	entryOp->editAttribute(entry, attribOp->createAttribute("utnurlTitle", 1, "Pagina de Prueba para LDAP"));


	sessionOp->editEntry(session, entry);

}





/**
 * Se realiza una consulta al directorio en una determinada
 * rama. Para iterar sobre los resultados se utiliza un
 * patron Iterator que recorre cada una de las entries
 *
 */
VOID selectEntries(PLDAP_SESSION session, PLDAP_SESSION_OP sessionOp, PLDAP_ENTRY_OP entryOp, PLDAP_ATTRIBUTE_OP attribOp) {

	/* hago una consulta en una determinada
	// rama aplicando la siguiente condicion */
	PLDAP_RESULT_SET resultSet 	= sessionOp->searchEntry(session, "ou=so,dn=utn,dn=edu", "utnArticleID=*");
	PLDAP_ITERATOR iterator 	= NULL;
	PLDAP_RECORD_OP	recordOp	= newLDAPRecordOperations();

	/* itero sobre los registros obtenidos
	// a traves de un iterador que conoce
	// la implementacion del recorset */
	for(iterator = resultSet->iterator; iterator->hasNext(resultSet);) {

		PLDAP_RECORD record = iterator->next(resultSet);

		printf("dn: %s\n", record->dn);

		/* Itero sobre los campos de cada
		// uno de los record */
		while(recordOp->hasNextField(record)) {

			PLDAP_FIELD field = recordOp->nextField(record);
			INT	index = 0;

			printf("	attribute: %s - values: %l\n", field->name, field->valuesSize);

			for(; index < field->valuesSize; index++) {

				printf("			Value[%d]: %s\n", index, field->values[index]);

			}


			/* se libera la memoria utilizada
			// por el field si este ya no es
			// necesario. */
			freeLDAPField(field);

		}

		/* libero los recursos consumidos
		// por el record */
		freeLDAPRecord(record);

	}

	/* libero los recursos */
	freeLDAPIterator(iterator);
	freeLDAPRecordOperations(recordOp);

}





INT main(INT argc, PCHAR *argv) {

	PLDAP_SESSION session;
	PLDAP_CONTEXT context = newLDAPContext();

	/* creo los objetos que me permiten operar sobre
	 un contexto y sobre una session */
	PLDAP_CONTEXT_OP 	ctxOp 		= newLDAPContextOperations();
	PLDAP_SESSION_OP 	sessionOp 	= newLDAPSessionOperations();
	PLDAP_ENTRY_OP 		entryOp 	= newLDAPEntryOperations();
	PLDAP_ATTRIBUTE_OP 	attribOp 	= newLDAPAttributeOperations();


	/* inicia el context de ejecucion de ldap
	 en el host especificado */
	ctxOp->initialize(context, "ldap://127.0.0.1:1389");


	/* creo una session para comenzar a operar
	 sobre el direcotio ldap */
	session = ctxOp->newSession(context, "cn=Directory Manager", "password");


	/* inicio la session con el server ldap */
	sessionOp->startSession(session);


	/* TODO: operacion de datos aqui */
    insertEntry(session, sessionOp, entryOp, attribOp);
	/* modifyEntry(session, sessionOp, entryOp, attribOp);
	 deleteEntry(session, sessionOp, entryOp, attribOp); */


	/* cierro la session con el servidor */
	sessionOp->endSession(session);


	/* libero los objetos de operaciones */
	freeLDAPSession(session);
	freeLDAPContext(context);
	freeLDAPContextOperations(ctxOp);
	freeLDAPSessionOperations(sessionOp);

	return 0;

}
