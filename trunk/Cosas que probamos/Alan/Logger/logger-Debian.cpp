#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <string.h>
#include <errno.h>
#include "logger.hpp"

using namespace std;

//constructor
Logger::Logger() {
	mkdir("./logs",0755);
	czNombreArchivo = new char[40];
	memset(czNombreArchivo, 0, 40);
    tid = (pid_t) syscall (SYS_gettid);
};

//destructor
Logger::~Logger() {
	/* libera memoria */
	delete czNombreArchivo;
};

//privados
void Logger::PrepararArchivoLogProceso(const char *czNombreProceso) {

	/* seteamos el nombre de archivo */
	sprintf(czNombreArchivo, "./logs/%s%d.LOG", czNombreProceso, getpid());

	/* se abre o crea al archivo log */
    arch.open(czNombreArchivo, ios::app);
    if (!(arch.is_open()))
        perror("No se pudo abrir el archivo de LOG.");
};
void Logger::CerrarArchivoLogProceso(void) {
	if(arch.is_open())
        arch.close();
};
void Logger::SetearFechaYHora(void) {
	/*Se obtiene el tiempo actual*/
	tiempoActual = time(NULL);

	/*transforma los datos de fecha y hora a un formato de cadena*/
	strftime(czFecha, 50, "%d/%m/%Y %T", localtime(&tiempoActual));
};

//m�todos de logging
void Logger::EscribirLog(char cTipoLog, const char *czData, const char *czNombreProceso) {

	PrepararArchivoLogProceso(czNombreProceso);
	SetearFechaYHora();

	/*registramos todo en el archivo */
    arch << czFecha << " " << czNombreProceso << " [" << getpid() << "][" << tid << "] ";

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

    arch << ":\t" << czData << endl;

	CerrarArchivoLogProceso();
};

void Logger::LoguearInformacion(const char *czData, const char *czNombreProceso) {
	EscribirLog(1, czData, czNombreProceso);
};


void Logger::LoguearWarning(const char *czData, const char *czNombreProceso) {
	EscribirLog(2, czData, czNombreProceso);
};

void Logger::LoguearError(const char *czData, const char *czNombreProceso) {
	EscribirLog(3, czData, czNombreProceso);
};

void Logger::LoguearDebugging(const char *czData, const char *czNombreProceso) {
	EscribirLog(4, czData, czNombreProceso);
};

