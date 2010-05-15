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

void guardarNoticiaEnCache(stArticle stArticulo);
#endif