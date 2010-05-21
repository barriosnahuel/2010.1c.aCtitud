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


string Comando::respuestaObtenida() {
	return respuesta;
}

void Comando::setRespuestaObtenida(string strRespuesta) {
    respuesta = strRespuesta;
}

string Comando::cadenaIngresada() {

    string strCadenaIngresada = nombreComando;

    if(parametro.length())
       strCadenaIngresada += ' '+parametro;

    return strCadenaIngresada;
}

void Comando::reset() {
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

     vectorDeParametros[0]= 1; //NEWSGROUPS
     vectorDeParametros[1]= 0;
     vectorDeParametros[2]= 1; //group
     vectorDeParametros[3]= 1; //message-id
     vectorDeParametros[4]= 1; //message-id
     vectorDeParametros[5]= 1; //message-id
     vectorDeParametros[6]= 1; //message-id
}

int Comando::init(string strCadena) {
	reset();
    extraerNombreYParametro(strCadena);
    return validacion();
}

int Comando::validacion() {

      int i ;
      int comandoOk = 0;
      int parametroOk = 0;

      for(i=0;i<=6;i++)
        if(nombreComando == vectorDeComandos[i]) {
            comandoOk = 1;
            break;
        }

     if(comandoOk) {
        if(vectorDeParametros[i]==1 && parametro.empty()!=1)
            parametroOk = 1;
        else
            if(vectorDeParametros[i]==0 && parametro.empty()==1)
            parametroOk = 1;
     };
     
     if(comandoOk + parametroOk == 2)
        return 1;
     else
        return 0;
}

int Comando::validarParametro() {
	if(nombreComando == vectorDeComandos[3] || nombreComando == vectorDeComandos[4] || nombreComando == vectorDeComandos[5] || nombreComando == vectorDeComandos[6]) {
		return parsearParametro(parametro);
	}
	return 1;
}

int Comando::parsearParametro(string parametro) {
	int i = 0;
	
	while(parametro[i] != '@' && i < parametro.length()) {
		i = i + 1;
	}
	
	if(i == parametro.length()) {
		return 0;
	}
	else {
		return 1;
	}
}


string Comando::sacaEspaciosIzquierda(string cadena) {
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
