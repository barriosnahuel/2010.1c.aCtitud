case "${1}" in
	start)
		echo "Iniciando Memcached..."
		echo "Este shell script no anda, ejecutar a mano: /usr/lib/memcached -l laIP -p elPuerto -d -u root"
		"/usr/lib/memcached -l ${2} -p ${3} -d -u root"
		exit ${?}
		;;
		
	install)
		echo "Instalando Memcached..."
		echo "Nota: El install no fue probado, asi que lo mas probable es que no ande."
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