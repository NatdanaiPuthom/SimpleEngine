#pragma once

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