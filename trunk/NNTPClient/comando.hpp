#include <iostream>
using namespace std;



class Comando {

  public:
        Comando();

        ~Comando();

        string getNombreComando();

        int getLlevaParametro();

        void setNombreComando(string nombreDelComando);

        void setLlevaParametro(int valor);

  private:

        //Esto es por ej "Article".
        string nombreComando;
        //Esto es 1 o 0 si el comando lleva parametro. Por ej para "Article 321", esto iria en 1.
        int llevaParametro;


};
