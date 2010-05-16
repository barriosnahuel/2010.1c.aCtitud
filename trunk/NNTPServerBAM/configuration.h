#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H

/* estructura que guarda la configuración */
typedef struct stConfiguracion {
    char czLocalIP[15+1];  //se supone que es una dirección IP
    unsigned int uiLocalPort;
    char czOpenDSServer[15+1];
    unsigned int uiOpenDSPort;
} stConfiguracion;

/* función que valida que una ip se haya ingresado correctamente */
int Valida_IP(const char *ip);

/* función que toma un valor del archivo del configuración */
char *GetVal(const char *sValBuff, const char *sBuff);

/* función que carga el archivo indicado */
int CargaConfiguracion(char *pszNombreArchivo, stConfiguracion *stConf);

/* verifica los argumentos para saber cómo cargar la conf */
int LoadConfiguration(int argn, char *argv[], stConfiguracion *stConf);
#endif
