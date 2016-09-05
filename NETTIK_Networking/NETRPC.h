#pragma once

//-----------------------------------------------
// Binds a proto callback to a function 
// in the invoker's object.
//-----------------------------------------------
#define CALL_RPC_BARE_PROTO(PacketType, ThisFunction)									\
	[this](const enet_uint8* data, size_t data_length, ENetPeer* enetPeer) {			\
		NETTIK::IPacketFactory::CProtoPacket<PacketType> packet(data, data_length);		\
																						\
		this->##ThisFunction##(packet);													\
	}

//-----------------------------------------------
// Extends the callback to functions outside of 
// the invoker's object. Passes ENET peer.
//-----------------------------------------------
#define CALL_RPC_FAR_PROTO(PacketType, ThisFunction)									\
	[this](const enet_uint8* data, size_t data_length, ENetPeer* enetPeer) {			\
		NETTIK::IPacketFactory::CProtoPacket<PacketType> packet(data, data_length);		\
																						\
		##ThisFunction##(enetPeer, packet);												\
	}
