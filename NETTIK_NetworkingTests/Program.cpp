#include "NETTIK_Networking.hpp"
using namespace NETTIK;

void main()
{
	
	IControllerClient_t client(new IControllerClient());

	IPacketFactory::IPacket<IPacketTest> packet(INetworkCodes::SharedPing);
	packet._PreventAutoDispatch();
	system("pause");

}