#include "NNTPClientDAO.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//Constructor
NNTPClientDAO::NNTPClientDAO() {}

//Destructor
NNTPClientDAO::~NNTPClientDAO() {}

void NNTPClientDAO::OpenConnection(const char *ipServer, int port) {
/*        struct hostent *host = gethostbyname(hostname);*/
        struct sockaddr_in addr;

        sdServer = socket(AF_INET, SOCK_STREAM, 0);
        bzero(&addr, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(ipServer);
        addr.sin_port = htons(port);
        if (connect(sdServer,(const sockaddr*) &addr, sizeof(addr))!=0) {
                cout << "Fallo en el connect del socket al servidor NNTP" << endl;
                close(sdServer);
                perror(ipServer);
                abort();
        }
}

/* Levanta un nuevo contexto para arrancar la conexi�n.*/
void NNTPClientDAO::InitCTX() {
        SSL_METHOD *method;
 
        SSL_library_init();
        OpenSSL_add_all_algorithms();
        SSL_load_error_strings();
        method = SSLv23_client_method();
        ctx = SSL_CTX_new(method);
        if ( ctx == NULL) {
                cout << "ERROR!! El ctx es null" << endl;
                abort();
        }
}

void NNTPClientDAO::abrirConexion(const char *hostname, int port) {
    InitCTX();
	cout << endl << hostname << ":" << port << endl;

    OpenConnection(hostname, port);

    ssl = SSL_new(ctx);

    /* "Bindeo" el socket con la conexion SSL.
        Si no quiero enviar mensajes por SSL usara send().
        Caso contrario usara ssl. */
    SSL_set_fd(ssl, sdServer);

    if ( SSL_connect(ssl) == -1 ) {
            cout << "Conexion al servidor fallida." << endl;
    } else {
            cout << "Conectado! Usando encriptado: " << SSL_get_cipher(ssl) << endl;
    }
}

void NNTPClientDAO::cerrarConexion(void) {
    cout << "Se iniciara el cierre de la conexion con el servidor" << endl;

    SSL_free(ssl);
    close(sdServer);
    SSL_CTX_free(ctx);
    cout << "Se cerro la conexion con el servidor y se liberaron todos los recursos." << endl;
}

void NNTPClientDAO::enviarMensaje(string comandoEscritoPorUsuario) {
    int bytesEscritos;
    
    bytesEscritos = SSL_write(ssl, comandoEscritoPorUsuario.c_str(), comandoEscritoPorUsuario.length());
    if (bytesEscritos <= 0)
        cout << "Hubo un error intentando escribir mediante un canal SSL. (Falta logguear esto)" << endl;
/*
    if (bytesEscritos > 0) {
        cout << "estos son los bytes escritos " << bytesEscritos << endl;}
    else
        cout << "-------- MAL!!!" << endl;
*/
}

string NNTPClientDAO::recibirRespuesta() {
        int bytesLeidos= 0;

		char cBuffer[3064];/*	TODO: Chequear este tamaño.	*/
        bytesLeidos = SSL_read(ssl, cBuffer, sizeof(cBuffer));
        cBuffer[bytesLeidos] = '\0';

        return cBuffer;
}
