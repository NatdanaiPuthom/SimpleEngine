#include "Engine/Precomplied/EnginePch.hpp"
#include "NodeScriptCustomTypes.hpp"
#include "NodeScript/FlyRegistration.hpp"


Fly::ViewAndEditResult ViewAndEditDataPtr(Fly::DataTypeID aDataTypeID, void* aDataPtr)
{
	return Fly::Internal::GetDataTypeManager().ViewAndEditData(aDataTypeID, aDataPtr);
}

void SaveDataPtr(Fly::DataTypeID aDataTypeID, const void* aDataPtr, nlohmann::json& aJson)
{
	Fly::Internal::GetDataTypeManager().SaveData(aDataTypeID, aDataPtr, aJson);
}

void LoadDataPtr(Fly::DataTypeID aDataTypeID, void* aDataPtr, const nlohmann::json& aJson)
{
	Fly::Internal::GetDataTypeManager().LoadData(aDataTypeID, aDataPtr, aJson);
}