/*
 * util.h
 *
 *  Created on: May 23, 2010
 *      Author: Barrios, Nahuel.
 */

#ifndef UTIL_H_
#define UTIL_H_

#include "LDAP/LdapWrapperHandler.h"

/**
 * Quita los elementos repetidos del array unListadoDeCadenas, y pone ceros en las posiciones donde se repiten.
 * Importante: Esta funcion NO es case sensitive.
 */
void quitarRepetidos(char* unListadoDeCadenas[], int iCantidadDeElementos);

/**
 * Pasa los campos que sean distintos de cero del array listadoGrupoNoticiasRepetidos a listadoGrupoNoticiasSinRepetir y retorna la cantidad de campos que posee este
 * ultimo.
 */
unsigned int pasarArrayEnLimpio(char* listadoGrupoNoticiasRepetidos[], int iCantidadDeGruposDeNoticias, char* listadoGrupoNoticiasSinRepetir[]);

/**
 * Reemplaza los %20 por espacios y devuelve la cadena sin %20.
 */
char* formatearEspacios(char* sRecursoPedido, char* sRecursoPedidoSinEspacios);

/**
 * Setea en sDest un substring de sSource a partir de la posicion begin (hasta el final).
 * Se asegura de que sDest tiene un espacio en memoria suficiente para almacenar el substring.
 */
char* substringFrom(char** sDest, const char* sSource, unsigned int begin);

/**
 * Recorre el char* completo y reemplaza cada ocurrencia del caracter \n por un tag <BR /> de HTML.
 */
char* reemplazarBarraNPorBR(char** unaCadena);

/**
 * Setea en sDest un substring de sSource a partir de la posicion 0, hasta la posicion till,
 * donde se setea un \0 para marcar la finalizacion del string.
 * Se asegura de que sDest tiene un espacio en memoria suficiente para almacenar el substring.
 */
char* substringTill(char** sDest, const char* sSource, unsigned int till);

/**
 * Dado un string "grupoNoticia@articleID" en sParametroDelComando, se obtienen de ese string
 * ambos valores y se los setea por separado.
 */
void obtenerParametrosDesdePK(	char** psGrupoNoticia
								, char**	psArticleID
								, char*		sParametroDelComando);

/**
 * Setea en pListadoGruposDeNoticias[] un listado de char* con los grupos de noticias
 * disponibles en la BD. Retorna la cantidad total de grupos.
 * Nota: Esta funcion retorna un listado SIN repetir.
 */
int obtenerListadoGruposDeNoticias(  char*					pListadoGruposDeNoticias[]
									, PLDAP_SESSION 		stPLDAPSession
									, PLDAP_SESSION_OP 		stPLDAPSessionOperations
									);

/**
 * Setea en pstArticleListado[] un listado de stArticle con los articulos disponibles
 * para un grupo de noticia dado (sGrupoDeNoticias). Retorna la cantidad total de noticias.
 */
int obtenerListadoNoticiasParaUnGrupo( stArticle			pstArticleListado[]
										, PLDAP_SESSION 	stPLDAPSession
										, PLDAP_SESSION_OP	stPLDAPSessionOperations
										, char* 			sGrupoDeNoticias
										);

/**
 * Pasa a mayusculas la cadena de texto que se le pasa como parametro.
 */
void pasarAMayusculas(char* sCadena);

#endif /* UTIL_H_ */
