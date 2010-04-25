#ifndef _LOGGER_HPP

#define _LOGGER_HPP

#include <iostream>
#include <fstream>

#define _INFO 1
#define _WARN 2 
#define _ERROR 3
#define _DEBUG 4

using namespace std;

class Logger {
    private:
        ofstream arch;
        time_t tiempoActual;
        char *czNombreArchivo;
        char czFecha[50];
        pid_t tid;

        void PrepararArchivoLogProceso(const char *czNombreProceso);
        void CerrarArchivoLogProceso(void);
        void SetearFechaYHora(void);
    public:
        //constructor
        Logger();
        //destructor
        virtual ~Logger();

        //metodos de logging
        void EscribirLog(char cTipoLog, const char *czData, const char *czNombreProceso);
        void LoguearInformacion(const char *czData, const char *czNombreProceso);
        void LoguearWarning(const char *czData, const char *czNombreProceso);
        void LoguearError(const char *czData, const char *czNombreProceso);
        void LoguearDebugging(const char *czData, const char *czNombreProceso);
};

#endif  /* _LOGGER_HPP */
