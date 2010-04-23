#include "comando.hpp"

    Comando::Comando() {
       cout << "Comando creado." << endl;
      }

      // Implementacion de destructor. Util para liberar memoria.
      Comando::~Comando() {
       cout << "Comando destruido." << endl;
      }

        string Comando::getNombreComando(void){
            return nombreComando;
        }

        int Comando::getLlevaParametro(void){
            return llevaParametro;
        }

        void Comando::setNombreComando(string nombreDelComando){
            nombreComando = nombreDelComando;
        }

        void Comando::setLlevaParametro(int valor){
            llevaParametro = valor;
        }

