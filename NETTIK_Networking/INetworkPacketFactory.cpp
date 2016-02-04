#include "INetworkPacketFactory.hpp"
#include "IController.hpp"
using namespace NETTIK;

void IPacketFactory::DispatchPacket(google::protobuf::Message* msg, INetworkCodes::msg_t code, void* enetPeer)
{
	IController* controller = IController::GetPeerSingleton();

	// Drop the packet.
	if (controller == nullptr)
		return;

	std::string m_sBuffer;

	// Read each byte of the data type into the packet buffer.
	for (int i = 0; i < sizeof(INetworkCodes::msg_t); i++)
		m_sBuffer += (static_cast<unsigned char>(
			((unsigned char*)(&code))[i]
		));

	msg->SerializeToString(&m_sBuffer);
	controller->Send(m_sBuffer, enetPeer);
}
