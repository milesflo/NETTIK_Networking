#include "IController.hpp"
#include "IDebug.hpp"
using NETTIK::IController;

//! Global singleton for the ENET peer.
static IController* g_PeerSingleton = nullptr;


IController* IController::GetPeerSingleton()
{
	return g_PeerSingleton;
}

void IController::DeletePeerSingleton()
{

	if (g_PeerSingleton == nullptr)
		NETTIK_EXCEPTION("Tried deleting invalid peer.");

	g_PeerSingleton = nullptr;
}

void IController::SetPeerSingleton(IController* peer)
{
	// Deleting should call DeletePeerSingleton.
	if (g_PeerSingleton != nullptr)
		delete(g_PeerSingleton);

	g_PeerSingleton = peer;
}