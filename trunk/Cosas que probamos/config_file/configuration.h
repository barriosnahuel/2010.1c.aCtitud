#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H

/* estructura que guarda la configuraci�n
 * se debe modificar acorde a los datos
 * que se tomar�n del archivo */
typedef struct stConfiguracion {
    char czServer[15+1];  //se supone que es una direcci�n IP
    unsigned int uiPuerto;
} stConfiguracion;

/* funci�n que valida que una ip se haya ingresado correctamente */
int Valida_IP(const char *ip);

/* funci�n que toma un valor del archivo del configuraci�n */
char *GetVal(const char *sValBuff, const char *sBuff);

/* funci�n que carga el archivo indicado. se debe modificar acorde 
 * a la configuraci�n que se desee cargar */
int CargaConfiguracion(char *pszNombreArchivo, stConfiguracion *stConf);

#endif
