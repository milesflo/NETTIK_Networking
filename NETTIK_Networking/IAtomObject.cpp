#include "IAtomObject.hpp"
#include "IController.hpp"
using namespace NETTIK;

static uint32_t s_CountIDs;

IAtomObject::IAtomObject() : m_ID(++s_CountIDs)
{

}

IAtomObject::~IAtomObject()
{
	NETTIK::IController::GetPeerSingleton()->RemoveObject(this);
}