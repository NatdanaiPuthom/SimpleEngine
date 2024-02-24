#include "Game/Precomplied/GamePch.hpp"
#include "Game/Managers/EventManager/EventManager.hpp"
#include "Game/NoClueWhatToName/SimpleWorldImpl.hpp"


namespace Simple
{
	EventManager::EventManager()
	{
		Impl::SimpleWorldEventManager::SetEventManager(this);
	}

	EventManager::~EventManager()
	{
	}

	Simple::PostMaster<Simple::eEvent>& EventManager::GetPostMaster()
	{
		return myPostMaster;
	}
}