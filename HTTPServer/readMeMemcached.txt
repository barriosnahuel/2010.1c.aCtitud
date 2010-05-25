su
osol0906

mkdir memcached
cd memcached

wget http://www.monkey.org/%7Eprovos/libevent-1.4.13-stable.tar.gz
	gtar -xvzf libevent-1.4.13-stable.tar.gz

wget http://memcached.googlecode.com/files/memcached-1.4.5.tar.gz 
	gtar -xvzf memcached-1.4.5.tar.gz


Dps hagan todo con el usuario root

-------------------------------------------------------------

cd libevent-1.4.13-stable
./configure 
make
make install

------------------------------------------------------------

cd memcached-1.4.5
./configure
make
make install


PARA LEVANTAR LA CACHE 
/usr/lib/memcached -l 192.168.0.102 -p 11211 -d -u root
/usr/lib/memcached -l 192.168.0.102 -p 11212 -d -u root


