#include <stdio.h>
#include <string.h>
#include "configuration.h"

int main(int argn, char *argv[]){

	stConfiguracion stConf;

    char czNombreProceso[20];
    memset(czNombreProceso, 0, 20);
    strcpy(czNombreProceso, "config\0");
    strcpy(argv[0], czNombreProceso);

	switch(argn) {
		case 1: // no se indic� archivo, se carga archivo por defecto (igual nombre que czNombreProceso)
			if(!CargaConfiguracion("config.conf\0", &stConf)) {
				printf("Archivo de configuraci�n no v�lido.\n");
				return -1;
			}
			break;
		case 2: // �nico par�metro: nombre de archivo de configuraci�n
			if(!CargaConfiguracion(argv[1], &stConf)) {
				printf("Archivo de configuraci�n no v�lido.\n");
				return -1;
			}
			break;
		case 3: // se pasaron los valores por par�metro
			
			/* primer par�metro: direcci�n IP del server
			   ATENCI�N: la estructura fue definida como un cha[15+1] suponiendo que se recibe
			   una IP. si es otra cosa tendr�a que ser char* y reservar memoria en este momento */
			strcpy(stConf.czServer,argv[1]);
			
			/* segundo par�metro: puerto del server */
			stConf.uiPuerto = atoi(argv[2]);
			break;
		default:
			printf("Otro caso.\n");
			break;
	}
	
	printf("\nConfiguracion cargada\n");
	printf("\nSe conectara a %s puerto %u.\n",stConf.czServer,stConf.uiPuerto);
    return 1;
}
