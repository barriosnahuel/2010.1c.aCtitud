#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include "logger.h"

void LoguearInformacion(char *czData) {
	EscribirLog(1,czData);
};

void LoguearWarning(char *czData) {
	EscribirLog(2,czData);
};

void LoguearError(char *czData) {
	EscribirLog(3,czData);
};

void LoguearDebugging(char *czData) {
    EscribirLog(4,czData);
};

void EscribirLog(char cTipoLog, char *czData) {

   FILE *arch;
   time_t tiempoActual;
   extern char czNombreProceso[20];
   char *czNombreArch;
   char czFecha[50];
   pid_t tid;

   mkdir("./logs",0755);
   czNombreArch = (char *)malloc(40);
   memset(czNombreArch, 0, 40);
   sprintf(czNombreArch, "./logs/%s%d.LOG", czNombreProceso, getpid());

   /* se abre o crea al archivo log */
   if((arch = fopen(czNombreArch, "a+")) == NULL)
      perror("Error al generar el archivo Log.");
   else
      stderr = arch;

   /*Se obtiene el tiempo actual*/
   tiempoActual = time(NULL);

   /*transforma los datos de fecha y hora a un formato de cadena*/
   strftime(czFecha, 50, "%d/%m/%Y %T", localtime(&tiempoActual));

    /*se obtiene el id del thread*/
   tid = (pid_t) syscall(SYS_gettid);

	/*registramos todo en el archivo */
	fprintf(stderr, "%s %s [%d][%d] ",czFecha,czNombreProceso,getpid(),tid);
	switch(cTipoLog) {
		case _INFO:
			fprintf(stderr, "%s:\t%s\n","INFO",czData);
			break;
		case _WARN:
            fprintf(stderr, "%s:\t%s\n","WARN",czData);
			break;
		case _ERROR:
            fprintf(stderr, "%s:\t%s\n","ERROR",czData);
            printf("%s:\t%s\n","ERROR",czData);
			break;
		case _DEBUG:
            fprintf(stderr, "%s:\t%s\n","DEBUG",czData);
			break;
		default:
            fprintf(stderr, "%s:\t%s\n","MSG",czData);
			break;
	};

   if(arch) fclose(arch);
   free(czNombreArch);
}
