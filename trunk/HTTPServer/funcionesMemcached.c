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
} stArticle;*/

void formarClave(char* claveCache,char* sGrupoDeNoticias, int ID )
{
  int largoID;
  int largoGrupoDeNoticias;
  largoID = sizeof(ID);
  claveCache = NULL;
  largoGrupoDeNoticias = strlen(sGrupoDeNoticias) + 1;
  claveCache = malloc(largoGrupoDeNoticias+largoID);
  sprintf(claveCache,"%s%d",sGrupoDeNoticias,largoID);
  return; 
	
}

void iniciarClusterCache(memcached_st **memCluster,char* memcachedServer1,int memcachedServer1Puerto,char* memcachedServer2,int memcachedServer2Puerto)
{
  memcached_return rc;
  /* Se crea el Cluster */  
  *memCluster = memcached_create(NULL); 
  /* Se agregan Servidores */
  rc = memcached_server_add(*memCluster, memcachedServer1,memcachedServer1Puerto); 
  printf("SERVIDOR 1 IP : %s  PUERTO : %d \n",memcachedServer1,memcachedServer1Puerto);  
  if (rc == MEMCACHED_SUCCESS)
    fprintf(stderr,"Se agrego el servidor  1 correctamente\n");
  else
    fprintf(stderr,"No se pudo agregar el servidor: %s\n",memcached_strerror(*memCluster, rc));
  return;
  rc = memcached_server_add(*memCluster, memcachedServer2,memcachedServer2Puerto); 
  printf("SERVIDOR 1 IP : %s  PUERTO : %d \n",memcachedServer2,memcachedServer2Puerto);  
  if (rc == MEMCACHED_SUCCESS)
    fprintf(stderr,"Se agrego el servidor  2 correctamente\n");
  else
    fprintf(stderr,"No se pudo agregar el servidor: %s\n",memcached_strerror(*memCluster, rc));
  return;
 
}


void guardarNoticiaEnCache(stArticle article, char *sGrupoDeNoticias ,memcached_st **memc)
{

  printf("##################### GUARDAR NOTICIA ######################\n");
  memcached_return rc;
  uint32_t flags;
  t_news *articuloCache = malloc(sizeof(t_news));
  
  char *ID;
  char *claveCache ;
  int largoID;
  int largoGrupoDeNoticias;
  
  largoID = sizeof(article.uiArticleID);
  ID = malloc(largoID);
printf("PASA POR ACA \n");
  sprintf(ID,"%d",article.uiArticleID);
  largoID = strlen(ID);
printf("PASA POR ACA \n");
  largoGrupoDeNoticias = strlen(sGrupoDeNoticias) + 1;
  claveCache = malloc(largoGrupoDeNoticias+largoID);
  sprintf(claveCache,"%s%s",sGrupoDeNoticias,ID/*article.uiArticleID*/);
  
  /*formarClave(claveCache,sGrupoDeNoticias,article.uiArticleID);*/
  printf("CLAVE CACHE %s \n",claveCache);
  
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
printf("VA A PASAR EL MEMCACHED_SET \n");
  rc=memcached_set(*memc,claveCache,strlen(claveCache),articuloEnBytes,articuloEnBytesLargo,0,0);
printf("YA PASO EL MEMCACHED_SET \n");
  printf("articuloEnBytes:%d \n",articuloEnBytes);
  printf("articuloEnBytes+sizeof(t_news_largos):%d \n",articuloEnBytes+sizeof(t_news_largos));
  printf("articuloEnBytes+sizeof(t_news_largos)+articuloCache->datos.largoHead:%d \n",articuloEnBytes+sizeof(t_news_largos)+articuloCache->datos.largoHead);
  
  if (rc == MEMCACHED_SUCCESS)
	printf("Se inserto correctamente el articulo en la cache\n");
  else
	printf("No se pudo insertar el articulo en la cache\n");	
  
  free(articuloEnBytes);
  free(articuloCache);
  free(claveCache);
  return;
    
}

int buscarNoticiaEnCache(stArticle* pstArticulo, char* sGrupoDeNoticias, char* sArticleID, memcached_st **memc)
{
printf("##################### BUSQUEDA EN LA CACHE ######################\n");
  uint32_t flags;
  memcached_return rc;
  t_news *resultNoticia = malloc(sizeof(t_news));
  char *resultadoCache  = NULL; 
  int resultNoticiaEnBytes_largo, resultado;

  char* claveCache;
  int largoID = strlen(sArticleID) + 1;
  int largoGrupoDeNoticias = strlen(sGrupoDeNoticias) + 1;
  claveCache = malloc(largoGrupoDeNoticias+largoID);
  sprintf(claveCache,"%s%s",sGrupoDeNoticias,sArticleID);
printf("Clave a buscar en la cache %s \n",claveCache);
    
  resultadoCache=memcached_get(*memc,claveCache,strlen(claveCache),&resultNoticiaEnBytes_largo,&flags,&rc);
  if(rc==MEMCACHED_SUCCESS)
	printf("Se encontro el articulo en la cache\n");
  else
  {
  	printf("No se encontro el articulo en la cache\n");
	return 0;
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
  pstArticulo->sHead	  = resultNoticia->head;
  pstArticulo->sBody	  = resultNoticia->body;
  
  free(claveCache);
  free(resultNoticia);
  return 1;
};