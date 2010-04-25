/*
 * Comando.cpp
 *
 *  Created on: Apr 22, 2010
 *      Author: Barrios, Nahuel.
 *      Nota:   Cualquier funcion y/o método que se agregue, ver si hay que agregarlo en los dos lugares (.h/.hpp y .cpp) y tienen que
 *      		respetar el formato que tiene la funcionDePrueba. Eso es:	ClaseALaQuePerteneceElMetodo::nombreMetodo.
 */

#include "Comando.hpp"
#include <iostream>
using namespace std;

	Comando::Comando(void){
		cadenaIngresada= "";
		llevaParametro= 0;
		nombreComando= "";
		parametro= "";
		respuestaAlUsuario= "";
	}

	Comando::~Comando(void){}

	void Comando::setNombreComando(string nombre){
		nombreComando= nombre;
	}

	string Comando::getNombreComando(void){
		return nombreComando;
	}

	void Comando::setRespuestaAlUsuario(string respuesta){
		respuestaAlUsuario= respuesta;
	}

	string Comando::getRespuestaAlUsuario(void){
		return respuestaAlUsuario;
	}

    bool Comando::getLlevaParametro(void){
        return llevaParametro;
    }

    string Comando::getParametro(void){
        return parametro;
    }

	void Comando::init(string cadena){
		cadenaIngresada= cadena;
		extraerNombreYParametro(cadenaIngresada);
	}

	string Comando::getCadenaIngresada(void){
		return cadenaIngresada;
	}

	void Comando::reset(void){
		cadenaIngresada= "";
	    parametro= "";
	    nombreComando= "";
	    respuestaAlUsuario= "";
	    llevaParametro= 0;
	}

    void Comando::inicializacionVector()
    {

         vectorDeComandos.push_back("LIST NEWSGROUPS");
         vectorDeComandos.push_back("QUIT");
         vectorDeComandos.push_back("LISTGROUP");
         vectorDeComandos.push_back("ARTICLE");
         vectorDeComandos.push_back("STAT");
         vectorDeComandos.push_back("HEAD");
         vectorDeComandos.push_back("BODY");
         vectorDeParametros.push_back(0);
         vectorDeParametros.push_back(0);
         vectorDeParametros.push_back(1);
         vectorDeParametros.push_back(1);
         vectorDeParametros.push_back(1);
         vectorDeParametros.push_back(1);
         vectorDeParametros.push_back(1);
    }

    int Comando::validacion()
    {/*
      cout<<"NombreComando:"<<nombreComando<<endl;
      cout<<"Parametro:"<<parametro<<endl;
      cout<<"VECTOR DE COMANDOS 0:"<<vectorDeComandos.push_back[0];
      int i ;
      int existeComando = 0;
      for(i=0;vectorDeComandos.size();i++)
        if(nombreComando == vectorDeComandos[i])
        {
            existeComando = 1;
            break;
        }
      cout << "Existe Comando : "<< existeComando;
        */
    }

    string Comando::sacaEspaciosIzquierda(string cadena)
    {
       string cadenaTransformada;
       int desde = 0;
       desde = consumeEspaciosDesde(desde,cadena);
       for(int i=desde;i<cadena.length();i++)
            cadenaTransformada=cadenaTransformada+cadena[i];
       return cadenaTransformada;

    }
    int Comando::consumeEspaciosDesde(int posicion,string cadena)
    {
      /* PARA DETECTAR ESPACIOS EXISTE LA FUNCION isspace(char*), PARA PODER PASARLE UN CARACTER DE LA CADENA
       * DEBO HACER (cadena.c_str())[posicion]
       */
        int i;
        for(i = posicion;isspace(cadena.c_str()[i]);i++);
        return i;
    }
    int Comando::extraerNombreYParametro(string comandoEntero)
    {
        comandoEntero = sacaEspaciosIzquierda(comandoEntero);
        //EXTRAE EL COMANDO DE LA CADENA INGRESADA
        int i = 0;
        for(i;i<comandoEntero.length()&& !isspace(comandoEntero.c_str()[i]);i++)
            nombreComando = nombreComando + comandoEntero[i];

        //ME FIJO SI ES LIST YA QUE PUEDE SEGUIR LA PALABRA NEWSGROUPS
        int espaciosDpsComando = consumeEspaciosDesde(i,comandoEntero);
        if(nombreComando=="LIST")
        {
            nombreComando = nombreComando + ' ';
            for(i=espaciosDpsComando;i<comandoEntero.length()&&!isspace(comandoEntero.c_str()[i]);i++)
                nombreComando = nombreComando + comandoEntero[i];
        }

        // EL RESTO DE LA CADENA ES EL PARAMETRO
        espaciosDpsComando = consumeEspaciosDesde(i,comandoEntero);
        for( i = espaciosDpsComando;i<comandoEntero.length();i++)
        {
            parametro = parametro + comandoEntero[i];
        }

    }
