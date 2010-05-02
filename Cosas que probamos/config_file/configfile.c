#include "configuration.h"

int main(int argn, char *argv[]){

	stConfiguracion stConf;

    char czNombreProceso[20];
    memset(czNombreProceso, 0, 20);
    strcpy(czNombreProceso, "configuration\0");
    strcpy(argv[0], czNombreProceso);

	printf("\e[2J\e[1;1H");
	printf("\n\n\e[1;33m");
	printf("La Casa Esta?En Orden (c) 2005");
	printf("\e[0m\n\n");

	
	switch(argn) {
		case 1: // no se indicó archivo, se carga archivo por defecto (igual nombre que czNombreProceso)
			if(!CargaConfPer("configuration.conf\0", &stConf)) {
				printf("Archivo de configuración no válido.\n");
				return -1;
			}
			break;
		case 2: // único parámetro: nombre de archivo de configuración
			if(!CargaConfPer(argv[1], &stConf)) {
				printf("Archivo de configuración no válido.\n");
				return -1;
			}
			break;
		case 3: // se pasaron los valores  por parámetro
			
			/* primer parámetro: dirección IP del server
			   ATENCIÓN: la estructura fue definida como un cha[15+1] suponiendo que se recibe
			   una IP. si es otra cosa tendría que ser char* y reservar memoria en este momento */
			strcpy(stConf.czServer,argv[1]);
			
			/* segundo parámetro: puerto del server */
			stConf.uiPuerto = atoi(argv[2]);
			break;
		default:
			printf("Otro caso.\n");
			break;
	}
	
	printf("Configuración cargada");
	printf("Se conectará a %s puerto %d.",stConf.czServer,stConf.uiPuerto);
}
