#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H


/* estructura que guarda la configuracion
 * se debe modificar acorde a los datos
 * que se tomaran del archivo */
typedef struct stConfiguracion {
    unsigned int uiAppPuerto;	/*	Puerto sobre el que estara escuchando la aplicacion.	*/

	char czBDServer[15+1];  	/* se supone que es una direccion IP */
    unsigned int uiBDPuerto;
} stConfiguracion;

/* funcion que valida que una ip se haya ingresado correctamente */
int Valida_IP(const char *ip);

/* funcion que toma un valor del archivo del configuracion */
char *GetVal(const char *sValBuff, const char *sBuff);

/* funcion que carga el archivo indicado. se debe modificar acorde
 * a la configuracion que se desee cargar */
int CargaConfiguracion(char *pszNombreArchivo, stConfiguracion *stConf);

#endif
