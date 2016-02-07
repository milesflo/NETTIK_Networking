#include "INetworkPacketFactory.hpp"
#include "IController.hpp"

using namespace NETTIK;

void IPacketFactory::GenerateStream(std::string& out, google::protobuf::Message* msg, INetworkCodes::msg_t code)
{
	// Read each byte of the data type into the packet buffer.
	for (uint32_t i = 0; i < sizeof(INetworkCodes::msg_t); i++)
		out += (static_cast<unsigned char>(
			((unsigned char*)(&code))[i]
			));

	std::string str;
	msg->SerializeToString(&str);
	
	out += str;
}

INetworkCodes::msg_t IPacketFactory::GetCode(std::string& data)
{
	INetworkCodes::msg_t result;
	for (uint32_t i = 0; i < sizeof(INetworkCodes::msg_t); i++)
		(&result)[i] = data.at(i);


	return result;
}
void IPacketFactory::DispatchPacket(google::protobuf::Message* msg, INetworkCodes::msg_t code, void* enetPeer)
{
	IController* controller = IController::GetPeerSingleton();

	// Drop the packet.
	if (controller == nullptr)
		return;

	std::string m_sBuffer;
	GenerateStream(m_sBuffer, msg, code);

	controller->Send(m_sBuffer, enetPeer);
}
