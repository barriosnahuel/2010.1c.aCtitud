/*
 * NNTPClientDAO.cpp
 *
 *  Created on: Apr 23, 2010
 *      Author: Barrios, Nahuel.
 */

#include "NNTPClientDAO.hpp"
#include <iostream>

//Constructor.
NNTPClientDAO::NNTPClientDAO() {}

//Destructor.
NNTPClientDAO::~NNTPClientDAO() {}

int OpenConnection(const char *hostname, int port)
{
        int sd;
        struct hostent *host;
        struct sockaddr_in addr;

        sd = socket(PF_INET, SOCK_STREAM, 0);
        bzero(&addr, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = *(long*)(host->h_addr);
        if ( connect(sd,(const sockaddr*) &addr, sizeof(addr)) != 0 )
        {
                close(sd);
                perror(hostname);
                abort();
        }
        return sd;
}

/* Levanta un nuevo contexto para arrancar la conexión.*/
SSL_CTX* InitCTX(void)
{
        SSL_METHOD *method;
        SSL_CTX *ctx;

        SSL_library_init();
        OpenSSL_add_all_algorithms();
        SSL_load_error_strings();
        method = SSLv23_client_method();
        ctx = SSL_CTX_new(method);
        if ( ctx == NULL )
        {
                cout << "ERROR!! El ctx es nullo" << endl;
                abort();
        }
        return ctx;
}

void NNTPClientDAO::abrirConexion(void)
{
        cout << "Se iniciará la apertura de la conexión con el servidor" << endl;

        ctx = InitCTX();

        // FGuerra - TODO: Obviamente, ver adonde chota nos vamos a conectar.
        server = OpenConnection("135.20.213.111", 5000);

        ssl = SSL_new(ctx);

        /* "Bindeo" el socket con la conexión SSL. Si no quiero enviar mensajes por SSL usaré server.send(). Caso contrario
        usaré ssl. */
        SSL_set_fd(ssl, server);
        if ( SSL_connect(ssl) == -1 )
        {
                cout << "Conexión al servidor fallida." << endl;
        } else {
                cout << "Conectado! Usando encriptado: " << SSL_get_cipher(ssl) << endl;
        }
}

void NNTPClientDAO::cerrarConexion(void) {
    cout << "Se iniciará el cierre de la conexión con el servidor" << endl;

    SSL_free(ssl);
    close(server);
    SSL_CTX_free(ctx);
    cout << "Se cerró la conexión con el servidor y se liberaron todos los recursos." << endl;
}

string NNTPClientDAO::enviarMensaje(string comandoEscritoPorUsuario) {
        int cantidadBytesDeRespuesta;
        cout << "Se intentará enviar el mensaje: " << comandoEscritoPorUsuario << "cuya longitud es: " << comandoEscritoPorUsuario.length() << endl;

        // Envío el comando al servidor.
        SSL_write(ssl, (const void*) &comandoEscritoPorUsuario, comandoEscritoPorUsuario.length());

        // Me responde la cantidad de bytes de la respuesta.
        cantidadBytesDeRespuesta = SSL_read(ssl, buf, sizeof(buf));

        return buf;
}



int main() {
    cout << "esto anduvo :O" << endl;
    return 1;
    }

