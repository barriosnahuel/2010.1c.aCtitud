#include "logger.hpp"

void LoguearInformacion(char *czData, char *czNombreProceso) {
	EscribirLog(1,czData);
};


void LoguearWarning(char *czData, char *czNombreProceso) {
	EscribirLog(2,czData);
};

void LoguearError(char *czData, char *czNombreProceso) {
	EscribirLog(3,czData);
};

void LoguearDebugging(char *czData, char *czNombreProceso) {
	EscribirLog(4,czData);
};

void EscribirLog(char cTipoLog, char *czData) {

   FILE *arch;
   time_t tiempoActual;
   extern char czNomProc[20];
   char *czNombreArch;
   char czFecha[50];

   mkdir("./logs",0755);
   czNombreArch = (char *)malloc(40);
   memset(czNombreArch, 0, 40);
   sprintf(czNombreArch, "./logs/%s%d.LOG", czNomProc, getpid());

   /* se abre o crea al archivo log */
   if((arch = fopen(czNombreArch, "a+")) == NULL)
      perror("Error al generar el archivo Log.");
   else
      stderr = arch;

   /*Se obtiene el tiempo actual*/
   tiempoActual = time(NULL);

   /*transforma los datos de fecha y hora a un formato de cadena*/
   strftime(czFecha, 50, "%d/%m/%Y %T", localtime(&tiempoActual));

	/*registramos todo en el archivo */
	fprintf(stderr, "%s %s [%d][%d] ",czFecha,czNombreProceso,getpid(),gettid());
	switch(cTipoLog) {
		case _INFO:
			fprintf(stderr, "%s: %s\n","INFO",czData);
			break;
		case _WARN:
            fprintf(stderr, "%s: %s\n","WARN",czData);
			break;
		case _ERROR:
            fprintf(stderr, "%s: %s\n","ERROR",czData);
			break;
		case _DEBUG:
            fprintf(stderr, "%s: %s\n","DEBUG",czData);
			break;
		default:
            fprintf(stderr, "%s: %s\n","MSG",czData);
			break;
	};

   if(arch) fclose(arch);
   free(czNombreArch);
}
