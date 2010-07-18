#ifndef _CONFIGURACION_HPP

#define _CONFIGURACION_HPP

using namespace std;

class Configuracion {
    private:
        char *czServidor;
        int iPuerto;

        char *GetVal(const char *sValBuff, const char *sBuff);
    public:
        //constructor
        Configuracion();
        //destructor
        ~Configuracion();

        int cargarDefault(void);
        int cargarDesdeArchivo(const char *czFilename);
        int cargarDesdeParametros(const char *czHost, int iPort);
        int Valida_IP(const char *ip);

        char * getServidor(void);
        int getPuerto(void);
};

#endif  /* _CONFIGURACION_HPP */
