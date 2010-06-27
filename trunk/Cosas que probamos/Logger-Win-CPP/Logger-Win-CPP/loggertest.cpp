#include <iostream>
#include <string.h>
#include "logger-win.hpp"

using namespace std;

char czNombreProceso[20]; /* global para el processs name */

int main(int argn, char *argv[]){

	Logger unLogger;

	memset(czNombreProceso, 0, 20);
    strcpy_s(czNombreProceso,11,"Mi_Proceso\0");

	unLogger.EscribirLog(1,"Log de info");
	unLogger.EscribirLog(2,"Log de warning");
	unLogger.EscribirLog(3,"Log de error");
	unLogger.EscribirLog(4,"Log de debug");
	unLogger.EscribirLog(5,"Log de otro mensaje");

	unLogger.LoguearInformacion("Log de info");
	unLogger.LoguearWarning("Log de warning");
	unLogger.LoguearError("Log de error");
	unLogger.LoguearDebugging("Log de debug");

}