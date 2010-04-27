#	Esto es porque podemos ejecutar make all, y si tuvieramos alguna otra aplicacion, la pondriamos como dependencia de "all" y 
#	podriamos compilar las 2 al mismo tiempo. De la misma manera, podemos ejecutar make HTTPServer para solo compilar el HTTP Server.
#	Esto es lo que tenemos que hacer para la entrega final: un inico makefile con todo!
all: HTTPServer

HTTPServer: httpServer.o httpServer.c
	gcc -o HTTPServer ./httpServer.o
	@echo '* Finished building target: $@'
	@echo ''

httpServer.o: httpServer.c
	gcc -c -o httpServer.o httpServer.c


#	Nota:	Podemos ejecutar "make clean" y esto elimina todos los *.o para poder generarlos
#			nuevamente cuando nos dice ya estan compilados con la ultima version.
clean:
	rm *.o
	@echo '* Se borraron todos los archivos ".o" (object).'
	@echo ''