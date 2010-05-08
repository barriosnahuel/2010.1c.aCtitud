OPTFLAGS=-g
CFLAGS=-ansi

MemcachedServer: 
	gcc insertando.c -lmemcached
	@echo '* Finished building target: $@'
	@echo ''


#	-------------------------------------------------------
#	Nota:	Podemos ejecutar "make clean" y esto elimina todos los *.o para poder generarlos
#			nuevamente cuando nos dice ya estan compilados con la ultima version.
clean:
	rm *.o
	@echo '* Se borraron todos los archivos ".o" (object).'
	@echo ''
		