#ifndef COMANDO_H_
#define COMANDO_H_

#include <vector>
#include <iostream>

using namespace std;

class Comando {
	//	Lo privado
    string parametro;
    string nombreComando;
    string respuesta;
    string vectorDeComandos[8];
    int    vectorDeParametros[8];

	int    indicaSalida(void);
    void   inicializarComandos(void);
    string sacaEspaciosIzquierda(string cadena);
    void   extraerNombreYParametro(string comandoEntero);
    int    consumeEspaciosDesde(int posicion,string cadena);
    int    validacion(void);
	void   reset(void);
public:
	Comando(void);
	~Comando(void);

	int    init(string strCadena);
	string cadenaIngresada(void);
	string respuestaObtenida(void);
	void   setRespuestaObtenida(string strRespuesta);
};

#endif /* COMANDO_H_ */
