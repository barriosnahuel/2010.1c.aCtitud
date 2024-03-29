#ifndef FUNCIONES_MEMCACHED
#define FUNCIONES_MEMCACHED
#include <libmemcached/memcached.h>
#include "../Article.h"
#include "../Logger/logger.h"

typedef struct _largoArticulo{
  size_t largoHead;
  size_t largoBody;
}t_news_largos;

typedef struct _articuloCache{
  t_news_largos datos;  
  char *head;
  char *body;
}t_news;

void iniciarClusterCache(memcached_st **memc,char* memcachedServer1,int memcachedServer1Puerto,char* memcachedServer2,int memcachedServer2Puerto);
void guardarNoticiaEnCache(stArticle stArticulo, char* sGrupoDeNoticias,char* grupoSinEspacios,memcached_st **memc);
int buscarNoticiaEnCache(stArticle* pstArticulo, char* sGrupoDeNoticias, char* sArticleID,char* grupoSinEspacios, memcached_st **memc);
#endif 
