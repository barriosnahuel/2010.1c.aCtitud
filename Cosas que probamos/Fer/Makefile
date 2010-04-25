NNTPClient: NNTPClientDAO.o NNTPClientDAO.cpp
	g++ -lssl -o  NNTPClient ./NNTPClientDAO.o
	@echo 'Finished building target: $@'
	@echo ' '


NNTPClientDAO.o: NNTPClientDAO.cpp
	g++ -o NNTPClientDAO.o NNTPClientDAO.cpp

#	Nota:	Podemos ejecutar "make clean" y esto elimina todos los *.o para poder generarlos
#			nuevamente cuando nos dice ya estan compilados con la ultima version.
clean:
	rm *.o
