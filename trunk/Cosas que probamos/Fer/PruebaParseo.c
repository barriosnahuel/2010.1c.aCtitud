#include <stdio.h>

char* obtenerRecursoDeCabecera(char* sMensajeHTTPCliente);
char* sacarEspacios(char* sMensajeHTTPCliente);

int main() {
	char cadenaAParsearDePrueba[1024] = "GET /pagina12/noticia1.html HTTP/1.1\n";
	char recursoObtenido[1024];
	
	printf("La cadena a parsear es: %s", cadenaAParsearDePrueba);
	obtenerRecursoDeCabecera(cadenaAParsearDePrueba);
	printf("El recurso obtenido es: %s", obtenerRecursoDeCabecera(cadenaAParsearDePrueba));
	return 1;
	
	
}

char* obtenerRecursoDeCabecera(char* sMensajeHTTPCliente) {
	int i, j, k = 0;
	char recurso[1024];
	
	while(sMensajeHTTPCliente[i] != '/') {
		i = i + 1;
	}
	printf("Saque el GET\n");
	k = i;
	
	while(sMensajeHTTPCliente[k] != ' ') {
		k = k + 1;
	}
	printf("Ubique el puntero al final del recurso (tengo a i y a k en el ppcio y en el final)\n");
	
	while(i != k) {
		recurso[j] = sMensajeHTTPCliente[i];
		i = i + 1;
		j = j + 1;
	}
	printf("Voy avanzando i y completando el nuevo string\n");
	//while(sMensajeHTTPCliente[i] != '.') {
		//recurso[j] = sMensajeHTTPCliente[i];
		//i = i + 1;
		//j = j + 1;
	//}
	recurso[j] = '\n';
	return recurso;
	
}