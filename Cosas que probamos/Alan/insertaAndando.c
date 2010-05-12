#include<string.h>
#include<stdio.h>
#include <unistd.h>
#include<stdlib.h>
#include <libmemcached/memcached.h>
#include "Article.h"

typedef struct _largoArticulo{
  size_t largoHead;
  size_t largoBody;
}t_news_largos;

typedef struct _articuloCache{
  t_news_largos datos;  
  char *head;
  char *body;
}t_news;

int main()
{
  memcached_server_st *servers = NULL;
  memcached_st *memc;
  memcached_return rc;
  uint32_t flags;


  memc = memcached_create(NULL); 
  servers = memcached_server_list_append(servers, "localhost", 11211,&rc);
  rc      = memcached_server_push(memc, servers);
  
  if (rc == MEMCACHED_SUCCESS)
    fprintf(stderr,"Se agrego el servidor correctamente\n");
  else
    fprintf(stderr,"No se pudo agregar el servidor: %s\n",memcached_strerror(memc, rc));

  stArticle article;
  stArticle * articleRespuesta;
  article.sBody= "body";
  article.sHead= "head";
  article.uiArticleID= 6969;
    
  stArticle * ptrArticulo = &article ;//memcached necesita ptros  
  printf("Se creara  el servidor 1 \n");
  

  t_news * articuloCache = malloc(sizeof(t_news));
  articuloCache->body = NULL;
  articuloCache->head = NULL;
  articuloCache->datos.largoHead = strlen(article.sHead) + 1;	
  printf("largo head %d \n",articuloCache->datos.largoHead);
  articuloCache->datos.largoBody = strlen(article.sBody) + 1;
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
  articuloEnBytes = malloc(articuloEnBytesLargo-1);

  memcpy(articuloEnBytes,(char*)&articuloCache->datos,sizeof(t_news_largos));
  memcpy(articuloEnBytes+sizeof(t_news_largos),articuloCache->head,articuloCache->datos.largoHead);
  memcpy(articuloEnBytes+sizeof(t_news_largos)+articuloCache->datos.largoHead,articuloCache->body,articuloCache->datos.largoBody);
  rc=memcached_set(memc,"111",strlen("111"),articuloEnBytes,articuloEnBytesLargo,(time_t)0,(uint32_t)0);

  printf("articuloEnBytes:%d \n",articuloEnBytes);
  printf("articuloEnBytes+sizeof(t_news_largos):%d \n",articuloEnBytes+sizeof(t_news_largos));
  printf("articuloEnBytes+sizeof(t_news_largos)+articuloCache->datos.largoHead:%d \n",articuloEnBytes+sizeof(t_news_largos)+articuloCache->datos.largoHead);
  //printf("Espacio en el cual tendria que terminar : %d",articuloEnBytes+)
  if(rc==MEMCACHED_SUCCESS)
	printf("Se inserto correctamente \n");
  else
	printf("No se pudo insertar \n");	
  
  free(articuloCache);

  
//MEMCACHED_GET	
	
  t_news *resultNoticia = malloc(sizeof(t_news));
  char *resultadoCache=NULL;
  int resultNoticiaEnBytes_largo, resultado;

  resultadoCache=memcached_get(memc,"111",strlen("111"),&resultNoticiaEnBytes_largo,&flags,&rc);

  memcpy(&resultNoticia->datos,resultadoCache,sizeof(t_news_largos));
  resultNoticia->head = malloc(resultNoticia->datos.largoHead);
  memcpy(resultNoticia->head,resultadoCache+sizeof(t_news_largos),resultNoticia->datos.largoHead);
  printf("Resultado HEAD: %s \n",resultNoticia->head);
  printf("Tamanio cabecera : %d \n",resultNoticia->datos.largoHead);

  resultNoticia->body=malloc(resultNoticia->datos.largoBody);
  memcpy(resultNoticia->body,resultadoCache+sizeof(t_news_largos)+resultNoticia->datos.largoHead,resultNoticia->datos.largoBody);
  printf("Resultado BODY : %s \n",resultNoticia->body);
  
  free(resultNoticia);

  return 0;
}
