#pragma once
#include "../ScriptDefines.h"
#include <string>

namespace SCR
{
	class Script;
	class ScriptManager;
	class ScriptFoundation;

	class ScriptLoader
	{
	public:

		
		static void SaveScript(const Script& aScript, std::string_view aFilePath);
		static void LoadScript(Script& aScript, std::string_view aFilePath);
		
		static void LoadAllScripts(std::string_view aFilePath);

		static void CreateCopy(const Script& aScript, std::string_view aFilePath, std::string_view aCopyName);

		static void SaveCustomEvents(std::string_view aFilePath);
		static void LoadCustomEvents(std::string_view aFilePath);

	};
}