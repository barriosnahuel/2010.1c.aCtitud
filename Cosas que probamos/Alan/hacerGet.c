#include<libmemcached/memcached.h>
#include<stdio.h>
int main()
{
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
				
	
        return 0;
}
