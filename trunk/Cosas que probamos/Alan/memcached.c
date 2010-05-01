#include<stdio.h>
int main()
{
  //The example below shows how to set up a libmemcached client which communicates with the server via UDP
  memcached_st *memc;
  //creates handle for client instance
  memc= memcached_create(NULL);
  //turns on udp behavior for the memc client handle
  memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_USE_UDP, 1);
  //add the server 127.0.0.1:11211 to the client
  memcached_server_add_udp(memc, "127.0.0.1", 11211);
  //store some data
  char *key= "foo";
  char *value= "when we sanitize";
  memcached_return rc= memcached_set(memc, key, strlen(key),value, strlen(value),(time_t)0, (uint32_t)0);

/*This example shows how to set up two clients, one using UDP and one using TCP*/

  memcached_st *udp_client;
  memcached_st *tcp_client;
  //creates udp handle for client instance
  udp_client= memcached_create(NULL);
  //creates tcp handle for client instance
  tcp_client= memcached_create(NULL);
  //turns on udp behavior for the udp client handle
  memcached_behavior_set(udp_client, MEMCACHED_BEHAVIOR_USE_UDP, 1);
  //add the server 127.0.0.1:11211 to the clients
  memcached_server_add_udp(udp_client, "127.0.0.1", 11211);

  memcached_server_add(tcp_client, "127.0.0.1", 11211);

 //store some data

  char *key= "foo";
  char *value= "when we sanitize";
  memcached_return rc= memcached_set(udp_client, key, strlen(key),value, strlen(value),(time_t)0, (uint32_t)0);
  //get some data; note we use the tcp client
  size_t vlen;
  uint32_t flags;
  char * value= memcached_get(tcp_client, key, strlen(key),&vlen,&flags,&rc);

}
