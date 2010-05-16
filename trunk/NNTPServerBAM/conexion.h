#ifndef _CONEXION_H
#define _CONEXION_H

int AbrirSocketEscucha(char *czIp, int iPuerto);

int AbrirSocketConexion(char *czIpRemoto, int iPuertoRemoto);

int Lee_Socket (int iSocket, void *Datos, int Longitud);

int Escribe_Socket (int iSocket, void *Datos, int Longitud);

int Acepta_Conexion_Cliente (int Descriptor);

#endif
