#include "Game/Precomplied/GamePch.hpp"
#include "Graphics/Camera/Camera.hpp"
#include "Game/Managers/LevelManager/LevelManager.hpp"
#include "Game/Managers/LevelManager/Scenes/DefaultScene.hpp"
#include "Game/Test/Scenes/NavmeshSceneTest.hpp"
#include "Game/NoClueWhatToName/SimpleWorldImpl.hpp"

namespace Simple
{
	LevelManager::LevelManager()
		: myActiveScene(0)
	{
		LoadSettingsFromJson();

		Impl::SimpleWorldLevelManager::SetLevelManager(this);
	}

	LevelManager::~LevelManager()
	{
	}

	void LevelManager::Init()
	{
		std::shared_ptr<Scenes::DefaultScene> defaultScene = std::make_shared<Scenes::DefaultScene>();
		defaultScene->Init();
		myScenes.emplace(0, defaultScene);

		std::shared_ptr<Scenes::NavmeshSceneTest> navmeshTest = std::make_shared<Scenes::NavmeshSceneTest>();
		navmeshTest->Init();
		myScenes.emplace(1, navmeshTest);

		SetActiveScene(myActiveSceneIndex);
	}

	void LevelManager::Update()
	{
		myActiveScene->Update();
	}

	void LevelManager::Render()
	{
		myActiveScene->Render();
		Global::GetGraphicsEngine()->GetEditorCamera()->Update(Global::GetDeltaTime());
	}

	void LevelManager::SetActiveScene(const int aSceneIndex)
	{
		if (myActiveScene != nullptr)
		{
			myActiveScene->OnExit();
		}

		myActiveSceneIndex = aSceneIndex;
		myActiveScene = myScenes.at(myActiveSceneIndex);
		myActiveScene->OnEnter();
	}

	Simple::Navmesh& LevelManager::GetNavmesh()
	{
		return myNavmesh;
	}

	std::shared_ptr<Simple::Scene> LevelManager::GetActiveScene()
	{
		return myActiveScene;
	}

	int LevelManager::GetActiveSceneIndex() const
	{
		return myActiveSceneIndex;
	}

	void LevelManager::LoadSettingsFromJson()
	{
		const std::string filename = SimpleUtilities::GetAbsolutePath(SIMPLE_SETTINGS_LEVELS);

		std::ifstream file(filename);
		assert(file.is_open() && "Failed To Open File");

		const nlohmann::json json = nlohmann::json::parse(file);
		file.close();

		myActiveSceneIndex = json["activeScene"];
	}
}