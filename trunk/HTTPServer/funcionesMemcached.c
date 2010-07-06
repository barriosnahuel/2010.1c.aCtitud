#include<string.h>
#include<stdio.h>
#include <unistd.h>
#include<stdlib.h>
#include"funcionesMemcached.h"
#include <ctype.h>
/** 
typedef struct stArticle {
     unsigned int uiArticleID;	
     char* sNewsgroup;			
     char* sHead;				
     char* sBody;				
} stArticle;*/

void iniciarClusterCache(memcached_st **memCluster,char* memcachedServer1,int memcachedServer1Puerto,char* memcachedServer2,int memcachedServer2Puerto)
{
  memcached_return rc;
  /* Se crea el Cluster */  
  *memCluster = memcached_create(NULL); 
  /* Se agregan Servidores */
  rc = memcached_server_add(*memCluster, memcachedServer1,memcachedServer1Puerto); 
  if (rc == MEMCACHED_SUCCESS)
	LoguearInformacion("Se agrego el servidor memcached 1 correctamente.");
  else
	LoguearError("No se pudo agregar el servidor memcached 1. ");

  rc = memcached_server_add(*memCluster, memcachedServer2,memcachedServer2Puerto); 
  if (rc == MEMCACHED_SUCCESS)
	LoguearInformacion("Se agrego el servidor memcached 2 correctamente.");
  else
	LoguearError("No se pudo agregar el servidor memcached 2. ");
  
  return;
}


void guardarNoticiaEnCache(stArticle article, char *sGrupoDeNoticias,char* grupoSinEspacios,memcached_st **memc)
{
  memcached_return rc;
  uint32_t flags;
  t_news *articuloCache = malloc(sizeof(t_news));
  char *ID = NULL;
  char *claveCache = NULL ;
  int largoID;
  int largoGrupoDeNoticias;
  
  largoID = sizeof(article.uiArticleID);
  ID = malloc(largoID);
  sprintf(ID,"%d",article.uiArticleID);
  largoID = strlen(ID) + 1;
  largoGrupoDeNoticias = strlen(grupoSinEspacios)+1;
  claveCache = malloc(largoGrupoDeNoticias+largoID);
  sprintf(claveCache,"%s%s",grupoSinEspacios,ID);
 
  articuloCache->body = NULL;
  articuloCache->head = NULL;
  articuloCache->datos.largoHead = strlen(article.sHead) + 1;	
  articuloCache->datos.largoBody = strlen(article.sBody) +1;
  articuloCache->head= malloc(articuloCache->datos.largoHead);
  articuloCache->body=malloc(articuloCache->datos.largoBody);
  strcpy(articuloCache->head,article.sHead);
  strcpy(articuloCache->body,article.sBody);
  char * articuloEnBytes;
  size_t  articuloEnBytesLargo;
  articuloEnBytesLargo=sizeof(t_news_largos)+articuloCache->datos.largoHead+articuloCache->datos.largoBody;
  articuloEnBytes = malloc(articuloEnBytesLargo);

  memcpy(articuloEnBytes,(char*)&articuloCache->datos,sizeof(t_news_largos));
  memcpy(articuloEnBytes+sizeof(t_news_largos),articuloCache->head,articuloCache->datos.largoHead);
  memcpy(articuloEnBytes+sizeof(t_news_largos)+articuloCache->datos.largoHead,articuloCache->body,articuloCache->datos.largoBody);
  rc=memcached_set(*memc,claveCache,strlen(claveCache),articuloEnBytes,articuloEnBytesLargo,0,0);

  if (rc == MEMCACHED_SUCCESS){
	LoguearInformacion("Se inserto correctamente el articulo en la cache.");
  }else{
	LoguearError("No se pudo insertar el articulo en la cache.");
  }
  free(articuloEnBytes);
  free(articuloCache);
  free(claveCache);
  return;
  
}


int buscarNoticiaEnCache(stArticle* pstArticulo, char* sGrupoDeNoticias, char* sArticleID,char* grupoSinEspacios,memcached_st **memc)
{
	LoguearDebugging("--> buscarNoticiaEnCache()");
  uint32_t flags;
  memcached_return rc;
  t_news *resultNoticia = malloc(sizeof(t_news));
  char *resultadoCache  = NULL; 
  char* claveCache;
  int resultNoticiaEnBytes_largo, resultado;

  LoguearDebugging("--> 1()");
  int largoID = strlen(sArticleID) + 1;
  int largoGrupoDeNoticias = strlen(grupoSinEspacios) + 1;
  claveCache = malloc(largoGrupoDeNoticias+largoID);
  sprintf(claveCache,"%s%s",grupoSinEspacios,sArticleID);
  
  LoguearDebugging("--> por hacer el get()");
  resultadoCache = memcached_get(*memc,claveCache,strlen(claveCache),&resultNoticiaEnBytes_largo,&flags,&rc);
  if(rc==MEMCACHED_SUCCESS){
	LoguearInformacion("Se encontro el articulo en la cache.");
  }else{
  	LoguearInformacion("No se encontro el articulo en la cache.");
	free(claveCache);
	free(resultNoticia);
	LoguearDebugging("<-- buscarNoticiaEnCache()");
	return 0;
  }  	
  
  memcpy(&resultNoticia->datos,resultadoCache,sizeof(t_news_largos));
  resultNoticia->head = malloc(resultNoticia->datos.largoHead);
  memcpy(resultNoticia->head,resultadoCache+sizeof(t_news_largos),resultNoticia->datos.largoHead);
  resultNoticia->body=malloc(resultNoticia->datos.largoBody);
  memcpy(resultNoticia->body,resultadoCache+sizeof(t_news_largos)+resultNoticia->datos.largoHead,resultNoticia->datos.largoBody);

  pstArticulo->sNewsgroup = sGrupoDeNoticias;
  pstArticulo->sHead	  = resultNoticia->head;
  pstArticulo->sBody	  = resultNoticia->body;
  
  free(claveCache);
  free(resultNoticia);
  LoguearDebugging("<-- buscarNoticiaEnCache()");
  return 1;
}
