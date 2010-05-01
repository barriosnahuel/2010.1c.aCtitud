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

----------- OpenDS -----------

Levantar openDS (asegurandonos que no hay otra instancia corriendo):
	make OpenDS-Start
	
Nota: Esto todavia no esta terminado, pero anda. Sin embargo hay que hacer mas cosas despues.


----------- Memcached -----------
?