#ifndef _CONFIGURACION_HPP

#define _CONFIGURACION_HPP

#include <string>

using namespace std;

class Configuracion {
    private:
        char *czServidor;
        int uiPuerto;

    public:
        //constructor
        Configuracion();
        //destructor
        ~Configuracion();

        int cargarDefault(void);
        int cargarDesdeArchivo(const char *czFilename);
        int cargarDesdeParametros(const char *czHost, int uiPort);

        char * getServidor(void);
        int getPuerto(void);
};

#endif  /* _CONFIGURACION_HPP */
