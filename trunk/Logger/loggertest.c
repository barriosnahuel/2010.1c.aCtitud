#include <string.h>
#include "logger.h"

int main(int argn, char *argv[]){

    strcpy(argv[0], "MiProcesoC\0");

	EscribirLog(1,"Log de info",argv[0]);
	EscribirLog(2,"Log de warning",argv[0]);
	EscribirLog(3,"Log de error",argv[0]);
	EscribirLog(4,"Log de debug",argv[0]);
	EscribirLog(5,"Log de otro mensaje",argv[0]);

	LoguearInformacion("Log de info",argv[0]);
	LoguearWarning("Log de warning",argv[0]);
	LoguearError("Log de error",argv[0]);
	LoguearDebugging("Log de debug",argv[0]);

}
