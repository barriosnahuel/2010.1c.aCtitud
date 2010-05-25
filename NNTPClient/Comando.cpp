#include <iostream>
#include <algorithm>
#include <cctype>
#include <string>
#include <stdio>
#include "Comando.hpp"

using namespace std;

#define APP_NAME_FOR_LOGGER "NNTPClient"
Logger logger;

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
	vectorDeComandos[0]="LIST NEWSGROUPS";
	vectorDeComandos[1]="QUIT";
	vectorDeComandos[2]="LISTGROUP";
	vectorDeComandos[3]="ARTICLE";
	vectorDeComandos[4]="STAT";
	vectorDeComandos[5]="HEAD";
	vectorDeComandos[6]="BODY";

	vectorDeParametros[0]= 0; 
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
	return 1;	/*	TODO: Aca en realidad tendriamos que retornar validacion() y usar init en un if ya para validar.	*/
}

int Comando::validacion() {
	logger.LoguearDebugging("--> Comando::validacion()", APP_NAME_FOR_LOGGER);

	int i ;
	int comandoOk = 0;
	int parametroOk = 0;

	for(i=0;i<=6;i++)
		if(nombreComando == vectorDeComandos[i]) {
			comandoOk = 1;
			break;
		}

printf(nombreComando);

	if(comandoOk) {
		if(vectorDeParametros[i]==1 && parametro.empty()!=1) {
			if(i == 2) {
				parametroOk = 1;
			}
			else if(nombreComando == vectorDeComandos[3] || nombreComando == vectorDeComandos[4] || nombreComando == vectorDeComandos[5] || nombreComando == vectorDeComandos[6]) {
				parametroOk = parsearParametro(parametro);
			}

			if(parametroOk)
				logger.LoguearInformacion("El parametro esta OK.", APP_NAME_FOR_LOGGER);
			else
				logger.LoguearInformacion("El parametro NO esta OK.", APP_NAME_FOR_LOGGER);
		}
		else if(vectorDeParametros[i]==0 && parametro.empty()==1){
			parametroOk = 1;
			logger.LoguearInformacion("El parametro esta OK.", APP_NAME_FOR_LOGGER);
		}

		logger.LoguearInformacion("El nombre del comando esta OK.", APP_NAME_FOR_LOGGER);
	}
	else
		logger.LoguearInformacion("El nombre del comando NO esta OK.", APP_NAME_FOR_LOGGER);


	logger.LoguearDebugging("<-- Comando::validacion()", APP_NAME_FOR_LOGGER);
	if(comandoOk + parametroOk == 2)
		return 1;
	else
		return 0;
}

int Comando::parsearParametro(string parametro) {
	logger.LoguearDebugging("--> Comando::parsearParametro()", APP_NAME_FOR_LOGGER);
	int i = 0;


	if(parametro[i] == '@') {
		return 0;
	}
	if(parametro[parametro.length()-1] == '@') {
		return 0;
	}

	while(parametro[i] != '@' && i < parametro.length()) {
		i = i + 1;
	}

	logger.LoguearDebugging("<-- Comando::parsearParametro()", APP_NAME_FOR_LOGGER);
	if(i == parametro.length()) {
		return 0;
	}
	else {
		return 1;
	}
}


string Comando::sacaEspaciosIzquierda(string cadena) {
	logger.LoguearDebugging("--> Comando::sacaEspaciosIzquierda()", APP_NAME_FOR_LOGGER);
	string cadenaTransformada;
	int desde = 0;
	desde = consumeEspaciosDesde(desde,cadena);
	for(int i=desde;i<cadena.length();i++)
		cadenaTransformada=cadenaTransformada+cadena[i];

	logger.LoguearDebugging("<-- Comando::sacaEspaciosIzquierda()", APP_NAME_FOR_LOGGER);
	return cadenaTransformada;

}

int Comando::consumeEspaciosDesde(int posicion,string cadena){
	logger.LoguearDebugging("--> Comando::consumeEspaciosDesde()", APP_NAME_FOR_LOGGER);
	/* PARA DETECTAR ESPACIOS EXISTE LA FUNCION isspace(char*), PARA PODER PASARLE UN CARACTER DE LA CADENA
	 * DEBO HACER (cadena.c_str())[posicion]
	 */
	int i;
	for(i = posicion;isspace(cadena.c_str()[i]);i++);

	logger.LoguearDebugging("<-- Comando::consumeEspaciosDesde()", APP_NAME_FOR_LOGGER);
	return i;
}

void Comando::extraerNombreYParametro(string comandoEntero){
	logger.LoguearDebugging("--> Comando::extraerNombreYParametro()", APP_NAME_FOR_LOGGER);

	int i = 0;
	int espaciosDpsComando ;

	comandoEntero = sacaEspaciosIzquierda(comandoEntero);
	//EXTRAE EL COMANDO DE LA CADENA INGRESADA

	for(i;
			i < comandoEntero.length() &&
					!isspace(comandoEntero.c_str()[i]);
			i++)
		nombreComando = nombreComando + comandoEntero[i];

	//ME FIJO SI ES LIST YA QUE PUEDE SEGUIR LA PALABRA NEWSGROUPS
	espaciosDpsComando = consumeEspaciosDesde(i,comandoEntero);
	if(nombreComando=="LIST")
	{
		nombreComando = nombreComando + ' ';
		for(i=espaciosDpsComando;i<comandoEntero.length()&&!isspace(comandoEntero.c_str()[i]);i++)
			nombreComando = nombreComando + comandoEntero[i];
	}
cout<<"nombre del comando"<<nombreComando<<endl;
	// EL RESTO DE LA CADENA ES EL PARAMETRO.
	espaciosDpsComando = consumeEspaciosDesde(i,comandoEntero);
	for( i = espaciosDpsComando;i<comandoEntero.length();i++)
	{
		parametro = parametro + comandoEntero[i];
	}

	// convierto a mayúsculas
	std::transform(nombreComando.begin(), nombreComando.end(), nombreComando.begin(), (int(*)(int)) toupper);

	logger.LoguearDebugging("<-- Comando::extraerNombreYParametro()", APP_NAME_FOR_LOGGER);
}

int Comando::indicaSalida() {
	return nombreComando.compare("QUIT");
}
