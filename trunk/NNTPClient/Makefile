NNTPClient: semaforo.o Comando.o NNTPClientDAO.o nntpClient.o
	g++ -lpthread -lssl -o NNTPClient ./semaforo.o ./Comando.o ./NNTPClientDAO.o ./nntpClient.o
	@echo '* Finished building target: $@'
	@echo ''

	
semaforo.o: semaforo.cpp semaforo.hpp
	g++ -c -o semaforo.o semaforo.cpp

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
