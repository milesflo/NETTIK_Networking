#include "NETTIK_Networking.hpp"
#include <iostream>
using namespace NETTIK;
using namespace std;

void main()
{

	
	IController* s_controller = nullptr;
	s_controller = new IControllerClient();
	cout << "s_controller: " << s_controller << endl;
	

	delete( s_controller );

	cout << "finished";
	cin.get();
}