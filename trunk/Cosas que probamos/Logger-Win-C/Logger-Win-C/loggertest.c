#include <string.h>
#include "logger-win.h"

char czNombreProceso[20]; /* global para el processs name */

int main(int argn, char *argv[]){

    memset(czNombreProceso, 0, 20);
    strcpy_s(czNombreProceso,11,"Mi_Proceso\0");
	/* strcpy_s(argv[0],"Mi_Proceso"); -- no funciona */
	
	EscribirLog(1,"Log de info");
	EscribirLog(2,"Log de warning");
	EscribirLog(3,"Log de error");
	EscribirLog(4,"Log de debug");
	EscribirLog(5,"Log de otro mensaje");

	LoguearInformacion("Log de info");
	LoguearWarning("Log de warning");
	LoguearError("Log de error");
	LoguearDebugging("Log de debug");

}