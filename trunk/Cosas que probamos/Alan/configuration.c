#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "configuration.h"

/*	Estas constantes son las que mapeo con el archivo de configuracion.	*/
#define APP_PORT "AppPort="
#define OPENDS_SERVER "OpenDSServer="
#define OPENDS_PORT "OpenDSPort="
#define APP_NAME_FOR_LOGGER "AppNameForLogger="
#define MEMCACHED_SERVER1_IP "MemcachedServer1IP="
#define MEMCACHED_SERVER1_PORT "MemcachedServer1Port="
#define MEMCACHED_SERVER2_IP "MemcachedServer2IP="
#define MEMCACHED_SERVER2_PORT "MemcachedServer2Port="

char *GetVal(const char *sValBuff, const char *sBuff){
    char *sVal;
    char *sVal2;

    sVal = (char *)malloc(50);
    sVal2 = (char *)malloc(2);
    memset(sVal2,0,2);
    strcpy(sVal2,"0");

    sVal[0] = '\0';
    sVal = strstr(sBuff, sValBuff);
    sVal = strtok(sVal, "=");
    if(!sVal) return sVal2;
    sVal = strtok(NULL, "\n");
    sVal[strlen(sVal)] = '\0';
    return sVal;
}

int Valida_IP(const char *ip) {
   int tam, cont;
   char *ptr, ipaux[15+1];
   cont = 0;

   if (!ip) return 0;
   strcpy(ipaux, ip);
   ptr = strtok(ipaux, "." );
   while(ptr) {
      tam = strlen(ptr);
      if ( tam < 1 || tam > 3 ) return 0;
      if ( atoi(ptr) < 0 || atoi(ptr) > 255 ) return 0;
      ptr = strtok ( NULL, "." );
      cont = cont + 1;
   }
   if (cont < 4) return 0;
   return 1;
}

int CargaConfiguracion(char *pszNombreArchivo, stConfiguracion *stConf) {
	FILE *pfsArchConfig;
	char *szLinea, *pszAux;

	szLinea = (char *)malloc(100+2); /* l�nea que se va a leer */
	pszAux = (char *)malloc(1000); /* se va a guardar todo el archivo */

	if((pfsArchConfig = fopen(pszNombreArchivo,"rt")) == NULL) {
	        printf("No se pudo abrir el archivo de configuracion %s.\n",pszNombreArchivo);
		return 0;
	}
	pszAux[0] = '\0';

	/* cargo todo el archivo en el buffer */
	while(!feof(pfsArchConfig)) {
		fgets(szLinea, 100, pfsArchConfig);
		if (*szLinea == '\n' || *szLinea == '#')
			continue;
		strcat(pszAux, szLinea);
	}

	fclose(pfsArchConfig);

	/* ------ modificar de ac� para abajo los valores que se quieren guardar --------*/
	
	/* Cargo las IPS y los puertos de los servidores memcached	*/
	stConf->memcachedServer2Puerto= atoi(GetVal(MEMCACHED_SERVER2_PORT, pszAux));
	if((stConf->memcachedServer2Puerto) < 1 || (stConf->memcachedServer2Puerto) > 65535 ) {
	    printf("Puerto de memcached server 2 inválido.");
		return 0;
	}

	strcpy(stConf->memcachedServer2,GetVal(MEMCACHED_SERVER2_IP, pszAux));
	/* valido la IP */
	if(!Valida_IP(stConf->memcachedServer2)) {
        printf("IP de memcached server 2 inválida.");
		return 0;
    };
	
	stConf->memcachedServer1Puerto= atoi(GetVal(MEMCACHED_SERVER1_PORT, pszAux));
	if((stConf->memcachedServer1Puerto) < 1 || (stConf->memcachedServer1Puerto) > 65535 ) {
	    printf("Puerto de memcached server 1 inválido.");
		return 0;
	}
	
	strcpy(stConf->memcachedServer1,GetVal(MEMCACHED_SERVER1_IP, pszAux));
	/* valido la IP */
	if(!Valida_IP(stConf->memcachedServer1)) {
        printf("IP de memcached server 1 inválida.");
		return 0;
    };
	
	/* Cargo el puerto de OpenDS */
	stConf->uiBDPuerto = atoi(GetVal(OPENDS_PORT, pszAux));
	if((stConf->uiBDPuerto) < 1 || (stConf->uiBDPuerto) > 65535 ) {
	    printf("Puerto de OpenDS invalido.");
		return 0;
	}

    /* Cargo la IP de OpenDS */
	strcpy(stConf->czBDServer,GetVal(OPENDS_SERVER, pszAux));
	/* valido la IP */
	if(!Valida_IP(stConf->czBDServer)) {
        printf("IP de OpenDS invalida.");
		return 0;
    };

	/* Cargo el puerto de la aplicacion */
	stConf->uiAppPuerto= atoi(GetVal(APP_PORT, pszAux));
	if((stConf->uiAppPuerto) < 1 || (stConf->uiAppPuerto) > 65535 ) {
	    printf("Puerto de la aplicacion invalido.");
		return 0;
	}
     
	
	
	


	
	
	free(szLinea);
	free(pszAux);
	return 1;
}
