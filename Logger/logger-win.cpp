#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <process.h>
#include "logger-win.hpp"

using namespace std;

//constructor
Logger::Logger() {
	CreateDirectoryA("./logs",NULL);
	czNombreArchivo = new char[40];
	memset(czNombreArchivo, 0, 40);
};

//destructor
Logger::~Logger() {
	/* libera memoria */
	delete czNombreArchivo;
};

//privados
void Logger::PrepararArchivoLogProceso() {

	extern char czNombreProceso[20];

	/* seteamos el nombre de archivo */
	sprintf_s(czNombreArchivo, 40, "./logs/%s%d.LOG\0", czNombreProceso, _getpid());

	/* se abre o crea al archivo log */
   arch = CreateFile ( czNombreArchivo, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
   if(arch == INVALID_HANDLE_VALUE) {
	  printf("Error al generar el archivo Log. (%d)\n", GetLastError());
      return;
   }

};
void Logger::SetearFechaYHora(void) {
	/*Se obtiene el tiempo actual*/
	time(&tiempoActual);
	_localtime64_s(&nuevoTiempoActual,&tiempoActual);

	/*transforma los datos de fecha y hora a un formato de cadena*/
	strftime(czFecha, 100, "%d/%m/%Y %X", &nuevoTiempoActual);
};

//metodos de logging
void Logger::EscribirLog(char cTipoLog, const char *czData) {

	extern char czNombreProceso[20];

	char *czWriteBuff;
	DWORD dwBytesToWrite;
	DWORD dwBytesWritten = 0;

	PrepararArchivoLogProceso();
	SetearFechaYHora();

	czWriteBuff = new char[200];
	memset(czWriteBuff, 0, 200);
	sprintf_s(czWriteBuff, 200, "%s %s [%d][%d] ",czFecha,czNombreProceso,_getpid(),_getpid());
	
	dwBytesToWrite = (DWORD)strlen(czWriteBuff);

	/*registramos todo en el archivo */
	SetFilePointer(arch, 0, 0, FILE_END);
    WriteFile(arch,(LPCVOID)czWriteBuff,dwBytesToWrite,&dwBytesWritten,NULL);
	delete czWriteBuff;

	switch(cTipoLog) {
		case _INFO:
            WriteFile(arch,"INFO",4,&dwBytesWritten,NULL);
			break;
		case _WARN:
            WriteFile(arch,"WARN",4,&dwBytesWritten,NULL);
			break;
		case _ERROR:
            WriteFile(arch,"ERROR",5,&dwBytesWritten,NULL);
            printf("ERROR: %s\n",czData);
			break;
		case _DEBUG:
            WriteFile(arch,"DEBUG",5,&dwBytesWritten,NULL);
			break;
		default:
            WriteFile(arch,"MSG",3,&dwBytesWritten,NULL);
			break;
	};

	czWriteBuff = new char[strlen(czData)+5];
	memset(czWriteBuff, 0, strlen(czData)+5);
	sprintf_s(czWriteBuff, strlen(czData)+5, ": %s\r\n",czData);
	dwBytesToWrite = (DWORD)strlen(czWriteBuff);
	WriteFile(arch,(LPCVOID)czWriteBuff,dwBytesToWrite,&dwBytesWritten,NULL);
	delete czWriteBuff;

	CloseHandle(arch);
};

void Logger::LoguearInformacion(const char *czData) {
	EscribirLog(1, czData);
};


void Logger::LoguearWarning(const char *czData) {
	EscribirLog(2, czData);
};

void Logger::LoguearError(const char *czData) {
	EscribirLog(3, czData);
};

void Logger::LoguearDebugging(const char *czData) {
	EscribirLog(4, czData);
};
