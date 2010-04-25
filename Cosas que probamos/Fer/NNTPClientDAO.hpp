/*
 * NNTPClientDAO.h
 *
 *  Created on: Apr 23, 2010
 *      Author: Barrios, Nahuel.
 */

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
        int server;
        SSL *ssl;
        char buf[1024];
        int bytes;

    public:
        NNTPClientDAO();
        virtual ~NNTPClientDAO();

        void abrirConexion(void);
        void cerrarConexion(void);

        string enviarMensaje(string*);

};

#endif /* NNTPCLIENTDAO_H_ */
