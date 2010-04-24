NNTPClient: util.o util.cpp Comando.o Comando.cpp NNTPClientDAO.o NNTPClientDAO.cpp ParseadorValidadorDeComandos.o ParseadorValidadorDeComandos.cpp nntpClient.o nntpClient.cpp
	g++ -o NNTPClient ./util.o ./Comando.o ./NNTPClientDAO.o ./ParseadorValidadorDeComandos.o ./nntpClient.o
	@echo 'Finished building target: $@'
	@echo ' '

util.o: util.cpp
	g++ -c -o util.o util.cpp

Comando.o: Comando.cpp
	g++ -c -o Comando.o Comando.cpp

NNTPClientDAO.o: NNTPClientDAO.cpp
	g++ -c -o NNTPClientDAO.o NNTPClientDAO.cpp

ParseadorValidadorDeComandos.o: ParseadorValidadorDeComandos.cpp
	g++ -c -o ParseadorValidadorDeComandos.o ParseadorValidadorDeComandos.cpp

nntpClient.o: nntpClient.cpp
	g++ -c -o nntpClient.o nntpClient.cpp
