#include <string>

using namespace std;

class Comando
{
    private:
            //Esto es por ej "Article".
            string nombreComando;
            //Esto es 1 o 0 si el comando lleva parametro. Por ej para "Article 321", esto iria en 1.
            int llevaParametro;
    public:
            //Constructor
            Comando comando(void);


            /* Los getters y settes los implemento directamente acá por cuestiones de comodidad y prolojidad. Según el ejemplo
            que estoy tomando, se puede... */

            string getNombreComando(void){
                return nombreComando;
            }

            int getLlevaParametro(void){
                return llevaParametro;
            }

            void setNombreComando(string nombreDelComando){
                nombreComando = nombreDelComando;
            }

            void setllevaParametro(int valor){
                llevaParametro = valor;
            }
}
