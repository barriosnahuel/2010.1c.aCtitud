OPTFLAGS=-c -o

NNTPClient: Logger.o Configuracion.o Comando.o NNTPClientDAO.o nntpClient.o
	g++ -lpthread -lssl -o NNTPClient Logger.o ./Configuracion.o ./Comando.o ./NNTPClientDAO.o ./nntpClient.o
	@echo '* Finished building target: $@'
	@echo ''

Logger.o: logger.cpp logger.hpp
	g++ $(OPTFLAGS) Logger.o logger.cpp

Configuracion.o: Configuracion.cpp Configuracion.hpp
	g++ $(OPTFLAGS) Configuracion.o Configuracion.cpp

Comando.o: Comando.cpp Comando.hpp
	g++ $(OPTFLAGS) Comando.o Comando.cpp

NNTPClientDAO.o: NNTPClientDAO.cpp NNTPClientDAO.hpp
	g++ $(OPTFLAGS) NNTPClientDAO.o NNTPClientDAO.cpp

nntpClient.o: nntpClient.cpp
	g++ $(OPTFLAGS) nntpClient.o nntpClient.cpp

#	Nota:	Podemos ejecutar "make clean" y esto elimina todos los *.o para poder generarlos
#			nuevamente cuando nos dice ya estan compilados con la ultima version.
clean:
	rm *.o
	@echo '* Se borraron todos los archivos ".o" (object).'
	@echo ''
