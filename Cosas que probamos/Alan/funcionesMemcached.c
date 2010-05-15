#include<string.h>
#include<stdio.h>
#include <unistd.h>
#include<stdlib.h>
#include"funcionesMemcached.h"


/** 
typedef struct stArticle {
     unsigned int uiArticleID;	
     char* sNewsgroup;			
     char* sHead;				
     char* sBody;				
} stArticle;
/*
void iniciarClusterCache(memcached_st* memc)
{
  memcached_server_st *servers = NULL;
  memcached_return rc;
  uint32_t flags;

  memc = memcached_create(NULL); 
  servers = memcached_server_list_append(servers, "localhost", 11211,&rc);
  rc      = memcached_server_push(memc, servers);
  
  if (rc == MEMCACHED_SUCCESS)
    fprintf(stderr,"Se agrego el servidor  1 correctamente\n");
  else
    fprintf(stderr,"No se pudo agregar el servidor: %s\n",memcached_strerror(memc, rc));
	
  servers = memcached_server_list_append(servers, "localhost", 11212,&rc);
  rc      = memcached_server_push(memc, servers);
  if (rc == MEMCACHED_SUCCESS)
    fprintf(stderr,"Se agrego el servidor  1 correctamente\n");
  else
    fprintf(stderr,"No se pudo agregar el servidor: %s\n",memcached_strerror(memc, rc));

  return;
}


void guardarNoticiaEnCache(stArticle article, char* sGrupoDeNoticias ,memcached_st* memc);
{
	
  t_news *articuloCache = malloc(sizeof(t_news));
  char* claveCache = NULL;
  int largoID;
  int largoGrupoDeNoticias;
  
  largoID = sizeof(article.uiArticleID);
  largoGrupoDeNoticias = strlen(sGrupoDeNoticias) + 1;
  claveCache = malloc(largoGrupoDeNoticias+largoID);
  sprintf(claveCache,"%s%d",sGrupoDeNoticias,largoID);
 
  articuloCache->body = NULL;
  articuloCache->head = NULL;
  articuloCache->datos.largoHead = strlen(article.sHead) + 1;	
  printf("largo head %d \n",articuloCache->datos.largoHead);
  articuloCache->datos.largoBody = strlen(article.sBody) +1;
  printf("largo body  %d \n",articuloCache->datos.largoBody);
  articuloCache->head= malloc(articuloCache->datos.largoHead);
  articuloCache->body=malloc(articuloCache->datos.largoBody);
  strcpy(articuloCache->head,article.sHead);
  strcpy(articuloCache->body,article.sBody);
  printf("Contenido d articuloCache->body :%s \n",articuloCache->body);
  printf("Contenido d articuloCache->head :%s \n",articuloCache->head);
  
  char * articuloEnBytes;
  size_t  articuloEnBytesLargo;
  printf("Tamanio de size_t : %d \n",sizeof(size_t));
  articuloEnBytesLargo=sizeof(t_news_largos)+articuloCache->datos.largoHead+articuloCache->datos.largoBody;
  printf("Largo estructura(t_news_largos): %d \n",sizeof(t_news_largos));
  printf("Tamanio de articuloEnBytesLargo %d \n",articuloEnBytesLargo);
  articuloEnBytes = malloc(articuloEnBytesLargo);

  memcpy(articuloEnBytes,(char*)&articuloCache->datos,sizeof(t_news_largos));
  memcpy(articuloEnBytes+sizeof(t_news_largos),articuloCache->head,articuloCache->datos.largoHead);
  memcpy(articuloEnBytes+sizeof(t_news_largos)+articuloCache->datos.largoHead,articuloCache->body,articuloCache->datos.largoBody);
  rc=memcached_set(memc,claveCache,strlen(claveCache),articuloEnBytes,articuloEnBytesLargo,(time_t)0,(uint32_t)0);

  printf("articuloEnBytes:%d \n",articuloEnBytes);
  printf("articuloEnBytes+sizeof(t_news_largos):%d \n",articuloEnBytes+sizeof(t_news_largos));
  printf("articuloEnBytes+sizeof(t_news_largos)+articuloCache->datos.largoHead:%d \n",articuloEnBytes+sizeof(t_news_largos)+articuloCache->datos.largoHead);
  
  if(rc==MEMCACHED_SUCCESS)
	printf("Se inserto correctamente el articulo en la cache\n");
  else
	printf("No se pudo insertar el articulo en la cache\n");	
  
  free(articuloEnBytes);
  free(articuloCache);
  free(claveCache);
  return;
}
*/
void formarClave(char* claveCache,char* sGrupoDeNoticias, int ID )
{
  int largoID;
  int largoGrupoDeNoticias;
  largoID = sizeof(ID);
  largoGrupoDeNoticias = strlen(sGrupoDeNoticias) + 1;
  claveCache = malloc(largoGrupoDeNoticias+largoID);
  sprintf(claveCache,"%s%d",sGrupoDeNoticias,largoID);
  return; 
	
}

int buscarNoticiaEnCache(stArticle* pstArticulo, char* sGrupoDeNoticias, char* sArticleID, memcached_st * memc)
{
  uint32_t flags;
  memcached_return rc;
 
  t_news *resultNoticia = malloc(sizeof(t_news));
  char *resultadoCache  = NULL;
  int resultNoticiaEnBytes_largo, resultado;

  char* claveCache;
  fomarClave(claveCache,sGrupoDeNoticias,pstArticulo>uiArticleID);
  
  resultadoCache=memcached_get(memc,claveCache,strlen(claveCache),&resultNoticiaEnBytes_largo,&flags,&rc);
  if(rc==MEMCACHED_SUCCESS)
	printf("Se encontro el articulo en la cache\n");
  else
  {
  	printf("No se encontro el articulo en la cache\n");
	return -1;
  }  	
  
  memcpy(&resultNoticia->datos,resultadoCache,sizeof(t_news_largos));
  resultNoticia->head = malloc(resultNoticia->datos.largoHead);
  memcpy(resultNoticia->head,resultadoCache+sizeof(t_news_largos),resultNoticia->datos.largoHead);
  printf("Resultado HEAD: %s \n",resultNoticia->head);
  printf("Tamanio cabecera : %d \n",resultNoticia->datos.largoHead);

  resultNoticia->body=malloc(resultNoticia->datos.largoBody);
  memcpy(resultNoticia->body,resultadoCache+sizeof(t_news_largos)+resultNoticia->datos.largoHead,resultNoticia->datos.largoBody);
  printf("Resultado BODY : %s \n",resultNoticia->body);
  
   
  pstArticulo->sNewsgroup = sGrupoDeNoticias;
  pstArticulo->uiArticulo = pstArticulo->uiArticleID;
  pstArticulo->sHead	  = resultNoticia->head;
  pstArticulo->sBody	  = resultNoticia->body;
  
  free(claveCache);
  free(resultNoticia);
  return 0;
};