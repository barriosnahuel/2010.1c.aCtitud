#include"funcionesMemcached.h"
#include <libmemcached/memcached.h>

#ifndef FUNCIONES_MEMCACHED
#define FUNCIONES_MEMCACHED

typedef struct _largoArticulo{
  size_t largoHead;
  size_t largoBody;
}t_news_largos;

typedef struct _articuloCache{
  t_news_largos datos;  
  char *head;
  char *body;
}t_news;

memcached_st iniciarClusterCache();


void guardarNoticiaEnCache(stArticle stArticulo, char* sGrupoDeNoticias);

/**
 * Busca la noticia en la cache, y setea el stArticulo con esa noticia.
 */
int buscarNoticiaEnCache(stArticle* pstArticulo, char* sGrupoDeNoticias, char* sArticleID, memcached_st memc);

#endif