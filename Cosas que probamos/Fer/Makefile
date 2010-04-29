HTTPServer: httpServer.o httpServer.c
	gcc -o HTTPServer ./httpServer.o
	@echo '* Finished building target: $@'
	@echo ''

httpServer.o: httpServer.cpp
	gcc -c -o nntpClient.o nntpClient.cpp


#	Nota:	Podemos ejecutar "make clean" y esto elimina todos los *.o para poder generarlos
#			nuevamente cuando nos dice ya estan compilados con la ultima version.
clean:
	rm *.o
	@echo '* Se borraron todos los archivos ".o" (object).'
	@echo ''
