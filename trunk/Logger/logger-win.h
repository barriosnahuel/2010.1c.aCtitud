#ifndef _LOGGER_WIN_H

#define _LOGGER_WIN_H

#define _INFO 1
#define _WARN 2
#define _ERROR 3
#define _DEBUG 4

void LoguearInformacion(char *czData);

void LoguearWarning(char *czData);

void LoguearError(char *czData);

void LoguearDebugging(char *czData);

void EscribirLog(char cTipoLog, char *czData);

#endif  /* _LOGGER_WIN_H */