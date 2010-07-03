#ifndef __BERKELEYFUNCTIONS__
#define __BERKELEYFUNCTIONS__

#include<db.h>
#include<windows.h>
#include<stdio.h>
#include<string.h>
#include <time.h>
#include"xmlFunctions.h"
#pragma comment(lib,"libdb48.lib")
#define BUFFERSIZE 1024

typedef struct newslen{
    int newsgrouplen;
    int bodylen;
    int headlen;
	int transmittedlen;
	int idlen;
}newslen;

typedef struct news{
   newslen largos;
   char*   head;
   char*   body;
   char*   newsgroup;
   //unsigned int id;
   char*	id;
   char* transmitted; // 0 NO FUE TRANSMITIDA , 1 SI .
}news;

	void putArticle(DB** dbp);
	void getArticle(DB** dbp);
	void closeDb(DB** dbp);
	void createDb(DB** dbp, HANDLE** memoryHandle);
	int lastID(DB** dbp); //Recorre db y busca la ultima id asignada
	void generateNewID(DB** dbp, char* buffer);

#endif