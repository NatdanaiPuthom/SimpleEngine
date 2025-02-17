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
		SceneManager(const SceneManager&) = delete;
		SceneManager(const SceneManager&&) = delete;
		SceneManager operator=(const SceneManager&) = delete;
		SceneManager operator=(const SceneManager&&) = delete;

		void Init();

		void EarlyUpdate();
		void FixedUpdate();
		void Update();
		void LateUpdate();

		void EarlyRender();
		void Render();
		void LateRender();

		void RenderBloomEntities();

		void ReloadSceneFromFile(const std::string& aSceneName);

		void ChangeScene(const std::string& aSceneName);
		void ChangeSceneName(const std::string& aNewSceneName);
		void CreateNewScene(const std::string& aFilePath);
	public:
		void SetIsPlaying(const bool aIsPlaying);
	public:
		const SceneInfo* GetCurrentSceneInfo() const;
		ECS::EntityComponentSystem& GetCurrentECS();
		bool GetIsPlaying() const;
	private:
		void LoadSettingsFromJson();
		void LoadSystems();
		void LoadDefaultScene(const std::string& aDefaultScenePath);
		bool LoadAndInitScene(const std::string& aSceneName);
	private:
		SceneManager();
		~SceneManager();

		bool AddScene(const std::string& aSceneName);
		void Destroy();
	private:
		std::unordered_map<SceneRelativePath, SceneInfo> mySceneInfos;
		std::unordered_map<SceneID, ECS::EntityComponentSystem> myECSs;
		SceneInfo* myCurrentSceneInfo;
		size_t myNextSceneID;
		bool myIsPlaying;
		const char myPaddings[8] = "Believe";
	};
}