#pragma once
#include <vector>
#include "SimpleScript/Core/ScriptFoundation.h"

struct SceneChanger3000
{
	bool myChangeScene;
	std::string mySceneName;
	Math::Vector2i myPlayerPosition = Math::Vector2i(7, 4);

	operator bool() const
	{
		return myChangeScene;
	}
};

struct Objective
{
	std::string instruction;
	int counter = 0;
	int maxCounter = 0;
	bool isDone = false;
};

class Game
{
public:
	Game();
	~Game();

	void Init();
	void Update(float aTimeDelta);
	void Render();

	void LoadLevel(const char* name, bool runScripts);

	void ChangeScene(const SceneChanger3000& aSceneChanger);
	const SceneChanger3000& GetSceneChanger() const;

	std::unordered_map<std::string, Objective> myObjectives;
	SCRIPT::ScriptManager* myGlobalScriptManager;

private:
	SceneChanger3000 mySceneChanger;
	SCRIPT::ScriptFoundation myScriptFoundation;
};