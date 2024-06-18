#pragma once
#include <unordered_map>
#include <string>

class MainSingleton;

namespace ECS
{
	class EntityComponentSystem;
}

namespace Simpleton
{
	class SceneManager
	{
		friend class MainSingleton;
	public:
	
		void Init();
		void Update();
		void Render();
		void LateRender();

	private:
		void LoadSettingsFromJson();
		void LoadDefaultScene();
	private:
		static SceneManager& GetInstance()
		{
			static SceneManager instance;
			return instance;
		}

		SceneManager();
		~SceneManager();

		void Destroy();
	private:
		std::unordered_map<std::string, ECS::EntityComponentSystem> myECSs;
		std::string myCurrentScene;
	};
}