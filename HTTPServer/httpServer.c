/*
 * httpServer.c
 *
 *  Created on: Apr 24, 2010
 */


void* gestionarSolicitudDelClienteThread(void* parametro){
	if(/*//	TODO: No esta en el cache*/true){
		//	TODO: Lo busco en la DB.

		//	TODO: Guardo en cache.

		//	TODO: Formateo la respuesta a HTML.

		//	TODO: Devuelvo la respuesta. (Ver como hacer esto, que paremetro es "parametro".. capaz tengo que pasar el socket como parametro para asi poder responderle!!)

		//	TODO: Termino el thread. (Chequear si no hay que hacer algo con el socket, o el medio por el cual devuelvo la rta).
	}

	return parametro;
}


int main(void){
	//	TODO: Crear un socket para escuchar las peticiones que van a venir del browser.

	//	TODO: Conectarme a la DB openDS.

	while(true){//	While true porque tengo que escuchar todo el tiempo conexiones entrantes. En realidad hay que ver como cerrar el programa de una manera "linda".
		//	TODO: Acepto una conexion nueva.

		//	TODO: Creo un nuevo thread para atender esa conexion. (Seria la funcion: gestionarSolicitudDelClienteThread)
	}

	//	TODO: Me desconecto de la DB.

	//	TODO: Cierro el socket.

	//	TODO: Libero la lo ultimo que pueda llegar a quedar de memoria pedida.

	return 0;
}
