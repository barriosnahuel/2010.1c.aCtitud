

#include <iostream>
#include <vector>
#include <string>

using namespace std;

main()
{
	/*vector<string> VectorDeComandos;
	
	VectorDeComandos.push_back("List newsgroups");
	VectorDeComandos.push_back("Quit");
	VectorDeComandos.push_back("Listgroup");
	VectorDeComandos.push_back("Article");
	VectorDeComandos.push_back("Stat");
	VectorDeComandos.push_back("Head");
	VectorDeComandos.push_back("Body");*/
	
	char* Comando;
	Comando = new char[sizeof(char)*50+1];
	
	vector<string> VectorDeComandos;
	VectorDeComandos = new vector[sizeof(Comando)*7];
	
	VectorDeComandos[2] = "Hola";

   cout << "Mostrando los comandos:" << endl;

   int ii;
   for(ii=0; ii < VectorDeComandos.size(); ii++)
   {
      cout << VectorDeComandos[ii] << endl;

   }
}
	
