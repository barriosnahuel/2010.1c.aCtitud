#include<string.h>
#include<stdio.h>
#include <unistd.h>
#include<stdlib.h>
#include"funcionesMemcached.h"
#include <ctype.h>
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
  if (rc == MEMCACHED_SUCCESS)
	LoguearInformacion("Se agrego el servidor  1 correctamente.", APP_NAME_FOR_LOGGER);
  else
	LoguearError("No se pudo agregar el servidor 1. ", APP_NAME_FOR_LOGGER);

  rc = memcached_server_add(*memCluster, memcachedServer2,memcachedServer2Puerto); 
  if (rc == MEMCACHED_SUCCESS)
	LoguearInformacion("Se agrego el servidor  2 correctamente.", APP_NAME_FOR_LOGGER);
  else
	LoguearError("No se pudo agregar el servidor 2. ", APP_NAME_FOR_LOGGER);
  
  return;
}

void guardarNoticiaEnCache(stArticle article, char *sGrupoDeNoticias ,memcached_st **memc)
{

  memcached_return rc;
  uint32_t flags;
  t_news *articuloCache = malloc(sizeof(t_news));
  
  char *ID;
  char *claveCache ;
  int largoID;
  int largoGrupoDeNoticias;
  
  largoID = sizeof(article.uiArticleID);
  ID = malloc(largoID);
  sprintf(ID,"%d",article.uiArticleID);
  largoID = strlen(ID);
  largoGrupoDeNoticias = strlen(sGrupoDeNoticias) + 1;
  claveCache = malloc(largoGrupoDeNoticias+largoID);
  sprintf(claveCache,"%s%s",sGrupoDeNoticias,ID);
  
  /*printf("CLAVE CACHE %s \n",claveCache);*/
  
  articuloCache->body = NULL;
  articuloCache->head = NULL;
  articuloCache->datos.largoHead = strlen(article.sHead) + 1;	
  articuloCache->datos.largoBody = strlen(article.sBody) +1;
  articuloCache->head= malloc(articuloCache->datos.largoHead);
  articuloCache->body=malloc(articuloCache->datos.largoBody);
  strcpy(articuloCache->head,article.sHead);
  strcpy(articuloCache->body,article.sBody);
  char * articuloEnBytes;
  size_t  articuloEnBytesLargo;
  articuloEnBytesLargo=sizeof(t_news_largos)+articuloCache->datos.largoHead+articuloCache->datos.largoBody;
  articuloEnBytes = malloc(articuloEnBytesLargo);

  memcpy(articuloEnBytes,(char*)&articuloCache->datos,sizeof(t_news_largos));
  memcpy(articuloEnBytes+sizeof(t_news_largos),articuloCache->head,articuloCache->datos.largoHead);
  memcpy(articuloEnBytes+sizeof(t_news_largos)+articuloCache->datos.largoHead,articuloCache->body,articuloCache->datos.largoBody);
  rc=memcached_set(*memc,claveCache,strlen(claveCache),articuloEnBytes,articuloEnBytesLargo,0,0);

/*printf("articuloEnBytes:%d \n",articuloEnBytes);
  printf("articuloEnBytes+sizeof(t_news_largos):%d \n",articuloEnBytes+sizeof(t_news_largos));
  printf("articuloEnBytes+sizeof(t_news_largos)+articuloCache->datos.largoHead:%d \n",articuloEnBytes+sizeof(t_news_largos)+articuloCache->datos.largoHead);
*/  
  if (rc == MEMCACHED_SUCCESS){
	LoguearInformacion("Se inserto correctamente el articulo en la cache.", APP_NAME_FOR_LOGGER);
	printf("Se inserto correctamente el articulo en la cache\n");
  }else{
	LoguearError("No se pudo insertar el articulo en la cache.", APP_NAME_FOR_LOGGER);
	printf("No se pudo insertar el articulo en la cache\n");	
  }
  free(articuloEnBytes);
  free(articuloCache);
  free(claveCache);
  return;
  
}

void sacarEspaciosEnGrupo(char* grupo)
{ 
printf("LLEGA A LA FUNCION1! \n");

	int i ;
	int j;
	char * grupoSinEspacios = malloc(strlen(*grupo)+1);
printf("LLEGA A LA FUNCION2! \n");
	char * copiaGrupo = malloc(strlen(*grupo)+1);
	copiaGrupo=grupo;
	printf("EL GRUPO QUE LE LLEGA A LA FUNCION : %s \n", grupo);
	for(i=0,j=0;i<=strlen(copiaGrupo);i++){
		if(!isspace(copiaGrupo[i])){
			grupoSinEspacios[j]= copiaGrupo[i];
			j++;
		}else printf("habia un espacio \n");	
	}
	grupoSinEspacios[i]='\0';
	printf("GRUPO SIN ESPACIOS %s \n", grupoSinEspacios);
	grupo = grupoSinEspacios;
	printf("Con lo que QUEDA GRUPO : %s\n",*grupo);
	return;
}

int buscarNoticiaEnCache(stArticle* pstArticulo, char* sGrupoDeNoticias, char* sArticleID, memcached_st **memc)
{
printf("##################### BUSQUEDA EN LA CACHE ######################\n");
  
  uint32_t flags;
printf("1");
  memcached_return rc;
printf("2");
  t_news *resultNoticia = malloc(sizeof(t_news));
printf("3");
  char *resultadoCache  = NULL; 
printf("4"); 
  int resultNoticiaEnBytes_largo, resultado;
printf("pasa por aca\n ");
  
  sacarEspaciosEnGrupo(sGrupoDeNoticias);
  printf("Grupo sin espacios %s \n",sGrupoDeNoticias);
  
  char* claveCache;
  int largoID = strlen(sArticleID) + 1;
  int largoGrupoDeNoticias = strlen(sGrupoDeNoticias) + 1;
  claveCache = malloc(largoGrupoDeNoticias+largoID);
  sprintf(claveCache,"%s%s",sGrupoDeNoticias,sArticleID);
  printf("Clave a buscar en la cache %s \n",claveCache);
    
  resultadoCache = memcached_get(*memc,claveCache,strlen(claveCache),&resultNoticiaEnBytes_largo,&flags,&rc);
  if(rc==MEMCACHED_SUCCESS){
	LoguearInformacion("Se encontro el articulo en la cache.", APP_NAME_FOR_LOGGER);
	printf("Se encontro el articulo en la cache\n");
  }else{
  	LoguearError("No se encontro el articulo en la cache.", APP_NAME_FOR_LOGGER);
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
}
