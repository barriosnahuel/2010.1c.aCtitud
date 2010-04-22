#include<stdio.h>
#include<iostream.h>
#include<string.h>
using namespace std;
class Usuario
{
    public:
        char* comando;

    void setComando(char* comandoRecibido)
    {
        comando = new char [sizeof(char)*strlen(comandoRecibido)+1];
        comando=comandoRecibido;
        cout<<"ComandoRecibido:"<<comando<<endl;
    }
    /*char* getComando()
    {
        return &comando;
    }*/
};

int main(){
    Usuario usuario;
    char* comandoSeleccionado;
    cout<<"Ingrese el comando:"<<endl;
    cin >> comandoSeleccionado;
    usuario.setComando(comandoSeleccionado);
    //cout <<"Se guardo el comando"<<usuario.getComando();
    return 1;
}
