#include "INetworkPacketFactory.hpp"
#include "IController.hpp"
#include <sstream>

using namespace NETTIK;

size_t IPacketFactory::GenerateStream(std::string& stream, google::protobuf::Message* msg, INetworkCodes::msg_t code)
{
	// Read each byte of the data type into the packet buffer.
	for (uint32_t i = 0; i < sizeof(INetworkCodes::msg_t); i++)
		stream += (static_cast<unsigned char>(
			((unsigned char*)(&code))[i]
			));

	std::string tmp;
	msg->SerializeToString(&tmp);

	stream += tmp;

	return msg->ByteSize() + sizeof(INetworkCodes::msg_t);
}

INetworkCodes::msg_t IPacketFactory::GetCode(std::string& data)
{
	INetworkCodes::msg_t result;
	for (uint32_t i = 0; i < sizeof(INetworkCodes::msg_t); i++)
		(&result)[i] = data.at(i);


	return result;
}
void IPacketFactory::DispatchPacket(google::protobuf::Message* msg, INetworkCodes::msg_t code, ENetPeer* enetPeer, uint32_t flags, uint8_t channel)
{
	IController* controller = IController::GetPeerSingleton();

	// Drop the packet.
	if (controller == nullptr)
		return;

	std::string stream;
	size_t      stream_len;
	stream_len = GenerateStream(stream, msg, code);

	controller->Send((enet_uint8*)stream.c_str(), stream_len, enetPeer, flags, channel);
}
