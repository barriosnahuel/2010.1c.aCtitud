#include <iostream>
#include"funcionesMSMQ.hpp"
#include "logger-win.hpp"
using namespace std;
extern Logger logger;

HRESULT MsmqProcess::crearCola()
{
	logger.LoguearDebugging("--> MsmqProcess::crearCola");
	OleInitialize(NULL);    // Hay que inicializar OLE para usar el COM de MSMQ//
	IMSMQQueueInfoPtr qInfo("MSMQ.MSMQQueueInfo");
	qInfo->PathName = ".\\Private$\\colaDeNoticias";
	try
	{
	  qInfo->Create();
	  logger.LoguearDebugging("Se creo la cola de mensajes");
	  logger.LoguearInformacion("Se creo la cola de mensajes");
	}
	catch (_com_error comerr)
	{
		/* En caso de que la cola ya exista, ignoro el error y uso la que ya existe. */
		logger.LoguearDebugging("Ya existe una cola con dicho nombre, por lo tanto no sera necesaria su creacion.");
		logger.LoguearInformacion("Ya existe una cola con dicho nombre, por lo tanto no sera necesaria su creacion.");
	}
	CoUninitialize();
	logger.LoguearDebugging("<-- MsmqProcess::crearCola");
	return 0;
}

IMSMQMessagePtr MsmqProcess::desencolarMensaje()
{
	logger.LoguearDebugging("--> MsmqProcess::desencolarMensaje");
  OleInitialize(NULL);                                 // Hay que inicializar OLE//
  IMSMQQueueInfoPtr qInfo("MSMQ.MSMQQueueInfo");
  IMSMQQueuePtr pQueue;
  IMSMQMessagePtr pMsg("MSMQ.MSMQMessage");
  qInfo->PathName = ".\\Private$\\colaDeNoticias";

  
  _variant_t    timeOut((long)1000);                   // 1 segundo de time-out
  _variant_t    wantBody((bool)true);                  // Configuramos para recibir el body
  
  //Abrimos la cola con derecho de Lectura
  
  pQueue = qInfo->Open(MQ_RECEIVE_ACCESS, MQ_DENY_NONE); 
  pMsg = pQueue->Receive(&vtMissing, &vtMissing, &wantBody, &timeOut);  //Recibimos el mensaje
  if(pMsg == NULL) {
	  logger.LoguearDebugging("No hay mensajes en la cola");
	  return NULL;
  }
  pQueue->Close();
  CoUninitialize();
  return pMsg;
  logger.LoguearDebugging("<-- MsmqProcess::desencolarMensaje");
}

void MsmqProcess::leerMensajes()
{
	logger.LoguearDebugging("--> MsmqProcess::leerMensajes");
  OleInitialize(NULL);                                 // Hay que inicializar OLE//
  IMSMQQueueInfoPtr qInfo("MSMQ.MSMQQueueInfo");
  IMSMQQueuePtr pQueue;
  IMSMQMessagePtr pMsg("MSMQ.MSMQMessage");
  qInfo->PathName = ".\\Private$\\colaDeNoticias";

  
  _variant_t    timeOut((long)1000);                   // 1 segundo de time-out
  _variant_t    wantBody((bool)true);                  // Configuramos para recibir el body
  
  //Abrimos la cola con derecho de Lectura
  
  pQueue = qInfo->Open(MQ_RECEIVE_ACCESS, MQ_DENY_NONE); 
  pMsg = pQueue->Receive(&vtMissing, &vtMissing, &wantBody, &timeOut);  //Recibimos el mensaje
  if(pMsg == NULL) 
	  logger.LoguearDebugging("No hay mensajes en la cola");
  else {
	  cout << "Label: " << pMsg->Label << endl;
	  cout << "Body: " << (char *)(_bstr_t) pMsg->Body << endl;
  }
  pQueue->Close();
  CoUninitialize();
  logger.LoguearDebugging("<-- MsmqProcess::leerMensajes");
  return;
}

void MsmqProcess::insertarMensaje(IMSMQMessagePtr pMsg)
{	
	logger.LoguearDebugging("--> MsmqProcess::insertarMensaje");
  OleInitialize(NULL);                                   // Hay que inicializar OLE//
  IMSMQQueueInfoPtr qInfo("MSMQ.MSMQQueueInfo");
  IMSMQQueuePtr qSend;
  qInfo->PathName = ".\\Private$\\colaDeNoticias";
  //Abro la Cola de Mensajería con derechos de escritura
  qSend = qInfo->Open(MQ_SEND_ACCESS, MQ_DENY_NONE);
  pMsg->Send(qSend);   //enviamos el mensaje
  qSend->Close();    //Cerramos la cola de mensajería
  CoUninitialize();
  logger.LoguearDebugging("<-- MsmqProcess::insertarMensaje");
  return;
}
