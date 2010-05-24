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
