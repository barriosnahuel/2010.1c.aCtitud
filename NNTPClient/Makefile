NNTPClient: Semaforo.o Semaforo.cpp Comando.o Comando.cpp NNTPClientDAO.o NNTPClientDAO.cpp ParseadorValidadorDeComandos.o ParseadorValidadorDeComandos.cpp nntpClient.o nntpClient.cpp
	g++ -lpthread -o NNTPClient ./Semaforo.o ./Comando.o ./NNTPClientDAO.o ./ParseadorValidadorDeComandos.o ./nntpClient.o
	@echo '* Finished building target: $@'
	@echo ''

Semaforo.o: Semaforo.cpp
	g++ -c -o Semaforo.o Semaforo.cpp

Comando.o: Comando.cpp
	g++ -c -o Comando.o Comando.cpp

NNTPClientDAO.o: NNTPClientDAO.cpp
	g++ -c -o NNTPClientDAO.o NNTPClientDAO.cpp

ParseadorValidadorDeComandos.o: ParseadorValidadorDeComandos.cpp
	g++ -c -o ParseadorValidadorDeComandos.o ParseadorValidadorDeComandos.cpp

nntpClient.o: nntpClient.cpp
	g++ -c -o nntpClient.o nntpClient.cpp
	
	
#	Nota:	Podemos ejecutar "make clean" y esto elimina todos los *.o para poder generarlos
#			nuevamente cuando nos dice ya estan compilados con la ultima version.
clean:
	rm *.o
	@echo '* Se borraron todos los archivos ".o" (object).'
	@echo ''