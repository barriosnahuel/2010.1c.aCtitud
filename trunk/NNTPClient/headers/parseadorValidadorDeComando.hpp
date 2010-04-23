#include "comando.hpp"
#include <iostream>
using namespace std;


class ParseadorValidadorDeComando
{
    public:
    //Constructor
    ParseadorValidadorDeComando();
    //Destructor
    ~ParseadorValidadorDeComando();
    //Metodo
    Comando validarYConvertirAObjetoComando(string* comandoEscritoPorElUsuario);



};
