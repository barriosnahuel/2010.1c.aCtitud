"resalta el código
    syntax on
"identación automática
    set cindent
"número de línea
    set nu
"fuente no brillosa
    set foldmethod=marker
"fondo negro
    set background=dark
"número de columnas por pantalla
    set columns=80
"margen derecho para ajuste de línea
    set wrapmargin=8
"fila y columna en la parte inferior
    set ruler
"no hace backups
    set nobackup
"ignora mayúsculas y minúsculas en las búsquedas
    set ic
"convertir tabulaciones a espacios
    set expandtab
    filetype plugin on
"abrir folds (+ de las funciones)
    map <F1> zo
"cerrar folds
    map <F2> zc
"abrir fold actual
    map <F3> [I
"caja de texto
    map <F4> i<CR>/************************************<CR><CR>**************************************/<CR><ESC>kki
"guardar todo, compilar y ver errores
    map <F5> :wall <CR> :make <CR> :copen <CR>
"navegar hacia adelante los errores
    map <F6> :cp<CR>
"navegar hacia atrás los errores
    map <F7> :cm<CR>
"aparece un arbol de directorios
    map <F10> :tabnew ./<CR>
"mover a la siguiente pestaña
    map <F12> zM
"barra de estado
    set laststatus=2
"espacios del tab y sangría (softtabstop shiftwidth)
    set ts=4
    set sw=4
"activa el mouse
    set mouse=a