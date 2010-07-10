#ifndef _LOGGER_WIN_HPP

#define _LOGGER_WIN_HPP

#include <windows.h>
#include <iostream>
//#include <stdio.h>
//#include <string.h>
#include <time.h>

#define _INFO 1
#define _WARN 2 
#define _ERROR 3
#define _DEBUG 4

using namespace std;

class Logger {
    private:
        HANDLE  arch;
        time_t tiempoActual;
        struct tm nuevoTiempoActual;
        char *czNombreArchivo;
        char czFecha[100];

        void PrepararArchivoLogProceso(void);
        void SetearFechaYHora(void);
    public:
        //constructor
        Logger();
        //destructor
        ~Logger();

        //metodos de logging
        void EscribirLog(char cTipoLog, const char *czData);
        void LoguearInformacion(const char *czData);
        void LoguearWarning(const char *czData);
        void LoguearError(const char *czData);
        void LoguearDebugging(const char *czData);
};

#endif  /* _LOGGER_WIN_HPP */
