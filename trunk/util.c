/*
 * util.c
 *
 *  Created on: May 23, 2010
 *      Author: Barrios, Nahuel.
 */

#include "util.h"


char* substringFrom(char** sDest, const char* sSource, unsigned int begin){
	asprintf(sDest, "%s", sSource);
	(*sDest)+= begin;
}

char* substringTill(char** sDest, const char* sSource, unsigned int till){
	asprintf(sDest, "%s", sSource);
	*((*sDest)+till)= '\0';
}





void quitarRepetidos(char* listadoGrupoNoticiasRepetidos[], int iCantidadDeGruposDeNoticias) {
	int i;
	int j;
	char grupoDeNoticias[70];/*	TODO: 70???	*/

	for(i = 0; i < iCantidadDeGruposDeNoticias; i++) {
		strcpy(grupoDeNoticias, listadoGrupoNoticiasRepetidos[i]);
		for(j = i+1; j < iCantidadDeGruposDeNoticias; j++) {
			if(strcmp(grupoDeNoticias, listadoGrupoNoticiasRepetidos[j]) == 0) {
				listadoGrupoNoticiasRepetidos[j] = "0\0";
			}
		}
	}
}

unsigned int pasarArrayEnLimpio(char* listadoGrupoNoticiasRepetidos[], int iCantidadDeGruposDeNoticias, char* listadoGrupoNoticiasSinRepetir[]) {
	int p;
	int l = 0;

	for(p = 0; p < iCantidadDeGruposDeNoticias; p++) {
		if(strcmp("0", listadoGrupoNoticiasRepetidos[p]) != 0) {
			listadoGrupoNoticiasSinRepetir[l] = listadoGrupoNoticiasRepetidos[p];
			l++;
		}
	}
	return l;
}







char* formatearEspacios(char* sRecursoPedido, char* sRecursoPedidoSinEspacios) {
	int i = 0;
	int j = 0;

	while(sRecursoPedido[i] != '\0') {
		if(sRecursoPedido[i] == '%') {
			sRecursoPedidoSinEspacios[j] = ' ';
			i = i + 3;
			j++;
		}
		else {
			sRecursoPedidoSinEspacios[j] = sRecursoPedido[i];
			i++;
			j++;
		}
	}
	sRecursoPedidoSinEspacios[j] = '\0';
}


void obtenerParametrosDesdePK(	char**		psGrupoNoticia
								, char**	psArticleID
								, char*		sParametroDelComando){

	int indexOfArroba= strcspn(sParametroDelComando, "@");

		/*	+1 porque sino entra el @ en el substring.	*/
	substringFrom(psArticleID, sParametroDelComando, indexOfArroba+1);
	substringTill(psGrupoNoticia, sParametroDelComando, indexOfArroba);
}


int obtenerListadoNoticiasParaUnGrupo( stArticle			pstArticleListado[]
										, PLDAP_SESSION 	stPLDAPSession
										, PLDAP_SESSION_OP	stPLDAPSessionOperations
										, char* 			sGrupoDeNoticias
										){
	LoguearDebugging("--> obtenerListadoNoticiasParaUnGrupo()");
	char* sLogMessage;

	/*	El limite impuesto por la bd, mas el largo del nombre del atributo, mas el igual.	*/
	unsigned int uiNumberOfCharacters= strlen(OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME)+1+OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME_MAX_LENGHT;

	char sCriterio[strlen(OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME)+1+OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME_MAX_LENGHT];
	sprintf(sCriterio, "%s=%s", OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME, sGrupoDeNoticias);

	asprintf(&sLogMessage, "El criterio para buscar en OpenDS es: %s", sCriterio);
	LoguearInformacion(sLogMessage);

	LoguearDebugging("Hago el select a OpenDS");
	unsigned int uiCantidadDeNoticias= 0;
	selectArticles(pstArticleListado, &uiCantidadDeNoticias, stPLDAPSession, stPLDAPSessionOperations, sCriterio);

	LoguearDebugging("<-- obtenerListadoNoticiasParaUnGrupo()");
	return uiCantidadDeNoticias;
}



int obtenerListadoGruposDeNoticias(
							char*					pListadoGruposDeNoticias[]
							, PLDAP_SESSION 		stPLDAPSession
							, PLDAP_SESSION_OP 		stPLDAPSessionOperations
							){
	LoguearDebugging("--> obtenerListadoGruposDeNoticias()");
	char* sLogMessage;

	/*	Sumo 3 por el =* y el \0	*/
	char sCriterio[strlen(OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME)+1+1+1];
	sprintf(sCriterio, "%s=%s", OPENDS_ATTRIBUTE_ARTICLE_GROUP_NAME, "*");

	unsigned int cantidadDeGrupos= 0;
	unsigned int uiCantidadDeGruposSinRepetir= 0;


	/* Este memset es importantisimo, ya que si no le seteamos ceros al array, y queremos ingresar a una posicion que no tiene nada tira Seg Fault */
	memset(pListadoGruposDeNoticias, 0, 1000);/*	TODO: Chequear este 1000, ver como deshardcodearlo	*/

	char* listadoGrupoNoticiasRepetidos[1000];/*	TODO: Chequear este 1000, ver como deshardcodearlo	*/
	selectEntries(listadoGrupoNoticiasRepetidos, &cantidadDeGrupos, stPLDAPSession, stPLDAPSessionOperations, sCriterio);

	quitarRepetidos(&listadoGrupoNoticiasRepetidos, cantidadDeGrupos);

	uiCantidadDeGruposSinRepetir= pasarArrayEnLimpio(&listadoGrupoNoticiasRepetidos, cantidadDeGrupos, pListadoGruposDeNoticias);

	LoguearDebugging("<-- obtenerListadoNoticiasParaUnGrupo()");
	return uiCantidadDeGruposSinRepetir;
}


