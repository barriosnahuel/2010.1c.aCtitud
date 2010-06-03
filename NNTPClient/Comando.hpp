#ifndef COMANDO_H_
#define COMANDO_H_

#include <vector>
#include <iostream>

#include "../Logger/logger.hpp"

using namespace std;

class Comando {
	//	Lo privado
    string parametro;
    string nombreComando;
    string respuesta;
    string vectorDeComandos[8];
    int    vectorDeParametros[8];

    void   inicializarComandos(void);
    string sacaEspaciosIzquierda(string cadena);
	string Comando::sacarCorchetesAngulares(string parametro);
    void   extraerNombreYParametro(string comandoEntero);
    int    consumeEspaciosDesde(int posicion, string cadena);
	void   reset(void);
public:
	Comando(void);
	~Comando(void);

	int    init(string strCadena);
	string cadenaIngresada(void);
	string respuestaObtenida(void);
	void   setRespuestaObtenida(string strRespuesta);
    int    indicaSalida(void);
    int    validacion(void);
    int	   parsearParametro(string parametro);
};

#endif /* COMANDO_H_ */
