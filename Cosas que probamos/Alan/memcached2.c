#include<libmemcached/memcached.h>
#include<stdio.h>
int main()
{
        memcached_st *memc;
        memc = memcached_create(NULL);
        memcached_server_add(memc,"localhost",11211);
        printf("Se conecta al demon y sale \n");
        return 0;




    /*
  memcached_server_st *servers;
  memcached_st *memc= memcached_create(NULL);
  char servername[]= "0.example.com";

  servers= memcached_server_list_append(NULL, servername, 400, &rc);

  for (x= 0; x < 20; x++)
  {
    char buffer[SMALL_STRING_LEN];

    snprintf(buffer, SMALL_STRING_LEN, "%u.example.com", 400+x);
    servers= memcached_server_list_append(servers, buffer, 401, &rc);
  }
  //rc = memcached_server_push(memc, servers);
  memcached_server_free(servers);
  memcached_free(memc);
*/}