#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include "Configuracion.hpp"

int main(int argn, char *argv[]) {

    Configuracion confCliente;

    switch(argn) {
        case 1: // no se indicó archivo, se carga archivo por defecto (cliente.conf)
            if(confCliente.cargarDefault() == 0) {
                cerr << "Archivo de configuración no válido.\n";
                return -1;
            }
            break;
        case 2: // único parámetro: nombre de archivo de configuración
            if(confCliente.cargarDesdeArchivo(argv[1]) == 0) {
                cerr << "Archivo de configuración no válido.\n";
                return -1;
            }
            break;
        case 3: // se pasaron los valores por parámetro
            if(confCliente.cargarDesdeParametros(argv[1],atoi(argv[2])) == 0) {
                cerr << "Parámetros no válidos.\n";
                return -1;
            }
            break;
        default:
            cerr << "Parámetros no válidos.\n";
            return -1;
            break;
    };
    
    cout << "Archivo cargado. Servidor " << confCliente.getServidor() << endl;
    cout << " Puerto " << confCliente.getPuerto() << endl;
}
