********************************************************************************
************************ openLDAP Wrapper - version 0.1 ************************
********************************************************************************


* Al codigo no está finalizado por ende puede contener errores
  en el diseño y de programación ya que como es a modo de ejemplo
  no se tuvieron en cuenta cuestiones de memoria.


* Contenido del paquete:

     Ldap.c  -------------  código de ejemplo
     LdapWrapper.c -------  definición de las funciones de la API
     LdapWrapper.h -------  declaración de la API.
     ldap_wrapper_tutorial.pdf ---- Tutorial explicativo de como utilizar la API.
     lib  ---------------   openLDAP dynamic link libraries
     include  -----------   openLDAP headers

* Compilación

     $ tar -zxvf openLdap-wrapper-0.1.tar.gz

     $ cd openLdap-wrapper-0.1/

     $ gcc -o ldap-test Ldap.c LdapWrapper.c -Iinclude -Llib -lldap -llber

Las libraries del directorio lib deben estan en el PATH del sistema. Por ejemplo

     $ export LD_LIBRARY_PATH=openLdap-wrapper-0.1/lib:$LD_LIBRARY_PATH 