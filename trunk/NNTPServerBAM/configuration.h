#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H

	/* estructura que guarda la configuracion */
	typedef struct stConfiguracion {
		char czLocalIP[15+1];  /*	se supone que es una direccion IP	*/
		unsigned int uiLocalPort;
		char czOpenDSServer[15+1];
		unsigned int uiOpenDSPort;
		int iSockServer;
		int iSockClient;
	} stConfiguracion;

	/* funcion que valida que una ip se haya ingresado correctamente */
	int Valida_IP(const char *ip);

	/* funcion que toma un valor del archivo del configuracion */
	char *GetVal(const char *sValBuff, const char *sBuff);

	/* funcion que carga el archivo indicado */
	int CargaConfiguracion(char *pszNombreArchivo, stConfiguracion *stConf);

	/* verifica los argumentos para saber como cargar la conf */
	int LoadConfiguration(int argn, char *argv[], stConfiguracion *stConf);

#endif
