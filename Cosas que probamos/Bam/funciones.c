#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "funciones.h"

/* recibe un comando y devuelve un número asociado */
int SeleccionarComando(char *czBuf) {
    char *czComandos[8];
    char czComandoTmp[10];
    int i;

    for (i=0;i<8;i++) {
        czComandos[i] = (char *)malloc(10);
        memset(czComandos[i], 0, 10);
    }
    
    strcpy(czComandos[0],"LIST");
    strcpy(czComandos[1],"QUIT");
    strcpy(czComandos[2],"LISTGROUP");
    strcpy(czComandos[3],"ARTICLE");
    strcpy(czComandos[4],"STAT");
    strcpy(czComandos[5],"HEAD");
    strcpy(czComandos[6],"BODY");
    strcpy(czComandos[7],"GROUP");
    
    i=0;
    while((czBuf[i] != ' ') && (czBuf[i] != '\0')) {
        czComandoTmp[i] = czBuf[i];
        i++;
    }
    czComandoTmp[i] = '\0';
    
    i=0;
    while(strcmp(czComandos[i],czComandoTmp) != 0) {
        /* este while es peligroso porque asumo que siempre
        viene un comando válido y voy a encontrar uno en el vector.
        si llegara una cadena inválida, podría tirar segfault */
        i++;
    }
    return i;
}

/* escribe un texto centrado en pantalla de 80 columnas */
void centrar(char *czBuf) {
	int i;
	i = (80-strlen(czBuf))/2;
	while(i-- > 0) printf(" ");
	printf("%s\n",czBuf);
}

/* hace un wait de N segundos */
void retardo(int iSegundos) {
	struct timeval tv;
	tv.tv_sec = iSegundos;
	tv.tv_usec = 0;
	select(1,NULL,NULL,NULL,&tv);
}

