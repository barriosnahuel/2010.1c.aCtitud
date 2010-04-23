#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include "logger.hpp"

using namespace std;

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
void Logger :: PrepararArchivoLogProceso(const char *czNombreProceso) {

	/* seteamos el nombre de archivo */
	sprintf(czNombreArchivo, "./logs/%s%d.LOG", czNombreProceso, getpid());

	/* se abre o crea al archivo log */
    arch.open(czNombreArchivo, ios::in);
    if (!(arch.is_open()))
        cout << "No se pudo abrir el archivo de LOG.";
}
void Logger :: CerrarArchivoLogProceso(void) {
	if(arch.is_open())
        arch.close();
}
void Logger :: SetearFechaYHora(void) {
	/*Se obtiene el tiempo actual*/
	tiempoActual = time(NULL);

	/*transforma los datos de fecha y hora a un formato de cadena*/
	strftime(czFecha, 50, "%d/%m/%Y %T", localtime(&tiempoActual));
}

//métodos de logging
void Logger :: EscribirLog(char cTipoLog, const char *czData, const char *czNombreProceso) {

	PrepararArchivoLogProceso(czNombreProceso);
	SetearFechaYHora();

	/*registramos todo en el archivo */
    arch << czFecha << " " << czNombreProceso << " [" << getpid() << "][" << gettid() << "] ";

	switch(cTipoLog) {
		case _INFO:
            arch << "INFO";
			break;
		case _WARN:
            arch << "WARN";
			break;
		case _ERROR:
            arch << "ERROR";
			break;
		case _DEBUG:
            arch << "DEBUG";
			break;
		default:
            arch << "MSG";
			break;
	};

    arch << ": " << czData << endl;

	CerrarArchivoLogProceso();
}

void LoguearInformacion(const char *czData, const char *czNombreProceso) {

	PrepararArchivoLogProceso(czNombreProceso);
	SetearFechaYHora();

	/*registramos todo en el archivo */
	fprintf(stderr, "%s %s [%d][%d] ",czFecha,czNombreProceso,getpid(),gettid());
	fprintf(stderr, "%s: %s\n","INFO",czData);

	CerrarArchivoLogProceso();
}


void LoguearWarning(const char *czData, const char *czNombreProceso) {

	PrepararArchivoLogProceso(czNombreProceso);
	SetearFechaYHora();

	/*registramos todo en el archivo */
	fprintf(stderr, "%s %s [%d][%d] ",czFecha,czNombreProceso,getpid(),gettid());
	fprintf(stderr, "%s: %s\n","WARN",czData);

	CerrarArchivoLogProceso();
}

void LoguearError(const char *czData, const char *czNombreProceso) {

	PrepararArchivoLogProceso(czNombreProceso);
	SetearFechaYHora();

	/*registramos todo en el archivo */
	fprintf(stderr, "%s %s [%d][%d] ",czFecha,czNombreProceso,getpid(),gettid());
	fprintf(stderr, "%s: %s\n","ERROR",czData);

	CerrarArchivoLogProceso();
}

void LoguearDebugging(const char *czData, const char *czNombreProceso) {

	PrepararArchivoLogProceso(czNombreProceso);
	SetearFechaYHora();

	/*registramos todo en el archivo */
	fprintf(stderr, "%s %s [%d][%d] ",czFecha,czNombreProceso,getpid(),gettid());
	fprintf(stderr, "%s: %s\n","DEBUG",czData);

	CerrarArchivoLogProceso();
}
