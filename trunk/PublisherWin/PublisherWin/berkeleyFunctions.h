#ifndef __BERKELEYFUNCTIONS__
#define __BERKELEYFUNCTIONS__

#include<db.h>
#include<windows.h>
#include<stdio.h>
#include <string.h>
#include <time.h>
#include "xmlFunctions.h"
#pragma comment(lib,"libdb48.lib")

#define BERKELEY_ID_LEN 10+1

typedef struct newslen {
    size_t newsgrouplen;
    size_t bodylen;
    size_t headlen;
	size_t transmittedlen;
} newslen;

typedef struct news{
   newslen largos;
   char*   head;
   char*   body;
   char*   newsgroup;
   char*   id;
   char*   transmitted; // 0 NO FUE TRANSMITIDA , 1 SI .
}news;

void putArticle(struct news* noticia,DB** dbp,HANDLE** memoryHandler);
void closeDb(DB** dbp);
void createDb(DB** dbp, HANDLE** memoryHandle,char* dbName);
int lastID(DB** dbp); //Recorre db y busca la ultima id asignada

void noticiasNoEnviadas(DB** dbp, HANDLE** memoryHandle, char* ipNNTP, int puertoNNTP);
	
	/**
	 *	Genero un nuevo ID con el formato: MMddHHmmss
	 *	Si el mes por ejemplo es 07 (Julio), entonces el cero a la izquierda se saca.
	 *	El formato para la funcion strftime, es similar al de printf, lo saque de:
	 *	http://www.cplusplus.com/reference/clibrary/ctime/strftime/
	 */
	void generateNewID(DB** dbp, char** buffer);

#endif