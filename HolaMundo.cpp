

#include <iostream>
#include <vector>
#include <string>

using namespace std;

main()
{
	vector<string> VectorDeComandos;

	VectorDeComandos[0] = "List newsgroups";
	VectorDeComandos[1] = "Quit";
	VectorDeComandos[2] = "Listgroup";
	VectorDeComandos[3] = "Article";
	VectorDeComandos[4] = "Stat";
	VectorDeComandos[5] = "Head";
	VectorDeComandos[6] = "Body";
	
	/*VectorDeComandos.push_back("List newsgroups");
	VectorDeComandos.push_back("Quit");
	VectorDeComandos.push_back("Listgroup");
	VectorDeComandos.push_back("Article");
	VectorDeComandos.push_back("Stat");
	VectorDeComandos.push_back("Head");
	VectorDeComandos.push_back("Body");*/

   cout << "Mostrando los comandos:" << endl;

   int ii;
   for(ii=0; ii < VectorDeComandos.size(); ii++)
   {
      cout << VectorDeComandos[ii] << endl;

   }
}
	
