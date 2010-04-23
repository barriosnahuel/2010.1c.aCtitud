NNTPClientTest: Comando.o Comando.cpp NNTPClientBO.o NNTPClientBO.cpp NNTPClientDAO.o NNTPClientDAO.cpp ParseadorValidadorDeComandos.o ParseadorValidadorDeComandos.cpp nntpClient.o nntpClient.cpp
	g++ -c -o Comando.o Comando.cpp
	g++ -c -o NNTPClientBO.o NNTPClientBO.cpp
	g++ -c -o NNTPClientDAO.o NNTPClientDAO.cpp
	g++ -c -o ParseadorValidadorDeComandos.o ParseadorValidadorDeComandos.cpp
	g++ -c -o nntpClient.o nntpClient.cpp
	g++ -o NNTPClientTest ./Comando.o ./NNTPClientBO.o ./NNTPClientDAO.o ./ParseadorValidadorDeComandos.o ./nntpClient.o
	@echo 'Finished building target: $@'
	@echo ' '