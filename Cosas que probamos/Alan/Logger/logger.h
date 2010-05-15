#ifndef _LOGGER_H

#define _LOGGER_H

#define _INFO 1
#define _WARN 2
#define _ERROR 3
#define _DEBUG 4

void LoguearInformacion(char *czData, char *czNombreProceso);

void LoguearWarning(char *czData, char *czNombreProceso);

void LoguearError(char *czData, char *czNombreProceso);

void LoguearDebugging(char *czData, char *czNombreProceso);

void EscribirLog(char cTipoLog, char *czData, char *czNombreProceso);

#endif  /* _LOGGER_H */

