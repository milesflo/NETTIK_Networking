#pragma once
#include <string>
#include "INetworkCodes.hpp"
#include "IDebug.hpp"
#include <functional>
#include <google\protobuf\message.h>
#include <unordered_map>
//! CNETTIK_ENET_PEER is owned by this related .cpp file.

namespace NETTIK
{

	class IController
	{

	public:

		typedef std::function<void(google::protobuf::Message* message, void* enetPeer)> CallbackFunction_f;

		static IController* GetPeerSingleton();
		static void SetPeerSingleton(IController* peer);
		static void DeletePeerSingleton();

	public:

		//! Useful for clients as the first peer will be
		// the server.
		void* GetFirstPeer()
		{
			return 0; /* todo: implement*/
		}

		IController()
		{
			SetPeerSingleton(this);
		}

		virtual ~IController()
		{
			DeletePeerSingleton();
		}

		//! Sends data to the ENET peer.
		void Send(std::string& data, void* peer)
		{
			// todo: implement sending to ENET peer.
		}

		//! Sends data to the first ENET peer.
		void Send(std::string& data)
		{
			Send(data, GetFirstPeer());
		}
		
		//! Processes a single data stream and emits
		// the appropriate function.
		void ProcessRecv(std::string& data)
		{
			const char* stream = data.c_str();

			// If data is too small for the message ID type, then
			// don't process the data. This could cause a memory violation by
			// reading too far over the `stream` pointer.
			if (data.size() < sizeof(INetworkCodes::msg_t))
				NETTIK_EXCEPTION("Cannot parse data that has less than the code data type size (out of bounds prevention)");

			INetworkCodes::msg_t code;
			code = (INetworkCodes::msg_t)(*stream);

			printf("debug: (id = %d, content: %s)\n", code, stream);
			// todo: lookup code in the unordered_map and execute function with
			// parsed packet.
		};

	protected: 

		std::unordered_map <
			INetworkCodes::msg_t,

			std::pair <
				CallbackFunction_f,
				google::protobuf::Message*
			>

		> m_Callbacks;

		template <class T>
		void Listen(INetworkCodes::msg_t code, CallbackFunction_f callback)
		{
			m_Callbacks[code] = make_pair(code, T);
		}

	};

	class IControllerClient : public IController
	{

	public:

		IControllerClient() : IController()
		{

		}

		virtual ~IControllerClient()
		{

		}

		void Send(std::string& data, void* peer)
		{

		}

		void Recv(std::string& data, void* peer)
		{

		}

	};

	class IControllerServer : public IController
	{

	public:

		IControllerServer() : IController()
		{

		}

		virtual ~IControllerServer()
		{

		}


		void Send(std::string& data, void* peer)
		{

		}

		void Recv(std::string& data, void* peer)
		{

		}

	};
}