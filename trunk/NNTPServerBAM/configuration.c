#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logger.h"
#include "configuration.h"

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
	char *szLinea, *pszAux, szMsgLog[100];

	szLinea = (char *)malloc(100+2); // línea que se va a leer
	pszAux = (char *)malloc(1000); // se va a guardar todo el archivo

	if((pfsArchConfig = fopen(pszNombreArchivo,"rt")) == NULL) {
	        sprintf(szMsgLog,"No se pudo abrir el archivo de configuracion %s.",pszNombreArchivo);
	        LoguearError(szMsgLog);
		return 0;
	}
	pszAux[0] = '\0';

	// cargo todo el archivo en el buffer
	while(!feof(pfsArchConfig)) {
		fgets(szLinea, 100, pfsArchConfig);
		if (*szLinea == '\n' || *szLinea == '#')
			continue;
		strcat(pszAux, szLinea);
	}

	fclose(pfsArchConfig);

	// cargo el puerto del servidor openDS
	stConf->uiOpenDSPort = atoi(GetVal("OPENDS_PORT=", pszAux));
	if((stConf->uiOpenDSPort) < 1 || (stConf->uiOpenDSPort) > 65535 ) {
	    LoguearError("Puerto invalido para servidor Open DS.");
		return 0;
	};

    // cargo la IP del servidor openDS
	strcpy(stConf->czOpenDSServer,GetVal("OPENDS_SERVER=", pszAux));
	if(!Valida_IP(stConf->czOpenDSServer)) {
        LoguearError("IP invalida para servidor Open DS.");
		return 0;
    };

	// cargo el puerto local
	stConf->uiLocalPort = atoi(GetVal("LOCAL_PORT=", pszAux));
	if((stConf->uiLocalPort) < 1 || (stConf->uiLocalPort) > 65535 ) {
	    LoguearError("Puerto local invalido.");
		return 0;
	};

    // cargo la IP local
	strcpy(stConf->czLocalIP,GetVal("LOCAL_IP=", pszAux));
	if(!Valida_IP(stConf->czLocalIP)) {
        LoguearError("IP local invalida.");
		return 0;
    };
    
	free(szLinea);
	free(pszAux);
	return 1;
}
int LoadConfiguration(int argn, char *argv[], stConfiguracion *stConf) {

	char szMsgLog[100];

    LoguearInformacion("Cargando configuracion...");

	switch(argn) {
		case 1: // no se indicó archivo, se carga archivo por defecto
			if(!CargaConfiguracion("server.conf", stConf)) {
				return 0;
			}
			break;
		case 2: // único parámetro: nombre de archivo de configuración
			if(!CargaConfiguracion(argv[1], stConf)) {
				return 0;
			}
			break;
		case 5: // se pasaron los valores por parámetro
			strcpy(stConf->czLocalIP,argv[1]);
	        if(!Valida_IP(stConf->czLocalIP)) {
                LoguearError("IP local invalida.");
		        return 0;
            };

			stConf->uiLocalPort = atoi(argv[2]);
	        if((stConf->uiLocalPort) < 1 || (stConf->uiLocalPort) > 65535 ) {
	            LoguearError("Puerto local invalido.");
		        return 0;
	        };

			strcpy(stConf->czOpenDSServer,argv[3]);
	        if(!Valida_IP(stConf->czOpenDSServer)) {
                LoguearError("IP invalida para servidor Open DS.");
		        return 0;
            };

			stConf->uiOpenDSPort = atoi(argv[4]);
	        if((stConf->uiOpenDSPort) < 1 || (stConf->uiOpenDSPort) > 65535 ) {
	            LoguearError("Puerto invalido para servidor Open DS.");
		        return 0;
	        };
			break;
		default:
			LoguearError("Cantidad de argumentos no valida.");
			return 0;
			break;
	}
	LoguearInformacion("Configuracion cargada correctamente.");

	sprintf(szMsgLog,"IP Local: %s - Puerto Local: %u",stConf->czLocalIP,stConf->uiLocalPort);
	LoguearInformacion(szMsgLog);
	sprintf(szMsgLog,"IP Open DS: %s - Puerto Open DS: %u",stConf->czOpenDSServer,stConf->uiOpenDSPort);
	LoguearInformacion(szMsgLog);
	
	return 1;
}
