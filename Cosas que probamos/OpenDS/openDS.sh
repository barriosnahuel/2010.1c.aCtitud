# Especifico una constante, que es el path absoluto del directorio donde esta el OopenDS
INSTANCE_ROOT="/export/home/osol/OpenDS-2.0.0"
export INSTANCE_ROOT

# Hago un case con el parametro 1, para ver que quiero hacer con el server.
case "${1}" in
	start)
		echo "Iniciando server OpenDS..."
		"${INSTANCE_ROOT}/bin/start-ds"
		exit ${?}
		;;
	  
	stop)
		echo "Deteniendo server..."
		"${INSTANCE_ROOT}/bin/stop-ds"
		exit ${?}
		;;
	  
	restart)
		echo "Reiniciando server..."
		"${INSTANCE_ROOT}/bin/stop-ds" --restart
		exit ${?}
		;;
	  
	*)
		echo "Los comandos disponibles son:  $0 { start | stop | restart }"
		exit 1
		;;
esac