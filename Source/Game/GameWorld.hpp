#pragma once

class ImGuiManager;
class PlaneReflection;
class RaycastManager;

namespace Simple
{
	class LevelManager;
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
		void RenderUpSideDown();
	private:
		std::unique_ptr<ImGuiManager> myImGuiManager;
		std::unique_ptr<Simple::LevelManager> myLevelManager;
		std::unique_ptr<RaycastManager> myRaycastManager;

		std::unique_ptr<PlaneReflection> myPlaneReflection;
	};
}