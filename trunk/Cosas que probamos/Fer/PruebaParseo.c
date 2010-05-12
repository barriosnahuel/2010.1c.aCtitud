#include <stdio.h>

char* obtenerRecursoDeCabecera(char* sMensajeHTTPCliente);
char* sacarEspacios(char* sMensajeHTTPCliente);

int main() {
	char cadenaAParsearDePrueba[1024] = "GET / HTTP/1.1\n";
	char recursoObtenido[1024];
	
	printf("La cadena a parsear es: %s", recursoObtenido);
	obtenerRecursoDeCabecera(cadenaAParsearDePrueba);
	printf("El recurso obtenido es: %s", obtenerRecursoDeCabecera(cadenaAParsearDePrueba));
	return 1;
	
	
}

char* obtenerRecursoDeCabecera(char* sMensajeHTTPCliente) {
	int i = 0;
	int j = 0;
	char recurso[1024];
	
	while(sMensajeHTTPCliente[i] != '/') {
		i = i + 1;
	}
	
	while(sMensajeHTTPCliente[i] != '.') {
		recurso[j] = sMensajeHTTPCliente[i];
		i = i + 1;
		j = j + 1;
	}
	recurso[j] = '\n';
	return recurso;
	
}