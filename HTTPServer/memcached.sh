# Especifico una constante, que es el path absoluto del directorio donde esta el OopenDS

# Hago un case con el parametro 1, para ver que quiero hacer con el server.
case "${1}" in
	start)
		echo "Iniciando Memcached..."
		"/usr/lib/memcached -l 192.168.1.116 -p 11211 -d -u root"
		exit ${?}
		;;
		
	install)
		echo "Instalando Memcached..."
		echo "Nota: El install no fue probado."
		"wget http://www.monkey.org/%7Eprovos/libevent-1.4.13-stable.tar.gz"
		"gtar -xvzf libevent-1.4.13-stable.tar.gz"
	
		"wget http://memcached.googlecode.com/files/memcached-1.4.5.tar.gz" 
		"gtar -xvzf memcached-1.4.5.tar.gz"
	
		"su"
		"osol0906"

		"cd libevent-1.4.13-stable"
		"./configure" 
		"make"
		"make install"
		
		"cd memcached-1.4.5"
		"./configure"
		"make"
		"make install"
		exit ${?}
		;;		
	  
	*)
		echo "Los comandos disponibles son:  $0 { start ip port | install }"
		exit 1
		;;
esac