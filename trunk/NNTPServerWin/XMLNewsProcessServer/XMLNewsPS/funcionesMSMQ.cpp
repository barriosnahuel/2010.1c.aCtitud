#include <iostream>
#include"funcionesMSMQ.hpp"
using namespace std;

HRESULT MsmqProcess::crearCola()
{
	OleInitialize(NULL);    // Hay que inicializar OLE para usar el COM de MSMQ//
	IMSMQQueueInfoPtr qInfo("MSMQ.MSMQQueueInfo");
	qInfo->PathName = ".\\Private$\\colaDeNoticias";
	try
	{
	  cout<<"Se intentara crear la cola de mensajes"<<endl;
	  qInfo->Create();
	  cout<<"Se creo la cola de mensajes"<<endl;
	}
	catch (_com_error comerr)
	{
	  cout<<"Ya existia una cola con el mismo nombre!!"<<endl;
	  CoUninitialize();
	}
	CoUninitialize();
	return 0;
}

void MsmqProcess::leerMensajes()
{
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
	  cout<<"No hay mensajes en la cola";
  else
	  cout<<"Label: "<<pMsg->Label;//cout<< pMsg->Body;
  pQueue->Close();
  CoUninitialize();
  return;
}

void MsmqProcess::insertarMensajes()
{	
  OleInitialize(NULL);                                   // Hay que inicializar OLE//
  IMSMQQueueInfoPtr qInfo("MSMQ.MSMQQueueInfo");
  IMSMQQueuePtr qSend;
  IMSMQMessagePtr pMsg("MSMQ.MSMQMessage");
  qInfo->PathName = ".\\Private$\\colaDeNoticias";
  //Abro la Cola de Mensajería con derechos de escritura
  qSend = qInfo->Open(MQ_SEND_ACCESS, MQ_DENY_NONE);         
  pMsg->Label = "Probando el Label";                     //Agrego el Label y Body y envío el mensaje
  pMsg->Body = "Este TP lo aprobamos";
  pMsg->Send(qSend);   //enviamos el mensaje
  qSend->Close();    //Cerramos la cola de mensajería
  CoUninitialize();
  return;
}
