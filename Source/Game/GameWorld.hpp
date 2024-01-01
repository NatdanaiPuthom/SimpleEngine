#pragma once


namespace Simple
{
	class ImGuiManager;
	class LevelManager;
	class RaycastManager;
	class PlaneReflection;
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
		void RenderReflection();
		void RenderEverythingUpSideDown();
	private:
		std::unique_ptr<Simple::ImGuiManager> myImGuiManager;
		std::unique_ptr<Simple::LevelManager> myLevelManager;
		std::unique_ptr<Simple::RaycastManager> myRaycastManager;

		std::unique_ptr<Simple::PlaneReflection> myPlaneReflection;
	};
}