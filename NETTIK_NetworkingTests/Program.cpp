#include "NETTIK_Networking.hpp"
using namespace NETTIK;

void main()
{
	
	IControllerClient_t client(new IControllerClient());

	IPacketFactory::IPacket<IPacketTest> packet(INetworkCodes::SharedPing);
	system("pause");

}