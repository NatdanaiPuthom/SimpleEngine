#pragma once
#include "../FlyDefines.hpp"
#include <string>
#include <nlohmann/json.hpp>

namespace FLY_NAMESPACE
{
	class Class;
	class Struct;
	class DataType;

	namespace Internal
	{
		void SaveDataType(const DataType& aDataType, std::string_view aFilePath);
		void LoadDataType(const nlohmann::json& aJsonData);
		void LoadDataType(const nlohmann::json& aJsonData, DataType& aDataType);
		//void SaveStruct(const Struct& aStruct, std::string_view aFilePath);
		//void LoadStruct(const nlohmann::json& aJsonData);
		//void LoadStruct(const nlohmann::json& aJsonData, Struct& aStruct);
		void SaveClass(const Class& aClass, std::string_view aFilePath);
		void LoadClass(const nlohmann::json& aJsonData);
		void LoadClass(const nlohmann::json& aJsonData, Class& aClass);

		void LoadAllFlyFiles(std::string_view aFilePath);

		void CreateCopyOfClass(const Class& aClass, std::string_view aFilePath, std::string_view aCopyName);

		void SaveCustomEvents(std::string_view aFilePath);
		void LoadCustomEvents(std::string_view aFilePath);
	}


}