#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <process.h>
#include "logger-win.h"

void LoguearInformacion(char *czData) {
	EscribirLog(1,czData);
};

void LoguearWarning(char *czData) {
	EscribirLog(2,czData);
};

void LoguearError(char *czData) {
	EscribirLog(3,czData);
};

void LoguearDebugging(char *czData) {
    EscribirLog(4,czData);
};

void EscribirLog(char cTipoLog, char *czData) {

   HANDLE  arch;
   HANDLE* memoryHandler;
   time_t tiempoActual;
   struct tm nuevoTiempoActual;
   extern char czNombreProceso[20];
   char *czNombreArch;
   char czFecha[100];
   char *czWriteBuff;
   DWORD dwBytesToWrite;
   DWORD dwBytesWritten = 0;
   
   memoryHandler =  HeapCreate(0,1024,0);

   CreateDirectoryA("./logs",NULL);

   czNombreArch = (char*)HeapAlloc(memoryHandler,HEAP_ZERO_MEMORY,40);
   ZeroMemory(czNombreArch,40);
   sprintf_s(czNombreArch, 40, "./logs/%s%d.LOG\0", czNombreProceso, _getpid());

   /* se abre o crea al archivo log */
   arch = CreateFile ( czNombreArch, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
   if(arch == INVALID_HANDLE_VALUE) {
	  //printf("Error al generar el archivo Log. (%d)\n", GetLastError());
      return;
   }

   /*Se obtiene el tiempo actual*/
   time(&tiempoActual);
   _localtime64_s(&nuevoTiempoActual,&tiempoActual);

   /*transforma los datos de fecha y hora a un formato de cadena*/
   strftime(czFecha, 100, "%d/%m/%Y %X", &nuevoTiempoActual);

   /* preparamos el texto a escribir */
   czWriteBuff = (char*)HeapAlloc(memoryHandler,HEAP_ZERO_MEMORY,200);
   ZeroMemory(czWriteBuff,200);
   sprintf_s(czWriteBuff, 200, "%s %s [%d][%d] ",czFecha,czNombreProceso,_getpid(),GetCurrentThreadId());

   dwBytesToWrite = (DWORD)strlen(czWriteBuff);

   /*registramos todo en el archivo */
   SetFilePointer(arch, 0, 0, FILE_END);
   WriteFile(arch,(LPCVOID)czWriteBuff,dwBytesToWrite,&dwBytesWritten,NULL);
   HeapFree(memoryHandler,HEAP_ZERO_MEMORY,czWriteBuff);

	switch(cTipoLog) {
		case _INFO:
			WriteFile(arch,"INFO",4,&dwBytesWritten,NULL);
			break;
		case _WARN:
			WriteFile(arch,"WARN",4,&dwBytesWritten,NULL);
			break;
		case _ERROR:
            WriteFile(arch,"ERROR",5,&dwBytesWritten,NULL);
            printf("ERROR: %s\n",czData);
			break;
		case _DEBUG:
			WriteFile(arch,"DEBUG",5,&dwBytesWritten,NULL);
			break;
		default:
			WriteFile(arch,"MSG",3,&dwBytesWritten,NULL);
			break;
	};

   czWriteBuff = (char*)HeapAlloc(memoryHandler,HEAP_ZERO_MEMORY,strlen(czData)+5);
   ZeroMemory(czWriteBuff,strlen(czData)+5);
   sprintf_s(czWriteBuff, strlen(czData)+5, ": %s\r\n",czData);
   dwBytesToWrite = (DWORD)strlen(czWriteBuff);
   WriteFile(arch,(LPCVOID)czWriteBuff,dwBytesToWrite,&dwBytesWritten,NULL);

   CloseHandle(arch);
   HeapFree(memoryHandler,HEAP_ZERO_MEMORY,czNombreArch);
   HeapFree(memoryHandler,HEAP_ZERO_MEMORY,czWriteBuff);
}