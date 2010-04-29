#ifndef COMANDO_H_
#define COMANDO_H_

#include <vector>
#include <iostream>

using namespace std;

class Comando {
	//	Lo privado
	string cadenaIngresada;
    string parametro;
    string nombreComando;
    string respuestaAlUsuario;

    bool   llevaParametro;

    string vectorDeComandos[8];
    int    vectorDeParametros[8];

public:
	Comando(void);
	~Comando(void);

	void   reset(void);
	int    init(string cadena);
    void   inicializarComandos(void);

//    string getCadenaIngresada(void);
	
//  void   setNombreComando(string nombre);
//	string getNombreComando(void);
	
//    void   setRespuestaAlUsuario(string respuesta);
	string respuestaObtenida(void);
	
//    bool   getLlevaParametro(void);
	
//    string getParametro(void);

    string sacaEspaciosIzquierda(string cadena);
    void   extraerNombreYParametro(string comandoEntero);
    int    consumeEspaciosDesde(int posicion,string cadena);
    int    validacion(void);
};

#endif /* COMANDO_H_ */
