#pragma once
#include "Game/Navmesh/Navmesh.hpp"

namespace Simple
{
	class Scene;
}

namespace Simple
{
	class LevelManager final
	{
	public:
		LevelManager();
		~LevelManager();

		void Init();
		void Update();
		void Render();
	public:
		void SetActiveScene(const int aSceneIndex);
	public:
		Simple::Navmesh& GetNavmesh();

		std::shared_ptr<Simple::Scene> GetActiveScene();
		int GetActiveSceneIndex() const;
	private:
		void LoadSettingsFromJson();
	private:
		std::shared_ptr<Simple::Scene> myActiveScene;
		std::unordered_map<int, std::shared_ptr<Simple::Scene>> myScenes;
		Simple::Navmesh myNavmesh;

		int myActiveSceneIndex;
	};
}