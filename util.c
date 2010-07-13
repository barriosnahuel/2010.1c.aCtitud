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

char* reemplazarBarraNPorBR(char** unaCadena){
	char* aux;
	asprintf(&aux, "%s", *unaCadena);
	asprintf(unaCadena, "%s", "");
	int index= 0;

	while(index<strlen(aux)){

		/*	Busco el index del \n	*/
		for(index= 0; aux[index]!='\n' && index!=strlen(aux); index++)
			;

		/*	Le asigno el resto de lo que le queda, es decir desde el \n en adelante.	*/
		if(index==strlen(aux)){
			asprintf(unaCadena, "%s%s", *unaCadena, aux);
			return *unaCadena;
		}

		/*	Obtengo un char* desde el inicio hasta el \n	*/
		char* tmp;
		substringTill(&tmp, aux, index);

		/*	Al char* anterior le concateno el tag <BR />	*/
		strcat(tmp, "<BR />");

		/*	Concateno la parte nueva, con lo que ya tenia	*/
		asprintf(unaCadena, "%s%s", *unaCadena, tmp);

		/*	Le asigno el resto de lo que le queda, es decir desde el \n en adelante.	*/
		substringFrom(&aux, aux, index+1);

		/*	Vuelvo a cero el index, por la linea anterior.	*/
		index= 0;
	}

	return *unaCadena;
}


void pasarAMayusculas(char* sCadena){
	LoguearDebugging("--> pasarAMayusculas()");

	unsigned int uiCantCaracteres= strlen(sCadena);
	int index;
	for(index= 0; index<uiCantCaracteres; index++){
		*(sCadena+index)= toupper(*(sCadena+index));
	}

	LoguearDebugging("<-- pasarAMayusculas()");
}


void quitarRepetidos(char* unListadoDeCadenas[], int iCantidadDeElementos) {
	int i;
	int j;
	char grupoDeNoticias[70];/*	TODO: 70???	*/

	char* listadoEnMayusculas[iCantidadDeElementos];
	for(i = 0; i < iCantidadDeElementos; i++) {
		char* sCadena;
		asprintf(&sCadena, "%s", unListadoDeCadenas[i]);
		pasarAMayusculas(sCadena);
		listadoEnMayusculas[i]= sCadena;
	}

	for(i = 0; i < iCantidadDeElementos; i++) {
		strcpy(grupoDeNoticias, listadoEnMayusculas[i]);
		for(j = i+1; j < iCantidadDeElementos; j++) {
			if(strcmp(grupoDeNoticias, listadoEnMayusculas[j]) == 0) {
				unListadoDeCadenas[j] = "0\0";
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
	int uiCantidadDeNoticias= 0;
	if(selectArticles(pstArticleListado, &uiCantidadDeNoticias, stPLDAPSession, stPLDAPSessionOperations, sCriterio) == 0) {
		LoguearDebugging("<-- obtenerListadoGruposDeNoticias()");
		return -1;
	}
	

	LoguearDebugging("<-- obtenerListadoNoticiasParaUnGrupo()");
	return uiCantidadDeNoticias;
}



int obtenerListadoGruposDeNoticias(	  char*					pListadoGruposDeNoticias[]
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
	if(selectEntries(listadoGrupoNoticiasRepetidos, &cantidadDeGrupos, stPLDAPSession, stPLDAPSessionOperations, sCriterio) == 0) {
		LoguearDebugging("<-- obtenerListadoGruposDeNoticias()");
		return -1;
	}
	

	asprintf(&sLogMessage, "Se encontraron %d grupos de noticias (contando repetidos).", cantidadDeGrupos);
	LoguearInformacion(sLogMessage);

	quitarRepetidos(listadoGrupoNoticiasRepetidos, cantidadDeGrupos);
	uiCantidadDeGruposSinRepetir= pasarArrayEnLimpio(listadoGrupoNoticiasRepetidos, cantidadDeGrupos, pListadoGruposDeNoticias);

	asprintf(&sLogMessage, "Hay %d grupos de noticias (sin repetir).", uiCantidadDeGruposSinRepetir);
	LoguearInformacion(sLogMessage);

	LoguearDebugging("<-- obtenerListadoGruposDeNoticias()");
	return uiCantidadDeGruposSinRepetir;
}


