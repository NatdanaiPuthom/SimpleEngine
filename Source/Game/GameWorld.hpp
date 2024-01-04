#pragma once


namespace Simple
{
	class ImGuiManager;
	class LevelManager;
	class RaycastManager;
	class EventManager;
}

namespace Simple
{
	class GameWorld final
	{
	public:
		GameWorld();
		~GameWorld();

		void Init();
		void Update();
		void Render();

		void RenderImGui();
	private:
		std::unique_ptr<Simple::ImGuiManager> myImGuiManager;
		std::unique_ptr<Simple::LevelManager> myLevelManager;
		std::unique_ptr<Simple::RaycastManager> myRaycastManager;
		std::unique_ptr<Simple::EventManager> myEventManager;
	};
}