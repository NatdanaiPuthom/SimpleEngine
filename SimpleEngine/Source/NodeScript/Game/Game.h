#pragma once
#include <vector>
#include "SimpleScript/Core/ScriptFoundation.h"

class Game
{
public:
	Game();
	~Game();

	void Init();
	void Update(float aTimeDelta);
	void Render();

	void LoadLevel(const char* name, bool runScripts);

	SCRIPT::ScriptManager* myGlobalScriptManager;
private:
	SCRIPT::ScriptFoundation myScriptFoundation;
};