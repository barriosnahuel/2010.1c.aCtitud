/*
 * NNTPClientDAO.h
 *
 *  Created on: Apr 23, 2010
 *      Author: Barrios, Nahuel.
 */

#ifndef NNTPCLIENTDAO_H_
#define NNTPCLIENTDAO_H_

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <openssl/ssl.h>
#include <openssl/ssl2.h>
#include <openssl/ssl3.h>
#include <openssl/ssl23.h>
#include <openssl/tls1.h>


using namespace std;

class NNTPClientDAO {

    private:
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
