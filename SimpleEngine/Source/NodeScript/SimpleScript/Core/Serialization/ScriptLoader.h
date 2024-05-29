#pragma once
#include "Core/ScriptDefines.h"
#include <string>

namespace SCR
{
	class Script;
	class ScriptManager;
	class ScriptFoundation;

	class ScriptLoader
	{
	public:

		static void Clear();
		
		static void Save(const Script& aScript);
		static void Load(Script& aScript);
		
		static void LoadAll(ScriptManager& aScriptManager);

		static void CreateCopy(const Script& aScript, const std::string& aCopyName);

		static void SaveCustomEvents(const std::string& aFilePath);
		static void LoadCustomEvents(ScriptFoundation& aFoundation);

		static std::string SavePath;
	};
}