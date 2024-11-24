#pragma once
#include "../FlyDefines.hpp"
#include <string>

namespace FLY_NAMESPACE
{
	class Class;

	namespace Internal
	{


		void SaveClass(const Class& aClass, std::string_view aFilePath);
		void LoadClass(Class& aClass, std::string_view aFilePath);

		void LoadAllClasses(std::string_view aFilePath);

		void CreateCopyOfClass(const Class& aClass, std::string_view aFilePath, std::string_view aCopyName);

		void SaveCustomEvents(std::string_view aFilePath);
		void LoadCustomEvents(std::string_view aFilePath);
	}


}