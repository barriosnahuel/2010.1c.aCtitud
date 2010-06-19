#include"berkeleyFunctions.h"

void createDb(DB** dbp, HANDLE** memoryHandle)
{
	char* ruta = "C:\\";
	char* dbName = "aCtitud2.db";	//Aca debe levantar el nombre del newsgroup
	char* rutaDb = NULL;
	int ret;
	int tamanioRutaDb = strlen(ruta) + strlen(dbName)+1;
	
	rutaDb= (char*)HeapAlloc(*memoryHandle,HEAP_ZERO_MEMORY,tamanioRutaDb ); 
	printf("strlen(%s): %d \n",ruta,strlen(ruta));
	printf("strlen(%s): %d \n",dbName,strlen(dbName));
	strcat(rutaDb,ruta);	
	strcat(rutaDb,dbName);
	printf("rutaDB: %s \n", rutaDb);
	printf("strlen(%s): %d \n",rutaDb,strlen(rutaDb));
	
	if (!(ret = db_create(&*dbp, NULL, 0))) {
		fprintf(stderr, "db_create: %s\n", db_strerror(ret));
	}
		
	if (!(ret = (*dbp)->open(*dbp,NULL, rutaDb , NULL, DB_BTREE, DB_CREATE, 0))){
		printf("ENTRA A  DBP->OPEN \n");
		(*dbp)->err(*dbp, ret, "%s", rutaDb);
	}

	printf("QUIERE RESERVAR MEMORIA EN CREATEDB \N");	
		
	HeapFree(*memoryHandle,HEAP_ZERO_MEMORY, rutaDb );
	return;
}

int lastID(DB** dpb)
{
	int ret;
	DBC* dbCursor;
	DBT key,data;
	memset (&data, 0, sizeof(data));
	memset (&key, 0, sizeof(key));
	
	printf("\n ####### LASTID ####### \n");
	
	if ((ret = (*dpb)->cursor(*dpb,NULL,&dbCursor,0)) != 0) {
		(*dpb)->err(*dpb, ret, "DB->cursor");
		return (1);
	}
	while ((ret = dbCursor->c_get(dbCursor, &key, &data, DB_NEXT)) == 0)
		printf("KEY ALMACENADA : %s  \n",key.data);
		//printf("%lu : %.*s\n",*(u_long *)key.data, (int)data.size, (char *)data.data);

/*
	while (dbCursor->get(&key, &data, DB_NEXT) != DB_NOTFOUND)
		printf("Key encontrada : %d ",key);
*/
	return 0 ;
}

void putArticle(DB** dbp)
{
	int ret;          
	DBT key, data;

	printf("############## Insertar en base de datos ##############\n");
	
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	key.data = "34037030";
	key.size = strlen(key.data) + 1 ;
	data.data = "Alan";
	data.size = strlen(data.data) + 1;

	switch (ret = (*dbp)->put(*dbp, NULL, &key, &data, DB_NOOVERWRITE)) {
	case 0:
		printf("db: %s: Articulo guardado.\n", (char *)key.data);
		break;
	case DB_KEYEXIST:
		printf("db: %s: El articulo ya se encontraba en la base.\n",(char*)key.data);
		break;
	default:
		(*dbp)->err(dbp, ret, "DB->put");
	}
	return;
}

void getArticle(DB** dbp)
{
	int ret;          
	DBT key, data;

	printf("############## Lee en BerkeleyDB ##############\n");
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	key.data = "34037022";
	key.size = strlen(key.data) + 1 ;
	data.data = "Alan";
	data.size = strlen(data.data) + 1;
	
	if (!(ret = (*dbp)->get(*dbp, NULL, &key, &data, 0)))
		printf("db: %s: key encontrada: datos: %s.\n",(char *)key.data, (char *)data.data);
	else 
		(*dbp)->err(*dbp, ret, "DB->get");

	return;
}
void closeDb(DB** dbp)
{
	(*dbp)->close(*dbp,0);
	return;
}

/**
int main()
{
	/**
		apCreate(0,             // sin banderas: por defecto es sincronizado
                 15 * 1024,     // compromiso inicial: 15 KB
                 25 * 1024 );   // reserva inicial y tamaño máximo:
	**/
/**	HANDLE* memoryHandler = HeapCreate( 0, 1024, 0); //esto debería ir en cada hilo
	DB* dbHandler;
	printf("<------------------- Berkeley aCtitud -------------------> \n");
	
	
	createDb(&dbHandler, &memoryHandler);
	putArticle(&dbHandler);	
	getArticle(&dbHandler);
	closeDb(&dbHandler);
	getchar();
	return 0;
}

**/