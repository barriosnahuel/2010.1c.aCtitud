#include<libmemcached/memcached.h>
#include<stdio.h>
int main()
{

    memcached_st * memc;
    uint32_t flags;
    memcached_return rc;
    memcached_server_st * servers;
    memc = memcached_create (NULL);
    servers = memcached_servers_parse ("127.0.0.1:12300");
    memcached_server_push (memc, servers);
    memcached_server_list_free (servers);
    memcached_behavior_set (memc, MEMCACHED_BEHAVIOR_BINARY_PROTOCOL, 0);

    /*

        memcached_st *memc;
		memcached_return rc;
        memc = memcached_create(NULL);
        rc =memcached_server_add(memc,"localhost",11211); //LEVANTA EL SERVIDOR 1
        if (rc == MEMCACHED_SUCCESS)
			fprintf(stderr,"Levanta el servidor 1\n");
		else
			fprintf(stderr,"No levanto el 1: %s\n",memcached_strerror(memc, rc));

		rc =memcached_server_add(memc,"localhost",11212); // LEVANTA EL SERVIDOR 2

		if (rc == MEMCACHED_SUCCESS)
			fprintf(stderr,"Levanta el servidor 2\n");
		else
			fprintf(stderr,"No levanto el 2: %s\n",memcached_strerror(memc, rc));
*/

        return 0;
}
