#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libmemcached/memcached.h>

int main(int argc, char *argv[])
{
  memcached_server_st *servers = NULL;
  memcached_st *memc;
  memcached_return rc;
  char *keys[]= {"huey", "dewey", "louie"};
  size_t key_length[3];
  char *values[]= {"red", "blue", "green"};
  size_t value_length[3];
  unsigned int x;
  uint32_t flags;

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
  
  if (rc == MEMCACHED_SUCCESS)
    fprintf(stderr,"Se agrego el servidor correctamente\n");
  else
    fprintf(stderr,"No se pudo agregar el servidor: %s\n",memcached_strerror(memc, rc));
	
  /*If( Si esta en la cache )
	 devuelve el resultado
	else
	  consulto en openDS
	  e inserto en la cache:
  */ 
	  tamañoID		 = srtlen(article.uiArticleID);
	  tamañoArticle = sizeof(article);
	  rc =memcache_set(memc,article.uiArticleID,tamañoID,article,tamañoArticle,(time_t)0,(uint32_t)0);
	  if (rc=MEMCACHED_SUCESS)
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
