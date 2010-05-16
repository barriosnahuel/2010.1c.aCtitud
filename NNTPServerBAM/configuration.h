#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H

/* estructura que guarda la configuraci�n */
typedef struct stConfiguracion {
    char czLocalIP[15+1];  //se supone que es una direcci�n IP
    unsigned int uiLocalPort;
    char czOpenDSServer[15+1];
    unsigned int uiOpenDSPort;
} stConfiguracion;

/* funci�n que valida que una ip se haya ingresado correctamente */
int Valida_IP(const char *ip);

/* funci�n que toma un valor del archivo del configuraci�n */
char *GetVal(const char *sValBuff, const char *sBuff);

/* funci�n que carga el archivo indicado */
int CargaConfiguracion(char *pszNombreArchivo, stConfiguracion *stConf);

/* verifica los argumentos para saber c�mo cargar la conf */
int LoadConfiguration(int argn, char *argv[], stConfiguracion *stConf);
#endif
