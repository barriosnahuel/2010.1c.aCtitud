#ifndef __XMLFUNCTIONS__
#define __XMLBERKELEYFUNCTIONS__

#pragma comment(lib,"libxml2.lib")
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

	xmlDocPtr crearXML(struct news* noticia, char* key);
	int enviarXML(xmlChar* memoriaXML,int tamanioXML,char* ipNNTP,int puertoNNTP,HANDLE** memoryHandle);
	void encoding(char*cadena);
#endif
