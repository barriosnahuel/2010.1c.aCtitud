#include "Comando.hpp"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <string>

using namespace std;

Comando::Comando(){
    reset();
    inicializarComandos();
}

Comando::~Comando(){}


string Comando::respuestaObtenida(){
	return respuesta;
}

void Comando::setRespuestaObtenida(string strRespuesta) {
    cout << "--- set rta obtenida entra " << strRespuesta << endl;
    respuesta = strRespuesta;
    cout << "--- set rta obtenida sale! --" << endl;
}

string Comando::cadenaIngresada(){

    string strCadenaIngresada = nombreComando;

    if(parametro.length())
       strCadenaIngresada += ' '+parametro;

    return strCadenaIngresada;
}

void Comando::reset(){
    parametro= "";
    nombreComando= "";
    respuesta= "";
}

void Comando::inicializarComandos() {
     vectorDeComandos[0]="LIST";
     vectorDeComandos[1]="QUIT";
     vectorDeComandos[2]="LISTGROUP";
     vectorDeComandos[3]="ARTICLE";
     vectorDeComandos[4]="STAT";
     vectorDeComandos[5]="HEAD";
     vectorDeComandos[6]="BODY";
     vectorDeComandos[7]="GROUP";

     vectorDeParametros[0]= 1; //NEWSGROUPS
     vectorDeParametros[1]= 0;
     vectorDeParametros[2]= 1; //group
     vectorDeParametros[3]= 1; //message-id
     vectorDeParametros[4]= 1; //message-id
     vectorDeParametros[5]= 1; //message-id
     vectorDeParametros[6]= 1; //message-id
     vectorDeParametros[7]= 0;
}

int Comando::init(string strCadena) {
	reset();
    extraerNombreYParametro(strCadena);
    return validacion();
}

int Comando::validacion() {

/*      cout<<"NombreComando:"<<nombreComando<<endl;
      cout<<"Parametro:"<<parametro<<endl;*/

      int i ;
      int comandoOk = 0;
      int parametroOk = 0;

      for(i=0;i<=7;i++)
        if(nombreComando == vectorDeComandos[i]) {
            comandoOk = 1;
            break;
        }

     if(comandoOk) {
        if(vectorDeParametros[i] && !parametro.empty())
            parametroOk = 1;
        else
            if(!vectorDeParametros[i] && parametro.empty())
            parametroOk = 1;
     }
     else {
         cout<<"El comando ingresado no existe"<<endl;;
     }

/*     if(!parametroOk) {
          if(vectorDeParametros[i]== 1 && parametro.empty()!=0)
            cout<<"El comando debe llevar un parametro"<<endl;
          else
            if(vectorDeParametros[i]==0 && parametro.empty()!=1)
            cout<<"El comando no debe llevar parametros"<<endl;
     }
*/
     return(comandoOk && parametroOk);
}



string Comando::sacaEspaciosIzquierda(string cadena){
       string cadenaTransformada;
       int desde = 0;
       desde = consumeEspaciosDesde(desde,cadena);
       for(int i=desde;i<cadena.length();i++)
            cadenaTransformada=cadenaTransformada+cadena[i];
       return cadenaTransformada;

}

int Comando::consumeEspaciosDesde(int posicion,string cadena){
      /* PARA DETECTAR ESPACIOS EXISTE LA FUNCION isspace(char*), PARA PODER PASARLE UN CARACTER DE LA CADENA
       * DEBO HACER (cadena.c_str())[posicion]
       */
        int i;
        for(i = posicion;isspace(cadena.c_str()[i]);i++);
        return i;
}

void Comando::extraerNombreYParametro(string comandoEntero){
        int i = 0;
        int espaciosDpsComando ;

        comandoEntero = sacaEspaciosIzquierda(comandoEntero);
        //EXTRAE EL COMANDO DE LA CADENA INGRESADA

        for(i;
              i < comandoEntero.length() && 
              !isspace(comandoEntero.c_str()[i]);
              i++)
            nombreComando = nombreComando + comandoEntero[i];

        // EL RESTO DE LA CADENA ES EL PARAMETRO
        espaciosDpsComando = consumeEspaciosDesde(i,comandoEntero);
        for( i = espaciosDpsComando;i<comandoEntero.length();i++)
        {
            parametro = parametro + comandoEntero[i];
        }

        // convierto a mayúsculas
        std::transform(nombreComando.begin(), nombreComando.end(), nombreComando.begin(), (int(*)(int)) toupper);
}

int Comando::indicaSalida() {
	return nombreComando.compare("QUIT");
}