/* pantalla que se muestra al inicio */
void PantallaInicio() {
    printf("\e[2J\e[1;1H");
    int op;

    srand((unsigned)time(NULL));

    op = rand()%7;

    printf("\n\n");

    switch(op) {
	    case 0:
        centrar("                                             ____________ ");
        centrar("   NNTP                       --)-----------|____________|");
        centrar("  Server                                    ,'       ,'   ");
        centrar("   Bam        -)------========            ,'  ____ ,'     ");
        centrar("                       `.    `.         ,'  ,'__ ,'       ");
        centrar("                         `.    `.     ,'       ,'         ");
        centrar("                           `.    `._,'_______,'__         ");
        centrar("                             [._ _| ^--      || |         ");
        centrar("                     ____,...-----|__________ll_|\\        ");
        centrar("    ,.,..-------\"\"\"\"\"     \"----'                 ||       ");
        centrar(".-\"\"  |=========================== ______________ |       ");
        centrar(" \"-...l_______________________    |  |'      || |_]       ");
        centrar("                              [`-.|__________ll_|         ");
        centrar(" a                          ,'    ,' `.        `.         ");
        centrar("  C                       ,'    ,'     `.    ____`.       ");
        centrar("   t          -)---------========        `.  `.____`.     ");
        centrar("    i                                      `.        `.   ");
        centrar("     t                                       `.________`. ");
        centrar("      u                      --)-------------|___________|");
        centrar("       d                                                  ");
	    break;

	    case 1:
        centrar("                 ..::::..          ..,;;,,.                               ");
        centrar("               ::::::::::::.    .::t$$$$$$$$bu:,.                .zn$$$$k ");
        centrar("              ::::d$$$$bi::::.,::::$$$$$$$$$$$$i::.            ur:d$$$$$$F");
        centrar("             ::::$$$$$$$$$L::::::::3$$$$$'zec`$$$d$,           ?$,\"bd$$$$.");
        centrar("             `:::3$$$$$$$$$;::::::::?$$$$ `\"$$`$$$$?,           4='$b'ue$\"");
        centrar("              ::::$$$$$$$$$$:::::uee$$$$$L. `?:$$$$`?k           4$bedP\"  ");
        centrar("               ::::$$$$$\"\"\"?:::z$$$$$$$$$$$$bu$$P\"\"-.?,      zMMbu'\"\"     ");
        centrar("                `:::\"'.;::::::d$$$$$$$$$C73$$$$$$;    `b ,MUMMMMMMMM      ");
        centrar("                  ueeeeec,\"i.\"$$$$$$$$$$$ `?$$$$$$;,.,d$F;MMMMMMMMF       ");
        centrar("              ,eMMMMMMMMMMMc`!;\"$$$$$$$$$.   \"?$$$$$$$$F,MMMMMMMP\"        ");
        centrar("          ,eM6\"TMMMMMMMMMMMMb`!i;\"$$$$$$$$ %,   .ueeP',dMMMMMM\"           ");
        centrar("        zMMMMMMc'MMMMP,c,_??Mk`!!;.?$$$$F$$buued$$F,dM?MMMMM\"             ");
        centrar("       MMMMMMMMMP\"MM\"d$$(?$b'ML`!!!;`?$$$bCCCdP\"; MMM'dMMP\"               ");
        centrar("        TMMMMMF,d\"ze$$$$?be ;MML`!!!!;`?$$P\",;!!'dMM\"P\"                   ");
        centrar("         .\"?PP,$'$$$$$$$LF\";MMMMb`!!!!!i;,,;i!!'dMMF                      ");
        centrar("        <!!!!';;;`!!!>dF\";MMMMMMMMc`<!!!!!!>',dMMMF                       ");
        centrar("     .<!!!!',i!!!!!!> +dMMMMMMMMMMMMMeeeeedMMMMMM\"                        ");
        centrar("   :!!!!!!  `!!!!!'''` c,\"TMMMMMMMMMMMMMMMMMMPF\"                          ");
        centrar(" :!!!!!!'     `!'udNu`>.\"$br`TMMMMMMMMPP\"\",                              ");
        centrar(" !!!!!!         :MMMMb`!!;,'     ;\"\"ii!>;i!                NNTP           ");
        centrar("'!!!!'          4MMMMM,<!!!!;,..;   `';i!!                  Server        ");
        centrar("'!!'         :> JMMMMMM`!!!!!!>'     i!!'                    Bam          ");
        centrar(" !         :!! 4MMMMMMMh..zeee'    i!!'                                   ");
        centrar("          :!!!:`MMMMMMP:MMMM\"                                  aCtitud    ");
        centrar("          !!!!! `TMMP,dMMP\"                                               ");
	    break;

	    case 2:
        printf("\n");
        centrar("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
        centrar("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
        centrar("@@@@@@@@@@@@@@@@@@@@@@'~~~     ~~~`@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@");
        centrar("@@@@@@@@@@@@@@@@@'                     `@@@@@@@@@@@@@ aCtitud @@@@@@");
        centrar("@@@@@@@@@@@@@@'                           `@@@@@@@@@@@@@@@@@@@@@@@@@");
        centrar("@@@@@@@@@@@@'         NNTP Server Bam       `@@@@@@@@@@@@@@@@@@@@@@@");
        centrar("@@@@@@@@@@'                                   `@@@@@@@@@@@@@@@@@@@@@");
        centrar("@@@@@@@@@'                                     `@@@@@@@@@@@@@@@@@@@@");
        centrar("@@@@@@@@'                                       `@@@@@@@@@@@@@@@@@@@");
        centrar("@@@@@@@@                                         @@@@@@@@@@@@@@@@@@@");
        centrar("@@@@@@@'                      n,                 `@@@@@@@@@@@@@@@@@@");
        centrar("@@@@@@@                     _/ | _                @@@@@@@@@@@@@@@@@@");
        centrar("@@@@@@@                    /'  `'/                @@@@@@@@@@@@@@@@@@");
        centrar("@@@@@@@a                 <~    .'                a@@@@@@@@@@@@@@@@@@");
        centrar("@@@@@@@@                 .'    |                 @@@@@@@@@@@@@@@@@@@");
        centrar("@@@@@@@@a              _/      |                a@@@@@@@@@@@@@@@@@@@");
        centrar("@@@@@@@@@a           _/      `.`.              a@@@@@@@@@@@@@@@@@@@@");
        centrar("@@@@@@@@@@a     ____/ '   \\__ | |______       a@@@@@@@@@@@@@@@@@@@@@");
        centrar("@@@@@@@@@@@@a__/___/      /__\\ \\ \\     \\___.a@@@@@@@@@@@@@@@@@@@@@@@");
        centrar("@@@@@@@@@@@@/  (___.'\\_______)\\_|_|        \\@@@@@@@@@@@@@@@@@@@@@@@@");
        centrar("@@@@@@@@@@@|\\________                       ~~~~~\\@@@@@@@@@@@@@@@@@@");
        centrar("@@@@@@@@@@@||       |\\___________________________/|@/~~~~~~~~~\\@@@@@");
        centrar("@@@@@@@@@@/ |  |    | |                         | ||\\__________|@@@@");
        centrar("@@@@@@@@@/  |  |    | |                         | || \\          \\@@@");
	    break;

	    case 3:
        printf("\n");
        printf(".     .       .  .   . .   .   . .    +  .            \n");
        printf("  .     .  :     .    .. :. .___---------___.         \n");
        printf("       .  .   .    .  :.:. _\".^ .^ ^.  '.. :\"-_. .    \n");
        printf("    .  :       .  .  .:../:            . .^  :.:\\.    \n");
        printf("        .   . :: +. :.:/: .   .    .        . . .:\\   \n");
        printf(" .  :    .     . _ :::/:               .  ^ .  . .:\\  \n");
        printf("  .. . .   . - : :.:./.                        .  .:\\ \n");
        printf("  .      .     . :..|:                    .  .  ^. .:|\n");
        printf("    .       . : : ..||        .                . . !:|\n");
        printf("  .     . . . ::. ::\\(                           . :)/\n");
        printf(" .   .     : . : .:.|. ######              .#######::|\n");
        printf("  :.. .  :-  : .:  ::|.#######           ..########:| \n");
        printf(" .  .  .  ..  .  .. :\\ ########          :######## :/ \n");
        printf("  .        .+ :: : -.:\\ ########       . ########.:/  \n");
        printf("    .  .+   . . . . :.:\\. #######       #######..:/   \n");
        printf("      :: . . . . ::.:..:.\\           .   .   ..:/     \n");
        printf("   .   .   .  .. :  -::::.\\.       | |     . .:/      \n");
        printf("      .  :  .  .  .-:.\":.::.\\             ..:/        \n");
        printf(" .      -.   . . . .: .:::.:.\\.           .:/         \n");
        printf(".   .   .  :      : ....::_:..:\\   ___.  :/           \n");
        printf("   .   .  .   .:. .. .  .: :.:.:\\       :/            NNTP\n");
        printf("     +   .   .   : . ::. :.:. .:.|\\  .:/|             Server\n");
        printf("     .         +   .  .  ...:: ..|  --.:|             Bam\n");
        printf(".      . . .   .  .  . ... :..:..\"(  ..)\"             \n");
        printf(" .   .       .      :  .   .: ::/  .  .::\\            \n");
        printf("\n");
        printf("                       -={    a C t i t u d    }=-\n");
	    break;

	    case 4:
        printf("\n");
        centrar("        (((((((\\            NNTP Server Bam                     ");
        centrar("       ((((((((((\\                                              ");
        centrar("      ((_  _  )))))                                             ");
        centrar("      ))o) o ((((((|              aCtitud                       ");
        centrar("      ((<     ))))))                                            ");
        centrar("       )\\o    .-'\\((\\                                           ");
        centrar("       ((`--.( _    '-.                                         ");
        centrar("       )))))::\\  - _   '-.          _.-'''''-.                  ");
        centrar("        ((((::: ''-._-    '-.    .-'          `.                ");
        centrar("         ))):::.  ': '-._    :-:'              :                ");
        centrar("            ::::    : ::::.   :.               :                ");
        centrar("            ::::     ' ::::.   ::..          .:                 ");
        centrar("            :: o     : ::::::.  :::)        .:                  ");
        centrar("            : ::`-.-'`'-'-'-'-;  )/        .:         .-._      ");
        centrar("           :  .:              /  ;        .: _       /_   '-.__ ");
        centrar("           : .:              (''o\\_______.:'' '''-.-';; '-.    )");
        centrar("          : .:        @>-`-,--\\ |`\"      /.-''-._  _.;.-..-'``` ");
        centrar("         : .:                  :        /        '`._/_   '-.__ ");
        centrar("  __.---' .:                   :                     ; '-._    )");
        centrar(" '====d..:/                     :___,;,,,,,,,,,,,,,,,;;....-'''`");
	    break;

	    case 5:
        printf("\n");
        centrar("     .... NO! ...                  ... MNO! ...    ");
        centrar("   ..... MNO!! ...................... MNNOO! ...   ");
        centrar(" ..... MMNO! ......................... MNNOO!! .   ");
        centrar(".... MNOONNOO!   MMMMMMMMMMPPPOII!   MNNO!!!! .    ");
        centrar(" ... !O! NNO! MMMMMMMMMMMMMPPPOOOII!! NO! ....     ");
        centrar("    ...... ! MMMMMMMMMMMMMPPPPOOOOIII! ! ...       ");
        centrar("   ........ MMMMMMMMMMMMPPPPPOOOOOOII!! .....      ");
        centrar("   ........ MMMMMOOOOOOPPPPPPPPOOOOMII! ...        ");
        centrar("    ....... MMMMM..    OPPMMP    .,OMI! ....       ");
        centrar("     ...... MMMM::   o.,OPMP,.o   ::I!! ...        ");
        centrar("         .... NNM:::.,,OOPM!P,.::::!! ....         ");
        centrar("          .. MMNNNNNOOOOPMO!!IIPPO!!O! .....       ");
        centrar("         ... MMMMMNNNNOO:!!:!!IPPPPOO! ....        ");
        centrar("           .. MMMMMNNOOMMNNIIIPPPOO!! ......       ");
        centrar("          ...... MMMONNMMNNNIIIOO!..........       ");
        centrar("       ....... MN MOMMMNNNIIIIIO! OO ..........    ");
        centrar("    ......... MNO! IiiiiiiiiiiiI OOOO ...........  ");
        centrar("  ...... NNN.MNO! . O!!!!!!!!!O . OONO NO! ........");
        centrar("   .... MNNNNNO! ...OOOOOOOOOOO .  MMNNON!........ ");
        centrar("   ...... MNNNNO! .. PPPPPPPPP .. MMNON!........   ");
        centrar("      ...... OO! ................. ON! .......     ");
        centrar("         ................................          ");
        centrar("-=<  a C t i t u d  >=-");
        centrar("- NNTP Server Bam -");
	    break;

	    case 6:
        printf("\n\n\n");
        centrar("~~ NNTP Server Bam ~~");
        printf("\n");
        centrar("                 _-.                       .-_                 ");
        centrar("              _..-'(                       )`-.._              ");
        centrar("           ./'. '||\\\\.       (\\_/)       .//||` .`\\.           ");
        centrar("        ./'.|'.'||||\\\\|..    )*.*(    ..|//||||`.`|.`\\.        ");
        centrar("     ./'..|'.|| |||||\\```````  \"  '''''''/||||| ||.`|..`\\.     ");
        centrar("   ./'.||'.|||| ||||||||||||.     .|||||||||||| ||||.`||.`\\.   ");
        centrar("  /'|||'.|||||| ||||||||||||{     }|||||||||||| ||||||.`|||`\\  ");
        centrar(" '.|||'.||||||| ||||||||||||{     }|||||||||||| |||||||.`|||.` ");
        centrar("'.||| ||||||||| |/'   ``\\||/`     '\\||/''   `\\| ||||||||| |||.`");
        centrar("|/' \\./'     `\\./          |/\\   /\\|          \\./'     `\\./ `\\|");
        centrar("V    V         V          }' `\\ /' `{          V         V    V");
        centrar("`    `         `               U               '         '     ");
        printf("\n");
        centrar("-<[  aCtitud  ]>-");
	    break;

    }

    retardo(2);
}

/* pantalla que se muestra al finalizar */
void PantallaFin() {
    printf("\n\n\e[1;33m");
    centrar("aCtitud (c) 2010");
    printf("\e[0m\n\n");
}
