#ifndef __XMLFUNCTIONS__
#define __XMLBERKELEYFUNCTIONS__

#pragma comment(lib,"libxml2.lib")
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

	xmlDocPtr crearXML(struct news* noticia, char* key);


#endif
