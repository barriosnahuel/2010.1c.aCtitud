#ifndef FUNCIONES_MEMCACHED
#define FUNCIONES_MEMCACHED

#include <libmemcached/memcached.h>
typedef struct _largoArticulo{
  size_t largoHead;
  size_t largoBody;
}t_news_largos;

typedef struct _articuloCache{
  t_news_largos datos;  
  char *head;
  char *body;
}t_news;

/*memc es el ptro a la memcached, se usa siempre*/
void iniciarClusterCache(memcached_st* memc);
void guardarNoticiaEnCache(stArticle stArticulo, char* sGrupoDeNoticias, memcached_st * memc);
int buscarNoticiaEnCache(stArticle* pstArticulo, char* sGrupoDeNoticias, char* sArticleID, memcached_st* memc);
#endif