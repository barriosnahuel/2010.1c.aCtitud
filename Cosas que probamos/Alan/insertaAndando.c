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
  size_t tamanioArticle;
  size_t tamanioID;
  size_t return_key_length;
  char *return_value;
  size_t return_value_length;

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
  
  
//  tamanioID=sizeof(article.uiArticleID)+1;
//  char ID[ tamanioID + 1];
//  sprintf(ID,"%d",ptrArticulo->uiArticleID);
//  printf("Convirtio el ID a un char* : %s ? \n",ID);
  
 // char ID[1000]; //= malloc(sizeof(article.uiArticleID)+1);
 // sprintf(ID,"%s",article.uiArticleID);
 //  printf("El ID quedon con: %s",article.uiArticleID);

  t_news * articuloCache         = malloc(sizeof(t_news));
  articuloCache->body				= NULL;
  articuloCache->head				= NULL;
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

// LEVANTAR LA NOTICIA 	----------

t_news * news = malloc(sizeof(t_news));
char * newsInBytes;
int   newsInBytes_size, return_code;

newsInBytes = memcached_get(memc, "1111", strlen("1111"), &newsInBytes_size, &flags, &rc);

news->datos =newsInBytes;
//memcpy(news->datos, newsInBytes, sizeof(t_news_largos));
news->head = malloc(news->datos.largoHead);
memcpy(news->head, newsInBytes + sizeof(t_news_largos), news->datos.largoHead);

news->body = malloc(news->datos.largoBody);
memcpy(news->body, newsInBytes + sizeof(t_news_largos) + news->datos.largoHead, news->datos.largoBody);

free(newsInBytes);

printf("Levanta bien el articulo buscado: %s \n",news->datos.largoBody); // tengo mis dudas sobre si lo que hice realmente traera algo.


/*
char * respuesta;
respuesta = memcached_get(memc,"111",strlen("111"),&articuloEnBytesLargo,&flags, &rc);

  if(rc == MEMCACHED_SUCCESS)
	printf("Levanta bien el articulo buscado: %s \n",respuesta); // tengo mis dudas sobre si lo que hice realmente traera algo.
  else
	printf("No levanta el articulo buscado \n");
*/  




/*

  tamanioID = sizeof(ptrArticulo->uiArticleID);
  printf("\n Tamanio ID: %d ", tamanioID);
  tamanioArticle = sizeof(article); 
  printf("\n Tamanio del articulo entero: %d \n",tamanioArticle);
  printf("Que corno es esto :%d \n ",(char*)&ptrArticulo->uiArticleID); 
  printf("Direccion de memoria del articulo ? :%d \n",&ptrArticulo);
  

rc=memcached_set(memc,(char*)&ptrArticulo->uiArticleID,tamanioID,(char*)&ptrArticulo,tamanioArticle,(time_t)0,(uint32_t)0);

  if (rc == MEMCACHED_SUCCESS)
    printf("El articulo se inserto correctamente =) \n");
  else
    printf("No se logro insertar el articulo =( \n");

  char * respuesta;
  
 respuesta = 
memcached_get(memc,(char*)&ptrArticulo->uiArticleID,tamanioID,&tamanioArticle,&flags,&rc);
 (stArticle *) respuesta;  
  if(rc == MEMCACHED_SUCCESS)
	printf("Levanta bien el articulo buscado \n");
  else
	printf("No levanta el articulo buscado \n");
  
  printf("Respuesta: %s \n",(stArticle *)&respuesta->sBody);
  int i;
  for(i=0; i<tamanioArticle;i++)
    printf("respuesta[%2d]: %4d \n",i,respuesta[i]);
  
*/

/*
  for(x= 0; x < 3; x++)
    {
      key_length[x] = strlen(keys[x]);
      value_length[x] = strlen(values[x]);

      rc= memcached_set(memc, keys[x], key_length[x], values[x],
                        value_length[x], (time_t)0, (uint32_t)0);
      if (rc == MEMCACHED_SUCCESS)
        fprintf(stderr,"Key %s stored successfully\n",keys[x]);
      else
        fprintf(stderr,"Couldn't store key: %s\n",memcached_strerror(memc, rc));
    }

aca estaba el get

  if (rc == MEMCACHED_SUCCESS)
    {
      while ((return_value= memcached_fetch(memc, return_key, &return_key_length,
                                            &return_value_length, &flags, &rc)) != NULL)
        {
          if (rc == MEMCACHED_SUCCESS)
            {
              fprintf(stderr,"Key %s returned %s\n",return_key, return_value);
            }
        }
    }
*/
  return 0;
}
