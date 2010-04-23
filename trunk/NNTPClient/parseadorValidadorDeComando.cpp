#include <vector>
#include "parseadorValidadorDeComando.hpp"
#include <ctype.h>
#include <iostream>


using namespace std;

//Imple del constructor
ParseadorValidadorDeComando::ParseadorValidadorDeComando() {
   cout << "ParseadorValidadorDeComando creado." << endl;
}

//Imple del destructor
ParseadorValidadorDeComando::~ParseadorValidadorDeComando() {
   cout << "ParseadorValidadorDeComando destruído." << endl;
}

//Implemento el metodo
Comando ParseadorValidadorDeComando::validarYConvertirAObjetoComando(string* comandoEscritoPorElUsuario) {
    //Aca les dejo un ejemplo de como creo el objeto, le seteo cosas y lo retorno.
    Comando comando = Comando();
    comando.setLlevaParametro(1);
    comando.setNombreComando("Article");
    return comando;

};

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
