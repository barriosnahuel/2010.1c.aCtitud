#include "NNTPClientDAO.hpp"
#include <iostream>

//Constructor
NNTPClientDAO::NNTPClientDAO() {}

//Destructor
NNTPClientDAO::~NNTPClientDAO() {}

void NNTPClientDAO::OpenConnection(const char *hostname, int port) {
        struct hostent *host = gethostbyname(hostname);
        struct sockaddr_in addr;

        sdServer = socket(PF_INET, SOCK_STREAM, 0);
        bzero(&addr, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = *(long*)(host->h_addr);
        if (connect(sdServer,(const sockaddr*) &addr, sizeof(addr))!=0) {
                cout << "Fallo en el connect del socket al servidor NNTP" << endl;
                close(sdServer);
                perror(hostname);
                abort();
        }
}

/* Levanta un nuevo contexto para arrancar la conexión.*/
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

    // FGuerra - TODO: Obviamente, ver adonde chota nos vamos a conectar.
    OpenConnection(hostname, port);

    ssl = SSL_new(ctx);

    /* "Bindeo" el socket con la conexión SSL.
        Si no quiero enviar mensajes por SSL usaré sdServer.send().
        Caso contrario usaré ssl. */
        SSL_set_fd(ssl, sdServer);

        if ( SSL_connect(ssl) == -1 ) {
                cout << "Conexión al servidor fallida." << endl;
        } else {
                cout << "Conectado! Usando encriptado: " << SSL_get_cipher(ssl) << endl;
        }
}

void NNTPClientDAO::cerrarConexion(void) {
    cout << "Se iniciará el cierre de la conexión con el servidor" << endl;

    SSL_free(ssl);
    close(sdServer);
    SSL_CTX_free(ctx);
    cout << "Se cerró la conexión con el servidor y se liberaron todos los recursos." << endl;
}

void NNTPClientDAO::enviarMensaje(string comandoEscritoPorUsuario) {
    cout << "Se intentará enviar el mensaje: " << comandoEscritoPorUsuario << " cuya longitud es: " << comandoEscritoPorUsuario.length() << endl;
    SSL_write(ssl, (const void*) &comandoEscritoPorUsuario, comandoEscritoPorUsuario.length());
}

string NNTPClientDAO::recibirRespuesta() {
        int bytesLeidos;
       
        // EN LA LINEA SIGUIeNTe Se MUeRe LA 2DA VEZ!!!! 
        bytesLeidos = SSL_read(ssl, cBuffer, sizeof(cBuffer));
        cBuffer[bytesLeidos] = '\0';

        return cBuffer;
}
