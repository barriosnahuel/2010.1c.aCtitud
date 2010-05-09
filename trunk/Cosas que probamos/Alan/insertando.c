#include <stdio.h>
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

  stArticle * article;
  article.sBody= "body probando hibernate!";
  article.sHead= "head probando hibernate!";
  article.sNewsgroup= "blablabla.com";
  article.uiArticleID= 6969;
  
  if (rc == MEMCACHED_SUCCESS)
    fprintf(stderr,"Se agrego el servidor correctamente\n");
  else
    fprintf(stderr,"No se pudo agregar el servidor: %s\n",memcached_strerror(memc, rc));
	
  tamanioID		 = (size_t)(srtlen(article.uiArticleID));
  tamanioArticle = (size_t)(sizeof(article));
  rc =memcached_set(memc,article.uiArticleID,tamanioID,article,tamanioArticle,(time_t)0,(uint32_t)0);
  if (rc == MEMCACHED_SUCCESS)
	printf("El articulo se inserto correctamente");
  else
    printf("No se logro insertar el articulo");
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
