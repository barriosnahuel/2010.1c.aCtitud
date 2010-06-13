#include "berkeleyFunctions.h"
#include<db.h>
#include<windows.h>
int main(){
	
/**	

	Leo archivo de configuracion para saber el newsgroup
    Creo la base Berkeley para dicho newsgroup

    Creo el thread GUIthread
        - El usuario crea la noticia
        - Se inserta en la base, si no existía una noticia con ese newsgroup e id!
        - Queda corriendo esto (??), es decir, inserta la noticia le dice "che inserté", y se queda a la espera de mas insersiones (??), porque si lo finalizo dps como hago para levantar el otro thread (??)

    Creo el XMLsender cada x cantidad de tiempo
        - Se fija si hay noticias nuevas en la base berkeley del newsgroup
        - Si hay noticias nuevas, las pasa a formato XML
        - Se hace el handshake , y luego se envía el XML al NTTPNewsServerProcess
        - Si no hubo problemas marco la noticia en Berkeley como transmitted (1)
        - Finaliza el thread
**/
	
	getchar();
	return 0;
}