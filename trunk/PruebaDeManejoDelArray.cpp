

#include <iostream>
#include <vector>
#include <string>

using namespace std;

main()
{
	vector<string> VectorDeComandos;
	vector<int> VectorDeParametros;
	
	VectorDeComandos.push_back("List newsgroups");
	VectorDeComandos.push_back("Quit");
	VectorDeComandos.push_back("Listgroup");
	VectorDeComandos.push_back("Article");
	VectorDeComandos.push_back("Stat");
	VectorDeComandos.push_back("Head");
	VectorDeComandos.push_back("Body");
	
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
      cout << VectorDeComandos[ii] << "--" << VectorDeParametros[ii] << endl;

   }
}
	
