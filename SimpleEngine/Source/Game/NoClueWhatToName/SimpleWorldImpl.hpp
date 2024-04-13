#pragma once

namespace Simple
{
	class LevelManager;
	class EventManager;
	class ComponentManager;
	class ECS;
}

namespace Impl
{
	class SimpleWorldLevelManager final
	{
		friend class Simple::LevelManager;
	private:
		static void SetLevelManager(Simple::LevelManager* aLevelManager);
	};

	class SimpleWorldEventManager final
	{
		friend class Simple::EventManager;
	private:
		static void SetEventManager(Simple::EventManager* aLevelManager);
	};

	class SimpleWorldECS final
	{
		friend class Simple::ECS;
	private:
		static void SetECS(Simple::ECS* aECS);
	};
}

namespace Disgusting
{
	class DisgustingWorldComponentManager final
	{
		friend class Simple::ECS;
	private:
		static void SetComponentManager(Simple::ComponentManager* aComponentManager);
	};
}