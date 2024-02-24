#pragma once
#include "Game/PostMaster/Template/PostMaster.hpp"
#include "Game/PostMaster/EventTypes.hpp"

namespace Simple
{
	class EventManager
	{
	public:
		EventManager();
		~EventManager();

		Simple::PostMaster<Simple::eEvent>& GetPostMaster();
	private:
		Simple::PostMaster<Simple::eEvent> myPostMaster;
	};
}