#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include "logger.hpp"

class Logger {
	private:
		FILE *arch;
		time_t tiempoActual;
		char *czNombreArchivo;
		char czFecha[50];

		void PrepararArchivoLogProceso(char *czNombreProceso);
		void CerrarArchivoLogProceso(void);
		void SetearFechaYHora(void);
	public:
		//constructor
		Logger(void);
		//destructor
		~Logger(void);
		//métodos de logging
        void EscribirLog(char cTipoLog, char *czData, char *czNombreProceso);
		void LoguearInformacion(char *czData, char *czNombreProceso);
		void LoguearWarning(char *czData, char *czNombreProceso);
		void LoguearError(char *czData, char *czNombreProceso);
		void LoguearDebugging(char *czData, char *czNombreProceso);
};

//constructor
Logger :: Logger(void) {
	mkdir("./logs",0755);
	czNombreArchivo = new char[40];
	memset(czNombreArchivo, 0, 40);
}

//destructor
Logger :: ~Logger(void) {
	/* libera memoria */
	delete czNombreArchivo;
}

//privados
void Logger :: PrepararArchivoLogProceso(char *czNombreProceso) {

	/* seteamos el nombre de archivo */
	sprintf(czNombreArchivo, "./logs/%s%d.LOG", czNombreProceso, getpid());

	/* se abre o crea al archivo log */
	if((arch = fopen(czNombreArchivo, "a+")) == NULL)
		printf("Error al generar el archivo Log.\n\n");
	else
		stderr = arch;
}
void Logger :: CerrarArchivoLogProceso(void) {
	if(arch) fclose(arch);
}
void Logger :: SetearFechaYHora(void) {
	/*Se obtiene el tiempo actual*/
	tiempoActual = time(NULL);

	/*transforma los datos de fecha y hora a un formato de cadena*/
	strftime(czFecha, 50, "%d/%m/%Y %T", localtime(&tiempoActual));
}

//métodos de logging
void Logger :: EscribirLog(char cTipoLog, char *czData, char *czNombreProceso) {

	PrepararArchivoLogProceso(czNombreProceso);
	SetearFechaYHora();

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
	}

	CerrarArchivoLogProceso();
}

void LoguearInformacion(char *czData, char *czNombreProceso) {

	PrepararArchivoLogProceso(czNombreProceso);
	SetearFechaYHora();

	/*registramos todo en el archivo */
	fprintf(stderr, "%s %s [%d][%d] ",czFecha,czNombreProceso,getpid(),gettid());
	fprintf(stderr, "%s: %s\n","INFO",czData);

	CerrarArchivoLogProceso();
}


void LoguearWarning(char *czData, char *czNombreProceso) {

	PrepararArchivoLogProceso(czNombreProceso);
	SetearFechaYHora();

	/*registramos todo en el archivo */
	fprintf(stderr, "%s %s [%d][%d] ",czFecha,czNombreProceso,getpid(),gettid());
	fprintf(stderr, "%s: %s\n","WARN",czData);

	CerrarArchivoLogProceso();
}

void LoguearError(char *czData, char *czNombreProceso) {

	PrepararArchivoLogProceso(czNombreProceso);
	SetearFechaYHora();

	/*registramos todo en el archivo */
	fprintf(stderr, "%s %s [%d][%d] ",czFecha,czNombreProceso,getpid(),gettid());
	fprintf(stderr, "%s: %s\n","ERROR",czData);

	CerrarArchivoLogProceso();
}

void LoguearDebugging(char *czData, char *czNombreProceso) {

	PrepararArchivoLogProceso(czNombreProceso);
	SetearFechaYHora();

	/*registramos todo en el archivo */
	fprintf(stderr, "%s %s [%d][%d] ",czFecha,czNombreProceso,getpid(),gettid());
	fprintf(stderr, "%s: %s\n","DEBUG",czData);

	CerrarArchivoLogProceso();
}