#include<string.h>
#include<stdio.h>
#include <unistd.h>
#include<stdlib.h>
#include <libmemcached/memcached.h>
#include "Article.h"
typedef struct _largoArticulo{
   int largoHead;
   int largoBody;
}t_news_largos;

typedef struct _articuloCache{
   t_news_largos datos;
   char *head = NULL;
   char *body = NULL;
}t_news;

int main(int argc, char *argv[])
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
  servers= memcached_server_list_append(servers, "localhost", 11211, &rc);
  rc= memcached_server_push(memc, servers);

  stArticle article;
  stArticle * articleRespuesta;
  article.sBody= "body";
  article.sHead= "head!";
  article.uiArticleID= 6969;
  char probando[]="1111";

  stArticle * ptrArticulo = &article ;//memcached necesita ptros
  printf("Se creara  el servidor 1 \n");

  if (rc == MEMCACHED_SUCCESS)
    fprintf(stderr,"Se agrego el servidor correctamente\n");
  else
    fprintf(stderr,"No se pudo agregar el servidor: %s\n",memcached_strerror(memc, rc));

  //  tamanioID=sizeof(article.uiArticleID)+1;
//  char ID[ tamanioID + 1];
//  sprintf(ID,"%d",ptrArticulo->uiArticleID);
//  printf("Convirtio el ID a un char* : %s ? \n",ID);

 // char ID[1000]; //= malloc(sizeof(article.uiArticleID)+1);
 // sprintf(ID,"%s",article.uiArticleID);
 //  printf("El ID quedon con: %s",article.uiArticleID);

  t_news * articuloCache         = malloc(sizeof(t_news));
  articuloCache->datos.largoHead = strlen(article.sHead);
  printf("largo head %d \n",articuloCache->datos.largoHead);
  articuloCache->datos.largoBody = strlen(article.sBody);
  printf("largo body  %d \n",articuloCache->datos.largoBody);
  articuloCache->head            =malloc(articuloCache->datos.largoHead);
  articuloCache->body         =malloc(articuloCache->datos.largoBody);
  strcpy(articuloCache->head,article.sHead);
  strcpy(articuloCache->body,article.sBody);

  char * articuloEnBytes;
  int  articuloEnBytesLargo;
  articuloEnBytesLargo=sizeof(t_news_largos)+articuloCache->datos.largoHead+articuloCache->datos.largoBody;
  printf("Largo estructura(t_news_largos): %d \n",sizeof(t_news_largos));
  articuloEnBytes = malloc(articuloEnBytesLargo);

memcpy(articuloEnBytes,&articuloCache->datos,sizeof(t_news_largos)); //MMM PUEDE SER POR ESTO QUE NO ANDE POR EL &articuloCache
memcpy(articuloEnBytes+sizeof(t_news_largos),articuloCache->head,articuloCache->datos.largoHead);
memcpy(articuloEnBytes+sizeof(t_news_largos)+articuloCache->datos.largoHead,articuloCache->body,articuloCache->datos.largoBody);
rc=memcached_set(memc,probando,strlen(probando),articuloEnBytes,articuloEnBytesLargo,0,0);

  if(rc=MEMCACHED_SUCCESS)
   printf("Se inserto correctamente \n");
  else
   printf("No se pudo insertar \n");

return 0;
}

  


/*#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libmemcached/memcached.h>
#include "Article.h"
int main(int argc, char *argv[])
{
  memcached_server_st *servers = NULL;
  memcached_st *memc;
  memcached_return rc;
  char *keys[]= {"huey", "dewey", "louie"};
  size_t key_length[3];
  char *values[]= {"red", "blue", "green"}; 
  unsigned int x;
  uint32_t flags;
  size_t tamanioID;
  size_t tamanioArticle;
  char return_key[MEMCACHED_MAX_KEY];
  size_t return_key_length;
  char *return_value;
  size_t return_value_length;

  memc= memcached_create(NULL);
  servers= memcached_server_list_append(servers, "localhost", 11211, &rc);
  rc= memcached_server_push(memc, servers);

  stArticle article;
  article.sBody= "body probando hibernate!";
  article.sHead= "head probando hibernate!";
  article.sNewsgroup= "blablabla.com";
  article.uiArticleID= 6969;
  
  stArticle * ptrArticulo = &article;
  printf("Se creara el servidor 1 \n");
  if (rc == MEMCACHED_SUCCESS)
    fprintf(stderr,"Se agrego el servidor correctamente\n");
  elsenn
  
    fprintf(stderr,"No se pudo agregar el servidor: %s\n",memcached_strerror(memc, rc));
  
  tamanioID		 = sizeof(ptrArticulo->uiArticleID);
  tamanioArticle = sizeof(article);
  printf("Tamanio ID : %d",tamanioID);
  printf("Tamanio articulo entero: %d",tamanioArticle);
  rc =memcached_set(memc,(char*)&ptrArticulo->uiArticleID,tamanioID,(char*)&ptrArticulo,tamanioArticle,(time_t)0,(uint32_t)0);
  if (rc == MEMCACHED_SUCCESS)
	printf("El articulo se inserto correctamente =) \n");
  else
    printf("No se logro insertar el articulo =( \n");
	
 respuesta = memcached_get(memc,(char*)&ptrArticulo->uiArticleID,tamanioID,&tamanioArticle,&$
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

  rc= memcached_mget(memc, keys, key_length, 3);

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