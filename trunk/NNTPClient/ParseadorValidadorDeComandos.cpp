/*
 * Validator.cpp
 *
 *  Created on: Apr 23, 2010
 *      Author: Barrios, Nahuel.
 *      Nota:   Cualquier funcion y/o método que se agregue, ver si hay que agregarlo en los dos lugares (.h/.hpp y .cpp) y tienen que
 *      		respetar el formato que tiene la funcionDePrueba. Eso es:	ClaseALaQuePerteneceElMetodo::nombreMetodo.
 */

#include "ParseadorValidadorDeComandos.h"
#include "Comando.hpp"
#include <iostream>
//#include <vector>
//#include <ctype.h>
using namespace std;


ParseadorValidadorDeComandos::ParseadorValidadorDeComandos() {
	// TODO Auto-generated constructor stub

}

ParseadorValidadorDeComandos::~ParseadorValidadorDeComandos() {
	// TODO Auto-generated destructor stub
}

void ParseadorValidadorDeComandos::funcionDePrueba(void){
	cout << "Entro bien a la funcion: void ParseadorValidadorDeComandos::funcionDePrueba(void)" << endl;


	cout << "Adentro del parser, pruebo un comando" << endl;
	Comando unComando;
	unComando.setNombreComando("unNombre");
	cout << "Imprimo el nombre del comando: " << unComando.getNombreComando() << endl;
	cout << "Termine de probar el comando adentro del parser" << endl;
}


/*{
    public:
    string comandoNombre;
    string parametro;
    vector<string> vectorDeComandos;
    vector<int> vectorDeParametros;

    public:

    void inicializacionVector()
    {

        vectorDeComandos.push_back("LIST NEWSGROUPS");
        vectorDeComandos.push_back("QUIT");
        vectorDeComandos.push_back("LISTGROUP");
        vectorDeComandos.push_back("ARTICLE");
        vectorDeComandos.push_back("STAT");
        vectorDeComandos.push_back("HEAD");
        vectorDeComandos.push_back("BODY");
        vectorDeComandos.push_back("GROUP");
        vectorDeComandos.push_back("NEXT");
        vectorDeParametros.push_back(0);
		vectorDeParametros.push_back(0);
		vectorDeParametros.push_back(1);
		vectorDeParametros.push_back(1);
		vectorDeParametros.push_back(1);
		vectorDeParametros.push_back(1);
		vectorDeParametros.push_back(1);
		vectorDeParametros.push_back(1);
		vectorDeParametros.push_back(1);

    }
    void setNombreYParametro()
    {
    }

    string sacaEspaciosIzquierda(string cadena)
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
    int extraerNombreYParametro(string comandoEntero)
    {
        int i = 0;
        comandoEntero = sacaEspaciosIzquierda(comandoEntero); //SACA ESPACIOS A IZQUIERDA

        //EXTRAE EL COMANDO DE LA CADENA INGRESADA
        for(i;i<comandoEntero.length()&&comandoEntero[i]!=' ';i++)
        {
             comandoNombre = comandoNombre + comandoEntero[i];
             cout <<"digito :"<<i<<":"<<comandoNombre[i]<<endl;

        }
        cout << "Nombre Comando:" << comandoNombre<<endl;
        //ME FIJO SI ES LIST YA QUE PUEDE SEGUIR LA PALABRA NEWSGROUPS
        if(comandoNombre=="LIST")
        {
            comandoNombre = comandoNombre + ' ';
            for(i=comandoNombre.length();i<comandoEntero.length()&&comandoEntero[i]!=' ';i++)
                comandoNombre = comandoNombre + comandoEntero[i];
        }
        cout << "Nombre Comando:" << comandoNombre<<endl;
        // EL RESTO DE LA CADENA ES EL PARAMETRO
        for(i=comandoNombre.length()+1;i<comandoEntero.length();i++)
        {
            parametro = parametro + comandoEntero[i];
        }
        cout<<"Parametro:"<<parametro;

        int estado;
        estado = validarComando(comandoNombre);
    }*/
/*
    void setNombre(string nombreComandoIngresado)
    {
        comando = nombreComandoIngresado;
    };
    void setParametro(string parametroIngresado)
    {
        parametro = parametroIngresado;
    };*/

   /* int validarComando(string comandoIngresado)
    {
        int comandoValido;
        for(int i = 0; i<8;i++)
            if(comandoNombre == vectorDeComandos[i])
            {
                cout<<"COMANDO RECONOCIDO :"<<vectorDeComandos[i];
                comandoValido = 1;
                return comandoValido;
            }
        cout<<"COMANDO NO RECONOCIDO !!";
        return 0 ;

    }
    string aMayusculas(string cadena)
    {
        for(int i = 0; i < cadena.length(); i++)
         cadena[i] = toupper(cadena[i]);
        return cadena;
    }


};
int main()
{

    string requerimientoUsuario;

    parseadorValidadorDeComando comandoIngresado;
    comandoIngresado.inicializacionVector();
    cout<<"Ingrese un comando:"<<endl;
    getline(cin, requerimientoUsuario, '\n'); //Ya que cin corta la cadena


    comandoIngresado.extraerNombreYParametro(requerimientoUsuario);

    return 0;
}*/
