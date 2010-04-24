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

	Comando::Comando(void){}
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

    void Comando::setLlevaParametro(int valor){
        llevaParametro = valor;
    }

    void Comando::funcionDePrueba(void){
    	cout << "Entro bien a la funcion: void Comando::funcionDePrueba(void)" << endl;
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

    string Comando::sacaEspaciosIzquierda(string cadena)
    {
        string cadenaTransformada;
        int desde = 0;
        for(int i =0;i<cadena.length();i++)
          if(cadena[i]==' ' && cadena[i+1]!=' ')  //SI EL SIGUIENTE NO ES UN ESPACIO SALE
          {
           desde=desde+1;
           break;
          }
          else
           if(cadena[i]==' ')                     //SI EL SIGUIENTE ES UN ESPACIO TIENE QUE SEGUIR
            desde=desde+1;
           else
            break;                               //SI NO COMIENZA CON ESPACIO NO  HACE NADA


        cout<<"EL ULTIMO ESPACIO ESTA EN "<<desde<<endl;
        for(int i=desde;i<cadena.length();i++)
            cadenaTransformada=cadenaTransformada+cadena[i];
        cout<<"CADENA FINAL SIN ESPACIOS A IZQUIERDA:"<<cadenaTransformada<<endl;
        return cadenaTransformada;

    }

    int Comando::extraerNombreYParametro(string comandoEntero)
    {
        int i = 0;
        comandoEntero = sacaEspaciosIzquierda(comandoEntero); //SACA ESPACIOS A IZQUIERDA

        //EXTRAE EL COMANDO DE LA CADENA INGRESADA
        for(i;i<comandoEntero.length()&&comandoEntero[i]!=' ';i++)
        {
             nombreComando = nombreComando + comandoEntero[i];
             cout <<"digito :"<<i<<":"<<nombreComando[i]<<endl;

        }
        cout << "Nombre Comando:" << nombreComando<<endl;
        //ME FIJO SI ES LIST YA QUE PUEDE SEGUIR LA PALABRA NEWSGROUPS
        if(nombreComando=="LIST")
        {
            nombreComando = nombreComando + ' ';
            for(i=nombreComando.length();i<comandoEntero.length()&&comandoEntero[i]!=' ';i++)
                nombreComando = nombreComando + comandoEntero[i];
        }
        cout << "Nombre Comando:" << nombreComando<<endl;
        // EL RESTO DE LA CADENA ES EL PARAMETRO
        for(i=nombreComando.length()+1;i<nombreComando.length();i++)
        {
            parametro = parametro + comandoEntero[i];
        }
        cout<<"Parametro:"<<parametro;

        /*int estado;
        estado = validarComando();*/
    }

