#pragma once
#include <memory>
#include <vector>
#include <string>

namespace Simple
{
	class Scene
	{
	public:
		std::string mySceneName;

	public:
		Scene()
		{
			static size_t currentSceneID = 0;
			mySceneName = "Scene " + std::to_string(currentSceneID);
			currentSceneID++;
		}

		virtual ~Scene() = default;

		void SetSceneName(const std::string& aSceneName)
		{
			mySceneName = aSceneName;
		}

		const std::string& GetSceneName() const
		{
			return mySceneName;
		}

		virtual void OnEnter() {};
		virtual void OnExit() {};

		virtual void Init() {};
		virtual void Update() {};
		virtual void Render() {};
	};
}