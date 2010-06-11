#ifndef MSMQ_H_
#define MSMQ_H_

#import "c:\windows\system32\mqoa.dll" no_namespace

class MsmqProcess{
	//Lo privado

public:
	HRESULT crearCola(void);
	void leerMensajes(void);
	void insertarMensaje(IMSMQMessagePtr pMsg);
};
#endif