#pragma once
#include "../FlyDefines.hpp"
#include <string>

namespace FLY_NAMESPACE
{
	class Class;

	class ScriptLoader
	{
	public:

		
		static void SaveClass(const Class& aClass, std::string_view aFilePath);
		static void LoadClass(Class& aClass, std::string_view aFilePath);
		
		static void LoadAllClasses(std::string_view aFilePath);

		static void CreateCopyOfClass(const Class& aClass, std::string_view aFilePath, std::string_view aCopyName);

		static void SaveCustomEvents(std::string_view aFilePath);
		static void LoadCustomEvents(std::string_view aFilePath);

	};
}