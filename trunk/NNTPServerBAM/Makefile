nntpServerBam: logger.o configuration.o conexion.o nntpServerBam.o
	gcc -ansi -lssl -o nntpServerBam ./logger.o ./configuration.o ./conexion.o ./nntpServerBam.o
	@echo '* Finished building target: $@'
	@echo ''

conexion.o: conexion.c conexion.h
	gcc -c -o conexion.o conexion.c

configuration.o: configuration.c configuration.h logger.h
	gcc -c -o configuration.o configuration.c

logger.o: logger.c logger.h
	gcc -c -o logger.o logger.c

nntpServerBam.o: nntpServerBam.c
	gcc -c -o nntpServerBam.o nntpServerBam.c

clean:
	rm *.o
	@echo '* Se borraron todos los archivos ".o" (object).'
	@echo ''
