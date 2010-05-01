----------- HTTPServer -----------

Compilar a mano:	
	gcc -lsocket -lnsl httpServer.c
Si no compila, probar con:		
	gcc -lsocket -lnsl -lresolv httpServer.c
	
Ejecutar:
	./a.out

--
Compilar con makefile:
	make
Otra opcion:
	make HTTPServer

Ejecutar:
	./HTTPServer

----------- Memcached -----------
?


----------- OpenDS -----------

Levantar OpenDS
	sh openDS.sh start

Detener OpenDS
	sh openDS.sh stop
	
Reiniciar OpenDS
	sh openDS.sh restart