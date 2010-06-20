#include"berkeleyFunctions.h" //Dentro tiene la llamada a #include"xmlFunctions"

//El formato propuesto para los mensajes XML es el siguiente: 
	//	<?xml version="1.0" encoding="iso-8859-1" ?> 
	//		<news> 
	//			<newsgroup>Minuto.com</newsgroup> 
	//			<idNoticia>12345</idNoticia> 
	//			<HEAD>MARTIN PALEMO IDOLO</HEAD> 
	//			<BODY>ARGENTINA CAMPEON MUNDIAL 2010</BODY> 
	//		</news> 


xmlDocPtr crearXML(struct news* noticia, char* key)
 {
	xmlNodePtr root,news;
	xmlDocPtr doc;
	char* NEWS     = "news";
	char* ENCODING = "iso-8859-1";
	
	printf("<------------------- creacion XML - aCtitud -------------------> \n");
	doc = xmlNewDoc("1.0");
	doc->encoding = ENCODING; //Propiedad del nodo padre
	
	//Nodo padre
	root = xmlNewDocNode (doc, NULL, NEWS, NULL);
	xmlDocSetRootElement (doc, root);
	
	//Hijos
	xmlNewChild (root, NULL, "newsgroup",  noticia->newsgroup);
	xmlNewChild (root, NULL,"idNoticia", key);
	xmlNewChild (root, NULL, "HEAD",  noticia->head);
	xmlNewChild (root, NULL, "BODY",  noticia->body);

	return doc;
}
