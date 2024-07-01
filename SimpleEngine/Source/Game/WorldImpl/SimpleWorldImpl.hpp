#pragma once

namespace Simple
{
	class EventManager;
}

namespace Impl
{
	class SimpleWorldEventManager final
	{
		friend class Simple::EventManager;
	private:
		static void SetEventManager(Simple::EventManager* aLevelManager);
	};
}