#include <iostream>
#include <string>
#include <string.h>
#include "Configuracion.hpp"

using namespace std;

//constructor
Configuracion::Configuracion() {
    czServidor = new char[40];
    memset(czServidor, 0, 40);
    strcpy(czServidor, "news.giganews.com");

    uiPuerto = 563;
};

//destructor
Configuracion::~Configuracion() {
    /* libera memoria */
    delete [] czServidor;
};

int Configuracion::cargarDefault(void){};

int Configuracion::cargarDesdeArchivo(const char *czFilename){};

int Configuracion::cargarDesdeParametros(const char *czHost, int uiPort){};

char * Configuracion::getServidor(void) {
    return czServidor;
};

int Configuracion::getPuerto(void) {
    return uiPuerto;
};
