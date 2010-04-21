#include <iostream>
#include <vector>
#include <string>

using namespace std;


class PruebaDeManejoDelArray {
	public:
		int mostrarArray(void) {
			vector<string> VectorDeComandos;
			vector<int> VectorDeParametros;
			
			VectorDeComandos.push_back("LIST NEWSGROUPS");
			VectorDeComandos.push_back("QUIT");
			VectorDeComandos.push_back("LISTGROUP");
			VectorDeComandos.push_back("ARTICLE");
			VectorDeComandos.push_back("STAT");
			VectorDeComandos.push_back("HEAD");
			VectorDeComandos.push_back("BODY");
			
			VectorDeParametros.push_back(0);
			VectorDeParametros.push_back(0);
			VectorDeParametros.push_back(1);
			VectorDeParametros.push_back(1);
			VectorDeParametros.push_back(1);
			VectorDeParametros.push_back(1);
			VectorDeParametros.push_back(1);
			

		   cout << "Mostrando los comandos y sus flags:" << endl;

		   int ii;
		   for(ii=0; ii < VectorDeParametros.size(); ii++)
		   {
			  cout << VectorDeComandos[ii] << " -Lleva parametros? (1 = Sí, 0 = No) = " << VectorDeParametros[ii] << endl;

		   }
		   return 1;
	   }
};
	
