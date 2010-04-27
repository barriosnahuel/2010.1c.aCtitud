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

    /**
     * Inicializa el Comando en base a la cadena ingresada por el usuario.
     * Parsea la cadena, la valida, y finalmente setea los atributos correspondientes
     */
	int Comando::init(string cadena){
	    parseaYValida(cadena);
		/*cadenaIngresada= cadena;
		extraerNombreYParametro(cadenaIngresada);*/
	}

	string Comando::getCadenaIngresada(void){
		return nombreComando+' '+parametro;
	}

	void Comando::reset(void){
		cadenaIngresada= "";
	    parametro= "";
	    nombreComando= "";
	    respuestaAlUsuario= "";
	    llevaParametro= 0;
	}

      int Comando::parseaYValida(string cadenaIngresada)
    {
        extraerNombreYParametro(cadenaIngresada);
        inicializacionVector();
        return validacion();
    }

    void Comando::inicializacionVector()
    {

         vectorDeComandos[0]="LIST NEWSGROUPS";
         vectorDeComandos[1]="QUIT";
         vectorDeComandos[2]="LISTGROUP";
         vectorDeComandos[3]="ARTICLE";
         vectorDeComandos[4]="STAT";
         vectorDeComandos[5]="HEAD";
         vectorDeComandos[6]="BODY";

         vectorDeParametros[0]= 0;
         vectorDeParametros[1]= 0;
         vectorDeParametros[2]= 1;
         vectorDeParametros[3]= 1;
         vectorDeParametros[4]= 1;
         vectorDeParametros[5]= 1;
         vectorDeParametros[6]= 1;
    }

    int Comando::validacion()
    {

      cout<<"NombreComando:"<<nombreComando<<endl;
      cout<<"Parametro:"<<parametro<<endl;

      int i ;
      int existeComando = 0;
      int existeParametro = 0;
      for(i=0;i<=6;i++)
        if(nombreComando == vectorDeComandos[i])
        {
            existeComando = 1;
            break;
        }

     if(existeComando!=0)
     {
        if(vectorDeParametros[i]== 1 && parametro.empty()==0)
            existeParametro = 1;
        else
            if(vectorDeParametros[i]==0 && parametro.empty()==1)
            existeParametro = 1;
     }
     else
     {
         /*cout<<*/respuestaAlUsuario="El comando ingresado no existe"<<endl;;
     }
     if(existeParametro==0)
     {
          if(vectorDeParametros[i]== 1 && parametro.empty()!=0)
            /*cout*<<*/respuestaAlUsuario="El comando debe llevar un parametro"<<endl;
          else
            if(vectorDeParametros[i]==0 && parametro.empty()!=1)
            /*cout<<*/respuestaAlUsuario="El comando no debe llevar parametros"<<endl;
     }

     return(existeComando!=0 && existeParametro!=0 );

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
    string Comando::aMayusculas(string cadena)
    {
        string cadenaTransformada="";
        for(int i=0;i<cadena.length();i++)
            cadenaTransformada = cadenaTransformada + (char)toupper(cadena.c_str()[i]);

        return cadenaTransformada;

    }
    void Comando::extraerNombreYParametro(string comandoEntero)
    {
        int i = 0;
        int espaciosDpsComando ;

        comandoEntero = sacaEspaciosIzquierda(comandoEntero);
        //EXTRAE EL COMANDO DE LA CADENA INGRESADA
        for(i;i<comandoEntero.length()&& !isspace(comandoEntero.c_str()[i]);i++)
            nombreComando = nombreComando + comandoEntero[i];

        //ME FIJO SI ES LIST YA QUE PUEDE SEGUIR LA PALABRA NEWSGROUPS
        espaciosDpsComando = consumeEspaciosDesde(i,comandoEntero);
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

        nombreComando=aMayusculas(nombreComando);

    }

