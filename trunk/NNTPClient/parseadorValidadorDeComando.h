#ifndef __PARSEADORVALIDADORDECOMANDO_H__
#define __PARSEADORVALIDADORDECOMANDO_H__

#include <iostream.h>
#include "comando.h"

class parseadorValidadorDeComando
{
    public:
            //Constructor
            parseadorValidadorDeComando(void);

            //Metodo
            comando validarYConvertirAObjetoComando(string* comandoEscritoPorElUsuario);
    }
