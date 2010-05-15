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

    // FGuerra - TODO: Obviamente, ver adonde chota nos vamos a conectar.
    OpenConnection(hostname, port);

    ssl = SSL_new(ctx);

    /* "Bindeo" el socket con la conexi�n SSL.
        Si no quiero enviar mensajes por SSL usar� sdServer.send().
        Caso contrario usar� ssl. */
        SSL_set_fd(ssl, sdServer);

        if ( SSL_connect(ssl) == -1 ) {
                cout << "Conexi�n al servidor fallida." << endl;
        } else {
                cout << "Conectado! Usando encriptado: " << SSL_get_cipher(ssl) << endl;
        }
}

void NNTPClientDAO::cerrarConexion(void) {
    cout << "Se iniciar� el cierre de la conexi�n con el servidor" << endl;

    SSL_free(ssl);
    close(sdServer);
    SSL_CTX_free(ctx);
    cout << "Se cerr� la conexi�n con el servidor y se liberaron todos los recursos." << endl;
}

void NNTPClientDAO::enviarMensaje(string comandoEscritoPorUsuario) {
    int bytesEscritos;
    
    cout << "Se intentar� enviar el mensaje: " << comandoEscritoPorUsuario << " cuya longitud es: " << comandoEscritoPorUsuario.length() << endl;
    
    bytesEscritos = SSL_write(ssl, (const void*) &comandoEscritoPorUsuario, comandoEscritoPorUsuario.length());
    if (bytesEscritos > 0) {
        cout << "-------- ok" << endl;
    cout << "estos son los bytes escritos" << bytesEscritos << endl;}
    else
        cout << "-------- MAL!!!" << endl;
}

string NNTPClientDAO::recibirRespuesta() {
        int bytesLeidos= 0;

printf("1 lo que quedo: %s\n", cBuffer);
        memset(cBuffer, 0, 3064);
        // EN LA LINEA SIGUIeNTe Se MUeRe LA 2DA VEZ!!!! 

        printf("2 lo limpie: %s\n", cBuffer);

        printf("sizeof buffer vale: %d\n", sizeof(cBuffer));
        bytesLeidos = SSL_read(ssl, cBuffer, sizeof(cBuffer));

        enviarMensaje("ARTICLE 69");
        char cBufferPrueba[3064];
        SSL_read(ssl, cBufferPrueba, sizeof(cBufferPrueba));

        printf("el buffer de prueba: %s\n", cBufferPrueba);

        printf("bytesleidos: %d\n", bytesLeidos);
        cBuffer[bytesLeidos] = '\0';

        printf("3: %s\n", cBuffer);


        return cBuffer;
}
