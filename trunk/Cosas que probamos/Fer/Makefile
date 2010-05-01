HTTPServer: ldapwrapper.o httpServer.o
	gcc -lsocket -lnsl -o HTTPServer ./httpServer.o
	@echo '* Finished building target: $@'
	@echo ''

httpServer.o: httpServer.c
	gcc -c -o httpServer.o httpServer.c


#	-------------------------------------------------------
#	Nota:	Podemos ejecutar "make clean" y esto elimina todos los *.o para poder generarlos
#			nuevamente cuando nos dice ya estan compilados con la ultima version.
clean:
	rm *.o
	@echo '* Se borraron todos los archivos ".o" (object).'
	@echo ''
	