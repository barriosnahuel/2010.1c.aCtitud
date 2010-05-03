#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    sVal[strlen(sVal)-1] = '\0';
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

	szLinea = (char *)malloc(100+2); // línea que se va a leer
	pszAux = (char *)malloc(1000); // se va a guardar todo el archivo

	if((pfsArchConfig = fopen(pszNombreArchivo,"rt")) == NULL) {
	        printf("No se pudo abrir el archivo de configuracion %s.\n",pszNombreArchivo);
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

	/* ------ modificar de acá para abajo los valores que se quieren guardar --------*/

	// cargo el puerto
	stConf->uiPuerto = atoi(GetVal("PORT=", pszAux));
	if((stConf->uiPuerto) < 1 || (stConf->uiPuerto) > 65535 ) {
	    printf("Puerto invalido.");
		return 0;
	}

    // cargo la IP
	strcpy(stConf->czServer,GetVal("SERVER=", pszAux));
	// valido la IP
	if(!Valida_IP(stConf->czServer)) {
        printf("IP invalida.");
		return 0;
    };

	free(szLinea);
	free(pszAux);
	return 1;
}
