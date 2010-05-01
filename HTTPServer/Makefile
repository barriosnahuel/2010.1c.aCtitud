HTTPServer: httpServer.o
	gcc -lsocket -lnsl -o HTTPServer ./httpServer.o
	@echo '* Finished building target: $@'
	@echo ''

nntpClient.o: httpServer.c
	gcc -c -o httpServer.o httpServer.c

	
#	-------------------------------------------------------	
#	Nota:	Para levantar el openDS cada vez (cada vez que iniciamos sesion) podemos ejecutar "make OpenDS-Start".
OpenDS-Start:
	/export/home/osol/OpenDS-2.0.0/bin/./start-ds

#	-------------------------------------------------------
#	Nota:	Podemos ejecutar "make clean" y esto elimina todos los *.o para poder generarlos
#			nuevamente cuando nos dice ya estan compilados con la ultima version.
clean:
	rm *.o
	@echo '* Se borraron todos los archivos ".o" (object).'
	@echo ''