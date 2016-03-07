#include "VirtualInstance.h"
#include "IController.hpp"
using namespace NETTIK;
//#include "IControllerClient.hpp"
//#include "IControllerServer.hpp"
void VirtualInstance::Update()
{
	std::vector<IPacketFactory::CBasePacket*> messageQueue;
	for (auto it = m_EntManagers.begin(); it != m_EntManagers.end(); it++)
		(*it).second->Update(messageQueue);
}

void VirtualInstance::DestroyEntityManager(std::string name)
{
	auto it = m_EntManagers.find(name);
	if (it != m_EntManagers.end())
		m_EntManagers.erase(it);
}

void VirtualInstance::DestroyEntityManager(IEntityManager* mgr)
{
	DestroyEntityManager(mgr->GetName());
}

VirtualInstance::~VirtualInstance()
{
	// Clear entity managers
	for (auto it = m_EntManagers.begin(); it != m_EntManagers.end();)
		it = m_EntManagers.erase(it);

};

VirtualInstance::VirtualInstance(std::string name, IController* controller)
{
	m_ParentController = controller;
	m_sName = name;
}