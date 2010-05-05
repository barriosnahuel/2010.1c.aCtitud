NNTPClient: Configuracion.o Comando.o NNTPClientDAO.o nntpClient.o
	g++ -lpthread -lssl -o NNTPClient ./Configuracion.o ./Comando.o ./NNTPClientDAO.o ./nntpClient.o
	@echo '* Finished building target: $@'
	@echo ''

Configuracion.o: Configuracion.cpp Configuracion.hpp
	g++ -c -o Configuracion.o Configuracion.cpp

Comando.o: Comando.cpp Comando.hpp
	g++ -c -o Comando.o Comando.cpp

NNTPClientDAO.o: NNTPClientDAO.cpp NNTPClientDAO.hpp
	g++ -c -o NNTPClientDAO.o NNTPClientDAO.cpp

nntpClient.o: nntpClient.cpp
	g++ -c -o nntpClient.o nntpClient.cpp


#	Nota:	Podemos ejecutar "make clean" y esto elimina todos los *.o para poder generarlos
#			nuevamente cuando nos dice ya estan compilados con la ultima version.
clean:
	rm *.o
	@echo '* Se borraron todos los archivos ".o" (object).'
	@echo ''
