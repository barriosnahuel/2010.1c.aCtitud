#ifndef NNTPCLIENTDAO_H_
#define NNTPCLIENTDAO_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <string>

#include <openssl/ssl.h>

using namespace std;

class NNTPClientDAO {
		SSL_CTX *ctx;
		int sdServer;
		SSL *ssl;
		char cBuffer[1024];

		void InitCTX(void);
		void OpenConnection(const char *hostname, int port);
	
    public:
        NNTPClientDAO();
        virtual ~NNTPClientDAO();

        void abrirConexion(void);
        void cerrarConexion(void);
        void enviarMensaje(string);

};

#endif /* NNTPCLIENTDAO_H_ */
