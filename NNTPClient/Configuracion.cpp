#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include "Configuracion.hpp"

using namespace std;

//constructor
Configuracion::Configuracion() {
    czServidor = new char[40];
    memset(czServidor, 0, 40);
};

//destructor
Configuracion::~Configuracion() {
    /* libera memoria */
    delete [] czServidor;
};

int Configuracion::cargarDefault() {
    return cargarDesdeArchivo("cliente.conf");
};

int Configuracion::cargarDesdeArchivo(const char *czFilename) {

	char *szLinea, *pszAux;

	szLinea = new char[100]; // línea que se va a leer
	pszAux = new char[1000]; // se va a guardar todo el archivo
    memset(szLinea,0,100);
	szLinea[0] = '\0';
	pszAux[0] = '\0';

    ifstream pfsArchConfig(czFilename, ios::in);
	if(!pfsArchConfig) {
	    cerr << "No se pudo abrir el archivo de configuracion " << czFilename << ".\n" << endl;
		return 0;
	};

	// cargo todo el archivo en el buffer
	while(!pfsArchConfig.eof()) {
		pfsArchConfig.getline(szLinea, 100);
		if (*szLinea == '\n' || *szLinea == '#')
			continue;
		strcat(pszAux, szLinea);
		strcat(pszAux, "\n");
	}

	pfsArchConfig.close();

	// cargo el puerto
	iPuerto = atoi(GetVal("PORT=", pszAux));
	if(iPuerto < 1 || iPuerto > 65535 ) {
	    cerr << "Puerto invalido." << endl;
		return 0;
	}

    // cargo el servidor
	strcpy(czServidor,GetVal("SERVER=", pszAux));

	delete [] szLinea;
	delete [] pszAux;
	return 1;

};

int Configuracion::cargarDesdeParametros(const char *czHost, int iPort){
	// cargo el puerto
	if(iPort < 1 || iPort > 65535 ) {
	    cerr << "Puerto invalido." << endl;
		return 0;
	}
	iPuerto = iPort;

    // cargo el servidor
	strcpy(czServidor,czHost);

};

char * Configuracion::getServidor() {
    return czServidor;
};

int Configuracion::getPuerto() {
    return iPuerto;
};

char *Configuracion::GetVal(const char *sValBuff, const char *sBuff){
    char *sVal;
    char *sVal2;

    sVal = new char[50];
    sVal2 = new char[2];
    memset(sVal2,0,2);
    strcpy(sVal2,"0");

    sVal[0] = '\0';
    sVal = strstr(sBuff, sValBuff);
    sVal = strtok(sVal, "=");
    if(!sVal) {
        delete [] sVal;
        return sVal2;
    };

    sVal = strtok(NULL, "\n");
    sVal[strlen(sVal)] = '\0';

    delete [] sVal2;
    return sVal;
}

int Configuracion::Valida_IP(const char *ip) {
   int tam, cont, idx;
   char *ptr, ipaux[15+1];
   cont = 0;
   idx= 0;

   if (!ip) return 0;
   strcpy(ipaux, ip);
   ptr = strtok(ipaux, "." );
   while(ptr) {
      tam = strlen(ptr);
      if ( tam < 1 || tam > 3 ) return 0;					/*	Se valida que la longitud sea de 1 a 3	*/

      /*	Valido que cada caracter sea un numero, y no haya letras	*/
      for(idx= 0; idx<tam; idx++){
    	  if(!isdigit(*(ptr+idx)))
    		  return 0;
      }

      if ( atoi(ptr) < 0 || atoi(ptr) > 255 ) return 0;		/*	Se valida que sea un numero entre 0-255*/
      ptr = strtok ( NULL, "." );
      cont = cont + 1;
   }
   if (cont < 4) return 0;									/*	Valido que la IP tenga al menos 4 partes.	*/
   return 1;
}
