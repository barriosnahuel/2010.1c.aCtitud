#ifndef __PARSEADORVALIDADORDECOMANDO_H__
#define __PARSEADORVALIDADORDECOMANDO_H__

#include "comando.h"


class parseadorValidadorDeComando
{
    public:
            //Constructor
            parseadorValidadorDeComando(void);

            //Metodo
            string validarYConvertirAObjetoComando(string* comandoEscritoPorElUsuario);
    }
#endif
