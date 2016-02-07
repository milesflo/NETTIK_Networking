#include "NETTIK_Networking.hpp"
using namespace NETTIK;

void test()
{

	IPacketFactory::IPacket<IPacketTest> packet(INetworkCodes::SharedPing);
	packet.set_test_uint(51234);

	std::string buffer;

	packet._GenerateToString(buffer);

	for (int i = 0; i < buffer.size(); i++)
	{
		printf("%1x ", (uint8_t) buffer.at(i));
	}

	system("pause");

	INetworkCodes::msg_t code =  IPacketFactory::GetCode(buffer);
	printf("Code: %d\n", code);

	switch (code)
	{
	case INetworkCodes::SharedPing:
	{
		printf("Ping packet, decyphering...\n");

		IPacketFactory::IPacket<IPacketTest> pdata;
		pdata._Read(buffer);

		printf("Result = %d\n", pdata.test_uint());
		break;
	}

	default:
		printf("Unknown packet type.\n");
	}

	system("pause");

}

void main()
{

	IControllerClient_t client(new IControllerClient());
	for (int i = 0; i < 10; i++)
		test();
}