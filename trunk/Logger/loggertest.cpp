#include <iostream>
#include <fstream>
#include <string.h>
#include "logger.hpp"

using namespace std;

int main(int argn, char *argv[]){
	Logger unLogger;

    char czNombreProceso[20];

    memset(czNombreProceso, 0, 20);
    strcpy(czNombreProceso, "MiProceso\0");
    strcpy(argv[0], czNombreProceso);

	unLogger.EscribirLog(1,"Log de info",argv[0]);
	unLogger.EscribirLog(2,"Log de warning",argv[0]);
	unLogger.EscribirLog(3,"Log de error",argv[0]);
	unLogger.EscribirLog(4,"Log de debug",argv[0]);
	unLogger.EscribirLog(5,"Log de otro mensaje",argv[0]);

	unLogger.LoguearInformacion("Log de info",argv[0]);
	unLogger.LoguearWarning("Log de warning",argv[0]);
	unLogger.LoguearError("Log de error",argv[0]);
	unLogger.LoguearDebugging("Log de debug",argv[0]);

}
