#pragma once
#include "Engine/ECS/ECS.hpp"
#include <unordered_map>
#include <string>

class MainSingleton;

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

		void ChangeScene(const std::string& aSceneName);

		const std::string& GetCurrentScenePath() const;
		ECS::EntityComponentSystem& GetCurrentECS();

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

		SceneManager(const SceneManager&) = delete;
		SceneManager(const SceneManager&&) = delete;
		SceneManager operator=(const SceneManager&) = delete;
		SceneManager operator=(const SceneManager&&) = delete;

		bool AddScene(const std::string& aSceneName);
		void Destroy();
	private:
		std::unordered_map<std::string, ECS::EntityComponentSystem> myECSs;
		std::string myCurrentScene;
		const int myPadding[2] = { INT_MIN };
	};
}