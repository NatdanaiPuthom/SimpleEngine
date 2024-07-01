#pragma once
#include "Engine/ECS/ECS.hpp"
#include <unordered_map>
#include <string>

class MainSingleton;

namespace Simpleton
{
	using SceneID = size_t;

	struct SceneInfo
	{
		SceneID id = static_cast<size_t>(-1);
		std::string name;
		std::string absolutePath;
		std::string relativePath;
	};

	class SceneManager
	{
		using SceneRelativePath = std::string;

		friend class MainSingleton;

	public:

		void Init();
		void Update();
		void Render();
		void LateRender();

		void ChangeScene(const std::string& aSceneName);
		void ChangeSceneName(const std::string& aNewSceneName);
		void CreateNewScene(const std::string& aFilePath);

		const SceneInfo* GetCurrentSceneInfo() const;
		ECS::EntityComponentSystem& GetCurrentECS();

	private:
		void LoadSettingsFromJson();
		void LoadDefaultScene(const std::string& aDefaultScenePath);
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
		std::unordered_map<SceneRelativePath, SceneInfo> mySceneInfos;
		std::unordered_map<SceneID, ECS::EntityComponentSystem> myECSs;
		SceneInfo* myCurrentSceneInfo;
		size_t myNextSceneID;
		const int myPaddings[4] = { -4 };
	};
}