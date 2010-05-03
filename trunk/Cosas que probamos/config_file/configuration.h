#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H

/* estructura que guarda la configuración
 * se debe modificar acorde a los datos
 * que se tomarán del archivo */
typedef struct stConfiguracion {
    char czServer[15+1];  //se supone que es una dirección IP
    unsigned int uiPuerto;
} stConfiguracion;

/* función que valida que una ip se haya ingresado correctamente */
int Valida_IP(const char *ip);

/* función que toma un valor del archivo del configuración */
char *GetVal(const char *sValBuff, const char *sBuff);

/* función que carga el archivo indicado. se debe modificar acorde 
 * a la configuración que se desee cargar */
int CargaConfiguracion(char *pszNombreArchivo, stConfiguracion *stConf);

#endif
