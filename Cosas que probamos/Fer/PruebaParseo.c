#include <stdio.h>

char* obtenerRecursoDeCabecera(char* sMensajeHTTPCliente);
char* sacarEspacios(char* sMensajeHTTPCliente);

int main() {
	char cadenaAParsearDePrueba[1024] = "GET /pagina12/noticia1.html HTTP/1.1\n";
	
	printf("La cadena a parsear es: %s", cadenaAParsearDePrueba);
	printf("El recurso obtenido es: %s", obtenerRecursoDeCabecera(cadenaAParsearDePrueba));
	return 1;
	
	
}

char* obtenerRecursoDeCabecera(char* sMensajeHTTPCliente) {
	int i = 0;
	int j = 0;
	int k = 0;
	char recurso[1024];
	
	/* En este while saco el GET */
	while(sMensajeHTTPCliente[i] != '/') {
		i = i + 1;
	}
	k = i;
	
	/* Aca situo a k al final del recurso (donde esta el primer espacio) */
	while(sMensajeHTTPCliente[k] != '.') {
		k = k + 1;
	}
	
	/* Cuando i = k quiere decir que i llego al .html (al punto en realidad). O sea que ya obtuve el recurso */
	while(i != k) {
		recurso[j] = sMensajeHTTPCliente[i];
		i = i + 1;
		j = j + 1;
	}
	
	recurso[j] = '\n';
	return recurso;
	
}