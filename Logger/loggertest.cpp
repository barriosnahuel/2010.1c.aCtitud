#include "logger.hpp"

int main(int argn, char *argv[]){
	Logger unLogger;

	unLogger.EscribirLog(1,"Log de info",argv[0]);
	unLogger.EscribirLog(2,"Log de warning",argv[0]);
	unLogger.EscribirLog(3,"Log de error",argv[0]);
	unLogger.EscribirLog(4,"Log de debug",argv[0]);
	unLogger.EscribirLog(5,"Log de otro mensaje",argv[0]);

	unLogger.LoguearInformacion("Log de info",argv[0]);
	unLogger.LoguearWarning("Log de warning",argv[0]);
	unLogger.LoguearError("Log de error",argv[0]);
	unLogger.LoguearDebugging("Log de debug",argv[0]);

	return EXIT_SUCCESS;
}
